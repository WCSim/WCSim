#include "cxxtest/TestSuite.h"
#include "WCSimPmtPulse.hh"

#include <math.h>

// isnan() gets screwed up by C++ headers on Macs
#ifndef isnan
bool isnan(double x) { return x != x; }
#endif

class TestWCSimPmtPulse : public CxxTest::TestSuite
{
public:
  void testConstructorAndGetters()
  {
    WCSimPmtPulse p(100.0, 1.5, 2.0, 0.5);
    
    TS_ASSERT_EQUALS(p.GetTimeOffset(), 100.0);
    TS_ASSERT_EQUALS(p.GetScaleFactor(), 1.5);
    TS_ASSERT_EQUALS(p.GetMu(), 2.0);
    TS_ASSERT_EQUALS(p.GetSigma(), 0.5);
  }
  
  void testSetters() {
    WCSimPmtPulse p;
    
    p.SetTimeOffset(100.0);
    TS_ASSERT_EQUALS(p.GetTimeOffset(), 100.0);
    
    p.SetScaleFactor(1.5);
    TS_ASSERT_EQUALS(p.GetScaleFactor(), 1.5);
    
    p.SetMu(2.0);
    TS_ASSERT_EQUALS(p.GetMu(), 2.0);
    
    p.SetSigma(0.5);
    TS_ASSERT_EQUALS(p.GetSigma(), 0.5);
  }
  
  void testBeforeStart() {
    WCSimPmtPulse p(100.0, 1.0, 2.0, 0.5);
    
    TSM_ASSERT_EQUALS("Causality requires pulse to be zero before timeOffset",
                      p.GetI(0.0), 0.0)
    TSM_ASSERT_EQUALS("Causality requires pulse to be zero before timeOffset",
                      p.GetI(50.0), 0.0)
    TSM_ASSERT_EQUALS("Causality requires pulse to be zero before timeOffset",
                      p.GetI(100.0 - 1e-5), 0.0)
  }
  
  void testNaN() {
    WCSimPmtPulse p(100.0, 1.0, 2.0, 0.5);
    
    // Possible problem with log-normal
    TSM_ASSERT("Pulse is NaN at start time", !isnan(p.GetI(100.0)))
  }
  
  void testPoints() {
    WCSimPmtPulse p(100.0, 1.0, 2.0, 0.5);
    const double norm_factor = 1.0/sqrt(2 * M_PI * 0.5 * 0.5);

    // Check some special values
    // log(x) == mu
    double x = exp(2.0);
    TS_ASSERT_DELTA(p.GetI(100.0+x), norm_factor/x, 1e-5);

    // log(x) - mu = sigma
    x = exp(2.0 + 0.5);
    TS_ASSERT_DELTA(p.GetI(100.0+x), norm_factor/x * exp(-0.5), 1e-5);

    // log(x) - mu = -sigma
    x = exp(2.0 - 0.5);
    TS_ASSERT_DELTA(p.GetI(100.0+x), norm_factor/x * exp(-0.5), 1e-5);
  }
  
  void testScaleFactor() {
    WCSimPmtPulse p(100.0, 2.5, 2.0, 0.5);
    const double norm_factor = 1.0/sqrt(2 * M_PI * 0.5 * 0.5);

    // Check some special values
    // log(x) == mu
    double x = exp(2.0);
    TS_ASSERT_DELTA(p.GetI(100.0+x), 2.5*norm_factor/x, 1e-5);

    // log(x) - mu = sigma
    x = exp(2.0 + 0.5);
    TS_ASSERT_DELTA(p.GetI(100.0+x), 2.5*norm_factor/x * exp(-0.5), 1e-5);

    // log(x) - mu = -sigma
    x = exp(2.0 - 0.5);
    TS_ASSERT_DELTA(p.GetI(100.0+x), 2.5*norm_factor/x * exp(-0.5), 1e-5);
    
  }
  
};
