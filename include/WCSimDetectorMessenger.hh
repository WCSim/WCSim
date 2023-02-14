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
#include <G4UIcmdWithAnInteger.hh>

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
  G4UIcmdWithAString* SavePi0;
  G4UIcmdWithAString* PMTQEMethod;
  G4UIcmdWithAString* PMTCollEff;
  G4UIcmdWithAString* LCConfig;
  G4UIcmdWithADoubleAndUnit* waterTank_Length;


  G4UIcmdWithAString* tubeCmd;
  G4UIcmdWithAString* distortionCmd;
  G4UIcmdWithoutParameter* WCConstruct;

  // OD Geometry
  G4UIcmdWithADoubleAndUnit* ODLateralWaterDepth;
  G4UIcmdWithADoubleAndUnit* ODHeightWaterDepth;
  G4UIcmdWithADoubleAndUnit* ODDeadSpace;
  G4UIcmdWithADoubleAndUnit* ODTyvekSheetThickness;
  G4UIcmdWithADoubleAndUnit* ODWLSPlatesThickness;
  G4UIcmdWithADoubleAndUnit* ODWLSPlatesLength;
  // OD PMTs
  G4UIcmdWithAString* PMTODRadius;
  // #PMTs and position
  G4UIcmdWithAnInteger* PMTODperCellHorizontal;
  G4UIcmdWithAnInteger* PMTODperCellVertical;
  G4UIcmdWithADouble* PMTODPercentCoverage;
  G4UIcmdWithADouble* PMTODPercentCoverageTop;
  G4UIcmdWithADouble* PMTODPercentCoverageBottom;
  G4UIcmdWithADoubleAndUnit* ODPMTShift;
  // isWLS constructed
  G4UIcmdWithoutParameter* isWLSFilled;
  // build cladding around WLS
  G4UIcmdWithoutParameter* BuildODWLSCladding;

};

#endif


