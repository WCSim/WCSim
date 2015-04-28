#include "WCSimWCDigitizerSKIV.hh"
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

//RawSignalHitCollection *collection = new RawSignalHitCollection;

const double WCSimWCDigitizerSKIV::calibdarknoise = 1.37676;

const double WCSimWCDigitizerSKIV::offset = 950.0 ; // ns
const double WCSimWCDigitizerSKIV::pmtgate = 200.0 ; // ns
const double WCSimWCDigitizerSKIV::eventgateup = 950.0 ; // ns
const double WCSimWCDigitizerSKIV::eventgatedown = -400.0 ; // ns
const double WCSimWCDigitizerSKIV::LongTime = 100000.0 ; // ns
// value in skdetsim
const int WCSimWCDigitizerSKIV::GlobalThreshold = 22 ; // # hit PMTs
//const int WCSimWCDigitizerSKIV::GlobalThreshold = 12 ; // # hit PMTs
// try to trigger early to reduce the width.
//const int WCSimWCDigitizerSKIV::GlobalThreshold = 10 ; // # hit PMTs


WCSimWCDigitizerSKIV::WCSimWCDigitizerSKIV(G4String name,
				   WCSimDetectorConstruction* myDetector)
  :WCSimWCDigitizerBase(name, myDetector)
{
//   G4String colName = "WCDigitizedStoreCollection";
//   this->myDetector = myDetector;
//   collectionName.push_back(colName);
  collectionName.push_back("WCDigitizedCollection");
//   DigiStoreHitMap.clear();
  //  DarkRateMessenger = new WCSimDarkRateMessenger(this);
}

WCSimWCDigitizerSKIV::~WCSimWCDigitizerSKIV(){
  //DarkRateMessenger = 0;
}

// void WCSimWCDigitizerSKIV::Digitize()
// {
//   //Temporary Storage of Digitized hits which is passed to the trigger
//   DigiStore = new WCSimWCDigitsCollection(collectionName[0],collectionName[0]);
//   DigiStoreHitMap.clear();

//   //DigitsCollection = new WCSimWCDigitsCollection ("/WCSim/glassFaceWCPMT",collectionName[0]);

//   G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();
  
//   // Get the PMT collection ID
//    G4int WCHCID = DigiMan->GetDigiCollectionID("WCRawPMTSignalCollection");

//   // Get the PMT Digits collection
//   WCSimWCDigitsCollection* WCHCPMT = 
//     (WCSimWCDigitsCollection*)(DigiMan->GetDigiCollection(WCHCID));
  
//   if (WCHCPMT) {
//     DigitizeHits(WCHCPMT);
//   }
  
//   StoreDigiCollection(DigiStore);

// }

//void WCSimWCDigitizerSKIV::DigitizeHits(WCSimWCDigitsCollection* WCHCPMT) {
////In the base class this does nothing
//
//
//}

void WCSimWCDigitizerSKIV::Digitize()
{
  
  DigitsCollection = new WCSimWCDigitsCollection ("/WCSim/glassFaceWCPMT","WCDigitizedCollection");
  //collectionName.push_back("WCDigitizedCollection");
  DigiHitMap.clear();
  //Temporary Storage of Digitized hits which is passed to the trigger
  DigiStore = new WCSimWCDigitsCollection();
  DigiStoreHitMap.clear();

  //DigitsCollection = new WCSimWCDigitsCollection ("/WCSim/glassFaceWCPMT",collectionName[0]);

  G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();
  
  // Get the PMT collection ID
   G4int WCHCID = DigiMan->GetDigiCollectionID("WCRawPMTSignalCollection");
  // Get the PMT Digits collection
  WCSimWCDigitsCollection* WCHCPMT = 
    (WCSimWCDigitsCollection*)(DigiMan->GetDigiCollection(WCHCID));
  
  if (WCHCPMT) {
    DigitizeHits(WCHCPMT);
  }
  
  StoreDigiCollection(DigitsCollection);

}


