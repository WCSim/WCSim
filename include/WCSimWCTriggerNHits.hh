#ifndef WCSimWCTriggerNHits_h
#define WCSimWCTriggerNHits_h 1

#include "WCSimWCTriggerBase.hh"

class WCSimWCTriggerNHits : public WCSimWCTriggerBase
{
public:

  //not recommended to override these methods
  WCSimWCTriggerNHits(G4String name, WCSimDetectorConstruction*);
  ~WCSimWCTriggerNHits();
  
private:
  void DoTheWork(WCSimWCDigitsCollection* WCDCPMT);

};

#endif








