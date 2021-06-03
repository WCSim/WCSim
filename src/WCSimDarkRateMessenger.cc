#include "WCSimDarkRateMessenger.hh"
#include "WCSimWCAddDarkNoise.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"

WCSimDarkRateMessenger::WCSimDarkRateMessenger(WCSimWCAddDarkNoise* darkratepoint)
  : WCSimAddDarkNoise(darkratepoint), WCSimDir(0), SetFrequency(0), SetFrequency2(0), SetConversionRate(0), SetConversionRate2(0), SetDarkMode(0), SetDarkLow(0), SetDarkHigh(0), SetDarkWindow(0), SetDetectorElement(0)
{
  //Initialize();
}

WCSimDarkRateMessenger* WCSimDarkRateMessenger::iInstance = NULL;

WCSimDarkRateMessenger* WCSimDarkRateMessenger::GetInstance(){
  //G4cout<<"making the static instance"<<G4endl;
  //static WCSimDarkRateMessenger* iInstance = new WCSimDarkRateMessenger(0);
  if(iInstance==NULL){
    iInstance = new WCSimDarkRateMessenger(0);
  }
  //G4cout<<"returning static instance at "<<iInstance<<G4endl;
  return iInstance;
}

void WCSimDarkRateMessenger::Initialize()
{
  G4cout << "Initializing Dark Rate Messenger for detector element " << detectorElement << G4endl;
  initaliseString = " (this is a default set; it may be overwritten by user commands)";

  if(WCSimDir==0){WCSimDir = new G4UIdirectory("/DarkRate/");}
  WCSimDir->SetGuidance("Commands to change the dark noise frequency of the simulation");
  
  double const conversion_to_kHz = 1000000;
  double defaultFrequency = 0;
  if(SetFrequency==0){SetFrequency = new G4UIcmdWithADoubleAndUnit("/DarkRate/SetDarkRate",this);}
  SetFrequency->SetGuidance("Commands to change the dark noise frequency of the simulation");
  SetFrequency->SetParameterName("DarkRate",false);
  SetFrequency->SetDefaultValue(defaultFrequency * conversion_to_kHz);
  //kilohertz is 10e-6 (CLHEP units: 1kHz = 1000 / (1e9 ns) = 1e-6 as ns := 1.)
  SetFrequency->SetUnitCategory("Frequency");
  SetFrequency->SetDefaultUnit("kHz");
  SetFrequency->SetUnitCandidates("Hz kHz MHz GHz");
 
  if(SetFrequency2==0){SetFrequency2 = new G4UIcmdWithADoubleAndUnit("/DarkRate/SetDarkRate2",this);}
  SetFrequency2->SetGuidance("Commands to change the dark noise frequency of the simulation for detectorElem2");
  SetFrequency2->SetParameterName("DarkRate",false);
  SetFrequency2->SetDefaultValue(defaultFrequency * conversion_to_kHz);
  //kilohertz is 10e-6 (CLHEP units: 1kHz = 1000 / (1e9 ns) = 1e-6 as ns := 1.)
  SetFrequency2->SetUnitCategory("Frequency");
  SetFrequency2->SetDefaultUnit("kHz");
  SetFrequency2->SetUnitCandidates("Hz kHz MHz GHz");

  double defaultConvRate = 1;
  if(SetConversionRate==0){ SetConversionRate = new G4UIcmdWithADouble("/DarkRate/SetConvert",this);}
  SetConversionRate->SetGuidance("Calibrate the frequency of dark noise before applying the threshold (typically 0.25pe)"); 
  SetConversionRate->SetParameterName("DigiCorr",false);
  SetConversionRate->SetDefaultValue(defaultConvRate);
 
  if(SetConversionRate2==0){ SetConversionRate2 = new G4UIcmdWithADouble("/DarkRate/SetConvert2",this);}
  SetConversionRate2->SetGuidance("Calibrate the frequency of dark noise before applying the threshold (typically 0.25pe) for detectorElem2"); 
  SetConversionRate2->SetParameterName("DigiCorr",false);
  SetConversionRate2->SetDefaultValue(defaultConvRate);

  //Mode 0 - Add dark rate in window defined by /DarkRate/SetDarkLow and /DarkRate/SetDarkHigh
  //If not set default is 0 and 100000ns
  //Mode 1 - Add dark rate to a window of size /DarkRate/SetDarkWindow around each hit
  int defaultDarkMode = 0;
  if(SetDarkMode==0){SetDarkMode = new G4UIcmdWithAnInteger("/DarkRate/SetDarkMode",this);}
  SetDarkMode->SetGuidance("Set the mode of adding dark noise to the event");
  SetDarkMode->SetParameterName("DarkMode",false);
  SetDarkMode->SetDefaultValue(defaultDarkMode);
  SetNewValue(SetDarkMode, G4UIcommand::ConvertToString(defaultDarkMode));
  
  double defaultDarkLow = 0;
  if(SetDarkLow==0){SetDarkLow = new G4UIcmdWithADoubleAndUnit("/DarkRate/SetDarkLow",this);}
  SetDarkLow->SetGuidance("Set the lower GEANT time limit to add dark noise");
  SetDarkLow->SetParameterName("DarkLow",false);
  SetDarkLow->SetDefaultValue(defaultDarkLow);
  SetDarkLow->SetUnitCategory("Time");
  SetDarkLow->SetDefaultUnit("ns");
  SetNewValue(SetDarkLow, G4UIcommand::ConvertToString(defaultDarkLow, "ns"));

  double defaultDarkHigh = 100000;
  if(SetDarkHigh==0){SetDarkHigh = new G4UIcmdWithADoubleAndUnit("/DarkRate/SetDarkHigh",this);}
  SetDarkHigh->SetGuidance("Set the upper GEANT time limit to add dark noise");
  SetDarkHigh->SetParameterName("DarkHigh",false);
  SetDarkHigh->SetDefaultValue(defaultDarkHigh);
  SetDarkHigh->SetUnitCategory("Time");
  SetDarkHigh->SetDefaultUnit("ns");
  SetNewValue(SetDarkHigh, G4UIcommand::ConvertToString(defaultDarkHigh, "ns"));

  double defaultDarkWindow = 5000;
  if(SetDarkWindow==0){SetDarkWindow = new G4UIcmdWithADoubleAndUnit("/DarkRate/SetDarkWindow",this);}
  SetDarkWindow->SetGuidance("Set the window width to add dark noise");
  SetDarkWindow->SetParameterName("DarkWindow",false);
  SetDarkWindow->SetDefaultValue(defaultDarkWindow);
  SetDarkWindow->SetUnitCategory("Time");
  SetDarkWindow->SetDefaultUnit("ns");
  SetNewValue(SetDarkWindow, G4UIcommand::ConvertToString(defaultDarkWindow, "ns"));

  G4String defaultDetectorElement = "tank";
  if(SetDetectorElement==0){ SetDetectorElement = new G4UIcmdWithAString("/DarkRate/SetDetectorElement", this); }
  SetDetectorElement->SetGuidance("Set the detector element for which trigger settings will be applied");
  G4String theguidance = "Available choices are:\n";
  G4String thecandidates="";
  for(std::map<std::string, WCSimWCAddDarkNoise*>::iterator it=darknoiseptrs.begin(); it!=darknoiseptrs.end(); ++it){
    theguidance+=it->first; theguidance+="\n";
    thecandidates+=it->first; thecandidates+=" ";
  }
  SetDetectorElement->SetGuidance(theguidance);
  SetDetectorElement->SetCandidates(thecandidates);
  SetDetectorElement->SetParameterName("detectorElement", false);
  SetDetectorElement->AvailableForStates(G4State_PreInit, G4State_Idle);
  SetDetectorElement->SetDefaultValue(defaultDetectorElement);

  initaliseString = "";
}

