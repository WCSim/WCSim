#ifndef WCSimWCTriggerNHits_h
#define WCSimWCTriggerNHits_h 1

#include "WCSimWCTriggerNHits.hh"

class WCSimWCTriggerNHits : public WCSimWCTriggerBase
{
public:

  //not recommended to override these methods
  WCSimWCTriggerNHits(G4String name, WCSimDetectorConstruction*);
  ~WCSimWCTriggerNHits();
  
  void Digitize();

private:
  virtual void ApplyTrigger(WCSimWCDigitsCollection* WCDCPMT);

protected:
  WCSimWCDigitsCollection*  DigitsCollection;


  //the following methods & data members may be removed
  
public:
  void ReInitialize() { DigiHitMap.clear(); }
  void AddNewDigit(int tube, int gate, float digihittime, float peSmeared);
  void DigitizeHits(WCSimWCDigitsCollection* WCHCPMT);
  void DigitizeGate(WCSimWCDigitsCollection* WCHC,G4int G);

  static G4double GetLongTime() { return LongTime;}
  static G4double GetEventGateDown() { return eventgatedown;}
  static G4double GetEventGateUp() { return eventgateup;}

private:
  static const double offset; // hit time offset
  static const double pmtgate; // ns
  static const double eventgateup; // ns
  static const double eventgatedown; // ns
  static const double calibdarknoise; // ns
  static const double LongTime; // ns
  static const int GlobalThreshold; //number of hit PMTs within an <=200ns sliding window that decides the global trigger t0
  double PMTDarkRate; // kHz
  double ConvRate; // kHz

  G4float RealOffset;  // t0 = offset corrected for trigger start
  G4float MinTime;  // very first hit time
  G4float PMTSize;
  G4double peSmeared;

  WCSimDetectorConstruction* myDetector;

protected:
  std::map<int,int> DigiHitMap; // need to check if a hit already exists..

};

#endif








