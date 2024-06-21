#include "WCSimWCDigitizer.hh"
#include "G4EventManager.hh"
#include "G4DigiManager.hh"

#include "WCSimDetectorConstruction.hh"
#include "WCSimPmtInfo.hh"
#include "WCSimDarkRateMessenger.hh"

#include <fstream>
#include <vector>

//Use this and one/two of below to debug hit information
#ifndef WCSIMWCDIGITIZER_VERBOSE
#define WCSIMWCDIGITIZER_VERBOSE
#endif
//Print out hits with PMT IDs up to N
#ifndef NPMTS_VERBOSE
#define NPMTS_VERBOSE 0
#endif //NPMTS_VERBOSE
//And/Or a specific PMT ID
#ifndef VERBOSE_PMT
#define VERBOSE_PMT -1
#endif //VERBOSE_PMT

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
					   G4String inDetectorElement)
  :G4VDigitizerModule(name), myDetector(inDetector), DAQMessenger(myMessenger), detectorElement(inDetectorElement), DigitizerClassName(""), DigitizerType(digitype)
{
  G4String colName;
  if(detectorElement=="tank") colName = "WCDigitizedStoreCollection";
  else if(detectorElement=="tankPMT2") colName = "WCDigitizedStoreCollection2";
  else if(detectorElement=="OD") colName = "WCDigitizedStoreCollection_OD";
  collectionName.push_back(colName);
  ReInitialize();

#ifdef HYPER_VERBOSITY
	if(detectorElement=="OD")G4cout<<"WCSimWCDigitizerBase::WCSimWCDigitizerBase ☆ recording collection name "<<colName<<" for "<<detectorElement<<G4endl;
#endif

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
  DigitizerTimingPrecision   = GetDefaultTimingPrecision();
  DigitizerPEPrecision       = GetDefaultPEPrecision();

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

  G4cout << "Using digitizer deadtime "           << DigitizerDeadTime          << " ns" << G4endl;
  G4cout << "Using digitizer integration window " << DigitizerIntegrationWindow << " ns" << G4endl;
  G4cout << "Using digitizer time resolution "    << DigitizerTimingPrecision   << " ns" << G4endl;
  G4cout << "Using digitizer charge resolution "  << DigitizerPEPrecision       << " p.e." << G4endl;
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
  G4String rawcollectionName;
  if(detectorElement=="tank") rawcollectionName = "WCRawPMTSignalCollection";
  else if(detectorElement=="tankPMT2") rawcollectionName = "WCRawPMTSignalCollection2";
  else if(detectorElement=="OD") rawcollectionName = "WCRawPMTSignalCollection_OD";
  G4int WCHCID = DigiMan->GetDigiCollectionID(rawcollectionName);

  // Get the PMT Digits collection
  WCSimWCDigitsCollection* WCHCPMT = 
    (WCSimWCDigitsCollection*)(DigiMan->GetDigiCollection(WCHCID));

#ifdef HYPER_VERBOSITY
	if(detectorElement=="OD"){
		G4cout << "WCSimWCDigitizerBase::Digitize ☆ making digits collection (WCSimWCDigitsCollection*)"<<collectionName[0]
					 << " for "<<detectorElement<<" and calling DigitizeHits on "<<rawcollectionName<<" to fill it"<<G4endl;}
#endif
  
  if (WCHCPMT) {
    DigitizeHits(WCHCPMT);
  } else {
	  G4cout << "WCSimWCDigitizerBase::Digitize didn't find hit collection for " << detectorElement << G4endl;
  }
  
  StoreDigiCollection(DigiStore);

}

