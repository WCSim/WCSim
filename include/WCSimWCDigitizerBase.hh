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
#include <utility> //for std::pair

class WCSimWCDigitizerBase : public G4VDigitizerModule
{
public:
  
  WCSimWCDigitizerBase(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*);
  virtual ~WCSimWCDigitizerBase();
  
  void AddNewDigit(int tube, int gate, float digihittime, float peSmeared, std::vector< std::pair<int,int> > digi_comp);
  virtual void DigitizeHits(WCSimWCDigitsCollection* WCHCPMT) = 0;
  void DigitizeGate(WCSimWCDigitsCollection* WCHC,G4int G);
  void Digitize();

  //.mac file option setting methods
  void SKDigitizerType(G4String); 

protected:
  void ReInitialize() { DigiStoreHitMap.clear(); }
  
  G4double peSmeared;

  WCSimDetectorConstruction* myDetector;
  WCSimWCDAQMessenger* DAQMessenger;

  WCSimWCDigitsCollection*  DigiStore;
  std::map<int,int> DigiStoreHitMap; // need to check if a hit already exists..

  bool SKDeadTime;

};

#endif








