#include "WCSimPmtPulse.hh"

#include <cmath>

WCSimPmtPulse::WCSimPmtPulse() 
  : fTimeOffset(0.0), fScaleFactor(1.0), fMu(1.0), fSigma(1.0)
{
}

WCSimPmtPulse::WCSimPmtPulse(double timeOffset, double scaleFactor, double mu, double sigma)
  : fTimeOffset(timeOffset), fScaleFactor(scaleFactor), fMu(mu), fSigma(sigma)
{    
}

double WCSimPmtPulse::GetTimeOffset() const
{
  return fTimeOffset;
}

void WCSimPmtPulse::SetTimeOffset(double timeOffset)
{
  fTimeOffset = timeOffset;
}

double WCSimPmtPulse::GetScaleFactor() const
{
  return fScaleFactor;
}

void WCSimPmtPulse::SetScaleFactor(double scaleFactor)
{
  fScaleFactor = scaleFactor;
}

double WCSimPmtPulse::GetMu() const
{
  return fMu;
}

void WCSimPmtPulse::SetMu(double mu)
{
  fMu = mu;
}

double WCSimPmtPulse::GetSigma() const
{
  return fSigma;
}

void WCSimPmtPulse::SetSigma(double sigma)
{
  fSigma = sigma;
}

double WCSimPmtPulse::GetI(double time) const
{
  if (time <= fTimeOffset)
    return 0.0;
  else {
    double x = time - fTimeOffset;
    double norm = 1.0/sqrt(2*M_PI*fSigma*fSigma);
    return fScaleFactor * norm/x * exp(-0.5 * pow( (log(x) - fMu)/fSigma, 2.0 ));
  }
}
