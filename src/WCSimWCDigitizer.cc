#include "WCSimWCDigitizer.hh"
#include "WCSimWCPMT.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4ios.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "WCSimDetectorConstruction.hh"
#include "WCSimPmtInfo.hh"
#include "WCSimDarkRateMessenger.hh"


#include <vector>
// for memset
#include <cstring>
#include <iostream>

// changed from 940 (april 2005) by MF
// 960 is unsuitable

const double WCSimWCDigitizer::calibdarknoise = 1.37676;

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


WCSimWCDigitizer::WCSimWCDigitizer(G4String name,
				   WCSimDetectorConstruction* myDetector)
  :G4VDigitizerModule(name)
{
  G4String colName = "WCDigitizedCollection";
  this->myDetector = myDetector;
  collectionName.push_back(colName);
  DigiHitMap.clear();

  DarkRateMessenger = new WCSimDarkRateMessenger(this);
}

WCSimWCDigitizer::~WCSimWCDigitizer(){
  DarkRateMessenger = 0;
}


void WCSimWCDigitizer::Digitize()
{
  DigitsCollection = new WCSimWCDigitsCollection ("/WCSim/glassFaceWCPMT",collectionName[0]);

  G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();
  
  // Get the PMT collection ID
   G4int WCHCID = DigiMan->GetDigiCollectionID("WCRawPMTSignalCollection");

  // Get the PMT Digits collection
  WCSimWCDigitsCollection* WCHCPMT = 
    (WCSimWCDigitsCollection*)(DigiMan->GetDigiCollection(WCHCID));
  
  if (WCHCPMT) {

    MakeHitsHistogram(WCHCPMT);
     //FindNumberOfGates(); //get list of t0 and number of triggers.
    FindNumberOfGatesFast(); //get list of t0 and number of triggers.
 
    if(this->PMTDarkRate>1E-307){
      AddPMTDarkRate(WCHCPMT);
    }
    
    for ( int i = 0 ; i < this->NumberOfGatesInThisEvent(); i++)
    {
	DigitizeGate(WCHCPMT,i);
    }
  }

  StoreDigiCollection(DigitsCollection);

}

