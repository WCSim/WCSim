#include "WCSimWCDigitizer.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"

#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4ios.hh"

#include "WCSimDetectorConstruction.hh"

#include <vector>
// for memset
#include <cstring>
#include <iostream>

// changed from 940 (april 2005) by MF
// 960 is unsuitable

const double WCSimWCDigitizer::offset = 950.0 ; // ns
const double WCSimWCDigitizer::pmtgate = 200.0 ; // ns
const double WCSimWCDigitizer::eventgateup = 950.0 ; // ns
const double WCSimWCDigitizer::eventgatedown = -400.0 ; // ns
const double WCSimWCDigitizer::LongTime = 100000.0 ; // ns
// value in skdetsim
const int WCSimWCDigitizer::GlobalThreshold = 25 ; // # hit PMTs
//const int WCSimWCDigitizer::GlobalThreshold = 12 ; // # hit PMTs
// try to trigger early to reduce the width. 
//const int WCSimWCDigitizer::GlobalThreshold = 10 ; // # hit PMTs

// special version -- based on a suggestion by K. Okumura
// M F, april 05
// use skdetsim's 1pe distribution

const double WCSimWCDigitizer::PMTDarkRate = 4.0; // kHz
const G4float qpe0[501] = {
    // 1
      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
      0.000000, 0.000129, 0.000754, 0.004060, 0.028471,
    // 2
      0.068449, 0.115679, 0.164646, 0.203466, 0.235631,
      0.262351, 0.282064, 0.303341, 0.320618, 0.338317,
      0.357825, 0.371980, 0.385820, 0.398838, 0.413595,
      0.428590, 0.444387, 0.461685, 0.482383, 0.502369,
      0.520779, 0.540011, 0.559293, 0.579354, 0.599337,
      0.619580, 0.639859, 0.659807, 0.679810, 0.699620,
      0.718792, 0.737382, 0.755309, 0.772042, 0.788232,
      0.803316, 0.817861, 0.831148, 0.844339, 0.855532,
      0.866693, 0.876604, 0.886067, 0.894473, 0.902150,
      0.909515, 0.915983, 0.922050, 0.927418, 0.932492,
    // 3 
      0.936951, 0.940941, 0.944660, 0.948004, 0.951090,
      0.953833, 0.956576, 0.958886, 0.961134, 0.963116,
      0.964930, 0.966562, 0.968008, 0.969424, 0.970687,
      0.971783, 0.972867, 0.973903, 0.974906, 0.975784,
      0.976632, 0.977438, 0.978190, 0.978891, 0.979543,
      0.980124, 0.980666, 0.981255, 0.981770, 0.982227,
      0.982701, 0.983146, 0.983566, 0.983975, 0.984357,
      0.984713, 0.985094, 0.985404, 0.985739, 0.986049,
      0.986339, 0.986630, 0.986922, 0.987176, 0.987431,
      0.987655, 0.987922, 0.988173, 0.988414, 0.988639,
    // 4  
      0.988856, 0.989065, 0.989273, 0.989475, 0.989662,
      0.989828, 0.990007, 0.990172, 0.990327, 0.990497,
      0.990645, 0.990797, 0.990981, 0.991135, 0.991272,
      0.991413, 0.991550, 0.991673, 0.991805, 0.991928,
      0.992063, 0.992173, 0.992296, 0.992406, 0.992514,
      0.992632, 0.992733, 0.992837, 0.992954, 0.993046,
      0.993148, 0.993246, 0.993354, 0.993458, 0.993549,
      0.993656, 0.993744, 0.993836, 0.993936, 0.994033,
      0.994134, 0.994222, 0.994307, 0.994413, 0.994495,
      0.994572, 0.994659, 0.994739, 0.994816, 0.994886,
    // 5  
      0.994970, 0.995032, 0.995110, 0.995178, 0.995250,
      0.995321, 0.995383, 0.995464, 0.995532, 0.995609,
      0.995674, 0.995750, 0.995821, 0.995889, 0.995952,
      0.996010, 0.996071, 0.996153, 0.996218, 0.996283,
      0.996335, 0.996384, 0.996431, 0.996484, 0.996537,
      0.996597, 0.996655, 0.996701, 0.996745, 0.996802,
      0.996860, 0.996917, 0.996962, 0.997014, 0.997079,
      0.997114, 0.997165, 0.997204, 0.997250, 0.997295,
      0.997335, 0.997379, 0.997418, 0.997454, 0.997488,
      0.997530, 0.997573, 0.997606, 0.997648, 0.997685,
    // 6  
      0.997725, 0.997762, 0.997795, 0.997835, 0.997866,
      0.997898, 0.997941, 0.997966, 0.997997, 0.998039,
      0.998065, 0.998104, 0.998128, 0.998153, 0.998179,
      0.998205, 0.998223, 0.998254, 0.998293, 0.998319,
      0.998346, 0.998374, 0.998397, 0.998414, 0.998432,
      0.998456, 0.998482, 0.998511, 0.998532, 0.998553,
      0.998571, 0.998594, 0.998614, 0.998638, 0.998669,
      0.998693, 0.998715, 0.998743, 0.998762, 0.998793,
      0.998812, 0.998834, 0.998857, 0.998872, 0.998888,
      0.998904, 0.998926, 0.998946, 0.998963, 0.998983,
    // 7  
      0.999007, 0.999027, 0.999044, 0.999064, 0.999079,
      0.999096, 0.999120, 0.999133, 0.999152, 0.999160,
      0.999174, 0.999188, 0.999206, 0.999221, 0.999234,
      0.999248, 0.999263, 0.999276, 0.999286, 0.999300,
      0.999313, 0.999321, 0.999331, 0.999347, 0.999356,
      0.999369, 0.999381, 0.999394, 0.999402, 0.999415,
      0.999427, 0.999433, 0.999446, 0.999458, 0.999472,
      0.999484, 0.999499, 0.999513, 0.999522, 0.999532,
      0.999540, 0.999550, 0.999559, 0.999567, 0.999574,
      0.999588, 0.999599, 0.999613, 0.999618, 0.999627,
    // 8  
      0.999635, 0.999639, 0.999652, 0.999662, 0.999667,
      0.999671, 0.999678, 0.999682, 0.999688, 0.999693,
      0.999698, 0.999701, 0.999706, 0.999711, 0.999718,
      0.999722, 0.999727, 0.999732, 0.999737, 0.999740,
      0.999746, 0.999750, 0.999754, 0.999763, 0.999766,
      0.999769, 0.999774, 0.999780, 0.999784, 0.999788,
      0.999796, 0.999803, 0.999807, 0.999809, 0.999815,
      0.999820, 0.999827, 0.999830, 0.999833, 0.999833,
      0.999836, 0.999839, 0.999842, 0.999845, 0.999850,
      0.999853, 0.999857, 0.999860, 0.999865, 0.999870,
    // 9  
      0.999873, 0.999877, 0.999880, 0.999882, 0.999883,
      0.999886, 0.999888, 0.999889, 0.999895, 0.999896,
      0.999897, 0.999901, 0.999902, 0.999905, 0.999907,
      0.999907, 0.999909, 0.999911, 0.999911, 0.999912,
      0.999913, 0.999914, 0.999917, 0.999919, 0.999921,
      0.999923, 0.999927, 0.999929, 0.999931, 0.999933,
      0.999936, 0.999942, 0.999942, 0.999944, 0.999947,
      0.999947, 0.999948, 0.999949, 0.999952, 0.999955,
      0.999957, 0.999957, 0.999961, 0.999962, 0.999963,
      0.999963, 0.999963, 0.999964, 0.999965, 0.999965,
    // 10
      0.999965, 0.999965, 0.999966, 0.999968, 0.999969,
      0.999971, 0.999972, 0.999972, 0.999973, 0.999975,
      0.999975, 0.999975, 0.999975, 0.999975, 0.999975,
      0.999975, 0.999979, 0.999979, 0.999980, 0.999982,
      0.999983, 0.999985, 0.999986, 0.999987, 0.999987,
      0.999988, 0.999989, 0.999989, 0.999989, 0.999989,
      0.999990, 0.999990, 0.999992, 0.999993, 0.999994,
      0.999994, 0.999994, 0.999994, 0.999994, 0.999995,
      0.999995, 0.999995, 0.999996, 0.999996, 0.999996,
      0.999996, 0.999998, 0.999999, 1.000000, 1.000000,
    // Dummy element for noticing if the loop reached the end of the array
      0.0
};



