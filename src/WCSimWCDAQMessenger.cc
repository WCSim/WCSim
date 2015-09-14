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
  initialiseString = " (this is a default set; it may be overwritten by user commands)";

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

  G4String defaultTrigger = "NDigits";
  TriggerChoice = new G4UIcmdWithAString("/DAQ/Trigger", this);
  TriggerChoice->SetGuidance("Set the Trigger type");
  TriggerChoice->SetGuidance("Available choices are:\n"
			     "NDigits\n"
			     "NDigits2\n"
			     "SKI_SKDETSIM (combined trigger & digitization (therefore ignores /DAQ/Digitization); buggy) \n"
			     );
  TriggerChoice->SetParameterName("Trigger", false);
  TriggerChoice->SetCandidates(
			       "NDigits "
			       "NDigits2 "
			       "SKI_SKDETSIM "
			       );
  TriggerChoice->AvailableForStates(G4State_PreInit, G4State_Idle);
  TriggerChoice->SetDefaultValue(defaultTrigger);
  StoreTriggerChoice = defaultTrigger;
  SetNewValue(TriggerChoice, defaultTrigger);


  //Generic digitizer specific options
  DigitizerDir = new G4UIdirectory("/DAQ/DigitizerOpt/");
  DigitizerDir->SetGuidance("Generic commands for digitizers");

  int defaultDigitizerDeadTime = -99;
  DigitizerDeadTime = new G4UIcmdWithAnInteger("/DAQ/DigitizerOpt/DeadTime", this);
  DigitizerDeadTime->SetGuidance("The deadtime for the digitizer (in ns)");
  DigitizerDeadTime->SetParameterName("DigitizerDeadTime",true);
  DigitizerDeadTime->SetDefaultValue(defaultDigitizerDeadTime);
  StoreDigitizerDeadTime = defaultDigitizerDeadTime;
  //don't SetNewValue -> defaults class-specific and taken from GetDefault*()

  int defaultDigitizerIntegrationWindow = -99;
  DigitizerIntegrationWindow = new G4UIcmdWithAnInteger("/DAQ/DigitizerOpt/IntegrationWindow", this);
  DigitizerIntegrationWindow->SetGuidance("The integration window for the digitizer (in ns)");
  DigitizerIntegrationWindow->SetParameterName("DigitizerIntegrationWindow",true);
  DigitizerIntegrationWindow->SetDefaultValue(defaultDigitizerIntegrationWindow);
  StoreDigitizerIntegrationWindow = defaultDigitizerIntegrationWindow;
  //don't SetNewValue -> defaults class-specific and taken from GetDefault*()


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

  //NDigits trigger specifc options
  NDigitsTriggerDir = new G4UIdirectory("/DAQ/TriggerNDigits/");
  NDigitsTriggerDir->SetGuidance("Commands specific to the NDigits trigger");
  
  int defaultNDigitsTriggerThreshold = 25;
  NDigitsTriggerThreshold = new G4UIcmdWithAnInteger("/DAQ/TriggerNDigits/Threshold", this);
  NDigitsTriggerThreshold->SetGuidance("Set the NDigits trigger threshold");
  NDigitsTriggerThreshold->SetParameterName("NDigitsThreshold",false);
  NDigitsTriggerThreshold->SetDefaultValue(defaultNDigitsTriggerThreshold);
  StoreNDigitsThreshold = defaultNDigitsTriggerThreshold;
  SetNewValue(NDigitsTriggerThreshold, G4UIcommand::ConvertToString(defaultNDigitsTriggerThreshold));

  int defaultNDigitsTriggerWindow = 200;
  NDigitsTriggerWindow = new G4UIcmdWithAnInteger("/DAQ/TriggerNDigits/Window", this);
  NDigitsTriggerWindow->SetGuidance("Set the NDigits trigger window (in ns)");
  NDigitsTriggerWindow->SetParameterName("NDigitsWindow",false);
  NDigitsTriggerWindow->SetDefaultValue(defaultNDigitsTriggerWindow);
  StoreNDigitsWindow = defaultNDigitsTriggerWindow;
  SetNewValue(NDigitsTriggerWindow, G4UIcommand::ConvertToString(defaultNDigitsTriggerWindow));

  bool defaultNDigitsTriggerAdjustForNoise = false;
  NDigitsTriggerAdjustForNoise = new G4UIcmdWithABool("/DAQ/TriggerNDigits/AdjustForNoise", this);
  NDigitsTriggerAdjustForNoise->SetGuidance("Adjust the NDigits trigger threshold automatically dependent on the average noise rate");
  NDigitsTriggerAdjustForNoise->SetParameterName("NDigitsAdjustForNoise",true);
  NDigitsTriggerAdjustForNoise->SetDefaultValue(defaultNDigitsTriggerAdjustForNoise);
  StoreNDigitsAdjustForNoise = defaultNDigitsTriggerAdjustForNoise;
  SetNewValue(NDigitsTriggerAdjustForNoise, G4UIcommand::ConvertToString(defaultNDigitsTriggerAdjustForNoise));


  //TODO remove this
  DAQConstruct = new G4UIcmdWithoutParameter("/DAQ/Construct", this);
  DAQConstruct->SetGuidance("Create the DAQ class instances");

  initialiseString = "";
}

