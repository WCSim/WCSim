#include "cxxtest/TestSuite.h"
#include "WCSimLBNEDigitizer.hh"
#include "TF1.h"
#include <cmath>

class TestWCSimLBNEDigitizer_Simulate : public CxxTest::TestSuite
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
  
  void testSimulate() {
    
  }
  
   
protected:
  WCSimLBNEDigitizer *fDaq;
  WCSimLBNEDigitizerConfig fConfig;
  WCSimDaqChannelConfig fChannelConfig;
  TH1D *fTimeDist;
  TH1D *fChargeDist;
};
