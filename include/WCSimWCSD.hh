#ifndef WCSimWCSD_h
#define WCSimWCSD_h 1

#include "G4VSensitiveDetector.hh"
#include "WCSimWCHit.hh"
#include "WCSimDetectorConstruction.hh"

class G4Step;
class G4HCofThisEvent;

class WCSimWCSD : public G4VSensitiveDetector
{
 public:
  WCSimWCSD(G4String,G4String,WCSimDetectorConstruction*, G4String);
  ~WCSimWCSD();
  
  void   Initialize(G4HCofThisEvent*);

  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void   EndOfEvent(G4HCofThisEvent*);
  
 private:

  G4int fHCID;
  WCSimDetectorConstruction* fdet;
  WCSimWCHitsCollection* fHitsCollection;
  std::map<int,int> fPMTHitMap;   // Whether a PMT was hit already


  G4String fDetectorElement;
};

#endif

