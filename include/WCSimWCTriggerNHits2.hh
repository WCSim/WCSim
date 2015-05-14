#ifndef WCSimWCTriggerNHits2_h
#define WCSimWCTriggerNHits2_h 1

#include "WCSimWCTriggerBase.hh"

class WCSimWCTriggerNHits2 : public WCSimWCTriggerBase
{
public:

  //not recommended to override these methods
  WCSimWCTriggerNHits2(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*);
  ~WCSimWCTriggerNHits2();
  
private:
  void DoTheWork(WCSimWCDigitsCollection* WCDCPMT);

};

#endif








