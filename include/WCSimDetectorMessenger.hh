#ifndef WCSimDetectorMessenger_h
#define WCSimDetectorMessenger_h 1

class WCSimDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;

#include "G4UImessenger.hh"
#include "globals.hh"
#include "G4ios.hh"
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"


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
  G4UIcmdWithAString* PMTQEMethod;
  G4UIcmdWithAString* PMTCollEff;
  G4UIcmdWithADoubleAndUnit* waterTank_Length;

  G4UIdirectory*             mPMTDir;
  G4UIcmdWithADoubleAndUnit* mPMT_CylHeight;
  G4UIcmdWithADoubleAndUnit* mPMT_CylRadius;
  G4UIcmdWithAString*        mPMT_orientation;
  G4UIcmdWithAString*        mPMT_material_inner; 
  G4UIcmdWithAString*        mPMT_material_outer;
  G4UIcmdWithADoubleAndUnit* mPMT_material_inner_thickness; 
  G4UIcmdWithADoubleAndUnit* mPMT_material_outer_thickness; 
  G4UIcmdWithADoubleAndUnit* mPMT_ID_reflector_height;
  G4UIcmdWithADoubleAndUnit* mPMT_ID_reflector_angle;
  G4UIcmdWithAnInteger*      mPMT_nID_PMTs;
  G4UIcmdWithAString*        mPMT_config;
  //G4UIcmdWithAString*        mPMT_PMTtype_inner;  //enums internally
  //G4UIcmdWithAString*        mPMT_PMTtype_outer; 
  
  G4UIcmdWithAString* tubeCmd;
  G4UIcmdWithAString* distortionCmd;
  G4UIcmdWithoutParameter* WCConstruct;

};

#endif