WCSimDarkRateMessenger::~WCSimDarkRateMessenger()
{

  delete SetFrequency;
  delete SetFrequency2;
  delete SetConversionRate;
  delete SetConversionRate2;
  delete SetDarkMode;
  delete SetDarkLow;
  delete SetDarkHigh;
  delete SetDarkWindow;
  delete WCSimDir;
  delete SetDetectorElement;
}

void WCSimDarkRateMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  for(std::map<std::string, WCSimWCAddDarkNoise*>::iterator it=darknoiseptrs.begin(); it!=darknoiseptrs.end(); ++it){
    WCSimAddDarkNoise = (WCSimWCAddDarkNoise*) it->second;    

    if(command == SetFrequency && it->first == "tank"){
      // Since kHz is 10e-3 for this class we must multiply by a 10e6 factor
      // to make default units in kHz
      // ToDo: make this consistent with CLHEP unit system. Makes it easier to just using CLHEP!
      double const conversion_to_kHz = 1000000;
      WCSimAddDarkNoise->SetDarkRate(conversion_to_kHz * SetFrequency->GetNewDoubleValue(newValue));
      G4cout << "Setting Dark Rate " << conversion_to_kHz * SetFrequency->GetNewDoubleValue(newValue)
        << initaliseString.c_str() << G4endl;
    }
    else if(command == SetFrequency2 && it->first == "tankPMT2"){
      // Since kHz is 10e-3 for this class we must multiply by a 10e6 factor
      // to make default units in kHz
      // ToDo: make this consistent with CLHEP unit system. Makes it easier to just using CLHEP!
      double const conversion_to_kHz = 1000000;
      WCSimAddDarkNoise->SetDarkRate(conversion_to_kHz * SetFrequency2->GetNewDoubleValue(newValue));
      G4cout << "Setting Dark Rate " << conversion_to_kHz * SetFrequency2->GetNewDoubleValue(newValue)
        << initaliseString.c_str() << G4endl;
    }
    else if(command == SetConversionRate && it->first == "tank"){
      WCSimAddDarkNoise->SetConversion(SetConversionRate->GetNewDoubleValue(newValue));
      G4cout << "Setting Dark Rate Conversion value " << SetConversionRate->GetNewDoubleValue(newValue)
        << initaliseString.c_str() << G4endl;
    }
    else if(command == SetConversionRate2 && it->first == "tankPMT2"){
      WCSimAddDarkNoise->SetConversion(SetConversionRate2->GetNewDoubleValue(newValue));
      G4cout << "Setting Dark Rate Conversion value " << SetConversionRate2->GetNewDoubleValue(newValue)
        << initaliseString.c_str() << G4endl;
    }
    else if(command == SetDarkMode){
      WCSimAddDarkNoise->SetDarkMode(SetDarkMode->GetNewIntValue(newValue));
      G4cout << "Setting DarkMode value " << SetDarkMode->GetNewIntValue(newValue)
        << initaliseString.c_str() << G4endl;
    }
    else if(command == SetDarkLow){
      WCSimAddDarkNoise->SetDarkLow(SetDarkLow->GetNewDoubleValue(newValue));
      G4cout << "Setting DarkLow value " << SetDarkLow->GetNewDoubleValue(newValue)
        << initaliseString.c_str() << G4endl;
    }
    else if(command == SetDarkHigh){
      WCSimAddDarkNoise->SetDarkHigh(SetDarkHigh->GetNewDoubleValue(newValue));
      G4cout << "Setting DarkHigh value " << SetDarkHigh->GetNewDoubleValue(newValue)
        << initaliseString.c_str() << G4endl;
    }
    else if(command == SetDarkWindow){
      WCSimAddDarkNoise->SetDarkWindow(SetDarkWindow->GetNewDoubleValue(newValue));
      G4cout << "Setting DarkWindow value " << SetDarkWindow->GetNewDoubleValue(newValue)
        << initaliseString.c_str() << G4endl;
    }
    else if(command == SetDetectorElement){
      if(darknoiseptrs.count(newValue)>0){
        WCSimAddDarkNoise=darknoiseptrs[newValue];
        G4cout << "Setting detectorElement value " << newValue
          << initaliseString.c_str() << G4endl;
      }
    }
  }
}