extern "C" void skrn1pe_(float* );
//extern "C" void rn1pe_(float* ); // 1Kton

WCSimWCDigitizer::WCSimWCDigitizer(G4String name)
  :G4VDigitizerModule(name)
{
  G4String colName = "WCDigitizedCollection";
  collectionName.push_back(colName);  
  DigiHitMap.clear();
}

WCSimWCDigitizer::~WCSimWCDigitizer() {;}


G4double WCSimWCDigitizer::rn1pe(){
  G4double random = G4UniformRand();
  G4int i;
  for(i = 0; i < 501; i++){
    if (random <= qpe0[i]) break;
  }
  if(i==500)
    random = G4UniformRand();
#ifndef TUNED_1PE
  return (G4double(i-50) + random)/22.83;
#else
  return (G4double(i-50) + random)*0.95/22.83;
#endif
}


void WCSimWCDigitizer::Digitize()
{
  DigitsCollection = new WCSimWCDigitsCollection
    ("/WCSim/glassFaceWCPMT",collectionName[0]); 
   
  G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();
  
  // Get the Associated Hit collection IDs
  G4int WCHCID = DigiMan->GetHitsCollectionID("glassFaceWCPMT");

  // The Hits collection
  WCSimWCHitsCollection* WCHC = 
    (WCSimWCHitsCollection*)(DigiMan->GetHitsCollection(WCHCID));

  if (WCHC) {
   
    MakeHitsHistogram(WCHC); 
    //FindNumberOfGates(); //get list of t0 and number of triggers.
    FindNumberOfGatesFast(); //get list of t0 and number of triggers.

    for ( int i = 0 ; i < this->NumberOfGatesInThisEvent(); i++)
      {
	DigitizeGate(WCHC,i);	
      }
  }

  StoreDigiCollection(DigitsCollection);

}

