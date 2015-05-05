#include "WCSimWCDAQMessenger.hh"
#include "WCSimEventAction.hh"
#include "WCSimWCDigitizerBase.hh"
#include "WCSimWCTriggerBase.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"

WCSimWCDAQMessenger::WCSimWCDAQMessenger(WCSimEventAction * eventaction):WCSimEvent(eventaction)
{
  Initialize();
  instanceType = kEventAction;
}

WCSimWCDAQMessenger::WCSimWCDAQMessenger(WCSimWCDigitizerBase* digitizer):WCSimDigitize(digitizer)
{
  Initialize();
  instanceType = kDigitizer;
}

WCSimWCDAQMessenger::WCSimWCDAQMessenger(WCSimWCTriggerBase* trigger):WCSimTrigger(trigger)
{
  Initialize();
  instanceType = kTrigger;
}

void WCSimWCDAQMessenger::Initialize()
{
  WCSimDAQDir = new G4UIdirectory("/DAQ/");
  WCSimDAQDir->SetGuidance("Commands to select DAQ options");

  DigitizerChoice = new G4UIcmdWithAString("/DAQ/Digitizer", this);
  DigitizerChoice->SetGuidance("Set the Digitizer type");
  DigitizerChoice->SetGuidance("Available choices are:\n"
			       "SK\n"
			       "SKI_SKDETSIM (combined trigger & digitization (therefore ignores /DAQ/Trigger); buggy) \n"
			       );
  DigitizerChoice->SetParameterName("Digitizer", false);
  DigitizerChoice->SetCandidates(
			   "SK "
			   "SKI_SKDETSIM "
			   );
  DigitizerChoice->AvailableForStates(G4State_PreInit, G4State_Idle);

  TriggerChoice = new G4UIcmdWithAString("/DAQ/Trigger", this);
  TriggerChoice->SetGuidance("Set the Trigger type");
  TriggerChoice->SetGuidance("Available choices are:\n"
			       "NHits\n"
			       );
  TriggerChoice->SetParameterName("Trigger", false);
  TriggerChoice->SetCandidates(
			   "NHits "
			   );
  TriggerChoice->AvailableForStates(G4State_PreInit, G4State_Idle);

  //NHits trigger specifc options
  NHitsTriggerDir = new G4UIdirectory("/DAQ/TriggerNHits/");
  NHitsTriggerDir->SetGuidance("Commands specific to the NHits trigger");

  NHitsTriggerThreshold = new G4UIcmdWithAnInteger("/DAQ/TriggerNHits/Threshold", this);
  NHitsTriggerThreshold->SetGuidance("Set the NHits trigger threshold");
  NHitsTriggerThreshold->SetParameterName("NHitsThreshold",true);
  NHitsTriggerThreshold->SetDefaultValue(25);

}

WCSimWCDAQMessenger::~WCSimWCDAQMessenger()
{
  delete DigitizerChoice;
  delete TriggerChoice;
  delete WCSimDAQDir;
}

void WCSimWCDAQMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if (command == DigitizerChoice) {
    if(instanceType == kEventAction) {
      WCSimEvent->SetDigitizerChoice(newValue); 
      G4cout << "Digitizer choice set to " << newValue << G4endl;
      WCSimEvent->CreateDigitizerInstance();
    }
    if(instanceType == kDigitizer) {
      //WCSimDigitize->SKElectronicsType(newValue);
      G4cout << "Digitizer choice set to " << newValue << G4endl;
    }
  }
  else if (command == TriggerChoice) {
    if(instanceType == kEventAction) {
      WCSimEvent->SetTriggerChoice(newValue); 
      G4cout << "Trigger choice set to " << newValue << G4endl;
      WCSimEvent->CreateTriggerInstance();
    }
  }
  else if (command == NHitsTriggerThreshold) {
    if(instanceType == kTrigger) {
      WCSimTrigger->SetNHitsThreshold(NHitsTriggerThreshold->GetNewIntValue(newValue));
      G4cout << "NHits trigger threshold set to " << newValue << G4endl;
    }
  }
}
