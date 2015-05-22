#ifndef WCSimEventAction_h
#define WCSimEventAction_h 1


#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4ios.hh"

#include "WCSimDetectorConstruction.hh"
#include "G4TrajectoryContainer.hh"
#include "WCSimWCHit.hh"
#include "WCSimWCDigi.hh"

#include "TRandom3.h"

class WCSimRunAction;
class WCSimPrimaryGeneratorAction;
class G4Event;

class WCSimEventAction : public G4UserEventAction
{
private:
  WCSimRunAction* runAction;
  WCSimPrimaryGeneratorAction* generatorAction;
  WCSimDetectorConstruction*   detectorConstructor;

  TRandom3 * randGen;
  
public:
  WCSimEventAction(WCSimRunAction*, WCSimDetectorConstruction*,
		    WCSimPrimaryGeneratorAction*);
  ~WCSimEventAction();
  
public:
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
  void FillRootEvent(G4int, 
		     const struct ntupleStruct&, 
		     G4TrajectoryContainer*,
		     WCSimWCHitsCollection*, 
		     WCSimWCDigitsCollection*);
  WCSimRunAction* GetRunAction(){return runAction;}

 private:
  G4int WCSimEventFindStartingVolume( G4ThreeVector vtx);
  G4int WCSimEventFindStoppingVolume( G4String stopVolumeName);

};


#endif

    