void WCSimWCDigitizer::MakeHitsHistogram(WCSimWCHitsCollection* WCHC)
{
  // Maximilien Fechner, april 2005
  // add an individual integration gate for each PMT
  // + a global gate for the event
  // define t0 as in SKdetsim : t0 = 960 - (time after which 25 hits have been observed), see 'dshigh.F'
  // if ever a laser sudy is done it seems (see K2K1KT meetings of Jan 5th
  // 2005, laser_simulation.ppt) that this is not good, and the offset should be constant.
  // updates aug 2006 by MF to deal with subevents.

    std::memset(triggerhisto,0,20000*sizeof(G4int));
    GateMap.clear();
    MinTime = WCSimWCDigitizer::LongTime;

    G4float tc;
    for (G4int i = 0 ; i < WCHC->entries() ; i++)
      {
	(*WCHC)[i]->SortHitTimes(); // VERY IMPORTANT!!!		
	tc = (*WCHC)[i]->GetTime(0);
	if ( tc < MinTime ) MinTime = tc; // find the earliest hit
	for ( G4int ip = 0 ; ip < (*WCHC)[i]->GetTotalPe() ; ip++)
	  {
	    tc = (*WCHC)[i]->GetTime(ip);
	    if ( tc < WCSimWCDigitizer::LongTime)
	    { 
	      //triggerhisto[int(floor(tc/5.0))]++;
	      GateMap[ int(floor(tc/5.0)) ]++;

	    }  
	  }	
      }    
    // the map must end with a value below trigger threshold
    // otherwise we will be stuck in infinite loops -- Maximilien Fechner, feb 22, 2007
    GateMap[20001]=0;
}


void WCSimWCDigitizer::FindNumberOfGatesFast()
{
//GateMap is indexed by time histogram bin, and
//references elements of the timing histogram

    G4int acc = 0; // accumulated # hits within time window...
    std::map< G4int, G4int>::iterator _mGateKeeper, _mNextGate;

    for( _mGateKeeper = GateMap.begin() ; _mGateKeeper != GateMap.end() ; _mGateKeeper++)
    {   
	acc = 0;
	_mNextGate = _mGateKeeper;
        RealOffset = 0.0; 				// will need to add the offset later
									// 40 means + 200ns
									// so check 39 bins ahead in the histogram..
	while ( _mNextGate != GateMap.lower_bound( _mGateKeeper->first + 39) 
	     && _mNextGate->first <= _mGateKeeper->first + 39 		// but not more than 200ns away though!
	      )
	{

	  acc += _mNextGate->second;
          if ( acc > WCSimWCDigitizer::GlobalThreshold )
	  {
	    //RealOffset = _mGateKeeper->first*5.0;
	    RealOffset = _mNextGate->first*5.0;
	    TriggerTimes.push_back(RealOffset);
	    std::cerr << "found a trigger..." << RealOffset/5.0  <<"\n";
	    _mGateKeeper = GateMap.lower_bound( _mNextGate->first + G4int(WCSimWCDigitizer::eventgateup )/5. ); 
	    std::cerr.flush();
	    break;
          }		  
	  _mNextGate++;							// look at the next time bin with hits
	}
    }

}

