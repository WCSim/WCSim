#ifndef WCSim_LBNEDigitizer
#define WCSim_LBNEDigitizer
#include "WCSimAbstractDigitizer.hh"
#include "TH1D.h"

#include "WCSimDaqChannel.hh"
#include "WCSimIntervalList.hh"

#include <vector>
#include <map>

class WCSimLBNEDigitizerConfig {
public:
  int nhitThreshold; // Number of coincident hits required to produce trigger
  
  // start and end gate time relative to trigger time (negative for before trigger time)
  double gateBeginDelta, gateEndDelta; 
  
  // nanoseconds before earliest photon and after last photon to include when adding dark hits
  double pmtDarkHitsTimeBefore, pmtDarkHitsTimeAfter;
  
  double pmtDarkRate; // rate of dark hits in Hz

  double cableDelay; // time in nanoseconds between PMT base and front end electronics
  
};

// map from tubeID to charge integral and time for each hit PMT
class WCSimLBNEDigitizerEvent {
public:
  // Time of trigger in absolute time (ns)
  double triggerTime;
  
  // Time of start of gate in absolute time (ns)
  double gateBegin;
  
  // Map from tubeID to hit time and integral charge for hit PMTs
  // note that hit times are relative to gateBegin;
  std::map<int, WCSimDaqIntegral> hits;
};

/////////////////////////////////////////////////////////////////
//
// Simulate a generic DAQ for LBNE detectors
//
/////////////////////////////////////////////////////////////////

class WCSimLBNEDigitizer : public WCSimAbstractDigitizer {
public:
  WCSimLBNEDigitizer(G4String name);
  virtual ~WCSimLBNEDigitizer();
  
  // WCSimAbstractDigitizer interface
  virtual void Digitize();
  virtual G4double GetTriggerTime(int i);
  virtual int NumberOfGatesInThisEvent();

  std::vector<WCSimLBNEDigitizerEvent> SimulateDaq(std::vector<WCSimDaqChannel> &channels);
  
  void SetConfig(const WCSimLBNEDigitizerConfig &digiConfig);
  void SetChannelConfig(const WCSimDaqChannelConfig &channelConfig);
  void SetTimeDistribution(TH1D *timeDist);
  void SetChargeDistribution(TH1D *chargeDist);
  void SetNumPmts(int numPmts) { fNumPmts = numPmts; };
  
  // Select a random PMT transit time from time distribution
  double PickTime();

  // Select a random pulse integral from the charge distribution
  double PickCharge();
  
  // Add dark hits to a channel between beginTime and endTime (nanoseconds)
  void AddDarkHits(WCSimDaqChannel &channel, double beginTime, double endTime);

  // Return times when NHIT trigger condition is met
  std::vector<double> GetNhitTriggerTimes(std::vector<WCSimIntervalList> &nhitPulses);

  // Obtain channel hit inside event gate window.  gateBegin and gateEnd are in absolute times,
  // not times relative to trigger.
  // 
  // Returns true if channel hit was recorded in that window and sets summedIntegral to 
  // the time of the first recorded integral, and charge of the sum of all integrals in window
  bool GetIntegral(double gateBegin, double gateEnd, const std::vector<WCSimDaqIntegral> &integrals, 
                   WCSimDaqIntegral &summedIntegral);

protected:
  int fNumPmts;
  WCSimLBNEDigitizerConfig fConfig;
  WCSimDaqChannelConfig fChannelConfig;
  TH1D *fTimeDist;
  TH1D *fChargeDist;
  
  // Support WCSimAbstractDigitizer interface
  std::vector<double> fTriggerTimes;
};

#endif