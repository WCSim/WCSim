#include "WCSimDetectorMessenger.hh"

#include "WCSimDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

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
			  "HyperK_20perCent\n"
			  "HyperKWithOD\n"
			  "EggShapedHyperK\n"
			  "EggShapedHyperK_withHPD\n"
			  );
  PMTConfig->SetParameterName("PMTConfig", false);
  PMTConfig->SetCandidates("SuperK "
			  "SuperK_20inchPMT_20perCent "
			  "SuperK_20inchBandL_20perCent "
			  "SuperK_12inchBandL_15perCent "
			  "SuperK_20inchBandL_14perCent "
			  "Cylinder_60x74_20inchBandL_14perCent "
			  "Cylinder_60x74_20inchBandL_40perCent "
			  "Cylinder_12inchHPD_15perCent "
			  "HyperK "
			  "HyperK_20perCent "
			  "HyperKWithOD "
			  "EggShapedHyperK "
			  "EggShapedHyperK_withHPD "
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


  PMTSize = new G4UIcmdWithAString("/WCSim/WCPMTsize",this);
  PMTSize->SetGuidance("Set alternate PMT size for the WC (Must be entered after geometry details are set).");
  PMTSize->SetGuidance("Available options 20inch 10inch");
  PMTSize->SetParameterName("PMTSize", false);
  PMTSize->SetCandidates("20inch 10inch");
  PMTSize->AvailableForStates(G4State_PreInit, G4State_Idle);

  SavePi0 = new G4UIcmdWithAString("/WCSim/SavePi0", this);
  SavePi0->SetGuidance("true or false");
  SavePi0->SetParameterName("SavePi0",false);
  SavePi0->SetCandidates("true false");
  SavePi0->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  
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
			     "DoNotApplyQE");
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
						  "8inch\n");
  PMTODRadius->SetParameterName("PMTODRadius", false);
  PMTODRadius->SetCandidates("3inch "
							 "5inch "
                             "8inch ");
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

  //
  ODFPConfigFile = new G4UIcmdWithAString("/WCSim/HyperKOD/ODFPConfigFile", this);
  ODFPConfigFile->SetGuidance("OD config file for free PMT+WLS unit placement");
  ODFPConfigFile->SetParameterName("ODFPConfigFile", true);
  ODFPConfigFile->SetDefaultValue("");

  // Top Cap OD PMTs cvg
  ODTopCapCvg = new G4UIcmdWithADouble("/WCSim/HyperKOD/ODTopCapCvg", this);
  ODTopCapCvg->SetGuidance("Set TOP Cap OD photocoverage percentage.");
  ODTopCapCvg->SetParameterName("ODTopCapCvg", true);
  ODTopCapCvg->SetDefaultValue(0.);
  ODTopCapCvg->SetRange("ODTopCapCvg>0");

  // Bottom Cap OD PMTs cvg
  ODBottomCapCvg = new G4UIcmdWithADouble("/WCSim/HyperKOD/ODBottomCapCvg", this);
  ODBottomCapCvg->SetGuidance("Set BOTTOM Cap OD photocoverage percentage.");
  ODBottomCapCvg->SetParameterName("ODBottomCapCvg", true);
  ODBottomCapCvg->SetDefaultValue(0.);
  ODBottomCapCvg->SetRange("ODBottomCapCvg>0");

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
}

WCSimDetectorMessenger::~WCSimDetectorMessenger()
{
  delete PMTConfig;
  delete LCConfig;
  delete SavePi0;
  delete PMTQEMethod;
  delete PMTCollEff;
  delete waterTank_Length;
  delete WCVisChoice;

  delete tubeCmd;
  delete distortionCmd;
  delete WCSimDir;
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
		} else if ( newValue == "HyperK" ){
		  WCSimDetector->SetHyperKGeometry();
		} else if ( newValue == "HyperK_20perCent" ){
		  WCSimDetector->SetHyperKGeometry_20perCent();
		} else if ( newValue == "HyperKWithOD" ){
		  WCSimDetector->SetHyperKWithODGeometry();
		  WCSimDetector->SetODEdited(false);
		} else if ( newValue == "EggShapedHyperK") {
		  WCSimDetector->SetIsEggShapedHyperK(true);
		  WCSimDetector->SetEggShapedHyperKGeometry();
		} else if ( newValue == "EggShapedHyperK_withHPD") {
		  WCSimDetector->SetIsEggShapedHyperK(true);
		  WCSimDetector->SetEggShapedHyperKGeometry_withHPD();
		} else
		  G4cout << "That geometry choice is not defined!" << G4endl;
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


	if(command == PMTSize) {
		G4cout << "SET PMT SIZE" << G4endl;
		if ( newValue == "20inch"){
//			WCSimDetector->Set20inchPMTs();
		}else if (newValue == "10inch"){
//			WCSimDetector->Set10inchPMTs();
		}else
			G4cout << "That PMT size is not defined!" << G4endl;	
	}


    /////////////////////////////////
    ////////////// OD ///////////////
    /////////////////////////////////

    if(command == PMTODRadius){
      WCSimDetector->SetODEdited(true);
      G4cout << "Set OD PMT size " << newValue << " ";
      if (newValue == "3inch"){
        WCSimDetector->SetWCPMTODSize("PMT3inch");
      }else if (newValue == "5inch"){
        WCSimDetector->SetWCPMTODSize("PMT5inch");
      }else if (newValue == "8inch"){
        WCSimDetector->SetWCPMTODSize("PMT8inch");
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
      WCSimDetector->SetWCPMTODperCellHorizontal((G4double)PMTODperCellHorizontal->GetNewIntValue(newValue));
    }

    if(command == PMTODperCellVertical){
	WCSimDetector->SetODEdited(true);
      G4cout << "Set nb of OD PMTs per cell Vertically " << newValue << " " << G4endl;
      WCSimDetector->SetWCPMTODperCellVertical((G4double)PMTODperCellVertical->GetNewIntValue(newValue));
    }

    if(command == PMTODPercentCoverage){
	WCSimDetector->SetODEdited(true);
      G4cout << "Set global photocoverage of the OD " << newValue << " " << G4endl;
      WCSimDetector->SetWCPMTODPercentCoverage(PMTODPercentCoverage->GetNewDoubleValue(newValue));
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

  if(command == ODFPConfigFile){
	G4cout << "Set path to OD config file for free PMT+WLS unit placement " << newValue << " ";
	WCSimDetector->SetODEdited(true);
	WCSimDetector->SetODFPConfigFile(newValue);
  }

  if(command == ODTopCapCvg){
	WCSimDetector->SetODEdited(true);
	G4cout << "Set global photocoverage of the OD " << newValue << " " << G4endl;
	WCSimDetector->SetWCPMTODTopCapCvg(ODTopCapCvg->GetNewDoubleValue(newValue));
  }

  if(command == ODBottomCapCvg){
	WCSimDetector->SetODEdited(true);
	G4cout << "Set global photocoverage of the OD " << newValue << " " << G4endl;
	WCSimDetector->SetWCPMTODBottomCapCvg(ODBottomCapCvg->GetNewDoubleValue(newValue));
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

}
