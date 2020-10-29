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
  G4UIcmdWithAString* WCVisChoice;
  G4UIcmdWithAString* PMTGeomDetails;
  G4UIcmdWithAString* PMTSize;
  G4UIcmdWithAString* PMTSize2;
  G4UIcmdWithAString* SavePi0;
  G4UIcmdWithAString* SaveCapture;
  G4UIcmdWithAString* PMTQEMethod;
  G4UIcmdWithAString* PMTCollEff;
  G4UIcmdWithABool* DopedWater;
  G4UIcmdWithADouble* DopingConcentration;
  G4UIcmdWithADoubleAndUnit* waterTank_Length;

  G4UIdirectory*             mPMTDir;
  G4UIcmdWithADoubleAndUnit* mPMT_CylHeight;
  G4UIcmdWithADoubleAndUnit* mPMT_CylRadius;
  G4UIcmdWithADoubleAndUnit* mPMT_CylRadiusCurv;
  G4UIcmdWithADoubleAndUnit* mPMT_DistPMTVessel;
  G4UIcmdWithAString*        mPMT_orientation;
  G4UIcmdWithAString*        mPMT_material_inner; 
  G4UIcmdWithAString*        mPMT_material_outer;
  G4UIcmdWithAString*        mPMT_PMTtype_inner;
  G4UIcmdWithAString*        mPMT_PMTtype_outer;
  G4UIcmdWithADoubleAndUnit* mPMT_material_outer_thickness; 
  G4UIcmdWithADoubleAndUnit* mPMT_ID_reflector_height;
  G4UIcmdWithADoubleAndUnit* mPMT_ID_reflector_z_offset;
  G4UIcmdWithADoubleAndUnit* mPMT_ID_reflector_angle;
  G4UIcmdWithAnInteger*      mPMT_nID_PMTs;
  G4UIcmdWithAString*        mPMT_config;
  G4UIcmdWithABool*          mPMT_numModulesFixed;
  G4UIcmdWithADoubleAndUnit* mPMT_pmtOpeningAngle;
  G4UIcmdWithAString*        mPMT_material_pmtAssembly;
  //G4UIcmdWithAString*        mPMT_PMTtype_inner;  //enums internally
  //G4UIcmdWithAString*        mPMT_PMTtype_outer; 
  
  G4UIcmdWithAString* tubeCmd;
  G4UIcmdWithAString* distortionCmd;
  G4UIcmdWithoutParameter* WCConstruct;

  //nuPRISM commands
  G4UIcmdWithoutParameter* UpdateNuPrism;
  G4UIcmdWithAString* SetPMTType;
  G4UIcmdWithABool* SetHybridPMT;
  G4UIcmdWithAString* SetPMTCoverage;
  G4UIcmdWithADoubleAndUnit* SetDetectorHeight;
  G4UIcmdWithADoubleAndUnit* SetDetectorVerticalPosition;
  G4UIcmdWithADoubleAndUnit* SetDetectorDiameter;

};

#endif


