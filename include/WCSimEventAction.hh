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

class WCSimRunAction;
class WCSimPrimaryGeneratorAction;
class G4Event;

class WCSimEventAction : public G4UserEventAction
{
private:
  WCSimRunAction* runAction;
  WCSimPrimaryGeneratorAction* generatorAction;
  WCSimDetectorConstruction*   detectorConstructor;
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
		     WCSimWCTriggeredDigitsCollection*);
  WCSimRunAction* GetRunAction(){return runAction;}
  void SetDigitizerChoice(G4String digitizer) { DigitizerChoice = digitizer; }
  void SetTriggerChoice  (G4String trigger)   { TriggerChoice   = trigger;   }

private:
  G4int WCSimEventFindStartingVolume( G4ThreeVector vtx);
  G4int WCSimEventFindStoppingVolume( G4String stopVolumeName);

  ///Create instances of the user-chosen digitizer and trigger classes
  void  CreateDAQInstances();

  G4String DigitizerChoice;
  G4String TriggerChoice;
  bool     ConstructedDAQClasses;
};


#endif

    
