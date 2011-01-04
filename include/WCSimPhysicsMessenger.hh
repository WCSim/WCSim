#ifndef WCSimPhysicsMessenger_h
#define WCSimPhysicsMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class WCSimPhysicsList;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAString;

class WCSimPhysicsMessenger: public G4UImessenger
{
public:
  WCSimPhysicsMessenger(WCSimPhysicsList*);
  ~WCSimPhysicsMessenger();

  void SetNewValue(G4UIcommand* command, G4String newValue);

private:
  WCSimPhysicsList* WCSimPhysics;

  G4UIdirectory*      WCSimDir;
  G4UIcmdWithAString* hadmodelCmd;

};

#endif
