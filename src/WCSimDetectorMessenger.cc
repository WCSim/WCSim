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
                          "HyperK_withHPD\n"
			  "Cylinder_60x74_3inchmPMT_14perCent\n"
			  "TestmPMT\n"
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
                           "HyperK_withHPD "
			   "Cylinder_60x74_3inchmPMT_14perCent "
			   "TestmPMT "
                           );
  PMTConfig->AvailableForStates(G4State_PreInit, G4State_Idle);

  PMTSize = new G4UIcmdWithAString("/WCSim/WCPMTsize",this);
  PMTSize->SetGuidance("Set alternate PMT size for the WC (Must be entered after geometry details is set).");
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
			     "SensitiveDetector_Only\n");
  PMTQEMethod->SetParameterName("PMTQEMethod", false);
  PMTQEMethod->SetCandidates("Stacking_Only "
			     "Stacking_And_SensitiveDetector "
			     "SensitiveDetector_Only ");
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


  waterTank_Length = new G4UIcmdWithADoubleAndUnit("/WCSim/HyperK/waterTank_Length", this);
  waterTank_Length->SetGuidance("Set the Length of Hyper-K detector (unit: mm cm m).");
  waterTank_Length->SetParameterName("waterTank_length", true);
  waterTank_Length->SetDefaultValue(49500.);
  waterTank_Length->SetUnitCategory("Length");
  waterTank_Length->SetDefaultUnit("mm");
  waterTank_Length->SetUnitCandidates("mm cm m");
  WCConstruct = new G4UIcmdWithoutParameter("/WCSim/Construct", this);
  WCConstruct->SetGuidance("Update detector construction with new settings.");


  // Params: - Cylinder height and radius of mPMT: DONE
  //         - Type of ID PMT (should be enum) and Type of OD PMT: Need to reorganize the PMTpart.
  //         - Params related to filling PMT: TODO and test 1 PMT case
  //         - Orientation of mPMT wrt wall : horizontal/vertical/perp
  //         - Reflector OFF/Winston/other
  //         - material of mPMT wall: water/glass/acrylic/
  //         - acrylic cover ON/OFF


  mPMTDir = new G4UIdirectory("/WCSim/mPMT/");
  mPMTDir->SetGuidance("Commands to change the properties of the multiPMT.");


  mPMT_CylHeight = new G4UIcmdWithADoubleAndUnit("/WCSim/mPMT/CylHeight",this);
  mPMT_CylHeight->SetGuidance("Set height of cylinder of multiPMT object.");
  mPMT_CylHeight->SetParameterName("CylHeight", true);
  mPMT_CylHeight->SetDefaultValue(50.); //mDOM (PINGU), 0 for KM3Net
  mPMT_CylHeight->SetUnitCategory("Length");
  mPMT_CylHeight->SetDefaultUnit("mm");
  mPMT_CylHeight->SetUnitCandidates("mm cm m");

  mPMT_CylRadius = new G4UIcmdWithADoubleAndUnit("/WCSim/mPMT/CylRadius",this);
  mPMT_CylRadius->SetGuidance("Set radius of cylinder of multiPMT object.");
  mPMT_CylRadius->SetParameterName("CylRadius", true);
  mPMT_CylRadius->SetDefaultValue(325.); 
  mPMT_CylRadius->SetUnitCategory("Length");
  mPMT_CylRadius->SetDefaultUnit("mm");
  mPMT_CylRadius->SetUnitCandidates("mm cm m");

  mPMT_orientation = new G4UIcmdWithAString("/WCSim/mPMT/orientation",this);
  mPMT_orientation->SetGuidance("Set orientation of multiPMT cylinder wrt wall.");
  mPMT_orientation->SetParameterName("orientation", true);
  mPMT_orientation->SetCandidates("Horizontal "
				  "Vertical " 
				  "Perpendicular "
				  ); 

  // untested/unfinished
  // ? Make this part of DetectorConfigs as is currently the case?
  // Let DetectorConfigs grab from here?
  /*
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
				    ); 
  */

  mPMT_material_outer = new G4UIcmdWithAString("/WCSim/mPMT/material_outer",this);
  mPMT_material_outer->SetGuidance("Set material of outer capsule of multiPMT.");
  mPMT_material_outer->SetParameterName("material_outer", true);
  mPMT_material_outer->SetCandidates("Water "
				     "Glass "
				     "Acrylic "); 

  mPMT_material_inner = new G4UIcmdWithAString("/WCSim/mPMT/material_inner",this);
  mPMT_material_inner->SetGuidance("Set material of inner structure next to PMT expose.");
  mPMT_material_inner->SetParameterName("material_inner", true);
  mPMT_material_inner->SetCandidates("Water "
				     "BlackSheet");    //ToDo: add some better absorbing variant of blackSheet

  //NEW: Thickness of outer and inner material
  mPMT_material_outer_thickness = new G4UIcmdWithADoubleAndUnit("/WCSim/mPMT/material_outer_thickness",this);
  mPMT_material_outer_thickness->SetGuidance("Set thickness of outer (pressure) vessel of multiPMT.");
  mPMT_material_outer_thickness->SetParameterName("material_outer_thickness", true);
  mPMT_material_outer_thickness->SetDefaultValue(20.); 
  mPMT_material_outer_thickness->SetUnitCategory("Length");
  mPMT_material_outer_thickness->SetDefaultUnit("mm");
  mPMT_material_outer_thickness->SetUnitCandidates("mm cm m");


  mPMT_material_inner_thickness = new G4UIcmdWithADoubleAndUnit("/WCSim/mPMT/material_inner_thickness",this);
  mPMT_material_inner_thickness->SetGuidance("Set material of inner (support) structure next to PMT expose.");
  mPMT_material_inner_thickness->SetParameterName("material_inner_thickness", true);
  mPMT_material_inner_thickness->SetDefaultValue(2.); 
  mPMT_material_inner_thickness->SetUnitCategory("Length");
  mPMT_material_inner_thickness->SetDefaultUnit("mm");
  mPMT_material_inner_thickness->SetUnitCandidates("mm cm m");

  // NEW: 
  mPMT_ID_reflector_height = new G4UIcmdWithADoubleAndUnit("/WCSim/mPMT/reflectorHeightID",this);
  mPMT_ID_reflector_height->SetGuidance("Set height of reflector cone for each ID PMT.");
  mPMT_ID_reflector_height->SetParameterName("reflectorHeightID", true);
  mPMT_ID_reflector_height->SetDefaultValue(7.5); 
  mPMT_ID_reflector_height->SetUnitCategory("Length");
  mPMT_ID_reflector_height->SetDefaultUnit("mm");
  mPMT_ID_reflector_height->SetUnitCandidates("mm cm m");


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
  mPMT_nID_PMTs->SetGuidance("Set angle of reflector cone wrt PMT axis for each ID PMT.");
  mPMT_nID_PMTs->SetParameterName("reflectorAngleID", true);
  mPMT_nID_PMTs->SetDefaultValue(20.); 
  
  //
  mPMT_config = new G4UIcmdWithAString("/WCSim/mPMT/configfile",this);
  mPMT_config->SetGuidance("Set filename for config file with viewing angles and tilt angles.");
  mPMT_config->SetParameterName("mPMTconfig", true);

}

