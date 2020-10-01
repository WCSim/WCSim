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
#include "WCSimWCTrigger.hh"
#include "WCSimWCDAQMessenger.hh"
#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"

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
  WCSimWCDAQMessenger* DAQMessenger;
  
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
		     WCSimWCDigitsCollection*,
		     WCSimWCTriggeredDigitsCollection*,
		     G4String detectorElement="tank");
  void FillRootEventHybrid(G4int, 
			   const struct ntupleStruct&, 
			   G4TrajectoryContainer*,
			   WCSimWCDigitsCollection*,
			   WCSimWCTriggeredDigitsCollection*,
			   G4String,
			   WCSimRootEvent*,
			   WCSimRootTrigger*);
  WCSimRunAction* GetRunAction(){return runAction;}
  void SetDigitizerChoice(G4String digitizer) { DigitizerChoice = digitizer; }
  void SetTriggerChoice  (G4String trigger)   { TriggerChoice   = trigger;   }

  void FillFlatTree(G4int,
		    const struct ntupleStruct&, 
		    G4TrajectoryContainer*,
		    WCSimWCDigitsCollection*,
		    WCSimWCTriggeredDigitsCollection*,
		    G4String detectorElement);
  

private:
  G4int WCSimEventFindStartingVolume( G4ThreeVector vtx);
  G4int WCSimEventFindStoppingVolume( G4String stopVolumeName);

  G4String vtxVolumeName;         //TF new

  ///Create instances of the user-chosen digitizer and trigger classes
  void  CreateDAQInstances();

  G4String DigitizerChoice;
  G4String TriggerChoice;
  bool     ConstructedDAQClasses;
  bool     SavedOptions;
};


#endif
    
