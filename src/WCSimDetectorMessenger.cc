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
                          "HyperK_withHPD\n"
                          "nuPRISM\n"
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
                           "nuPRISM "
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

 // First, the PMT type
  SetPMTType = new G4UIcmdWithAString("/WCSim/nuPRISM/SetPMTType", this);
  SetPMTType->SetGuidance("Set the type of PMT to be used for nuPRISM");
  SetPMTType->SetGuidance("Available options are:\n"
          "PMT8inch\n"
          "PMT10inchHQE\n"
          "PMT10inch\n"
          "PMT12inchHQE\n"
          "HPD20inchHQE\n"
          "PMT20inch\n");
  SetPMTType->SetParameterName("PMTType", false);
  SetPMTType->SetCandidates("PMT8inch PMT10inchHQE PMT10inch PMT12inchHQE HPD20inchHQE PMT20inch");
  SetPMTType->SetDefaultValue("PMT10inch");

  // Next, the PMT coverage
  SetPMTCoverage = new G4UIcmdWithAString("/WCSim/nuPRISM/SetPMTPercentCoverage", this);
  SetPMTCoverage->SetGuidance("Set the PMT percentage coverage to be used for nuPRISM");
  SetPMTCoverage->SetGuidance("Any number is allowed (such as 40)");
  SetPMTCoverage->SetParameterName("PMTCoverage", false);
  SetPMTCoverage->SetDefaultValue("40");

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

  UpdateNuPrism = new G4UIcmdWithoutParameter("/WCSim/nuPRISM/Update", this);
  UpdateNuPrism->SetGuidance("Update nuPRISM detector construction with new settings.");
}

WCSimDetectorMessenger::~WCSimDetectorMessenger()
{
  delete PMTConfig;
  delete SavePi0;
  delete PMTQEMethod;
  delete PMTCollEff;
  delete waterTank_Length;
  delete WCVisChoice;

  delete SetDetectorDiameter;
  delete SetDetectorHeight;
  delete SetDetectorVerticalPosition;
  delete SetPMTCoverage;
  delete SetPMTType;

  delete tubeCmd;
  delete distortionCmd;
  delete WCSimDir;
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
        } else if ( newValue == "nuPRISM") {
          WCSimDetector->SetIsNuPrism(true);
          WCSimDetector->SetDefaultNuPrismGeometry();
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

    // Customize nuPRISM tank setup
    if( WCSimDetector->GetIsNuPrism()){
        if (command == SetPMTType) WCSimDetector->SetPMTType(newValue);
        else if (command == SetPMTCoverage) WCSimDetector->SetPMTCoverage(atof(newValue));
        else if (command == SetDetectorHeight) WCSimDetector->SetDetectorHeight(SetDetectorHeight->GetNewDoubleValue(newValue));
        else if (command == SetDetectorVerticalPosition) WCSimDetector->SetDetectorVerticalPosition(SetDetectorVerticalPosition->GetNewDoubleValue(newValue));
        else if (command == SetDetectorDiameter) WCSimDetector->SetDetectorDiameter(SetDetectorDiameter->GetNewDoubleValue(newValue));
        else if (command == UpdateNuPrism){
            WCSimDetector->SetNuPrismGeometry(WCSimDetector->GetPMTType(),
                    WCSimDetector->GetPMTCoverage(),
                    WCSimDetector->GetWCIDHeight(),
                    WCSimDetector->GetWCIDDiameter(),
                    WCSimDetector->GetWCIDVerticalPosition());
        }
    }

	if(command == WCConstruct) {
		WCSimDetector->UpdateGeometry();
	}

}
