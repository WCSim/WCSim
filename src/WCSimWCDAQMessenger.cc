#include "WCSimWCDAQMessenger.hh"
#include "WCSimEventAction.hh"
#include "WCSimWCDigitizerNew.hh"
#include "WCSimWCTrigger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"

#include <string>

WCSimWCDAQMessenger::WCSimWCDAQMessenger(WCSimEventAction* eventaction) :
  WCSimEvent(eventaction)
{
  initaliseString = " (this is a default set; it may be overwritten by user commands)";

  WCSimDAQDir = new G4UIdirectory("/DAQ/");
  WCSimDAQDir->SetGuidance("Commands to select DAQ options");

  G4String defaultDigitizer = "SKI";
  DigitizerChoice = new G4UIcmdWithAString("/DAQ/Digitizer", this);
  DigitizerChoice->SetGuidance("Set the Digitizer type");
  DigitizerChoice->SetGuidance("Available choices are:\n"
			       "SKI\n"
			       "SKI_SKDETSIM (combined trigger & digitization (therefore ignores /DAQ/Trigger); buggy) \n"
			       );
  DigitizerChoice->SetParameterName("Digitizer", false);
  DigitizerChoice->SetCandidates(
				 "SKI "
				 "SKI_SKDETSIM "
				 );
  DigitizerChoice->AvailableForStates(G4State_PreInit, G4State_Idle);
  DigitizerChoice->SetDefaultValue(defaultDigitizer);
  StoreDigitizerChoice = defaultDigitizer;
  SetNewValue(DigitizerChoice, defaultDigitizer);

  G4String defaultTrigger = "NHits";
  TriggerChoice = new G4UIcmdWithAString("/DAQ/Trigger", this);
  TriggerChoice->SetGuidance("Set the Trigger type");
  TriggerChoice->SetGuidance("Available choices are:\n"
			     "NHits\n"
			     "NHits2\n"
			     "SKI_SKDETSIM (combined trigger & digitization (therefore ignores /DAQ/Digitization); buggy) \n"
			     );
  TriggerChoice->SetParameterName("Trigger", false);
  TriggerChoice->SetCandidates(
			       "NHits "
			       "NHits2 "
			       "SKI_SKDETSIM "
			       );
  TriggerChoice->AvailableForStates(G4State_PreInit, G4State_Idle);
  TriggerChoice->SetDefaultValue(defaultTrigger);
  StoreTriggerChoice = defaultTrigger;
  SetNewValue(TriggerChoice, defaultTrigger);


  //Generic digitizer specific options
  DigitizerDir = new G4UIdirectory("/DAQ/DigitizerOpt/");
  DigitizerDir->SetGuidance("Generic commands for digitizers");

  int defaultDigitizerDeadTime = 0;
  DigitizerDeadTime = new G4UIcmdWithAnInteger("/DAQ/DigitizerOpt/DeadTime", this);
  DigitizerDeadTime->SetGuidance("The deadtime for the digitizer (in ns)");
  DigitizerDeadTime->SetParameterName("DigitizerDeadTime",true);
  DigitizerDeadTime->SetDefaultValue(defaultDigitizerDeadTime);
  StoreDigitizerDeadTime = defaultDigitizerDeadTime;
  SetNewValue(DigitizerDeadTime, G4UIcommand::ConvertToString(defaultDigitizerDeadTime));


  //Save failure trigger specific options
  SaveFailuresTriggerDir = new G4UIdirectory("/DAQ/TriggerSaveFailures/");
  SaveFailuresTriggerDir->SetGuidance("Commands specific to the Save Failures trigger");

  int defaultSaveFailuresTriggerMode = 0;
  SaveFailuresTriggerMode = new G4UIcmdWithAnInteger("/DAQ/TriggerSaveFailures/Mode", this);
  SaveFailuresTriggerMode->SetGuidance("0: save only triggered events; 1: save both triggered and failed events; 2: save only failed events");
  SaveFailuresTriggerMode->SetParameterName("SaveFailuresMode",true);
  SaveFailuresTriggerMode->SetDefaultValue(defaultSaveFailuresTriggerMode);
  StoreSaveFailuresMode = defaultSaveFailuresTriggerMode;
  SetNewValue(SaveFailuresTriggerMode, G4UIcommand::ConvertToString(defaultSaveFailuresTriggerMode));

  double defaultSaveFailuresTriggerTime = 100;
  SaveFailuresTriggerTime = new G4UIcmdWithADouble("/DAQ/TriggerSaveFailures/TriggerTime", this);
  SaveFailuresTriggerTime->SetGuidance("The trigger time for the events which failed other triggers");
  SaveFailuresTriggerTime->SetParameterName("SaveFailuresTime",true);
  SaveFailuresTriggerTime->SetDefaultValue(defaultSaveFailuresTriggerTime);
  StoreSaveFailuresTime = defaultSaveFailuresTriggerTime;
  SetNewValue(SaveFailuresTriggerTime, G4UIcommand::ConvertToString(defaultSaveFailuresTriggerTime));

  //NHits trigger specifc options
  NHitsTriggerDir = new G4UIdirectory("/DAQ/TriggerNHits/");
  NHitsTriggerDir->SetGuidance("Commands specific to the NHits trigger");
  
  int defaultNHitsTriggerThreshold = 25;
  NHitsTriggerThreshold = new G4UIcmdWithAnInteger("/DAQ/TriggerNHits/Threshold", this);
  NHitsTriggerThreshold->SetGuidance("Set the NHits trigger threshold");
  NHitsTriggerThreshold->SetParameterName("NHitsThreshold",false);
  NHitsTriggerThreshold->SetDefaultValue(defaultNHitsTriggerThreshold);
  StoreNHitsThreshold = defaultNHitsTriggerThreshold;
  SetNewValue(NHitsTriggerThreshold, G4UIcommand::ConvertToString(defaultNHitsTriggerThreshold));

  int defaultNHitsTriggerWindow = 200;
  NHitsTriggerWindow = new G4UIcmdWithAnInteger("/DAQ/TriggerNHits/Window", this);
  NHitsTriggerWindow->SetGuidance("Set the NHits trigger window (in ns)");
  NHitsTriggerWindow->SetParameterName("NHitsWindow",false);
  NHitsTriggerWindow->SetDefaultValue(defaultNHitsTriggerWindow);
  StoreNHitsWindow = defaultNHitsTriggerWindow;
  SetNewValue(NHitsTriggerWindow, G4UIcommand::ConvertToString(defaultNHitsTriggerWindow));

  bool defaultNHitsTriggerAdjustForNoise = false;
  NHitsTriggerAdjustForNoise = new G4UIcmdWithABool("/DAQ/TriggerNHits/AdjustForNoise", this);
  NHitsTriggerAdjustForNoise->SetGuidance("Adjust the NHits trigger threshold automatically dependent on the average noise rate");
  NHitsTriggerAdjustForNoise->SetParameterName("NHitsAdjustForNoise",true);
  NHitsTriggerAdjustForNoise->SetDefaultValue(defaultNHitsTriggerAdjustForNoise);
  StoreNHitsAdjustForNoise = defaultNHitsTriggerAdjustForNoise;
  SetNewValue(NHitsTriggerAdjustForNoise, G4UIcommand::ConvertToString(defaultNHitsTriggerAdjustForNoise));


  //TODO remove this
  DAQConstruct = new G4UIcmdWithoutParameter("/DAQ/Construct", this);
  DAQConstruct->SetGuidance("Create the DAQ class instances");

  initaliseString = "";
}

