#ifndef WCSimDetectorMessenger_h
#define WCSimDetectorMessenger_h 1

class WCSimDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADouble;

#include "G4UImessenger.hh"
#include "globals.hh"
#include "G4ios.hh"
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithABool.hh"


class WCSimDetectorMessenger: public G4UImessenger
{
 public:
  WCSimDetectorMessenger(WCSimDetectorConstruction* mpga);
  ~WCSimDetectorMessenger();
  
 public:
  void         SetNewValue(G4UIcommand* command, G4String newValues);
 
 private:
  WCSimDetectorConstruction* WCSimDetector;

 private: //commands
  
  G4UIdirectory*      WCSimDir;
  G4UIcmdWithAString* PMTConfig;
  G4UIcmdWithAString* PMTGeomDetails;
  G4UIcmdWithAString* PMTSize;
  G4UIcmdWithAString* SavePi0;
  G4UIcmdWithAString* WLSPSHAPE;

  G4UIcmdWithAString* PMTQEMethod;
  G4UIcmdWithAString* PMTCollEff;
  G4UIcmdWithAString* PMTCollEff_Method;
  G4UIcmdWithAString* PMT_Timing_Var;
  G4UIcmdWithAString* tubeCmd;
  G4UIcmdWithAString* distortionCmd;
  G4UIcmdWithoutParameter* WCConstruct;

  G4UIcmdWithADouble* LCoffset;
  G4UIcmdWithADouble* LC_rmin;
  G4UIcmdWithADouble* LC_rmax;
  G4UIcmdWithADouble* LC_a;
  G4UIcmdWithADouble* LC_b;
  G4UIcmdWithADouble* LC_d;
  
  G4UIcmdWithADouble* WLSP_offset;
  G4UIcmdWithADouble* WLSP_outradius;
  

  //Winston cone and WLS options: K.Z.
  G4UIcmdWithABool* ConstructWLS;
  G4UIcmdWithABool* ConstructWLSP;
  G4UIcmdWithABool* ConstructLC;

};

#endif


