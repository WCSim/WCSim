#ifndef WCSim_PmtPulse
#define WCSim_PmtPulse

/////////////////////////////////////////////////////////////////
//
// Simulate the observed voltage from a PMT pulse.
//
/////////////////////////////////////////////////////////////////


class WCSimPmtPulse {
public:
  WCSimPmtPulse();
  WCSimPmtPulse(double timeOffset, double scaleFactor, double mu, double sigma);
  double GetTimeOffset() const;
  void SetTimeOffset(double timeOffset);
  
  double GetScaleFactor() const;
  void SetScaleFactor(double scaleFactor);
  
  double GetMu() const;
  void SetMu(double mu);
  
  double GetSigma() const;
  void SetSigma(double sigma);
  
  double GetI(double time) const;
  
private:
  double fTimeOffset;
  double fScaleFactor;
  double fMu;
  double fSigma;
};

#endif
