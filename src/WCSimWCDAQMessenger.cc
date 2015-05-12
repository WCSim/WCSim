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
  WCSimDAQDir = new G4UIdirectory("/DAQ/");
  WCSimDAQDir->SetGuidance("Commands to select DAQ options");

  DigitizerChoice = new G4UIcmdWithAString("/DAQ/Digitizer", this);
  DigitizerChoice->SetGuidance("Set the Digitizer type");
  DigitizerChoice->SetGuidance("Available choices are:\n"
			       "SKI\n"
			       "SKIV\n"
			       "SKI_SKDETSIM (combined trigger & digitization (therefore ignores /DAQ/Trigger); buggy) \n"
			       );
  DigitizerChoice->SetParameterName("Digitizer", false);
  DigitizerChoice->SetCandidates(
				 "SKI "
				 "SKIV "
				 "SKI_SKDETSIM "
				 );
  DigitizerChoice->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  TriggerChoice = new G4UIcmdWithAString("/DAQ/Trigger", this);
  TriggerChoice->SetGuidance("Set the Trigger type");
  TriggerChoice->SetGuidance("Available choices are:\n"
			     "NHits\n"
			     "SKI_SKDETSIM (combined trigger & digitization (therefore ignores /DAQ/Digitization); buggy) \n"
			     );
  TriggerChoice->SetParameterName("Trigger", false);
  TriggerChoice->SetCandidates(
			       "NHits "
			       "SKI_SKDETSIM "
			       );
  TriggerChoice->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  //NHits trigger specifc options
  NHitsTriggerDir = new G4UIdirectory("/DAQ/TriggerNHits/");
  NHitsTriggerDir->SetGuidance("Commands specific to the NHits trigger");
  
  NHitsTriggerThreshold = new G4UIcmdWithAnInteger("/DAQ/TriggerNHits/Threshold", this);
  NHitsTriggerThreshold->SetGuidance("Set the NHits trigger threshold");
  NHitsTriggerThreshold->SetParameterName("NHitsThreshold",true);
  NHitsTriggerThreshold->SetDefaultValue(25);

  NHitsTriggerWindow = new G4UIcmdWithAnInteger("/DAQ/TriggerNHits/Window", this);
  NHitsTriggerWindow->SetGuidance("Set the NHits trigger window (in ns)");
  NHitsTriggerWindow->SetParameterName("NHitsWindow",true);
  NHitsTriggerWindow->SetDefaultValue(200);
}

WCSimWCDAQMessenger::~WCSimWCDAQMessenger()
{
  delete DigitizerChoice;
  delete TriggerChoice;
  delete WCSimDAQDir;
}

void WCSimWCDAQMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  //Because this Messenger class contains options for classes that don't exist when options are
  // read in (Trigger and Digitizer class options) we need to store each options' value
  // for use in the Tell*() methods later

  if (command == DigitizerChoice) {
    WCSimEvent->SetDigitizerChoice(newValue); 
    G4cout << "Digitizer choice set to " << newValue << G4endl;
    WCSimEvent->CreateDigitizerInstance();
    StoreDigitizerChoice = newValue;
  }
  else if (command == TriggerChoice) {
    WCSimEvent->SetTriggerChoice(newValue); 
    G4cout << "Trigger choice set to " << newValue << G4endl;
    WCSimEvent->CreateTriggerInstance();
    StoreTriggerChoice = newValue;
  }
  else if (command == NHitsTriggerThreshold) {
    StoreSetNHitsThreshold = NHitsTriggerThreshold->GetNewIntValue(newValue);
  }
  else if (command == NHitsTriggerWindow) {
    StoreSetNHitsWindow = NHitsTriggerWindow->GetNewIntValue(newValue);
  }
}

void WCSimWCDAQMessenger::TellTrigger()
{
  G4cout << "Passing Trigger options to the trigger class instance" << G4endl;
  G4int threshold = StoreSetNHitsThreshold;
  WCSimTrigger->SetNHitsThreshold(threshold);
  G4cout << "NHits trigger threshold set to " << threshold << G4endl;
  G4int window = StoreSetNHitsWindow;
  WCSimTrigger->SetNHitsWindow(window);
  G4cout << "NHits trigger window set to " << window << G4endl;
}

void WCSimWCDAQMessenger::TellDigitizer()
{
  G4cout << "Passing Digitizer options to the digitizer class instance" << G4endl;
  WCSimDigitize->SKDigitizerType(StoreDigitizerChoice);
  G4cout << "Digitizer choice set to " << StoreDigitizerChoice << G4endl;
}