void WCSimWCDigitizer::FindNumberOfGates()
{
  G4int acc = 0; // accumulated # hits within time window...
  G4int compte = 0;
  
  G4int I = 1;
  G4int j;

  G4int SearchWindow = G4int ( WCSimWCDigitizer::LongTime ) / 5 ;

//  for ( I = 0 ; I < 10000 ; I ++ )
//  	  G4cout << " triggerhisto: " << triggerhisto[I] << " t: " << I*5.0 << " " << G4endl;
//  I = 1;


  while ( I < SearchWindow )
    {
      RealOffset = 0.0; // will need to add the offset later
      for ( j = I ; j <= SearchWindow ; j++)
	{                          // 40 corresponds to 200ns
	  G4int beginning = ( (j+1-40>I) ? (j+1-40) : I );
	  acc = 0;
	  for ( G4int k = beginning ; k <= j; k++)
	    { 
	      acc += triggerhisto[k-1];
	    }
	  if ( acc > WCSimWCDigitizer::GlobalThreshold ) {
	    RealOffset = float(j)*5.0;
	    TriggerTimes.push_back(RealOffset);
	    I = j+G4int(WCSimWCDigitizer::eventgateup )/5. ;
	    std::cerr << "found a trigger..." << j 
		      << " ; new start = " << I << "\n";
	    std::cerr.flush();
	    break;	    
	  }	  
	}  

      compte = 0;
      for (int index = I ; index < SearchWindow ; index++)
	compte += triggerhisto[index];            
      if ( compte < WCSimWCDigitizer::GlobalThreshold ) break;
      if (j==SearchWindow) break; //means we've looped all over and found nothing
      I++;
    }
  
  
}


