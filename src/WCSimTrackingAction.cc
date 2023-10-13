#include "WCSimTrackingAction.hh"
#include "WCSimTrajectory.hh"
#include "G4ParticleTypes.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4ios.hh"
#include "G4VProcess.hh"
#include "G4RunManager.hh"
#include "WCSimTrackInformation.hh"
#include "WCSimTrackingMessenger.hh"
#include "WCSimPrimaryGeneratorAction.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

WCSimTrackingAction::WCSimTrackingAction()
{

  ProcessList.insert("Decay") ;                         // Michel e- from pi+ and mu+
  ProcessList.insert("conv") ;                         // Products of gamma conversion

  //ProcessList.insert("muMinusCaptureAtRest") ;          // Includes Muon decay from K-shell: for Michel e- from mu0. This dominates/replaces the mu- decay (noticed when switching off this process in PhysicsList)                                                   // TF: IMPORTANT: ONLY USE FROM G4.9.6 onwards because buggy/double counting before.
  ////////// ToDo: switch ON the above when NuPRISM uses G4 >= 4.9.6
  ProcessList.insert("nCapture");

//   ProcessList.insert("conv");

  // F. Nova One can check here if the photon comes from WLS
  ProcessList.insert("OpWLS");

  ParticleList.insert(111); // pi0
  ParticleList.insert(211); // pion+
  ParticleList.insert(-211);
  ParticleList.insert(321);
  ParticleList.insert(-321); // kaon-
  ParticleList.insert(311); // kaon0
  ParticleList.insert(-311); // kaon0 bar
  //ParticleList.insert(22); // I add photons (B.Q)
  ParticleList.insert(11); // e-
  ParticleList.insert(-11); // e+
  ParticleList.insert(13); // mu-
  ParticleList.insert(-13); // mu+
  // don't put gammas there or there'll be too many

  //TF: add protons and neutrons
  ParticleList.insert(2212);
  ParticleList.insert(2112);

  percentageOfCherenkovPhotonsToDraw = 0.0;

  messenger = new WCSimTrackingMessenger(this);

  // Max time for radioactive decay:
  fMaxTime    = 1. * CLHEP::second;
  fTime_birth = 0.;
}

WCSimTrackingAction::~WCSimTrackingAction(){;}

void WCSimTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  //TF: userdefined now
  //G4double percentageOfCherenkovPhotonsToDraw = 100.0;
  // if larger than zero, will keep trajectories of many secondaries as well
  // and store them in output file. Difficult to control them all, so best only
  // use for visualization, not for storing in ROOT.

  if ( aTrack->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()
       || G4UniformRand() < percentageOfCherenkovPhotonsToDraw/100. )
    {
      WCSimTrajectory* thisTrajectory = new WCSimTrajectory(aTrack);
      fpTrackingManager->SetTrajectory(thisTrajectory);
      fpTrackingManager->SetStoreTrajectory(true);
    }
  else 
    fpTrackingManager->SetStoreTrajectory(false);
	
  WCSimPrimaryGeneratorAction *primaryGenerator = (WCSimPrimaryGeneratorAction *) (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  if(!primaryGenerator->IsConversionFound()) {
    if(aTrack->GetParentID()==0){
      primaryID = aTrack->GetTrackID();
    }
    else if(aTrack->GetParentID() == primaryID) {
      if (aTrack->GetCreatorProcess()->GetProcessName() == "conv") {
          primaryGenerator->FoundConversion();
      }
      G4EventManager::GetEventManager()->AbortCurrentEvent();
      G4EventManager::GetEventManager()->GetNonconstCurrentEvent()->SetEventAborted();
    }
  }

  // Kill nucleus generated after TrackID 1
  G4ParticleDefinition* particle = aTrack->GetDefinition();
  G4String name   = particle->GetParticleName();
  G4double fCharge = particle->GetPDGCharge();

  G4Track* tr = (G4Track*) aTrack;
  if ( aTrack->GetTrackID() == 1 ) {
  	// Re-initialize time
  	fTime_birth = 0;
  	// Ask G4 to kill the track when all secondary are done (will exclude other decays)
  	if ( fCharge > 2. )
  		tr->SetTrackStatus(fStopButAlive);
  }

  if ( aTrack->GetTrackID() == 2 ) {
  	// First track of the decay save time
  	fTime_birth = aTrack->GetGlobalTime(); 
  }
}

void WCSimTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
  // added by M Fechner
  const G4VProcess* creatorProcess = aTrack->GetCreatorProcess();
  //  if ( creatorProcess )


  WCSimTrackInformation* anInfo;
  if (aTrack->GetUserInformation())
    anInfo = (WCSimTrackInformation*)(aTrack->GetUserInformation());   //eg. propagated to all secondaries blelow.
  else anInfo = new WCSimTrackInformation();

    // Simplified tracking: track all primaries, particles from the chosen list of particle types or creation processes,
    // Optionally, all particles producing cherekov hits and their parents, grandparents, etc. will be added later.

    // is it a primary ?
    // is the process in the set ? 
    // is the particle in the set ?
    if( aTrack->GetParentID()==0
	|| ((creatorProcess!=0) && ProcessList.count(creatorProcess->GetProcessName()))
	|| (ParticleList.count(aTrack->GetDefinition()->GetPDGEncoding()))
      )
    {
      // if so the track is worth saving
      anInfo->WillBeSaved(true);
    }
  else {
    anInfo->WillBeSaved(false);
  }


  G4Track* theTrack = (G4Track*)aTrack;
  theTrack->SetUserInformation(anInfo);

  // pass primary parent ID to children
  G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
  WCSimTrajectory *currentTrajectory = (WCSimTrajectory*)fpTrackingManager->GimmeTrajectory();
  if(!anInfo->GetMyTrajectory())
    anInfo->SetMyTrajectory(currentTrajectory);
  if(secondaries)
  {
    size_t nSeco = secondaries->size();
    if(nSeco>0)
    {
      for(size_t i=0;i<nSeco;i++)
      { 
	WCSimTrackInformation* infoSec = new WCSimTrackInformation(anInfo);
	if(anInfo->isSaved()){ // Parent is saved, so store pointer to its trajectory in the secondary's user info
        infoSec->SetParentTrajectory(anInfo->GetMyTrajectory());
	}
	infoSec->WillBeSaved(false); // ADDED BY MFECHNER, temporary, 30/8/06
	(*secondaries)[i]->SetUserInformation(infoSec);
      }
    } 
  }

  // If this track produces a hit, traverse back through parent trajectories to flag that the parents produce a hit
  if (anInfo->GetProducesHit() && saveHitProducingTracks){
      WCSimTrajectory* parentTrajectory = anInfo->GetParentTrajectory();
      while(parentTrajectory != 0 && !parentTrajectory->GetProducesHit()){
          parentTrajectory->SetProducesHit(true);
          parentTrajectory = parentTrajectory->GetParentTrajectory();
      }
  }

  if ( aTrack->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition() )
    //   if (aTrack->GetDefinition()->GetPDGCharge() == 0) 
  {

    G4ThreeVector currentPosition      = aTrack->GetPosition();
    G4VPhysicalVolume* currentVolume   = aTrack->GetVolume();

    currentTrajectory->SetStoppingPoint(currentPosition);
    currentTrajectory->SetStoppingVolume(currentVolume);

    currentTrajectory->SetSaveFlag(anInfo->isSaved());// mark it for WCSimEventAction ;
    currentTrajectory->SetProducesHit(anInfo->GetProducesHit());
  }
	
  WCSimPrimaryGeneratorAction *primaryGenerator = (WCSimPrimaryGeneratorAction *) (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  if(!primaryGenerator->IsConversionFound() && 
     aTrack->GetTrackID() == primaryID &&
     aTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep() &&
     aTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "conv"){
      for(int i=0; i<2; i++){
          primaryGenerator->SetConversionProductParticle(i, secondaries->at(i)->GetParticleDefinition());
          primaryGenerator->SetConversionProductMomentum(i, secondaries->at(i)->GetMomentum());
      }
  }
}




