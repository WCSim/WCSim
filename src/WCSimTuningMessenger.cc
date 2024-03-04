#include "WCSimTuningMessenger.hh"
#include "WCSimTuningParameters.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"


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

  TVSpacing = new G4UIcmdWithADouble("/WCSim/tuning/tvspacing",this);
  TVSpacing->SetGuidance("Set the Top Veto PMT Spacing, in cm.");
  TVSpacing->SetParameterName("TVSpacing",true);
  TVSpacing->SetDefaultValue(100.0);

  TopVeto = new G4UIcmdWithABool("/WCSim/tuning/topveto",this);
  TopVeto->SetGuidance("Turn Top Veto simulation on/off");
  TopVeto->SetParameterName("TopVeto",true);
  TopVeto->SetDefaultValue(0);

  CommandWCODWLSCladdingReflectivity = new G4UIcmdWithADouble("/WCSim/tuning/WCODWLSCladdingReflectivity",this);
  CommandWCODWLSCladdingReflectivity->SetGuidance("Set OD WLS plate cladding reflectivity");
  CommandWCODWLSCladdingReflectivity->SetParameterName("WCODWLSCladdingReflectivity",true);
  CommandWCODWLSCladdingReflectivity->SetDefaultValue(0.90);

  CommandWCODTyvekReflectivityInWall = new G4UIcmdWithADouble("/WCSim/tuning/WCODTyvekReflectivityInWall",this);
  CommandWCODTyvekReflectivityInWall->SetGuidance("Set OD tyvek cladding reflectivity on OD in-wall");
  CommandWCODTyvekReflectivityInWall->SetParameterName("WCODTyvekReflectivityInWall",true);
  CommandWCODTyvekReflectivityInWall->SetDefaultValue(0.90);

  CommandWCODTyvekReflectivityOutWall = new G4UIcmdWithADouble("/WCSim/tuning/WCODTyvekReflectivityOutWall",this);
  CommandWCODTyvekReflectivityOutWall->SetGuidance("Set OD tyvek reflectivity on OD out-wall");
  CommandWCODTyvekReflectivityOutWall->SetParameterName("WCODTyvekReflectivityOutWall",true);
  CommandWCODTyvekReflectivityOutWall->SetDefaultValue(0.90);
}

WCSimTuningMessenger::~WCSimTuningMessenger()
{
  delete Rayff;
  delete Bsrff;
  delete Abwff;
  delete Rgcff;
  delete Mieff;

  delete TVSpacing;
  delete TopVeto;

  delete CommandWCODWLSCladdingReflectivity;
  delete CommandWCODTyvekReflectivityInWall;
  delete CommandWCODTyvekReflectivityOutWall;

  delete WCSimDir;
}

void WCSimTuningMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{    

  if(command == Rayff) {
    // Set the Rayleigh scattering parameter
    WCSimTuningParams->SetRayff(Rayff->GetNewDoubleValue(newValue));
    G4cout << "Setting Rayleigh scattering parameter " << Rayff->GetNewDoubleValue(newValue) << G4endl;
  }

  else if(command == Bsrff) {
    // Set the blacksheet reflection parameter
    WCSimTuningParams->SetBsrff(Bsrff->GetNewDoubleValue(newValue));
    G4cout << "Setting blacksheet reflection parameter " << Bsrff->GetNewDoubleValue(newValue) << G4endl;
  }

  else if(command == Abwff) {
    // Set the water attenuation  parameter
    WCSimTuningParams->SetAbwff(Abwff->GetNewDoubleValue(newValue));
    G4cout << "Setting water attenuation parameter " << Abwff->GetNewDoubleValue(newValue) << G4endl;
  }

  else if(command == Rgcff) {
    // Set the cathode reflectivity parameter
    WCSimTuningParams->SetRgcff(Rgcff->GetNewDoubleValue(newValue));
    G4cout << "Setting cathode reflectivity parameter " << Rgcff->GetNewDoubleValue(newValue) << G4endl;
  }

  else if(command == Mieff) {
    // Set the Mie scattering parameter
    WCSimTuningParams->SetMieff(Mieff->GetNewDoubleValue(newValue));
    G4cout << "Setting Mie scattering parameter " << Mieff->GetNewDoubleValue(newValue) << G4endl;
  }

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

  else if(command == CommandWCODWLSCladdingReflectivity) {
    // Set the Top Veto PMT Spacing
    WCSimTuningParams->SetWCODWLSCladdingReflectivity(CommandWCODWLSCladdingReflectivity->GetNewDoubleValue(newValue));
    printf("Setting OD WLS plate cladding reflectivity %f\n",CommandWCODWLSCladdingReflectivity->GetNewDoubleValue(newValue));
  }

  else if(command == CommandWCODTyvekReflectivityInWall) {
    // Set the Top Veto PMT Spacing
    WCSimTuningParams->SetWCODTyvekReflectivityInWall(CommandWCODTyvekReflectivityInWall->GetNewDoubleValue(newValue));
    printf("Setting OD tyvek reflectivity on in-wall %f\n",CommandWCODTyvekReflectivityInWall->GetNewDoubleValue(newValue));
  }
  else if(command == CommandWCODTyvekReflectivityOutWall) {
    WCSimTuningParams->SetWCODTyvekReflectivityOutWall(CommandWCODTyvekReflectivityOutWall->GetNewDoubleValue(newValue));
    printf("Setting OD tyvek reflectivity on out-wall %f\n",CommandWCODTyvekReflectivityOutWall->GetNewDoubleValue(newValue));
  }
}
