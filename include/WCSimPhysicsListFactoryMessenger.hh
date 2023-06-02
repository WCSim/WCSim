#ifndef WCSimPhysicsListFactoryMessenger_h
#define WCSimPhysicsListFactoryMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class WCSimPhysicsListFactory;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAString;

class WCSimPhysicsListFactoryMessenger: public G4UImessenger
{
public:
  WCSimPhysicsListFactoryMessenger(WCSimPhysicsListFactory*, G4String);
  ~WCSimPhysicsListFactoryMessenger();

  void SetNewValue(G4UIcommand* command, G4String newValue);

private:
  WCSimPhysicsListFactory* thisWCSimPhysicsListFactory;
  G4String ValidListsString;

  G4UIdirectory*      WCSimDir;
  G4UIcmdWithAString* physListCmd;
  G4UIcmdWithAString* nCaptureModelCmd;

};

#endif