WCSimWCDAQMessenger::~WCSimWCDAQMessenger()
{
  delete DAQConstruct; //TODO remove this

  delete SaveFailuresTriggerDir;
  delete SaveFailuresTriggerMode;
  delete SaveFailuresTriggerTime;

  delete NDigitsTriggerDir;
  delete NDigitsTriggerThreshold;
  delete NDigitsTriggerWindow;
  delete NDigitsTriggerAdjustForNoise;

  delete DigitizerDir;
  delete DigitizerDeadTime;
  delete DigitizerIntegrationWindow;

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
    G4cout << "Digitizer choice set to " << newValue << initialiseString.c_str() << G4endl;
    WCSimEvent->SetDigitizerChoice(newValue);
    StoreDigitizerChoice = newValue;
  }
  else if (command == TriggerChoice) {
    G4cout << "Trigger choice set to " << newValue << initialiseString.c_str() << G4endl;
    WCSimEvent->SetTriggerChoice(newValue);
    StoreTriggerChoice = newValue;
  }

  //Generic digitizer options
  else if (command == DigitizerDeadTime) {
    G4cout << "Digitizer deadtime set to " << newValue << " ns" << initialiseString.c_str() << G4endl;
    StoreDigitizerDeadTime = DigitizerDeadTime->GetNewIntValue(newValue);
  }
  else if (command == DigitizerIntegrationWindow) {
    G4cout << "Digitizer integration window set to " << newValue << " ns" << initialiseString.c_str() << G4endl;
    StoreDigitizerIntegrationWindow = DigitizerIntegrationWindow->GetNewIntValue(newValue);
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
    G4cout << failuremode << initialiseString.c_str() << G4endl;
  }
  else if (command == SaveFailuresTriggerTime) {
    G4cout << "Trigger time for events which fail all triggers will be set to " << newValue << " ns" << initialiseString.c_str() << G4endl;
    StoreSaveFailuresTime = SaveFailuresTriggerTime->GetNewDoubleValue(newValue);
  }

  //NDigits trigger
  else if (command == NDigitsTriggerThreshold) {
    G4cout << "NDigits trigger threshold set to " << newValue << initialiseString.c_str() << G4endl;
    StoreNDigitsThreshold = NDigitsTriggerThreshold->GetNewIntValue(newValue);
  }
  else if (command == NDigitsTriggerAdjustForNoise) {
    StoreNDigitsAdjustForNoise = NDigitsTriggerAdjustForNoise->GetNewBoolValue(newValue);
    if(StoreNDigitsAdjustForNoise)
      G4cout << "Will adjust NDigits trigger threshold using average dark noise rate" << initialiseString.c_str() << G4endl;
  }
  else if (command == NDigitsTriggerWindow) {
    G4cout << "NDigits trigger window set to " << newValue << " ns" << initialiseString.c_str() << G4endl;
    StoreNDigitsWindow = NDigitsTriggerWindow->GetNewIntValue(newValue);
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
  G4cout << "\tTrigger time for events which fail all triggers will be set to " << StoreSaveFailuresTime << " ns" << G4endl;

  WCSimTrigger->SetNDigitsThreshold(StoreNDigitsThreshold);
  G4cout << "\tNDigits trigger threshold set to " << StoreNDigitsThreshold << G4endl;
  WCSimTrigger->SetNDigitsAdjustForNoise(StoreNDigitsAdjustForNoise);
  if(StoreNDigitsAdjustForNoise)
    G4cout << "\tWill adjust NDigits trigger threshold using average dark noise rate" << G4endl;
  WCSimTrigger->SetNDigitsWindow(StoreNDigitsWindow);
  G4cout << "\tNDigits trigger window set to " << StoreNDigitsWindow << " ns" << G4endl;
}

void WCSimWCDAQMessenger::SetDigitizerOptions()
{
  G4cout << "Passing Digitizer options to the digitizer class instance" << G4endl;
  if(StoreDigitizerDeadTime >= 0) {
    WCSimDigitize->SetDigitizerDeadTime(StoreDigitizerDeadTime);
    G4cout << "\tDigitizer deadtime set to " << StoreDigitizerDeadTime << " ns"  << G4endl;
  }
  if(StoreDigitizerIntegrationWindow >= 0) {
    WCSimDigitize->SetDigitizerIntegrationWindow(StoreDigitizerIntegrationWindow);
    G4cout << "\tDigitizer integration window set to " << StoreDigitizerIntegrationWindow << " ns" << G4endl;
  }
}