void WCSimWCDigitizerSKIV::DigitizeHits(WCSimWCDigitsCollection* WCHCPMT) {
  std::cout<<"START (WCHCPMT->entries() = "<<WCHCPMT->entries()<<std::endl;
  //We must first sort hits by PMT in time.  This is very important as the code
  //assumes that each hit is in time order from lowest to highest.
  
  //Sorting done.  Now we integrate the charge on each PMT.  SK IV has a 400ns 
  //integration gate, discharges for 350ns to calculate the charge and
  //this is followed by a 150ns veto
  //Total processing time for a hit is 900ns
  for (G4int i = 0 ; i < WCHCPMT->entries() ; i++)
    {
      //loop over entires in WCHCPMT, each entry corresponds to
      //the photons on one PMT

      //Sort photons on this pmt
      (*WCHCPMT)[i]->SortArrayByHitTime();
      int tube = (*WCHCPMT)[i]->GetTubeID();
      //std::cout<<"tube "<<tube<<" totalpe = "<<(*WCHCPMT)[i]->GetTotalPe()<<std::endl;

      //look over all photons on the PMT
      //integrate charge and start digitizing
      float intgr_start=0;
      float upperlimit=0;
      G4double efficiency = 0.985; // with skrn1pe (AP tuning) & 30% QE increase in stacking action
      int ngate=0;

      // Variables to store photon uniqueid that make up a digit
      int digi_unique_id   = 0;
      int photon_unique_id = 0;
      std::vector< std::pair<int,int> > digi_comp; 

      for( G4int ip = 0 ; ip < (*WCHCPMT)[i]->GetTotalPe() ; ip++)
	{
	  float time = (*WCHCPMT)[i]->GetTime(ip);
          float pe = (*WCHCPMT)[i]->GetPe(ip);

	  //start the integration time as the time of the first hit
	  //Hits must be sorted in time
	  if(ip==0) {
	    intgr_start=time;
	    peSmeared = 0;
	    //Set the limits of the integration window [intgr_start,upperlimit]                                                                                            
	    upperlimit = intgr_start + pmtgate;
	  }
	  
	  bool MakeDigit = false;
	  if(time >= intgr_start && time <= upperlimit) {
	    peSmeared += pe;
	    digi_comp.push_back( std::make_pair(digi_unique_id, photon_unique_id) );
       
	    //if this is the last digit, make sure to make the digit
	    if(ip + 1 == (*WCHCPMT)[i]->GetTotalPe()){
	      MakeDigit = true;
	    }
	    
	  }
	  else {
	    //this hit is outside the integration time window.
	    //Charge integration is over.  There is now a 350ns + 150ns
	    //time period where no hits can be recorded
	    //Check if previous hit passed the threshold.  If so we will digitize the hit
	    MakeDigit = true;
	    // 	    int iflag;
	    // 	    WCSimWCDigitizerSKIV::Threshold(peSmeared,iflag);
	    // 	    if(iflag == 0) {
	    // 	      //digitize hit
	    // 	      peSmeared *= efficiency;
	    // }
	    // 	    else {
	    // 	      //reject hit
	    // 	    }
	    //Now we need to reject hits that are after the integration
	    //period to the end of the veto signal
	  }
	  
	  //Make digit here
	  if(MakeDigit) {
	    int iflag;                                                                                                                                                        
	    WCSimWCDigitizerSKIV::Threshold(peSmeared,iflag);                                                                                                                      
	    if(iflag == 0) {                                                                                                                                             
	      //digitize hit                                                                                                                                  
	      peSmeared *= efficiency;             
	      WCSimWCDigitizerSKIV::AddNewDigit(tube, ngate, intgr_start, peSmeared);
	      ngate++;
	      
	      digi_unique_id++;
	      // Save digit composition info
	      (*WCHCPMT)[i]->AddDigiCompositionInfo(digi_comp);
	      
	    }                                                                                                                                                           
	    else {                                                                                                                                                            
	      //reject hit                                                                                                                                             
	    }
	  }
	  
	  //Now try and deal with the next hit
	  if(time > upperlimit && time <= upperlimit + 350 + 150) {
	    continue;
	  }
	  else if(time > upperlimit + 350 + 150){
	    std::cout<<"*** SECOND DIGIT IN EVENT ***"<<std::endl;
	    //we now need to start integrating from the hit
	    intgr_start=time;
	    peSmeared = pe;
	    //Set the limits of the integration window [intgr_start,upperlimit]                                                                                 
	    upperlimit = intgr_start + pmtgate;
	    //if this is the last hit we must handle the creation of the digit 
	    //as the loop will not evaluate again
	    if(ip+1 == (*WCHCPMT)[i]->GetTotalPe()) {
	      int iflag;
	      WCSimWCDigitizerSKIV::Threshold(peSmeared,iflag);
	      if(iflag == 0) {
		//digitize hit                                                                                                                                                  
		peSmeared *= efficiency;
		WCSimWCDigitizerSKIV::AddNewDigit(tube, ngate, intgr_start, peSmeared);
		ngate++;	
	      }
	      else {
		//reject hit                                                                                                                           
	      }
	    }
	  }
	  photon_unique_id++;
	}
    }   
  std::cout<<"END\n";
  
  std::cout<<"\n\n\nCHECK DIGI COMP:"<<std::endl;
  for (G4int i = 0 ; i < WCHCPMT->entries() ; i++){
    std::vector< std::pair<int,int> > comp = (*WCHCPMT)[i]->GetDigiCompositionInfo();
    for(int i = 0; i < (int) comp.size(); i++){
      std::cout<<"entry "<<i<<" digi "<<comp[i].first<< " p_id "<<comp[i].second<<std::endl;
    }
  }
  
  temporaryTrig();
}