void WCSimDarkRateMessenger::AddDarkRateInstance(WCSimWCAddDarkNoise* darkratepoint, G4String detectorElementin){
  if(darknoiseptrs.count(detectorElementin)>0){
    G4cout<<"new dark rate messenger for existing detector Element "<<detectorElementin<<G4endl;
  } else {
    G4cout<<"added "<<detectorElementin<<" to the DarkRateMessenger"<<G4endl;
    darknoiseptrs.insert(std::pair<std::string,WCSimWCAddDarkNoise*>(detectorElementin,darkratepoint));
    WCSimAddDarkNoise=darkratepoint;
    detectorElement=detectorElementin;
  }
}

void WCSimDarkRateMessenger::RemoveDarkRateInstance(G4String detectorElementin){
  if(darknoiseptrs.count(detectorElementin)>0){
    G4cout<<"deleting dark rate messenger for detector Element "<<detectorElementin<<G4endl;
    std::map<std::string, WCSimWCAddDarkNoise*>::iterator thepointer = darknoiseptrs.find(detectorElementin);
    darknoiseptrs.erase(thepointer);
    if(darknoiseptrs.size()==0){ delete this; }
  } else {
    G4cout<<"Attempt to remove nonexistant element "<<detectorElementin<<" from DarkRateMessenger!"<<G4endl;
  }
}
