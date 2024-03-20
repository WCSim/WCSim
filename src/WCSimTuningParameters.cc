// Tuning parameters

#include "WCSimTuningParameters.hh"
#include "WCSimTuningMessenger.hh"


WCSimTuningParameters::WCSimTuningParameters()
{

 TuningMessenger = new WCSimTuningMessenger(this);
 // Default values
 rayff=0.75;
 bsrff=2.50;
 abwff=1.30;
 rgcff=0.32;
 mieff=0.0;

 //jl145 - For Top Veto
 tvspacing = 100.0;
 topveto = false;

 WCODWLSCladdingReflectivity   = 0.90; //
 WCODTyvekReflectivityInWallTop   = 0.90; //
 WCODTyvekReflectivityInWallBarrel   = 0.90; //
 WCODTyvekReflectivityInWallBottom   = 0.90; //
 WCODTyvekReflectivityOutWallTop   = 0.90; //
 WCODTyvekReflectivityOutWallBarrel   = 0.90; //
 WCODTyvekReflectivityOutWallBottom   = 0.90; //

}

WCSimTuningParameters::~WCSimTuningParameters()
{
  delete TuningMessenger;
  TuningMessenger = 0;
}

void WCSimTuningParameters::SaveOptionsToOutput(WCSimRootOptions * wcopt)
{
  wcopt->SetRayff(rayff);
  wcopt->SetBsrff(bsrff);
  wcopt->SetAbwff(abwff);
  wcopt->SetRgcff(rgcff);
  wcopt->SetMieff(mieff);
  wcopt->SetTvspacing(tvspacing);
  wcopt->SetTopveto(topveto);
}
