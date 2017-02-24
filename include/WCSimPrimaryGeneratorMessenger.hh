#ifndef WCSimPrimaryGeneratorMessenger_h
#define WCSimPrimaryGeneratorMessenger_h 1

class WCSimPrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithADouble;

#include "G4UImessenger.hh"
#include "globals.hh"

class WCSimPrimaryGeneratorMessenger: public G4UImessenger
{
 public:
  WCSimPrimaryGeneratorMessenger(WCSimPrimaryGeneratorAction* mpga);
  ~WCSimPrimaryGeneratorMessenger();
  
 public:
  void     SetNewValue(G4UIcommand* command, G4String newValues);
  G4String GetCurrentValue(G4UIcommand* command);
  
 private:
  WCSimPrimaryGeneratorAction* myAction;
  // Boolean to determine whether the neutrino generator has been set yet
  // Need to know whether we expect to open a ROOT file or a text file
  bool genSet;
  
 private: //commands
  G4UIdirectory*      mydetDirectory;
  G4UIcmdWithAString* genCmd;
  G4UIcmdWithAString* fileNameCmd;
  G4UIcmdWithABool*   poisCmd;
  G4UIcmdWithADouble* poisMeanCmd;

};

#endif