// void WCSimWCDigitizerSKIV::AddNewDigit(int tube, int gate, float digihittime, float peSmeared) {
//   //gate is not a trigger, but just the position of the digit in the array
//   //inside the WCSimWCDigi object
//   std::cout<<"Adding hit "<<gate<<"\n";
//   if ( digihittime > 0.0 && peSmeared>0.0)
    
//     {
//       if ( DigiStoreHitMap[tube] == 0) {
// 	WCSimWCDigi* Digi = new WCSimWCDigi();
// 	Digi->AddParentID(1);
	
// 	Digi->SetTubeID(tube);
// 	//	Digi->AddGate(gate,digihittime);
// 	Digi->SetPe(gate,peSmeared);
// 	Digi->AddPe(digihittime);
// 	Digi->SetTime(gate,digihittime);
// 	DigiStoreHitMap[tube] = DigiStore->insert(Digi);
//       }
//       else {
// 	//G4cout << "deja vu " << tube << " " << G << "  " << TriggerTimes[G] << " " << digihittime
// 	//     << "  " <<   peSmeared <<" ";
// 	//(*DigitsCollection)[GigiStoreHitMap[tube]-1]->AddParentID(parentID);
// 	//(*DigiStore)[DigiStoreHitMap[tube]-1]->AddGate(gate,digihittime);
// 	(*DigiStore)[DigiStoreHitMap[tube]-1]->SetPe(gate,peSmeared);
// 	(*DigiStore)[DigiStoreHitMap[tube]-1]->SetTime(gate,digihittime);
// 	(*DigiStore)[DigiStoreHitMap[tube]-1]->AddPe(digihittime);
//       }
//     }
//   else { }//G4cout << "discarded negative time hit\n";}
// }



