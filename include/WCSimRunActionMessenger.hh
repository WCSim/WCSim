#ifndef WCSimRunActionMessenger_h
#define WCSimRunActionMessenger_h 1

class WCSimRunAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;

#include "G4UImessenger.hh"
#include "globals.hh"
#include "G4UIcmdWithABool.hh"

class WCSimRunActionMessenger: public G4UImessenger
{
 public:
  WCSimRunActionMessenger(WCSimRunAction* mpga);
  ~WCSimRunActionMessenger();
  
 public:
  void     SetNewValue(G4UIcommand* command, G4String newValues);
  
 private:
  WCSimRunAction* WCSimRun;

 private: //commands
  G4UIdirectory*      WCSimIODir;
  G4UIcmdWithAString* RootFile;
  G4UIcmdWithABool* WriteDefaultRootFile;
  G4UIcmdWithABool* RooTracker;

};

#endif


