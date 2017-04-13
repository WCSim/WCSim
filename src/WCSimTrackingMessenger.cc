#include "WCSimTrackingMessenger.hh"
#include "WCSimTrackingAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"

WCSimTrackingMessenger::WCSimTrackingMessenger(WCSimTrackingAction* trackAction)
  : myTracking(trackAction)
{

  WCSimDir = new G4UIdirectory("/Tracking/");
  WCSimDir->SetGuidance("Commands to change particles to draw and save during tracking");
  
  fractionPhotonsToDraw = new G4UIcmdWithADouble("/Tracking/fractionOpticalPhotonsToDraw",this);
  fractionPhotonsToDraw->SetGuidance("Command to change the fraction of Optical Photons to track");
  fractionPhotonsToDraw->SetParameterName("fractionOpticalPhotonsToDraw",true);
  fractionPhotonsToDraw->SetDefaultValue(0.0);

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

}