WCSimWCDAQMessenger::~WCSimWCDAQMessenger()
{
  delete DAQConstruct; //TODO remove this

  delete SaveFailuresTriggerDir;
  delete SaveFailuresTriggerMode;
  delete SaveFailuresTriggerTime;

  delete NHitsTriggerDir;
  delete NHitsTriggerThreshold;
  delete NHitsTriggerWindow;
  delete NHitsTriggerAdjustForNoise;

  delete DigitizerDir;
  delete DigitizerDeadTime;

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
    G4cout << "Digitizer choice set to " << newValue << initaliseString.c_str() << G4endl;
    WCSimEvent->SetDigitizerChoice(newValue);
    StoreDigitizerChoice = newValue;
  }
  else if (command == TriggerChoice) {
    G4cout << "Trigger choice set to " << newValue << initaliseString.c_str() << G4endl;
    WCSimEvent->SetTriggerChoice(newValue);
    StoreTriggerChoice = newValue;
  }

  //Generic digitizer options
  else if (command == DigitizerDeadTime) {
    G4cout << "Digitizer deadtime set to " << newValue << initaliseString.c_str() << G4endl;
    StoreDigitizerDeadTime = DigitizerDeadTime->GetNewIntValue(newValue);
  }

  //Save failures "trigger"
  else if (command == SaveFailuresTriggerMode) {
    StoreSaveFailuresMode = SaveFailuresTriggerMode->GetNewIntValue(newValue);
    std::string failuremode;
    if(StoreSaveFailuresMode == 0)
      failuremode = "Saving only triggered events";
    else if(StoreSaveFailuresMode == 1)
      failuremode = "Saving both triggered and failed events";
    else if(StoreSaveFailuresMode == 2)
      failuremode = "Saving only failed events";
    else {
      G4cerr << "Unknown value of /DAQ/TriggerSaveFailures/Mode " << StoreSaveFailuresMode << " Exiting..." << G4endl;
      exit(-1);
    }
    G4cout << failuremode << initaliseString.c_str() << G4endl;
  }
  else if (command == SaveFailuresTriggerTime) {
    G4cout << "Trigger time for events which fail all triggers will be set to " << newValue << initaliseString.c_str() << G4endl;
    StoreSaveFailuresTime = SaveFailuresTriggerTime->GetNewDoubleValue(newValue);
  }

  //NHits trigger
  else if (command == NHitsTriggerThreshold) {
    G4cout << "NHits trigger threshold set to " << newValue << initaliseString.c_str() << G4endl;
    StoreNHitsThreshold = NHitsTriggerThreshold->GetNewIntValue(newValue);
  }
  else if (command == NHitsTriggerAdjustForNoise) {
    StoreNHitsAdjustForNoise = NHitsTriggerAdjustForNoise->GetNewBoolValue(newValue);
    if(StoreNHitsAdjustForNoise)
      G4cout << "Will adjust NHits trigger threshold using average dark noise rate" << initaliseString.c_str() << G4endl;
  }
  else if (command == NHitsTriggerWindow) {
    G4cout << "NHits trigger window set to " << newValue << initaliseString.c_str() << G4endl;
    StoreNHitsWindow = NHitsTriggerWindow->GetNewIntValue(newValue);
  }

  //TODO remove this
  else if(command == DAQConstruct) {
    G4cout << "Calling WCSimEventAction::CreateDAQInstances()" << G4endl;
    WCSimEvent->CreateDAQInstances();
  }
}

