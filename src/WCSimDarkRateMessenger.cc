#include "WCSimDarkRateMessenger.hh"
#include "WCSimWCDigitizer.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"

WCSimDarkRateMessenger::WCSimDarkRateMessenger(WCSimWCDigitizer* darkratepoint):WCSimDigitize(darkratepoint)
{
  WCSimDir = new G4UIdirectory("/DarkRate/");
  WCSimDir->SetGuidance("Commands to change the dark noise frequency of the simulation");
  
  SetFrequency = new G4UIcmdWithADoubleAndUnit("/DarkRate/SetDarkRate",this);
  SetFrequency->SetGuidance("Commands to change the dark noise frequency of the simulation");
  SetFrequency->SetParameterName("DarkRate",true);
  SetFrequency->SetDefaultValue(0);
  //kilohertz is 10e-6 (CLHEP units: 1kHz = 1000 / (1e9 ns) = 1e-6 as ns := 1.)
  SetFrequency->SetUnitCategory("Frequency");

  SetFrequency->SetDefaultUnit("kHz");

  
  SetFrequency->SetUnitCandidates("Hz kHz MHz GHz");

  SetConversionRate = new G4UIcmdWithADouble("/DarkRate/SetConvert",this);
  SetConversionRate->SetGuidance("Calibrate the frequency of dark noise after digitization");
  SetConversionRate->SetParameterName("DigiCorr",true);
  SetConversionRate->SetDefaultValue(1);

}

WCSimDarkRateMessenger::~WCSimDarkRateMessenger()
{

  delete SetFrequency;
  delete SetConversionRate;
  delete WCSimDir;
}

void WCSimDarkRateMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if(command == SetFrequency){
    // Since kHz is 10e-3 for this class we must multiply by a 10e6 factor
    // to make default units in kHz
    // ToDo: make this consistent with CLHEP unit system. Makes it easier to just using CLHEP!
    double const conversion_to_kHz = 1000000;
    WCSimDigitize->SetDarkRate(conversion_to_kHz * SetFrequency->GetNewDoubleValue(newValue));

    printf("Setting Dark Rate %f\n",conversion_to_kHz * SetFrequency->GetNewDoubleValue(newValue));

  }

  if(command == SetConversionRate){
    WCSimDigitize->SetConversion(SetConversionRate->GetNewDoubleValue(newValue));

    printf("Setting conversion value %f\n",SetConversionRate->GetNewDoubleValue(newValue));
  }


}
