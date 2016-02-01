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
  
  WCSimWCPMT(G4String name, WCSimDetectorConstruction*);
  ~WCSimWCPMT();
  
   void ReInitialize() { DigiHitMapPMT.clear(); TriggerTimes.clear(); }
    
   
public:
  
  void AddPMTDarkRate(WCSimWCDigitsCollection*);
  void MakePeCorrection(WCSimWCHitsCollection*);
  void Digitize();
  G4double GetTriggerTime(int i) { return TriggerTimes[i];}
  // void SetConversion(double iconvrate){ ConvRate = iconvrate; }
  //  static G4double GetLongTime() { return LongTime;}
  
  G4double rn1pe();
  G4double peSmeared;
  // double ConvRate; // kHz
  std::vector<G4double> TriggerTimes;
  std::map<int,int> DigiHitMapPMT; // need to check if a hit already exists..

  WCSimWCDigitsCollection*  DigitsCollection;  
  WCSimDetectorConstruction* myDetector;

};

#endif








