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
			  "Cylinder_12inchHPD_15perCent\n"
                          "HyperK\n"
                          "HyperK_withHPD\n"
                          "DUSEL_100kton_10inch_40perCent\n"
                          "DUSEL_100kton_10inch_HQE_12perCent\n"
                          "DUSEL_100kton_10inch_HQE_30perCent\n"
                          "DUSEL_100kton_10inch_HQE_30perCent_Gd\n"
                          "DUSEL_150kton_10inch_HQE_30perCent\n"
                          "DUSEL_200kton_10inch_HQE_12perCent\n"
                          "DUSEL_200kton_12inch_HQE_10perCent\n"
                          "DUSEL_200kton_12inch_HQE_14perCent\n"
                         );
  PMTConfig->SetParameterName("PMTConfig", false);
  PMTConfig->SetCandidates("SuperK "
			   "SuperK_20inchPMT_20perCent "
			   "SuperK_20inchBandL_20perCent "
			   "SuperK_12inchBandL_15perCent "
			   "SuperK_20inchBandL_14perCent "
			   "Cylinder_12inchHPD_15perCent "
			   "HyperK "
                           "HyperK_withHPD "
                           "DUSEL_100kton_10inch_40perCent "
                           "DUSEL_100kton_10inch_HQE_12perCent "
                           "DUSEL_100kton_10inch_HQE_30perCent "
                           "DUSEL_100kton_10inch_HQE_30perCent_Gd "
                           "DUSEL_150kton_10inch_HQE_30perCent "
                           "DUSEL_200kton_10inch_HQE_12perCent "
                           "DUSEL_200kton_12inch_HQE_10perCent "
                           "DUSEL_200kton_12inch_HQE_14perCent "
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
		} else if (newValue == "Cylinder_12inchHPD_15perCent" ){
		  WCSimDetector->Cylinder_12inchHPD_15perCent();
                } else if ( newValue == "HyperK") {
                        WCSimDetector->SetIsHyperK(true);
			WCSimDetector->SetHyperKGeometry();
                } else if ( newValue == "HyperK_withHPD") {
                        WCSimDetector->SetIsHyperK(true);
			WCSimDetector->SetHyperKGeometry_withHPD();
		} else if(newValue == "DUSEL_100kton_10inch_40perCent") {
			WCSimDetector->DUSEL_100kton_10inch_40perCent();
		} else if(newValue == "DUSEL_100kton_10inch_HQE_12perCent"){
		  WCSimDetector->DUSEL_100kton_10inch_HQE_12perCent();
		} else if(newValue == "DUSEL_100kton_10inch_HQE_30perCent") {
		  WCSimDetector->DUSEL_100kton_10inch_HQE_30perCent();
		} else if(newValue == "DUSEL_100kton_10inch_HQE_30perCent_Gd") {
		  WCSimDetector->DUSEL_100kton_10inch_HQE_30perCent_Gd();
		} else if(newValue == "DUSEL_150kton_10inch_HQE_30perCent") {
		  WCSimDetector->DUSEL_150kton_10inch_HQE_30perCent(); 
		} else if(newValue == "DUSEL_200kton_10inch_HQE_12perCent") {
		  WCSimDetector->DUSEL_200kton_10inch_HQE_12perCent();	
		} else if(newValue == "DUSEL_200kton_12inch_HQE_10perCent") {
		  WCSimDetector->DUSEL_200kton_12inch_HQE_10perCent();	
		} else if(newValue == "DUSEL_200kton_12inch_HQE_14perCent") {
		  WCSimDetector->DUSEL_200kton_12inch_HQE_14perCent();	
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

	if(command == WCConstruct) {
		WCSimDetector->UpdateGeometry();
	}

}
