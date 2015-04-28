#include "WCSimWCDAQMessenger.hh"
#include "WCSimWCDigitizer.hh"
#include "WCSimWCAddDarkNoise.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"

WCSimWCDAQMessenger::WCSimWCDAQMessenger(WCSimWCEventAction* eventaction):WCSimEvent(eventaction)
{
  Initalize();
  constructor = 0;
}

WCSimWCDAQMessenger::WCSimWCDAQMessenger(WCSimWCEventAction* eventaction):WCSimEvent(eventaction)
{
  Initalize();
  constructor = 1;
}

WCSimWCDAQMessenger::WCSimWCDAQMessenger(WCSimWCEventAction* eventaction):WCSimEvent(eventaction)
{
  Initalize();
  constructor = 2;
}

void WCSimWCDAQMessenger::Initalize()
{
  WCSimDAQDir = new G4UIdirectory("/WCSimDAQ/");
  WCSimDAQDir->SetGuidance("Commands to select DAQ options");

  DigitizerChoice = new G4UIcmdWithAString("/WCSimDAQ/Digitizer", this);
  DigitizerChoice->SetGuidance("Set the Digitizer type");
  DigitizerChoice->SetGuidance("Available choices are:\n"
			       "SKIV\n"
			       "SKI_SKDETSIM (combined trigger & digitization (therefore ignores /WCSimDAQ/Trigger); buggy) \n"
			       );
  DigitizerChoice->SetParameterName("Digitizer", false);
  DigitizerChoice->SetCandidates(
			   "SKIV "
			   "SKI_SKDETSIM "
			   );
  DigitizerChoice->AvailableForStates(G4State_PreInit, G4State_Idle);

  TriggerChoice = new G4UIcmdWithAString("/WCSimDAQ/Trigger", this);
  TriggerChoice->SetGuidance("Set the Trigger type");
  TriggerChoice->SetGuidance("Available choices are:\n"
			       "NHits\n"
			       );
  TriggerChoice->SetParameterName("Trigger", false);
  TriggerChoice->SetCandidates(
			   "NHits "
			   );
  TriggerChoice->AvailableForStates(G4State_PreInit, G4State_Idle);
}

WCSimWCDAQMessenger::~WCSimWCDAQMessenger()
{
  delete DigitizerChoice;
  delete TriggerChoice;
  delete WCSimDAQDir;
}

void WCSimWCDAQMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if (command == DigitizerChoice)
    {
      if(constructor == 0) {
	WCSimEvent->SetDigitizerChoice(newValue); 
	G4cout << "Digitizer choice set to " << newValue << G4endl;
      }
    }

  else if (command == TriggerChoice)
    {
      if(constructor == 0) {
	WCSimEvent->SetTriggerChoice(newValue); 
	G4cout << "Trigger choice set to " << newValue << G4endl;
      }
    }
}
