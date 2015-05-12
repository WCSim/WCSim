#ifndef WCSimWCDigitizerBase_h
#define WCSimWCDigitizerBase_h 1

#include "WCSimDarkRateMessenger.hh"
#include "WCSimWCDAQMessenger.hh"
#include "WCSimDetectorConstruction.hh"
#include "G4VDigitizerModule.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"
#include "globals.hh"
#include "Randomize.hh"
#include <map>
#include <vector>


class WCSimWCDigitizerBase : public G4VDigitizerModule
{
public:
  
  WCSimWCDigitizerBase(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*);
  ~WCSimWCDigitizerBase();
  
  void ReInitialize() { DigiStoreHitMap.clear(); }
  
  void AddNewDigit(int tube, int gate, float digihittime, float peSmeared);
  virtual void DigitizeHits(WCSimWCDigitsCollection* WCHCPMT) = 0;
  void DigitizeGate(WCSimWCDigitsCollection* WCHC,G4int G);
  void Digitize();

  //.mac file option setting methods
  void SKDigitizerType(G4String); 

protected:
  G4double peSmeared;

  WCSimDetectorConstruction* myDetector;
  WCSimWCDAQMessenger* DAQMessenger;

  WCSimWCDigitsCollection*  DigiStore;
  std::map<int,int> DigiStoreHitMap; // need to check if a hit already exists..

  bool SKDeadTime;

};

#endif








