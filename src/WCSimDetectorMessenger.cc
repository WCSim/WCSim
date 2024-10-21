#include "WCSimDetectorMessenger.hh"

#include "WCSimDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"

WCSimDetectorMessenger::WCSimDetectorMessenger(WCSimDetectorConstruction* WCSimDet)
:WCSimDetector(WCSimDet)
{ 
  WCSimDir = new G4UIdirectory("/WCSim/");
  WCSimDir->SetGuidance("Commands to change the geometry of the simulation");


  PMTConfig = new G4UIcmdWithAString("/WCSim/WCgeom",this);
  PMTConfig->SetGuidance("Set the geometry configuration for the WC.");
  PMTConfig->SetGuidance("Available options are:\n"
			  "SuperK\n"
			  "SuperK_20inchPMT_20perCent\n"
			  "SuperK_20inchBandL_20perCent\n"
			  "SuperK_12inchBandL_15perCent\n"
			  "SuperK_20inchBandL_14perCent\n"
			  "Cylinder_60x74_20inchBandL_14perCent\n"
			  "Cylinder_60x74_20inchBandL_40perCent\n"
			  "Cylinder_12inchHPD_15perCent\n"
                          "HyperK\n"
                          "HyperK_3inch\n"
                          "HyperK_8inch\n"
                          "HyperK_10inch\n"
                          "HyperK_mPMT\n"
                          "HyperK_HybridmPMT\n"
                          "HyperK_HybridmPMT10PC\n"
                          "HyperK_HybridFake\n"
			  "HyperK_20perCent\n"
			  "HyperKWithOD\n"
			  "HyperK20pcWithOD\n"
 			  "HyperK_HybridmPMT_WithOD\n"
        "HyperK_HybridmPMT_WithOD_Realistic\n"
        "HyperK_HybridmPMT_IDonly_Realistic\n"
			  "EggShapedHyperK\n"
			  "EggShapedHyperK_withHPD\n"
                          "nuPRISM\n"
                          "nuPRISM_mPMT\n"
			  "nuPRISMBeamTest_mPMT\n"
			  "nuPRISMBeamTest_16cShort_mPMT\n"
    			  "nuPRISMShort_mPMT\n"
        "IWCD_mPMT\n"
        "IWCD_mPMT_WithOD\n"
        "IWCD_mPMT_WithOD_OptionA\n"
        "IWCD_mPMT_WithOD_OptionC\n"
        "IWCD_mPMT_WithOD_Old\n"
			  "Cylinder_60x74_3inchmPMT_14perCent\n"
			  "Cylinder_60x74_3inchmPMT_40perCent\n"
			  "Cylinder_60x74_3inch_14perCent\n"
			  "Cylinder_60x74_3inch_40perCent\n"
			  "TestSinglemPMT\n"
                         );
  PMTConfig->SetParameterName("PMTConfig", false);
  PMTConfig->SetCandidates("SuperK "
			   "SuperK_20inchPMT_20perCent "
			   "SuperK_20inchBandL_20perCent "
			   "SuperK_12inchBandL_15perCent "
			   "SuperK_20inchBandL_14perCent "
			   "Cylinder_60x74_20inchBandL_14perCent\n"
      			   "Cylinder_60x74_20inchBandL_40perCent\n"
			   "Cylinder_12inchHPD_15perCent "
			   "HyperK "
			   "HyperK_3inch "
			   "HyperK_8inch "
			   "HyperK_10inch "
			   "HyperK_mPMT "
			   "HyperK_HybridmPMT "
			   "HyperK_HybridmPMT10PC "
			   "HyperK_HybridFake "
			   "HyperK_20perCent "
			   "HyperKWithOD "
			   "HyperK20pcWithOD "
			   "HyperK_HybridmPMT_WithOD "
			   "HyperK_HybridmPMT_WithOD_Realistic "
			   "HyperK_HybridmPMT_IDonly_Realistic "
			   "EggShapedHyperK "
			   "EggShapedHyperK_withHPD "
                           "nuPRISM "
                           "nuPRISM_mPMT "
			   "nuPRISMBeamTest_mPMT "
			   "nuPRISMBeamTest_16cShort_mPMT\n"
			   "nuPRISMShort_mPMT "
         "IWCD_mPMT "
         "IWCD_mPMT_WithOD "
         "IWCD_mPMT_WithOD_OptionA "
         "IWCD_mPMT_WithOD_OptionC "
         "IWCD_mPMT_WithOD_Old "
			   "Cylinder_60x74_3inchmPMT_14perCent "
			   "Cylinder_60x74_3inchmPMT_40perCent "
			   "Cylinder_60x74_3inch_14perCent "
			   "Cylinder_60x74_3inch_40perCent "
			   "TestSinglemPMT\n"
                           );
  PMTConfig->AvailableForStates(G4State_PreInit, G4State_Idle);

  WCVisChoice = new G4UIcmdWithAString("/WCSim/WCVisChoice",this);
  WCVisChoice->SetGuidance("Set the visualization style for the WC.");
  WCVisChoice->SetGuidance("Available options are:\n"
			  "OGLSX\n"
			  "RayTracer\n"
			  );
  WCVisChoice->SetParameterName("WCVisChoice", false);
  WCVisChoice->SetCandidates("OGLSX "
			  "RayTracer "
			  );
  WCVisChoice->AvailableForStates(G4State_PreInit, G4State_Idle);

  DopedWater = new G4UIcmdWithABool("/WCSim/DopedWater", this);
  DopedWater->SetGuidance("Set whether water is doped with Gadolinium");
  DopedWater->SetParameterName("DopedWater", false);
  DopedWater->AvailableForStates(G4State_PreInit, G4State_Idle);

  DopingConcentration = new G4UIcmdWithADouble("/WCSim/DopingConcentration", this);
  DopingConcentration->SetGuidance("Set percentage concentration Gadolinium doping");
  DopingConcentration->SetParameterName("DopingConcentration", false);
  DopingConcentration->AvailableForStates(G4State_PreInit, G4State_Idle);

  BGOPlacement = new G4UIcmdWithABool("/WCSim/BGOPlacement", this);
  BGOPlacement->SetGuidance("Place BGO Scintillation Crystal Inside Detector");
  BGOPlacement->SetParameterName("BGOPlacement", false);
  BGOPlacement->AvailableForStates(G4State_PreInit, G4State_Idle);

  BGOPosition = new G4UIcmdWith3VectorAndUnit("/WCSim/BGOPosition", this);
  BGOPosition->SetGuidance("Set BGO position inside the tank (unit: mm cm m). Default will be 0 0 0 mm");
  BGOPosition->SetParameterName("X", "Y", "Z", false);
  BGOPosition->SetDefaultValue(G4ThreeVector(0,0,0));
  BGOPosition->SetUnitCategory("Length");
  BGOPosition->SetDefaultUnit("mm");

  PMTSize = new G4UIcmdWithAString("/WCSim/WCPMTsize",this);
  PMTSize->SetGuidance("Set alternate PMT size for the WC (Must be entered after geometry details are set).");
  PMTSize->SetGuidance("Available options 20inch 10inch");
  PMTSize->SetParameterName("PMTSize", false);
  PMTSize->SetCandidates("20inch 10inch");
  PMTSize->AvailableForStates(G4State_PreInit, G4State_Idle);

  PMTSize2 = new G4UIcmdWithAString("/WCSim/WCPMTsize2",this);//B.Q, for the second PMT type
  PMTSize2->SetGuidance("Set alternate PMT size for the WC (Must be entered after geometry details are set).");
  PMTSize2->SetGuidance("Available options 20inch 10inch");
  PMTSize2->SetParameterName("PMTSize2", false);
  PMTSize2->SetCandidates("20inch 10inch");
  PMTSize2->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  SavePi0 = new G4UIcmdWithAString("/WCSim/SavePi0", this);
  SavePi0->SetGuidance("true or false");
  SavePi0->SetParameterName("SavePi0",false);
  SavePi0->SetCandidates("true false");
  SavePi0->AvailableForStates(G4State_PreInit, G4State_Idle);

  SaveCapture = new G4UIcmdWithAString("/WCSim/SaveCapture", this);
  SaveCapture->SetGuidance("true or false");
  SaveCapture->SetParameterName("SaveCapture",false);
  SaveCapture->SetCandidates("true false");
  SaveCapture->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  
  PMTQEMethod = new G4UIcmdWithAString("/WCSim/PMTQEMethod", this);
  PMTQEMethod->SetGuidance("Set the PMT configuration.");
  PMTQEMethod->SetGuidance("Available options are:\n"
			     "Stacking_Only\n"
			     "Stacking_And_SensitiveDetector\n"
			     "SensitiveDetector_Only\n"
			     "DoNotApplyQE");

  PMTQEMethod->SetParameterName("PMTQEMethod", false);
  PMTQEMethod->SetCandidates("Stacking_Only "
			     "Stacking_And_SensitiveDetector "
			     "SensitiveDetector_Only "
			     "DoNotApplyQE ");

  PMTQEMethod->AvailableForStates(G4State_PreInit, G4State_Idle);

  PMTCollEff = new G4UIcmdWithAString("/WCSim/PMTCollEff", this);
  PMTCollEff->SetGuidance("Set the PMT configuration.");
  PMTCollEff->SetGuidance("Available options are:\n"
			  "on\n"
			  "off\n");
  PMTCollEff->SetParameterName("PMTCollEff", false);
  PMTCollEff->SetCandidates("on "
			    "off ");
  PMTCollEff->AvailableForStates(G4State_PreInit, G4State_Idle);

  // PMT coverage
  SetPMTCoverage = new G4UIcmdWithADouble("/WCSim/SetPMTPercentCoverage", this);
  SetPMTCoverage->SetGuidance("Set the PMT percentage coverage to be used. Should be necessarily called before WCGeom, as the detector parameters are defined there based on coverage.");
  SetPMTCoverage->SetGuidance("Any number is allowed (such as 40)");
  SetPMTCoverage->SetParameterName("PMTCoverage", true);
  SetPMTCoverage->SetDefaultValue(-1.);

  // PMT coverage for second PMT type
  SetPMTCoverage2 = new G4UIcmdWithADouble("/WCSim/SetPMTPercentCoverage2", this);
  SetPMTCoverage2->SetGuidance("Set the PMT percentage coverage to be used for second PMT type. Should be necessarily called before WCGeom, as the detector parameters are defined there based on coverage.");
  SetPMTCoverage2->SetGuidance("Any number is allowed (such as 40)");
  SetPMTCoverage2->SetParameterName("PMTCoverage2", false);
  SetPMTCoverage2->SetDefaultValue(-1.);

  waterTank_Length = new G4UIcmdWithADoubleAndUnit("/WCSim/EggShapedHyperK/waterTank_Length", this);
  waterTank_Length->SetGuidance("Set the Length of the egg-shaped HyperK detector (unit: mm cm m).");
  waterTank_Length->SetParameterName("waterTank_length", true);
  waterTank_Length->SetDefaultValue(49500.);
  waterTank_Length->SetUnitCategory("Length");
  waterTank_Length->SetDefaultUnit("mm");
  waterTank_Length->SetUnitCandidates("mm cm m");

  /////////////////////////////////
  ////////////// OD ///////////////
  /////////////////////////////////

  // PMT size
  PMTODRadius = new G4UIcmdWithAString("/WCSim/HyperKOD/PMTODRadius", this);
  PMTODRadius->SetGuidance("Set the size of OD PMTs (only for Hyper-K Geom atm)");
  PMTODRadius->SetGuidance("Available options are:\n"
						  "3inch\n"
						  "5inch\n"
						  "8inch\n"
			                          "BoxandLine20inchHQE\n");
  PMTODRadius->SetParameterName("PMTODRadius", false);
  PMTODRadius->SetCandidates("3inch "
			     "5inch "
                             "8inch "
			     "BoxandLine20inchHQE ");
  PMTODRadius->AvailableForStates(G4State_PreInit, G4State_Idle);

  // OD Lateral water depth
  ODLateralWaterDepth = new G4UIcmdWithADoubleAndUnit("/WCSim/HyperKOD/ODLateralWaterDepth", this);
  ODLateralWaterDepth->SetGuidance("Set water depth of LATERAL segment of the OD (unit: m cm mm).");
  ODLateralWaterDepth->SetParameterName("ODLateralWaterDepth", true);
  ODLateralWaterDepth->SetDefaultValue(1.);
  ODLateralWaterDepth->SetUnitCategory("Length");
  ODLateralWaterDepth->SetDefaultUnit("m");
  ODLateralWaterDepth->SetUnitCandidates("m cm mm");

  // OD Height water depth
  ODHeightWaterDepth = new G4UIcmdWithADoubleAndUnit("/WCSim/HyperKOD/ODHeightWaterDepth", this);
  ODHeightWaterDepth->SetGuidance("Set water depth of HEIGHT segment of the OD (unit: m cm mm).");
  ODHeightWaterDepth->SetParameterName("ODHeightWaterDepth", true);
  ODHeightWaterDepth->SetDefaultValue(2.);
  ODHeightWaterDepth->SetUnitCategory("Length");
  ODHeightWaterDepth->SetDefaultUnit("m");
  ODHeightWaterDepth->SetUnitCandidates("m cm mm");

  // OD Dead space size
  ODDeadSpace = new G4UIcmdWithADoubleAndUnit("/WCSim/HyperKOD/ODDeadSpace", this);
  ODDeadSpace->SetGuidance("Set dead space width between ID and OD (unit: m cm mm).");
  ODDeadSpace->SetParameterName("ODDeadSpace", true);
  ODDeadSpace->SetDefaultValue(0.6);
  ODDeadSpace->SetUnitCategory("Length");
  ODDeadSpace->SetDefaultUnit("m");
  ODDeadSpace->SetUnitCandidates("m cm");

  // OD Tyvek sheet thickness
  ODTyvekSheetThickness = new G4UIcmdWithADoubleAndUnit("/WCSim/HyperKOD/ODTyvekSheetThickness", this);
  ODTyvekSheetThickness->SetGuidance("Set OD Tyvek sheet thickness (unit: mm).");
  ODTyvekSheetThickness->SetParameterName("ODTyvekSheetThickness", true);
  ODTyvekSheetThickness->SetDefaultValue(2.);
  ODTyvekSheetThickness->SetUnitCategory("Length");
  ODTyvekSheetThickness->SetDefaultUnit("mm");
  ODTyvekSheetThickness->SetUnitCandidates("mm");

  // OD WLS Plates thickness
  ODWLSPlatesThickness = new G4UIcmdWithADoubleAndUnit("/WCSim/HyperKOD/ODWLSPlatesThickness", this);
  ODWLSPlatesThickness->SetGuidance("Set OD WLS plates thickness (unit: cm mm).");
  ODWLSPlatesThickness->SetParameterName("ODWLSPlatesThickness", true);
  ODWLSPlatesThickness->SetDefaultValue(1.);
  ODWLSPlatesThickness->SetUnitCategory("Length");
  ODWLSPlatesThickness->SetDefaultUnit("cm");
  ODWLSPlatesThickness->SetUnitCandidates("cm mm");

  // OD WLS Plates length
  ODWLSPlatesLength = new G4UIcmdWithADoubleAndUnit("/WCSim/HyperKOD/ODWLSPlatesLength", this);
  ODWLSPlatesLength->SetGuidance("Set OD WLS plates length (unit: cm mm).");
  ODWLSPlatesLength->SetParameterName("ODWLSPlatesLength", true);
  ODWLSPlatesLength->SetDefaultValue(60.);
  ODWLSPlatesLength->SetUnitCategory("Length");
  ODWLSPlatesLength->SetDefaultUnit("cm");
  ODWLSPlatesLength->SetUnitCandidates("cm mm");

  // Nb of OD PMT per cell HORIZONTALLY
  PMTODperCellHorizontal = new G4UIcmdWithAnInteger("/WCSim/HyperKOD/PMTODperCellHorizontal", this);
  PMTODperCellHorizontal->SetGuidance("Set number of OD PMT per cell HORIZONTALLY.");
  PMTODperCellHorizontal->SetParameterName("PMTODperCellHorizontal", true);
  PMTODperCellHorizontal->SetDefaultValue(1);
  PMTODperCellHorizontal->SetRange("PMTODperCellHorizontal>=0");

  // Nb of OD PMT per cell HORIZONTALLY
  PMTODperCellVertical = new G4UIcmdWithAnInteger("/WCSim/HyperKOD/PMTODperCellVertical", this);
  PMTODperCellVertical->SetGuidance("Set number of OD PMT per cell VERTICALLY.");
  PMTODperCellVertical->SetParameterName("PMTODperCellVertical", true);
  PMTODperCellVertical->SetDefaultValue(1);
  PMTODperCellVertical->SetRange("PMTODperCellVertical>=0");

  // Nb of OD PMT per cell HORIZONTALLY
  PMTODPercentCoverage = new G4UIcmdWithADouble("/WCSim/HyperKOD/PMTODPercentCoverage", this);
  PMTODPercentCoverage->SetGuidance("Set global OD photocoverage percentage.");
  PMTODPercentCoverage->SetParameterName("PMTODPercentCoverage", true);
  PMTODPercentCoverage->SetDefaultValue(1.);
  PMTODPercentCoverage->SetRange("PMTODPercentCoverage>0");

  PMTODPercentCoverageTop = new G4UIcmdWithADouble("/WCSim/HyperKOD/PMTODPercentCoverageTop", this);
  PMTODPercentCoverageTop->SetGuidance("Set Top OD photocoverage percentage.");
  PMTODPercentCoverageTop->SetParameterName("PMTODPercentCoverageTop", true);
  PMTODPercentCoverageTop->SetDefaultValue(1.);
  //  PMTODPercentCoverageTop->SetRange("PMTODPercentCoverageTop>0");

  PMTODPercentCoverageBottom = new G4UIcmdWithADouble("/WCSim/HyperKOD/PMTODPercentCoverageBottom", this);
  PMTODPercentCoverageBottom->SetGuidance("Set Bottom OD photocoverage percentage.");
  PMTODPercentCoverageBottom->SetParameterName("PMTODPercentCoverageBottom", true);
  PMTODPercentCoverageBottom->SetDefaultValue(1.);
  //  PMTODPercentCoverageBottom->SetRange("PMTODPercentCoverageBottom>0");

  PMTODPercentCoverageBarrel = new G4UIcmdWithADouble("/WCSim/HyperKOD/PMTODPercentCoverageBarrel", this);
  PMTODPercentCoverageBarrel->SetGuidance("Set Barrel OD photocoverage percentage.");
  PMTODPercentCoverageBarrel->SetParameterName("PMTODPercentCoverageBarrel", true);
  PMTODPercentCoverageBarrel->SetDefaultValue(1.);
  //  PMTODPercentCoverageBarrel->SetRange("PMTODPercentCoverageBarrel>0");

  // OD Tyvek sheet thickness
  ODPMTShift = new G4UIcmdWithADoubleAndUnit("/WCSim/HyperKOD/ODPMTShift", this);
  ODPMTShift->SetGuidance("Set a horizontal shift between rows of PMTs OD (unit: m cm mm).");
  ODPMTShift->SetParameterName("ODPMTShift", true);
  ODPMTShift->SetDefaultValue(0.);
  ODPMTShift->SetUnitCategory("Length");
  ODPMTShift->SetDefaultUnit("m");
  ODPMTShift->SetUnitCandidates("m cm mm");

  // Fill the OD WLS with material
  isWLSFilled = new G4UIcmdWithoutParameter("/WCSim/HyperKOD/DeactivateWLS", this);
  isWLSFilled->SetGuidance("Deactivate WLS plates by filling them with water");

  // Build reflective cladding around WLS plate
  BuildODWLSCladding = new G4UIcmdWithoutParameter("/WCSim/HyperKOD/BuildODWLSCladding", this);
  BuildODWLSCladding->SetGuidance("Build reflective cladding around WLS plate");

  /////////// END OD //////////////
  /////////////////////////////////


  LCConfig = new G4UIcmdWithAString("/WCSim/LCConfig",this);
  LCConfig->SetGuidance("Set the geometry configuration for the light collecting mirror.");
  LCConfig->SetGuidance("**For 20 inch PMT Only**");
  LCConfig->SetGuidance("Available options are:\n"
			  "No_Mirror\n"
			  "Mirror_OldLC\n"
			  "Mirror_2018Oct\n"
			  );
  LCConfig->SetParameterName("LCConfig", true);
  LCConfig->SetCandidates("No_Mirror "
			  "Mirror_OldLC "
			  "Mirror_2018Oct "
			  );
  LCConfig->SetDefaultValue("No_Mirror");
  LCConfig->AvailableForStates(G4State_PreInit, G4State_Idle);

  WCConstruct = new G4UIcmdWithoutParameter("/WCSim/Construct", this);
  WCConstruct->SetGuidance("Update detector construction with new settings.");

  // Params: - Cylinder height and radius of mPMT: DONE
  //         - Type of ID PMT (should be enum) and Type of OD PMT: Need to reorganize the PMTpart.
  //         - Params related to filling PMT: txt filename
  //         - Orientation of mPMT wrt wall : horizontal/vertical/perp
  //         - Reflector OFF/Winston/other
  //         - material of mPMT wall: water/glass/acrylic/
  //         - acrylic cover ON/OFF

  mPMTDir = new G4UIdirectory("/WCSim/mPMT/");
  mPMTDir->SetGuidance("Commands to change the properties of the multiPMT.");

  mPMT_CylHeight = new G4UIcmdWithADoubleAndUnit("/WCSim/mPMT/VesselCylHeight",this);
  mPMT_CylHeight->SetGuidance("Set height of cylinder of pressure vessel of multiPMT object.");
  mPMT_CylHeight->SetParameterName("CylHeight", true);
  mPMT_CylHeight->SetDefaultValue(50.); //mDOM (PINGU), 0 for KM3Net
  mPMT_CylHeight->SetUnitCategory("Length");
  mPMT_CylHeight->SetDefaultUnit("mm");
  mPMT_CylHeight->SetUnitCandidates("mm cm m");

  mPMT_CylRadius = new G4UIcmdWithADoubleAndUnit("/WCSim/mPMT/VesselRadius",this);
  mPMT_CylRadius->SetGuidance("Set vessel radius of multiPMT object.");
  mPMT_CylRadius->SetParameterName("CylRadius", true);
  mPMT_CylRadius->SetDefaultValue(325.); 
  mPMT_CylRadius->SetUnitCategory("Length");
  mPMT_CylRadius->SetDefaultUnit("mm");
  mPMT_CylRadius->SetUnitCandidates("mm cm m");

  mPMT_CylRadiusCurv = new G4UIcmdWithADoubleAndUnit("/WCSim/mPMT/VesselRadiusCurv",this);
  mPMT_CylRadiusCurv->SetGuidance("Set vessel radius of curvature of multiPMT object.");
  mPMT_CylRadiusCurv->SetParameterName("CylRadiusCurv", true);
  mPMT_CylRadiusCurv->SetDefaultValue(325.); 
  mPMT_CylRadiusCurv->SetUnitCategory("Length");
  mPMT_CylRadiusCurv->SetDefaultUnit("mm");
  mPMT_CylRadiusCurv->SetUnitCandidates("mm cm m");

  mPMT_orientation = new G4UIcmdWithAString("/WCSim/mPMT/orientation",this);
  mPMT_orientation->SetGuidance("Set orientation of multiPMT cylinder wrt wall.");
  mPMT_orientation->SetParameterName("orientation", true);
  mPMT_orientation->SetCandidates("Horizontal "
				  "Vertical " 
				  "Perpendicular "
				  ); 

  // untested/unfinished
  // This makes it possible to overwrite the DetectorConfigs
  
  mPMT_PMTtype_inner = new G4UIcmdWithAString("/WCSim/mPMT/PMTtype_inner",this);
  mPMT_PMTtype_inner->SetGuidance("Set type of PMT for ID.");
  mPMT_PMTtype_inner->SetParameterName("PMTtype_inner", true);
  mPMT_PMTtype_inner->SetCandidates("PMT20inch "
				    "PMT8inch "
				    "PMT10inch "
				    "PMT10inchHQE "
				    "PMT12inchHQE "
				    "HPD20inchHQE "
				    "HPD12inchHQE "
				    "BoxandLine20inchHQE "
				    "BoxandLine12inchHQE "
				    "PMT3inchR12199_02 "
				    "PMT3inchR14374 "
				    "PMT3inchR14374_WCTE "
				    "PMT3inch_ETEL9302B "
				    "PMT4inchR12199_02 "
				    "PMT5inchR12199_02 "
				    ); 

  mPMT_PMTtype_outer = new G4UIcmdWithAString("/WCSim/mPMT/PMTtype_outer",this);
  mPMT_PMTtype_outer->SetGuidance("Set type of PMT for OD.");
  mPMT_PMTtype_outer->SetParameterName("PMTtype_outer", true);
  mPMT_PMTtype_outer->SetCandidates("PMT20inch "
				    "PMT8inch "
				    "PMT10inch "
				    "PMT10inchHQE "
				    "PMT12inchHQE "
				    "HPD20inchHQE "
				    "HPD12inchHQE "
				    "BoxandLine20inchHQE "
				    "BoxandLine12inchHQE "
				    "PMT3inchR12199_02 "
				    "PMT3inchR14374 "
				    "PMT3inchR14374_WCTE "
				    "PMT3inch_ETEL9302B "
				    "PMT4inchR12199_02 "
				    "PMT5inchR12199_02 "
				    ); 
  

  mPMT_material_outer = new G4UIcmdWithAString("/WCSim/mPMT/material_outer",this);
  mPMT_material_outer->SetGuidance("Set material of outer capsule of multiPMT.");
  mPMT_material_outer->SetParameterName("material_outer", true);
  mPMT_material_outer->SetCandidates("Water "
				     "Glass "
				     "Acrylic "); 

  mPMT_material_inner = new G4UIcmdWithAString("/WCSim/mPMT/material_inner",this);
  mPMT_material_inner->SetGuidance("Set material of region in between PMTs.");
  mPMT_material_inner->SetParameterName("material_inner", true);
  mPMT_material_inner->SetCandidates("Water "
				     "SilGel "
				     "Air " );

  // Thickness of outer and inner material
  mPMT_material_outer_thickness = new G4UIcmdWithADoubleAndUnit("/WCSim/mPMT/material_outer_thickness",this);
  mPMT_material_outer_thickness->SetGuidance("Set thickness of outer (pressure) vessel of multiPMT.");
  mPMT_material_outer_thickness->SetParameterName("material_outer_thickness", true);
  mPMT_material_outer_thickness->SetDefaultValue(20.); 
  mPMT_material_outer_thickness->SetUnitCategory("Length");
  mPMT_material_outer_thickness->SetDefaultUnit("mm");
  mPMT_material_outer_thickness->SetUnitCandidates("mm cm m");


  mPMT_ID_reflector_height = new G4UIcmdWithADoubleAndUnit("/WCSim/mPMT/reflectorHeightID",this);
  mPMT_ID_reflector_height->SetGuidance("Set height of reflector cone for each ID PMT.");
  mPMT_ID_reflector_height->SetParameterName("reflectorHeightID", true);
  mPMT_ID_reflector_height->SetDefaultValue(7.5); 
  mPMT_ID_reflector_height->SetUnitCategory("Length");
  mPMT_ID_reflector_height->SetDefaultUnit("mm");
  mPMT_ID_reflector_height->SetUnitCandidates("mm cm m");
  
  mPMT_ID_reflector_z_offset = new G4UIcmdWithADoubleAndUnit("/WCSim/mPMT/reflectorZoffsetID",this);
  mPMT_ID_reflector_z_offset->SetGuidance("Set z position offset of reflector cone for each ID PMT.");
  mPMT_ID_reflector_z_offset->SetParameterName("reflectorZoffsetID", true);
  mPMT_ID_reflector_z_offset->SetDefaultValue(12); 
  mPMT_ID_reflector_z_offset->SetUnitCategory("Length");
  mPMT_ID_reflector_z_offset->SetDefaultUnit("mm");
  mPMT_ID_reflector_z_offset->SetUnitCandidates("mm cm m");


  mPMT_ID_reflector_angle = new G4UIcmdWithADoubleAndUnit("/WCSim/mPMT/reflectorAngleID",this);
  mPMT_ID_reflector_angle->SetGuidance("Set angle of reflector cone wrt PMT axis for each ID PMT.");
  mPMT_ID_reflector_angle->SetParameterName("reflectorAngleID", true);
  mPMT_ID_reflector_angle->SetDefaultValue(45.); 
  mPMT_ID_reflector_angle->SetUnitCategory("Angle");
  mPMT_ID_reflector_angle->SetDefaultUnit("deg");
  mPMT_ID_reflector_angle->SetUnitCandidates("deg rad");


  // Options related to mPMT filling:
  // Total number of ID PMTs: used to cross check with provided configfile
  mPMT_nID_PMTs = new G4UIcmdWithAnInteger("/WCSim/mPMT/nID_PMTs",this);
  mPMT_nID_PMTs->SetGuidance("Set number of ID mPMT PMTs.");
  mPMT_nID_PMTs->SetParameterName("nID_PMTs", true);
  mPMT_nID_PMTs->SetDefaultValue(20.); 
  
  mPMT_config = new G4UIcmdWithAString("/WCSim/mPMT/configfile",this);
  mPMT_config->SetGuidance("Set filename for config file with viewing angles and tilt angles.");
  mPMT_config->SetParameterName("mPMTconfig", true);

  // NEW!
  mPMT_numModulesFixed = new G4UIcmdWithABool("/WCSim/mPMT/keep_num_mPMTs_fixed",this);
  mPMT_numModulesFixed->SetGuidance("Keep the number of mPMT modules in the detector fixed even though mPMT dimensions change.");
  mPMT_numModulesFixed->SetParameterName("numModulesFixed",true);
  mPMT_numModulesFixed->SetDefaultValue(false);
  
  mPMT_pmtOpeningAngle = new G4UIcmdWithADoubleAndUnit("/WCSim/mPMT/pmt_opening_angle",this);
  mPMT_pmtOpeningAngle->SetGuidance("Set size of PMT sub-assembly (opening angle). Must be able to contain reflector if wanted."); 
  mPMT_pmtOpeningAngle->SetParameterName("pmtOpeningAngle", true);
  mPMT_pmtOpeningAngle->SetDefaultValue(8.7);
  mPMT_pmtOpeningAngle->SetUnitCategory("Angle");
  mPMT_pmtOpeningAngle->SetDefaultUnit("deg");
  mPMT_pmtOpeningAngle->SetUnitCandidates("deg rad");

  mPMT_material_pmtAssembly = new G4UIcmdWithAString("/WCSim/mPMT/material_pmtAssembly",this);
  mPMT_material_pmtAssembly->SetGuidance("Set material of region around the PMT itself.");
  mPMT_material_pmtAssembly->SetParameterName("material_pmtAssembly", true);
  mPMT_material_pmtAssembly->SetCandidates("Water "
					   "SilGel "
					   "Air " );

  // B.Q: Add the hybrid configuration
  SetHybridPMT = new G4UIcmdWithABool("/WCSim/mPMT/SetHybridPMT", this);
  SetHybridPMT->SetGuidance("Set the possibility of having hybrid PMT types");
  SetHybridPMT->SetGuidance("Should be 1 or 0");
  SetHybridPMT->SetParameterName("HybridPMT", false);
  //SetHybridPMT->SetCandidates();
  SetHybridPMT->SetDefaultValue(false);

 // First, the PMT type
  SetPMTType = new G4UIcmdWithAString("/WCSim/nuPRISM/SetPMTType", this);
  SetPMTType->SetGuidance("Set the type of PMT to be used for nuPRISM");
  SetPMTType->SetGuidance("Available options are:\n"
          "PMT3inch\n"
          "PMT3inchGT\n"
          "PMT3inchR12199_02\n"
          "PMT3inchR14374\n"
	  "PMT3inchR14374_WCTE\n"
	  "PMT3inch_ETEL9302B\n"
          "PMT5inch\n"
          "PMT8inch\n"
          "PMT10inchHQE\n"
          "PMT10inch\n"
          "PMT12inchHQE\n"
          "HPD20inchHQE\n"
          "PMT20inch\n");
  SetPMTType->SetParameterName("PMTType", false);
  SetPMTType->SetCandidates("PMT3inch PMT3inchGT PMT3inchR12199_02 PMT3inchR14374 PMT3inch_ETEL9302B PMT5inch PMT8inch PMT10inchHQE PMT10inch PMT12inchHQE HPD20inchHQE PMT20inch");
  SetPMTType->SetDefaultValue("PMT10inch");

  // Set the vertical position of the nuPRISM-lite detector
  SetDetectorVerticalPosition = new G4UIcmdWithADoubleAndUnit("/WCSim/nuPRISM/SetDetectorVerticalPosition", this);
  SetDetectorVerticalPosition->SetGuidance("Set the vertical position of the nuPRISM inner detector (unit mm cm m).");
  SetDetectorVerticalPosition->SetGuidance("The default will be 0m, so particle guns are easy to create.");
  SetDetectorVerticalPosition->SetParameterName("DetectorVerticalPosition", false);
  SetDetectorVerticalPosition->SetDefaultValue(0.0);
  SetDetectorVerticalPosition->SetUnitCategory("Length");
  SetDetectorVerticalPosition->SetDefaultUnit("m");

  // Set the height of the nuPRISM-lite detector
  SetDetectorHeight = new G4UIcmdWithADoubleAndUnit("/WCSim/nuPRISM/SetDetectorHeight", this);
  SetDetectorHeight->SetGuidance("Set the height of the nuPRISM inner detector (unit mm cm m).");
  SetDetectorHeight->SetGuidance("The default will be 52.4m, the full height of nuPRISM.");
  SetDetectorHeight->SetParameterName("DetectorHeight", false);
  SetDetectorHeight->SetDefaultValue(52.4);
  SetDetectorHeight->SetUnitCategory("Length");
  SetDetectorHeight->SetDefaultUnit("m");

  // Set the diameter of the nuPRISM-lite detector
  SetDetectorDiameter = new G4UIcmdWithADoubleAndUnit("/WCSim/nuPRISM/SetDetectorDiameter", this);
  SetDetectorDiameter->SetGuidance("Set the diameter of the nuPRISM inner detector (unit mm cm m).");
  SetDetectorDiameter->SetGuidance("The default will be 6m, the nominal diameter of nuPRISM.");
  SetDetectorDiameter->SetParameterName("DetectorDiameter", false);
  SetDetectorDiameter->SetDefaultValue(6.);
  SetDetectorDiameter->SetUnitCategory("Length");
  SetDetectorDiameter->SetDefaultUnit("m");

  // Rotate barrel by half a tower for alternative alignment of barrel (m)PMTs
  RotateBarrelHalfTower = new G4UIcmdWithABool("/WCSim/Geometry/RotateBarrelHalfTower",this);
  RotateBarrelHalfTower->SetGuidance("Rotate barrel by half a tower for alternative alignment of barrel (m)PMTs");
  RotateBarrelHalfTower->SetParameterName("RotateBarrelHalfTower",false);
  RotateBarrelHalfTower->SetDefaultValue(false);

  // Use the default replica method to place PMTs or not
  UseReplica = new G4UIcmdWithABool("/WCSim/PMT/ReplicaPlacement",this);
  UseReplica->SetGuidance("Use replica method to place PMTs (default = true)");
  UseReplica->SetParameterName("UseReplica",true);
  UseReplica->SetDefaultValue(true);

  // Apply random fluctuation to PMT placement
  PMTPosVar = new G4UIcmdWithADoubleAndUnit("/WCSim/PMT/PositionVariation", this);
  PMTPosVar->SetGuidance("Set the position variation in PMT placement (unit: mm cm m). Default will be 0 mm");
  PMTPosVar->SetParameterName("PMTPositionVariation", false);
  PMTPosVar->SetDefaultValue(0.0);
  PMTPosVar->SetUnitCategory("Length");
  PMTPosVar->SetDefaultUnit("mm");

  // Change ID radius for PMT placement
  TankRadiusChange = new G4UIcmdWith3VectorAndUnit("/WCSim/PMT/TankRadiusChange", this);
  TankRadiusChange->SetGuidance("Set the tank radius change at top, middle and bottom for PMT placement (unit: mm cm m). Default will be 0 0 0 mm");
  TankRadiusChange->SetParameterName("TopRadiusChange","MidRadiusChange","BotRadiusChange", false);
  TankRadiusChange->SetDefaultValue(G4ThreeVector(0,0,0));
  TankRadiusChange->SetUnitCategory("Length");
  TankRadiusChange->SetDefaultUnit("mm");

  // Set the input file to read PMT positions
  SetPMTPositionInput = new G4UIcmdWithAString("/WCSim/PMT/PositionFile",this);
  SetPMTPositionInput->SetGuidance("Set filename for PMT position file");
  SetPMTPositionInput->SetParameterName("PMTPositionInput", true);

  // Set CDS file input
  SetCDSFile = new G4UIcmdWithAString("/WCSim/Geometry/SetCDSFile",this);
  SetCDSFile->SetGuidance("Set filename for CDS model file");
  SetCDSFile->SetParameterName("CDSFileInput", true);
}

