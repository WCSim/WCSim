#include "WCSimTrackingMessenger.hh"
#include "WCSimTrackingAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"

WCSimTrackingMessenger::WCSimTrackingMessenger(WCSimTrackingAction* trackAction)
  : myTracking(trackAction)
{

  WCSimDir = new G4UIdirectory("/Tracking/");
  WCSimDir->SetGuidance("Commands to change particles to draw and save during tracking");
  
  fractionPhotonsToDraw = new G4UIcmdWithADouble("/Tracking/fractionOpticalPhotonsToDraw",this);
  fractionPhotonsToDraw->SetGuidance("Command to change the fraction of Optical Photons to track");
  fractionPhotonsToDraw->SetParameterName("fractionOpticalPhotonsToDraw",true);
  fractionPhotonsToDraw->SetDefaultValue(0.0);

  particleToTrack = new G4UIcmdWithAnInteger("/Tracking/trackParticle",this);
  particleToTrack->SetGuidance("Command to track all particles of given type");
  particleToTrack->SetParameterName("particleToTrack",false);

  processToTrack = new G4UIcmdWithAString("/Tracking/trackProcess",this);
  processToTrack->SetGuidance("Command to track all particles created by given process");
  processToTrack->SetParameterName("processToTrack",false);

}

WCSimTrackingMessenger::~WCSimTrackingMessenger()
{

  delete fractionPhotonsToDraw;
  delete WCSimDir;
}

void WCSimTrackingMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if(command == fractionPhotonsToDraw){
    myTracking->SetFractionChPhotons(fractionPhotonsToDraw->GetNewDoubleValue(newValue));
    G4cout << "Setting fraction of optical photons to draw and save to " << fractionPhotonsToDraw->GetNewDoubleValue(newValue) << "%" << G4endl;
    G4cout << "=======================================================================================. " << G4endl;
    G4cout << "WARNING: Necessary to visualize photons but will also create many tracks in ROOT output. " << G4endl;
    G4cout << "WARNING: Recommended to not save to root when this is > 0.0 % " << G4endl;
    G4cout << "=======================================================================================. " << G4endl;
  }
  else if(command == particleToTrack){
    G4int pid = particleToTrack->GetNewIntValue(newValue);
    myTracking->AddParticle(pid);
    G4cout << "Tracking all particles with PID = " << pid << G4endl;
  }
  else if(command == processToTrack){
    myTracking->AddProcess(newValue);
    G4cout << "Tracking all particles created by the " << newValue << " process" << G4endl;
  }
  

}
