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

  //Added by B. Quilain 2018/07/25
  Qeff = new G4UIcmdWithADouble("/WCSim/tuning/qeff",this);
  Qeff->SetGuidance("Set the correction of cathode QE parameter (nominal = 1). Used when changing the cathode reflectivity to keep the total charge constant.");
  Qeff->SetParameterName("Qeff",true);
  Qeff->SetDefaultValue(1.);

  Mieff = new G4UIcmdWithADouble("/WCSim/tuning/mieff",this);
  Mieff->SetGuidance("Set the Mie scattering parameter");
  Mieff->SetParameterName("Mieff",true);
  Mieff->SetDefaultValue(0.0);

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

}

WCSimTuningMessenger::~WCSimTuningMessenger()
{
  delete Rayff;
  delete Bsrff;
  delete Abwff;
  delete Rgcff;
  delete Qeff;//B.Q
  delete Mieff;
  delete Ttsff;
  delete PMTSatur;//TD 2019.7.16
  //delete Qoiff; //TD 2019.6.26
  //delete NLTinfo;
  
  //jl145 - for Top Veto
  delete TVSpacing;
  delete TopVeto;

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

 if(command == Bsrff) {
	  // Set the blacksheet reflection parameter
	  //	  printf("Input parameter %f\n",Bsrff->GetNewDoubleValue(newValue));

  WCSimTuningParams->SetBsrff(Bsrff->GetNewDoubleValue(newValue));

  printf("Setting blacksheet reflection parameter %f\n",Bsrff->GetNewDoubleValue(newValue));

  }

 if(command == Abwff) {
	  // Set the water attenuation  parameter
	  //	  printf("Input parameter %f\n",Abwff->GetNewDoubleValue(newValue));

  WCSimTuningParams->SetAbwff(Abwff->GetNewDoubleValue(newValue));

  printf("Setting water attenuation parameter %f\n",Abwff->GetNewDoubleValue(newValue));

  }

 if(command == Rgcff) {
	  // Set the cathode reflectivity parameter
	  //	  printf("Input parameter %f\n",Rgcff->GetNewDoubleValue(newValue));

  WCSimTuningParams->SetRgcff(Rgcff->GetNewDoubleValue(newValue));

  printf("Setting cathode reflectivity parameter %f\n",Rgcff->GetNewDoubleValue(newValue));

  }

 if(command == Qeff) {//B. Quilain
	  // Set the cathode QE correction parameter
	  //	  printf("Input parameter %f\n",Qeff->GetNewDoubleValue(newValue));

  WCSimTuningParams->SetQeff(Qeff->GetNewDoubleValue(newValue));

  printf("Seting the correction of cathode QE parameter (nominal = 1) %f\n",Qeff->GetNewDoubleValue(newValue));

  }

 if(command == Mieff) {
	  // Set the Mie scattering parameter
	  //	  printf("Input parameter %f\n",Mieff->GetNewDoubleValue(newValue));

  WCSimTuningParams->SetMieff(Mieff->GetNewDoubleValue(newValue));

  printf("Setting Mie scattering parameter %f\n",Mieff->GetNewDoubleValue(newValue));

  }

 if(command == Ttsff) {
	  // Set the transit time smearing parameter
	  //	  printf("Input parameter %f\n",Ttsff->GetNewDoubleValue(newValue));

  WCSimTuningParams->SetTtsff(Ttsff->GetNewDoubleValue(newValue));

  printf("Setting transit time smearing parameter %f\n",Ttsff->GetNewDoubleValue(newValue));

  }

 //TD 2019.7.16
 if(command == PMTSatur) {

   WCSimTuningParams->SetPMTSatur(PMTSatur->GetNewDoubleValue(newValue));

   printf("Setting saturation threshold for PMTs %f (-1 means no saturation)\n",PMTSatur->GetNewDoubleValue(newValue));
 }

//TD 2019.6.26
//if(command == Qoiff) {
////Set the uncertainty on amount of charge created in the PMT
////  printf("Input parameter %f\n",Qoiff->GetNewDoubleValue(newValue));

//  WCSimTuningParams->SetQoiff(Qoiff->GetNewDoubleValue(newValue));

//  printf(" Setting the uncertainty on amount of charge created in the PMT %f\n",Qoiff->GetNewDoubleValue(newValue));

// }


//if(command == NLTinfo) {
////Set the non-linearity info for time resolutions in PMTs
////  printf("Input parameter %f\n",NLTinfo->GetNewDoubleValue(newValue));

//  WCSimTuningParams->SetNLTinfo(NLTinfo->GetNewDoubleValue(newValue));

//  printf(" Setting the non-linearity info for time resolutions in PMTs to %f\n",NLTinfo->GetNewDoubleValue(newValue));

// }

  //jl145 - For Top Veto

  else if(command == TVSpacing) {
    // Set the Top Veto PMT Spacing
    WCSimTuningParams->SetTVSpacing(TVSpacing->GetNewDoubleValue(newValue));
    printf("Setting Top Veto PMT Spacing %f\n",TVSpacing->GetNewDoubleValue(newValue));
  }

  else if(command == TopVeto) {
    // Set the Top Veto on/off
    WCSimTuningParams->SetTopVeto(TopVeto->GetNewBoolValue(newValue));
    if(TopVeto->GetNewBoolValue(newValue))
      printf("Setting Top Veto On\n");
    else
      printf("Setting Top Veto Off\n");
  }


}