WCSimDetectorMessenger::~WCSimDetectorMessenger()
{
  delete PMTConfig;
  delete SavePi0;
  delete PMTQEMethod;
  delete PMTCollEff;
  delete waterTank_Length;

  delete tubeCmd;
  delete distortionCmd;

  delete mPMT_CylHeight;
  delete mPMT_CylRadius;
  delete WCSimDir;
  delete mPMTDir;
}

void WCSimDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{    
	if( command == PMTConfig ) { 
		WCSimDetector->SetIsUpright(false);
                WCSimDetector->SetIsHyperK(false);
                if ( newValue == "SuperK") {
		  WCSimDetector->SetSuperKGeometry();
		} else if (newValue == "SuperK_20inchPMT_20perCent" ){
		  WCSimDetector->SuperK_20inchPMT_20perCent();
		} else if (newValue == "SuperK_20inchBandL_20perCent" ){
		  WCSimDetector->SuperK_20inchBandL_20perCent();
		} else if ( newValue == "SuperK_12inchBandL_15perCent" ) {
		  WCSimDetector->SuperK_12inchBandL_15perCent();
		} else if ( newValue == "SuperK_20inchBandL_14perCent" ) {
		  WCSimDetector->SuperK_20inchBandL_14perCent();
		} else if ( newValue == "Cylinder_60x74_20inchBandL_14perCent" ) {
		  WCSimDetector->Cylinder_60x74_20inchBandL_14perCent();
		} else if ( newValue == "Cylinder_60x74_20inchBandL_40perCent" ) {
		  WCSimDetector->Cylinder_60x74_20inchBandL_40perCent();
		} else if (newValue == "Cylinder_12inchHPD_15perCent" ){
		  WCSimDetector->Cylinder_12inchHPD_15perCent();
                } else if ( newValue == "HyperK") {
                        WCSimDetector->SetIsHyperK(true);
			WCSimDetector->SetHyperKGeometry();
                } else if ( newValue == "HyperK_withHPD") {
                        WCSimDetector->SetIsHyperK(true);
			WCSimDetector->SetHyperKGeometry_withHPD();
		} else if(newValue == "TestmPMT") {
		  WCSimDetector->SetTestmPMTGeometry();	
		} else if(newValue == "Cylinder_60x74_3inchmPMT_14perCent" ) {
		  WCSimDetector->Cylinder_60x74_3inchmPMT_14perCent(); // MUST be Called after the mPMT settings, otherwise unknown
		} else
		  G4cout << "That geometry choice not defined!" << G4endl;
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
	
	if (command == waterTank_Length){
	bool isHyperK = WCSimDetector->GetIsHyperK();
	  if(isHyperK==true){
	    G4cout << "Set Partition Length in a cylinder " << newValue << " " << G4endl;
	    WCSimDetector->SetwaterTank_Length(waterTank_Length->GetNewDoubleValue(newValue));
	    WCSimDetector->SetWaterTubeLength(waterTank_Length->GetNewDoubleValue(newValue));
	    //	    WCSimDetector->SetIsHyperK(true);
	  }
	  else {
	    G4cout << "Not HyperK Geometry. Detector length unchanged." << G4endl;
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

	if (command == mPMT_CylHeight){
	  G4cout << "Set Cylinder Height of MultiPMT to " << newValue  << " " << G4endl; //doesn't work
	  //std::cout << "Set Cylinder Height of MultiPMT to " << newValue  << " " << std::endl;
	  WCSimDetector->SetmPMT_CylHeight(mPMT_CylHeight->GetNewDoubleValue(newValue));
	}


	if (command == mPMT_CylRadius){
	  G4cout << "Set Cylinder Radius of MultiPMT to " << newValue  << " " << G4endl; //doesn't work
	  //std::cout << "Set Cylinder Radius of MultiPMT to " << newValue  << " " << std::endl;
	  WCSimDetector->SetmPMT_CylRadius(mPMT_CylRadius->GetNewDoubleValue(newValue));
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
	if(command == mPMT_material_inner_thickness){
	  WCSimDetector->SetmPMT_MaterialInnerThickness(mPMT_material_inner_thickness->GetNewDoubleValue(newValue));
	}
	if(command == mPMT_nID_PMTs){
	  WCSimDetector->SetmPMT_nID(mPMT_nID_PMTs->GetNewIntValue(newValue));
	}
	if(command == mPMT_config){
	  WCSimDetector->SetmPMT_Config(newValue);
	}
	/*
	if (command == mPMT_PMTtype_inner)
	  std::cout << "Need to implement PMT enums and restructure WCSim to use PMT type ID" << std::endl;
	if (command == mPMT_PMTtype_outer)
	  std::cout << "Need to implement PMT enums and restructure WCSim to use PMT type OD" << std::endl;
	*/

	if(command == WCConstruct) {
	  WCSimDetector->UpdateGeometry();
	}
}
