#include "WCSimDarkRateMessenger.hh"
#include "WCSimWCDigitizer.hh"
#include "WCSimWCAddDarkNoise.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"

WCSimDarkRateMessenger::WCSimDarkRateMessenger(WCSimWCDigitizer* darkratepoint):WCSimDigitize(darkratepoint)
{
  //inform the DarkRateMessenger which constructor was called
  constructor = 0;

  Initialize();
}

WCSimDarkRateMessenger::WCSimDarkRateMessenger(WCSimWCAddDarkNoise* darkratepoint):WCSimAddDarkNoise(darkratepoint)
{
  //inform the DarkRateMessenger which constructor was called
  constructor = 1;

  Initialize();
}

void WCSimDarkRateMessenger::Initialize()
{
  WCSimDir = new G4UIdirectory("/DarkRate/");
  WCSimDir->SetGuidance("Commands to change the dark noise frequency of the simulation");
  
  double const conversion_to_kHz = 1000000;
  double defaultFrequency = 0;
  SetFrequency = new G4UIcmdWithADoubleAndUnit("/DarkRate/SetDarkRate",this);
  SetFrequency->SetGuidance("Commands to change the dark noise frequency of the simulation");
  SetFrequency->SetParameterName("DarkRate",true);
  SetFrequency->SetDefaultValue(defaultFrequency * conversion_to_kHz);
  //kilohertz is 10e-6
  SetFrequency->SetUnitCategory("Frequency");
  SetFrequency->SetDefaultUnit("kHz");
  SetFrequency->SetUnitCandidates("Hz kHz MHz GHz");
  if(constructor == 0)
    WCSimDigitize->SetDarkRate(defaultFrequency * conversion_to_kHz);
  else if(constructor == 1)
    WCSimAddDarkNoise->SetDarkRate(defaultFrequency * conversion_to_kHz);

  double defaultConvRate = 1;
  SetConversionRate = new G4UIcmdWithADouble("/DarkRate/SetConvert",this);
  SetConversionRate->SetGuidance("Caribrate the frequency of dark noise after digitization");
  SetConversionRate->SetParameterName("DigiCorr",true);
  SetConversionRate->SetDefaultValue(defaultConvRate);
  if(constructor == 0)
    WCSimDigitize->SetConversion(defaultConvRate);
  else if(constructor == 1)
    WCSimAddDarkNoise->SetConversion(defaultConvRate);

  //Mode 0 - Add dark rate in window defined by /DarkRate/SetDarkLow and /DarkRate/SetDarkHigh
  //If not set default is 0 and 100000ns
  //Mode 1 - Add dark rate to a window of size /DarkRate/SetDarkWindow around each hit
  int defaultDarkMode = 0;
  SetDarkMode = new G4UIcmdWithAnInteger("/DarkRate/SetDarkMode",this);
  SetDarkMode->SetGuidance("Set the mode of adding dark noise to the event");
  SetDarkMode->SetParameterName("DarkMode",true);
  SetDarkMode->SetDefaultValue(defaultDarkMode);
  if(constructor == 1)
    WCSimAddDarkNoise->SetDarkMode(defaultDarkMode);
  
  double defaultDarkLow = 0;
  SetDarkLow = new G4UIcmdWithADoubleAndUnit("/DarkRate/SetDarkLow",this);
  SetDarkLow->SetGuidance("Set the lower GEANT time limit to add dark noise");
  SetDarkLow->SetParameterName("DarkLow",true);
  SetDarkLow->SetDefaultValue(defaultDarkLow);
  SetDarkLow->SetUnitCategory("Time");
  SetDarkLow->SetDefaultUnit("ns");
  if(constructor == 1)
    WCSimAddDarkNoise->SetDarkLow(defaultDarkLow);

  double defaultDarkHigh = 100000;
  SetDarkHigh = new G4UIcmdWithADoubleAndUnit("/DarkRate/SetDarkHigh",this);
  SetDarkHigh->SetGuidance("Set the upper GEANT time limit to add dark noise");
  SetDarkHigh->SetParameterName("DarkHigh",true);
  SetDarkHigh->SetDefaultValue(defaultDarkHigh);
  SetDarkHigh->SetUnitCategory("Time");
  SetDarkHigh->SetDefaultUnit("ns");
  if(constructor == 1)
    WCSimAddDarkNoise->SetDarkHigh(defaultDarkHigh);

  double defaultDarkWindow = 5000;
  SetDarkWindow = new G4UIcmdWithADoubleAndUnit("/DarkRate/SetDarkWindow",this);
  SetDarkWindow->SetGuidance("Set the window width to add dark noise");
  SetDarkWindow->SetParameterName("DarkWindow",true);
  SetDarkWindow->SetDefaultValue(defaultDarkWindow);
  SetDarkWindow->SetUnitCategory("Time");
  SetDarkWindow->SetDefaultUnit("ns");
  if(constructor == 1)
    WCSimAddDarkNoise->SetDarkWindow(defaultDarkWindow);
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
    double const conversion_to_kHz = 1000000;
    if(constructor == 0)
      WCSimDigitize->SetDarkRate(conversion_to_kHz * SetFrequency->GetNewDoubleValue(newValue));
    else 
      WCSimAddDarkNoise->SetDarkRate(conversion_to_kHz * SetFrequency->GetNewDoubleValue(newValue));
    printf("Setting Dark Rate %f\n",conversion_to_kHz * SetFrequency->GetNewDoubleValue(newValue));

  }

  if(command == SetConversionRate){
    if(constructor == 0)
      WCSimDigitize->SetConversion(SetConversionRate->GetNewDoubleValue(newValue));
    else
      WCSimAddDarkNoise->SetConversion(SetConversionRate->GetNewDoubleValue(newValue));
    printf("Setting conversion value %f\n",SetConversionRate->GetNewDoubleValue(newValue));
  }

  if(command == SetDarkMode){
    if(constructor == 0){
      //      WCSimDigitize->SetDarkMode(SetDarkMode->GetNewDoubleValue(newValue));
      }
    else
      WCSimAddDarkNoise->SetDarkMode(SetDarkMode->GetNewIntValue(newValue));
    //printf("Setting DarkMode value %f\n",SetDarkMode->GetNewDoubleValue(newValue));
  }

  if(command == SetDarkLow){
    if(constructor == 0){
      //      WCSimDigitize->SetDarkLow(SetDarkLow->GetNewDoubleValue(newValue));
    }   
    else
      WCSimAddDarkNoise->SetDarkLow(SetDarkLow->GetNewDoubleValue(newValue));
    //printf("Setting DarkLow value %f\n",SetDarkLow->GetNewDoubleValue(newValue));
  }

  if(command == SetDarkHigh){
    if(constructor == 0) {
      //      WCSimDigitize->SetDarkHigh(SetDarkHigh->GetNewDoubleValue(newValue));
    }   
 else
      WCSimAddDarkNoise->SetDarkHigh(SetDarkHigh->GetNewDoubleValue(newValue));
    //printf("Setting DarkHigh value %f\n",SetDarkHigh->GetNewDoubleValue(newValue));
  }

  if(command == SetDarkWindow){
    if(constructor == 0){
      //      WCSimDigitize->SetDarkWidth(SetDarkWidth->GetNewDoubleValue(newValue));
      }
    else
      WCSimAddDarkNoise->SetDarkWindow(SetDarkWindow->GetNewDoubleValue(newValue));
    //printf("Setting DarkWidth value %f\n",SetDarkWidth->GetNewDoubleValue(newValue));
  }

}
