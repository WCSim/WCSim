#include "WCSimTuningMessenger.hh"
#include "WCSimTuningParameters.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh" //jl145
#include "G4UIcmdWithAString.hh"


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

  //Added by B. Quilain 2018/07/25
  Qeff = new G4UIcmdWithADouble("/WCSim/tuning/qeff",this);
  Qeff->SetGuidance("Set the correction of cathode QE parameter (nominal = 1). Used when changing the cathode reflectivity to keep the total charge constant.");
  Qeff->SetParameterName("Qeff",true);
  Qeff->SetDefaultValue(1.);

  Mieff = new G4UIcmdWithADouble("/WCSim/tuning/mieff",this);
  Mieff->SetGuidance("Set the Mie scattering parameter");
  Mieff->SetParameterName("Mieff",true);
  Mieff->SetDefaultValue(0.0);

  PMTCathodePara = new G4UIcmdWithAString("/WCSim/tuning/PMTCathodePara",this);
  PMTCathodePara->SetGuidance("Input file for PMT cathode parameters");
  PMTCathodePara->SetParameterName("PMTCathodePara",false);

  //TD 2019.06.22
  Ttsff = new G4UIcmdWithADouble("/WCSim/tuning/ttsff",this);
  Ttsff->SetGuidance("Set the transit time smearing parameter for the PMTs");
  Ttsff->SetParameterName("Ttsff",true);
  Ttsff->SetDefaultValue(1.0);

  //TD 2019.6.26
  PMTSatur = new G4UIcmdWithADouble("/WCSim/tuning/pmtsatur",this);
  PMTSatur->SetGuidance("Set the pe threshold where saturation starts to occur");
  PMTSatur->SetParameterName("PMTSatur",true);
  PMTSatur->SetDefaultValue(-1);
  
  // Qoiff = new G4UIcmdWithADouble("/WCSim/tuning/qoiff",this);
  // Qoiff->SetGuidance("Set the uncertainty on amount of charge created in a PMT");
  // Qoiff->SetParameterName("Qoiff",true);
  // Qoiff->SetDefaultValue(0.0);
  
  // NLTinfo = new G4UIcmdWithADouble("/WCSim/tuning/nltinfo",this);
  // NLTinfo->SetGuidance("Set the non-linearity info for time resolutions in PMTs");
  // NLTinfo->SetParameterName("NLTinfo",true);
  // NLTinfo->SetDefaultValue(0.0);
  
  //jl145 - for Top Veto
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

  CommandWCODTyvekReflectivity = new G4UIcmdWithADouble("/WCSim/tuning/WCODTyvekReflectivity",this);
  CommandWCODTyvekReflectivity->SetGuidance("Set OD tyvek cladding reflectivity");
  CommandWCODTyvekReflectivity->SetParameterName("WCODTyvekReflectivity",true);
  CommandWCODTyvekReflectivity->SetDefaultValue(0.90);

}

WCSimTuningMessenger::~WCSimTuningMessenger()
{
  delete Rayff;
  delete Bsrff;
  delete Abwff;
  delete Rgcff;
  delete Qeff;//B.Q
  delete Mieff;
  delete PMTCathodePara;
  delete Ttsff;
  delete PMTSatur;//TD 2019.7.16
  //delete Qoiff; //TD 2019.6.26
  //delete NLTinfo;
  
  //jl145 - for Top Veto
  delete TVSpacing;
  delete TopVeto;

  delete CommandWCODWLSCladdingReflectivity;
  delete CommandWCODTyvekReflectivity;

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

  else if(command == Qeff) {//B. Quilain
    // Set the cathode QE correction parameter
    WCSimTuningParams->SetQeff(Qeff->GetNewDoubleValue(newValue));
    G4cout << "Seting the correction of cathode QE parameter (nominal = 1) " << Qeff->GetNewDoubleValue(newValue) << G4endl;
  }

 else if(command == Ttsff) {
   // Set the transit time smearing parameter
  WCSimTuningParams->SetTtsff(Ttsff->GetNewDoubleValue(newValue));
  G4cout << "Setting transit time smearing parameter " << Ttsff->GetNewDoubleValue(newValue) << G4endl;

  }

 //TD 2019.7.16
 else if(command == PMTSatur) {
   WCSimTuningParams->SetPMTSatur(PMTSatur->GetNewDoubleValue(newValue));
   G4cout << "Setting saturation threshold for PMTs %f (-1 means no saturation) " << PMTSatur->GetNewDoubleValue(newValue) << G4endl;
 }

//TD 2019.6.26
//else if(command == Qoiff) {
////Set the uncertainty on amount of charge created in the PMT
//  WCSimTuningParams->SetQoiff(Qoiff->GetNewDoubleValue(newValue));
//  G4cout << " Setting the uncertainty on amount of charge created in the PMT " << Qoiff->GetNewDoubleValue(newValue) << G4endl;

// }


//else if(command == NLTinfo) {
////Set the non-linearity info for time resolutions in PMTs
//  WCSimTuningParams->SetNLTinfo(NLTinfo->GetNewDoubleValue(newValue));
//  G4cout << " Setting the non-linearity info for time resolutions in PMTs to " << NLTinfo->GetNewDoubleValue(newValue) << G4endl;

// }

  else if(command == Mieff) {
    // Set the Mie scattering parameter
    WCSimTuningParams->SetMieff(Mieff->GetNewDoubleValue(newValue));
    G4cout << "Setting Mie scattering parameter " << Mieff->GetNewDoubleValue(newValue) << G4endl;
  }

  else if (command == PMTCathodePara) {
    WCSimTuningParams->ReadCathodeParaTable(newValue);
    G4cout << "Setting PMT photocathode surface optical model as Model " << WCSimTuningParams->GetPMTSurfType() << " (0 means default dielectric model)," << G4endl;
    G4cout << " cathode thickness = " << WCSimTuningParams->GetCathodeThickness() << " nm, nCathodePara = " << WCSimTuningParams->GetNCathodePara() << G4endl;
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
    G4cout << "Setting OD WLS plate cladding reflectivity " << CommandWCODWLSCladdingReflectivity->GetNewDoubleValue(newValue) << G4endl;
  }

  else if(command == CommandWCODTyvekReflectivity) {
    // Set the Top Veto PMT Spacing
    WCSimTuningParams->SetWCODTyvekReflectivity(CommandWCODTyvekReflectivity->GetNewDoubleValue(newValue));
    G4cout << "Setting OD tyvek reflectivity " << CommandWCODTyvekReflectivity->GetNewDoubleValue(newValue) << G4endl;
  }
}
