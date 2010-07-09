#include "WCSimPhysicsListFactoryMessenger.hh"
#include "WCSimPhysicsListFactory.hh"

#include "G4UIdirectory.hh"
#include "G4ios.hh"
#include "globals.hh"
#include "G4UIcmdWithAString.hh"

WCSimPhysicsListFactoryMessenger::WCSimPhysicsListFactoryMessenger(WCSimPhysicsListFactory* WCSimPhysFactory)
  :WCSimPhysicsListFactory(WCSimPhysFactory)
{
  
  //WCSimDir = new G4UIdirectory("/WCSim/physics/list/");
  //WCSimDir->SetGuidance("Commands to change secondary interaction model for protons");

  hadmodelCmd = new G4UIcmdWithAString("/WCSim/physics/list",this);
  hadmodelCmd->SetGuidance("Available options: ");
  hadmodelCmd->SetGuidance("QGSP_BERT"); // TODO get list of physics lists from G4PhysicsListFactory
  hadmodelCmd->SetGuidance("Description:");
  //hadmodelCmd->SetGuidance("GHEISHA = standard, fast G4 hadronic interaction model");
  //hadmodelCmd->SetGuidance("BERTINI = Bertini cascade model");
  //hadmodelCmd->SetGuidance("BINARY  = Binary cascade model (2KM default)");
  hadmodelCmd->SetDefaultValue("QGSP_BERT");
  hadmodelCmd->SetCandidates("QGSP_BERT");  // TODO get list of physics lists from G4PhysicsListFactory

}

WCSimPhysicsListFactoryMessenger::~WCSimPhysicsListFactoryMessenger()
{
  delete hadmodelCmd;
  delete WCSimDir;
}

void WCSimPhysicsMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == hadmodelCmd)
    WCSimPhysics->SetSecondaryHad(newValue);

}