bool WCSimWCDigitizerBase::AddNewDigit(int tube, int gate, double digihittime, double peSmeared, std::vector<int> digi_comp)
{
  //digitised hit information does not have infinite precision
  //so need to round the charge and time information
  double digihittime_d = Truncate(digihittime, DigitizerTimingPrecision);
  double peSmeared_d   = Truncate(peSmeared,   DigitizerPEPrecision);

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
      if ( DigiStoreHitMap[tube] == 0) {
	WCSimWCDigi* Digi = new WCSimWCDigi();
	Digi->SetTubeID(tube);
	Digi->SetPe(gate,peSmeared_d);
	Digi->AddPe(digihittime_d);
	Digi->SetTime(gate,digihittime_d);
	Digi->AddDigiCompositionInfo(digi_comp);
	DigiStoreHitMap[tube] = DigiStore->insert(Digi);
#ifdef WCSIMWCDIGITIZER_VERBOSE
	if(tube < NPMTS_VERBOSE || tube == VERBOSE_PMT)
	  G4cout << " NEW HIT" << G4endl;
#endif
      }
      else {
	(*DigiStore)[DigiStoreHitMap[tube]-1]->SetPe(gate,peSmeared_d);
	(*DigiStore)[DigiStoreHitMap[tube]-1]->SetTime(gate,digihittime_d);
	(*DigiStore)[DigiStoreHitMap[tube]-1]->AddPe(digihittime_d);
	(*DigiStore)[DigiStoreHitMap[tube]-1]->AddDigiCompositionInfo(digi_comp);
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
  wcopt->SetDigitizerClassName(DigitizerClassName);
  wcopt->SetDigitizerDeadTime(DigitizerDeadTime);
  wcopt->SetDigitizerIntegrationWindow(DigitizerIntegrationWindow);
  wcopt->SetDigitizerTimingPrecision(DigitizerTimingPrecision);
  wcopt->SetDigitizerPEPrecision(DigitizerPEPrecision);
}


// *******************************************
// DERIVED CLASS
// *******************************************

WCSimWCDigitizerSKI::WCSimWCDigitizerSKI(G4String name,
					 WCSimDetectorConstruction* inDetector,
					 WCSimWCDAQMessenger* myMessenger,
					 G4String inDetectorElement)
  : WCSimWCDigitizerBase(name, inDetector, myMessenger, kDigitizerSKI, inDetectorElement)
{
  DigitizerClassName = "SKI";
  GetVariables();
}

WCSimWCDigitizerSKI::~WCSimWCDigitizerSKI(){
}

void WCSimWCDigitizerSKI::DigitizeHits(WCSimWCDigitsCollection* WCHCPMT) {

  if(detectorElement=="tank") G4cout << "TANK 20in # ";
  if(detectorElement=="tankPMT2") G4cout << "TANK mPMT # ";
  if(detectorElement=="OD")   G4cout << "OD # ";
#ifdef WCSIMWCDIGITIZER_VERBOSE
  G4cout << "WCSimWCDigitizerSKI::DigitizeHits START WCHCPMT->entries() = " << WCHCPMT->entries() << " hits" << G4endl;
#endif

  //********************************************************************************************************* TD 2019.07.16 : to take PMT saturation effect into account *********************
  double saturThreshold = myDetector->GetParameters()->GetPMTSatur();                                                                                             
  //double qoiff = myDetector->GetParameters()->GetQoiff();

  G4String WCCollectionName;
  if(detectorElement=="tank") WCCollectionName = myDetector->GetIDCollectionName();
  else if(detectorElement=="tankPMT2") WCCollectionName = myDetector->GetIDCollectionName2();
  else if(detectorElement=="OD") WCCollectionName = myDetector->GetODCollectionName();
  WCSimPMTObject * PMT = myDetector->GetPMTPointer(WCCollectionName);
  
  // G. Pronost 2019/09/09:
  // Hit need to be sorted! (This is done no where!)
  std::sort(WCHCPMT->GetVector()->begin(), WCHCPMT->GetVector()->end(), WCSimWCDigi::SortFunctor_Hit());

  //std::fstream file;
  //G4String filename = Form("saturation_effect_%.2fpeThreshold.txt",saturThreshold);
  //file.open(filename.c_str(), std::ios::out | std::ios::app);
  //******************************************************************************************************************************************************************************************
  
  //loop over entires in WCHCPMT, each entry corresponds to
  //the photons on one PMT
  int absoluteindex=0;
  double peSmeared = 0.;
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
      // Integration occurs for DigitizerIntegrationWindow ns (user set)
      // Digitizer is then dead for DigitizerDeadTime ns (user set)

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
          //const int parent_id = (*WCHCPMT)[i]->GetParentID(ip);

	  //start the integration time as the time of the first hit
	  //Hits must be sorted in time
	  if(ip==0) {
	    intgr_start=time;
	    peSmeared = 0;
	    //Set the limits of the integration window [intgr_start,upperlimit]                                                                                            
	    upperlimit = intgr_start + DigitizerIntegrationWindow;
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
	    peSmeared += pe;

	    // set to (effectively) the position in the WCSimRootCherenkovHitTimes array
	    // (i.e. the total photon number)
	    // previously
	    //  - the position would be the position of the hit on this particular PMT
	    //    (i.e. the photon number on this PMT)
	    //  - except -1 would indicate dark noise
	    photon_unique_id = ip + absoluteindex;
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
	      //apply time smearing
	      double Q = (peSmeared > 0.5) ? peSmeared : 0.5;
	      //digitize hit
	      peSmeared *= efficiency;

	      //******************************************************************************************************** TD 2019.07.16 : to take PMT saturation effect into account *********************
	      //file << peSmeared << " ";
	      peSmeared *= PMT->SaturFactor(peSmeared, saturThreshold);
	      //file << peSmeared << "\n";
	      //*****************************************************************************************************************************************************************************************

	      bool accepted = WCSimWCDigitizerBase::AddNewDigit(tube, digi_unique_id,
								intgr_start + PMT->HitTimeSmearing(Q, 1.),
								peSmeared, digi_comp);
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
	  if(time > upperlimit && time <= upperlimit + DigitizerDeadTime) {
	    //Now we need to reject hits that are after the integration
	    //period to the end of the veto signal
	    continue;
	  }
	  else if(time > upperlimit + DigitizerDeadTime){
#ifdef WCSIMWCDIGITIZER_VERBOSE
	    if(tube < NPMTS_VERBOSE || tube == VERBOSE_PMT)
	      G4cout<<"*** PREPARING FOR >1 DIGI ***"<<G4endl;
#endif
	    //we now need to start integrating from the hit
	    intgr_start=time;
	    peSmeared = pe;
	    //Set the limits of the integration window [intgr_start,upperlimit]
	    upperlimit = intgr_start + DigitizerIntegrationWindow;

	    //store the digi composition information
	    photon_unique_id = ip+absoluteindex;
            digi_comp.push_back(photon_unique_id);

	    //if this is the last hit we must handle the creation of the digit 
	    //as the loop will not evaluate again
	    if(ip+1 == (*WCHCPMT)[i]->GetTotalPe()) {
	      int iflag;
	      WCSimWCDigitizerSKI::Threshold(peSmeared,iflag);
	      if(iflag == 0) {
		//apply time smearing
		double Q = (peSmeared > 0.5) ? peSmeared : 0.5;
		//digitize hit
		peSmeared *= efficiency;
		bool accepted = WCSimWCDigitizerBase::AddNewDigit(tube, digi_unique_id,
								  intgr_start + PMT->HitTimeSmearing(Q, 1.0),
								  peSmeared, digi_comp);
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
#ifdef WCSIMWCDIGITIZER_VERBOSE
  G4cout<<"WCSimWCDigitizerSKI::DigitizeHits END DigiStore->entries() " << DigiStore->entries() << " digits" << G4endl;
#endif
  //******************************************************************************************************** TD 2019.07.16 : to take PMT saturation effect into account *********************
  //file.close();
  //*****************************************************************************************************************************************************************************************

#ifdef WCSIMWCDIGITIZER_VERBOSE
  if(NPMTS_VERBOSE > 0 || VERBOSE_PMT > 0)
    G4cout<<"\n\n\nCHECK DIGI COMP:"<<G4endl;
  for (G4int idigi = 0 ; idigi < DigiStore->entries() ; idigi++){
    int tubeid = (*DigiStore)[idigi]->GetTubeID();
    if(tubeid < NPMTS_VERBOSE || tubeid == VERBOSE_PMT) {
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

WCSimWCDigitizerWCTE::WCSimWCDigitizerWCTE(G4String name,
					  WCSimDetectorConstruction* inDetector,
					  WCSimWCDAQMessenger* myMessenger,
					  G4String inDetectorElement)
  : WCSimWCDigitizerBase(name, inDetector, myMessenger, kDigitizerSKI, inDetectorElement)
{
  DigitizerClassName = "WCTE";
  GetVariables();
}

WCSimWCDigitizerWCTE::~WCSimWCDigitizerWCTE(){
}

void WCSimWCDigitizerWCTE::DigitizeHits(WCSimWCDigitsCollection* WCHCPMT) {

  if(detectorElement=="tank") G4cout << "TANK 20in # ";
  if(detectorElement=="tankPMT2") G4cout << "TANK mPMT # ";
  if(detectorElement=="OD")   G4cout << "OD # ";
#ifdef WCSIMWCDIGITIZER_VERBOSE
  G4cout << "WCSimWCDigitizerWCTE::DigitizeHits START WCHCPMT->entries() = " << WCHCPMT->entries() << " hits" << G4endl;
#endif

  //********************************************************************************************************* TD 2019.07.16 : to take PMT saturation effect into account *********************
  double saturThreshold = myDetector->GetParameters()->GetPMTSatur();                                                                                             
  //double qoiff = myDetector->GetParameters()->GetQoiff();

  G4String WCCollectionName;
  if(detectorElement=="tank") WCCollectionName = myDetector->GetIDCollectionName();
  else if(detectorElement=="tankPMT2") WCCollectionName = myDetector->GetIDCollectionName2();
  else if(detectorElement=="OD") WCCollectionName = myDetector->GetODCollectionName();
  WCSimPMTObject * PMT = myDetector->GetPMTPointer(WCCollectionName);
  
  // G. Pronost 2019/09/09:
  // Hit need to be sorted! (This is done no where!)
  std::sort(WCHCPMT->GetVector()->begin(), WCHCPMT->GetVector()->end(), WCSimWCDigi::SortFunctor_Hit());

  //std::fstream file;
  //G4String filename = Form("saturation_effect_%.2fpeThreshold.txt",saturThreshold);
  //file.open(filename.c_str(), std::ios::out | std::ios::app);
  //******************************************************************************************************************************************************************************************
  
  //loop over entires in WCHCPMT, each entry corresponds to
  //the photons on one PMT
  int absoluteindex=0;
  double peSmeared = 0.;
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
      // Integration occurs for DigitizerIntegrationWindow ns (user set)
      // Digitizer is then dead for DigitizerDeadTime ns (user set)

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
          //const int parent_id = (*WCHCPMT)[i]->GetParentID(ip);

	  //start the integration time as the time of the first hit
	  //Hits must be sorted in time
	  if(ip==0) {
	    intgr_start=time;
	    peSmeared = 0;
	    //Set the limits of the integration window [intgr_start,upperlimit]                                                                                            
	    upperlimit = intgr_start + DigitizerIntegrationWindow;
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
	    peSmeared += pe;

	    // set to (effectively) the position in the WCSimRootCherenkovHitTimes array
	    // (i.e. the total photon number)
	    // previously
	    //  - the position would be the position of the hit on this particular PMT
	    //    (i.e. the photon number on this PMT)
	    //  - except -1 would indicate dark noise
	    photon_unique_id = ip + absoluteindex;
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
	    //Charge integration is over.  The is now a DigitizerDeadTime ns dead
	    //time period where no hits can be recorded
	    MakeDigit = true;
	  }
	  
	  //Make digit here
	  if(MakeDigit) {
	    int iflag;
	    WCSimWCDigitizerWCTE::Threshold(peSmeared,iflag);


	    //Check if previous hit passed the threshold.  If so we will digitize the hit
	    if(iflag == 0) {
	      //apply time smearing
	      double Q = (peSmeared > 0.5) ? peSmeared : 0.5;
	      //digitize hit
	      peSmeared *= efficiency;

	      //******************************************************************************************************** TD 2019.07.16 : to take PMT saturation effect into account *********************
	      //file << peSmeared << " ";
	      peSmeared *= PMT->SaturFactor(peSmeared, saturThreshold);
	      //file << peSmeared << "\n";
	      //*****************************************************************************************************************************************************************************************

	      bool accepted = WCSimWCDigitizerBase::AddNewDigit(tube, digi_unique_id,
								intgr_start + PMT->HitTimeSmearing(Q, 1.),
								peSmeared, digi_comp);
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
	  if(time > upperlimit && time <= upperlimit + DigitizerDeadTime) {
	    //Now we need to reject hits that are after the integration
	    //period to the end of the veto signal
	    continue;
	  }
	  else if(time > upperlimit + DigitizerDeadTime){
#ifdef WCSIMWCDIGITIZER_VERBOSE
	    if(tube < NPMTS_VERBOSE || tube == VERBOSE_PMT)
	      G4cout<<"*** PREPARING FOR >1 DIGI ***"<<G4endl;
#endif
	    //we now need to start integrating from the hit
	    intgr_start=time;
	    peSmeared = pe;
	    //Set the limits of the integration window [intgr_start,upperlimit]
	    upperlimit = intgr_start + DigitizerIntegrationWindow;

	    //store the digi composition information
	    photon_unique_id = ip+absoluteindex;
            digi_comp.push_back(photon_unique_id);

	    //if this is the last hit we must handle the creation of the digit 
	    //as the loop will not evaluate again
	    if(ip+1 == (*WCHCPMT)[i]->GetTotalPe()) {
	      int iflag;
	      WCSimWCDigitizerWCTE::Threshold(peSmeared,iflag);
	      if(iflag == 0) {
		//apply time smearing
		double Q = (peSmeared > 0.5) ? peSmeared : 0.5;
		//digitize hit
		peSmeared *= efficiency;
		bool accepted = WCSimWCDigitizerBase::AddNewDigit(tube, digi_unique_id,
								  intgr_start + PMT->HitTimeSmearing(Q, 1.0),
								  peSmeared, digi_comp);
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
#ifdef WCSIMWCDIGITIZER_VERBOSE
  G4cout<<"WCSimWCDigitizerWCTE::DigitizeHits END DigiStore->entries() " << DigiStore->entries() << " digits" << G4endl;
#endif
  //******************************************************************************************************** TD 2019.07.16 : to take PMT saturation effect into account *********************
  //file.close();
  //*****************************************************************************************************************************************************************************************

#ifdef WCSIMWCDIGITIZER_VERBOSE
  if(NPMTS_VERBOSE > 0 || VERBOSE_PMT > 0)
    G4cout<<"\n\n\nCHECK DIGI COMP:"<<G4endl;
  for (G4int idigi = 0 ; idigi < DigiStore->entries() ; idigi++){
    int tubeid = (*DigiStore)[idigi]->GetTubeID();
    if(tubeid < NPMTS_VERBOSE || tubeid == VERBOSE_PMT) {
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