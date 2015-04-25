#include "WCSimEventAction.hh"
#include "WCSimTrajectory.hh"
#include "WCSimRunAction.hh"
#include "WCSimPrimaryGeneratorAction.hh"
#include "WCSimWCHit.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCDigitizer.hh"
#include "WCSimWCDigitizerBase.hh"
#include "WCSimWCDigitizerSKIV.hh"
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

#define _SAVE_RAW_HITS


WCSimEventAction::WCSimEventAction(WCSimRunAction* myRun, 
				     WCSimDetectorConstruction* myDetector, 
				     WCSimPrimaryGeneratorAction* myGenerator)
  :runAction(myRun), generatorAction(myGenerator), 
   detectorConstructor(myDetector)
{
  G4DigiManager* DMman = G4DigiManager::GetDMpointer();
  WCSimWCPMT* WCDMPMT = new WCSimWCPMT( "WCReadoutPMT", myDetector);
  //  WCSimWCDigitizer* WCDM = new WCSimWCDigitizer( "WCReadout", myDetector);
  WCSimWCDigitizerSKIV* WCDM = new WCSimWCDigitizerSKIV( "WCReadout", myDetector);
  WCSimWCAddDarkNoise* WCDNM = new WCSimWCAddDarkNoise( "WCDarkNoise", myDetector); 
  DMman->AddNewModule(WCDMPMT);
  DMman->AddNewModule(WCDM);
  DMman->AddNewModule(WCDNM);
}

WCSimEventAction::~WCSimEventAction(){}

void WCSimEventAction::BeginOfEventAction(const G4Event*){}

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

  if (HCE)
  { 
    G4String name =   "glassFaceWCPMT";
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
  
  
  //  TStopwatch* ms = new TStopwatch();
  // ms->Start();
  
  //Convert the hits to PMT pulse
  WCDMPMT->Digitize();
 
  //We should probably add dark noise hits here
  //Add Dark noise hits before digitizing
  //Get a pointer to the WC Digitizer Module                                                                                                                                   
  WCSimWCAddDarkNoise* WCDNM =
    (WCSimWCAddDarkNoise*)DMman->FindDigitizerModule("WCDarkNoise");

  //clear old info inside the darknoise routine                                                                                                                                  
  WCDNM->ReInitialize();  

  WCDNM->AddDarkNoise();

  //Get a pointer to the WC Digitizer Module
  WCSimWCDigitizerSKIV* WCDM =
    (WCSimWCDigitizerSKIV*)DMman->FindDigitizerModule("WCReadout");
  
  //clear old info inside the digitizer
   WCDM->ReInitialize();
   
   // Figure out what size PMTs we are using in the WC detector.
   G4float PMTSize = detectorConstructor->GetPMTSize();
   WCDM->SetPMTSize(PMTSize);
   
   WCDM->Digitize();


   //ms->Stop();
   //std::cout << " Digtization :  Real = " << ms->RealTime() 
   //	    << " ; CPU = " << ms->CpuTime() << "\n";  
   
   // Get the digitized collection for the WC
   G4int WCDCID = DMman->GetDigiCollectionID("WCDigitizedCollection");
   WCSimWCDigitsCollection * WCDC = (WCSimWCDigitsCollection*) DMman->GetDigiCollection(WCDCID);
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
		WCHC,
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
				      WCSimWCHitsCollection* WCHC, 
				      WCSimWCDigitsCollection* WCDC)
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
  WCSimWCDigitizerSKIV* WCDM =
    (WCSimWCDigitizerSKIV*)DMman->FindDigitizerModule("WCReadout");
  int ngates = WCDM->NumberOfGatesInThisEvent(); 
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

	if ( ttime > WCDM->GetTriggerTime(0)+950. && WCDM->GetTriggerTime(1)+950. > ttime ) choose_event=1; 
	if ( ttime > WCDM->GetTriggerTime(1)+950. && WCDM->GetTriggerTime(2)+950. > ttime ) choose_event=2; 
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
  if (WCHC) 
  {
    G4cout<<"RAW HITS"<<G4endl;
    wcsimrootevent->SetNumTubesHit(WCHC->entries());
    for (k=0;k<WCHC->entries();k++){
    
      std::vector<float> truetime;
      std::vector<int>   primaryParentID;

      int tubeID  = (*WCHC)[k]->GetTubeID();
      int totalpe = (*WCHC)[k]->GetTotalPe();

      for (int l=0;l<totalpe;l++)
      {
	truetime.push_back((*WCHC)[k]->GetTime(l));
	primaryParentID.push_back((*WCHC)[k]->GetParentID(l));
      }

      wcsimrootevent->AddCherenkovHit(tubeID,
				      truetime,
				      primaryParentID); 
    } 
  }

#endif

  // Add the digitized hits

  if (WCDC) 
  {
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
	      wcsimrootevent->AddCherenkovDigiHit((*WCDC)[k]->GetPe(index),
						  (*WCDC)[k]->GetTime(index),
						  (*WCDC)[k]->GetTubeID());  
	      sumq_tmp = sumq_tmp + (*WCDC)[k]->GetPe(index);
	   
	      countdigihits++;
	      wcsimrootevent->SetNumDigitizedTubes(countdigihits);
	      wcsimrootevent->SetSumQ(sumq_tmp);
	    }
	  }
	/*
		G4cout << "checking digi hits ...\n";
	  G4cout << "hits collection size =  " << 
	  wcsimrootevent->GetCherenkovHits()->GetEntries() << "\n";
	  G4cout << "hits collection size =  " << 
	  wcsimrootevent->GetCherenkovHitTimes()->GetEntries() << "\n";
	  G4cout << "digihits collection size =  " << 
	  wcsimrootevent->GetCherenkovDigiHits()->GetEntries() << "\n";
	  G4cout << "tracks collection size =  " << 
	  wcsimrootevent->GetTracks()->GetEntries() 
	  <<" get ntracks = " <<  wcsimrootevent->GetNtrack() << "\n";
	*/
	gatestart = WCDM->GetTriggerTime(index);
	WCSimRootEventHeader*HH = wcsimrootevent->GetHeader();
	HH->SetDate(int(gatestart));
      }
    
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
