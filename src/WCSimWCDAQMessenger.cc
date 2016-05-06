#include "WCSimWCDAQMessenger.hh"
#include "WCSimEventAction.hh"
#include "WCSimWCDigitizer.hh"
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
  initialised = false;

  WCSimDAQDir = new G4UIdirectory("/DAQ/");
  WCSimDAQDir->SetGuidance("Commands to select DAQ options");

  G4String defaultDigitizer = "SKI";
  DigitizerChoice = new G4UIcmdWithAString("/DAQ/Digitizer", this);
  DigitizerChoice->SetGuidance("Set the Digitizer type");
  DigitizerChoice->SetGuidance("Available choices are:\n"
			       "SKI\n"
			       );
  DigitizerChoice->SetParameterName("Digitizer", false);
  DigitizerChoice->SetCandidates(
				 "SKI "
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
			     );
  TriggerChoice->SetParameterName("Trigger", false);
  TriggerChoice->SetCandidates(
			       "NDigits "
			       "NDigits2 "
			       );
  TriggerChoice->AvailableForStates(G4State_PreInit, G4State_Idle);
  TriggerChoice->SetDefaultValue(defaultTrigger);
  StoreTriggerChoice = defaultTrigger;
  SetNewValue(TriggerChoice, defaultTrigger);

  bool defaultMultiDigitsPerTrigger = false;
  MultiDigitsPerTrigger = new G4UIcmdWithABool("/DAQ/MultiDigitsPerTrigger", this);
  MultiDigitsPerTrigger->SetGuidance("Allow the number of digits per PMT per trigger to be > 1?");
  MultiDigitsPerTrigger->SetParameterName("MultiDigitsPerTrigger",true);
  MultiDigitsPerTrigger->SetDefaultValue(defaultMultiDigitsPerTrigger);
  StoreMultiDigitsPerTrigger = defaultMultiDigitsPerTrigger;
  MultiDigitsPerTriggerSet = false; //this variable is bool & defaults are class specfic; use this to know if the default is overidden
  //don't SetNewValue -> defaults class-specific and taken from GetDefault*()


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

  int defaultSaveFailuresPreTriggerWindow = -400;
  SaveFailuresPreTriggerWindow = new G4UIcmdWithAnInteger("/DAQ/TriggerSaveFailures/PreTriggerWindow", this);
  SaveFailuresPreTriggerWindow->SetGuidance("Set the SaveFailures pretrigger window (in ns)");
  SaveFailuresPreTriggerWindow->SetParameterName("SaveFailuresPreTriggerWindow",false);
  SaveFailuresPreTriggerWindow->SetDefaultValue(defaultSaveFailuresPreTriggerWindow);
  StoreSaveFailuresPreWindow = defaultSaveFailuresPreTriggerWindow;
  SetNewValue(SaveFailuresPreTriggerWindow, G4UIcommand::ConvertToString(defaultSaveFailuresPreTriggerWindow));

  int defaultSaveFailuresPostTriggerWindow = 950;
  SaveFailuresPostTriggerWindow = new G4UIcmdWithAnInteger("/DAQ/TriggerSaveFailures/PostTriggerWindow", this);
  SaveFailuresPostTriggerWindow->SetGuidance("Set the SaveFailures posttrigger window (in ns)");
  SaveFailuresPostTriggerWindow->SetParameterName("SaveFailuresPostTriggerWindow",false);
  SaveFailuresPostTriggerWindow->SetDefaultValue(defaultSaveFailuresPostTriggerWindow);
  StoreSaveFailuresPostWindow = defaultSaveFailuresPostTriggerWindow;
  SetNewValue(SaveFailuresPostTriggerWindow, G4UIcommand::ConvertToString(defaultSaveFailuresPostTriggerWindow));


  //NDigits trigger specifc options
  NDigitsTriggerDir = new G4UIdirectory("/DAQ/TriggerNDigits/");
  NDigitsTriggerDir->SetGuidance("Commands specific to the NDigits trigger");
  
  int defaultNDigitsTriggerThreshold = -99;
  NDigitsTriggerThreshold = new G4UIcmdWithAnInteger("/DAQ/TriggerNDigits/Threshold", this);
  NDigitsTriggerThreshold->SetGuidance("Set the NDigits trigger threshold");
  NDigitsTriggerThreshold->SetParameterName("NDigitsThreshold",false);
  NDigitsTriggerThreshold->SetDefaultValue(defaultNDigitsTriggerThreshold);
  StoreNDigitsThreshold = defaultNDigitsTriggerThreshold;
  //don't SetNewValue -> defaults class-specific and taken from GetDefault*()

  int defaultNDigitsTriggerWindow = -99;
  NDigitsTriggerWindow = new G4UIcmdWithAnInteger("/DAQ/TriggerNDigits/Window", this);
  NDigitsTriggerWindow->SetGuidance("Set the NDigits trigger window (in ns)");
  NDigitsTriggerWindow->SetParameterName("NDigitsWindow",false);
  NDigitsTriggerWindow->SetDefaultValue(defaultNDigitsTriggerWindow);
  StoreNDigitsWindow = defaultNDigitsTriggerWindow;
  //don't SetNewValue -> defaults class-specific and taken from GetDefault*()

  bool defaultNDigitsTriggerAdjustForNoise = true;
  NDigitsTriggerAdjustForNoise = new G4UIcmdWithABool("/DAQ/TriggerNDigits/AdjustForNoise", this);
  NDigitsTriggerAdjustForNoise->SetGuidance("Adjust the NDigits trigger threshold automatically dependent on the average noise rate");
  NDigitsTriggerAdjustForNoise->SetParameterName("NDigitsAdjustForNoise",true);
  NDigitsTriggerAdjustForNoise->SetDefaultValue(defaultNDigitsTriggerAdjustForNoise);
  StoreNDigitsAdjustForNoise = defaultNDigitsTriggerAdjustForNoise;
  SetNewValue(NDigitsTriggerAdjustForNoise, G4UIcommand::ConvertToString(defaultNDigitsTriggerAdjustForNoise));

  int defaultNDigitsPreTriggerWindow = -99;
  NDigitsPreTriggerWindow = new G4UIcmdWithAnInteger("/DAQ/TriggerNDigits/PreTriggerWindow", this);
  NDigitsPreTriggerWindow->SetGuidance("Set the NDigits pretrigger window (in ns)");
  NDigitsPreTriggerWindow->SetParameterName("NDigitsPreTriggerWindow",false);
  NDigitsPreTriggerWindow->SetDefaultValue(defaultNDigitsPreTriggerWindow);
  StoreNDigitsPreWindow = defaultNDigitsPreTriggerWindow;
  //don't SetNewValue -> defaults class-specific and taken from GetDefault*()

  int defaultNDigitsPostTriggerWindow = -99;
  NDigitsPostTriggerWindow = new G4UIcmdWithAnInteger("/DAQ/TriggerNDigits/PostTriggerWindow", this);
  NDigitsPostTriggerWindow->SetGuidance("Set the NDigits posttrigger window (in ns)");
  NDigitsPostTriggerWindow->SetParameterName("NDigitsPostTriggerWindow",false);
  NDigitsPostTriggerWindow->SetDefaultValue(defaultNDigitsPostTriggerWindow);
  StoreNDigitsPostWindow = defaultNDigitsPostTriggerWindow;
  //don't SetNewValue -> defaults class-specific and taken from GetDefault*()

  initialiseString = "";
  initialised = true;
}

