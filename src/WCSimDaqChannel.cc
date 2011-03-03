#include "WCSimDaqChannel.hh"
#include "TRandom.h"
#include <algorithm>
#include <iostream>

// Implementation notes:
//  * fPulses must always be sorted by time.
//
//  * fStartTimeBeforePulse can be zero because the log normal PMT pulse
//    starts at zero current at the pulse start time.
//
//  * We assume the PMT pulses are unimodal and shorter than either the integrationWindow
//    or the triggerCoincidenceWindow.

WCSimDaqChannel::WCSimDaqChannel()
 : fTimeStep(0.0625)
{
}

WCSimDaqChannelConfig WCSimDaqChannel::GetConfig() const
{
  return fConfig;
}

void WCSimDaqChannel::SetConfig(const WCSimDaqChannelConfig &config)
{
  fConfig = config;
}

bool PmtPulseLessThan(const WCSimPmtPulse &a, const WCSimPmtPulse &b)
{
  return a.GetTimeOffset() < b.GetTimeOffset();
}

void WCSimDaqChannel::AddPulse(double startTime, double charge)
{
  fPulses.push_back(WCSimPmtPulse(startTime, charge, fConfig.pmtPulseMu, fConfig.pmtPulseSigma));
  sort(fPulses.begin(), fPulses.end(), PmtPulseLessThan);
}

void WCSimDaqChannel::ClearPulses()
{
  fPulses.clear();
}

int WCSimDaqChannel::GetPulseCount() const
{
  return fPulses.size();
}

double WCSimDaqChannel::GetFirstPulseTime() const
{
  if (fPulses.size() == 0)
    return 0.0;
  else
    return fPulses[0].GetTimeOffset();
}

double WCSimDaqChannel::GetLastPulseTime() const
{
  if (fPulses.size() == 0)
    return 0.0;
  else
    return fPulses[fPulses.size()-1].GetTimeOffset();
}


void WCSimDaqChannel::Simulate(WCSimIntervalList &nhitPulses, std::vector<WCSimDaqIntegral> &chargeIntegrals)
{
  nhitPulses.Clear();
  chargeIntegrals.clear();
  
  // No pulses, nothing happens
  if (fPulses.size() == 0)
    return;
  
  bool done = false, integrating = false;
  double integralCharge, integralStartTime, integralStopTime=-1e9;
  double nhitStopTime = -1e9;

  double time = fPulses[0].GetTimeOffset();
  unsigned currentPulseIndex = 0;
  
  while (!done) {
    double current = GetI(time);
    
    // Check for NHIT discriminator threshold crossings
    if (nhitStopTime < time && current >= fConfig.triggerDiscriminatorThreshold) {
      nhitStopTime = time + fConfig.triggerCoincidenceWindow;
      nhitPulses.AddInterval(time, nhitStopTime);
    }
    
    // Check for integration transitions
    if (!integrating && current >= fConfig.integrationDiscriminatorThreshold) {
      // discriminator on, and not already integrating
      integralCharge = 0.0;
      integrating = true;
      integralStartTime = time;
      integralStopTime = time + fConfig.integrationWindow;
    } else if (integrating && time > integralStopTime) {
      integrating = false;
      WCSimDaqIntegral integral;
      integral.time = integralStartTime;
      integral.charge = integralCharge + gRandom->Gaus(0.0, fConfig.pedestalChargeRMS);
      chargeIntegrals.push_back(integral);
    }
    
    // Accumulate charge
    if (integrating) {
      integralCharge += fTimeStep * GetI(time - fConfig.integrationLag);
    }
    
    // Decide what time step to take, and if we are done
    if (!integrating && time > fPulses[currentPulseIndex].GetTimeOffset() + fConfig.pmtPulsePeakTime) {
      // Not integrating, and we are past the pulse peak, so nothing new can happen
      // until next pulse
      while (currentPulseIndex < fPulses.size() && time > fPulses[currentPulseIndex].GetTimeOffset()) {
        currentPulseIndex++;
      }

      // If there is another pulse in future, jump there
      if (currentPulseIndex < fPulses.size() && fPulses[currentPulseIndex].GetTimeOffset() > time)
        time = fPulses[currentPulseIndex].GetTimeOffset();
      else
        done = true; // nothing else can happen in future, so stop now

    } else
      time += fTimeStep; // keep stepping through PMT pulse      
    
  } // loop over time steps
}

double WCSimDaqChannel::GetI(double time) const
{
  double current = 0.0;
  
  for (unsigned i=0; i < fPulses.size(); i++)
    current += fPulses[i].GetI(time);
    
  return current;
}
