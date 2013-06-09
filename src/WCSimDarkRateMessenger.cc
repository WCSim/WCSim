#include "WCSimDarkRateMessenger.hh"
#include "WCSimWCDigitizer.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

WCSimDarkRateMessenger::WCSimDarkRateMessenger(WCSimWCDigitizer* darkratepoint):WCSimDigitize(darkratepoint)
{
  WCSimDir = new G4UIdirectory("/DarkRate/");
  WCSimDir->SetGuidance("Commands to change the dark noise frequency of the simulation");
  
  SetFrequency = new G4UIcmdWithADoubleAndUnit("/DarkRate/SetDarkRate",this);
  SetFrequency->SetGuidance("Commands to change the dark noise frequency of the simulation");
  SetFrequency->SetParameterName("DarkRate",true);
  SetFrequency->SetDefaultValue(0);
  //kilohertz is 10e-6
  SetFrequency->SetUnitCategory("Frequency");

  SetFrequency->SetDefaultUnit("kHz");

  
  SetFrequency->SetUnitCandidates("Hz kHz MHz GHz");
}

WCSimDarkRateMessenger::~WCSimDarkRateMessenger()
{

  delete SetFrequency;
  delete WCSimDir;
}

void WCSimDarkRateMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if(command == SetFrequency){
    // Since kHz is 10e-3 for this class we must multiply by a 10e6 factor
    // to make default units in kHz
    double const conversion_to_kHz = 1000000;
    WCSimDigitize->SetDarkRate(conversion_to_kHz * SetFrequency->GetNewDoubleValue(newValue));

    printf("Setting Dark Rate %f\n",conversion_to_kHz * SetFrequency->GetNewDoubleValue(newValue));

  }

}
