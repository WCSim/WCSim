#include "WCSimTuningMessenger.hh"
#include "WCSimTuningParameters.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithADouble.hh"

WCSimTuningMessenger::WCSimTuningMessenger(WCSimTuningParameters* WCTuningPars):WCSimTuningParams(WCTuningPars) { 
  WCSimDir = new G4UIdirectory("/WCSim/tuning/");
  WCSimDir->SetGuidance("Commands to change tuning parameters");

  Rayff = new G4UIcmdWithADouble("/WCSim/tuning/rayff",this);
  Rayff->SetGuidance("Set the Rayleigh scattering parameter");
  Rayff->SetParameterName("Rayff",true);
  Rayff->SetDefaultValue(0.606);


}

WCSimTuningMessenger::~WCSimTuningMessenger()
{
  delete Rayff;
  delete WCSimDir;
}

void WCSimTuningMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{    

  if(command == Rayff) {
	  // Set the Rayleigh scattering parameter
	  //	  printf("Input parameter %f\n",Rayff->GetNewDoubleValue(newValue));

  WCSimTuningParams->SetRayff(Rayff->GetNewDoubleValue(newValue));

  printf("Setting Rayleigh scattering parameter %f\n",Rayff->GetNewDoubleValue(newValue));

  }

}
