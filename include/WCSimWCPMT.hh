#ifndef WCSimWCPMT_h
#define WCSimWCPMT_h 1

#include "WCSimDarkRateMessenger.hh"
#include "WCSimDetectorConstruction.hh"
#include "G4VDigitizerModule.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"
#include "globals.hh"
#include "Randomize.hh"
#include <map>
#include <vector>


class WCSimWCPMT : public G4VDigitizerModule
{
public:

  WCSimWCPMT(G4String name, WCSimDetectorConstruction*, G4String detectorElement);
  ~WCSimWCPMT();
  
   void ReInitialize() { DigiHitMapPMT.clear(); TriggerTimes.clear(); }
    
   // Set Relative Digitized Hit time mode and reinitialize first_time hit
   void SetRelativeDigitizedHitTime (bool val) { RelativeHitTime = val; first_time = 0; }
   
public:
  
  void AddPMTDarkRate(WCSimWCDigitsCollection*);
  void MakePeCorrection(WCSimWCHitsCollection*);
  void Digitize();
  G4double GetTriggerTime(int i) { return TriggerTimes[i];}
  // void SetConversion(double iconvrate){ ConvRate = iconvrate; }
  //  static G4double GetLongTime() { return LongTime;}
  
  G4double rn1pe();
  // double ConvRate; // kHz
  std::vector<G4double> TriggerTimes;
  std::map<int,int> DigiHitMapPMT; // need to check if a hit already exists..

  WCSimWCDigitsCollection*  DigitsCollection;  
  WCSimDetectorConstruction* myDetector;
  G4String detectorElement;

private:
  bool RelativeHitTime;
  
  static G4double first_time;

};

#endif