WCSimWCDAQMessenger::~WCSimWCDAQMessenger()
{
  delete SaveFailuresTriggerDir;
  delete SaveFailuresTriggerMode;
  delete SaveFailuresTriggerTime;
  delete SaveFailuresPreTriggerWindow;
  delete SaveFailuresPostTriggerWindow;

  delete NDigitsTriggerDir;
  delete NDigitsTriggerThreshold;
  delete NDigitsTriggerWindow;
  delete NDigitsTriggerAdjustForNoise;
  delete NDigitsPreTriggerWindow;
  delete NDigitsPostTriggerWindow;

  delete DigitizerDir;
  delete DigitizerDeadTime;
  delete DigitizerIntegrationWindow;

  delete DigitizerChoice;
  delete TriggerChoice;
  delete MultiDigitsPerTrigger;
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
  else if (command == MultiDigitsPerTrigger) {
    StoreMultiDigitsPerTrigger = MultiDigitsPerTrigger->GetNewBoolValue(newValue);
    if(!StoreMultiDigitsPerTrigger)
      G4cout << "Will restrict number of digits per PMT per trigger to <= 1" << initialiseString.c_str() << G4endl;
    else
      G4cout << "Will allow number of digits per PMT per trigger to go > 1" << initialiseString.c_str() << G4endl;
    if(initialised)
      MultiDigitsPerTriggerSet = true;
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
  else if (command == SaveFailuresPreTriggerWindow) {
    G4cout << "SaveFailures pretrigger window set to " << newValue << " ns" << initialiseString.c_str() << G4endl;
    StoreSaveFailuresPreWindow = SaveFailuresPreTriggerWindow->GetNewIntValue(newValue);
  }
  else if (command == SaveFailuresPostTriggerWindow) {
    G4cout << "SaveFailures posttrigger window set to " << newValue << " ns" << initialiseString.c_str() << G4endl;
    StoreSaveFailuresPostWindow = SaveFailuresPostTriggerWindow->GetNewIntValue(newValue);
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
  else if (command == NDigitsPreTriggerWindow) {
    G4cout << "NDigits pretrigger window set to " << newValue << " ns" << initialiseString.c_str() << G4endl;
    StoreNDigitsPreWindow = NDigitsPreTriggerWindow->GetNewIntValue(newValue);
  }
  else if (command == NDigitsPostTriggerWindow) {
    G4cout << "NDigits posttrigger window set to " << newValue << " ns" << initialiseString.c_str() << G4endl;
    StoreNDigitsPostWindow = NDigitsPostTriggerWindow->GetNewIntValue(newValue);
  }
}

void WCSimWCDAQMessenger::SetTriggerOptions()
{
  G4cout << "Passing Trigger options to the trigger class instance" << G4endl;

  if(MultiDigitsPerTriggerSet) {
    WCSimTrigger->SetMultiDigitsPerTrigger(StoreMultiDigitsPerTrigger);
    if(!StoreMultiDigitsPerTrigger)
      G4cout << "\tWill restrict number of digits per PMT per trigger to <= 1" << G4endl;
    else
      G4cout << "\tWill allow number of digits per PMT per trigger to go > 1" << initialiseString.c_str() << G4endl;
  }

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
  if(StoreSaveFailuresPreWindow >= -1E6) {
    WCSimTrigger->SetSaveFailuresPreTriggerWindow(StoreSaveFailuresPreWindow);
    G4cout << "\tSaveFailures pretrigger window set to " << StoreSaveFailuresPreWindow << " ns" << G4endl;
  }
  if(StoreSaveFailuresPostWindow >= 0) {
    WCSimTrigger->SetSaveFailuresPostTriggerWindow(StoreSaveFailuresPostWindow);
    G4cout << "\tSaveFailures posttrigger window set to " << StoreSaveFailuresPostWindow << " ns" << G4endl;
  }

  if(StoreNDigitsThreshold >= 0) {
   WCSimTrigger->SetNDigitsThreshold(StoreNDigitsThreshold);
    G4cout << "\tNDigits trigger threshold set to " << StoreNDigitsThreshold << G4endl;
  }
  WCSimTrigger->SetNDigitsAdjustForNoise(StoreNDigitsAdjustForNoise);
  if(StoreNDigitsAdjustForNoise)
    G4cout << "\tWill adjust NDigits trigger threshold using average dark noise rate" << G4endl;
  if(StoreNDigitsWindow >= 0) {
    WCSimTrigger->SetNDigitsWindow(StoreNDigitsWindow);
    G4cout << "\tNDigits trigger window set to " << StoreNDigitsWindow << " ns" << G4endl;
  }
  if(StoreNDigitsPreWindow >= 0) {
    WCSimTrigger->SetNDigitsPreTriggerWindow(StoreNDigitsPreWindow);
    G4cout << "\tNDigits pretrigger window set to " << StoreNDigitsPreWindow << " ns" << G4endl;
  }
  if(StoreNDigitsPostWindow >= 0) {
    WCSimTrigger->SetNDigitsPostTriggerWindow(StoreNDigitsPostWindow);
    G4cout << "\tNDigits posttrigger window set to " << StoreNDigitsPostWindow << " ns" << G4endl;
  }
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
