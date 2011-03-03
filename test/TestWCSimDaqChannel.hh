#include "cxxtest/TestSuite.h"
#include "WCSimDaqChannel.hh"

#include <cmath>

class TestWCSimDaqChannel : public CxxTest::TestSuite
{
public:

  void setUp() {
    fChannel.ClearPulses();
    fConfig.pmtPulseMu = 2.26;
    fConfig.pmtPulseSigma = 0.23;
    fConfig.pmtPulsePeakTime = exp(fConfig.pmtPulseMu - fConfig.pmtPulseSigma * fConfig.pmtPulseSigma);
    
    fConfig.integrationDiscriminatorThreshold = 0.01;
    fConfig.integrationLag = 10.0;
    fConfig.integrationWindow = 400.0;
    fConfig.pedestalChargeRMS = 0.1;

    fConfig.triggerDiscriminatorThreshold = fConfig.integrationDiscriminatorThreshold;
    fConfig.triggerCoincidenceWindow = 100.0;
  
    fChannel.SetConfig(fConfig);
    
    fIntegrals.clear();
    fNhit.Clear();
  }

  void testEmpty() {
    WCSimDaqChannel channel;
    channel.SetConfig(fConfig);
    
    TS_ASSERT_EQUALS(channel.GetPulseCount(), 0);
    channel.Simulate(fNhit, fIntegrals);
    TS_ASSERT_EQUALS(fNhit.GetBeginPoints().size(), 0);
    TS_ASSERT_EQUALS(fIntegrals.size(), 0);
  }
  
  
  void testAddPulse() {
    WCSimDaqChannel daq;

    TS_ASSERT_EQUALS(daq.GetPulseCount(), 0);
    daq.AddPulse(0.0, 1.0);
    TS_ASSERT_EQUALS(daq.GetPulseCount(), 1);
    daq.AddPulse(100.0, 2.0);
    TS_ASSERT_EQUALS(daq.GetPulseCount(), 2);
    daq.AddPulse(100.0, 1.0);
    TS_ASSERT_EQUALS(daq.GetPulseCount(), 3);
  }
  
  void testClearPulses() {
    WCSimDaqChannel daq;

    daq.AddPulse(0.0, 1.0);
    daq.AddPulse(100.0, 2.0);
    daq.AddPulse(100.0, 1.0);
    TS_ASSERT_EQUALS(daq.GetPulseCount(), 3);
    
    daq.ClearPulses();
    TS_ASSERT_EQUALS(daq.GetPulseCount(), 0);
  }
  
  void testGetFirstPulseTime() {
    WCSimDaqChannel daq;

    daq.AddPulse(0.0, 1.0);
    daq.AddPulse(150.0, 2.0);
    daq.AddPulse(100.0, 2.0);
    daq.AddPulse(100.0, 1.0);
    
    TS_ASSERT_EQUALS(daq.GetFirstPulseTime(), 0.0);
  }
  
  void testGetLastPulseTime() {
    WCSimDaqChannel daq;

    daq.AddPulse(0.0, 1.0);
    daq.AddPulse(150.0, 2.0);
    daq.AddPulse(100.0, 2.0);
    daq.AddPulse(100.0, 1.0);
    
    TS_ASSERT_EQUALS(daq.GetLastPulseTime(), 150.0);
  }
  
  void testGetISinglePulse() {
    fChannel.AddPulse(0.0, 1.0);
    const double norm_factor = 1.0/sqrt(2 * M_PI * fConfig.pmtPulseSigma*fConfig.pmtPulseSigma);
    
    // Check some special values
    // log(x) == mu
    double x = exp(fConfig.pmtPulseMu);
    TS_ASSERT_DELTA(fChannel.GetI(x), norm_factor/x, 1e-5);

    // log(x) - mu = sigma
    x = exp(fConfig.pmtPulseMu + fConfig.pmtPulseSigma);
    TS_ASSERT_DELTA(fChannel.GetI(x), norm_factor/x * exp(-0.5), 1e-5);

    // log(x) - mu = -sigma
    x = exp(fConfig.pmtPulseMu - fConfig.pmtPulseSigma);
    TS_ASSERT_DELTA(fChannel.GetI(x), norm_factor/x * exp(-0.5), 1e-5);
  }
  
