#ifndef WCSimTrackingMessenger_h
#define WCSimTrackingMessenger_h 1


#include "G4UImessenger.hh"

class G4UIcommand;
class G4UIdirectory;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class WCSimTrackingAction;

class WCSimTrackingMessenger: public G4UImessenger
{
public:
  WCSimTrackingMessenger(WCSimTrackingAction*);

  ~WCSimTrackingMessenger();

  void SetNewValue(G4UIcommand* command, G4String newValue);

private:
  WCSimTrackingAction* myTracking;

  G4UIdirectory* WCSimDir;
  G4UIcmdWithADouble* fractionPhotonsToDraw;
  G4UIcmdWithAnInteger* particleToTrack;
  G4UIcmdWithAString* processToTrack;
  G4UIcmdWithABool* saveHitProducingTracks;
};

#endif
