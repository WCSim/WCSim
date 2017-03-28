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
#ifndef _SAVE_RAW_HITS_VERBOSE
//#define _SAVE_RAW_HITS_VERBOSE
#endif
#endif
#ifndef SAVE_DIGITS_VERBOSE
//#define SAVE_DIGITS_VERBOSE
#endif
#ifndef TIME_DAQ_STEPS
//#define TIME_DAQ_STEPS
#endif

#ifndef NPMTS_VERBOSE
#define NPMTS_VERBOSE 10
#endif

WCSimEventAction::WCSimEventAction(WCSimRunAction* myRun, 
				   WCSimDetectorConstruction* myDetector, 
				   WCSimPrimaryGeneratorAction* myGenerator)
  :runAction(myRun), generatorAction(myGenerator), 
   detectorConstructor(myDetector),
   ConstructedDAQClasses(false),
   SavedOptions(false)
{
  DAQMessenger = new WCSimWCDAQMessenger(this);

  G4DigiManager* DMman = G4DigiManager::GetDMpointer();

  //create PMT response module
  WCSimWCPMT* WCDMPMT = new WCSimWCPMT( "WCReadoutPMT", myDetector);
  DMman->AddNewModule(WCDMPMT);

  randGen = new TRandom3();

  //create dark noise module
  WCSimWCAddDarkNoise* WCDNM = new WCSimWCAddDarkNoise("WCDarkNoise", detectorConstructor);
  DMman->AddNewModule(WCDNM);
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
    WCSimWCDigitizerSKI* WCDM = new WCSimWCDigitizerSKI("WCReadoutDigits", detectorConstructor, DAQMessenger);
    DMman->AddNewModule(WCDM);
  }
  else {
    G4cerr << "Unknown DigitizerChoice " << DigitizerChoice << G4endl;
    exit(-1);
  }

  //create your choice of trigger module
  if(TriggerChoice == "NDigits") {
    WCSimWCTriggerNDigits* WCTM = new WCSimWCTriggerNDigits("WCReadout", detectorConstructor, DAQMessenger);
    DMman->AddNewModule(WCTM);
  }
  else if(TriggerChoice == "NDigits2") {
    WCSimWCTriggerNDigits2* WCTM = new WCSimWCTriggerNDigits2("WCReadout", detectorConstructor, DAQMessenger);
    DMman->AddNewModule(WCTM);
  }
  else {
    G4cerr << "Unknown TriggerChoice " << TriggerChoice << G4endl;
    exit(-1);
  }

  ConstructedDAQClasses = true;
}


void WCSimEventAction::BeginOfEventAction(const G4Event*)
{
  if(!ConstructedDAQClasses) {
    CreateDAQInstances();

    //and save options in output file
    G4DigiManager* DMman = G4DigiManager::GetDMpointer();

  }
}