void WCSimWCDigitizer::AddPMTDarkRate(WCSimWCDigitsCollection* WCHCPMT)
{
    // Introduces dark noise into each PMT during an event window
    // This won't introduce noise only events, and isn't written
    // to handle different rates for each PMT (although this shouldn't
    // be too difficult to add at a later time)
    // 
    // Added by: Morgan Askins (maskins@ucdavis.edu)

    const G4int number_entries = WCHCPMT->entries();
    const G4int number_pmts = myDetector->GetTotalNumPmts();
    int *PMTindex = new int [number_pmts+1];
    //   int PMTindex[number_pmts];
    //initialize PMTindex
    for (int l=0; l<number_pmts+1; l++){
      PMTindex[l] =0;
    }
    //Set up proper indices for tubes which have already been hit
    for (int g=0; g<number_entries; g++){
      G4int tube = (*WCHCPMT)[g]->GetTubeID();
      for (int gp=0; gp<(*WCHCPMT)[g]->GetTotalPe(); gp++){
	PMTindex[tube]++;
      }
    }

    // Get the info for pmt positions
    std::vector<WCSimPmtInfo*> *pmts = myDetector->Get_Pmts();
    // It works out that the pmts here are ordered !
    // pmts->at(i) has tubeid i+1

    // Don't add noise if there are zero events!
    if( number_entries == 0 )
        return;

    std::vector<int> list;
    list.assign( number_pmts, 0 );

    for( int h = 0; h < number_entries; h++ )
      {
        list[(*WCHCPMT)[h]->GetTubeID()-1] = h+1;
    }

    // Add noise to PMT's here, do so for time < LongTime
    double current_time = 0;
    double pe = 0.0;
    //    double poisson_mean = 1 / (this->PMTDarkRate * calibdarknoise * 1E-6 * number_pmts);
    double poisson_mean = 1 / (this->PMTDarkRate * this->ConvRate * 1E-6 * number_pmts);
    G4DigiManager* DMman = G4DigiManager::GetDMpointer();
    WCSimWCPMT* WCPMT = (WCSimWCPMT*)DMman->FindDigitizerModule("WCReadoutPMT");
    // Only add noise to triggered time windows!
   
    for( int i = 0; i < TriggerTimes.size(); i++ )
    {
        current_time = TriggerTimes[i];

	while( current_time < TriggerTimes[i] + (eventgateup - eventgatedown) )
        {
            // Get random time ahead for this poisson process
            current_time += -poisson_mean*log( 1-G4UniformRand() );
	 
            // Now add that hit to a random PMT ( Here we assume all PMT's are
            // equivelent. If that changes the following line would be replaced
            // by a random number with a weight factor.
	    int noise_pmt = static_cast<int>( G4UniformRand() * number_pmts );

	    if ( current_time >= TriggerTimes[i] + (eventgateup - eventgatedown ) )
	      break;

	    if( list[ noise_pmt ] == 0 )
	    {
	      //	      WCSimWCHit* ahit = new WCSimWCHit();
	      WCSimWCDigi* ahit = new WCSimWCDigi();

	      ahit->SetTubeID( noise_pmt );
	      // This Logical volume is GlassFaceWCPMT
	      ahit->SetLogicalVolume((*WCHCPMT)[0]->GetLogicalVolume());
	      ahit->SetTrackID(-1);
	      ahit->AddParentID(-1);
	      // Set the position and rotation of the pmt
	      Float_t hit_pos[3];
	      Float_t hit_rot[3];
	      //Int_t hit_cylLoc;
	      // TODO: need to change the format of hit_pos to G4ThreeVector
	      // and change hit_rot to G4RotationMatrix

	      WCSimPmtInfo* pmtinfo = (WCSimPmtInfo*)pmts->at( noise_pmt );
	      hit_pos[0] = 10*pmtinfo->Get_transx();
	      hit_pos[1] = 10*pmtinfo->Get_transy();
	      hit_pos[2] = 10*pmtinfo->Get_transz();
	      hit_rot[0] = pmtinfo->Get_orienx();
	      hit_rot[1] = pmtinfo->Get_orieny();
	      hit_rot[2] = pmtinfo->Get_orienz();
	      //hit_cylLoc = pmtinfo->Get_cylocation();
	      G4RotationMatrix pmt_rotation(hit_rot[0], hit_rot[1], hit_rot[2]);
	      G4ThreeVector pmt_position(hit_pos[0], hit_pos[1], hit_pos[2]);

	      ahit->SetRot(pmt_rotation);
	      ahit->SetPos(pmt_position);
	      ahit->SetTime(PMTindex[noise_pmt],current_time);
	      pe = WCPMT->rn1pe();
	      ahit->SetPe(PMTindex[noise_pmt],pe);
	      WCHCPMT->insert(ahit);
	      PMTindex[noise_pmt]++;
	      list[ noise_pmt ] = WCHCPMT->entries();
	    }
	    else{
	      (*WCHCPMT)[ list[noise_pmt]-1 ]->AddPe(current_time);
	      (*WCHCPMT)[ list[noise_pmt]-1 ]->SetTubeID(noise_pmt);
	      pe = WCPMT->rn1pe();
	      (*WCHCPMT)[ list[noise_pmt]-1 ]->SetPe(PMTindex[noise_pmt],pe);
	      (*WCHCPMT)[ list[noise_pmt]-1 ]->SetTime(PMTindex[noise_pmt],current_time);
	      PMTindex[noise_pmt]++;
	    }
        }
    }    
    delete [] PMTindex;
    return;
}


