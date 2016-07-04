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

#ifndef NPMTS_VERBOSE
#define NPMTS_VERBOSE 10
#endif


// *******************************************
// BASE CLASS
// *******************************************

#ifndef WCSIMWCDIGITIZER_VERBOSE
//#define WCSIMWCDIGITIZER_VERBOSE
#endif

WCSimWCDigitizerBase::WCSimWCDigitizerBase(G4String name,
					   WCSimDetectorConstruction* inDetector,
					   WCSimWCDAQMessenger* myMessenger,
					   DigitizerType_t digitype)
  :G4VDigitizerModule(name), myDetector(inDetector), DAQMessenger(myMessenger), DigitizerType(digitype)
{
  G4String colName = "WCDigitizedStoreCollection";
  collectionName.push_back(colName);
  ReInitialize();

  if(DAQMessenger == NULL) {
    G4cerr << "WCSimWCDAQMessenger pointer is NULL when passed to WCSimWCDigitizerBase constructor. Exiting..." 
	   << G4endl;
    exit(-1);
  }
}

WCSimWCDigitizerBase::~WCSimWCDigitizerBase(){
}

void WCSimWCDigitizerBase::GetVariables()
{
  //set the options to digitizer-specific defaults
  DigitizerDeadTime          = GetDefaultDeadTime();
  DigitizerIntegrationWindow = GetDefaultIntegrationWindow();

  //read the .mac file to override them
  if(DAQMessenger != NULL) {
    DAQMessenger->TellMeAboutTheDigitizer(this);
    DAQMessenger->SetDigitizerOptions();
  }
  else {
    G4cerr << "WCSimWCDAQMessenger pointer is NULL when used in WCSimWCDigitizerBase::GetVariables(). Exiting..." 
	   << G4endl;
    exit(-1);
  }

  G4cout << "Using digitizer deadtime " << DigitizerDeadTime << " ns" << G4endl;
  G4cout << "Using digitizer integration window " << DigitizerIntegrationWindow << " ns" << G4endl;
}

