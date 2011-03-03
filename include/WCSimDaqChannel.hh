#ifndef WCSim_DaqChannel
#define WCSim_DaqChannel

#include "WCSimIntervalList.hh"
#include <vector>
#include "WCSimPmtPulse.hh"

class WCSimDaqChannelConfig
{
public:
  double pmtPulseMu; // mu parameter in log normal function
  double pmtPulseSigma; // sigma parameter in log normal function
  double pmtPulsePeakTime; // time in ns after start of PMT pulse where peak current is reached
  
  double integrationDiscriminatorThreshold; //  current threshold in picoamps to start integration
  double integrationLag; // delay on signal to charge integrator in nanoseconds
  double integrationWindow; // time duration in nanoseconds to integrate pulses
  double pedestalChargeRMS; // RMS in integrated charge over the integrationWindow

  double triggerDiscriminatorThreshold; // current threshold in picoamps to emit NHIT trigger pulse
  double triggerCoincidenceWindow; // time duration in nanoseconds for NHIT trigger pulse
};

class WCSimDaqIntegral
{
public:
  double time; // Time in nanoseconds when integration began
  double charge; // Integrated charge in picocoulombs
};


// Simulate the behavior of a single DAQ channel.
// Each channel emits two information streams as a function of time:
//   1. NHIT trigger pulses: Used to implement a coincidence trigger
//   2. Charge integrals: Gathered to create events if the global trigger fires
//
// The logic to decide when to generate global triggers and which integrals to bundle
// together is assumed to exist above the WCSimDaqChannel level.
//
// To use this class, set the channel configuration, and then add all the PMT pulses.
// The call the Simulate() method to get a list of time intervals when the NHIT coincidence
// output from the channel is active, and charge integrals from the channel.
//
// Discriminator walk from pulse rise times times is simulated by the PMT pulse shape,
// and jitter in timing is extrapolated from the pedestalChargeRMS and the integrationWindow.
//
// The assumption is that while pedestal noise can jitter timing, it cannot fire either the
// NHIT or integration discriminator on its own.
class WCSimDaqChannel {
public:
  WCSimDaqChannel();
  
  // Get/Set channel configuration parameters
  WCSimDaqChannelConfig GetConfig() const;
  void SetConfig(const WCSimDaqChannelConfig &config);
  
  void AddPulse(double startTime, double charge);
  void ClearPulses();
  int GetPulseCount() const;
  
  double GetFirstPulseTime() const;
  double GetLastPulseTime() const;
  
  void Simulate(WCSimIntervalList &nhitPulses, std::vector<WCSimDaqIntegral> &chargeIntegrals);

  // Return the summed PMT pulse current without noise added
  double GetI(double time) const;
      
private:

  
  WCSimDaqChannelConfig fConfig;
  std::vector<WCSimPmtPulse> fPulses;
  double fTimeStep;
};

#endif