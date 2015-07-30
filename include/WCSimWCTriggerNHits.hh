/**
 * \class WCSimWCTriggerNHits
 *
 * \brief A simple NHits trigger class
 *
 */

#ifndef WCSimWCTriggerNHits_h
#define WCSimWCTriggerNHits_h 1

#include "WCSimWCTriggerBase.hh"

class WCSimWCTriggerNHits : public WCSimWCTriggerBase
{
public:

  ///Create WCSimWCTriggerNHits instance with knowledge of the detector and DAQ options
  WCSimWCTriggerNHits(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*);

  ~WCSimWCTriggerNHits();
  
private:
  ///Calls the workhorse of this class: AlgNHits
  void DoTheWork(WCSimWCDigitsCollection* WCDCPMT);

};

#endif








