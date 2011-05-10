#include "WCSimRandomMessenger.hh"
#include "WCSimRandomParameters.hh"

#include "G4UIdirectory.hh"
#include "G4ios.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"

WCSimRandomMessenger::WCSimRandomMessenger(WCSimRandomParameters* WCRandomPars):WCSimRandomParams(WCRandomPars) 
{
  WCSimDir = new G4UIdirectory("/WCSim/random/");
  WCSimDir->SetGuidance("Commands to set the random number generator parameters");

  Rangen = new G4UIcmdWithAString("/WCSim/random/generator",this);
  Rangen->SetGuidance("Sets the random number generator.  Possible values are: RANLUX, RANECU, and HEPJAMES (the G4 default)");
  Rangen->SetParameterName("generator",true);
  Rangen->SetDefaultValue("HEPJAMES");

  Ranseed = new G4UIcmdWithAnInteger("/WCSim/random/seed",this);
  Ranseed->SetGuidance("Sets the random number seed (integer)");
  Ranseed->SetParameterName("Ranseed",true);
  Ranseed->SetDefaultValue(31415);
}

WCSimRandomMessenger::~WCSimRandomMessenger()
{
  delete Ranseed;
  delete WCSimDir;
}

void WCSimRandomMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if (command == Rangen)
    {
      if (newValue == "RANLUX") WCSimRandomParams->SetGenerator(RANDOM_E_RANLUX);
      if (newValue == "RANECU") WCSimRandomParams->SetGenerator(RANDOM_E_RANECU);
      if (newValue == "HEPJAMES") WCSimRandomParams->SetGenerator(RANDOM_E_HEPJAMES);
    }
  else if (command == Ranseed)
    {
      WCSimRandomParams->SetSeed(Ranseed->GetNewIntValue(newValue));
    }
}

