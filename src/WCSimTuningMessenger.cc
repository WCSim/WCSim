#include "WCSimTuningMessenger.hh"
#include "WCSimTuningParameters.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh" //jl145


WCSimTuningMessenger::WCSimTuningMessenger(WCSimTuningParameters* WCTuningPars):WCSimTuningParams(WCTuningPars) { 
  WCSimDir = new G4UIdirectory("/WCSim/tuning/");
  WCSimDir->SetGuidance("Commands to change tuning parameters");

  Rayff = new G4UIcmdWithADouble("/WCSim/tuning/rayff",this);
  Rayff->SetGuidance("Set the Rayleigh scattering parameter");
  Rayff->SetParameterName("Rayff",true);
  Rayff->SetDefaultValue(0.75);

  Bsrff = new G4UIcmdWithADouble("/WCSim/tuning/bsrff",this);
  Bsrff->SetGuidance("Set the Blacksheet reflection parameter");
  Bsrff->SetParameterName("Bsrff",true);
  Bsrff->SetDefaultValue(2.50);

  Abwff = new G4UIcmdWithADouble("/WCSim/tuning/abwff",this);
  Abwff->SetGuidance("Set the water attenuation parameter");
  Abwff->SetParameterName("Abwff",true);
  Abwff->SetDefaultValue(1.30);

  Rgcff = new G4UIcmdWithADouble("/WCSim/tuning/rgcff",this);
  Rgcff->SetGuidance("Set the cathode reflectivity parameter");
  Rgcff->SetParameterName("Rgcff",true);
  Rgcff->SetDefaultValue(0.32);

  Mieff = new G4UIcmdWithADouble("/WCSim/tuning/mieff",this);
  Mieff->SetGuidance("Set the Mie scattering parameter");
  Mieff->SetParameterName("Mieff",true);
  Mieff->SetDefaultValue(0.0);

  //jl145 - for Top Veto
  TVSpacing = new G4UIcmdWithADouble("/WCSim/tuning/tvspacing",this);
  TVSpacing->SetGuidance("Set the Top Veto PMT Spacing, in cm.");
  TVSpacing->SetParameterName("TVSpacing",true);
  TVSpacing->SetDefaultValue(100.0);

  TopVeto = new G4UIcmdWithABool("/WCSim/tuning/topveto",this);
  TopVeto->SetGuidance("Turn Top Veto simulation on/off");
  TopVeto->SetParameterName("TopVeto",true);
  TopVeto->SetDefaultValue(0);

}

WCSimTuningMessenger::~WCSimTuningMessenger()
{
  delete Rayff;
  delete Bsrff;
  delete Abwff;
  delete Rgcff;
  delete Mieff;

  //jl145 - for Top Veto
  delete TVSpacing;
  delete TopVeto;

  delete WCSimDir;
}

void WCSimTuningMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{    

  if(command == Rayff) {
	  // Set the Rayleigh scattering parameter
	  //	  G4cout << "Input parameter " << Rayff->GetNewDoubleValue(newValue) << G4endl;

  WCSimTuningParams->SetRayff(Rayff->GetNewDoubleValue(newValue));

  G4cout << "Setting Rayleigh scattering parameter " << Rayff->GetNewDoubleValue(newValue) << G4endl;

  }

 if(command == Bsrff) {
	  // Set the blacksheet reflection parameter
	  //	  G4cout << "Input parameter " << Bsrff->GetNewDoubleValue(newValue) << G4endl;

  WCSimTuningParams->SetBsrff(Bsrff->GetNewDoubleValue(newValue));

  G4cout << "Setting blacksheet reflection parameter " << Bsrff->GetNewDoubleValue(newValue) << G4endl;

  }

 if(command == Abwff) {
	  // Set the water attenuation  parameter
	  //	  G4cout << "Input parameter " << Abwff->GetNewDoubleValue(newValue) << G4endl;

  WCSimTuningParams->SetAbwff(Abwff->GetNewDoubleValue(newValue));

  G4cout << "Setting water attenuation parameter " << Abwff->GetNewDoubleValue(newValue) << G4endl;

  }

 if(command == Rgcff) {
	  // Set the cathode reflectivity parameter
	  //	  G4cout << "Input parameter " << Rgcff->GetNewDoubleValue(newValue) << G4endl;

  WCSimTuningParams->SetRgcff(Rgcff->GetNewDoubleValue(newValue));

  G4cout << "Setting cathode reflectivity parameter " << Rgcff->GetNewDoubleValue(newValue) << G4endl;

  }

 if(command == Mieff) {
	  // Set the Mie scattering parameter
	  //	  G4cout << "Input parameter " << Mieff->GetNewDoubleValue(newValue) << G4endl;

  WCSimTuningParams->SetMieff(Mieff->GetNewDoubleValue(newValue));

  G4cout << "Setting Mie scattering parameter " << Mieff->GetNewDoubleValue(newValue) << G4endl;

  }

  //jl145 - For Top Veto

  else if(command == TVSpacing) {
    // Set the Top Veto PMT Spacing
    WCSimTuningParams->SetTVSpacing(TVSpacing->GetNewDoubleValue(newValue));
    G4cout << "Setting Top Veto PMT Spacing " << TVSpacing->GetNewDoubleValue(newValue) << G4endl;
  }

  else if(command == TopVeto) {
    // Set the Top Veto on/off
    WCSimTuningParams->SetTopVeto(TopVeto->GetNewBoolValue(newValue));
    if(TopVeto->GetNewBoolValue(newValue))
      G4cout << "Setting Top Veto On" << G4endl;
    else
      G4cout << "Setting Top Veto Off" << G4endl;
  }


}
