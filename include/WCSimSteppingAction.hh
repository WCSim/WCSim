#ifndef WCSimSteppingAction_h
#define WCSimSteppingAction_h 1

#include <TTree.h>
#include "G4Event.hh"
#include "G4UserSteppingAction.hh"
#include "G4ThreeVector.hh"
#include "WCSimRunAction.hh"

class G4HCofThisEvent;
class G4Event;

inline void CreateEmissionHistogram();

class WCSimSteppingAction : public G4UserSteppingAction
{
 private:
  WCSimRunAction* runAction;

public:
  WCSimSteppingAction(WCSimRunAction*);

  ~WCSimSteppingAction()
  {};

  void UserSteppingAction(const G4Step*);

  G4int G4ThreeVectorToWireTime(G4ThreeVector *pos3d,
				G4ThreeVector lArPos,
				G4ThreeVector start,
				G4int i);
  
  void Distortion(G4double x,
		  G4double y);

  G4double FieldLines(G4double x,
		      G4double y,
		      G4int xy);

  WCSimRunAction* GetRunAction(){return runAction;}
  void DebugWLSPlates(const G4Step*);

private:

  G4double ret[2];

};


#endif