void WCSimWCDAQMessenger::SetTriggerOptions()
{
  G4cout << "Passing Trigger options to the trigger class instance" << G4endl;

  WCSimTrigger->SetSaveFailuresMode(StoreSaveFailuresMode);
  std::string failuremode;
  if(StoreSaveFailuresMode == 0)
    failuremode = "Saving only triggered events";
  else if(StoreSaveFailuresMode == 1)
    failuremode = "Saving both triggered and failed events";
  else if(StoreSaveFailuresMode == 2)
    failuremode = "Saving only failed events";
  G4cout << "\t" << failuremode << G4endl;
  WCSimTrigger->SetSaveFailuresTime(StoreSaveFailuresTime);
  G4cout << "\tTrigger time for events which fail all triggers will be set to " << StoreSaveFailuresTime << G4endl;

  WCSimTrigger->SetNHitsThreshold(StoreNHitsThreshold);
  G4cout << "\tNHits trigger threshold set to " << StoreNHitsThreshold << G4endl;
  WCSimTrigger->SetNHitsAdjustForNoise(StoreNHitsAdjustForNoise);
  if(StoreNHitsAdjustForNoise)
    G4cout << "\tWill adjust NHits trigger threshold using average dark noise rate" << G4endl;
  WCSimTrigger->SetNHitsWindow(StoreNHitsWindow);
  G4cout << "\tNHits trigger window set to " << StoreNHitsWindow << G4endl;
}

void WCSimWCDAQMessenger::SetDigitizerOptions()
{
  G4cout << "Passing Digitizer options to the digitizer class instance" << G4endl;
  WCSimDigitize->SetDigitizerDeadTime(StoreDigitizerDeadTime);
  G4cout << "\tDigitizer deadtime set to " << StoreDigitizerDeadTime << G4endl;
}
