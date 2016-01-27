#include "WCSimDarkRateMessenger.hh"
#include "WCSimWCAddDarkNoise.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"

WCSimDarkRateMessenger::WCSimDarkRateMessenger(WCSimWCAddDarkNoise* darkratepoint)
  : WCSimAddDarkNoise(darkratepoint)
{
  Initialize();
}

void WCSimDarkRateMessenger::Initialize()
{
  initaliseString = " (this is a default set; it may be overwritten by user commands)";

  WCSimDir = new G4UIdirectory("/DarkRate/");
  WCSimDir->SetGuidance("Commands to change the dark noise frequency of the simulation");
  
  double const conversion_to_kHz = 1000000;
  double defaultFrequency = 0;
  SetFrequency = new G4UIcmdWithADoubleAndUnit("/DarkRate/SetDarkRate",this);
  SetFrequency->SetGuidance("Commands to change the dark noise frequency of the simulation");
  SetFrequency->SetParameterName("DarkRate",false);
  SetFrequency->SetDefaultValue(defaultFrequency * conversion_to_kHz);
  //kilohertz is 10e-6 (CLHEP units: 1kHz = 1000 / (1e9 ns) = 1e-6 as ns := 1.)
  SetFrequency->SetUnitCategory("Frequency");
  SetFrequency->SetDefaultUnit("kHz");
  SetFrequency->SetUnitCandidates("Hz kHz MHz GHz");
 

  double defaultConvRate = 1;
  SetConversionRate = new G4UIcmdWithADouble("/DarkRate/SetConvert",this);
  SetConversionRate->SetGuidance("Calibrate the frequency of dark noise before applying the threshold (typically 0.25pe)"); 
  SetConversionRate->SetParameterName("DigiCorr",false);
  SetConversionRate->SetDefaultValue(defaultConvRate);
 
  //Mode 0 - Add dark rate in window defined by /DarkRate/SetDarkLow and /DarkRate/SetDarkHigh
  //If not set default is 0 and 100000ns
  //Mode 1 - Add dark rate to a window of size /DarkRate/SetDarkWindow around each hit
  int defaultDarkMode = 0;
  SetDarkMode = new G4UIcmdWithAnInteger("/DarkRate/SetDarkMode",this);
  SetDarkMode->SetGuidance("Set the mode of adding dark noise to the event");
  SetDarkMode->SetParameterName("DarkMode",false);
  SetDarkMode->SetDefaultValue(defaultDarkMode);
  SetNewValue(SetDarkMode, G4UIcommand::ConvertToString(defaultDarkMode));
  
  double defaultDarkLow = 0;
  SetDarkLow = new G4UIcmdWithADoubleAndUnit("/DarkRate/SetDarkLow",this);
  SetDarkLow->SetGuidance("Set the lower GEANT time limit to add dark noise");
  SetDarkLow->SetParameterName("DarkLow",false);
  SetDarkLow->SetDefaultValue(defaultDarkLow);
  SetDarkLow->SetUnitCategory("Time");
  SetDarkLow->SetDefaultUnit("ns");
  SetNewValue(SetDarkLow, G4UIcommand::ConvertToString(defaultDarkLow, "ns"));

  double defaultDarkHigh = 100000;
  SetDarkHigh = new G4UIcmdWithADoubleAndUnit("/DarkRate/SetDarkHigh",this);
  SetDarkHigh->SetGuidance("Set the upper GEANT time limit to add dark noise");
  SetDarkHigh->SetParameterName("DarkHigh",false);
  SetDarkHigh->SetDefaultValue(defaultDarkHigh);
  SetDarkHigh->SetUnitCategory("Time");
  SetDarkHigh->SetDefaultUnit("ns");
  SetNewValue(SetDarkHigh, G4UIcommand::ConvertToString(defaultDarkHigh, "ns"));

  double defaultDarkWindow = 5000;
  SetDarkWindow = new G4UIcmdWithADoubleAndUnit("/DarkRate/SetDarkWindow",this);
  SetDarkWindow->SetGuidance("Set the window width to add dark noise");
  SetDarkWindow->SetParameterName("DarkWindow",false);
  SetDarkWindow->SetDefaultValue(defaultDarkWindow);
  SetDarkWindow->SetUnitCategory("Time");
  SetDarkWindow->SetDefaultUnit("ns");
  SetNewValue(SetDarkWindow, G4UIcommand::ConvertToString(defaultDarkWindow, "ns"));

  initaliseString = "";
}

WCSimDarkRateMessenger::~WCSimDarkRateMessenger()
{

  delete SetFrequency;
  delete SetConversionRate;
  delete SetDarkMode;
  delete SetDarkLow;
  delete SetDarkHigh;
  delete SetDarkWindow;
  delete WCSimDir;
}

void WCSimDarkRateMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if(command == SetFrequency){
    // Since kHz is 10e-3 for this class we must multiply by a 10e6 factor
    // to make default units in kHz
    // ToDo: make this consistent with CLHEP unit system. Makes it easier to just using CLHEP!
    double const conversion_to_kHz = 1000000;
    WCSimAddDarkNoise->SetDarkRate(conversion_to_kHz * SetFrequency->GetNewDoubleValue(newValue));
    G4cout << "Setting Dark Rate " << conversion_to_kHz * SetFrequency->GetNewDoubleValue(newValue)
	   << initaliseString.c_str() << G4endl;
  }
  else if(command == SetConversionRate){
    WCSimAddDarkNoise->SetConversion(SetConversionRate->GetNewDoubleValue(newValue));
    G4cout << "Setting Dark Rate Conversion value " << SetConversionRate->GetNewDoubleValue(newValue)
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

}
