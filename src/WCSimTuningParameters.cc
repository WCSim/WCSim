// Tuning parameters

#include "WCSimTuningParameters.hh"
#include "WCSimTuningMessenger.hh"


WCSimTuningParameters::WCSimTuningParameters()
{

 TuningMessenger = new WCSimTuningMessenger(this);
 // Default values
 rayff=0.61;

}

WCSimTuningParameters::~WCSimTuningParameters()
{
  delete TuningMessenger;
  TuningMessenger = 0;
}

