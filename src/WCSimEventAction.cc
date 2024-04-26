#include "WCSimEventAction.hh"
#include "WCSimTrajectory.hh"
#include "WCSimRunAction.hh"
#include "WCSimPrimaryGeneratorAction.hh"
#include "WCSimWCHit.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCDigitizer.hh"
#include "WCSimWCTrigger.hh"
#include "WCSimWCAddDarkNoise.hh"
#include "WCSimWCPMT.hh"
#include "WCSimDetectorConstruction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4UImanager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4UnitsTable.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <set>
#include <iomanip>
#include <string>
#include <vector>

#include "jhfNtuple.h"
#include "TTree.h"
#include "TFile.h"
#include "WCSimRootEvent.hh"
#include "TStopwatch.h"

#ifndef _SAVE_RAW_HITS
#define _SAVE_RAW_HITS
//Use this and one/two of below to debug hit information
//#define _SAVE_RAW_HITS_VERBOSE
#endif

//Use this and one/two of below to debug digit information
//#define SAVE_DIGITS_VERBOSE

//Print out hits with PMT IDs up to N
#define NPMTS_VERBOSE -1
//And/Or a specific PMT ID
#define VERBOSE_PMT -1

#ifndef TIME_DAQ_STEPS
//#define TIME_DAQ_STEPS
#endif

//#define DEBUG

WCSimEventAction::WCSimEventAction(const WCSimRunAction* myRun,
				   const WCSimDetectorConstruction* myDetector,
				   const WCSimPrimaryGeneratorAction* myGenerator)
  :runAction(myRun), generatorAction(myGenerator),
   detectorConstructor(myDetector),
   ConstructedDAQClasses(false),
   SavedOptions(false),
   fEvNum(0)
{
  DAQMessenger = new WCSimWCDAQMessenger(this);

  G4DigiManager* DMman = G4DigiManager::GetDMpointer();

  //create PMT response module
  WCSimWCPMT* WCDMPMT = new WCSimWCPMT( "WCReadoutPMT", myDetector, "tank");
  DMman->AddNewModule(WCDMPMT);

  randGen = new TRandom3();

  //create dark noise module
  WCSimWCAddDarkNoise* WCDNM = new WCSimWCAddDarkNoise("WCDarkNoise", detectorConstructor,"tank");
  DMman->AddNewModule(WCDNM);

  WCSimWCPMT* WCDMPMT2;
  WCSimWCAddDarkNoise* WCDNM2;
#ifdef DEBUG
  G4cout<<"Debug B.Q: hybrid PMT = "<<myDetector->GetHybridPMT()<<", hybrid geom, so construct digitizer for 2 PMT types"<<G4endl;
#endif
  //if(myDetector->GetHybridPMT()){
    WCDMPMT2 = new WCSimWCPMT( "WCReadoutPMT2", myDetector,"tankPMT2");
    DMman->AddNewModule(WCDMPMT2);

    WCDNM2 = new WCSimWCAddDarkNoise("WCDarkNoise2", detectorConstructor,"tankPMT2");
    DMman->AddNewModule(WCDNM2);
    //}

  ////////////////////
  ///// -- OD -- /////
  ////////////////////
  WCSimWCPMT* WCDMPMT_OD;
  WCDMPMT_OD = new WCSimWCPMT( "WCReadoutPMT_OD", myDetector, "OD");
  DMman->AddNewModule(WCDMPMT_OD);

  WCSimWCAddDarkNoise* WCDNM_OD;
  WCDNM_OD = new WCSimWCAddDarkNoise("WCDarkNoise_OD", detectorConstructor, "OD");
  DMman->AddNewModule(WCDNM_OD);
}

WCSimEventAction::~WCSimEventAction()
{
  delete DAQMessenger;
}

void WCSimEventAction::CreateDAQInstances()
{
  if(ConstructedDAQClasses) {
    G4cerr << "WCSimEventAction::CreateDAQInstances() has already been called. Exiting..." << G4endl;
    exit(-1);
  }

  G4cout << "Creating digitizer and trigger class instances in WCSimEventAction::CreateDAQInstances()" << G4endl;

  G4DigiManager* DMman = G4DigiManager::GetDMpointer();

  //create your choice of digitizer module
  if(DigitizerChoice == "SKI") {
    WCSimWCDigitizerSKI* WCDM = new WCSimWCDigitizerSKI("WCReadoutDigits", detectorConstructor, DAQMessenger, "tank");
    DMman->AddNewModule(WCDM);
  }
  else {
    G4cerr << "Unknown DigitizerChoice " << DigitizerChoice << G4endl;
    exit(-1);
  }

  //create your choice of trigger module
  if(TriggerChoice == "NDigits") {
    WCSimWCTriggerNDigits* WCTM = new WCSimWCTriggerNDigits("WCReadout", detectorConstructor, DAQMessenger, "tank");
    DMman->AddNewModule(WCTM);
  }
  else if(TriggerChoice == "NDigits2") {
    WCSimWCTriggerNDigits2* WCTM = new WCSimWCTriggerNDigits2("WCReadout", detectorConstructor, DAQMessenger, "tank");
    DMman->AddNewModule(WCTM);
  }
  else if(TriggerChoice == "NoTrigger") {
    WCSimWCTriggerNoTrigger* WCTM = new WCSimWCTriggerNoTrigger("WCReadout", detectorConstructor, DAQMessenger, "tank");
    DMman->AddNewModule(WCTM);
  }
  else {
    G4cerr << "Unknown TriggerChoice " << TriggerChoice << G4endl;
    exit(-1);
  }

  //B.Q: For the second type of PMT in the hybrid configuration
    //create your choice of digitizer module
  if(DigitizerChoice == "SKI") {
    WCSimWCDigitizerSKI* WCDM2;
    if(detectorConstructor->GetHybridPMT()){
      WCDM2 = new WCSimWCDigitizerSKI("WCReadoutDigits2", detectorConstructor, DAQMessenger,"tankPMT2");
      DMman->AddNewModule(WCDM2);
    }
  }
  else {
    G4cerr << "Unknown DigitizerChoice " << DigitizerChoice << G4endl;
    exit(-1);
  }

  //create your choice of trigger module
  if(TriggerChoice == "NDigits") {
    WCSimWCTriggerNDigits* WCTM2;
    if(detectorConstructor->GetHybridPMT()){
      WCTM2 = new WCSimWCTriggerNDigits("WCReadout2", detectorConstructor, DAQMessenger,"tankPMT2");
      DMman->AddNewModule(WCTM2);
    }
  }
  else if(TriggerChoice == "NDigits2") {
    WCSimWCTriggerNDigits2* WCTM2;
    if(detectorConstructor->GetHybridPMT()){
      WCTM2 = new WCSimWCTriggerNDigits2("WCReadout2", detectorConstructor, DAQMessenger,"tankPMT2");
      DMman->AddNewModule(WCTM2);
    }
  }
  else if(TriggerChoice == "NoTrigger") {
    WCSimWCTriggerNoTrigger* WCTM2;
    if(detectorConstructor->GetHybridPMT()){
      WCTM2 = new WCSimWCTriggerNoTrigger("WCReadout2", detectorConstructor, DAQMessenger, "tankPMT2");
      DMman->AddNewModule(WCTM2);
    }
  }
  else {
    G4cerr << "Unknown TriggerChoice " << TriggerChoice << G4endl;
    exit(-1);
  }

  ////////////////////
  ///// -- OD -- /////
  ////////////////////
  if(detectorConstructor->GetIsODConstructed()) {
    if (DigitizerChoice == "SKI") {
      WCSimWCDigitizerSKI
          *WCDM_OD = new WCSimWCDigitizerSKI("WCReadoutDigits_OD", detectorConstructor, DAQMessenger, "OD");
      DMman->AddNewModule(WCDM_OD);
    }

    //create your choice of trigger module
    if (TriggerChoice == "NDigits") {
      WCSimWCTriggerNDigits
          *WCTM_OD = new WCSimWCTriggerNDigits("WCReadout_OD", detectorConstructor, DAQMessenger, "OD");
      DMman->AddNewModule(WCTM_OD);
    } else if (TriggerChoice == "NDigits2") {
      WCSimWCTriggerNDigits2
          *WCTM_OD = new WCSimWCTriggerNDigits2("WCReadout_OD", detectorConstructor, DAQMessenger, "OD");
      DMman->AddNewModule(WCTM_OD);
    } else if (TriggerChoice == "NoTrigger") {
      WCSimWCTriggerNoTrigger
          *WCTM_OD = new WCSimWCTriggerNoTrigger("WCReadout_OD", detectorConstructor, DAQMessenger, "OD");
      DMman->AddNewModule(WCTM_OD);
    } else {
      G4cerr << "Unknown TriggerChoice " << TriggerChoice << G4endl;
      exit(-1);
    }
  }

  ConstructedDAQClasses = true;
}


void WCSimEventAction::BeginOfEventAction(const G4Event*)
{
  if(!ConstructedDAQClasses) {
    CreateDAQInstances();

    //and save options in output file
    //G4DigiManager* DMman = G4DigiManager::GetDMpointer();

  }
  G4cout << "Starting event " << fEvNum << G4endl;
  fEvNum++;
}

