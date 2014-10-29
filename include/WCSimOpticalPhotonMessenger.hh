#ifndef WCSimOpticalPhotonMessenger_h
#define WCSimOpticalPhotonMessenger_h 1

 class WCSimOpticalPhotonTrackInfo;
 class G4UIdirectory;
 class G4UIcmdWithABool;

#include "globals.hh"
#include "G4ios.hh"
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"

class WCSimOpticalPhotonMessenger: public G4UImessenger{

public:
  WCSimOpticalPhotonMessenger(WCSimOpticalPhotonTrackInfo* info);
  ~WCSimOpticalPhotonMessenger();
  
  void SetNewValue(G4UIcommand* command, G4String newValues);

private:
  WCSimOpticalPhotonTrackInfo* opInfo;
  
  G4UIdirectory* opInfoDir;
  G4UIcmdWithABool* opEnabled;
};

#endif