WCSimDetectorMessenger::~WCSimDetectorMessenger()
{
  delete PMTConfig;
  delete LCConfig;
  delete SavePi0;
  delete SaveCapture;
  delete PMTQEMethod;
  delete PMTCollEff;
  delete waterTank_Length;
  delete WCVisChoice;

  delete SetDetectorDiameter;
  delete SetDetectorHeight;
  delete SetDetectorVerticalPosition;
  delete SetPMTCoverage;
  delete SetPMTCoverage2;
  delete SetPMTType;
  delete SetHybridPMT;

  delete tubeCmd;
  delete distortionCmd;

  delete mPMT_CylHeight;
  delete mPMT_CylRadius;
  delete WCSimDir;
  delete mPMTDir;
  delete RotateBarrelHalfTower;

  delete UseReplica;
  delete PMTPosVar;
  delete TankRadiusChange;
  delete SetPMTPositionInput;
  delete SetCDSFile;

  delete BGOPlacement;
  delete BGOPosition;
}

void WCSimDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{    
	if( command == PMTConfig ) { 
		WCSimDetector->SetIsUpright(false);
		WCSimDetector->SetIsEggShapedHyperK(false);
		if ( newValue == "SuperK") {
		  WCSimDetector->SetSuperKGeometry();
		} else if ( newValue == "SuperK_20inchPMT_20perCent" ){
		  WCSimDetector->SuperK_20inchPMT_20perCent();
		} else if ( newValue == "SuperK_20inchBandL_20perCent" ){
		  WCSimDetector->SuperK_20inchBandL_20perCent();
		} else if ( newValue == "SuperK_12inchBandL_15perCent" ) {
		  WCSimDetector->SuperK_12inchBandL_15perCent();
		} else if ( newValue == "SuperK_20inchBandL_14perCent" ) {
		  WCSimDetector->SuperK_20inchBandL_14perCent();
		} else if ( newValue == "Cylinder_60x74_20inchBandL_14perCent" ) {
		  WCSimDetector->Cylinder_60x74_20inchBandL_14perCent();
		} else if ( newValue == "Cylinder_60x74_20inchBandL_40perCent" ) {
		  WCSimDetector->Cylinder_60x74_20inchBandL_40perCent();
		} else if ( newValue == "Cylinder_12inchHPD_15perCent" ){
		  WCSimDetector->Cylinder_12inchHPD_15perCent();
                } else if ( newValue == "HyperK") {
			WCSimDetector->SetHyperKGeometry();
		} else if ( newValue == "HyperK_3inch") {
			WCSimDetector->SetHyperK_3inchGeometry();
		} else if ( newValue == "HyperK_8inch") {
			WCSimDetector->SetHyperK_8inchGeometry();
		} else if ( newValue == "HyperK_10inch") {
			WCSimDetector->SetHyperK_10inchGeometry();
		} else if ( newValue == "HyperK_mPMT") {
			WCSimDetector->SetHyperK_mPMTGeometry();
		} else if ( newValue == "HyperK_HybridmPMT") {
		  WCSimDetector->SetHyperK_HybridmPMTGeometry();
		} else if ( newValue == "HyperK_HybridmPMT10PC") {
			WCSimDetector->SetHyperK_HybridmPMT10PCGeometry();
		} else if ( newValue == "HyperK_HybridFake") {
			WCSimDetector->SetHyperK_HybridFakeGeometry();
		} else if ( newValue == "HyperK_20perCent" ){
		  WCSimDetector->SetHyperKGeometry_20perCent();
		} else if ( newValue == "HyperKWithOD" ){
		  WCSimDetector->SetHyperKWithODGeometry();
		  WCSimDetector->SetODEdited(false);
		} else if ( newValue == "HyperK20pcWithOD" ){
		  WCSimDetector->SetHyperK20pcWithODGeometry();
		  WCSimDetector->SetODEdited(false);
		} else if ( newValue == "HyperK_HybridmPMT_WithOD") {
		  WCSimDetector->SetHyperK_HybridmPMT_WithOD_Geometry();
		  WCSimDetector->SetODEdited(false);
    } else if ( newValue == "HyperK_HybridmPMT_WithOD_Realistic") {
		  WCSimDetector->SetHyperK_HybridmPMT_WithOD_Realistic_Geometry();
		  WCSimDetector->SetODEdited(false);
    } else if ( newValue == "HyperK_HybridmPMT_IDonly_Realistic") {
		  WCSimDetector->SetHyperK_HybridmPMT_IDonly_Realistic_Geometry();
		  WCSimDetector->SetODEdited(false);
		} else if ( newValue == "EggShapedHyperK") {
		  WCSimDetector->SetIsEggShapedHyperK(true);
		  WCSimDetector->SetEggShapedHyperKGeometry();
		} else if ( newValue == "EggShapedHyperK_withHPD") {
		  WCSimDetector->SetIsEggShapedHyperK(true);
		  WCSimDetector->SetEggShapedHyperKGeometry_withHPD();
		} else if(newValue == "TestSinglemPMT") {
		  WCSimDetector->SetTestSinglemPMTGeometry();	
		} else if(newValue == "Cylinder_60x74_3inchmPMT_14perCent" ) {
		  WCSimDetector->Cylinder_60x74_3inchmPMT_14perCent(); // MUST be Called after the mPMT settings, otherwise unknown
		} else if(newValue == "Cylinder_60x74_3inchmPMT_40perCent" ) {
		  WCSimDetector->Cylinder_60x74_3inchmPMT_40perCent(); // MUST be Called after the mPMT settings, otherwise unknown
		} else if(newValue == "Cylinder_60x74_3inch_14perCent" ) {
		  WCSimDetector->Cylinder_60x74_3inch_14perCent(); // MUST be Called after the mPMT settings, otherwise unknown
		} else if(newValue == "Cylinder_60x74_3inch_40perCent" ) {
		  WCSimDetector->Cylinder_60x74_3inch_40perCent(); // MUST be Called after the mPMT settings, otherwise unknown
                } else if ( newValue == "nuPRISM") {
		  WCSimDetector->SetIsNuPrism(true);
		  WCSimDetector->SetDefaultNuPrismGeometry();
                } else if ( newValue == "nuPRISM_mPMT") {
		  WCSimDetector->SetIsNuPrism(true);
		  WCSimDetector->SetNuPrism_mPMTGeometry();
                } else if ( newValue == "nuPRISMBeamTest_mPMT") {
		WCSimDetector->SetIsNuPrismBeamTest(true); // Jun.04,2020 M.Shinoki
		WCSimDetector->SetIsNuPrism(true);
		WCSimDetector->SetNuPrismBeamTest_mPMTGeometry();
		} else if ( newValue == "nuPRISMBeamTest_16cShort_mPMT") {
		WCSimDetector->SetIsNuPrismBeamTest_16cShort(true); // Jul.01,2021 L.Anthony
                WCSimDetector->SetIsNuPrism(true);
                WCSimDetector->SetNuPrismBeamTest_16cShort_mPMTGeometry();
		} else if ( newValue == "nuPRISMShort_mPMT") {
		  WCSimDetector->SetIsNuPrism(true);
		  WCSimDetector->SetNuPrismShort_mPMTGeometry();
		} else if ( newValue == "IWCD_mPMT") {
		  WCSimDetector->SetIsNuPrism(true);
		  WCSimDetector->SetIWCDGeometry();
		} else if ( newValue == "IWCD_mPMT_WithOD") {
		  WCSimDetector->SetIsNuPrism(true);
		  WCSimDetector->SetIWCD_WithOD_Geometry();
      WCSimDetector->SetODEdited(false);
		} else if ( newValue == "IWCD_mPMT_WithOD_OptionA") {
		  WCSimDetector->SetIsNuPrism(true);
		  WCSimDetector->SetIWCD_WithOD_Geometry_OptionA();
      WCSimDetector->SetODEdited(false);
    } else if ( newValue == "IWCD_mPMT_WithOD_OptionC") {
		  WCSimDetector->SetIsNuPrism(true);
		  WCSimDetector->SetIWCD_WithOD_Geometry_OptionC();
      WCSimDetector->SetODEdited(false);
    } else if ( newValue == "IWCD_mPMT_WithOD_Old") {
		  WCSimDetector->SetIsNuPrism(true);
		  WCSimDetector->SetIWCD_WithOD_Geometry_Old();
      WCSimDetector->SetODEdited(false);
		} else {
		  G4cerr << "That geometry choice is not defined!" << G4endl;
		  exit(-1);
		}
		WCSimDetector->DumpDetectorConfigInfo();
	}

	if (command == SavePi0){
		G4cout << "Set the flag for saving pi0 info " << newValue << G4endl;
		if (newValue=="true"){
			WCSimDetector->SavePi0Info(true);
		}else if (newValue == "false"){
			WCSimDetector->SavePi0Info(false);
		}else{

		}
	}

	if (command == SaveCapture){
		G4cout << "Set the flag for saving neutron capture info " << newValue << G4endl;
		if (newValue=="true"){
			WCSimDetector->SaveCaptureInfo(true);
		}else if (newValue == "false"){
			WCSimDetector->SaveCaptureInfo(false);
		}else{

		}
	}

	if (command == PMTQEMethod){
	  G4cout << "Set PMT QE Method " << newValue << " ";
	  if (newValue == "Stacking_Only"){
	    WCSimDetector->SetPMT_QE_Method(1);
	    G4cout << "1";
	  }else if (newValue == "Stacking_And_SensitiveDetector"){
	    WCSimDetector->SetPMT_QE_Method(2);
	    G4cout << "2";
	  }else if (newValue == "SensitiveDetector_Only"){
	    WCSimDetector->SetPMT_QE_Method(3);
	    G4cout << "3";
	  }else if (newValue == "DoNotApplyQE"){
	    WCSimDetector->SetPMT_QE_Method(4);
	    G4cout << "4";
	  }else{
	    
	  }
	  G4cout << G4endl;
	}

	if (command == PMTCollEff){
	  G4cout << "Set PMT Collection Efficiency " << newValue << " ";
	  if (newValue == "on"){
	    WCSimDetector->SetPMT_Coll_Eff(1);
	    G4cout << "1";
	  }else if (newValue == "off"){
	    WCSimDetector->SetPMT_Coll_Eff(0);
	    G4cout << "0";
	  }
	  G4cout << G4endl;
	}
	
	if (command == WCVisChoice){
	  G4cout << "Set Vis Choice " << newValue << " ";
	  if (newValue == "OGLSX"){
	    WCSimDetector->SetVis_Choice("OGLSX");
	  }else if (newValue == "RayTracer"){
	    WCSimDetector->SetVis_Choice("RayTracer");
	  }
	  G4cout << G4endl;
	}

	if (command == waterTank_Length){
	  bool isEggShapedHyperK = WCSimDetector->GetIsEggShapedHyperK();
	  if(isEggShapedHyperK == true){
	    G4cout << "Set Partition Length in a cylinder " << newValue << " " << G4endl;
	    WCSimDetector->SetwaterTank_Length(waterTank_Length->GetNewDoubleValue(newValue));
	    WCSimDetector->SetWaterTubeLength(waterTank_Length->GetNewDoubleValue(newValue));
	    // WCSimDetector->SetIsEggShapedHyperK(true);
	  }
	  else {
	    G4cout << "Not egg-shaped HyperK Geometry. Detector length unchanged." << G4endl;
	  }
	}

	if(command == DopedWater) {
		G4cout << "Setting Gadolinium doping of water: " << newValue << G4endl;
		WCSimDetector->SetDopedWater(DopedWater->GetNewBoolValue(newValue));
	}


	if(command == DopingConcentration) {
		G4cout << "Setting Gadolinium doping concentration: " << newValue << "percent" << G4endl;
            WCSimDetector->AddDopedWater(DopingConcentration->GetNewDoubleValue(newValue));
	}
  
  if(command == BGOPlacement) {
    if (BGOPlacement->GetNewBoolValue(newValue)) {
      G4cout << "Placing BGO Scintillation Crystal" << G4endl;
      WCSimDetector->SetPlaceBGOGeometry(true);
    }
    else {
      G4cout << "Removing BGO Scintillation Crystal from Geometry" << G4endl; 
      WCSimDetector->SetPlaceBGOGeometry(false);
    }
  }

  if(command == BGOPosition) {
    G4ThreeVector BGOvec = BGOPosition->GetNew3VectorValue(newValue);
    WCSimDetector->SetPositionBGOGeometry(BGOvec.x(),BGOvec.y(),BGOvec.z());

  }

	if(command == PMTSize) {
		G4cout << "SET PMT SIZE" << G4endl;
		if ( newValue == "20inch"){
//			WCSimDetector->Set20inchPMTs();
		}else if (newValue == "10inch"){
//			WCSimDetector->Set10inchPMTs();
		}else
			G4cout << "That PMT size is not defined!" << G4endl;	
	}

	if(command == PMTSize2) {
	  G4cout << "SET PMT of TYPE 2 SIZE" << G4endl;
	  if ( newValue == "20inch"){
	    //			WCSimDetector->Set20inchPMTs();
	  }else if (newValue == "10inch"){
	    //			WCSimDetector->Set10inchPMTs();
	  }else
	    G4cout << "That PMT size is not defined!" << G4endl;	
	}
	if(command == SetPMTCoverage){
	  WCSimDetector->SetPMTCoverage(atof(newValue));
	}
	if(command == SetPMTCoverage2){
	  WCSimDetector->SetPMTCoverage2(atof(newValue));
	}

    /////////////////////////////////
    ////////////// OD ///////////////
    /////////////////////////////////

    if(command == PMTODRadius){
      WCSimDetector->SetODEdited(true);
      G4cout << "Set OD PMT size " << newValue << " ";
      if (newValue == "3inch"){
	//        WCSimDetector->SetWCPMTODSize("PMT3inch_ETEL9302B");
        WCSimDetector->SetWCPMTODSize("PMT3inchR14374");
      }else if (newValue == "5inch"){
        WCSimDetector->SetWCPMTODSize("PMT5inch");
      }else if (newValue == "8inch"){
        WCSimDetector->SetWCPMTODSize("PMT8inch");
      }else if (newValue == "BoxandLine20inchHQE"){
        WCSimDetector->SetWCPMTODSize("BoxandLine20inchHQE");
      }
      G4cout << G4endl;
    }

    if(command == ODLateralWaterDepth){
	WCSimDetector->SetODEdited(true);
      G4cout << "Set water depth on OD lateral side " << newValue << " " << G4endl;
      WCSimDetector->SetWCODLateralWaterDepth(ODLateralWaterDepth->GetNewDoubleValue(newValue));
    }

    if(command == ODHeightWaterDepth){
	WCSimDetector->SetODEdited(true);
      G4cout << "Set water depth on OD vertical side " << newValue << " " << G4endl;
      WCSimDetector->SetWCODHeightWaterDepth(ODHeightWaterDepth->GetNewDoubleValue(newValue));
    }

    if(command == ODDeadSpace){
	WCSimDetector->SetODEdited(true);
      G4cout << "Set dead space width between ID and OD " << newValue << " " << G4endl;
      WCSimDetector->SetWCODDeadSpace(ODDeadSpace->GetNewDoubleValue(newValue));
    }

    if(command == ODTyvekSheetThickness){
	WCSimDetector->SetODEdited(true);
      G4cout << "Set OD Tyvek thickness " << newValue << " " << G4endl;
      WCSimDetector->SetWCODTyvekSheetThickness(ODTyvekSheetThickness->GetNewDoubleValue(newValue));
    }

    if(command == ODWLSPlatesThickness){
	WCSimDetector->SetODEdited(true);
      G4cout << "Set OD WLS plates thickness " << newValue << " " << G4endl;
      WCSimDetector->SetWCODWLSPlatesThickness(ODWLSPlatesThickness->GetNewDoubleValue(newValue));
    }

    if(command == ODWLSPlatesLength){
	WCSimDetector->SetODEdited(true);
      G4cout << "Set OD WLS plates length " << newValue << " " << G4endl;
      WCSimDetector->SetWCODWLSPlatesLength(ODWLSPlatesLength->GetNewDoubleValue(newValue));
    }

    if(command == PMTODperCellHorizontal){
	WCSimDetector->SetODEdited(true);
      G4cout << "Set nb of OD PMTs per cell Horizontally " << newValue << " " << G4endl;
      WCSimDetector->SetWCPMTODperCellHorizontal(PMTODperCellHorizontal->GetNewIntValue(newValue));
    }

    if(command == PMTODperCellVertical){
	WCSimDetector->SetODEdited(true);
      G4cout << "Set nb of OD PMTs per cell Vertically " << newValue << " " << G4endl;
      WCSimDetector->SetWCPMTODperCellVertical(PMTODperCellVertical->GetNewIntValue(newValue));
    }

    if(command == PMTODPercentCoverage){
	WCSimDetector->SetODEdited(true);
      G4cout << "Set global photocoverage of the OD " << newValue << " " << G4endl;
      WCSimDetector->SetWCPMTODPercentCoverage(PMTODPercentCoverage->GetNewDoubleValue(newValue));
    }

    if(command == PMTODPercentCoverageTop){
	WCSimDetector->SetODEdited(true);
      G4cout << "Set Top photocoverage of the OD " << newValue << " " << G4endl;
      WCSimDetector->SetWCPMTODPercentCoverageTop(PMTODPercentCoverageTop->GetNewDoubleValue(newValue));
    }

    if(command == PMTODPercentCoverageBottom){
	WCSimDetector->SetODEdited(true);
      G4cout << "Set Bottom photocoverage of the OD " << newValue << " " << G4endl;
      WCSimDetector->SetWCPMTODPercentCoverageBottom(PMTODPercentCoverageBottom->GetNewDoubleValue(newValue));
    }

    if(command == PMTODPercentCoverageBarrel){
	WCSimDetector->SetODEdited(true);
      G4cout << "Set Barrel photocoverage of the OD " << newValue << " " << G4endl;
      WCSimDetector->SetWCPMTODPercentCoverageBarrel(PMTODPercentCoverageBarrel->GetNewDoubleValue(newValue));
    }

    if(command == ODPMTShift){
	WCSimDetector->SetODEdited(true);
      G4cout << "Set shift between OD PMTs rows " << newValue << " " << G4endl;
      WCSimDetector->SetWCODPMTShift(ODPMTShift->GetNewDoubleValue(newValue));
    }

    if(command == isWLSFilled) {
      WCSimDetector->SetODEdited(true);
      G4cout << "Deactivate WLS plates by filling them with water " << G4endl;
      WCSimDetector->SetIsWLSFilled(false);
    }

    if(command == BuildODWLSCladding) {
      WCSimDetector->SetODEdited(true);
      G4cout << "Add cladding around WLS plate " << G4endl;
      WCSimDetector->SetBuildODWLSCladding(true);
    }


    /////////// END OD //////////////
    /////////////////////////////////

    if( command == LCConfig ) {
      // LC Type is defined in WCSimDetectorConstruction.hh
      if ( newValue == "No_Mirror") {
	WCSimDetector->SetLCType(0);
      } else if ( newValue == "Mirror_OldLC" ){
	WCSimDetector->SetLCType(1);
      } else if ( newValue == "Mirror_2018Oct" ){
	WCSimDetector->SetLCType(2);
      }
    }

    if(command == WCConstruct) {
      //If the OD geometry has been changed, then reconstruct the whole tank with the proper recalculated dimensions
      if (WCSimDetector->GetODEdited() == true) {WCSimDetector->UpdateODGeo();}
      WCSimDetector->UpdateGeometry();
    }


	if (command == mPMT_CylHeight){
	  G4cout << "Set Vessel Cylinder Height of MultiPMT to " << newValue  << " " << G4endl; //doesn't work
	  //G4cout << "Set Cylinder Height of MultiPMT to " << newValue  << " " << G4endl;
	  WCSimDetector->SetmPMT_VesselCylHeight(mPMT_CylHeight->GetNewDoubleValue(newValue));
	}


	if (command == mPMT_CylRadiusCurv){
	  G4cout << "Set Vessel Radius of Curvature of MultiPMT to " << newValue  << " " << G4endl; //doesn't work
	  //G4cout << "Set Cylinder Radius of MultiPMT to " << newValue  << " " << G4endl;
	  WCSimDetector->SetmPMT_VesselRadiusCurv(mPMT_CylRadiusCurv->GetNewDoubleValue(newValue));
	}
	
	if (command == mPMT_CylRadius){
	  G4cout << "Set Vessel Radius of MultiPMT to " << newValue  << " " << G4endl; //doesn't work
	  //G4cout << "Set Cylinder Radius of MultiPMT to " << newValue  << " " << G4endl;
	  WCSimDetector->SetmPMT_VesselRadius(mPMT_CylRadius->GetNewDoubleValue(newValue));
	}

	
	if (command == mPMT_orientation){
	  if(newValue == "Horizontal")
	    WCSimDetector->SetmPMT_Orientation(HORIZONTAL);
	  else if(newValue == "Vertical")
	    WCSimDetector->SetmPMT_Orientation(VERTICAL);
	  else if(newValue == "Perpendicular")
	    WCSimDetector->SetmPMT_Orientation(PERPENDICULAR);
	  else
	    G4cout << "Undefined orientation wrt wall: reverting to default Perpendicular" << G4endl;
	  
	}
	if (command == mPMT_ID_reflector_height){
	  G4cout << "Set Height of reflector cone to " << newValue  << " " << G4endl;	
	  WCSimDetector->SetmPMT_ReflectorHeight(mPMT_ID_reflector_height->GetNewDoubleValue(newValue));
	}
	if (command == mPMT_ID_reflector_z_offset){
	  G4cout << "Set Z position offset of reflector cone to " << newValue  << " " << G4endl;	
	  WCSimDetector->SetmPMT_ReflectorZoffset(mPMT_ID_reflector_z_offset->GetNewDoubleValue(newValue));
	}
	if (command == mPMT_ID_reflector_angle){
	  G4cout << "Set Angle of reflector cone to " << newValue  << " " << G4endl;	
	  WCSimDetector->SetmPMT_ReflectorAngle(mPMT_ID_reflector_angle->GetNewDoubleValue(newValue));
	}
	if (command == mPMT_material_outer){
	  WCSimDetector->SetmPMT_MaterialOuter(newValue);
	}
	if (command == mPMT_material_inner){
	  WCSimDetector->SetmPMT_MaterialInner(newValue);
	}
	if(command == mPMT_material_outer_thickness){
	  WCSimDetector->SetmPMT_MaterialOuterThickness(mPMT_material_outer_thickness->GetNewDoubleValue(newValue));
	}
	if(command == mPMT_nID_PMTs){
	  WCSimDetector->SetmPMT_nID(mPMT_nID_PMTs->GetNewIntValue(newValue));
	}
	if(command == mPMT_config){
	  WCSimDetector->SetmPMT_Config(newValue);
	}
	
	if (command == mPMT_PMTtype_inner)
	  WCSimDetector->SetmPMT_PMTtype_inner(newValue);
	if (command == mPMT_PMTtype_outer)
	  WCSimDetector->SetmPMT_PMTtype_outer(newValue);
	
	
	if (command == mPMT_DistPMTVessel){
	  G4cout << "Set Distance of PMT(s) to pressure vessel to " << newValue  << " " << G4endl; //doesn't work
	  //G4cout << "Set Cylinder Radius of MultiPMT to " << newValue  << " " << G4endl;
	  WCSimDetector->SetmPMT_DistPMTVessel(mPMT_DistPMTVessel->GetNewDoubleValue(newValue));
	}

	if (command == mPMT_numModulesFixed){
	  G4cout << "Do you want fix the number of mPMT modules: " << newValue << G4endl;
	  WCSimDetector->SetmPMT_FixModules(mPMT_numModulesFixed->GetNewBoolValue(newValue));
	}

	if (command == mPMT_pmtOpeningAngle){
	  WCSimDetector->SetmPMT_OpeningAngle(mPMT_pmtOpeningAngle->GetNewDoubleValue(newValue));	  
	}

	if (command == mPMT_material_pmtAssembly){
	  WCSimDetector->SetmPMT_MaterialPMTassembly(newValue);
	}
	if (command == SetHybridPMT){
	  G4cout << "Set hybrid config = " << newValue << G4endl;
	  WCSimDetector->SetHybridPMT(SetHybridPMT->GetNewBoolValue(newValue));
	}
	
	// Customize nuPRISM tank setup
	if( WCSimDetector->GetIsNuPrism()){
	  if (command == SetPMTType) WCSimDetector->SetPMTType(newValue);
	  //else if (command == SetPMTCoverage) WCSimDetector->SetPMTCoverage(atof(newValue));
	  else if (command == SetDetectorHeight) WCSimDetector->SetDetectorHeight(SetDetectorHeight->GetNewDoubleValue(newValue));
	  else if (command == SetDetectorVerticalPosition) WCSimDetector->SetDetectorVerticalPosition(SetDetectorVerticalPosition->GetNewDoubleValue(newValue));
	  else if (command == SetDetectorDiameter) WCSimDetector->SetDetectorDiameter(SetDetectorDiameter->GetNewDoubleValue(newValue));
	  /* TF: deprecated. Don't remake geometry, just update variables in the setters.
	  else if (command == UpdateNuPrism){
            WCSimDetector->SetNuPrismGeometry(WCSimDetector->GetPMTType(),
					      WCSimDetector->GetPMTCoverage(),
					      WCSimDetector->GetWCIDHeight(),
					      WCSimDetector->GetWCIDDiameter(),
					      WCSimDetector->GetWCIDVerticalPosition());
					      } */
	}

  if (command == RotateBarrelHalfTower){
        G4cout << "Rotate barrel by half of one tower?  " << newValue << G4endl;
        WCSimDetector->SetRotateBarrelHalfTower(RotateBarrelHalfTower->GetNewBoolValue(newValue));
  }
  
	if (command == UseReplica){
	  G4cout << "Use replica method to place PMTs ?  " << newValue << G4endl;
	  WCSimDetector->SetUseReplica(UseReplica->GetNewBoolValue(newValue));
	}

	if (command == PMTPosVar) {
	  G4cout << "Apply fluctuations to PMT placement: sigma = " << newValue << G4endl;
	  WCSimDetector->SetPMTPosVar(PMTPosVar->GetNewDoubleValue(newValue));
	}

	if (command == TankRadiusChange) {
	  G4ThreeVector vec = TankRadiusChange->GetNew3VectorValue(newValue);
	  G4cout << "Set top, mid, bot radius change = " << vec.x() << ", " << vec.y() << ", " << vec.z() << G4endl;
	  WCSimDetector->SetRadiusChange(vec.x(),vec.y(),vec.z());
	}

	if(command == SetPMTPositionInput){
	  WCSimDetector->SetPMTPositionInput(newValue);
	}

  if(command == SetCDSFile){
	  WCSimDetector->SetCDSFile(newValue);
	}

}