void WCSimEventAction::EndOfEventAction(const G4Event* evt)
{

  if(evt->IsAborted() || evt->GetEventID() < 0){
      return;
  }	
  // ----------------------------------------------------------------------
  //  Get Particle Table
  // ----------------------------------------------------------------------

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  // ----------------------------------------------------------------------
  //  Get Trajectory Container
  // ----------------------------------------------------------------------

  G4TrajectoryContainer* trajectoryContainer = evt->GetTrajectoryContainer();

  G4int n_trajectories = 0;
  if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

  // ----------------------------------------------------------------------
  //  Get Event Information
  // ----------------------------------------------------------------------

  G4int         event_id = evt->GetEventID();
  //G4int         mode     = generatorAction->GetMode();
  //InteractionType_t mode     = generatorAction->GetMode();

  unsigned int      nvtxs   = generatorAction->GetNvtxs();
  G4ThreeVector vtxs[MAX_N_VERTICES];
  G4int         vtxsvol[MAX_N_VERTICES];
  G4double      vtxTimes[MAX_N_VERTICES];
  for( unsigned int u=0; u<nvtxs; u++ ){
    vtxs[u]      = generatorAction->GetVtx(u);
    vtxsvol[u]   = WCSimEventFindStartingVolume(vtxs[u]);
    vtxTimes[u]  = generatorAction->GetVertexTime(u);
  }
  G4int         vecRecNumber = generatorAction->GetVecRecNumber();
  G4double      timeUnit=generatorAction->GetTimeUnit();

  // ----------------------------------------------------------------------
  //  Get WC Hit Collection
  // ----------------------------------------------------------------------

  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  // Get Hit collection of this event
  G4HCofThisEvent* HCE         = evt->GetHCofThisEvent();
  WCSimWCHitsCollection* WCHC = 0;
  G4String WCIDCollectionName = detectorConstructor->GetIDCollectionName();
#ifdef DEBUG
  G4cout << "Load the first PMT type hits" << G4endl;
#endif
  if (HCE)
  {
    G4String name =   WCIDCollectionName;
    G4int collectionID = SDman->GetCollectionID(name);
    if(collectionID>-1) WCHC = (WCSimWCHitsCollection*)HCE->GetHC(collectionID);
    G4cout << "WCSimEventAction::EndOfEventAction ☆ (WCSimWCHitsCollection*)" << WCIDCollectionName
           << " has " << WCHC->entries() << " entries (hit PMTs)" << G4endl;
  }

  //B.Q for the hybrid version
  WCSimWCHitsCollection* WCHC2 = 0;
  G4String WCIDCollectionName2;
  if(detectorConstructor->GetHybridPMT()) {
    WCIDCollectionName2 = detectorConstructor->GetIDCollectionName2();
    G4cout << "Load the second PMT type hits" << G4endl;
  }

  if (HCE)
    {
      G4String name;
      G4int collectionID;
      if(detectorConstructor->GetHybridPMT()){
	name =   WCIDCollectionName2;
	collectionID = SDman->GetCollectionID(name);
	WCHC2 = (WCSimWCHitsCollection*)HCE->GetHC(collectionID);
      }
    }

  // If this option is chosen
  // pe's will be generated on the pmts.
  if( generatorAction->IsUsingPoissonPMT() ){

    // Loop through PMTs in detector
    for (
	 std::vector<WCSimPmtInfo*>::const_iterator pmtIt = detectorConstructor->Get_Pmts()->cbegin();
	 pmtIt != detectorConstructor->Get_Pmts()->cend();
	 pmtIt++ ){

      // Generate number of PEs acording to Poisson with macro defined mean
      int nPoisson = randGen->Poisson(generatorAction->GetPoissonPMTMean());

      // Check if PMTs have existing hits
      bool hitExists = false;
      int hitIndex = -1;
      for (size_t existingHit = 0; existingHit < WCHC->GetSize(); existingHit++){

	if( (*WCHC)[existingHit]->GetTubeID() == (*pmtIt)->Get_tubeid() ){

	  hitExists = true;
	  hitIndex = existingHit;

	  G4cout << "/mygen/pmtPoisson Error: Hits found when placing dummy PEs. Use lower energy primary particles or set '/mygen/pmtPoisson' to false for a real run." << G4endl;

	  if ((*WCHC)[existingHit]->GetTotalPe() > nPoisson){
	    G4cout << "/mygen/pmtPoisson Error: More PEs in original hit than in the Poisson throw. Poisson distribution will be distorted. Use lower energy primary particles or set '/mygen/pmtPoisson' to false for a real run." << G4endl;

	  } else {
	    // Take into account already exising PEs... this is not ideal as they might not be in time.
	    nPoisson -= (*WCHC)[existingHit]->GetTotalPe();
	  }
	}
	if (hitExists) break;
      }

      if (! hitExists){
	WCHC->insert((WCSimWCHit*) new WCSimWCHit() );
	hitIndex = WCHC->GetSize()-1;

      }

      const G4ThreeVector &pos = detectorConstructor->GetTubeTransform((*pmtIt)->Get_tubeid()).getTranslation();
      (*WCHC)[hitIndex]->SetTubeID((*pmtIt)->Get_tubeid());
      (*WCHC)[hitIndex]->SetEdep(0.);
      (*WCHC)[hitIndex]->SetPos(pos);
      (*WCHC)[hitIndex]->SetRot(detectorConstructor->GetTubeTransform((*pmtIt)->Get_tubeid()).getRotation());

      // Ignore logical volume for now...
      for (int pe = 0; pe < nPoisson; pe++) {
	G4float time = G4RandGauss::shoot(0.0,10.);
	G4ThreeVector dir(0, 0, 0);
  ProcessType_t photcreatorproc = kUnknownProcess;
	(*WCHC)[hitIndex]->AddPe(time);
  (*WCHC)[hitIndex]->AddTrackID(0);
	(*WCHC)[hitIndex]->AddParentID(0); // Make parent a geantino (whatever that is)
	(*WCHC)[hitIndex]->AddPhotonStartPos(pos);
	(*WCHC)[hitIndex]->AddPhotonEndPos(pos);
	(*WCHC)[hitIndex]->AddPhotonStartDir(dir);
	(*WCHC)[hitIndex]->AddPhotonEndDir(dir);
  (*WCHC)[hitIndex]->AddPhotonCreatorProcess(photcreatorproc);
	(*WCHC)[hitIndex]->AddPhotonStartTime(time);
      }

      G4cout << "The option using pmtPoisson is not implemented for the hybrid version yet." << G4endl;
    }
  }//IsUsingPoissonPMT



  // To use Do like This:
  // --------------------
  //   if (WCHC)
  //     for (G4int i=0; i< WCHC->entries() ;i++)
  //       G4cout << (*WCHC)[i]->GetTotalPe() << G4endl;


  // ----------------------------------------------------------------------
  //  Get Digitized Hit Collection
  // ----------------------------------------------------------------------
#ifdef SAVE_DIGITS_VERBOSE
   G4cout<< "WCSimEventAction::EndOfEventAction() (WCSimWCHitsCollection*)" << WCIDCollectionName
	 << " has " << WCHC->entries() << " entries" << G4endl;
   G4cout << G4endl;
#endif

  // Get a pointer to the Digitizing Module Manager
  G4DigiManager* DMman = G4DigiManager::GetDMpointer();

  // Get a pointer to the WC PMT module
  WCSimWCPMT* WCDMPMT =
    (WCSimWCPMT*)DMman->FindDigitizerModule("WCReadoutPMT");

  // new MFechner, aug 2006
  // need to clear up the old info inside PMT
  WCDMPMT->ReInitialize();


#ifdef TIME_DAQ_STEPS
  TStopwatch* ms = new TStopwatch();
  ms->Start();
#endif

#ifdef DEBUG
  G4cout << "Convert separated hits in one PMT to a single hit for first PMT type" << G4endl;
#endif
  //Convert the hits to PMT pulse
  WCDMPMT->SetRelativeDigitizedHitTime(RelativeHitTime);
  WCDMPMT->Digitize();

  //
  // Do the Dark Noise, then Digitization, then Trigger
  //

  //
  // First, add Dark noise hits before digitizing

  //Get a pointer to the WC Dark Noise Module
  WCSimWCAddDarkNoise* WCDNM =
    (WCSimWCAddDarkNoise*)DMman->FindDigitizerModule("WCDarkNoise");

#ifdef DEBUG
  G4cout << "Add Dark Hits first PMT" << G4endl;
#endif

  //Add the dark noise
  WCDNM->AddDarkNoise();

  //
  // Next, do the digitization

  //Get a pointer to the WC Digitizer Module
  WCSimWCDigitizerBase* WCDM =
    (WCSimWCDigitizerBase*)DMman->FindDigitizerModule("WCReadoutDigits");

#ifdef DEBUG
  G4cout << "Digitize hits first PMT" << G4endl;
#endif
  //Digitize the hits
  WCDM->Digitize();

  //
  // Finally, apply the trigger
#ifdef DEBUG
  G4cout << "Trigger for first PMT type" << G4endl;
#endif

  //Get a pointer to the WC Trigger Module
  WCSimWCTriggerBase* WCTM =
    (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout");

  //tell it the dark noise rate (for calculating the average dark occupancy -> can adjust the NDigits threshold)
  WCTM->SetDarkRate(WCDNM->GetDarkRate());
#ifdef DEBUG
  G4cout << "B.Q" << G4endl;
#endif

  //Apply the trigger
  // This takes the digits, and places them into trigger gates
  // Also throws away digits not contained in an trigger gate
  WCTM->Digitize();
#ifdef DEBUG
  G4cout << "Trigger for first PMT type is over" << G4endl;
#endif

#ifdef TIME_DAQ_STEPS
  ms->Stop();
  G4cout << " Digtization :  Real = " << ms->RealTime()
	 << " ; CPU = " << ms->CpuTime() << "\n";
#endif

   // Get the post-noise hit collection for the WC
   G4int WCDChitsID = DMman->GetDigiCollectionID("WCRawPMTSignalCollection");
   WCSimWCDigitsCollection * WCDC_hits = (WCSimWCDigitsCollection*) DMman->GetDigiCollection(WCDChitsID);

   // Get the digitized collection for the WC
   G4int WCDCID = DMman->GetDigiCollectionID("WCDigitizedCollection");
   WCSimWCTriggeredDigitsCollection * WCDC = (WCSimWCTriggeredDigitsCollection*) DMman->GetDigiCollection(WCDCID);
   /*
   // To use Do like This:
   // --------------------
   if(WCDC)
     for (G4int i=0; i < WCDC->entries(); i++)
       {
	 G4int   tubeID         = (*WCDC)[i]->GetTubeID();
	 G4double photoElectrons = (*WCDC)[i]->GetPe(i);
	 G4double time           = (*WCDC)[i]->GetTime(i);
	 //	 G4cout << "time " << i << " " <<time << G4endl;
	 //	 G4cout << "tubeID " << i << " " <<tubeID << G4endl;
	 //	 G4cout << "Pe " << i << " " <<photoElectrons << G4endl;
	 //   (*WCDC)[i]->Print();
       }
   */

  // ----------------------------------------------------------------------
  //  Digitization for the second PMT type in the hybrid configuration, B.Q
  // ----------------------------------------------------------------------
     // Get a pointer to the WC PMT module
     WCSimWCPMT* WCDMPMT2;
     //Get a pointer to the WC Dark Noise Module
     WCSimWCAddDarkNoise* WCDNM2 = nullptr;
     //Get a pointer to the WC Digitizer Module
     WCSimWCDigitizerBase* WCDM2;
     //Get a pointer to the WC Trigger Module
     WCSimWCTriggerBase* WCTM2;
     // Get the post-noise hit collection for the WC
     G4int WCDChitsID2;
     WCSimWCDigitsCollection * WCDC_hits2 = nullptr;
     // Get the digitized collection for the WC
     G4int WCDCID2;

#ifdef DEBUG
     G4cout<< "Debug B.Q: Entering the PMT2 type readout, hybrid = " << detectorConstructor->GetHybridPMT() << ", PMT list = " << detectorConstructor->GetIDCollectionName() << G4endl;
#endif
     WCSimWCTriggeredDigitsCollection * WCDC2 = nullptr;

     if(detectorConstructor->GetHybridPMT()){
       G4cout<< "Debug B.Q: Entering the PMT2 type readout, hybrid = " << detectorConstructor->GetHybridPMT() << G4endl;

       G4cout<< "WCSimEventAction::EndOfEventAction() (WCSimWCHitsCollection*)" << WCIDCollectionName2
	     << " has " << WCHC2->entries() << " entries" << G4endl;
       G4cout << G4endl;

       // Get a pointer to the WC PMT module
       WCDMPMT2 =
	 (WCSimWCPMT*)DMman->FindDigitizerModule("WCReadoutPMT2");

#ifdef DEBUG
       G4cout<<"Debug B.Q: tmp"<<G4endl;
#endif

       // new MFechner, aug 2006
       // need to clear up the old info inside PMT
      WCDMPMT2->ReInitialize();


#ifdef TIME_DAQ_STEPS
     TStopwatch* ms = new TStopwatch();
     ms->Start();
#endif

#ifdef DEBUG
     G4cout<< "Debug B.Q: Digitization in a PMT: photons are gathered" << G4endl;
#endif

     //Convert the hits to PMT pulse
     WCDMPMT2->SetRelativeDigitizedHitTime(RelativeHitTime, WCDMPMT->GetRelativeTimeShift() );
     WCDMPMT2->Digitize();

     //
     // Do the Dark Noise, then Digitization, then Trigger
     //

     //
     // First, add Dark noise hits before digitizing

#ifdef DEBUG
     G4cout<< "Debug B.Q: Add DN " << G4endl;
#endif

     //Get a pointer to the WC Dark Noise Module
     WCDNM2 =
       (WCSimWCAddDarkNoise*)DMman->FindDigitizerModule("WCDarkNoise2");

     //Add the dark noise
     WCDNM2->AddDarkNoise();

     //
     // Next, do the digitization

     //Get a pointer to the WC Digitizer Module
     WCDM2 =
       (WCSimWCDigitizerBase*)DMman->FindDigitizerModule("WCReadoutDigits2");

#ifdef DEBUG
     G4cout<< "Debug B.Q: Digitization by electronics" << G4endl;
#endif
     //Digitize the hits
     WCDM2->Digitize();

     //
     // Finally, apply the trigger

     //Get a pointer to the WC Trigger Module
     WCTM2 =
       (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout2");

     //tell it the dark noise rate (for calculating the average dark occupancy -> can adjust the NDigits threshold)
     WCTM2->SetDarkRate(WCDNM2->GetDarkRate());

#ifdef DEBUG
     G4cout<< "Debug B.Q: Trigger" << G4endl;
#endif

     //Apply the trigger
     // This takes the digits, and places them into trigger gates
     // Also throws away digits not contained in an trigger gate
     WCTM2->Digitize();

#ifdef TIME_DAQ_STEPS
     ms->Stop();
     G4cout << " Digtization :  Real = " << ms->RealTime()
	    << " ; CPU = " << ms->CpuTime() << "\n";
#endif

     // Get the post-noise hit collection for the WC
     WCDChitsID2 = DMman->GetDigiCollectionID("WCRawPMTSignalCollection2");
     WCDC_hits2 = (WCSimWCDigitsCollection*) DMman->GetDigiCollection(WCDChitsID2);

     // Get the digitized collection for the WC
     WCDCID2 = DMman->GetDigiCollectionID("WCDigitizedCollection2");
     WCDC2 = (WCSimWCTriggeredDigitsCollection*) DMman->GetDigiCollection(WCDCID2);
   }//End of hybrid

  // Repeat the steps for the OD
  // ----------------------------------------------------------------------

  WCSimWCHitsCollection* WCHC_OD=NULL;
  WCSimWCPMT* WCDMPMT_OD=NULL;
  WCSimWCAddDarkNoise* WCDNM_OD=NULL;
  WCSimWCDigitizerBase* WCDM_OD=NULL;
  WCSimWCTriggerBase* WCTM_OD=NULL;
  G4int WCDChitsID_OD;
  WCSimWCDigitsCollection* WCDC_hits_OD=NULL;
  WCSimWCTriggeredDigitsCollection *WCDC_OD=NULL;
  if(detectorConstructor->GetIsODConstructed()){
    WCHC_OD = 0;
    G4String WCODCollectionName = detectorConstructor->GetODCollectionName();
    if(HCE){
      G4String name = WCODCollectionName;
      G4int collectionID = SDman->GetCollectionID(name);
      if(collectionID>-1) WCHC_OD = (WCSimWCHitsCollection*)HCE->GetHC(collectionID);
      G4cout << G4endl;
      G4cout<< "WCSimEventAction::EndOfEventAction() (WCSimWCHitsCollection*)" << WCODCollectionName
            << " has " << WCHC_OD->entries() << " entries" << G4endl;
      G4cout << G4endl;
    }

    WCDMPMT_OD = (WCSimWCPMT*)DMman->FindDigitizerModule("WCReadoutPMT_OD");
    if(WCDMPMT_OD == 0) G4cout << "WCReadoutPMT_OD digitzer module not found!" << G4endl;
    WCDMPMT_OD->ReInitialize();
    WCDMPMT_OD->SetRelativeDigitizedHitTime(RelativeHitTime);
    WCDMPMT_OD->Digitize();

    WCDNM_OD = (WCSimWCAddDarkNoise*)DMman->FindDigitizerModule("WCDarkNoise_OD");
    if(WCDNM_OD == 0) G4cout << "WCDarkNoise_OD dark noise module not found!" << G4endl;
    WCDNM_OD->AddDarkNoise();

    WCDM_OD = (WCSimWCDigitizerBase*)DMman->FindDigitizerModule("WCReadoutDigits_OD");
    if(WCDM_OD == 0) G4cout << "WCReadoutDigits_OD digitizer module not found!" << G4endl;
    WCDM_OD->Digitize();

    WCTM_OD = (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout_OD");
    if(WCTM_OD == 0) G4cout << "WCReadout_OD trigger module not found!" << G4endl;
    WCTM_OD->SetDarkRate(WCDNM_OD->GetDarkRate());
    WCTM_OD->Digitize();

    WCDChitsID_OD = DMman->GetDigiCollectionID("WCRawPMTSignalCollection_OD");
    WCDC_hits_OD = (WCSimWCDigitsCollection*) DMman->GetDigiCollection(WCDChitsID_OD);
    // printouts
    G4cout << "WCSimEventAction::EndOfEventAction() retrieving raw hits" << G4endl
           << " (WCSimWCDigitsCollection*)WCRawPMTSignalCollection_OD for FillRootEvent, which has ";
    if(WCDC_hits_OD){
      G4cout << WCDC_hits_OD->entries();
    } else {
      G4cout << "no";
    }
    G4cout << " entries" << G4endl;

    G4int WCDCID_OD = DMman->GetDigiCollectionID("WCDigitizedCollection_OD");
    WCDC_OD = (WCSimWCTriggeredDigitsCollection*) DMman->GetDigiCollection(WCDCID_OD);
    // printouts
    G4cout << "WCSimEventAction::EndOfEventAction() retrieving readout hits"
           << " (WCSimWCTriggeredDigitsCollection*)WCDigitizedCollection_OD for FillRootEvent, which has ";
    if(WCDC_hits_OD){
      G4cout << WCDC_OD->entries();
    } else {
      G4cout << "no";
    }
    G4cout << " entries" << G4endl;
  }

  // ----------------------------------------------------------------------
  //  Fill Ntuple
  // ----------------------------------------------------------------------
   for(unsigned int ivx = 0;ivx<nvtxs;ivx++)
     jhfNtuple.mode[ivx]   = generatorAction->GetMode(ivx);         // interaction mode
   jhfNtuple.nvtxs = nvtxs;       // number of vertices
   for( unsigned int u=0; u<nvtxs; u++ ){
     jhfNtuple.vtxsvol[u] = vtxsvol[u];       // volume of vertex
     // unit mismatch between geant4 and reconstruction, M Fechner
     jhfNtuple.vtxs[u][0] = vtxs[u][0]/cm; // interaction vertex
     jhfNtuple.vtxs[u][1] = vtxs[u][1]/cm; // interaction vertex
     jhfNtuple.vtxs[u][2] = vtxs[u][2]/cm; // interaction vertex
     jhfNtuple.vtxs[u][3] = vtxTimes[u]/timeUnit; // interaction vertex
   }
   jhfNtuple.vecRecNumber = vecRecNumber; //vectorfile record number

   // mustop, pstop, npar will be filled later

   // Next in the ntuple is an array of tracks.
   // We will keep count with npar

   G4int npar = 0;

   // First two tracks are special: beam and target
   for( unsigned int u=0; u<nvtxs; u++ ){
     /////////////////////////////////
     // npar = 0        NEUTRINO /////
     /////////////////////////////////

     G4int         beampdg;
     G4double      beamenergy;
     G4ThreeVector beamdir;

     beampdg    = generatorAction->GetBeamPDG(u);
     beamenergy = generatorAction->GetBeamEnergy(u);
     beamdir    = generatorAction->GetBeamDir(u);

     jhfNtuple.ipnu[npar]    = beampdg;               // id
     jhfNtuple.flag[npar]    = -1;                    // incoming neutrino
     jhfNtuple.m[npar]       = 0.0;                   // mass (always a neutrino)
     jhfNtuple.p[npar]       = beamenergy;            // momentum magnitude
     jhfNtuple.E[npar]       = beamenergy;            // energy
     jhfNtuple.startvol[npar]= vtxsvol[u];                    // starting volume, vtxvol should be referred
     jhfNtuple.stopvol[npar] = -1;                    // stopping volume
     jhfNtuple.dir[npar][0]  = beamdir[0];            // direction
     jhfNtuple.dir[npar][1]  = beamdir[1];            // direction
     jhfNtuple.dir[npar][2]  = beamdir[2];            // direction
     jhfNtuple.pdir[npar][0] = beamenergy*beamdir[0]; // momentum-vector
     jhfNtuple.pdir[npar][1] = beamenergy*beamdir[1]; // momentum-vector
     jhfNtuple.pdir[npar][2] = beamenergy*beamdir[2]; // momentum-vector
     // M Fechner, same as above
     jhfNtuple.stop[npar][0] = vtxs[u][0]/cm;  // stopping point (not meaningful)
     jhfNtuple.stop[npar][1] = vtxs[u][1]/cm;  // stopping point (not meaningful)
     jhfNtuple.stop[npar][2] = vtxs[u][2]/cm;  // stopping point (not meaningful)
     /* Alex Finch
	Create an imaginary start position for the incoming neutrino, to help event display
      */
     double distance=10000.0;
     for(int idim=0;idim<3;idim++)
       jhfNtuple.start[npar][idim]=jhfNtuple.stop[npar][idim] - (distance*jhfNtuple.dir[npar][idim]);

     jhfNtuple.parent[npar] = 0;

     npar++;
     /////////////////////////////////
     // npar = 1        TARGET ///////
     /////////////////////////////////

     G4double      targetpmag = 0.0, targetmass = 0.0;
     G4int         targetpdg    = generatorAction->GetTargetPDG(u);
     G4double      targetenergy = generatorAction->GetTargetEnergy(u);
     G4ThreeVector targetdir    = generatorAction->GetTargetDir(u);

     if (targetpdg!=0) {            // protects against seg-fault
       if (targetpdg > 999)         // 16O nucleus not in pdg table
	 targetmass = targetenergy; // 16O is at rest, so E = m
       else
	 targetmass = particleTable->FindParticle(targetpdg)->GetPDGMass();
       if (targetenergy > targetmass)
	 //      targetpmag = sqrt(targetenergy*targetenergy - targetmass*targetenergy);
	 // MF : bug fix
	 targetpmag = sqrt(targetenergy*targetenergy - targetmass*targetmass);
       else // protect against NaN
	 targetpmag = 0.0;
     }

     jhfNtuple.ipnu[npar]     = targetpdg;    // id
     jhfNtuple.flag[npar]    = -2;            // target
     jhfNtuple.m[npar]       = targetmass;    // mass (always a neutrino)
     jhfNtuple.p[npar]       = targetpmag;    // momentum magnitude
     jhfNtuple.E[npar]       = targetenergy;  // energy (total!)
     jhfNtuple.startvol[npar] = vtxsvol[u];           // starting volume
     jhfNtuple.stopvol[npar] = -1;            // stopping volume
     jhfNtuple.dir[npar][0]  = targetdir[0];  // direction
     jhfNtuple.dir[npar][1]  = targetdir[1];  // direction
     jhfNtuple.dir[npar][2]  = targetdir[2];  // direction
     // MF feb9,2006 : we want the momentum, not the energy...
     //  jhfNtuple.pdir[npar][0] = targetenergy*targetdir[0];  // momentum-vector
     //  jhfNtuple.pdir[npar][1] = targetenergy*targetdir[1];  // momentum-vector
     //  jhfNtuple.pdir[npar][2] = targetenergy*targetdir[2];  // momentum-vector
     jhfNtuple.pdir[npar][0] = targetpmag*targetdir[0];  // momentum-vector
     jhfNtuple.pdir[npar][1] = targetpmag*targetdir[1];  // momentum-vector
     jhfNtuple.pdir[npar][2] = targetpmag*targetdir[2];  // momentum-vector
     // M Fechner, same as above
     jhfNtuple.stop[npar][0] = vtxs[u][0]/cm;  // stopping point (not meaningful)
     jhfNtuple.stop[npar][1] = vtxs[u][1]/cm;  // stopping point (not meaningful)
     jhfNtuple.stop[npar][2] = vtxs[u][2]/cm;  // stopping point (not meaningful)
     jhfNtuple.parent[npar] = 0;

     npar++;
  }

  ////////////////////////
  // npar > nvertices  ///
  ////////////////////////

  // Draw Charged Tracks
  G4int PDG_e=11,PDG_v_e=12,PDG_gam=22;
  for( unsigned int u=0; u<nvtxs; u++ ){
    G4int trkid_e=INT_MAX,trkid_v_e=INT_MAX,trkid_gam=INT_MAX;
    G4int idx_e=INT_MAX,idx_v_e=INT_MAX,idx_gam=INT_MAX;
    for (G4int i=0; i < n_trajectories; i++)
      {
	WCSimTrajectory* trj =
	  (WCSimTrajectory*)((*(evt->GetTrajectoryContainer()))[i]);

	if (trj->GetCharge() != 0.)
	  trj->DrawTrajectory(/*50*/);

	if(abs(trj->GetPDGEncoding()) == PDG_e && trj->GetParentID() == (int)u+1 && trj->GetTrackID() < trkid_e) {
	  trkid_e = trj->GetTrackID();
	  idx_e = i;
	}
	if(abs(trj->GetPDGEncoding()) == PDG_v_e && trj->GetParentID() == (int)u+1 && trj->GetTrackID() < trkid_v_e) {
	  trkid_v_e = trj->GetTrackID();
	  idx_v_e = i;
	}
	if(abs(trj->GetPDGEncoding()) == PDG_gam && trj->GetParentID() == (int)u+1 && trj->GetTrackID() < trkid_gam) {
	  trkid_gam = trj->GetTrackID();
	  idx_gam = i;
	}
      }

    if(idx_e != INT_MAX) {
      WCSimTrajectory* trj =
	(WCSimTrajectory*)((*(evt->GetTrajectoryContainer()))[idx_e]);
      jhfNtuple.ipnu[npar]     = trj->GetPDGEncoding();    // id
      jhfNtuple.flag[npar]    = 0;            // target
      jhfNtuple.m[npar]       = particleTable->FindParticle(trj->GetPDGEncoding())->GetPDGMass();    // mass (always a neutrino)
      jhfNtuple.p[npar]       = trj->GetInitialMomentum().mag();    // momentum magnitude
      jhfNtuple.E[npar]       = sqrt(jhfNtuple.m[npar]*jhfNtuple.m[npar]+jhfNtuple.p[npar]*jhfNtuple.p[npar]);  // energy (total!)
      jhfNtuple.startvol[npar] = -1;           // starting volume
      jhfNtuple.stopvol[npar] = -1;            // stopping volume
      jhfNtuple.dir[npar][0]  = trj->GetInitialMomentum().unit().getX();  // direction
      jhfNtuple.dir[npar][1]  = trj->GetInitialMomentum().unit().getY();  // direction
      jhfNtuple.dir[npar][2]  = trj->GetInitialMomentum().unit().getZ();  // direction
      // MF feb9,2006 : we want the momentum, not the energy...
      //  jhfNtuple.pdir[npar][0] = targetenergy*targetdir[0];  // momentum-vector
      //  jhfNtuple.pdir[npar][1] = targetenergy*targetdir[1];  // momentum-vector
      //  jhfNtuple.pdir[npar][2] = targetenergy*targetdir[2];  // momentum-vector
      jhfNtuple.pdir[npar][0] = trj->GetInitialMomentum().getX();  // momentum-vector
      jhfNtuple.pdir[npar][1] = trj->GetInitialMomentum().getY();  // momentum-vector
      jhfNtuple.pdir[npar][2] = trj->GetInitialMomentum().getZ();  // momentum-vector
      // M Fechner, same as above
      jhfNtuple.stop[npar][0] = vtxs[u][0]/cm;  // stopping point (not meaningful)
      jhfNtuple.stop[npar][1] = vtxs[u][1]/cm;  // stopping point (not meaningful)
      jhfNtuple.stop[npar][2] = vtxs[u][2]/cm;  // stopping point (not meaningful)
      jhfNtuple.parent[npar] = 0;

      npar++;
    }

    if(idx_v_e != INT_MAX) {
      WCSimTrajectory* trj =
	(WCSimTrajectory*)((*(evt->GetTrajectoryContainer()))[idx_v_e]);
      jhfNtuple.ipnu[npar]     = trj->GetPDGEncoding();    // id
      jhfNtuple.flag[npar]    = 0;            // target
      jhfNtuple.m[npar]       = particleTable->FindParticle(trj->GetPDGEncoding())->GetPDGMass();    // mass (always a neutrino)
      jhfNtuple.p[npar]       = trj->GetInitialMomentum().mag();    // momentum magnitude
      jhfNtuple.E[npar]       = sqrt(jhfNtuple.m[npar]*jhfNtuple.m[npar]+jhfNtuple.p[npar]*jhfNtuple.p[npar]);  // energy (total!)
      jhfNtuple.startvol[npar] = -1;           // starting volume
      jhfNtuple.stopvol[npar] = -1;            // stopping volume
      jhfNtuple.dir[npar][0]  = trj->GetInitialMomentum().unit().getX();  // direction
      jhfNtuple.dir[npar][1]  = trj->GetInitialMomentum().unit().getY();  // direction
      jhfNtuple.dir[npar][2]  = trj->GetInitialMomentum().unit().getZ();  // direction
      // MF feb9,2006 : we want the momentum, not the energy...
      //  jhfNtuple.pdir[npar][0] = targetenergy*targetdir[0];  // momentum-vector
      //  jhfNtuple.pdir[npar][1] = targetenergy*targetdir[1];  // momentum-vector
      //  jhfNtuple.pdir[npar][2] = targetenergy*targetdir[2];  // momentum-vector
      jhfNtuple.pdir[npar][0] = trj->GetInitialMomentum().getX();  // momentum-vector
      jhfNtuple.pdir[npar][1] = trj->GetInitialMomentum().getY();  // momentum-vector
      jhfNtuple.pdir[npar][2] = trj->GetInitialMomentum().getZ();  // momentum-vector
      // M Fechner, same as above
      jhfNtuple.stop[npar][0] = vtxs[u][0]/cm;  // stopping point (not meaningful)
      jhfNtuple.stop[npar][1] = vtxs[u][1]/cm;  // stopping point (not meaningful)
      jhfNtuple.stop[npar][2] = vtxs[u][2]/cm;  // stopping point (not meaningful)
      jhfNtuple.parent[npar] = 0;

      npar++;
    }


    if(idx_gam != INT_MAX) {
      WCSimTrajectory* trj =
	(WCSimTrajectory*)((*(evt->GetTrajectoryContainer()))[idx_gam]);
      jhfNtuple.ipnu[npar]     = trj->GetPDGEncoding();    // id
      jhfNtuple.flag[npar]    = 0;            // target
      jhfNtuple.m[npar]       = particleTable->FindParticle(trj->GetPDGEncoding())->GetPDGMass();    // mass (always a neutrino)
      jhfNtuple.p[npar]       = trj->GetInitialMomentum().mag();    // momentum magnitude
      jhfNtuple.E[npar]       = sqrt(jhfNtuple.m[npar]*jhfNtuple.m[npar]+jhfNtuple.p[npar]*jhfNtuple.p[npar]);  // energy (total!)
      jhfNtuple.startvol[npar] = -1;           // starting volume
      jhfNtuple.stopvol[npar] = -1;            // stopping volume
      jhfNtuple.dir[npar][0]  = trj->GetInitialMomentum().unit().getX();  // direction
      jhfNtuple.dir[npar][1]  = trj->GetInitialMomentum().unit().getY();  // direction
      jhfNtuple.dir[npar][2]  = trj->GetInitialMomentum().unit().getZ();  // direction
      // MF feb9,2006 : we want the momentum, not the energy...
      //  jhfNtuple.pdir[npar][0] = targetenergy*targetdir[0];  // momentum-vector
      //  jhfNtuple.pdir[npar][1] = targetenergy*targetdir[1];  // momentum-vector
      //  jhfNtuple.pdir[npar][2] = targetenergy*targetdir[2];  // momentum-vector
      jhfNtuple.pdir[npar][0] = trj->GetInitialMomentum().getX();  // momentum-vector
      jhfNtuple.pdir[npar][1] = trj->GetInitialMomentum().getY();  // momentum-vector
      jhfNtuple.pdir[npar][2] = trj->GetInitialMomentum().getZ();  // momentum-vector
      // M Fechner, same as above
      jhfNtuple.stop[npar][0] = vtxs[u][0]/cm;  // stopping point (not meaningful)
      jhfNtuple.stop[npar][1] = vtxs[u][1]/cm;  // stopping point (not meaningful)
      jhfNtuple.stop[npar][2] = vtxs[u][2]/cm;  // stopping point (not meaningful)
      jhfNtuple.parent[npar] = 0;

      npar++;
    }
  }
  jhfNtuple.npar = npar;

  //fill correct variables for track from decay

   //   G4cout << "event_id: " << &event_id << G4endl;
   // G4cout << "jhfNtuple: " << &jhfNtuple << G4endl;
   //  G4cout << "WCHC: " << &WCHC << G4endl;
   //  G4cout << "WCDC: " << &WCDC << G4endl;
   //  G4cout << "WCFVHC: " << &WCFVHC << G4endl;
   //  G4cout << "WCFVDC: " << &WCFVDC << G4endl;
   // G4cout << "lArDHC: " << &lArDHC << G4endl;
   // G4cout << "FGDxHC: " << &FGDxHC << G4endl;
   // G4cout << "FGDyHC: " << &FGDyHC << G4endl;
   // G4cout << "MRDxHC: " << &MRDxHC << G4endl;
   // G4cout << "MRDyHC: " << &MRDyHC << G4endl;

  if(GetRunAction()->GetRootFileOption()){
    WCSimRootEvent * wcsimrootsuperevent  = GetRunAction()->GetRootEvent("tank");
    WCSimRootTrigger * wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
    WCSimRootEvent * wcsimrootsuperevent2  = GetRunAction()->GetRootEvent("tankPMT2");
    WCSimRootTrigger * wcsimrootevent2 = wcsimrootsuperevent2->GetTrigger(0);

    bool skipFillingTracks = false;
    FillRootEventHybrid(event_id,
			jhfNtuple,
			trajectoryContainer,
			WCDC_hits,
			WCDC,
			"tank",wcsimrootsuperevent,wcsimrootevent,
			skipFillingTracks);
    skipFillingTracks = true;
    if(detectorConstructor->GetHybridPMT()){
      FillRootEventHybrid(event_id,
			  jhfNtuple,
			  trajectoryContainer,
			  WCDC_hits2,
			  WCDC2,
			  "tankPMT2",wcsimrootsuperevent2,wcsimrootevent2,
			  skipFillingTracks);
    }

    if(detectorConstructor->GetIsODConstructed()){
      FillRootEvent(event_id,
		    jhfNtuple,
		    trajectoryContainer,
		    WCDC_hits_OD,
		    WCDC_OD,
		    "OD",
		    skipFillingTracks);
    }

#ifdef DEBUG
    G4cout << "B.Q: open the tree" << G4endl;
#endif
    TTree* tree = GetRunAction()->GetTree();
    tree->Fill();
  }

  //save DAQ options here. This ensures that when the user selects a default option
  // (e.g. with -99), the saved option value in the output reflects what was run
  if(!SavedOptions) {
    WCSimRootOptions * wcsimopt = runAction->GetRootOptions();
    //Dark noise
    WCDNM->SaveOptionsToOutput(wcsimopt, "tank");
    //Digitizer
    WCDM->SaveOptionsToOutput(wcsimopt);
    //Trigger
    WCTM->SaveOptionsToOutput(wcsimopt);
    //Generator
    generatorAction->SaveOptionsToOutput(wcsimopt);
    //Generator for hybrid configuration, B.Q
    if(detectorConstructor->GetHybridPMT()) WCDNM2->SaveOptionsToOutput(wcsimopt,"tankPMT2");

    if(detectorConstructor->GetIsODConstructed()) {
      //Dark noise
      WCDNM_OD->SaveOptionsToOutput(wcsimopt, "OD");
    }

    SavedOptions = true;
  }
  G4cout << "End of event " << fEvNum << G4endl << G4endl;
}

G4int WCSimEventAction::WCSimEventFindStartingVolume(G4ThreeVector vtx)
{
  // Get volume of starting point (see GEANT4 FAQ)

  G4int vtxvol = -1;

  G4Navigator* tmpNavigator =
    G4TransportationManager::GetTransportationManager()->
    GetNavigatorForTracking();

  G4VPhysicalVolume* tmpVolume = tmpNavigator->LocateGlobalPointAndSetup(vtx);
  //  G4String       vtxVolumeName = tmpVolume->GetName();
  vtxVolumeName = tmpVolume->GetName();                  //TF: class member now


  if ( vtxVolumeName == "outerTube" ||
	    vtxVolumeName == "innerTube" ||
	    vtxVolumeName == "rearEndCap"||
	    vtxVolumeName == "frontEndCap" )
    vtxvol = 10;

  else if ( vtxVolumeName.contains("WC") && !vtxVolumeName.contains("FV") )
  {
// aah original line  ->if (vtxVolumeName.contains("WCBarrel"))
	  if ((vtxVolumeName.contains("WCBarrel"))|| (vtxVolumeName.contains("Tank")))	//aah I added "Tank" as MB equivalent of Barrel
      vtxvol = 10;
    else if (vtxVolumeName == "WCBox")
      vtxvol = -2;
    else if (vtxVolumeName.contains("PMT") ||
	     vtxVolumeName.contains("Cap") ||
	     vtxVolumeName.contains("Cell"))
      vtxvol = 11;
    else if (vtxVolumeName.contains("OD"))
      vtxvol = 12;
    else
    {
      G4cout << vtxVolumeName << " unkown vtxVolumeName " << G4endl;
      vtxvol = -3;
    }
  }
  else if ( vtxVolumeName == "expHall" )
    vtxvol = 0;
  else if ( vtxVolumeName == "catcher" )
    vtxvol = 40;


  return vtxvol;
}

G4int WCSimEventAction::WCSimEventFindStoppingVolume(G4String stopVolumeName)
{
  G4int stopvol = -1;

  if ( stopVolumeName.contains("WC") && !stopVolumeName.contains("FV") )
  {
//aah Original line->    if (stopVolumeName.contains("WCBarrel"))
	  if ((stopVolumeName.contains("WCBarrel"))|| (stopVolumeName.contains("Tank")))	// aah I added "Tank" as MB equivalent of Barrel
      stopvol = 10;
    else if (stopVolumeName == "WCBox")
      stopvol = -2;
    else if (stopVolumeName.contains("PMT") ||
	     stopVolumeName.contains("Cap") ||
	     stopVolumeName.contains("Cell"))
      stopvol = 11;
    else if (stopVolumeName.contains("OD"))
      stopvol = 12;
    else
    {
      G4cout << stopVolumeName << " unkown stopVolumeName " << G4endl;
      stopvol = -3;
    }
  }

  else if ( stopVolumeName.contains("FV") )
  {
    if (stopVolumeName == "WCFVBarrel" ||
	stopVolumeName == "WCFVAnnulus" ||
	stopVolumeName == "WCFVRing" )
      stopvol = 10;
    else if (stopVolumeName.contains("FVPMT"))
      stopvol = 13;
    else
    {
      G4cout << stopVolumeName << " unkown stopVolumeName " << G4endl;
      stopvol = -3;
    }
  }
  else if ( stopVolumeName == "expHall" )
    stopvol = 0;
  else if ( stopVolumeName == "catcher" )
    stopvol = 40;


  return stopvol;
}

void WCSimEventAction::FillRootEvent(G4int event_id,
				     const struct ntupleStruct& injhfNtuple,
				     G4TrajectoryContainer* TC,
				     WCSimWCDigitsCollection* WCDC_hits,
				     WCSimWCTriggeredDigitsCollection* WCDC,
				     G4String detectorElement,
				     bool skipFillingTracks)
{
  // Fill up a Root event with stuff from the ntuple

  WCSimRootEvent* wcsimrootsuperevent = GetRunAction()->GetRootEvent(detectorElement);
  wcsimrootsuperevent->ReInitialize();

  // start with the first "sub-event"
  // if the WC digitization requires it, we will add another subevent
  // for the WC.
  // all the rest goes into the first "sub-event".
  WCSimRootTrigger* wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  // get number of gates
  G4DigiManager* DMman = G4DigiManager::GetDMpointer();
  WCSimWCTriggerBase* WCTM;
  if(detectorElement=="tank"){
    WCTM = (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout");
  } else if(detectorElement=="tankPMT2"){
    WCTM = (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout2");
  } else if(detectorElement=="OD"){
    WCTM = (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout_OD");
  } else {
    G4cout << "Unknown detector element: " << detectorElement << G4endl;
    exit(1);
  }

  int ngates = WCTM->NumberOfGatesInThisEvent();
  for (int index = 0 ; index < ngates ; index++)
    {
      if (index >=1 ) {
	wcsimrootsuperevent->AddSubEvent();
	wcsimrootevent = wcsimrootsuperevent->GetTrigger(index);
	wcsimrootevent->SetHeader(event_id, GetRunAction()->GetRunID(), 0, index+1); // date & # of subevent
	wcsimrootevent->SetMode(injhfNtuple.mode[0]);
      }
      //wcsimrootevent->SetTriggerInfo(WCTM->GetTriggerType(index),
      //			     WCTM->GetTriggerInfo(index));
      //Added by B.Q to keep track of he trigger, 2019/01/17
      std::vector<G4double> triggerInfo = WCTM->GetTriggerInfo(index);
      triggerInfo.push_back(950.);
      triggerInfo.push_back(WCTM->GetTriggerTime(index));
      wcsimrootevent->SetTriggerInfo(WCTM->GetTriggerType(index),triggerInfo);//Added by B.Q to keep track of he trigger, 2019/01/17
      triggerInfo.clear();
    }


  // Fill the header
  // Need to add date
  wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  wcsimrootevent->SetHeader(event_id, GetRunAction()->GetRunID(), 0); // will be set later.

  std::map<int,int> trajMap; // mapping of trackID and index

  // Fill other info for this event

  if(skipFillingTracks) {
    wcsimrootevent->SetNvtxs(0);
#ifdef WCSIM_SAVE_PHOTON_HISTORY
    // still need the trackID mapping
    G4int n_trajectories = 0;
    if (TC)
      n_trajectories = TC->entries();
    for (int i=0; i <n_trajectories; i++)
    {
	    WCSimTrajectory* trj = (WCSimTrajectory*)(*TC)[i];
      trajMap[trj->GetTrackID()] = i;
    }
#endif  
  }
  else {
    wcsimrootevent->SetNvtxs(injhfNtuple.nvtxs);
    for( Int_t u=0; u<injhfNtuple.nvtxs; u++ ){
      wcsimrootevent->SetVtxsvol(u,injhfNtuple.vtxsvol[u]);
      for (int j=0;j<4;j++)
	{
	  wcsimrootevent->SetVtxs(u,j,injhfNtuple.vtxs[u][j]);
	}
      wcsimrootevent->SetMode(u,injhfNtuple.mode[u]);
    }
    wcsimrootevent->SetJmu(injhfNtuple.jmu);           //TF: undefined and only for Nuance
    wcsimrootevent->SetJp(injhfNtuple.jp);             //TF: undefined and only for Nuance
    wcsimrootevent->SetNpar(injhfNtuple.npar);         //TF: undefined
    wcsimrootevent->SetVecRecNumber(injhfNtuple.vecRecNumber);  //TF: undefined and only for Nuance

    // Add the tracks with the particle information
    // First two tracks come from injhfNtuple, as they are special

    for (int k=0;k<injhfNtuple.npar;k++) // should be just 2
      {
	double dir[3];
	double pdir[3];
	double stop[3];
	double start[3];
	ProcessType_t creatorP = kUnknownProcess;
	for (int l=0;l<3;l++)
	  {
	    dir[l]=injhfNtuple.dir[k][l];
	    pdir[l]=injhfNtuple.pdir[k][l];
	    stop[l]=injhfNtuple.stop[k][l];
	    start[l]=injhfNtuple.start[k][l];
	    //G4cout<< "start[" << k << "][" << l <<"]: "<< injhfNtuple.start[k][l] <<G4endl;
	    //G4cout<< "stop[" << k << "][" << l <<"]: "<< injhfNtuple.stop[k][l] <<G4endl;
	  }

	// Add the track to the TClonesArray
	wcsimrootevent->AddTrack(injhfNtuple.ipnu[k],
				 injhfNtuple.flag[k],
				 injhfNtuple.m[k],
				 injhfNtuple.p[k],
				 injhfNtuple.E[k],
				 injhfNtuple.startvol[k],
				 injhfNtuple.stopvol[k],
				 dir,
				 pdir,
				 stop,
				 start,
				 injhfNtuple.parent[k],
				 creatorP,
				 injhfNtuple.time[k],
				 0,
				 0,
				 std::vector<std::vector<float>>(),
				 std::vector<float>(),
				 std::vector<double>(),
				 std::vector<int>());
      }

    // the rest of the tracks come from WCSimTrajectory

    std::set<int> pizeroList;
    // added by M Fechner, dec 16th, 2004
    std::set<int> muonList;
    std::set<int> antimuonList;
    // same, april 7th 2005
    std::set<int> pionList;
    std::set<int> antipionList;
    std::set<int> primaryList;

    // Pi0 specific variables
    Double_t pi0Vtx[3];
    Int_t   gammaID[2];
    Double_t gammaE[2];
    Double_t gammaVtx[2][3];
    Int_t   r = 0;

    G4int n_trajectories = 0;
    if (TC)
      n_trajectories = TC->entries();

    // M Fechner : removed this limit to get to the primaries...
    //if (n_trajectories>50)  // there is no need for this limit, but it has
    //n_trajectories=50;    // existed in previous versions of the code.  It also
    // makes the ROOT file smaller.

    for (int i=0; i <n_trajectories; i++)
      {
	WCSimTrajectory* trj = (WCSimTrajectory*)(*TC)[i];

	// If this track is a pizero remember it for later
	if ( trj->GetPDGEncoding() == 111)
	  pizeroList.insert(trj->GetTrackID());
	// If it is a mu+/mu- also remember it
	if ( trj->GetPDGEncoding() == 13 ) muonList.insert(trj->GetTrackID());
	if ( trj->GetPDGEncoding() == -13 ) antimuonList.insert(trj->GetTrackID());
	if ( trj->GetPDGEncoding() == 211 ) pionList.insert(trj->GetTrackID());
	if ( trj->GetPDGEncoding() == -211 ) antipionList.insert(trj->GetTrackID());
	if ( trj->GetParentID() == 0 ) primaryList.insert(trj->GetTrackID());

#ifdef WCSIM_SAVE_PHOTON_HISTORY
	trajMap[trj->GetTrackID()] = i;
#endif

	// Process primary tracks or the secondaries from pizero or muons...

	if ( trj->GetSaveFlag() || trj->GetProducesHit() )
	  {
	    // initial point of the trajectory
	    G4TrajectoryPoint* aa =   (G4TrajectoryPoint*)trj->GetPoint(0) ;

	    G4int         ipnu   = trj->GetPDGEncoding();
	    G4int         id     = trj->GetTrackID();
	    G4int         idPrnt = trj->GetParentID();
	    G4int         flag   = 0;    // will be set later
	    G4double      mass   = trj->GetParticleDefinition()->GetPDGMass();
	    G4ThreeVector mom    = trj->GetInitialMomentum();
	    G4double      mommag = mom.mag();
	    G4double      energy = sqrt(mom.mag2() + mass*mass);
	    G4ThreeVector Stop   = trj->GetStoppingPoint();
	    G4ThreeVector Start  = aa->GetPosition();

	    G4String stopVolumeName = trj->GetStoppingVolume()->GetName();
	    G4int    stopvol     = WCSimEventFindStoppingVolume(stopVolumeName);
	    G4int    startvol    = WCSimEventFindStartingVolume(Start);

	    G4double ttime = trj->GetGlobalTime();

	    G4int parentType;
	    ProcessType_t creatorProcess = WCSimEnumerations::ProcessTypeStringToEnum(trj->GetCreatorProcessName());
      
	    // Right now only secondaries whose parents are pi0's are stored
	    // This may change later
	    // M Fechner : dec 16, 2004 --> added decay e- from muons
	    if (trj->GetParentID() == 0){
	      parentType = 0;
	    } else if (pizeroList.count(trj->GetParentID())   ) {
	      parentType = 111;
	    } else if (muonList.count(trj->GetParentID())     ) {
	      parentType = 13;
	    } else if (antimuonList.count(trj->GetParentID()) ) {
	      parentType = -13;
	    } else if (antipionList.count(trj->GetParentID()) ) {
	      parentType = -211;
	    } else if (pionList.count(trj->GetParentID()) ) {
	      parentType = 211;
	    } else if (primaryList.count(trj->GetParentID()) ) {
	      parentType = 1;
	    } else {  // no identified parent, but not a primary
	      parentType = 999;
	    }

	    // G4cout << parentType << " " << ipnu << " "
	    //      << id << " " << energy << "\n";

	    // fill ntuple
	    double dir[3];
	    double pdir[3];
	    double stop[3];
	    double start[3];
	    for (int l=0;l<3;l++)
	      {
		dir[l]= mom[l]/mommag; // direction
		pdir[l]=mom[l];        // momentum-vector
		stop[l]=Stop[l]/cm; // stopping point
		start[l]=Start[l]/cm; // starting point
		//G4cout<<"part 2 start["<<l<<"]: "<< start[l] <<G4endl;
		//G4cout<<"part 2 stop["<<l<<"]: "<< stop[l] <<G4endl;
	      }

	    // Add the track to the TClonesArray, watching out for times
	    int choose_event = 0;

	    /*
	    //Keep all tracks in the 0th trigger. Otherwise it is very confusing
	    if (ngates) {

	      if (ttime > WCTM->GetTriggerTime(0) + 950. && WCTM->GetTriggerTime(1) + 950. > ttime) choose_event = 1;
	      if (ttime > WCTM->GetTriggerTime(1) + 950. && WCTM->GetTriggerTime(2) + 950. > ttime) choose_event = 2;
	      if (choose_event >= ngates) choose_event = ngates - 1; // do not overflow the number of events

	    }
	    */

	    wcsimrootevent = wcsimrootsuperevent->GetTrigger(choose_event);
	    wcsimrootevent->AddTrack(ipnu,
				     flag,
				     mass,
				     mommag,
				     energy,
				     startvol,
				     stopvol,
				     dir,
				     pdir,
				     stop,
				     start,
				     parentType,
				     creatorProcess,
				     ttime,
				     id,
				     idPrnt,
				     trj->GetBoundaryPoints(),
				     trj->GetBoundaryKEs(),
				     trj->GetBoundaryTimes(),
				     trj->GetBoundaryTypesAsInt());

	    if (detectorConstructor->SavePi0Info())
	      {
#ifdef SAVE_DIGITS_VERBOSE
		G4cout<<"Pi0 parentType: " << parentType <<G4endl;
#endif
		if (parentType == 111)
		  {
		    if (r>1)
		      G4cout<<"WARNING: more than 2 primary gammas found"<<G4endl;
		    else
		      {

			for (int y=0;y<3;y++)
			  {
			    pi0Vtx[y] = start[y];
			    gammaVtx[r][y] = stop[y];
			  }

			gammaID[r] = id;
			gammaE[r] = energy;
			r++;

			//amb79
			G4cout<<"Pi0 data: " << id <<G4endl;
			wcsimrootevent->SetPi0Info(pi0Vtx, gammaID, gammaE, gammaVtx);
		      }
		  }
	      }

	    if (detectorConstructor->SaveCaptureInfo() && trj->GetCreatorProcessName()=="nCapture"){
	      G4cout << "Capture particle: " << trj->GetParticleName()
		     << " Parent: " << trj->GetParentID()
		     << " T:" << ttime
		     << " vtx:(" << start[0] << "," << start[1] << "," << start[2]
		     << ") dir:(" << dir[0] << "," << dir[1] << "," << dir[2]
		     << ") E:" << energy << G4endl;
	      wcsimrootevent->SetCaptureParticle(trj->GetParentID(), ipnu, ttime, start, dir, energy, id);
	    }
	  }//if this trajectory should be saved
      }//end loop i over n_trajectories
  }//!skipFillingTracks

  // Add the Cherenkov hits
  wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

  wcsimrootevent->SetNumTubesHit(injhfNtuple.numTubesHit);

  const std::vector<WCSimPmtInfo*> *fpmts = detectorConstructor->Get_Pmts_By_Type(detectorElement);

#ifdef _SAVE_RAW_HITS

  if (WCDC_hits)
  {
    //add the truth raw hits
    // Both the pre- and post-PMT smearing hit times are accessible
    // Choose to save just the pre-smeared times for now
#ifdef _SAVE_RAW_HITS_VERBOSE
    G4cout<<"RAW HITS"<<G4endl;
#endif
    wcsimrootevent->SetNumTubesHit(WCDC_hits->entries());
    std::vector<double> truetime, smeartime;
    std::vector<int>   parentSavedTrackID;
    std::vector<float> photonStartTime;
    std::vector<TVector3> photonStartPos;
    std::vector<TVector3> photonEndPos;
    std::vector<TVector3> photonStartDir;
    std::vector<TVector3> photonEndDir;
    std::vector<ProcessType_t> photonCreatorProcess;
#ifdef _SAVE_RAW_HITS_VERBOSE
    double hit_time_smear;
#endif
    double hit_time_true;
    int hit_parentid;
    float hit_photon_starttime;
    TVector3 hit_photon_startpos;
    TVector3 hit_photon_endpos;
    TVector3 hit_photon_startdir;
    TVector3 hit_photon_enddir;
    ProcessType_t hit_photon_creatorprocess;
    //loop over the DigitsCollection
    for(int idigi = 0; idigi < WCDC_hits->entries(); idigi++) {
      int digi_tubeid = (*WCDC_hits)[idigi]->GetTubeID();
      WCSimPmtInfo *pmt = ((WCSimPmtInfo*)fpmts->at(digi_tubeid -1));

      for(G4int id = 0; id < (*WCDC_hits)[idigi]->GetTotalPe(); id++){
#ifdef WCSIM_SAVE_PHOTON_HISTORY
        int trackID = (*WCDC_hits)[idigi]->GetTrackID(id);
        int hit_photon_RayScatter = 0;
        int hit_photon_MieScatter = 0;
        std::vector<ReflectionSurface_t> hit_photon_reflection = std::vector<ReflectionSurface_t>();
        if (trackID>0) // skip noise hit
        {
          WCSimTrajectory* trj = (WCSimTrajectory*)(*TC)[trajMap[trackID]];
          hit_photon_RayScatter = trj->GetPhotonRayScatter();
          hit_photon_MieScatter = trj->GetPhotonMieScatter();
          hit_photon_reflection = trj->GetPhotonReflection();
        }
        wcsimrootevent->AddCherenkovHitHistory(hit_photon_RayScatter,hit_photon_MieScatter,hit_photon_reflection);
#endif
	hit_time_true  = (*WCDC_hits)[idigi]->GetPreSmearTime(id);
	hit_parentid = (*WCDC_hits)[idigi]->GetParentID(id);
	hit_photon_starttime = (*WCDC_hits)[idigi]->GetPhotonStartTime(id);
	hit_photon_startpos = TVector3(
	        (*WCDC_hits)[idigi]->GetPhotonStartPos(id)[0],
	        (*WCDC_hits)[idigi]->GetPhotonStartPos(id)[1],
	        (*WCDC_hits)[idigi]->GetPhotonStartPos(id)[2]);
	hit_photon_endpos = TVector3(
	        (*WCDC_hits)[idigi]->GetPhotonEndPos(id)[0],
	        (*WCDC_hits)[idigi]->GetPhotonEndPos(id)[1],
	        (*WCDC_hits)[idigi]->GetPhotonEndPos(id)[2]);
	hit_photon_startdir = TVector3(
	        (*WCDC_hits)[idigi]->GetPhotonStartDir(id)[0],
	        (*WCDC_hits)[idigi]->GetPhotonStartDir(id)[1],
	        (*WCDC_hits)[idigi]->GetPhotonStartDir(id)[2]);
	hit_photon_enddir = TVector3(
	        (*WCDC_hits)[idigi]->GetPhotonEndDir(id)[0],
	        (*WCDC_hits)[idigi]->GetPhotonEndDir(id)[1],
	        (*WCDC_hits)[idigi]->GetPhotonEndDir(id)[2]);
  hit_photon_creatorprocess = (*WCDC_hits)[idigi]->GetPhotonCreatorProcess(id);

	truetime.push_back(hit_time_true);
	parentSavedTrackID.push_back(hit_parentid);
	photonStartTime.push_back(hit_photon_starttime);
	photonStartPos.push_back(hit_photon_startpos);
	photonEndPos.push_back(hit_photon_endpos);
	photonStartDir.push_back(hit_photon_startdir);
	photonEndDir.push_back(hit_photon_enddir);
  photonCreatorProcess.push_back(hit_photon_creatorprocess);
#ifdef _SAVE_RAW_HITS_VERBOSE
	hit_time_smear = (*WCDC_hits)[idigi]->GetTime(id);
	smeartime.push_back(hit_time_smear);
#endif
      }//id
#ifdef _SAVE_RAW_HITS_VERBOSE
      if(digi_tubeid < NPMTS_VERBOSE || digi_tubeid == VERBOSE_PMT) {
	G4cout << "Adding " << truetime.size()
	       << " Cherenkov hits in tube " << digi_tubeid
	       << " with truetime:smeartime:parentSavedTrackID";
	for(size_t id = 0; id < truetime.size(); id++) {
	  G4cout << " " << truetime[id]
		 << "\t" << smeartime[id]
		 << "\t" << parentSavedTrackID[id] << G4endl;
	}//id
	G4cout << G4endl;
      }
#endif
      wcsimrootevent->AddCherenkovHit(digi_tubeid,
				      pmt->Get_mPMTid(),
				      pmt->Get_mPMT_pmtid(),
				      truetime,
				      parentSavedTrackID,
				      photonStartTime,
				      photonStartPos,
				      photonEndPos,
				      photonStartDir,
				      photonEndDir,
              photonCreatorProcess);
      smeartime.clear();
      truetime.clear();
      parentSavedTrackID.clear();
      photonStartTime.clear();
      photonStartPos.clear();
      photonEndPos.clear();
      photonStartDir.clear();
      photonEndDir.clear();
      photonCreatorProcess.clear();
    }//idigi
  }//if(WCDC_hits)


#endif //_SAVE_RAW_HITS

  // Add the digitized hits

  if (WCDC)
  {
#ifdef SAVE_DIGITS_VERBOSE
    G4cout << "DIGI HITS" << G4endl;
#endif

    G4double sumq_tmp = 0.;

    for ( int index = 0 ; index < ngates ; index++)
      {
	sumq_tmp = 0.0;
	G4double gatestart;
	int countdigihits = 0;
	wcsimrootevent = wcsimrootsuperevent->GetTrigger(index);
	for (int k=0;k<WCDC->entries();k++)
	  {
	    if ( (*WCDC)[k]->HasHitsInGate(index)) {
	      std::vector<double> vec_pe                  = (*WCDC)[k]->GetPe(index);
	      std::vector<double> vec_time                = (*WCDC)[k]->GetTime(index);
	      std::vector<std::vector<int> > vec_digicomp = (*WCDC)[k]->GetDigiCompositionInfo(index);
	      const int tubeID                           = (*WCDC)[k]->GetTubeID();
	      WCSimPmtInfo *pmt = ((WCSimPmtInfo*)fpmts->at(tubeID -1));
	      assert(vec_pe.size() == vec_time.size());
	      assert(vec_pe.size() == vec_digicomp.size());
	      for(unsigned int iv = 0; iv < vec_pe.size(); iv++) {
#ifdef SAVE_DIGITS_VERBOSE
		if(tubeID < NPMTS_VERBOSE || tubeID == VERBOSE_PMT) {
		  G4cout << "Adding digit " << iv
			 << " for PMT " << tubeID
			 << " pe "   << vec_pe[iv]
			 << " time " << vec_time[iv]
			 << " digicomp";
		  for(unsigned int ivv = 0; ivv < vec_digicomp[iv].size(); ivv++)
		    G4cout << " " << vec_digicomp[iv][ivv];
		  G4cout << G4endl;
		}
#endif
		assert(vec_digicomp[iv].size() > 0);
		wcsimrootevent->AddCherenkovDigiHit(vec_pe[iv], vec_time[iv],
						    tubeID, pmt->Get_mPMTid(), pmt->Get_mPMT_pmtid(),
						    vec_digicomp[iv]);
		sumq_tmp += vec_pe[iv];
		countdigihits++;
	      }//iv
	    }//Digit exists in Gate
	  }//k
	wcsimrootevent->SetNumDigitizedTubes(countdigihits);    // TF: Aren't these total number of hits, NOT Tubes?
	wcsimrootevent->SetSumQ(sumq_tmp);

#ifdef SAVE_DIGITS_VERBOSE
	G4cout << "checking digi hits ...\n";
	G4cout << "hits collection size (number of PMTs hit) =  " <<
	  wcsimrootevent->GetCherenkovHits()->GetEntries() << "\n";
	G4cout << "hits collection size (number of true photon + dark noise hits) =  " <<
	  wcsimrootevent->GetCherenkovHitTimes()->GetEntries() << "\n";
	G4cout << "digihits collection size =  " <<
	  wcsimrootevent->GetCherenkovDigiHits()->GetEntries() << "\n";
	G4cout << "tracks collection size =  " <<
	  wcsimrootevent->GetTracks()->GetEntries()
	       <<" get ntracks = " <<  wcsimrootevent->GetNtrack() << "\n";
	for(size_t itrack = 0; itrack < wcsimrootevent->GetNtrack(); itrack++)
	  G4cout << ((WCSimRootTrack*)(wcsimrootevent->GetTracks()->At(itrack)))->GetId() << "\t"
		 << ((WCSimRootTrack*)(wcsimrootevent->GetTracks()->At(itrack)))->GetIpnu() << "\t"
		 << ((WCSimRootTrack*)(wcsimrootevent->GetTracks()->At(itrack)))->GetStartvol() << "\t"
		 << ((WCSimRootTrack*)(wcsimrootevent->GetTracks()->At(itrack)))->GetStopvol() << "\t"
		 << G4endl;
#endif

	gatestart = WCTM->GetTriggerTime(index);
	WCSimRootEventHeader*HH = wcsimrootevent->GetHeader();
	HH->SetDate(gatestart);
      }//index (loop over ngates)

    // end of loop over WC trigger gates --> back to the main sub-event
    wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

  }


  for (int i = 0 ; i < wcsimrootsuperevent->GetNumberOfEvents(); i++) {
    wcsimrootevent = wcsimrootsuperevent->GetTrigger(i);
    int evtID=wcsimrootevent->GetHeader()->GetEvtNum();
    if(evtID%100==0) G4cout << ">>>Root event "
			<<std::setw(5)<<evtID<<"\n";
    //   if (WCDC){
    // G4cout <<"WC digi:"<<std::setw(4)<<wcsimrootevent->GetNcherenkovdigihits()<<"  ";
    // G4cout <<"WC digi sumQ:"<<std::setw(4)<<wcsimrootevent->GetSumQ()<<"  ";
  }

#ifdef _SAVE_RAW_HITS
  //if (WCHC)
  //     G4cout <<"WC:"<<std::setw(4)<<wcsimrootevent->GetNcherenkovhits()<<"  ";
  //    if (WCFVHC)
  //G4cout <<"WCFV:"<<std::setw(4)<<wcsimrootevent->GetINcherenkovhits()<<"  ";
#endif

  //  if (WCFVDC){
  //G4cout <<"WCFV digi:"<<std::setw(4)<<wcsimrootevent->GetNcherenkovdigihits()<<"  ";
  //G4cout <<"WCFV digi sumQ:"<<std::setw(4)<<wcsimrootevent->GetSumQ()<<"  ";
  //  }

  /*
  // Check we are supposed to be saving the NEUT vertex and that the generator was given a NEUT vector file to process
  // If there is no NEUT vector file an empty NEUT vertex will be written to the output file
  if(GetRunAction()->GetSaveRooTracker() && generatorAction->IsUsingRootrackerEvtGenerator()){
      GetRunAction()->ClearRootrackerVertexArray();
      generatorAction->CopyRootrackerVertex(GetRunAction()->GetRootrackerVertex()); //will increment NVtx
      GetRunAction()->FillRootrackerVertexTree();
  }
  */

}

void WCSimEventAction::FillRootEventHybrid(G4int event_id,
				       const struct ntupleStruct& injhfNtuple,
				       G4TrajectoryContainer* TC,
				       WCSimWCDigitsCollection* WCDC_hits,
				       WCSimWCTriggeredDigitsCollection* WCDC,
				       G4String detectorElement,
				       WCSimRootEvent * wcsimrootsuperevent,
					   WCSimRootTrigger * wcsimrootevent,
					   bool skipFillingTracks)
 {
  wcsimrootsuperevent->ReInitialize();
  // start with the first "sub-event"
  // if the WC digitization requires it, we will add another subevent
  // for the WC.
  // all the rest goes into the first "sub-event".
  // get number of gates
  G4DigiManager* DMman = G4DigiManager::GetDMpointer();
  WCSimWCTriggerBase* WCTM;
  if(detectorElement=="tank"){
    WCTM = (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout");
  } else if(detectorElement=="tankPMT2"){
    WCTM = (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout2");
  } else if(detectorElement=="OD"){
    WCTM = (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout_OD");
  } else {
    G4cout << "Unknown detector element: " << detectorElement << G4endl;
    exit(1);
  }

  int ngates = WCTM->NumberOfGatesInThisEvent();
  for (int index = 0 ; index < ngates ; index++)
    {
      if (index >=1 ) {
	wcsimrootsuperevent->AddSubEvent();
	wcsimrootevent = wcsimrootsuperevent->GetTrigger(index);
	wcsimrootevent->SetHeader(event_id, GetRunAction()->GetRunID(), 0, index+1); // date & # of subevent
	wcsimrootevent->SetMode(injhfNtuple.mode[0]);
      }
      //wcsimrootevent->SetTriggerInfo(WCTM->GetTriggerType(index),
      //			     WCTM->GetTriggerInfo(index));
      //Added by B.Q to keep track of he trigger, 2019/01/17
      std::vector<G4double> triggerInfo = WCTM->GetTriggerInfo(index);
      triggerInfo.push_back(950.);
      triggerInfo.push_back(WCTM->GetTriggerTime(index));
      wcsimrootevent->SetTriggerInfo(WCTM->GetTriggerType(index),triggerInfo);//Added by B.Q to keep track of he trigger, 2019/01/17
      triggerInfo.clear();
    }


  // Fill the header
  // Need to add date
  wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  wcsimrootevent->SetHeader(event_id, GetRunAction()->GetRunID(), 0); // will be set later.

  std::map<int,int> trajMap; // mapping of trackID and index

  // Fill other info for this event


  if(skipFillingTracks) {
    wcsimrootevent->SetNvtxs(0);
#ifdef WCSIM_SAVE_PHOTON_HISTORY
    // still need the trackID mapping
    G4int n_trajectories = 0;
    if (TC)
      n_trajectories = TC->entries();
    for (int i=0; i <n_trajectories; i++)
    {
	    WCSimTrajectory* trj = (WCSimTrajectory*)(*TC)[i];
      trajMap[trj->GetTrackID()] = i;
    }
#endif  
  }
  else {
    wcsimrootevent->SetNvtxs(injhfNtuple.nvtxs);
    for( Int_t u=0; u<injhfNtuple.nvtxs; u++ ){
      wcsimrootevent->SetVtxsvol(u,injhfNtuple.vtxsvol[u]);
      for (int j=0;j<4;j++)
	{
	  wcsimrootevent->SetVtxs(u,j,injhfNtuple.vtxs[u][j]);
	}
      wcsimrootevent->SetMode(u,injhfNtuple.mode[u]);
    }
    wcsimrootevent->SetJmu(injhfNtuple.jmu);           //TF: undefined and only for Nuance
    wcsimrootevent->SetJp(injhfNtuple.jp);             //TF: undefined and only for Nuance
    wcsimrootevent->SetNpar(injhfNtuple.npar);         //TF: undefined
    wcsimrootevent->SetVecRecNumber(injhfNtuple.vecRecNumber);  //TF: undefined and only for Nuance

    // Add the tracks with the particle information
    // First two tracks come from injhfNtuple, as they are special

    for (int k=0;k<2;k++) // should be just 2
      {
	double dir[3];
	double pdir[3];
	double stop[3];
	double start[3];
	ProcessType_t creatorP = kUnknownProcess;
	for (int l=0;l<3;l++)
	  {
	    dir[l]=injhfNtuple.dir[k][l];
	    pdir[l]=injhfNtuple.pdir[k][l];
	    stop[l]=injhfNtuple.stop[k][l];
	    start[l]=injhfNtuple.start[k][l];
	    //G4cout<< "start[" << k << "][" << l <<"]: "<< injhfNtuple.start[k][l] <<G4endl;
	    //G4cout<< "stop[" << k << "][" << l <<"]: "<< injhfNtuple.stop[k][l] <<G4endl;
	  }

	// Add the track to the TClonesArray
	wcsimrootevent->AddTrack(injhfNtuple.ipnu[k],
				 injhfNtuple.flag[k],
				 injhfNtuple.m[k],
				 injhfNtuple.p[k],
				 injhfNtuple.E[k],
				 injhfNtuple.startvol[k],
				 injhfNtuple.stopvol[k],
				 dir,
				 pdir,
				 stop,
				 start,
				 injhfNtuple.parent[k],
				 creatorP,
				 injhfNtuple.time[k],
				 0,
				 0,
				 std::vector<std::vector<float>>(),
				 std::vector<float>(),
				 std::vector<double>(),
				 std::vector<int>());
      }

    // the rest of the tracks come from WCSimTrajectory

    std::set<int> pizeroList;
    // added by M Fechner, dec 16th, 2004
    std::set<int> muonList;
    std::set<int> antimuonList;
    // same, april 7th 2005
    std::set<int> pionList;
    std::set<int> antipionList;

    // Pi0 specific variables
    Double_t pi0Vtx[3];
    Int_t   gammaID[2];
    Double_t gammaE[2];
    Double_t gammaVtx[2][3];
    Int_t   r = 0;

    G4int n_trajectories = 0;
    if (TC)
      n_trajectories = TC->entries();

    // M Fechner : removed this limit to get to the primaries...
    //if (n_trajectories>50)  // there is no need for this limit, but it has
    //n_trajectories=50;    // existed in previous versions of the code.  It also
    // makes the ROOT file smaller.

    for (int i=0; i <n_trajectories; i++)
      {
	WCSimTrajectory* trj = (WCSimTrajectory*)(*TC)[i];

	// If this track is a pizero remember it for later
	if ( trj->GetPDGEncoding() == 111)
	  pizeroList.insert(trj->GetTrackID());
	// If it is a mu+/mu- also remember it
	if ( trj->GetPDGEncoding() == 13 ) muonList.insert(trj->GetTrackID());
	if ( trj->GetPDGEncoding() == -13 ) antimuonList.insert(trj->GetTrackID());
	if ( trj->GetPDGEncoding() == 211 ) pionList.insert(trj->GetTrackID());
	if ( trj->GetPDGEncoding() == -211 ) antipionList.insert(trj->GetTrackID());

#ifdef WCSIM_SAVE_PHOTON_HISTORY
	trajMap[trj->GetTrackID()] = i;
#endif

	// Process primary tracks or the secondaries from pizero or muons...

	if ( trj->GetSaveFlag() || trj->GetProducesHit() )
	  {
	    // initial point of the trajectory
	    G4TrajectoryPoint* aa =   (G4TrajectoryPoint*)trj->GetPoint(0) ;

	    G4int         ipnu   = trj->GetPDGEncoding();
	    G4int         id     = trj->GetTrackID();
	    G4int         idPrnt = trj->GetParentID();
	    G4int         flag   = 0;    // will be set later
	    G4double      mass   = trj->GetParticleDefinition()->GetPDGMass();
	    G4ThreeVector mom    = trj->GetInitialMomentum();
	    G4double      mommag = mom.mag();
	    G4double      energy = sqrt(mom.mag2() + mass*mass);
	    G4ThreeVector Stop   = trj->GetStoppingPoint();
	    G4ThreeVector Start  = aa->GetPosition();

	    G4String stopVolumeName = trj->GetStoppingVolume()->GetName();
	    G4int    stopvol     = WCSimEventFindStoppingVolume(stopVolumeName);
	    G4int    startvol    = WCSimEventFindStartingVolume(Start);

	    G4double ttime = trj->GetGlobalTime();

	    G4int parentType;
	    ProcessType_t creatorProcess = WCSimEnumerations::ProcessTypeStringToEnum(trj->GetCreatorProcessName());

	    // Right now only secondaries whose parents are pi0's are stored
	    // This may change later
	    // M Fechner : dec 16, 2004 --> added decay e- from muons
	    if (trj->GetParentID() == 0){
	      parentType = 0;
	    } else if (pizeroList.count(trj->GetParentID())   ) {
	      parentType = 111;
	    } else if (muonList.count(trj->GetParentID())     ) {
	      parentType = 13;
	    } else if (antimuonList.count(trj->GetParentID()) ) {
	      parentType = -13;
	    } else if (antipionList.count(trj->GetParentID()) ) {
	      parentType = -211;
	    } else if (pionList.count(trj->GetParentID()) ) {
	      parentType = 211;
	    } else {  // no identified parent, but not a primary
	      parentType = 999;
	    }

	    // G4cout << parentType << " " << ipnu << " "
	    //      << id << " " << energy << "\n";

	    // fill ntuple
	    double dir[3];
	    double pdir[3];
	    double stop[3];
	    double start[3];
	    for (int l=0;l<3;l++)
	      {
		dir[l]= mom[l]/mommag; // direction
		pdir[l]=mom[l];        // momentum-vector
		stop[l]=Stop[l]/cm; // stopping point
		start[l]=Start[l]/cm; // starting point
		//G4cout<<"part 2 start["<<l<<"]: "<< start[l] <<G4endl;
		//G4cout<<"part 2 stop["<<l<<"]: "<< stop[l] <<G4endl;
	      }

	    // Add the track to the TClonesArray, watching out for times
	    int choose_event = 0;

	    /*
	    //Keep all tracks in the 0th trigger. Otherwise it is very confusing
	    if (ngates) {

	      if (ttime > WCTM->GetTriggerTime(0) + 950. && WCTM->GetTriggerTime(1) + 950. > ttime) choose_event = 1;
	      if (ttime > WCTM->GetTriggerTime(1) + 950. && WCTM->GetTriggerTime(2) + 950. > ttime) choose_event = 2;
	      if (choose_event >= ngates) choose_event = ngates - 1; // do not overflow the number of events

	    }
	    */

	    wcsimrootevent = wcsimrootsuperevent->GetTrigger(choose_event);
	    wcsimrootevent->AddTrack(ipnu,
				     flag,
				     mass,
				     mommag,
				     energy,
				     startvol,
				     stopvol,
				     dir,
				     pdir,
				     stop,
				     start,
				     parentType,
				     creatorProcess,
				     ttime,
				     id,
				     idPrnt,
				     trj->GetBoundaryPoints(),
				     trj->GetBoundaryKEs(),
				     trj->GetBoundaryTimes(),
				     trj->GetBoundaryTypesAsInt());

	    if (detectorConstructor->SavePi0Info())
	      {
#ifdef SAVE_DIGITS_VERBOSE
		G4cout<<"Pi0 parentType: " << parentType <<G4endl;
#endif
		if (parentType == 111)
		  {
		    if (r>1)
		      G4cout<<"WARNING: more than 2 primary gammas found"<<G4endl;
		    else
		      {

			for (int y=0;y<3;y++)
			  {
			    pi0Vtx[y] = start[y];
			    gammaVtx[r][y] = stop[y];
			  }

			gammaID[r] = id;
			gammaE[r] = energy;
			r++;

			//amb79
			G4cout<<"Pi0 data: " << id <<G4endl;
			wcsimrootevent->SetPi0Info(pi0Vtx, gammaID, gammaE, gammaVtx);
		      }
		  }
	      }

	    if (detectorConstructor->SaveCaptureInfo() && trj->GetCreatorProcessName()=="nCapture"){
	      G4cout << "Capture particle: " << trj->GetParticleName()
		     << " Parent: " << trj->GetParentID()
		     << " T:" << ttime
		     << " vtx:(" << start[0] << "," << start[1] << "," << start[2]
		     << ") dir:(" << dir[0] << "," << dir[1] << "," << dir[2]
		     << ") E:" << energy << G4endl;
	      wcsimrootevent->SetCaptureParticle(trj->GetParentID(), ipnu, ttime, start, dir, energy, id);
	    }
	  }//if track should be saved
      }//end loop i over n_trajectories
  }//!skipFillingTracks
  
  // Add the Cherenkov hits
  wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

  wcsimrootevent->SetNumTubesHit(injhfNtuple.numTubesHit);

  const std::vector<WCSimPmtInfo*> *fpmts = detectorConstructor->Get_Pmts_By_Type(detectorElement);

#ifdef _SAVE_RAW_HITS

  if (WCDC_hits)
  {
    //add the truth raw hits
    // Both the pre- and post-PMT smearing hit times are accessible
    // Choose to save just the pre-smeared times for now
#ifdef _SAVE_RAW_HITS_VERBOSE
    G4cout<<"RAW HITS"<<G4endl;
#endif
    wcsimrootevent->SetNumTubesHit(WCDC_hits->entries());
    std::vector<double> truetime, smeartime;
    std::vector<int>   parentSavedTrackID;
    std::vector<float> photonStartTime;
    std::vector<TVector3> photonStartPos;
    std::vector<TVector3> photonEndPos;
    std::vector<TVector3> photonStartDir;
    std::vector<TVector3> photonEndDir;
    std::vector<ProcessType_t> photonCreatorProcess;               // ADD A TEMPORARY VECTOR OF CREATOR PROCESSES
#ifdef _SAVE_RAW_HITS_VERBOSE
    double hit_time_smear;
#endif
    double hit_time_true;
    int hit_parentid;
    float hit_photon_starttime;
    TVector3 hit_photon_startpos;
    TVector3 hit_photon_endpos;
    TVector3 hit_photon_startdir;
    TVector3 hit_photon_enddir;
    ProcessType_t hit_photon_creatorprocess;
    //loop over the DigitsCollection
    for(int idigi = 0; idigi < WCDC_hits->entries(); idigi++) {
      int digi_tubeid = (*WCDC_hits)[idigi]->GetTubeID();
      WCSimPmtInfo *pmt = ((WCSimPmtInfo*)fpmts->at(digi_tubeid -1));

      for(G4int id = 0; id < (*WCDC_hits)[idigi]->GetTotalPe(); id++){
#ifdef WCSIM_SAVE_PHOTON_HISTORY
        int trackID = (*WCDC_hits)[idigi]->GetTrackID(id);
        int hit_photon_RayScatter = 0;
        int hit_photon_MieScatter = 0;
        std::vector<ReflectionSurface_t> hit_photon_reflection = std::vector<ReflectionSurface_t>();
        if (trackID>0) // skip noise hit
        {
          WCSimTrajectory* trj = (WCSimTrajectory*)(*TC)[trajMap[trackID]];
          hit_photon_RayScatter = trj->GetPhotonRayScatter();
          hit_photon_MieScatter = trj->GetPhotonMieScatter();
          hit_photon_reflection = trj->GetPhotonReflection();
        }
        wcsimrootevent->AddCherenkovHitHistory(hit_photon_RayScatter,hit_photon_MieScatter,hit_photon_reflection);
#endif
	hit_time_true  = (*WCDC_hits)[idigi]->GetPreSmearTime(id);
	hit_parentid = (*WCDC_hits)[idigi]->GetParentID(id);
	hit_photon_starttime = (*WCDC_hits)[idigi]->GetPhotonStartTime(id);
  hit_photon_creatorprocess = (*WCDC_hits)[idigi]->GetPhotonCreatorProcess(id);
	hit_photon_startpos = TVector3(
	        (*WCDC_hits)[idigi]->GetPhotonStartPos(id)[0],
	        (*WCDC_hits)[idigi]->GetPhotonStartPos(id)[1],
	        (*WCDC_hits)[idigi]->GetPhotonStartPos(id)[2]);
	hit_photon_endpos = TVector3(
	        (*WCDC_hits)[idigi]->GetPhotonEndPos(id)[0],
	        (*WCDC_hits)[idigi]->GetPhotonEndPos(id)[1],
	        (*WCDC_hits)[idigi]->GetPhotonEndPos(id)[2]);
	hit_photon_startdir = TVector3(
	        (*WCDC_hits)[idigi]->GetPhotonStartDir(id)[0],
	        (*WCDC_hits)[idigi]->GetPhotonStartDir(id)[1],
	        (*WCDC_hits)[idigi]->GetPhotonStartDir(id)[2]);
	hit_photon_enddir = TVector3(
	        (*WCDC_hits)[idigi]->GetPhotonEndDir(id)[0],
	        (*WCDC_hits)[idigi]->GetPhotonEndDir(id)[1],
	        (*WCDC_hits)[idigi]->GetPhotonEndDir(id)[2]);
	truetime.push_back(hit_time_true);
	parentSavedTrackID.push_back(hit_parentid);
	photonStartTime.push_back(hit_photon_starttime);
	photonStartPos.push_back(hit_photon_startpos);
	photonEndPos.push_back(hit_photon_endpos);
	photonStartDir.push_back(hit_photon_startdir);
	photonEndDir.push_back(hit_photon_enddir);
  photonCreatorProcess.push_back(hit_photon_creatorprocess);                         // ADD A GENERIC PROCESS NAME
#ifdef _SAVE_RAW_HITS_VERBOSE
	hit_time_smear = (*WCDC_hits)[idigi]->GetTime(id);
	smeartime.push_back(hit_time_smear);
#endif
      }//id
#ifdef _SAVE_RAW_HITS_VERBOSE
  G4cout << "Hits CreatorProcess: " << photonCreatorProcess << G4endl;
      if(digi_tubeid < NPMTS_VERBOSE) {
	G4cout << "Adding " << truetime.size()
	       << " Cherenkov hits in tube " << digi_tubeid
	       << " with truetime:smeartime:parentSavedTrackID";
	for(size_t id = 0; id < truetime.size(); id++) {
	  G4cout << " " << truetime[id]
		 << ":" << smeartime[id]
		 << ":" << parentSavedTrackID[id];
	}//id
	G4cout << G4endl;
      }
#endif
      wcsimrootevent->AddCherenkovHit(digi_tubeid,
				      pmt->Get_mPMTid(),
				      pmt->Get_mPMT_pmtid(),
				      truetime,
				      parentSavedTrackID,
				      photonStartTime,
				      photonStartPos,
				      photonEndPos,
				      photonStartDir,
				      photonEndDir,
              photonCreatorProcess);         // INCLUDE THE VECTOR OF CREATOR PROCESSES
      smeartime.clear();
      truetime.clear();
      parentSavedTrackID.clear();
      photonStartTime.clear();
      photonStartPos.clear();
      photonEndPos.clear();
      photonStartDir.clear();
      photonEndDir.clear();
      photonCreatorProcess.clear();                  // CLEAR THE TEMPORARY VECTOR
    }//idigi
  }//if(WCDC_hits)


#endif //_SAVE_RAW_HITS

  // Add the digitized hits

  if (WCDC)
  {
#ifdef SAVE_DIGITS_VERBOSE
    G4cout << "DIGI HITS" << G4endl;
#endif

    G4double sumq_tmp = 0.;

    for ( int index = 0 ; index < ngates ; index++)
      {
	sumq_tmp = 0.0;
	G4double gatestart;
	int countdigihits = 0;
	wcsimrootevent = wcsimrootsuperevent->GetTrigger(index);
	for (int k=0;k<WCDC->entries();k++)
	  {
	    if ( (*WCDC)[k]->HasHitsInGate(index)) {
	      std::vector<double> vec_pe                  = (*WCDC)[k]->GetPe(index);
	      std::vector<double> vec_time                = (*WCDC)[k]->GetTime(index);
	      std::vector<std::vector<int> > vec_digicomp = (*WCDC)[k]->GetDigiCompositionInfo(index);
	      const int tubeID                           = (*WCDC)[k]->GetTubeID();
	      WCSimPmtInfo *pmt = ((WCSimPmtInfo*)fpmts->at(tubeID -1));
	      assert(vec_pe.size() == vec_time.size());
	      assert(vec_pe.size() == vec_digicomp.size());
	      for(unsigned int iv = 0; iv < vec_pe.size(); iv++) {
#ifdef SAVE_DIGITS_VERBOSE
		G4cout << iv << G4endl;
		if(tubeID < NPMTS_VERBOSE) {
		  G4cout << "Adding digit " << iv
			 << " for PMT " << tubeID
			 << " pe "   << vec_pe[iv]
			 << " time " << vec_time[iv]
			 << " digicomp";
		  for(unsigned int ivv = 0; ivv < vec_digicomp[iv].size(); ivv++)
		    G4cout << " " << vec_digicomp[iv][ivv];
		  G4cout << G4endl;
		}
#endif
		assert(vec_digicomp[iv].size() > 0);
		wcsimrootevent->AddCherenkovDigiHit(vec_pe[iv], vec_time[iv],
						    tubeID, pmt->Get_mPMTid(), pmt->Get_mPMT_pmtid(),
						    vec_digicomp[iv]);
		sumq_tmp += vec_pe[iv];
		countdigihits++;
	      }//iv
	    }//Digit exists in Gate
	  }//k
	wcsimrootevent->SetNumDigitizedTubes(countdigihits);    // TF: Aren't these total number of hits, NOT Tubes?
	wcsimrootevent->SetSumQ(sumq_tmp);

#ifdef SAVE_DIGITS_VERBOSE
	G4cout << "checking digi hits ...\n";
	G4cout << "hits collection size (number of PMTs hit) =  " <<
	  wcsimrootevent->GetCherenkovHits()->GetEntries() << "\n";
	G4cout << "hits collection size (number of true photon + dark noise hits) =  " <<
	  wcsimrootevent->GetCherenkovHitTimes()->GetEntries() << "\n";
	G4cout << "digihits collection size =  " <<
	  wcsimrootevent->GetCherenkovDigiHits()->GetEntries() << "\n";
	G4cout << "tracks collection size =  " <<
	  wcsimrootevent->GetTracks()->GetEntries()
	       <<" get ntracks = " <<  wcsimrootevent->GetNtrack() << "\n";
#endif

	gatestart = WCTM->GetTriggerTime(index);
	WCSimRootEventHeader*HH = wcsimrootevent->GetHeader();
	HH->SetDate(int(gatestart));
      }//index (loop over ngates)

    // end of loop over WC trigger gates --> back to the main sub-event
    wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

  }


  for (int i = 0 ; i < wcsimrootsuperevent->GetNumberOfEvents(); i++) {
    wcsimrootevent = wcsimrootsuperevent->GetTrigger(i);
    int evtID=wcsimrootevent->GetHeader()->GetEvtNum();
    if(evtID%100==0) G4cout << ">>>Root event "
			    <<std::setw(5)<<evtID<<G4endl;
    //   if (WCDC){
    // G4cout <<"WC digi:"<<std::setw(4)<<wcsimrootevent->GetNcherenkovdigihits()<<"  ";
    // G4cout <<"WC digi sumQ:"<<std::setw(4)<<wcsimrootevent->GetSumQ()<<"  ";
  }

#ifdef _SAVE_RAW_HITS
  //if (WCHC)
  //     G4cout <<"WC:"<<std::setw(4)<<wcsimrootevent->GetNcherenkovhits()<<"  ";
  //    if (WCFVHC)
  //G4cout <<"WCFV:"<<std::setw(4)<<wcsimrootevent->GetINcherenkovhits()<<"  ";
#endif

  //  if (WCFVDC){
  //G4cout <<"WCFV digi:"<<std::setw(4)<<wcsimrootevent->GetNcherenkovdigihits()<<"  ";
  //G4cout <<"WCFV digi sumQ:"<<std::setw(4)<<wcsimrootevent->GetSumQ()<<"  ";
  //  }

  /*
  // Check we are supposed to be saving the NEUT vertex and that the generator was given a NEUT vector file to process
  // If there is no NEUT vector file an empty NEUT vertex will be written to the output file
  if(GetRunAction()->GetSaveRooTracker() && generatorAction->IsUsingRootrackerEvtGenerator()){
      GetRunAction()->ClearRootrackerVertexArray();
      generatorAction->CopyRootrackerVertex(GetRunAction()->GetRootrackerVertex()); //will increment NVtx
      GetRunAction()->FillRootrackerVertexTree();
  }
  */
}


