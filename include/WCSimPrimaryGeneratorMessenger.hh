#ifndef WCSimPrimaryGeneratorMessenger_h
#define WCSimPrimaryGeneratorMessenger_h 1

class WCSimPrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithADouble;

#include "G4UImessenger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "globals.hh"
#include "G4Tokenizer.hh"

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
  G4UIcmdWithAString* fileNameCmdCosmics;
  G4UIcmdWithAString* timeUnitCmd;
  G4UIcmdWithAString* isotopeCmd;
  G4UIcmdWithAString* radonScalingCmd;
  G4UIcmdWithADouble* radioactive_time_window_Cmd;
  G4UIcmdWithAnInteger* radonGeoSymCmd;

  // K.M.Tsui: addition of injector events
  G4UIcmdWithAnInteger* nPhotonsCmd;
  G4UIcmdWithAnInteger* injectorOnCmd;
  G4UIcmdWithADouble* injectorTimeCmd;
  G4UIcmdWithADouble* openingAngleCmd;
  G4UIcmdWithADouble* injectorWavelengthCmd;
  
  void IsotopeCommand(G4String newValue);
  void RadonScalingCommand(G4String newValue);
};

#endif


