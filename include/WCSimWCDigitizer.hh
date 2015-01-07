#ifndef WCSimWCDigitizer_h
#define WCSimWCDigitizer_h 1

#include "WCSimDarkRateMessenger.hh"
#include "WCSimDetectorConstruction.hh"
#include "G4VDigitizerModule.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"
#include "globals.hh"
#include "Randomize.hh"
#include <map>
#include <vector>


class WCSimWCDigitizer : public G4VDigitizerModule
{
public:
  
  WCSimWCDigitizer(G4String name, WCSimDetectorConstruction*);
  ~WCSimWCDigitizer();
  
  void SetPMTSize(G4float inputSize) {PMTSize = inputSize;}

  void ReInitialize() { DigiHitMap.clear(); TriggerTimes.clear(); }
    

  int NumberOfGatesInThisEvent() { return TriggerTimes.size(); }
  
public:

  void AddPMTDarkRate(WCSimWCDigitsCollection*);
  void MakeHitsHistogram(WCSimWCDigitsCollection*);
  void FindNumberOfGates();
  void FindNumberOfGatesFast();
  void DigitizeGate(WCSimWCDigitsCollection* WCHC,G4int G);
  void Digitize();
  void SetDarkRate(double idarkrate){ PMTDarkRate = idarkrate; }
  void SetConversion(double iconvrate){ ConvRate = iconvrate; }
  G4double GetTriggerTime(int i) { return TriggerTimes[i];}

  static G4double GetLongTime() { return LongTime;}
  static G4double GetEventGateDown() { return eventgatedown;}
  static G4double GetEventGateUp() { return eventgateup;}
  static G4double GetCalibDarkNoise() { return eventgateup;}
  G4double GetPMTDarkRate(){ return PMTDarkRate; }
  G4double GetConversion(){ return ConvRate; }

private:
  
  static const double offset; // hit time offset
  static const double pmtgate; // ns
  static const double eventgateup; // ns
  static const double eventgatedown; // ns
  static const double calibdarknoise; // ns
  static const double LongTime; // ns
  static const int GlobalThreshold; //number of hit PMTs within an <=200ns sliding window that decides the global trigger t0
  WCSimDarkRateMessenger *DarkRateMessenger;
  double PMTDarkRate; // kHz
  double ConvRate; // kHz

  G4int triggerhisto[20000]; // for finding t0
  G4float RealOffset;  // t0 = offset corrected for trigger start
  G4float MinTime;  // very first hit time
  G4float PMTSize;
  G4double peSmeared;

  std::vector<G4double> TriggerTimes;
  std::map<G4int, G4int> GateMap;
  std::map<int,int> DigiHitMap; // need to check if a hit already exists..

  WCSimWCDigitsCollection*  DigitsCollection;  
  WCSimDetectorConstruction* myDetector;

};

#endif








