#include "cxxtest/TestSuite.h"
#include "WCSimLBNEDigitizer.hh"
#include "TF1.h"
#include <cmath>

class TestWCSimLBNEDigitizer_Nhit : public CxxTest::TestSuite
{
public:
  
  
  void setUp() {
    fDaq = new WCSimLBNEDigitizer("digi");
    fDaq->SetNumPmts(10);
    
    // Configure DAQ to be deterministic for now
    fChannelConfig.pmtPulseMu = 2.26;
    fChannelConfig.pmtPulseSigma = 0.23;
    fChannelConfig.pmtPulsePeakTime = exp(fChannelConfig.pmtPulseMu - fChannelConfig.pmtPulseSigma * fChannelConfig.pmtPulseSigma);
    
    fChannelConfig.integrationDiscriminatorThreshold = 0.01;
    fChannelConfig.integrationLag = 10.0;
    fChannelConfig.integrationWindow = 400.0;
    fChannelConfig.pedestalChargeRMS = 0.0; // no pedestal smearing

    fChannelConfig.triggerDiscriminatorThreshold = fChannelConfig.integrationDiscriminatorThreshold;
    fChannelConfig.triggerCoincidenceWindow = 100.0;
    
    fConfig.nhitThreshold = 3;
    fConfig.gateBeginDelta = -100.0;
    fConfig.gateEndDelta = 300.0;
    fConfig.pmtDarkRate = 0.0; // no dark hits
    fConfig.cableDelay = 400.0;
    
    TF1 *gauss = new TF1("gauss","exp(-0.5 * (x-[0])**2 / [1]**2)", -10, 10);
    
    fTimeDist = new TH1D("timeDist", ";Time (ns)", 100, -10, 10);
    gauss->SetParameters(0.0, 3.0);
    fTimeDist->FillRandom("gauss", 10000);
    
    fChargeDist = new TH1D("chargeDist", ";Time (ns)", 100, 0, 2);
    gauss->SetParameters(1.0, 0.1);
    fChargeDist->FillRandom("gauss", 10000);
    
    fDaq->SetConfig(fConfig);
    fDaq->SetChannelConfig(fChannelConfig);
    fDaq->SetTimeDistribution(fTimeDist);
    fDaq->SetChargeDistribution(fChargeDist);
    
    delete gauss;
  }
  
  void tearDown() {
    delete fDaq;
    delete fChargeDist;
    delete fTimeDist;
  }
  
  void testGetNhitTriggerTimes_Empty() {
    std::vector<WCSimIntervalList> nhitPulses(10);
    std::vector<double> triggerTimes;
    
    triggerTimes = fDaq->GetNhitTriggerTimes(nhitPulses);
    TS_ASSERT_EQUALS(triggerTimes.size(), 0);
  }
  
  void testGetNhitTriggerTimes_SimpleAligned() {
    // 3 hits in perfect coincidence
    WCSimIntervalList interval;

    std::vector<WCSimIntervalList> nhitPulses(10);
    std::vector<double> triggerTimes;
    
    interval.AddInterval(10.0, 110.0);
    nhitPulses[2] = nhitPulses[5] = nhitPulses[8] = interval;
    
    triggerTimes = fDaq->GetNhitTriggerTimes(nhitPulses);
    TS_ASSERT_EQUALS(triggerTimes.size(), 1);
    TS_ASSERT_DELTA(triggerTimes[0], 10.0, 1e-5);
  }

  void testGetNhitTriggerTimes_SimpleUnaligned() {
    // 3 hits staggered
    WCSimIntervalList interval;

    std::vector<WCSimIntervalList> nhitPulses(10);
    std::vector<double> triggerTimes;
    
    interval.Clear(); interval.AddInterval(10.0, 110.0);
    nhitPulses[1] = interval;
    
    interval.Clear(); interval.AddInterval(15.0, 115.0);
    nhitPulses[4] = interval;

    interval.Clear(); interval.AddInterval(20.0, 120.0);
    nhitPulses[9] = interval;  
    
    triggerTimes = fDaq->GetNhitTriggerTimes(nhitPulses);
    TS_ASSERT_EQUALS(triggerTimes.size(), 1);
    TS_ASSERT_DELTA(triggerTimes[0], 20.0, 1e-5);
  }

  void testGetNhitTriggerTimes_SimpleUnalignedLate() {
    // 3 hits staggered, extra late pulse on one channel
    WCSimIntervalList interval;

    std::vector<WCSimIntervalList> nhitPulses(10);
    std::vector<double> triggerTimes;
    
    interval.Clear(); interval.AddInterval(10.0, 110.0);
    nhitPulses[1] = interval;
    
    interval.Clear(); interval.AddInterval(15.0, 115.0);
    nhitPulses[4] = interval;

    interval.Clear(); interval.AddInterval(20.0, 120.0);
    interval.AddInterval(1000.0, 1100.0);
    nhitPulses[9] = interval;  
    
    triggerTimes = fDaq->GetNhitTriggerTimes(nhitPulses);
    TS_ASSERT_EQUALS(triggerTimes.size(), 1);
    TS_ASSERT_DELTA(triggerTimes[0], 20.0, 1e-5);
  }


  void testGetNhitTriggerTimes_Multiple() {
    // multiple triggers
    WCSimIntervalList interval;

    std::vector<WCSimIntervalList> nhitPulses(10);
    std::vector<double> triggerTimes;
    
    interval.Clear(); 
    interval.AddInterval(10.0, 110.0);
    interval.AddInterval(1110.0, 1210.0);
    interval.AddInterval(1500.0, 1600.0);
    nhitPulses[1] = interval;
    
    interval.Clear();
    interval.AddInterval(25.0, 120.0);
    interval.AddInterval(1110.0, 1210.0);
    interval.AddInterval(1550.0, 1650.0);
    nhitPulses[4] = interval;

    interval.Clear(); 
    interval.AddInterval(15.0, 115.0);
    interval.AddInterval(1520.0, 1620.0);
    nhitPulses[9] = interval;  
    
    triggerTimes = fDaq->GetNhitTriggerTimes(nhitPulses);
    TS_ASSERT_EQUALS(triggerTimes.size(), 2);
    TS_ASSERT_DELTA(triggerTimes[0], 25.0, 1e-5);
    TS_ASSERT_DELTA(triggerTimes[1], 1550.0, 1e-5);
  }

  void testGetNhitTriggerTimes_Lockout() {
    // multiple triggers
    WCSimIntervalList interval;

    std::vector<WCSimIntervalList> nhitPulses(10);
    std::vector<double> triggerTimes;
    
    interval.Clear(); 
    interval.AddInterval(10.0, 110.0);
    interval.AddInterval(150.0, 250.0);
    nhitPulses[1] = interval;
    
    interval.Clear();
    interval.AddInterval(25.0, 120.0);
    interval.AddInterval(150.0, 250.0);
    nhitPulses[4] = interval;

    interval.Clear(); 
    interval.AddInterval(15.0, 115.0);
    interval.AddInterval(155.0, 255.0);
    nhitPulses[9] = interval;  
    
    triggerTimes = fDaq->GetNhitTriggerTimes(nhitPulses);
    TS_ASSERT_EQUALS(triggerTimes.size(), 1);
    TS_ASSERT_DELTA(triggerTimes[0], 25.0, 1e-5);
  }

  
protected:
  WCSimLBNEDigitizer *fDaq;
  WCSimLBNEDigitizerConfig fConfig;
  WCSimDaqChannelConfig fChannelConfig;
  TH1D *fTimeDist;
  TH1D *fChargeDist;
};
