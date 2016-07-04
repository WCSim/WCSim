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
   ConstructedDAQClasses(false)
{
  DAQMessenger = new WCSimWCDAQMessenger(this);

  G4DigiManager* DMman = G4DigiManager::GetDMpointer();

  //create PMT response module
  WCSimWCPMT* WCDMPMT = new WCSimWCPMT( "WCReadoutPMT", myDetector);
  DMman->AddNewModule(WCDMPMT);

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
  if(!ConstructedDAQClasses)
    CreateDAQInstances();
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
  
  // ----------------------------------------------------------------------
  //  Get Event Information
  // ----------------------------------------------------------------------

  G4int         event_id = evt->GetEventID();
  G4int         mode     = generatorAction->GetMode();
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
   

  FillRootEvent(event_id,
		jhfNtuple,
		trajectoryContainer,
		WCDC_hits,
		WCDC);

}

G4int WCSimEventAction::WCSimEventFindStartingVolume(G4ThreeVector vtx)
{
  // Get volume of starting point (see GEANT4 FAQ)

  G4int vtxvol = -1;

  G4Navigator* tmpNavigator = 
    G4TransportationManager::GetTransportationManager()->
    GetNavigatorForTracking();

  G4VPhysicalVolume* tmpVolume = tmpNavigator->LocateGlobalPointAndSetup(vtx);
  G4String       vtxVolumeName = tmpVolume->GetName();



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
  wcsimrootevent->SetJmu(jhfNtuple.jmu);
  wcsimrootevent->SetJp(jhfNtuple.jp);
  wcsimrootevent->SetNpar(jhfNtuple.npar);
  wcsimrootevent->SetVecRecNumber(jhfNtuple.vecRecNumber);

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
	G4cout<< "start[" << k << "][" << l <<"]: "<< jhfNtuple.start[k][l] <<G4endl;
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
      //	     << id << " " << energy << "\n";

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
	G4cout<<"part 2 start["<<l<<"]: "<< start[l] <<G4endl;
      }


      // Add the track to the TClonesArray, watching out for times
      if ( ! ( (ipnu==22)&&(parentType==999))  ) {
	int choose_event=0;

	if (ngates)
	{

	  if ( ttime > WCTM->GetTriggerTime(0)+950. && WCTM->GetTriggerTime(1)+950. > ttime ) choose_event=1; 
	  if ( ttime > WCTM->GetTriggerTime(1)+950. && WCTM->GetTriggerTime(2)+950. > ttime ) choose_event=2; 
	  if (choose_event >= ngates) choose_event = ngates-1; // do not overflow the number of events
	
	}

	wcsimrootevent= wcsimrootsuperevent->GetTrigger(choose_event);
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
				 ttime,id); 
      }
      

      if (detectorConstructor->SavePi0Info() == true)
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
    }
  }

  // Add the Cherenkov hits
  wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

  wcsimrootevent->SetNumTubesHit(jhfNtuple.numTubesHit);

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
						    tubeID, vec_digicomp[iv]);
		sumq_tmp += vec_pe[iv];
		countdigihits++;
	      }//iv
	    }//Digit exists in Gate
	  }//k
	wcsimrootevent->SetNumDigitizedTubes(countdigihits);
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
  TFile* hfile = tree->GetCurrentFile();
  // MF : overwrite the trees -- otherwise we have as many copies of the tree
  // as we have events. All the intermediate copies are incomplete, only the
  // last one is useful --> huge waste of disk space.
  hfile->Write("",TObject::kOverwrite);
  
  // M Fechner : reinitialize the super event after the writing is over
  wcsimrootsuperevent->ReInitialize();
  
}
