#include "WCSimWCDigitizerNew.hh"
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



// *******************************************
// BASE CLASS
// *******************************************

#ifndef WCSIMWCDIGITIZER_VERBOSE
//#define WCSIMWCDIGITIZER_VERBOSE
#endif

// changed from 940 (april 2005) by MF
// 960 is unsuitable

//RawSignalHitCollection *collection = new RawSignalHitCollection;

WCSimWCDigitizerBase::WCSimWCDigitizerBase(G4String name,
					   WCSimDetectorConstruction* inDetector,
					   WCSimWCDAQMessenger* myMessenger)
  :G4VDigitizerModule(name), myDetector(inDetector)
{
  G4String colName = "WCDigitizedStoreCollection";
  collectionName.push_back(colName);
  ReInitialize();

  //  DarkRateMessenger = new WCSimDarkRateMessenger(this);
  if(myMessenger != NULL) {
    DAQMessenger = myMessenger;
    DAQMessenger->TellMeAboutTheDigitizer(this);
    DAQMessenger->SetDigitizerOptions();
  }
  else {
    G4cerr << "WCSimWCDAQMessenger pointer is NULL when passed to WCSimWCDigitizerBase constructor. Exiting..." 
	   << G4endl;
    exit(-1);
  }
}

WCSimWCDigitizerBase::~WCSimWCDigitizerBase(){
  //DarkRateMessenger = 0;
}

void WCSimWCDigitizerBase::Digitize()
{
  //Clear the DigiStoreHitMap
  ReInitialize();

  //Temporary Storage of Digitized hits which is passed to the trigger
  DigiStore = new WCSimWCDigitsCollection(collectionName[0],collectionName[0]);

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
  
  StoreDigiCollection(DigiStore);

}

void WCSimWCDigitizerBase::AddNewDigit(int tube, int gate, float digihittime, float peSmeared, std::vector< std::pair<int,int> > digi_comp)
{
  //gate is not a trigger, but just the position of the digit in the array
  //inside the WCSimWCDigi object
#ifdef WCSIMWCDIGITIZER_VERBOSE
  G4cout<<"Adding hit "<<gate<<" in tube "<<tube
	<< " with time " << digihittime << " charge " << peSmeared
	<< " (made of " << digi_comp.size() << " raw hits with IDs ";
  for(unsigned int iv = 0; iv < digi_comp.size(); iv++)
    G4cout << " " << digi_comp[iv].first
	   << "," << digi_comp[iv].second;
  G4cout << ")";
#endif

  //  if ( digihittime > 0.0 && peSmeared>0.0)
  if (peSmeared > 0.0) {
      if ( DigiStoreHitMap[tube] == 0) {
	WCSimWCDigi* Digi = new WCSimWCDigi();
	Digi->AddParentID(1);
	
	Digi->SetTubeID(tube);
	//	Digi->AddGate(gate,digihittime);
	Digi->SetPe(gate,peSmeared);
	Digi->AddPe(digihittime);
	Digi->SetTime(gate,digihittime);
	Digi->AddDigiCompositionInfo(digi_comp);
	DigiStoreHitMap[tube] = DigiStore->insert(Digi);
#ifdef WCSIMWCDIGITIZER_VERBOSE
	G4cout << " NEW HIT" << G4endl;
#endif
      }
      else {
	//G4cout << "deja vu " << tube << " " << G << "  " << TriggerTimes[G] << " " << digihittime
	//     << "  " <<   peSmeared <<" ";
	//(*DigitsCollection)[GigiStoreHitMap[tube]-1]->AddParentID(parentID);
	//(*DigiStore)[DigiStoreHitMap[tube]-1]->AddGate(gate,digihittime);
	(*DigiStore)[DigiStoreHitMap[tube]-1]->SetPe(gate,peSmeared);
	(*DigiStore)[DigiStoreHitMap[tube]-1]->SetTime(gate,digihittime);
	(*DigiStore)[DigiStoreHitMap[tube]-1]->AddPe(digihittime);
	(*DigiStore)[DigiStoreHitMap[tube]-1]->AddDigiCompositionInfo(digi_comp);
#ifdef WCSIMWCDIGITIZER_VERBOSE
	G4cout << " DEJA VU" << G4endl;
#endif
      }
  }//peSmeared > 0
  else {
#ifdef WCSIMWCDIGITIZER_VERBOSE
    G4cout << "DIGIT REJECTED with charge " << peSmeared
	   << " time " << digihittime << G4endl;
#endif
  }
}