  void testGetIMultiplePulse() {
    const int noffsets = 3;
    const double offsets[noffsets] = {0.0, 100.0, 500.0};
    
    for (int i=0; i < noffsets; i++)
      fChannel.AddPulse(offsets[i], 1.0);

    const double norm_factor = 1.0/sqrt(2 * M_PI * fConfig.pmtPulseSigma*fConfig.pmtPulseSigma);
    
    for (int i=0; i < noffsets; i++) {
      double offset = offsets[i];
      
      // Check some special values
      // log(x) == mu
      double x = exp(fConfig.pmtPulseMu);
      TS_ASSERT_DELTA(fChannel.GetI(offset+x), norm_factor/x, 1e-5);

      // log(x) - mu = sigma
      x = exp(fConfig.pmtPulseMu + fConfig.pmtPulseSigma);
      TS_ASSERT_DELTA(fChannel.GetI(offset+x), norm_factor/x * exp(-0.5), 1e-5);

      // log(x) - mu = -sigma
      x = exp(fConfig.pmtPulseMu - fConfig.pmtPulseSigma);
      TS_ASSERT_DELTA(fChannel.GetI(offset+x), norm_factor/x * exp(-0.5), 1e-5);
    }
    
  }
  
  void testSimulateSinglePulseNoNoise() {
    // Turn off noise for predictable test results
    fConfig.pedestalChargeRMS = 0.0;
    fChannel.SetConfig(fConfig);
      
    fChannel.AddPulse(0.0, 1.0);
    fChannel.Simulate(fNhit, fIntegrals);
    
    TS_ASSERT_EQUALS(fNhit.GetBeginPoints().size(), 1);
    TS_ASSERT_EQUALS(fIntegrals.size(), 1);
    TS_ASSERT(fNhit.Contains(10.0));
    if (fIntegrals.size() > 0) {
      TS_ASSERT_DELTA(fIntegrals[0].charge, 1.0, 1e-4);
      TS_ASSERT_DELTA(fIntegrals[0].time, 5.212, 0.1 /* uncert from time stepping */);
    }
  }

  void testSimulateMultiPulseNoNoise() {
    // Turn off noise for predictable test results
    fConfig.pedestalChargeRMS = 0.0;
    fChannel.SetConfig(fConfig);
      
    const int noffsets = 3;
    const double offsets[noffsets] = {0.0, 500.0, 1000.0};
    for (int i=0; i < noffsets; i++)
      fChannel.AddPulse(offsets[i], 1.0 * (i+1)); // different charge for each one

    fChannel.Simulate(fNhit, fIntegrals);
    TS_ASSERT_EQUALS(fNhit.GetBeginPoints().size(), 3);
    TS_ASSERT_EQUALS(fIntegrals.size(), 3);
    
    for (int i=0; i < noffsets; i++) {
      double offset = offsets[i];
      
      TS_ASSERT(fNhit.Contains(offset + 10.0));      
      TS_ASSERT_DELTA(fIntegrals[i].charge, 1.0 * (i+1), 1e-4);
      TS_ASSERT_DELTA(fIntegrals[i].time, offset + 5, 1 /* different threshold crossings for diff charge */);
    }
  }
  
  void testSimulateOverlapPulseNoNoise() {
    // Turn off noise for predictable test results
    fConfig.pedestalChargeRMS = 0.0;
    fChannel.SetConfig(fConfig);
    
    // First two pulses overlap  
    const int noffsets = 3;
    const double offsets[noffsets] = {0.0, 200.0, 1000.0};
    for (int i=0; i < noffsets; i++)
      fChannel.AddPulse(offsets[i], 1.0 * (i+2)); // different charge for each one

    fChannel.Simulate(fNhit, fIntegrals);
    TS_ASSERT_EQUALS(fNhit.GetBeginPoints().size(), 3);
    TS_ASSERT_EQUALS(fIntegrals.size(), 2);

    TS_ASSERT(fNhit.Contains(10.0));
    TS_ASSERT(fNhit.Contains(210.0));
    TS_ASSERT(fNhit.Contains(1010.0));
    
    TS_ASSERT_DELTA(fIntegrals[0].charge, 2.0+3.0, 1e-4);
    TS_ASSERT_DELTA(fIntegrals[0].time, 5.0, 1); // first pulse triggers
    
    TS_ASSERT_DELTA(fIntegrals[1].charge, 4.0 , 1e-4);
    TS_ASSERT_DELTA(fIntegrals[1].time, 1005.0, 1); // first pulse triggers    
  }
  
  
  
protected:
  WCSimDaqChannel fChannel;
  WCSimDaqChannelConfig fConfig;
  std::vector<WCSimDaqIntegral> fIntegrals;
  WCSimIntervalList fNhit;
};