void WCSimEventAction::EndOfEventAction(const G4Event* evt)
{

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

  if(generatorAction->GetIsRooTrackerFileFinished()){
      const G4Run* run;
      GetRunAction()->EndOfRunAction(run);
      exit(0);
  }

  // ----------------------------------------------------------------------
  //  Get Event Information
  // ----------------------------------------------------------------------

  G4int         event_id = evt->GetEventID();
  //G4int         mode     = generatorAction->GetMode();
  InteractionType_t mode     = generatorAction->GetMode();
  G4ThreeVector vtx      = generatorAction->GetVtx();
  G4int         vtxvol   = WCSimEventFindStartingVolume(vtx);
  G4int         vecRecNumber = generatorAction->GetVecRecNumber();

  // ----------------------------------------------------------------------
  //  Get WC Hit Collection
  // ----------------------------------------------------------------------

  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  // Get Hit collection of this event
  G4HCofThisEvent* HCE         = evt->GetHCofThisEvent();
  WCSimWCHitsCollection* WCHC = 0;
  G4String WCIDCollectionName = detectorConstructor->GetIDCollectionName();
  if (HCE)
  { 
    G4String name =   WCIDCollectionName;
    G4int collectionID = SDman->GetCollectionID(name);
    WCHC = (WCSimWCHitsCollection*)HCE->GetHC(collectionID);
  }

  // If this option is chosen
  // pe's will be generated on the pmts.
  if( generatorAction->IsUsingPoissonPMT() ){

    // Loop through PMTs in detector
    for (
	 std::vector<WCSimPmtInfo*>::iterator pmtIt = detectorConstructor->Get_Pmts()->begin();
	 pmtIt != detectorConstructor->Get_Pmts()->end();
	 pmtIt++ ){
      
      // Generate number of PEs acording to Poisson with macro defined mean
      int nPoisson = randGen->Poisson(generatorAction->GetPoissonPMTMean());
      
      // Check if PMTs have existing hits
      bool hitExists = false;
      int hitIndex = -1;
      for (int existingHit = 0; existingHit < WCHC->GetSize(); existingHit++){

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
     
      (*WCHC)[hitIndex]->SetTubeID((*pmtIt)->Get_tubeid());
      (*WCHC)[hitIndex]->SetTrackID(0);
      (*WCHC)[hitIndex]->SetEdep(0.);
      (*WCHC)[hitIndex]->SetPos(detectorConstructor->GetTubeTransform((*pmtIt)->Get_tubeid()).getTranslation());
      (*WCHC)[hitIndex]->SetRot(detectorConstructor->GetTubeTransform((*pmtIt)->Get_tubeid()).getRotation());
      
      // Ignore logical volume for now...
      for (int pe = 0; pe < nPoisson; pe++) {
	(*WCHC)[hitIndex]->AddPe(G4RandGauss::shoot(0.0,10.));
	(*WCHC)[hitIndex]->AddParentID(0); // Make parent a geantino (whatever that is)
      }
    }
  }
  


  // To use Do like This:
  // --------------------
  //   if (WCHC)
  //     for (G4int i=0; i< WCHC->entries() ;i++)
  //       G4cout << (*WCHC)[i]->GetTotalPe() << G4endl;
  
  
  // ----------------------------------------------------------------------
  //  Get Digitized Hit Collection
  // ----------------------------------------------------------------------

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

  //Convert the hits to PMT pulse
  WCDMPMT->Digitize();

  //
  // Do the Dark Noise, then Digitization, then Trigger
  //

  //
  // First, add Dark noise hits before digitizing
    
  //Get a pointer to the WC Dark Noise Module
  WCSimWCAddDarkNoise* WCDNM =
    (WCSimWCAddDarkNoise*)DMman->FindDigitizerModule("WCDarkNoise");
  
  //Add the dark noise
  WCDNM->AddDarkNoise();

  //
  // Next, do the digitization
  
  //Get a pointer to the WC Digitizer Module
  WCSimWCDigitizerBase* WCDM =
    (WCSimWCDigitizerBase*)DMman->FindDigitizerModule("WCReadoutDigits");

  //Digitize the hits
  WCDM->Digitize();

  //
  // Finally, apply the trigger
  
  //Get a pointer to the WC Trigger Module
  WCSimWCTriggerBase* WCTM =
    (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout");
  
  //tell it the dark noise rate (for calculating the average dark occupancy -> can adjust the NDigits threshold)
  WCTM->SetDarkRate(WCDNM->GetDarkRate());
  
  //Apply the trigger
  // This takes the digits, and places them into trigger gates
  // Also throws away digits not contained in an trigger gate
  WCTM->Digitize();

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
	 G4float photoElectrons = (*WCDC)[i]->GetPe(i);
	 G4float time           = (*WCDC)[i]->GetTime(i);
	 //	 G4cout << "time " << i << " " <<time << G4endl; 
	 //	 G4cout << "tubeID " << i << " " <<tubeID << G4endl; 
	 //	 G4cout << "Pe " << i << " " <<photoElectrons << G4endl; 
	 //   (*WCDC)[i]->Print();
       }
   */
   
  // ----------------------------------------------------------------------
  //  Fill Ntuple
  // ----------------------------------------------------------------------

   jhfNtuple.mode   = mode;         // interaction mode
   jhfNtuple.vtxvol = vtxvol;       // volume of vertex
   // unit mismatch between geant4 and reconstruction, M Fechner
   //  jhfNtuple.vtx[0] = vtx[0]/1000.; // interaction vertex
   //jhfNtuple.vtx[1] = vtx[1]/1000.; // interaction vertex
   //jhfNtuple.vtx[2] = vtx[2]/1000.; // interaction vertex
   jhfNtuple.vtx[0] = vtx[0]/cm; // interaction vertex
   jhfNtuple.vtx[1] = vtx[1]/cm; // interaction vertex
   jhfNtuple.vtx[2] = vtx[2]/cm; // interaction vertex
   jhfNtuple.vecRecNumber = vecRecNumber; //vectorfile record number
   
   // mustop, pstop, npar will be filled later
   
   // Next in the ntuple is an array of tracks.
   // We will keep count with npar
   
   G4int npar = 0;
   
   // First two tracks are special: beam and target
   
   G4int         beampdg    = generatorAction->GetBeamPDG();
   G4double      beamenergy = generatorAction->GetBeamEnergy();
   G4ThreeVector beamdir    = generatorAction->GetBeamDir();
   
   jhfNtuple.ipnu[npar]    = beampdg;               // id
   jhfNtuple.flag[npar]    = -1;                    // incoming neutrino
   jhfNtuple.m[npar]       = 0.0;                   // mass (always a neutrino)
   jhfNtuple.p[npar]       = beamenergy;            // momentum magnitude
   jhfNtuple.E[npar]       = beamenergy;            // energy 
   jhfNtuple.startvol[npar]= -1;                    // starting volume, vtxvol should be referred
   jhfNtuple.stopvol[npar] = -1;                    // stopping volume 
   jhfNtuple.dir[npar][0]  = beamdir[0];            // direction 
   jhfNtuple.dir[npar][1]  = beamdir[1];            // direction 
   jhfNtuple.dir[npar][2]  = beamdir[2];            // direction 
   jhfNtuple.pdir[npar][0] = beamenergy*beamdir[0]; // momentum-vector 
   jhfNtuple.pdir[npar][1] = beamenergy*beamdir[1]; // momentum-vector 
   jhfNtuple.pdir[npar][2] = beamenergy*beamdir[2]; // momentum-vector 
   // M Fechner, same as above
   jhfNtuple.stop[npar][0] = vtx[0]/cm;  // stopping point (not meaningful)
   jhfNtuple.stop[npar][1] = vtx[1]/cm;  // stopping point (not meaningful)
   jhfNtuple.stop[npar][2] = vtx[2]/cm;  // stopping point (not meaningful)
   jhfNtuple.parent[npar] = 0;
   
   npar++;

  G4double      targetpmag = 0.0, targetmass = 0.0;
  G4int         targetpdg    = generatorAction->GetTargetPDG();
  G4double      targetenergy = generatorAction->GetTargetEnergy();
  G4ThreeVector targetdir    = generatorAction->GetTargetDir();

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
  jhfNtuple.startvol[npar] = -1;           // starting volume 
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
  jhfNtuple.stop[npar][0] = vtx[0]/cm;  // stopping point (not meaningful)
  jhfNtuple.stop[npar][1] = vtx[1]/cm;  // stopping point (not meaningful)
  jhfNtuple.stop[npar][2] = vtx[2]/cm;  // stopping point (not meaningful)
  jhfNtuple.parent[npar] = 0;

  npar++;

  // Draw Charged Tracks

  for (G4int i=0; i < n_trajectories; i++) 
    {
      WCSimTrajectory* trj = 
	(WCSimTrajectory*)((*(evt->GetTrajectoryContainer()))[i]);

      if (trj->GetCharge() != 0.)
 	trj->DrawTrajectory(50);
    }

   G4cout << " Filling Root Event " << G4endl;

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
     FillRootEvent(event_id,
		   jhfNtuple,
		   trajectoryContainer,
		   WCDC_hits,
		   WCDC);
   }
  
  
  FillFlatTree(event_id,
	       jhfNtuple,
	       trajectoryContainer,
	       WCDC_hits,
	       WCDC);

  //save DAQ options here. This ensures that when the user selects a default option
  // (e.g. with -99), the saved option value in the output reflects what was run
  if(!SavedOptions) {
    WCSimRootOptions * wcsimopt = runAction->GetRootOptions();
    //Dark noise
    WCDNM->SaveOptionsToOutput(wcsimopt);
    //Digitizer
    WCDM->SaveOptionsToOutput(wcsimopt);
    //Trigger
    WCTM->SaveOptionsToOutput(wcsimopt);
    //Generator
    generatorAction->SaveOptionsToOutput(wcsimopt);
    
    SavedOptions = true;
  }
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
				     const struct ntupleStruct& jhfNtuple,
				     G4TrajectoryContainer* TC,
				     WCSimWCDigitsCollection* WCDC_hits,
				     WCSimWCTriggeredDigitsCollection* WCDC)
{
  // Fill up a Root event with stuff from the ntuple

  WCSimRootEvent* wcsimrootsuperevent = GetRunAction()->GetRootEvent();

  // start with the first "sub-event"
  // if the WC digitization requires it, we will add another subevent
  // for the WC.
  // all the rest goes into the first "sub-event".
  WCSimRootTrigger* wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  // get number of gates
  G4DigiManager* DMman = G4DigiManager::GetDMpointer();
  WCSimWCTriggerBase* WCTM =
    (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout");
  int ngates = WCTM->NumberOfGatesInThisEvent(); 
  G4cout << "ngates =  " << ngates << "\n";
  for (int index = 0 ; index < ngates ; index++) 
    {
      if (index >=1 ) {
	wcsimrootsuperevent->AddSubEvent();
	wcsimrootevent = wcsimrootsuperevent->GetTrigger(index);
	wcsimrootevent->SetHeader(event_id,0,
				   0,index+1); // date & # of subevent 
	wcsimrootevent->SetMode(jhfNtuple.mode);
      }
      wcsimrootevent->SetTriggerInfo(WCTM->GetTriggerType(index),
				     WCTM->GetTriggerInfo(index));
    }
  

  // Fill the header
  // Need to add run and date
  wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  wcsimrootevent->SetHeader(event_id,0,0); // will be set later.

  // Fill other info for this event

  wcsimrootevent->SetMode(jhfNtuple.mode);
  wcsimrootevent->SetVtxvol(jhfNtuple.vtxvol);
  for (int j=0;j<3;j++)
  {
    wcsimrootevent->SetVtx(j,jhfNtuple.vtx[j]);
  }
  wcsimrootevent->SetJmu(jhfNtuple.jmu);           //TF: undefined and only for Nuance
  wcsimrootevent->SetJp(jhfNtuple.jp);             //TF: undefined and only for Nuance
  wcsimrootevent->SetNpar(jhfNtuple.npar);         //TF: undefined
  wcsimrootevent->SetVecRecNumber(jhfNtuple.vecRecNumber);  //TF: undefined and only for Nuance

  // Add the tracks with the particle information
  // First two tracks come from jhfNtuple, as they are special

  int k;
  for (k=0;k<2;k++) // should be just 2
  {
    float dir[3];
    float pdir[3];
    float stop[3];
    float start[3];
    for (int l=0;l<3;l++)
    {
      dir[l]=jhfNtuple.dir[k][l];
      pdir[l]=jhfNtuple.pdir[k][l];
      stop[l]=jhfNtuple.stop[k][l];
      start[l]=jhfNtuple.start[k][l];
      //G4cout<< "start[" << k << "][" << l <<"]: "<< jhfNtuple.start[k][l] <<G4endl;
      //G4cout<< "stop[" << k << "][" << l <<"]: "<< jhfNtuple.stop[k][l] <<G4endl;
    }

    // Add the track to the TClonesArray
    wcsimrootevent->AddTrack(jhfNtuple.ipnu[k], 
			      jhfNtuple.flag[k], 
			      jhfNtuple.m[k], 
			      jhfNtuple.p[k], 
			      jhfNtuple.E[k], 
			      jhfNtuple.startvol[k], 
			      jhfNtuple.stopvol[k], 
			      dir, 
			      pdir, 
			      stop,
			      start,
			      jhfNtuple.parent[k],
			     jhfNtuple.time[k],0); 
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
    Float_t pi0Vtx[3];
    Int_t   gammaID[2];
    Float_t gammaE[2];
    Float_t gammaVtx[2][3];
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
       
    
    // Process primary tracks or the secondaries from pizero or muons...

    if ( trj->GetSaveFlag() )
    {
      // initial point of the trajectory
      G4TrajectoryPoint* aa =   (G4TrajectoryPoint*)trj->GetPoint(0) ;   
      runAction->incrementEventsGenerated();
	
      G4int         ipnu   = trj->GetPDGEncoding();
      G4int         id     = trj->GetTrackID();
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
      float dir[3];
      float pdir[3];
      float stop[3];
      float start[3];
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
      if ( trj->GetCreatorProcessName()=="nCapture" ?
           detectorConstructor->SaveCaptureInfo() :
           ! ( (ipnu==22)&&(parentType==999)) ) {
          int choose_event = 0;

          if (ngates) {

              if (ttime > WCTM->GetTriggerTime(0) + 950. && WCTM->GetTriggerTime(1) + 950. > ttime) choose_event = 1;
              if (ttime > WCTM->GetTriggerTime(1) + 950. && WCTM->GetTriggerTime(2) + 950. > ttime) choose_event = 2;
              if (choose_event >= ngates) choose_event = ngates - 1; // do not overflow the number of events

          }

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
                                   ttime, id);
      }
      

      if (detectorConstructor->SavePi0Info())
      {
	G4cout<<"Pi0 parentType: " << parentType <<G4endl;
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
    }
  }

  // Add the Cherenkov hits
  wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

  wcsimrootevent->SetNumTubesHit(jhfNtuple.numTubesHit);

  std::vector<WCSimPmtInfo*> *fpmts = detectorConstructor->Get_Pmts();
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
    std::vector<float> truetime, smeartime;
    std::vector<int>   primaryParentID;
    double hit_time_smear, hit_time_true;
    int hit_parentid;
    //loop over the DigitsCollection
    for(int idigi = 0; idigi < WCDC_hits->entries(); idigi++) {
      int digi_tubeid = (*WCDC_hits)[idigi]->GetTubeID();
      WCSimPmtInfo *pmt = ((WCSimPmtInfo*)fpmts->at(digi_tubeid -1));

      for(G4int id = 0; id < (*WCDC_hits)[idigi]->GetTotalPe(); id++){
	hit_time_true  = (*WCDC_hits)[idigi]->GetPreSmearTime(id);
	hit_parentid = (*WCDC_hits)[idigi]->GetParentID(id);
	truetime.push_back(hit_time_true);
	primaryParentID.push_back(hit_parentid);
#ifdef _SAVE_RAW_HITS_VERBOSE
	hit_time_smear = (*WCDC_hits)[idigi]->GetTime(id);
	smeartime.push_back(hit_time_smear);
#endif
      }//id
#ifdef _SAVE_RAW_HITS_VERBOSE
      if(digi_tubeid < NPMTS_VERBOSE) {
	G4cout << "Adding " << truetime.size()
	       << " Cherenkov hits in tube " << digi_tubeid
	       << " with truetime:smeartime:primaryparentID";
	for(G4int id = 0; id < truetime.size(); id++) {
	  G4cout << " " << truetime[id]
		 << ":" << smeartime[id]
		 << ":" << primaryParentID[id];
	}//id
	G4cout << G4endl;
      }
#endif
      wcsimrootevent->AddCherenkovHit(digi_tubeid,
				      pmt->Get_mPMTid(),
				      pmt->Get_mPMT_pmtid(),
				      truetime,
				      primaryParentID);
      smeartime.clear();
      truetime.clear();
      primaryParentID.clear();
    }//idigi
  }//if(WCDC_hits)
#endif //_SAVE_RAW_HITS

  // Add the digitized hits

  if (WCDC) 
  {
#ifdef SAVE_DIGITS_VERBOSE
    G4cout << "DIGI HITS" << G4endl;
#endif

    G4float sumq_tmp = 0.;
    
    for ( int index = 0 ; index < ngates ; index++)
      {	
	sumq_tmp = 0.0;	
	G4float gatestart;
	int countdigihits = 0;
	wcsimrootevent = wcsimrootsuperevent->GetTrigger(index);
	for (k=0;k<WCDC->entries();k++)
	  {
	    if ( (*WCDC)[k]->HasHitsInGate(index)) {
	      std::vector<float> vec_pe                  = (*WCDC)[k]->GetPe(index);
	      std::vector<float> vec_time                = (*WCDC)[k]->GetTime(index);
	      std::vector<std::vector<int> > vec_digicomp = (*WCDC)[k]->GetDigiCompositionInfo(index);
	      const int tubeID                           = (*WCDC)[k]->GetTubeID();
	      WCSimPmtInfo *pmt = ((WCSimPmtInfo*)fpmts->at(tubeID -1));
	      assert(vec_pe.size() == vec_time.size());
	      assert(vec_pe.size() == vec_digicomp.size());
	      for(unsigned int iv = 0; iv < vec_pe.size(); iv++) {
#ifdef SAVE_DIGITS_VERBOSE
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
    G4cout << ">>>Root event "
	   <<std::setw(5)<<wcsimrootevent->GetHeader()->GetEvtNum()<<"\n";
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
  
  TTree* tree = GetRunAction()->GetTree();
  tree->Fill();


  // Check we are supposed to be saving the NEUT vertex and that the generator was given a NEUT vector file to process
  // If there is no NEUT vector file an empty NEUT vertex will be written to the output file
  if(GetRunAction()->GetSaveRooTracker() && generatorAction->IsUsingRootrackerEvtGenerator()){
      GetRunAction()->ClearRootrackerVertexArray();
      generatorAction->CopyRootrackerVertex(GetRunAction()->GetRootrackerVertex()); //will increment NVtx 
      GetRunAction()->FillRootrackerVertexTree();
  }


  TFile* hfile = tree->GetCurrentFile();
  hfile->cd();                    // make sure tree is ONLY written to CurrentFile and not to all files!
  // MF : overwrite the trees -- otherwise we have as many copies of the tree
  // as we have events. All the intermediate copies are incomplete, only the
  // last one is useful --> huge waste of disk space.
  tree->Write("",TObject::kOverwrite);

  // M Fechner : reinitialize the super event after the writing is over
  wcsimrootsuperevent->ReInitialize();
  
}



void WCSimEventAction::FillFlatTree(G4int event_id,
				    const struct ntupleStruct& jhfNtuple,  //hopefully deprecate this soon
				    G4TrajectoryContainer* TC,
				    WCSimWCDigitsCollection* WCDC_hits,
				    WCSimWCTriggeredDigitsCollection* WCDC){
  
  // For each event : primary event info and tracks are the same
  // The hits are triggers can be different per sub-event
  // First deal with common event-wise quantities:
  
  //point branched struct to the one filled here. 
  eventNtuple *thisNtuple = GetRunAction()->GetMyStruct(); 
  thisNtuple->interaction_mode = jhfNtuple.mode;
  strcpy(thisNtuple->vtxVolume,vtxVolumeName.c_str());  
  thisNtuple->vtx_x = jhfNtuple.vtx[0];
  thisNtuple->vtx_y = jhfNtuple.vtx[1];
  thisNtuple->vtx_z = jhfNtuple.vtx[2];  //now copying from jhfNuple, in the future replacing jhfNtuple
  
  // Add the tracks with the particle information : beam and target are special or also parents from 
  // trajectorylist.
 
   //Primary tracks are also in the tracjectoryList (don't know about target though)
  // => Maybe treat beam and target separately but for now, just grab all Primary Particles
  // ParentID == 0 from the trajectorylist
  thisNtuple->nTracks = 0;
  G4int n_trajectories = 0;
  if (TC)
    n_trajectories = TC->entries();

  for (int i=0; i <n_trajectories; i++) 
    {
      WCSimTrajectory* trj = (WCSimTrajectory*)(*TC)[i];
      // SaveFlag is defined in WCSimTrackingAction:
      // Currently: primaries (parentID == 0)
      // Products from decay (eg. Michel e-)
      // Particles: pi0, pi+-, kaons, NEW (protons and neutrons)
      // Gamma > 50 MeV, new mu+- > CherenkovThreshold
      // Add later?: https://twiki.cern.ch/twiki/bin/view/Geant4/LoweAtomicDeexcitation (neutron capture and O16)
      if(trj->GetSaveFlag() ){        	  
	// initial point of the trajectory
	G4TrajectoryPoint* init_pt =   (G4TrajectoryPoint*)trj->GetPoint(0) ;   
	runAction->incrementEventsGenerated();       // Why??
	
	thisNtuple->pid[thisNtuple->nTracks]     = trj->GetPDGEncoding();
	//trj->GetTrackID();
	thisNtuple->flag[thisNtuple->nTracks]    = 0;    
	thisNtuple->mass[thisNtuple->nTracks]    = trj->GetParticleDefinition()->GetPDGMass();
	G4ThreeVector mom                        = trj->GetInitialMomentum();
	thisNtuple->p[thisNtuple->nTracks]       = mom.mag()/CLHEP::MeV;
	thisNtuple->pdir_x[thisNtuple->nTracks]  = mom[0];
	thisNtuple->pdir_y[thisNtuple->nTracks]  = mom[1];
	thisNtuple->pdir_z[thisNtuple->nTracks]  = mom[2];
	thisNtuple->dir_x[thisNtuple->nTracks]   = mom[0]/mom.mag();
	thisNtuple->dir_y[thisNtuple->nTracks]   = mom[1]/mom.mag();
	thisNtuple->dir_z[thisNtuple->nTracks]   = mom[2]/mom.mag();
	
	thisNtuple->energy[thisNtuple->nTracks]  = 
	  sqrt(mom.mag2() + thisNtuple->mass[thisNtuple->nTracks]*thisNtuple->mass[thisNtuple->nTracks])/CLHEP::MeV;
	
	G4ThreeVector Start                      = init_pt->GetPosition();
	thisNtuple->start_x[thisNtuple->nTracks] = Start[0]/CLHEP::cm;
	thisNtuple->start_y[thisNtuple->nTracks] = Start[1]/CLHEP::cm;
	thisNtuple->start_z[thisNtuple->nTracks] = Start[2]/CLHEP::cm;
	thisNtuple->startvol[thisNtuple->nTracks]= WCSimEventFindStartingVolume(Start);
	//G4String startVolumeName;               
	G4ThreeVector Stop                       = trj->GetStoppingPoint();
	thisNtuple->stop_x[thisNtuple->nTracks]  = Stop[0]/CLHEP::cm;
	thisNtuple->stop_y[thisNtuple->nTracks]  = Stop[1]/CLHEP::cm;
	thisNtuple->stop_z[thisNtuple->nTracks]  = Stop[2]/CLHEP::cm;
	G4String stopVolumeName                  = trj->GetStoppingVolume()->GetName();
	thisNtuple->stopvol[thisNtuple->nTracks] = WCSimEventFindStoppingVolume(stopVolumeName);
	
	thisNtuple->time[thisNtuple->nTracks]    = trj->GetGlobalTime(); 
	thisNtuple->parent[thisNtuple->nTracks]  = trj->GetParentID();
	thisNtuple->trackID[thisNtuple->nTracks]  = trj->GetTrackID();
	
	//myDetector has some more info for distance calculations!
	thisNtuple->length[thisNtuple->nTracks]  = (Stop - Start).mag()/CLHEP::cm;
	
	/* ToDo
	   thisNtuple->distStartWall[thisNtuple->nTracks]
	   = ;
	   thisNtuple->distStopWall[thisNtuple->nTracks]
	   = ;
	*/
	thisNtuple->nTracks++;
      }
    }// end loop over trajectories
  
  
  // Add the Cherenkov hits
  int nMpmtID_pmts = detectorConstructor->GetmPMT_nID();

#ifdef _SAVE_RAW_HITS

  // Num Tubes hit : WCDC_hits ->entries()
  // Num Tubes hit wo noise: count myself
  // Total charge with noise
  // Total charge without noise
  int totNumHits = 0;
  int totNumHits_noDN = 0;
  int numTubesHit_noDN= 0;

  // Easier to grab PMT orientation from detector constructor than from G4RotationMatrix
  std::vector<WCSimPmtInfo*> *fpmts = detectorConstructor->Get_Pmts();
  

  if (WCDC_hits) 
  {
    //add the truth raw hits
    G4cout << " RAW HITS " << G4endl;

    //loop over the DigitsCollection
    // TF: whole loop is deprecated IF parentID is filled for Noise Hits
    //     which it is now. Also a THitsMap is easier, but as all info is already
    //     passed on to the DigitsCollection, not required.

    for(int idigi = 0; idigi < WCDC_hits->entries(); idigi++) {
      int digi_tubeid = (*WCDC_hits)[idigi]->GetTubeID();
      int numNoiseHits = 0;
#ifdef _SAVE_RAW_HITS_VERBOSE
      G4cout << "=========================================================================" << G4endl;
      G4cout << "TubeID " << digi_tubeid << " (TrackID " << (*WCDC_hits)[idigi]->GetTrackID() << 
	") has " << (*WCDC_hits)[idigi]->GetTotalPe() << " CherenkovHits (Dark noise included)" << G4endl;
#endif
      
      WCSimPmtInfo *pmt = ((WCSimPmtInfo*)fpmts->at(digi_tubeid -1));

      for(G4int id = 0; id < (*WCDC_hits)[idigi]->GetTotalPe(); id++){
	float digi_time = (*WCDC_hits)[idigi]->GetTime(id);
	int hit_parentid = (*WCDC_hits)[idigi]->GetParentID(id);
#ifdef _SAVE_RAW_HITS_VERBOSE
	G4cout << "Hit " << id << " with time " << digi_time << " and parentID " << hit_parentid << G4endl;
#endif
      /* I want to know for each HIT : 
	 time, tubeID, parentID, pmt_x, pmt_y, pmt_z, 
	 pmt_dirX, dirY, dirZ, trackID
      */
	thisNtuple->truetime[totNumHits] = digi_time;
	thisNtuple->totalPe[totNumHits] = (*WCDC_hits)[idigi]->GetTotalPe();
	thisNtuple->parentid[totNumHits] = hit_parentid;
	thisNtuple->vector_index[totNumHits] = id;
	thisNtuple->tubeid[totNumHits] = digi_tubeid;
	thisNtuple->mPMTid[totNumHits] = digi_tubeid/nMpmtID_pmts;
 	thisNtuple->mPMT_pmtid[totNumHits] = (digi_tubeid%nMpmtID_pmts == 0 ? nMpmtID_pmts : digi_tubeid%nMpmtID_pmts ); // No. 1 to nID

	thisNtuple->trackid[totNumHits] = (*WCDC_hits)[idigi]->GetTrackID();
	G4ThreeVector pos = (*WCDC_hits)[idigi]->GetPos();       // Can also grab it from theDetector also.
	thisNtuple->tube_x[totNumHits] = pos[0];                 //already in CLHEP::cm
	thisNtuple->tube_y[totNumHits] = pos[1];
	thisNtuple->tube_z[totNumHits] = pos[2];
	thisNtuple->tube_dirx[totNumHits] = pmt->Get_orienx();  //Can now also grab the ThreeVector Orientation from WCDigi
	thisNtuple->tube_diry[totNumHits] = pmt->Get_orieny();
	thisNtuple->tube_dirz[totNumHits] = pmt->Get_orienz();
	totNumHits++;
	if(hit_parentid > 0){
	  totNumHits_noDN++;
	  
	} else if(hit_parentid == -1)
	  numNoiseHits ++;
      }
      if (numNoiseHits < (*WCDC_hits)[idigi]->GetTotalPe())
	numTubesHit_noDN++;
      //Subtract noiseHits from realHits:
      for(G4int id = totNumHits; id > (totNumHits-(*WCDC_hits)[idigi]->GetTotalPe()); id--){
	thisNtuple->totalPe_noNoise[id-1] = (*WCDC_hits)[idigi]->GetTotalPe() - numNoiseHits; //id-1 because I already did totNumHits++ above for next PMT
      }
    }//idigi
  }
  thisNtuple->numTubesHit = WCDC_hits->entries();
  thisNtuple->numTubesHit_noNoise= numTubesHit_noDN;
  thisNtuple->totalNumHits = totNumHits;
  thisNtuple->totalNumHits_noNoise = totNumHits_noDN;
#endif //_SAVE_RAW_HITS
    
  // Setup Sub-events: only for Trigger and Digitized info. Everything else has no relation to the trigger
  // Dubious concept of Main Sub-event gone now: common non-subevent specific info shared between subevents!
  // --> flat structure!
  // get number of gates
  G4DigiManager* DMman = G4DigiManager::GetDMpointer();
  WCSimWCTriggerBase* WCTM =
    (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout");
  int ngates = WCTM->NumberOfGatesInThisEvent(); 
  G4cout << "ngates =  " << ngates << "\n";


  // nGates == 0: I still want to keep untriggered event
  if(ngates == 0){
    GetRunAction()->SetEventHeaderNew(0,event_id+1,1);   //ToDo: run 
    //std::cout << event_id << std::endl; //TF debug
    //General case for a vector triggerInfo:
    //GetRunAction()->SetTriggerInfoNew(kTriggerUndefined, std::vector<G4float>(),0.,0.);

    // TriggerInfo vector has one element (see WCDigitizer.cc and WCTrigger.cc), so just store ints
    // => should be better explained why it is std::vector<float> while I would just store an int.
    GetRunAction()->SetTriggerInfoNew(kTriggerUndefined,0,0.,0.);
    
    // Fill Tree for each subevent
    GetRunAction()->GetTracksTree()->Fill();
    GetRunAction()->GetCherenkovHitsTree()->Fill();
    GetRunAction()->GetCherenkovDigiHitsTree()->Fill();
    GetRunAction()->GetTriggerTree()->Fill();
    GetRunAction()->GetEventInfoTree()->Fill();
    if(GetRunAction()->GetSaveRooTracker() && generatorAction->IsUsingRootrackerEvtGenerator()){
      generatorAction->CopyRootrackerVertex(GetRunAction()->GetRootrackerVertex());
      GetRunAction()->GetFlatRooTrackerTree()->Fill();
    }
    
  }
  

  for (int index = 0 ; index < ngates ; index++) {
    //WCSim (FillRootEvent) counts its sub-events from 1 to nGate, while counting events from 0 to n-1
    //Be consistent and start both from 1 here:
    GetRunAction()->SetEventHeaderNew(0,event_id+1,index+1);   //ToDo: run
    std::cout << event_id << std::endl;

    //First Trigger details of THIS subevent (index+1)
    // Fixed 950ns ? Not a setting in WCTrigger?
    // Gatestart == triggerTime, used to be stored as "date" in eventHeader (rather irrelevant for MC)
    GetRunAction()->SetTriggerInfoNew(WCTM->GetTriggerType(index),
				      WCTM->GetTriggerInfo(index)[0],950.,WCTM->GetTriggerTime(index));
    

    
    // Add the digitized hits
    if (WCDC) {
      G4float sumq_tmp = 0.;
      int countdigihits = 0;

      // An entry per Tube with hits per Trigger
      for (int k = 0; k < WCDC->entries() ; k++) {
	if ( (*WCDC)[k]->HasHitsInGate(index)) {
	  std::vector<float> vec_pe                   = (*WCDC)[k]->GetPe(index);
	  std::vector<float> vec_time                 = (*WCDC)[k]->GetTime(index);
	  std::vector<std::vector<int> > vec_digicomp = (*WCDC)[k]->GetDigiCompositionInfo(index);
	  const int tubeID                            = (*WCDC)[k]->GetTubeID();
	  WCSimPmtInfo *pmt = ((WCSimPmtInfo*)fpmts->at(tubeID -1));
	  assert(vec_pe.size() == vec_time.size());
	  assert(vec_pe.size() == vec_digicomp.size());
	  
	  for(unsigned int iv = 0; iv < vec_pe.size(); iv++) {                     // TF: so far haven't seen iv > 0 yet. Delayed hits? 
#ifdef SAVE_DIGITS_VERBOSE
	    G4cout << "Adding digit " << iv  << " for PMT " << tubeID
		   << " pe "   << vec_pe[iv]  << " time " << vec_time[iv] << " digicomp";
	    for(unsigned int ivv = 0; ivv < vec_digicomp[iv].size(); ivv++)
	      G4cout << " " << vec_digicomp[iv][ivv];
	    G4cout << G4endl;
#endif
	    assert(vec_digicomp[iv].size() > 0);
	    
	    // Note : once CPU time becomes an issue: grab these once outside this loop
	    //        from theDetector and store only here, however, iv > 0 is rare!!
	    thisNtuple->digitubeid[countdigihits] = tubeID;
	    thisNtuple->digimPMTid[countdigihits] = tubeID/nMpmtID_pmts;
	    thisNtuple->digimPMT_pmtid[countdigihits] = (tubeID%nMpmtID_pmts == 0 ? nMpmtID_pmts : tubeID%nMpmtID_pmts ); // No. 1 to nID
	    thisNtuple->digitube_x[countdigihits] = pmt->Get_transx();        //already in CLHEP::cm
	    thisNtuple->digitube_y[countdigihits] = pmt->Get_transy();
	    thisNtuple->digitube_z[countdigihits] = pmt->Get_transz();
	    thisNtuple->digitube_dirx[countdigihits] = pmt->Get_orienx();  
	    thisNtuple->digitube_diry[countdigihits] = pmt->Get_orieny();
	    thisNtuple->digitube_dirz[countdigihits] = pmt->Get_orienz();
	    thisNtuple->digivector_index[countdigihits] = iv;                
	    thisNtuple->q[countdigihits] = vec_pe[iv];
	    thisNtuple->t[countdigihits] = vec_time[iv];
	    sumq_tmp += vec_pe[iv];

	    /// vec_digicomp[iv]);                            ToDo if interesting at some point
	    countdigihits++;
	  }//iv
	}//Digit exists in Gate
      }//k
      thisNtuple->totalNumDigiHits  = countdigihits;
      thisNtuple->numDigiTubesHit  = WCDC->entries();
      thisNtuple->sumq             = sumq_tmp;
    }//end WCDC

    // Fill Tree for each subevent
    GetRunAction()->GetTracksTree()->Fill();
    GetRunAction()->GetCherenkovHitsTree()->Fill();
    GetRunAction()->GetCherenkovDigiHitsTree()->Fill();
    GetRunAction()->GetTriggerTree()->Fill();
    GetRunAction()->GetEventInfoTree()->Fill();
    // Check we are supposed to be saving the NEUT vertex and that the generator was given a NEUT vector file to process
    // If there is no NEUT vector file an empty NEUT vertex will be written to the output file
    if(GetRunAction()->GetSaveRooTracker() && generatorAction->IsUsingRootrackerEvtGenerator()){
      NRooTrackerVtx *thisRooTracker = GetRunAction()->GetMyRooTracker();
      generatorAction->CopyRootrackerVertex(thisRooTracker);

      std::cout << "Test: " << thisRooTracker->NuParentDecMode << std::endl;
      GetRunAction()->GetFlatRooTrackerTree()->Fill(); //ok
    }

  }



}