void WCSimWCDigitizerBase::SKDigitizerType(G4String type) {
  if(type == "SKI")
    SKDeadTime = false;
  else if(type == "SKIV")
    SKDeadTime = true;
}




// *******************************************
// DERIVED CLASS
// *******************************************

const double WCSimWCDigitizerSK::pmtgate = 200.0 ; // ns

WCSimWCDigitizerSK::WCSimWCDigitizerSK(G4String name,
				       WCSimDetectorConstruction* myDetector,
				       WCSimWCDAQMessenger* myMessenger)
  :WCSimWCDigitizerBase(name, myDetector, myMessenger)
{
}

WCSimWCDigitizerSK::~WCSimWCDigitizerSK(){
}

void WCSimWCDigitizerSK::DigitizeHits(WCSimWCDigitsCollection* WCHCPMT) {
  G4cout << "WCSimWCDigitizerSK::DigitizeHits START WCHCPMT->entries() = " << WCHCPMT->entries() << G4endl;
  //We must first sort hits by PMT in time.  This is very important as the code
  //assumes that each hit is in time order from lowest to highest.
  
  //Sorting done.  Now we integrate the charge on each PMT.
  //SK IV has a 400ns 
  //integration gate, discharges for 350ns to calculate the charge and
  //this is followed by a 150ns veto (500ns total deadtime)
  //Total processing time for a hit is 900ns
  //SK I has 2 capacitors, allowing zero deadtime
  int deadtime = 0;
  if(SKDeadTime)
    deadtime = 350 + 150;

  for (G4int i = 0 ; i < WCHCPMT->entries() ; i++)
    {
      //loop over entires in WCHCPMT, each entry corresponds to
      //the photons on one PMT

      //Sort photons on this pmt
      (*WCHCPMT)[i]->SortArrayByHitTime();
      int tube = (*WCHCPMT)[i]->GetTubeID();
#ifdef WCSIMWCDIGITIZER_VERBOSE
      G4cout << "tube " << tube
	     << " totalpe = " << (*WCHCPMT)[i]->GetTotalPe()
	     << " times";
      for(int ip = 0; ip < (*WCHCPMT)[i]->GetTotalPe(); ip++)
	G4cout << " " << (*WCHCPMT)[i]->GetTime(ip);
      /*
	G4cout<<" parents =\t";
      for( G4int ip = 0 ; ip < (*WCHCPMT)[i]->GetTotalPe() ; ip++)
	G4cout << " " << (*WCHCPMT)[i]->GetParentID(ip);
      */
      G4cout <<G4endl;
#endif
      //look over all hits on the PMT
      //integrate charge and start digitizing
      float intgr_start=0;
      float upperlimit=0;
      G4double efficiency = 0.985; // with skrn1pe (AP tuning) & 30% QE increase in stacking action
      int ngate=0;

      // Variables to store photon uniqueid that make up a digit
      int digi_unique_id   = 0;
      int photon_unique_id = 0;
      std::vector< std::pair<int,int> > digi_comp; 

      /*
      G4cout << "DEBUGGING TUBE " << tube
	     << " total pe " << (*WCHCPMT)[i]->GetTotalPe();
      for( G4int ip = 0 ; ip < (*WCHCPMT)[i]->GetTotalPe() ; ip++)
	G4cout << " time " << (*WCHCPMT)[i]->GetTime(ip)
	       << " pe " << (*WCHCPMT)[i]->GetPe(ip);
      G4cout << G4endl;
      */

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
	  
#ifdef WCSIMWCDIGITIZER_VERBOSE
	  G4cout << "ip "    << ip
		 << " pe "   << pe
		 << " time " << time
		 << " intgr_start " << intgr_start
		 << " upperlimit "  << upperlimit
		 << G4endl;
#endif

	  bool MakeDigit = false;
	  if(time >= intgr_start && time <= upperlimit) {
	    peSmeared += pe;
	    photon_unique_id = ip;
	    digi_comp.push_back( std::make_pair(digi_unique_id, photon_unique_id) );
      
#ifdef WCSIMWCDIGITIZER_VERBOSE
	    std::cout<<"INFO: time "<<time<<" digi_id "<<digi_unique_id<<" p_id "<<photon_unique_id<<std::endl;
#endif
	    //if this is the last digit, make sure to make the digit
	    if(ip + 1 == (*WCHCPMT)[i]->GetTotalPe()){
	      MakeDigit = true;
	    }
	    
	  }
	  //if ensures we don't append the same digit multiple times while in the integration window
	  else if(digi_comp.size()) {
	    //this hit is outside the integration time window.
	    //Charge integration is over.  For SKIV, there is now a 350ns + 150ns
	    //time period where no hits can be recorded
	    //Check if previous hit passed the threshold.  If so we will digitize the hit
	    MakeDigit = true;
	    // 	    int iflag;
	    // 	    WCSimWCDigitizerSK::Threshold(peSmeared,iflag);
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
	    WCSimWCDigitizerSK::Threshold(peSmeared,iflag);                                                                                                                      
	    if(iflag == 0) {                                                                                                                                             
	      //digitize hit                                                                                                                                  
	      peSmeared *= efficiency;
	      WCSimWCDigitizerBase::AddNewDigit(tube, ngate, intgr_start, peSmeared, digi_comp);
	      ngate++;

	      assert(digi_comp.size());
	      
	      digi_unique_id++;
	      digi_comp.clear();
	    }
	    else {
	      //reject hit
#ifdef WCSIMWCDIGITIZER_VERBOSE
	      G4cout << "DIGIT REJECTED with time " << intgr_start << G4endl;
#endif
	    }
	  }
	  
	  //Now try and deal with the next hit
	  if(time > upperlimit && time <= upperlimit + deadtime) {
	    continue;
	  }
	  else if(time > upperlimit + deadtime){
#ifdef WCSIMWCDIGITIZER_VERBOSE
	    G4cout<<"*** PREPARING FOR >1 DIGI ***"<<G4endl;
#endif
	    //we now need to start integrating from the hit
	    intgr_start=time;
	    peSmeared = pe;
	    //Set the limits of the integration window [intgr_start,upperlimit]                                                                                 
	    upperlimit = intgr_start + pmtgate;

	    //store the digi composition information
	    photon_unique_id = ip;
            digi_comp.push_back( std::make_pair(digi_unique_id, photon_unique_id) );

	    //if this is the last hit we must handle the creation of the digit 
	    //as the loop will not evaluate again
	    if(ip+1 == (*WCHCPMT)[i]->GetTotalPe()) {
	      int iflag;
	      WCSimWCDigitizerSK::Threshold(peSmeared,iflag);
	      if(iflag == 0) {
		//digitize hit                                                                                                                                                  
		peSmeared *= efficiency;
		WCSimWCDigitizerBase::AddNewDigit(tube, ngate, intgr_start, peSmeared, digi_comp);
		ngate++;

		assert(digi_comp.size());

		digi_unique_id++;
		digi_comp.clear();
	      }
	      else {
		//reject hit
#ifdef WCSIMWCDIGITIZER_VERBOSE
		G4cout << "DIGIT REJECTED with time " << intgr_start << G4endl;
#endif
	      }
	    }
	  }
	}//ip (totalpe)
    }//i (WCHCPMT->entries())
  G4cout<<"WCSimWCDigitizerSK::DigitizeHits END DigiStore->entries() " << DigiStore->entries() << "\n";
  
#ifdef WCSIMWCDIGITIZER_VERBOSE
  G4cout<<"\n\n\nCHECK DIGI COMP:"<<G4endl;
  for (G4int i = 0 ; i < DigiStore->entries() ; i++){
    std::vector< std::pair<int,int> > comp = (*DigiStore)[i]->GetDigiCompositionInfo();
    int tubeid = (*DigiStore)[i]->GetTubeID();
    for(int i = 0; i < (int) comp.size(); i++){
      G4cout<<"tube " <<tubeid<<" entry "<<i<<" digi "<<comp[i].first<< " p_id "<<comp[i].second<<G4endl;
    }
  }
#endif
}
