#ifndef WCSimWCSD_h
#define WCSimWCSD_h 1

#include "G4VSensitiveDetector.hh"
#include "WCSimWCHit.hh"

class G4Step;
class G4HCofThisEvent;

class WCSimWCSD : public G4VSensitiveDetector
{
 public:
  WCSimWCSD(G4String);
  ~WCSimWCSD();
  
  void   Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void   EndOfEvent(G4HCofThisEvent*);
  
 private:

  G4int HCID;
  WCSimWCHitsCollection* hitsCollection;
  std::map<int,int> PMTHitMap;   // Whether a PMT was hit already

};

#endif