void WCSimWCDigitizerSKIV::temporaryTrig() {
  

  //Now we will try to find triggers
  int ii=0;
  float lower=0;
  float upper = WCSimWCDigitizerSKIV::pmtgate;
  int ntrig=0;
  std::cout<<"NUMBER OF ENTRIES "<<DigiStore->entries()<<"\n";
  while(ii<=WCSimWCDigitizerSKIV::LongTime-WCSimWCDigitizerSKIV::pmtgate) {
    //loop over PMTs and hits in each PMT.  If nhits > Threshhold then we have a trigger                                                                                         
    int n_pmt=0;
    //a time that is not possible for a hit
    float firstinwindow=WCSimWCDigitizerSKIV::LongTime+1;
    float triggertime;
    bool triggerfound=false;
    for (G4int i = 0 ; i < DigiStore->entries() ; i++) {
      //see if there are hits over the threshold
      int tube=(*DigiStore)[i]->GetTubeID();

      for ( G4int ip = 0 ; ip < (*DigiStore)[i]->GetTotalPe() ; ip++){
	int tc = (*DigiStore)[i]->GetTime(ip);
	if(tc>=ii && tc<=ii+WCSimWCDigitizerSKIV::pmtgate) {
	  //hit in trigger window
	  n_pmt++;
	  if(firstinwindow > tc)
	    firstinwindow=tc;
	  
	}
      }
      //if over threshold issue trigger
      if(n_pmt > WCSimWCDigitizerSKIV::GlobalThreshold) {
	ntrig++;
	triggertime=firstinwindow;
	TriggerTimes.push_back(triggertime);
	triggerfound = true;
	break;
	//	float lowerbound = triggertime+WCSimWCDigitizerSKIV::eventgatedown;
	//float upperbound = triggertime+WCSimWCDigitizerSKIV::eventgateup;
      }
    }
    //Check if trigger was issued
    if(triggerfound) {
      //trigger was issued with trigger time 'triggertime'
      //Loop over hits create digits for each trigger
      //Time window for hits:
      float lowerbound = triggertime+WCSimWCDigitizerSKIV::eventgatedown;
      float upperbound = triggertime+WCSimWCDigitizerSKIV::eventgateup;
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
      
      for (G4int i = 0 ; i < DigiStore->entries() ; i++) {
	//see if there are hits over the threshold
	int tube=(*DigiStore)[i]->GetTubeID();
	
	for ( G4int ip = 0 ; ip < (*DigiStore)[i]->GetTotalPe() ; ip++){
	  int tc = (*DigiStore)[i]->GetTime(ip);
	  float peSmeared = (*DigiStore)[i]->GetPe(ip);
	  //int parentID=(*DigiStore)[i]->GetParentID(ip);
	  if(tc>= lowerbound && tc <= upperbound) {
	    //hit in event window	  
	    //add to DigitsCollection
	    
	    float Q = (peSmeared > 0.5) ? peSmeared : 0.5;
	    float timingResolution = 0.33 + sqrt(timingConstant/Q);
	    // looking at SK's jitter function for 20" tubes
	    if (timingResolution < 0.58) timingResolution=0.58;
	    
	    G4double digihittime = -triggertime
	      + WCSimWCDigitizerSKIV::offset
	      + tc
	      + G4RandGauss::shoot(0.0,timingResolution);
	    //add hit
	    if ( DigiHitMap[tube] == 0) {
	      WCSimWCDigi* Digi = new WCSimWCDigi();
	      Digi->AddParentID(1);
	      
	      Digi->SetTubeID(tube);
	      Digi->AddGate(ntrig-1,triggertime);
	      Digi->SetPe(ntrig-1,peSmeared);
	      Digi->AddPe(digihittime);
	      Digi->SetTime(ntrig-1,digihittime);
	      DigiHitMap[tube] = DigitsCollection->insert(Digi);
	    }
	    else {
	      //G4cout << "deja vu " << tube << " " << G << "  " << TriggerTimes[G] << " " << digihittime
	      //     << "  " <<   peSmeared <<" ";
	      //(*DigitsCollection)[DigiHitMap[tube]-1]->AddParentID(parentID);
	      (*DigitsCollection)[DigiHitMap[tube]-1]->AddGate(ntrig-1,triggertime);
	      (*DigitsCollection)[DigiHitMap[tube]-1]->SetPe(ntrig-1,peSmeared);
	      (*DigitsCollection)[DigiHitMap[tube]-1]->SetTime(ntrig-1,digihittime);
	      (*DigitsCollection)[DigiHitMap[tube]-1]->AddPe(digihittime);
	    }
	  }
	}
      }
      ii=triggertime+upperbound;
    }
    else {
      ii=ii+10;
    }
  }

  //  StoreDigiCollection(DigitsCollection);
}
