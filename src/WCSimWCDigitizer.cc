#include "WCSimWCDigitizer.hh"
#include "G4EventManager.hh"
#include "G4DigiManager.hh"

// for memset

//Use this and one/two of below to debug hit information
//#define WCSIMWCDIGITIZER_VERBOSE
//Print out hits with PMT IDs up to N
#define NPMTS_VERBOSE -1
//And/Or a specific PMT ID
#define VERBOSE_PMT -1

#ifndef HYPER_VERBOSITY
// #define HYPER_VERBOSITY
#endif

// *******************************************
// BASE CLASS
// *******************************************

WCSimWCDigitizerBase::WCSimWCDigitizerBase(G4String name,
										   WCSimDetectorConstruction* inDetector,
										   WCSimWCDAQMessenger* myMessenger,
										   DigitizerType_t digitype,
										   G4String detectorElement)
  :G4VDigitizerModule(name), fMyDetector(inDetector),
        fDAQMessenger(myMessenger), fDigitizerClassName(""),
        fDigitizerType(digitype), fDetectorElement(detectorElement)
{
  // G4String colName = "WCDigitizedStoreCollection";
  G4String colName;
  if(fDetectorElement=="tank") colName = "WCDigitizedStoreCollection";
  else if(fDetectorElement=="OD") colName = "WCDigitizedStoreCollection_OD";
  collectionName.push_back(colName);
  ReInitialize();

#ifdef HYPER_VERBOSITY
	if(fDetectorElement=="OD")G4cout<<"WCSimWCDigitizerBase::WCSimWCDigitizerBase ☆ recording collection name "<<colName<<" for "<<fDetectorElement<<G4endl;
#endif

  if(fDAQMessenger == NULL) {
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
  fDigitizerDeadTime          = GetDefaultDeadTime();
  fDigitizerIntegrationWindow = GetDefaultIntegrationWindow();
  fDigitizerTimingPrecision   = GetDefaultTimingPrecision();
  fDigitizerPEPrecision       = GetDefaultPEPrecision();

  //read the .mac file to override them
  if(fDAQMessenger != NULL) {
    fDAQMessenger->TellMeAboutTheDigitizer(this);
    fDAQMessenger->SetDigitizerOptions();
  }
  else {
    G4cerr << "WCSimWCDAQMessenger pointer is NULL when used in WCSimWCDigitizerBase::GetVariables(). Exiting..." 
	   << G4endl;
    exit(-1);
  }

  G4cout << "Using digitizer deadtime "           << fDigitizerDeadTime          << " ns" << G4endl;
  G4cout << "Using digitizer integration window " << fDigitizerIntegrationWindow << " ns" << G4endl;
  G4cout << "Using digitizer time resolution "    << fDigitizerTimingPrecision   << " ns" << G4endl;
  G4cout << "Using digitizer charge resolution "  << fDigitizerPEPrecision       << " p.e." << G4endl;
}

void WCSimWCDigitizerBase::Digitize()
{
  //Input is WCSimWCDigitsCollection with raw PMT hits (photon + dark noise)
  //Output is WCSimWCDigitsCollection with digitied PMT hits

  //Clear the fDigiStoreHitMap
  ReInitialize();

  //Temporary Storage of Digitized hits which is passed to the trigger
  fDigiStore = new WCSimWCDigitsCollection(collectionName[0],collectionName[0]);

  G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();
  
  // Get the PMT collection ID
  // G4int WCHCID = DigiMan->GetDigiCollectionID("WCRawPMTSignalCollection");

  G4String rawcollectionName;
  if(fDetectorElement=="tank") rawcollectionName = "WCRawPMTSignalCollection";
  else if(fDetectorElement=="OD") rawcollectionName = "WCRawPMTSignalCollection_OD";
  G4int WCHCID = DigiMan->GetDigiCollectionID(rawcollectionName);

  // Get the PMT Digits collection
  WCSimWCDigitsCollection* WCHCPMT = 
    (WCSimWCDigitsCollection*)(DigiMan->GetDigiCollection(WCHCID));

#ifdef HYPER_VERBOSITY
	if(fDetectorElement=="OD"){
		G4cout << "WCSimWCDigitizerBase::Digitize ☆ making digits collection (WCSimWCDigitsCollection*)"<<collectionName[0]
					 << " for "<<fDetectorElement<<" and calling DigitizeHits on "<<rawcollectionName<<" to fill it"<<G4endl;}
#endif
  
  if (WCHCPMT) {
    DigitizeHits(WCHCPMT);
  } else {
	  G4cout << "WCSimWCDigitizerBase::Digitize didn't find hit collection for " << fDetectorElement << G4endl;
  }
  
  StoreDigiCollection(fDigiStore);

}

bool WCSimWCDigitizerBase::AddNewDigit(int tube, int gate, double digihittime, double peSmeared, std::vector<int> digi_comp)
{
  //digitised hit information does not have infinite precision
  //so need to round the charge and time information
  double digihittime_d = Truncate(digihittime, fDigitizerTimingPrecision);
  double peSmeared_d   = Truncate(peSmeared,   fDigitizerPEPrecision);

  //gate is not a trigger, but just the position of the digit in the array
  //inside the WCSimWCDigi object
#ifdef WCSIMWCDIGITIZER_VERBOSE
  if(tube < NPMTS_VERBOSE || tube == VERBOSE_PMT) {
    G4cout<<"Adding hit "<<gate<<" in tube "<<tube
	  << " with time " << digihittime_d << " charge " << peSmeared_d
	  << " (truncated from t: " << digihittime << " q: " << peSmeared << ")"
	  << " (made of " << digi_comp.size() << " raw hits with IDs ";
    for(unsigned int iv = 0; iv < digi_comp.size(); iv++)
      G4cout << " " << digi_comp[iv] << ",";
    G4cout << ")";
  }
#endif

  //use un-truncated peSmeared here, so that truncation does not affect the test
  if (peSmeared > 0.0) {
      if ( fDigiStoreHitMap[tube] == 0) {
	WCSimWCDigi* Digi = new WCSimWCDigi();
	Digi->SetTubeID(tube);
	Digi->SetPe(gate,peSmeared_d);
	Digi->AddPe(digihittime_d);
	Digi->SetTime(gate,digihittime_d);
	Digi->AddDigiCompositionInfo(digi_comp);
	fDigiStoreHitMap[tube] = fDigiStore->insert(Digi);
#ifdef WCSIMWCDIGITIZER_VERBOSE
	if(tube < NPMTS_VERBOSE || tube == VERBOSE_PMT)
	  G4cout << " NEW HIT" << G4endl;
#endif
      }
      else {
	(*fDigiStore)[fDigiStoreHitMap[tube]-1]->SetPe(gate,peSmeared_d);
	(*fDigiStore)[fDigiStoreHitMap[tube]-1]->SetTime(gate,digihittime_d);
	(*fDigiStore)[fDigiStoreHitMap[tube]-1]->AddPe(digihittime_d);
	(*fDigiStore)[fDigiStoreHitMap[tube]-1]->AddDigiCompositionInfo(digi_comp);
#ifdef WCSIMWCDIGITIZER_VERBOSE
	if(tube < NPMTS_VERBOSE || tube == VERBOSE_PMT)
	  G4cout << " DEJA VU" << G4endl;
#endif
      }
      return true;
  }//peSmeared > 0
  else {
#ifdef WCSIMWCDIGITIZER_VERBOSE
    if(tube < NPMTS_VERBOSE || tube == VERBOSE_PMT)
      G4cout << "DIGIT REJECTED with charge " << peSmeared_d
	     << " time " << digihittime_d << G4endl;
#endif
    return false;
  }
}

void WCSimWCDigitizerBase::SaveOptionsToOutput(WCSimRootOptions * wcopt)
{
  wcopt->SetDigitizerClassName(fDigitizerClassName);
  wcopt->SetDigitizerDeadTime(fDigitizerDeadTime);
  wcopt->SetDigitizerIntegrationWindow(fDigitizerIntegrationWindow);
  wcopt->SetDigitizerTimingPrecision(fDigitizerTimingPrecision);
  wcopt->SetDigitizerPEPrecision(fDigitizerPEPrecision);
}


// *******************************************
// DERIVED CLASS
// *******************************************

WCSimWCDigitizerSKI::WCSimWCDigitizerSKI(G4String name,
                                         WCSimDetectorConstruction* myDetector,
                                         WCSimWCDAQMessenger* myMessenger,
                                         G4String detectorElement)
  : WCSimWCDigitizerBase(name, myDetector, myMessenger, kDigitizerSKI, detectorElement)
{
  fDigitizerClassName = "SKI";
  GetVariables();
}

WCSimWCDigitizerSKI::~WCSimWCDigitizerSKI(){
}

void WCSimWCDigitizerSKI::DigitizeHits(WCSimWCDigitsCollection* WCHCPMT) {

  if(fDetectorElement=="tank") G4cout << "TANK # ";
  if(fDetectorElement=="OD")   G4cout << "OD # ";
  G4cout << "WCSimWCDigitizerSKI::DigitizeHits START WCHCPMT->entries() = " << WCHCPMT->entries() << G4endl;

  //Get the PMT info for hit time smearing
  G4String WCIDCollectionName = fMyDetector->GetIDCollectionName();
  WCSimPMTObject * PMT = fMyDetector->GetPMTPointer(WCIDCollectionName);

  // G. Pronost 2019/09/09:
  // Hit need to be sorted! (This is done no where!)
  std::sort(WCHCPMT->GetVector()->begin(), WCHCPMT->GetVector()->end(), WCSimWCDigi::SortFunctor_Hit());
  
  //loop over entires in WCHCPMT, each entry corresponds to
  //the photons on one PMT
  int absoluteindex=0;
  for (G4int i = 0 ; i < WCHCPMT->entries() ; i++)
    {

      //We must first sort hits by PMT in time.  This is very important as the code
      //assumes that each hit is in time order from lowest to highest.
      (*WCHCPMT)[i]->SortDigiMapsByHitTime();
      int tube = (*WCHCPMT)[i]->GetTubeID();
#ifdef WCSIMWCDIGITIZER_VERBOSE
      if(tube < NPMTS_VERBOSE || tube == VERBOSE_PMT) {
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
      // Integration occurs for fDigitizerIntegrationWindow ns (user set)
      // Digitizer is then dead for fDigitizerDeadTime ns (user set)

      //look over all hits on the PMT
      //integrate charge and start digitizing
      double intgr_start=0;
      double upperlimit=0;
      G4double efficiency = 0.985; // with skrn1pe (AP tuning) & 30% QE increase in stacking action

      // Variables to store photon uniqueid that make up a digit
      int digi_unique_id   = 0;
      int photon_unique_id = 0;
      std::vector<int> digi_comp; 

      //loop over the hits on this PMT
      for( G4int ip = 0 ; ip < (*WCHCPMT)[i]->GetTotalPe() ; ip++)
	{
	  double time = (*WCHCPMT)[i]->GetTime(ip);
          double pe = (*WCHCPMT)[i]->GetPe(ip);

	  //start the integration time as the time of the first hit
	  //Hits must be sorted in time
	  if(ip==0) {
	    intgr_start=time;
	    fPESmeared = 0;
	    //Set the limits of the integration window [intgr_start,upperlimit]                                                                                            
	    upperlimit = intgr_start + fDigitizerIntegrationWindow;
	  }
	  
#ifdef WCSIMWCDIGITIZER_VERBOSE
	  if(tube < NPMTS_VERBOSE || tube == VERBOSE_PMT)
	    G4cout << "ip "    << ip
		   << " pe "   << pe
		   << " time " << time
		   << " intgr_start " << intgr_start
		   << " upperlimit "  << upperlimit
		   << G4endl;
#endif

	  bool MakeDigit = false;
	  if(time >= intgr_start && time <= upperlimit) {
	    fPESmeared += pe;
	    photon_unique_id = ip+absoluteindex;
	    digi_comp.push_back(photon_unique_id);
      
#ifdef WCSIMWCDIGITIZER_VERBOSE
	    if(tube < NPMTS_VERBOSE || tube == VERBOSE_PMT)
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
	    //Charge integration is over.  The is now a fDigitizerDeadTime ns dead
	    //time period where no hits can be recorded
	    MakeDigit = true;
	  }
	  
	  //Make digit here
	  if(MakeDigit) {
	    int iflag;
	    WCSimWCDigitizerSKI::Threshold(fPESmeared,iflag);

	    //Check if previous hit passed the threshold.  If so we will digitize the hit
	    if(iflag == 0) {
	      //apply time smearing
	      double Q = (fPESmeared > 0.5) ? fPESmeared : 0.5;
	      //digitize hit
	      fPESmeared *= efficiency;
	      bool accepted = WCSimWCDigitizerBase::AddNewDigit(tube, digi_unique_id,
								intgr_start + PMT->HitTimeSmearing(Q),
								fPESmeared, digi_comp);
	      if(accepted) {
		digi_unique_id++;
	      }
	      assert(digi_comp.size());
	      digi_comp.clear();
	    }
	    else {
	      //reject hit
#ifdef WCSIMWCDIGITIZER_VERBOSE
	      if(tube < NPMTS_VERBOSE || tube == VERBOSE_PMT)
		G4cout << "DIGIT REJECTED with time " << intgr_start << G4endl;
#endif
	      digi_comp.clear();
	    }
	  }
	  
	  //Now try and deal with the next hit
	  if(time > upperlimit && time <= upperlimit + fDigitizerDeadTime) {
	    //Now we need to reject hits that are after the integration
	    //period to the end of the veto signal
	    continue;
	  }
	  else if(time > upperlimit + fDigitizerDeadTime){
#ifdef WCSIMWCDIGITIZER_VERBOSE
	    if(tube < NPMTS_VERBOSE || tube == VERBOSE_PMT)
	      G4cout<<"*** PREPARING FOR >1 DIGI ***"<<G4endl;
#endif
	    //we now need to start integrating from the hit
	    intgr_start=time;
	    fPESmeared = pe;
	    //Set the limits of the integration window [intgr_start,upperlimit]
	    upperlimit = intgr_start + fDigitizerIntegrationWindow;

	    //store the digi composition information
	    photon_unique_id = ip+absoluteindex;
            digi_comp.push_back(photon_unique_id);

	    //if this is the last hit we must handle the creation of the digit 
	    //as the loop will not evaluate again
	    if(ip+1 == (*WCHCPMT)[i]->GetTotalPe()) {
	      int iflag;
	      WCSimWCDigitizerSKI::Threshold(fPESmeared,iflag);
	      if(iflag == 0) {
		//apply time smearing
		double Q = (fPESmeared > 0.5) ? fPESmeared : 0.5;
		//digitize hit
		fPESmeared *= efficiency;
		bool accepted = WCSimWCDigitizerBase::AddNewDigit(tube, digi_unique_id,
								  intgr_start + PMT->HitTimeSmearing(Q),
								  fPESmeared, digi_comp);
		if(accepted) {
		  digi_unique_id++;
		}
		assert(digi_comp.size());
		digi_comp.clear();
	      }
	      else {
		//reject hit
#ifdef WCSIMWCDIGITIZER_VERBOSE
		if(tube < NPMTS_VERBOSE || tube == VERBOSE_PMT)
		  G4cout << "DIGIT REJECTED with time " << intgr_start << G4endl;
#endif
		digi_comp.clear();
	      }
	    }
	  }
	}//ip (totalpe)
	absoluteindex+=(*WCHCPMT)[i]->GetTotalPe();
    }//i (WCHCPMT->entries())
  G4cout<<"WCSimWCDigitizerSKI::DigitizeHits END fDigiStore->entries() " << fDigiStore->entries() << "\n";
  
#ifdef WCSIMWCDIGITIZER_VERBOSE
  G4cout<<"\n\n\nCHECK DIGI COMP:"<<G4endl;
  for (G4int idigi = 0 ; idigi < fDigiStore->entries() ; idigi++){
    int tubeid = (*fDigiStore)[idigi]->GetTubeID();
    if(tubeid < NPMTS_VERBOSE) {
      std::map< int, std::vector<int> > comp = (*fDigiStore)[idigi]->GetDigiCompositionInfo();
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

