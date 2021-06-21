#ifndef WCSimSteppingAction_h
#define WCSimSteppingAction_h 1

//#include <TTree.h>
#include "G4Event.hh"
#include "G4UserSteppingAction.hh"
#include "G4ThreeVector.hh"

#include "WCSimRunAction.hh"

class G4HCofThisEvent;
class G4Event;

// First we have a structure to hold the values of the emitted photon.
typedef struct {

  G4ThreeVector direction; // The direction of the emitted photon.
  G4ThreeVector position; // The position where the emitted photon was created (same as where the incident photon was absorbed).
  double energy; // The energy value of the emitted photon.
  double wavelength; // The wavelength of the emitted photon.

} EmittedPhoton;


class WCSimSteppingAction : public G4UserSteppingAction
{
 private:
  WCSimRunAction* runAction;
  WCSimDetectorConstruction* det;

public:
  WCSimSteppingAction(WCSimRunAction*,WCSimDetectorConstruction*);

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
  void CollectPhotonsInfoInWLSPlates(const G4Step*);
  void WLSPhysicsProcess(const G4Step*);
  EmittedPhoton EmitPhoton(G4ThreeVector absorbedPhotonPosition);
  double EmittedPhotonWavelength();

private:

  G4double ret[2];

};


#endif
