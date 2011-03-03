#include "cxxtest/TestSuite.h"
#include "WCSimLBNEDigitizer.hh"
#include "TF1.h"
#include <cmath>

class TestWCSimLBNEDigitizer : public CxxTest::TestSuite
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
  
  
  void testPickTime() {
    const int count = 10000;
    double mom1 = 0.0, mom2 = 0.0;
    
    for (int i=0; i < count; i++) {
      double value = fDaq->PickTime();
      mom1 += value;
      mom2 += value * value;
    }
    
    double mean = mom1/count;
    double rms = sqrt(count*mom2 - mom1*mom1)/count;
    
    TS_ASSERT_DELTA(mean, 0.0, 5e-2);
    TS_ASSERT_DELTA(rms, 3.0, 1e-1);
  }
  
  void testPickCharge() {
    const int count = 10000;
    double mom1 = 0.0, mom2 = 0.0;
    
    for (int i=0; i < count; i++) {
      double value = fDaq->PickCharge();
      mom1 += value;
      mom2 += value * value;
    }
    
    double mean = mom1/count;
    double rms = sqrt(count*mom2 - mom1*mom1)/count;
    
    TS_ASSERT_DELTA(mean, 1.0, 5e-2);
    TS_ASSERT_DELTA(rms, 0.1, 1e-1);    
  }
  
  void testAddDarkHits() {
    // Add many, many dark hits to get a good estimate of the average
    fConfig.pmtDarkRate = 10000.0; // 10 kHz
    fDaq->SetConfig(fConfig);
    
    WCSimDaqChannel channel;
    channel.SetConfig(fChannelConfig);
    
    double windowBegin = 0.0;
    double windowEnd = 1e4;
    const int iterations = 100;
    for (int i=0; i < iterations; i++) {
      fDaq->AddDarkHits(channel, windowBegin, windowEnd);
    }
    
    // Require number of hits within 5 sigma of estimate
    double expectedDarkHits = (windowEnd-windowBegin)*1e-9/*s/ns*/*fConfig.pmtDarkRate*iterations;
    TS_ASSERT_DELTA(expectedDarkHits, channel.GetPulseCount(), 5*sqrt(expectedDarkHits));
    
    // Check that no pulse is outside of the requested window
    TS_ASSERT_LESS_THAN_EQUALS(windowBegin, channel.GetFirstPulseTime());
    TS_ASSERT_LESS_THAN_EQUALS(channel.GetLastPulseTime(), windowEnd);
    
    // Do a crude integral to verify we have dark pulses with expected mean charge
    double deltaT = 0.5;
    double padding = 25.0; // give 25 ns to catch any pulse at end
    double charge = 0.0;
    for (double time=windowBegin; time < windowEnd + padding; time += deltaT)
      charge += channel.GetI(time)*deltaT;
    
    // mean charge per pulse is 1.0 from setUp()
    TS_ASSERT_DELTA(charge, expectedDarkHits, 5*sqrt(expectedDarkHits));
  }
  
  void testGetIntegral() {
    std::vector<WCSimDaqIntegral> integrals(4);
    
    // populate vector
    integrals[0].time =    0.0; integrals[0].charge = 1.0;
    integrals[1].time =  100.0; integrals[1].charge = 4.0;
    integrals[2].time =  200.0; integrals[2].charge = 3.0;
    integrals[3].time = 1000.0; integrals[3].charge = 6.0;
    
    bool hit;
    WCSimDaqIntegral summedIntegral;

    hit = fDaq->GetIntegral(-100, -50, integrals, summedIntegral);
    TS_ASSERT(!hit);
    
    hit = fDaq->GetIntegral(-100, 10, integrals, summedIntegral);
    TS_ASSERT(hit);
    TS_ASSERT_DELTA(summedIntegral.time, 0.0, 1e-5);
    TS_ASSERT_DELTA(summedIntegral.charge, 1.0, 1e-5);

    hit = fDaq->GetIntegral(-100, 101, integrals, summedIntegral);
    TS_ASSERT(hit);
    TS_ASSERT_DELTA(summedIntegral.time, 0.0, 1e-5);
    TS_ASSERT_DELTA(summedIntegral.charge, 5.0, 1e-5);

    hit = fDaq->GetIntegral(50, 101, integrals, summedIntegral);
    TS_ASSERT(hit);
    TS_ASSERT_DELTA(summedIntegral.time, 100.0, 1e-5);
    TS_ASSERT_DELTA(summedIntegral.charge, 4.0, 1e-5);    
    
    hit = fDaq->GetIntegral(150, 199, integrals, summedIntegral);
    TS_ASSERT(!hit);
    
    hit = fDaq->GetIntegral(50, 1100, integrals, summedIntegral);
    TS_ASSERT(hit);
    TS_ASSERT_DELTA(summedIntegral.time, 100.0, 1e-5);
    TS_ASSERT_DELTA(summedIntegral.charge, 13.0, 1e-5); 
  }
   
protected:
  WCSimLBNEDigitizer *fDaq;
  WCSimLBNEDigitizerConfig fConfig;
  WCSimDaqChannelConfig fChannelConfig;
  TH1D *fTimeDist;
  TH1D *fChargeDist;
};