void WCSimWCDigitizer::DigitizeGate(WCSimWCHitsCollection* WCHC,G4int G)
{

  G4float timingConstant = 0.0;
  
  if (round(PMTSize) == 0.254*m)      // 20 inch tube
    //      timingConstant = 3.0;      // sqrt(3.0ns) @1pe + 1ns = 2.7 ns  
    // M Fechner : essai 
    //      timingConstant = 8.3885;
    timingConstant = 10.0;  //actual value is 13 nanoseconds
  else if (round(PMTSize) == 0.1016*m) // 8 inch tube
    //      timingConstant = .58;      // sqrt(.58ns) @1pe + 1ns = 1.76 ns  
    timingConstant = 1.890; // same scaling, M Fechner
  else if (round(PMTSize) == 0.127*m) //10 inch tube
    timingConstant = 2.0; // (JF) Need real value
  else if (round(PMTSize) == 0.1524*m) //12 inch tube
    timingConstant = 2.0;// (JF) Need the real value
  else
    {
      G4cout << "Sorry.  Resolution for " << PMTSize/cm 
	     << "cm PMTs is unknown.  Exiting program. " 
	     << G4endl;
      exit(-1);
    }

  G4double EvtG8Down = WCSimWCDigitizer::eventgatedown;
  G4double EvtG8Up = WCSimWCDigitizer::eventgateup;  // this is a negative number...
  
  
  G4double lowerbound;
  if (G==0) lowerbound = TriggerTimes[G]+EvtG8Down;
  else {
    lowerbound = ( TriggerTimes[G]+EvtG8Down >= TriggerTimes[G-1] + EvtG8Up ) ?  TriggerTimes[G]+EvtG8Down : TriggerTimes[G-1] + EvtG8Up;
  }
  G4double upperbound = TriggerTimes[G]+EvtG8Up;
  
  for (G4int i=0; i < WCHC->entries(); i++)
    {
      //G4double peCutOff = .3;
      // MF, based on S.Mine's suggestion : global scaling factor applied to
      // all the smeared charges.
      // means that we need to increase the collected light by 
      // (efficiency-1)*100% to 
      // match K2K 1KT data  : maybe due to PMT curvature ?
      G4double efficiency = 0.985; // with skrn1pe (AP tuning) & 30% QE increase in stacking action
      
      // Get the information from the hit
      G4int   tube         = (*WCHC)[i]->GetTubeID();
      // august 2004 : sorting is necessary before doing this !!!     
      //      G4float firstHitTime = (*WCHC)[i]->GetTime(0);
      G4float firstHitTime = (*WCHC)[i]->GetFirstHitTimeInGate(lowerbound,
							       upperbound);
      
      if (firstHitTime < 0.) { //PMT not hit in this gate
	//	G4cout << "skipped this hit\n";
	continue; // move on to the next Hit PMT
      }
      

      double bound1 = firstHitTime+WCSimWCDigitizer::pmtgate;      
      G4float totalPe = (*WCHC)[i]->GetPeInGate(lowerbound,upperbound,bound1);
      // Now digitize this hit
      
      G4double peSmeared;
      //float qtmp = 0.;
      peSmeared = 0.0;
      for (int npe=0; npe < totalPe ; npe++)
	{
	  //qtmp=0.0;
	  // MF : note : if you use skrn1pe, you'll need to increase the
	  // charge /number of generated photons by ~4%
	  //	  skrn1pe_(&qtmp);
	  //rn1pe_(&qtmp);
	  //peSmeared += qtmp;
          peSmeared += rn1pe(); 
	}
      
      //      peSmeared *= G4RandGauss::shoot(1.0,0.05); // extra smearing
      int iflag;
      WCSimWCDigitizer::Threshold(peSmeared,iflag);
      peSmeared *= efficiency; // MC tuning correction
      
      
      if ( iflag==0 )
	//      if ( peSmeared > peCutOff )
	{
	  // warning the times are still absolute G4 times.
	  // need to take care of offsets here !!
	  
	  // MF : reorganized the algorithm. Because of the offset, very early
	  // hits can appear to be negative (if the t0 is triggered much later)
	  // they have to be removed otherwise nothing will work.
	  
	  // MF : found 'by hand', fits SK PMT resolution (2002 values ?)
	  // Add cutoffs 
	  float Q = (peSmeared > 0.5) ? peSmeared : 0.5;
	  float timingResolution = 0.33 + sqrt(timingConstant/Q);
	  // looking at SK's jitter function for 20" tubes
	  if (timingResolution < 0.58) timingResolution=0.58;
	  
	  G4double digihittime = -TriggerTimes[G]
	    + WCSimWCDigitizer::offset
	    + firstHitTime
	    + G4RandGauss::shoot(0.0,timingResolution);
	  
	  if ( digihittime > 0.0 && peSmeared>0.0) 
	    {	  
	      if ( DigiHitMap[tube] == 0) {
		//G4cout << tube << " " << G << "  " << TriggerTimes[G] << " " << digihittime 
		//       << "  " <<   peSmeared <<" ";
		WCSimWCDigi* Digi = new WCSimWCDigi();
		Digi->SetTubeID(tube);
		Digi->AddGate(G,TriggerTimes[G]);
		Digi->SetPe(G,peSmeared);
		Digi->SetTime(G,digihittime);
		DigiHitMap[tube] = DigitsCollection->insert(Digi);
		//G4cout  << DigiHitMap[tube] << "\n";
	      }
	      else { 
		//G4cout << "deja vu " << tube << " " << G << "  " << TriggerTimes[G] << " " << digihittime 
		//     << "  " <<   peSmeared <<" ";
		(*DigitsCollection)[DigiHitMap[tube]-1]->AddGate(G,TriggerTimes[G]);
		(*DigitsCollection)[DigiHitMap[tube]-1]->SetPe(G,peSmeared);
		(*DigitsCollection)[DigiHitMap[tube]-1]->SetTime(G,digihittime);
	      }
	    }
	  else { G4cout << "discarded negative time hit\n";}	
	  
	} // Loop over hits
      
    }
  
}
 

