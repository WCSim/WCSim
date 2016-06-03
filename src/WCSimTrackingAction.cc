#include "WCSimTrackingAction.hh"
#include "WCSimTrajectory.hh"
#include "G4ParticleTypes.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4ios.hh"
#include "G4VProcess.hh"
#include "WCSimTrackInformation.hh"

WCSimTrackingAction::WCSimTrackingAction()
{
  ProcessList.insert("Decay") ;
  //ProcessList.insert("MuonMinusCaptureAtRest") ;
//   ProcessList.insert("conv");
  ParticleList.insert(111); // pi0
  ParticleList.insert(211); // pion+
  ParticleList.insert(-211);
  ParticleList.insert(321);
  ParticleList.insert(-321); // kaon-
  ParticleList.insert(311); // kaon0
  ParticleList.insert(-311); // kaon0 bar
  
  ParticleList.insert(11);
  ParticleList.insert(-11);
  ParticleList.insert(12);
  ParticleList.insert(-12);
  // don't put gammas there or there'll be too many
}

WCSimTrackingAction::~WCSimTrackingAction(){;}

void WCSimTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  G4float percentageOfCherenkovPhotonsToDraw = 0.0;
  
  if ( aTrack->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()
       || G4UniformRand() < percentageOfCherenkovPhotonsToDraw )
    {
      WCSimTrajectory* thisTrajectory = new WCSimTrajectory(aTrack);
      fpTrackingManager->SetTrajectory(thisTrajectory);
      fpTrackingManager->SetStoreTrajectory(true);
    }
  else 
    fpTrackingManager->SetStoreTrajectory(false);
}

void WCSimTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
  // added by M Fechner
  const G4VProcess* creatorProcess = aTrack->GetCreatorProcess();
  //  if ( creatorProcess )
  //    G4cout << "process name " << creatorProcess->GetProcessName() << G4endl;


  WCSimTrackInformation* anInfo;
  if (aTrack->GetUserInformation())
    anInfo = (WCSimTrackInformation*)(aTrack->GetUserInformation());
  else anInfo = new WCSimTrackInformation();

  // is it a primary ?
  // is the process in the set ? 
  // is the particle in the set ?
  // is it a gamma 
  // due to lazy evaluation of the 'or' in C++ the order is important
  if( aTrack->GetParentID()==0 || 
      ((creatorProcess!=0) && ProcessList.count(creatorProcess->GetProcessName()) ) || 
      (ParticleList.count(aTrack->GetDefinition()->GetPDGEncoding()) )
      || (aTrack->GetDefinition()->GetPDGEncoding()==22 && aTrack->GetTotalEnergy() > 50.0*MeV)
      )
  {
    // if so the track is worth saving
    anInfo->WillBeSaved(true);

    //      G4cout << "track # " << aTrack->GetTrackID() << " is worth saving\n";
    //      G4cout << "It is a " <<aTrack->GetDefinition()->GetParticleName() << G4endl;
  }
  else
    anInfo->WillBeSaved(false);

  if (aTrack->GetDefinition()->GetPDGEncoding()==111)
    pi0List.insert(aTrack->GetTrackID()); // list of all pi0-s 

  if (aTrack->GetParentID()==0 || // primary particle
      (aTrack->GetDefinition()->GetPDGEncoding()==22 && // primary gamma from
       pi0List.count(aTrack->GetParentID())))            // a pi0
    anInfo->SetPrimaryParentID(aTrack->GetTrackID());

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
}





