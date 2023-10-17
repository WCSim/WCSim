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
  ProcessList.insert("phot");
  ProcessList.insert("comp");
  ProcessList.insert("NeutronInelastic");

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
  
  ParticleList.insert(22);
  ParticleList.insert(11);
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

  /** TF's particle list (ToDo: discuss/converge)
  
   // is it a primary ?
  // is the process in the set ? eg. Michel e-, but only keep e-
  // is the particle in the set ? eg. pi0, pi+-, K, p, n
  // is it a gamma above 50 MeV ? OR gamma from nCapture? ToDo: Oxygen de-excitation
  // is it a muon that can still produce Cherenkov light?
  // due to lazy evaluation of the 'or' in C++ the order is important  
  if( aTrack->GetParentID()==0 ||                                                                            // Primary
      ((creatorProcess!=0) && ProcessList.count(creatorProcess->GetProcessName())                            
       && aTrack->GetMomentum().mag() > .5*MeV && abs(aTrack->GetDefinition()->GetPDGEncoding()) == 11) ||   
      (ParticleList.count(aTrack->GetDefinition()->GetPDGEncoding()) )                                       
      || (aTrack->GetDefinition()->GetPDGEncoding()==22 && aTrack->GetVertexKineticEnergy() > 4.*MeV)       // Bugfixed: need vertex kinetic energy for gamma, rest is zero. ToDo: check whether pi0 gamma's are saved!
      || (aTrack->GetDefinition()->GetPDGEncoding()==22 && creatorProcess->GetProcessName() == "nCapture")   
      || (abs(aTrack->GetDefinition()->GetPDGEncoding()== 13) && aTrack->GetMomentum().mag() > 110.0*MeV) //mu+- above Cherenkov Threshold in water (119 MeV/c)

  **/
    // TF: Currently use the nuPRISM one

    // is it a primary ?
    // is the process in the set ? 
    // is the particle in the set ?
    // is it a gamma above 1 MeV ?
    // is it a mu- capture at rest above 1 MeV ?
    // due to lazy evaluation of the 'or' in C++ the order is important
    if( aTrack->GetParentID()==0 
	|| ((creatorProcess!=0) && ProcessList.count(creatorProcess->GetProcessName()))
	|| (ParticleList.count(aTrack->GetDefinition()->GetPDGEncoding()))
	|| (aTrack->GetDefinition()->GetPDGEncoding()==22 && aTrack->GetTotalEnergy() > 1.0*MeV)
	|| (creatorProcess->GetProcessName() == "muMinusCaptureAtRest" && aTrack->GetTotalEnergy() > 1.0*MeV)
      )
    {
    // if so the track is worth saving
    anInfo->WillBeSaved(true);
    
    //      G4cout << "track # " << aTrack->GetTrackID() << " is worth saving\n";
    //      G4cout << "It is a " <<aTrack->GetDefinition()->GetParticleName() << G4endl;


    //For Ch hits: use Parent ID of actual mother process:
    // Decay: keep both decaying particle and Michel e-, Hit Parent ID should be Track ID from Michel e-
    // Pi0 : keep both pi0 and gamma's, Hit Parent ID should be Track ID from gamma
    // nCapture: keep both n and gamma, Hit Parent ID should be Track ID from gamma.
    // Hits from LE electrons from muIonization, etc. : Hit Parent ID should be from Mother particle, not secondary track ID.
    
    if (aTrack->GetDefinition()->GetPDGEncoding()==111)
      pi0List.insert(aTrack->GetTrackID()); // list of all pi0-s 
    
    // Be careful with gamma's. I want the ones closest to the actual mother process, not all secondaries.
    if (aTrack->GetDefinition()->GetPDGEncoding() == 22){
      // also use lazy evaluation of "or" here:
      if( aTrack->GetParentID() == 0  || // then this gamma has no creator process (eg. nRooTracker particles)
	  pi0List.count(aTrack->GetParentID()) ||
	  (creatorProcess->GetProcessName() == "nCapture") ||
	  (creatorProcess->GetProcessName() == "NeutronInelastic")	  
	  )
	anInfo->SetPrimaryParentID(aTrack->GetTrackID());  
    }
    //TF: crucial bugfix: I want this for all tracks that I save to match Ch hits with tracks that can
    // produce Cherenkov light.
    else
      anInfo->SetPrimaryParentID(aTrack->GetTrackID());
    }
  else {
    anInfo->WillBeSaved(false);
  }


  G4Track* theTrack = (G4Track*)aTrack;
  theTrack->SetUserInformation(anInfo);

  // pass primary parent ID to children
  G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
  if(secondaries)
  {
    size_t nSeco = secondaries->size();
    if(nSeco>0)
    {
      for(size_t i=0;i<nSeco;i++)
      { 
	WCSimTrackInformation* infoSec = new WCSimTrackInformation(anInfo);
	if(anInfo->isSaved()){ // Parent is primary, so we want start pos & time of this secondary
		infoSec->SetPhotonStartTime((*secondaries)[i]->GetGlobalTime());
		infoSec->SetPhotonStartPos((*secondaries)[i]->GetPosition());
		infoSec->SetPhotonStartDir((*secondaries)[i]->GetMomentumDirection());
	}
	infoSec->WillBeSaved(false); // ADDED BY MFECHNER, temporary, 30/8/06
	(*secondaries)[i]->SetUserInformation(infoSec);
      }
    } 
  }

  if ( aTrack->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition() )
    //   if (aTrack->GetDefinition()->GetPDGCharge() == 0) 
  {
    WCSimTrajectory *currentTrajectory = 
      (WCSimTrajectory*)fpTrackingManager->GimmeTrajectory();

    G4ThreeVector currentPosition      = aTrack->GetPosition();
    G4VPhysicalVolume* currentVolume   = aTrack->GetVolume();

    currentTrajectory->SetStoppingPoint(currentPosition);
    currentTrajectory->SetStoppingVolume(currentVolume);

    if (anInfo->isSaved())
      currentTrajectory->SetSaveFlag(true);// mark it for WCSimEventAction ;
    else currentTrajectory->SetSaveFlag(false);// mark it for WCSimEventAction ;
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




