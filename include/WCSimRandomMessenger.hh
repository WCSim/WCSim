#ifndef WCSimRandomMessenger_h
#define WCSimRandomMessenger_h 1

#include "G4UImessenger.hh"
class WCSimRandomParameters;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;

class WCSimRandomMessenger: public G4UImessenger
{
public:
  WCSimRandomMessenger(WCSimRandomParameters*);
  ~WCSimRandomMessenger();

  void SetNewValue(G4UIcommand* command, G4String newValue);

private:

  WCSimRandomParameters* WCSimRandomParams;

  G4UIdirectory*      WCSimDir;
  G4UIcmdWithAString* Rangen;
  G4UIcmdWithAnInteger* Ranseed;
};

#endif
