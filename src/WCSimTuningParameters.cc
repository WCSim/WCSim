// Tuning parameters

#include "WCSimTuningParameters.hh"
#include "WCSimTuningMessenger.hh"


WCSimTuningParameters::WCSimTuningParameters()
{

 TuningMessenger = new WCSimTuningMessenger(this);
 // Default values
 rayff=0.75;
 bsrff=2.50;
 abwff=1.30424242;
 rgcff=0.32;
 qeff=1.0;
 mieff=0.0;
 ttsff=1.0;
 //nltinfo=0.0; //meaning time resolution should not be linear w/ Q by default
 //qoiff=0.0; //TD 2019.6.26
 pmtsatur=-1;

 //jl145 - For Top Veto
 tvspacing = 100.0;
 topveto = false;

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
  wcopt->SetQeff(qeff);
  wcopt->SetMieff(mieff);
  wcopt->SetTtsff(ttsff);
  //wcopt->SetQoiff(qoiff);//TD 2019.6.26
  //wcopt->SetNLTinfo(nltinfo);
  wcopt->SetPMTSatur(pmtsatur);
  wcopt->SetTvspacing(tvspacing);
  wcopt->SetTopveto(topveto);
}
