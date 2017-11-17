#ifndef WCSimTrackingMessenger_h
#define WCSimTrackingMessenger_h 1


#include "G4UImessenger.hh"

class G4UIcommand;
class G4UIdirectory;
class G4UIcmdWithADouble;
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
};

#endif