void WCSimWCDigitizer::MakeHitsHistogram(WCSimWCDigitsCollection* WCHCPMT)
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

    for (G4int i = 0 ; i < WCHCPMT->entries() ; i++)
      {

	(*WCHCPMT)[i]->SortHitTimes(); // VERY IMPORTANT!!!
	tc = (*WCHCPMT)[i]->GetTime(0);
	if ( tc < MinTime ) MinTime = tc; // find the earliest hit
	
	for ( G4int ip = 0 ; ip < (*WCHCPMT)[i]->GetTotalPe() ; ip++)
	  
	  {
	    tc = (*WCHCPMT)[i]->GetTime(ip);
	 
	    if (tc < WCSimWCDigitizer::LongTime)
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
	    //std::cerr << "found a trigger..." << RealOffset/5.0  <<"\n";
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
	    //std::cerr << "found a trigger..." << j
	    //<< " ; new start = " << I << "\n";
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



void WCSimWCDigitizer::DigitizeGate(WCSimWCDigitsCollection* WCHCPMT,G4int G)
{

  G4float timingConstant = 0.0;
 
  if (round(PMTSize) == 0.254*m)      // 20 inch tube
    //timingConstant = 3.0;      // sqrt(3.0ns) @1pe + 1ns = 2.7 ns
    // M Fechner : essai    //      timingConstant = 8.3885;

    timingConstant = 10.0;  //actual value is 13 nanoseconds

  else if (round(PMTSize) == 0.1016*m) // 8 inch tube
    //      timingConstant = .58;      // sqrt(.58ns) @1pe + 1ns = 1.76 ns
    timingConstant = 1.890; // same scaling, M Fechner
  else if (round(PMTSize) == 0.127*m) //10 inch tube
    timingConstant = 2.0; // (JF) Need real value
  else if (round(PMTSize) == round(0.1524*m)) //12 inch tube
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

  G4float tc;
  G4double lowerbound;
  if (G==0) lowerbound = TriggerTimes[G]+EvtG8Down;
  else {
    lowerbound = ( TriggerTimes[G]+EvtG8Down >= TriggerTimes[G-1] + EvtG8Up ) ?  TriggerTimes[G]+EvtG8Down : TriggerTimes[G-1] + EvtG8Up;
  }
  G4double upperbound = TriggerTimes[G]+EvtG8Up;
  G4double efficiency = 0.985; // with skrn1pe (AP tuning) & 30% QE increase in stacking action
 
  for (G4int i=0; i < WCHCPMT->entries(); i++)
    {
      //G4double peCutOff = .3;
      // MF, based on S.Mine's suggestion : global scaling factor applied to
      // all the smeared charges.
      // means that we need to increase the collected light by
      // (efficiency-1)*100% to
      // match K2K 1KT data  : maybe due to PMT curvature ?

      //  G4double efficiency = 0.985; // with skrn1pe (AP tuning) & 30% QE increase in stacking action

      // Get the information from the hit
      G4int   tube         = (*WCHCPMT)[i]->GetTubeID();
    
      // august 2004 : sorting is necessary before doing this !!!
      //      G4float firstHitTime = (*WCHCPMT)[i]->GetTime(0);
      G4float firstHitTime = (*WCHCPMT)[i]->GetFirstHitTimeInGate(lowerbound,
							       upperbound);
      G4double peSmeared=0.0;
      double bound1 = firstHitTime+WCSimWCDigitizer::pmtgate;
      G4float mintime = (upperbound < bound1) ? upperbound : bound1;
      /*
      if (firstHitTime < 0.) { //PMT not hit in this gate
	//	G4cout << "skipped this hit\n";
	continue; // move on to the next Hit PMT
      }
      */
     
      for ( G4int ip = 0 ; ip < (*WCHCPMT)[i]->GetTotalPe() ; ip++){
	tc = (*WCHCPMT)[i]->GetTime(ip);
	//Add up pe for each time in the gate.
	if (tc >= lowerbound && tc <= mintime){
	  G4double   pe = (*WCHCPMT)[i]->GetPe(ip);
	  peSmeared += pe;
	}
      }
      
      // Now digitize this hit

     
      //float qtmp = 0.;
 
      //      peSmeared *= G4RandGauss::shoot(1.0,0.05); // extra smearing
      int iflag;

      //check if hits in PMT are above threshold
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
		WCSimWCDigi* Digi = new WCSimWCDigi();

		Digi->SetTubeID(tube);
		Digi->AddGate(G,TriggerTimes[G]);
		Digi->SetPe(G,peSmeared);
		Digi->SetTime(G,digihittime);
		DigiHitMap[tube] = DigitsCollection->insert(Digi);
	      }
	      else {
		//G4cout << "deja vu " << tube << " " << G << "  " << TriggerTimes[G] << " " << digihittime
		//     << "  " <<   peSmeared <<" ";
		(*DigitsCollection)[DigiHitMap[tube]-1]->AddGate(G,TriggerTimes[G]);
		(*DigitsCollection)[DigiHitMap[tube]-1]->SetPe(G,peSmeared);
		(*DigitsCollection)[DigiHitMap[tube]-1]->SetTime(G,digihittime);
	      }
	    }
	  else { }//G4cout << "discarded negative time hit\n";}
	}
      //    else { G4cout << iflag << G4endl;}
    } // Loop over hits
}