void WCSimWCDigitizerBase::Digitize()
{
  //Input is WCSimWCDigitsCollection with raw PMT hits (photon + dark noise)
  //Output is WCSimWCDigitsCollection with digitied PMT hits

  //Clear the DigiStoreHitMap
  ReInitialize();

  //Temporary Storage of Digitized hits which is passed to the trigger
  DigiStore = new WCSimWCDigitsCollection(collectionName[0],collectionName[0]);

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

bool WCSimWCDigitizerBase::AddNewDigit(int tube, int gate, float digihittime, float peSmeared, std::vector<int> digi_comp)
{
  //gate is not a trigger, but just the position of the digit in the array
  //inside the WCSimWCDigi object
#ifdef WCSIMWCDIGITIZER_VERBOSE
  if(tube < NPMTS_VERBOSE) {
    G4cout<<"Adding hit "<<gate<<" in tube "<<tube
	  << " with time " << digihittime << " charge " << peSmeared
	  << " (made of " << digi_comp.size() << " raw hits with IDs ";
    for(unsigned int iv = 0; iv < digi_comp.size(); iv++)
      G4cout << " " << digi_comp[iv] << ",";
    G4cout << ")";
  }
#endif

  if (peSmeared > 0.0) {
      if ( DigiStoreHitMap[tube] == 0) {
	WCSimWCDigi* Digi = new WCSimWCDigi();
	Digi->SetTubeID(tube);
	Digi->SetPe(gate,peSmeared);
	Digi->AddPe(digihittime);
	Digi->SetTime(gate,digihittime);
	Digi->AddDigiCompositionInfo(digi_comp);
	DigiStoreHitMap[tube] = DigiStore->insert(Digi);
#ifdef WCSIMWCDIGITIZER_VERBOSE
	if(tube < NPMTS_VERBOSE)
	  G4cout << " NEW HIT" << G4endl;
#endif
      }
      else {
	(*DigiStore)[DigiStoreHitMap[tube]-1]->SetPe(gate,peSmeared);
	(*DigiStore)[DigiStoreHitMap[tube]-1]->SetTime(gate,digihittime);
	(*DigiStore)[DigiStoreHitMap[tube]-1]->AddPe(digihittime);
	(*DigiStore)[DigiStoreHitMap[tube]-1]->AddDigiCompositionInfo(digi_comp);
#ifdef WCSIMWCDIGITIZER_VERBOSE
	if(tube < NPMTS_VERBOSE)
	  G4cout << " DEJA VU" << G4endl;
#endif
      }
      return true;
  }//peSmeared > 0
  else {
#ifdef WCSIMWCDIGITIZER_VERBOSE
    if(tube < NPMTS_VERBOSE)
      G4cout << "DIGIT REJECTED with charge " << peSmeared
	     << " time " << digihittime << G4endl;
#endif
    return false;
  }
}


// *******************************************
// DERIVED CLASS
// *******************************************

WCSimWCDigitizerSKI::WCSimWCDigitizerSKI(G4String name,
					 WCSimDetectorConstruction* myDetector,
					 WCSimWCDAQMessenger* myMessenger)
  : WCSimWCDigitizerBase(name, myDetector, myMessenger, kDigitizerSKI)
{
  GetVariables();
}

WCSimWCDigitizerSKI::~WCSimWCDigitizerSKI(){
}

void WCSimWCDigitizerSKI::DigitizeHits(WCSimWCDigitsCollection* WCHCPMT) {
  G4cout << "WCSimWCDigitizerSKI::DigitizeHits START WCHCPMT->entries() = " << WCHCPMT->entries() << G4endl;
  
  //loop over entires in WCHCPMT, each entry corresponds to
  //the photons on one PMT
  for (G4int i = 0 ; i < WCHCPMT->entries() ; i++)
    {

      //We must first sort hits by PMT in time.  This is very important as the code
      //assumes that each hit is in time order from lowest to highest.
      (*WCHCPMT)[i]->SortArrayByHitTime();
      int tube = (*WCHCPMT)[i]->GetTubeID();
#ifdef WCSIMWCDIGITIZER_VERBOSE
      if(tube < NPMTS_VERBOSE) {
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
      }
#endif

      //Sorting done.  Now we integrate the charge on each PMT.
      // Integration occurs for DigitizerIntegrationWindow ns (user set)
      // Digitizer is then dead for DigitizerDeadTime ns (user set)

      //look over all hits on the PMT
      //integrate charge and start digitizing
      float intgr_start=0;
      float upperlimit=0;
      G4double efficiency = 0.985; // with skrn1pe (AP tuning) & 30% QE increase in stacking action

      // Variables to store photon uniqueid that make up a digit
      int digi_unique_id   = 0;
      int photon_unique_id = 0;
      std::vector<int> digi_comp; 

      //loop over the hits on this PMT
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
	    upperlimit = intgr_start + DigitizerIntegrationWindow;
	  }
	  
#ifdef WCSIMWCDIGITIZER_VERBOSE
	  if(tube < NPMTS_VERBOSE)
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
	    digi_comp.push_back(photon_unique_id);
      
#ifdef WCSIMWCDIGITIZER_VERBOSE
	    if(tube < NPMTS_VERBOSE)
	      G4cout<<"INFO: time "<<time<<" digi_id "<<digi_unique_id<<" p_id "<<photon_unique_id<<G4endl;
#endif
	    //if this is the last digit, make sure to make the digit
	    if(ip + 1 == (*WCHCPMT)[i]->GetTotalPe()){
	      MakeDigit = true;
	    }
	    
	  }
	  //if ensures we don't append the same digit multiple times while in the integration window
	  else if(digi_comp.size()) {
	    //this hit is outside the integration time window.
	    //Charge integration is over.  The is now a DigitizerDeadTime ns dead
	    //time period where no hits can be recorded
	    MakeDigit = true;
	  }
	  
	  //Make digit here
	  if(MakeDigit) {
	    int iflag;
	    WCSimWCDigitizerSKI::Threshold(peSmeared,iflag);

	    //Check if previous hit passed the threshold.  If so we will digitize the hit
	    if(iflag == 0) {
	      //digitize hit
	      peSmeared *= efficiency;
	      bool accepted = WCSimWCDigitizerBase::AddNewDigit(tube, digi_unique_id, intgr_start, peSmeared, digi_comp);
	      if(accepted) {
		digi_unique_id++;
	      }
	      assert(digi_comp.size());
	      digi_comp.clear();
	    }
	    else {
	      //reject hit
#ifdef WCSIMWCDIGITIZER_VERBOSE
	      if(tube < NPMTS_VERBOSE)
		G4cout << "DIGIT REJECTED with time " << intgr_start << G4endl;
#endif
	      digi_comp.clear();
	    }
	  }
	  
	  //Now try and deal with the next hit
	  if(time > upperlimit && time <= upperlimit + DigitizerDeadTime) {
	    //Now we need to reject hits that are after the integration
	    //period to the end of the veto signal
	    continue;
	  }
	  else if(time > upperlimit + DigitizerDeadTime){
#ifdef WCSIMWCDIGITIZER_VERBOSE
	    if(tube < NPMTS_VERBOSE)
	      G4cout<<"*** PREPARING FOR >1 DIGI ***"<<G4endl;
#endif
	    //we now need to start integrating from the hit
	    intgr_start=time;
	    peSmeared = pe;
	    //Set the limits of the integration window [intgr_start,upperlimit]
	    upperlimit = intgr_start + DigitizerIntegrationWindow;

	    //store the digi composition information
	    photon_unique_id = ip;
            digi_comp.push_back(photon_unique_id);

	    //if this is the last hit we must handle the creation of the digit 
	    //as the loop will not evaluate again
	    if(ip+1 == (*WCHCPMT)[i]->GetTotalPe()) {
	      int iflag;
	      WCSimWCDigitizerSKI::Threshold(peSmeared,iflag);
	      if(iflag == 0) {
		//digitize hit
		peSmeared *= efficiency;
		bool accepted = WCSimWCDigitizerBase::AddNewDigit(tube, digi_unique_id, intgr_start, peSmeared, digi_comp);
		if(accepted) {
		  digi_unique_id++;
		}
		assert(digi_comp.size());
		digi_comp.clear();
	      }
	      else {
		//reject hit
#ifdef WCSIMWCDIGITIZER_VERBOSE
		if(tube < NPMTS_VERBOSE)
		  G4cout << "DIGIT REJECTED with time " << intgr_start << G4endl;
#endif
		digi_comp.clear();
	      }
	    }
	  }
	}//ip (totalpe)
    }//i (WCHCPMT->entries())
  G4cout<<"WCSimWCDigitizerSKI::DigitizeHits END DigiStore->entries() " << DigiStore->entries() << "\n";
  
#ifdef WCSIMWCDIGITIZER_VERBOSE
  G4cout<<"\n\n\nCHECK DIGI COMP:"<<G4endl;
  for (G4int idigi = 0 ; idigi < DigiStore->entries() ; idigi++){
    int tubeid = (*DigiStore)[idigi]->GetTubeID();
    if(tubeid < NPMTS_VERBOSE) {
      std::map< int, std::vector<int> > comp = (*DigiStore)[idigi]->GetDigiCompositionInfo();
      for(size_t i = 0; i < comp.size(); i++){
	G4cout << "tube "  << tubeid
	       << " gate " << i << " p_id";
	for(size_t iv = 0; iv < comp[i].size(); iv++) {
	  G4cout << " " << comp[i][iv];
	}//iv
	G4cout << G4endl;
      }//i
    }
  }//idigi
#endif
}
