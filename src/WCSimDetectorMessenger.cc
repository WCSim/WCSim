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
                          "DUSEL_100kton_10inch_40perCent\n"
                          "DUSEL_100kton_10inch_HQE_12perCent\n"
                          "DUSEL_100kton_10inch_HQE_30perCent\n"
                          "DUSEL_100kton_10inch_HQE_30perCent_Gd\n"
                          "DUSEL_150kton_10inch_HQE_30perCent\n"
						  "DUSEL_200kton_10inch_HQE_12perCent\n"
                          "150kTMailbox_10inch_HQE_30perCent\n"
						  "150kTMailbox_10inch_40perCent");
  PMTConfig->SetParameterName("PMTConfig", false);
  PMTConfig->SetCandidates("SuperK "
                          "DUSEL_100kton_10inch_40perCent "
                          "DUSEL_100kton_10inch_HQE_12perCent "
                          "DUSEL_100kton_10inch_HQE_30perCent "
                          "DUSEL_100kton_10inch_HQE_30perCent_Gd "
                          "DUSEL_150kton_10inch_HQE_30perCent "
						  "DUSEL_200kton_10inch_HQE_12perCent "
                          "150kTMailbox_10inch_HQE_30perCent "
						  "150kTMailbox_10inch_40perCent ");
  PMTConfig->AvailableForStates(G4State_PreInit, G4State_Idle);

  PMTSize = new G4UIcmdWithAString("/WCSim/WCPMTsize",this);
  PMTSize->SetGuidance("Set alternate PMT size for the WC (Must be entered after geometry details is set).");
  PMTSize->SetGuidance("Available options 20inch 10inch");
  PMTSize->SetParameterName("PMTSize", false);
  PMTSize->SetCandidates("20inch 10inch");
  PMTSize->AvailableForStates(G4State_PreInit, G4State_Idle);
  

  ConstructWLS = new G4UIcmdWithABool("/WCSim/ConstructWLS",this);
  ConstructWLS->SetGuidance("Add WLS true or false");
  ConstructWLS->SetParameterName("ConstructWLS",false);
  //ConstructWLS->SetCandidates("true false");
  //ConstructWLS->AvailableForStates(G4State_PreInit, G4State_Idle);
 
  ConstructWLSP = new G4UIcmdWithABool("/WCSim/ConstructWLSP",this);
  ConstructWLSP->SetGuidance("Add WLS Plate true or false");
  ConstructWLSP->SetParameterName("ConstructWLSP",false);
 
  ConstructLC = new G4UIcmdWithABool("/WCSim/ConstructLC", this);
  ConstructLC->SetGuidance("Construct Light Collectors.");
  ConstructLC->SetParameterName("ContructLC",false);

  SavePi0 = new G4UIcmdWithAString("/WCSim/SavePi0", this);
  SavePi0->SetGuidance("true or false\n");
  SavePi0->SetParameterName("SavePi0",false);
  SavePi0->SetCandidates("true " 
			 "false ");
  SavePi0->AvailableForStates(G4State_PreInit, G4State_Idle);

  WLSPSIZE = new G4UIcmdWithAString("/WCSim/WLSPSIZE", this);
  WLSPSIZE->SetGuidance("0 or 1\n");
  WLSPSIZE->SetParameterName("WLSPSIZE",false);
  WLSPSIZE->SetCandidates("0 "
			  "1 ");
  WLSPSIZE->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  
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

  WCConstruct = new G4UIcmdWithoutParameter("/WCSim/Construct", this);
  WCConstruct->SetGuidance("Update detector construction with new settings.");
}

WCSimDetectorMessenger::~WCSimDetectorMessenger()
{
  delete ConstructWLS;
  delete ConstructWLSP;
  delete ConstructLC;
  delete PMTSize;
  delete PMTConfig;
  delete WLSPSIZE;
  delete SavePi0;
  delete PMTQEMethod;
  delete tubeCmd;
  delete distortionCmd;
  delete WCSimDir;
}

void WCSimDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{    
	if( command == PMTConfig ) { 
		WCSimDetector->SetIsMailbox(false);
		WCSimDetector->SetIsUpright(false);
		if ( newValue == "150kTMailbox_10inch_HQE_30perCent") {
			WCSimDetector->SetIsMailbox(true);
			WCSimDetector->SetMailBox150kTGeometry_10inch_HQE_30perCent();//aah
		} else if ( newValue == "150kTMailbox_10inch_40perCent") {
			WCSimDetector->SetIsMailbox(true);
			WCSimDetector->SetMailBox150kTGeometry_10inch_40perCent();//aah
		} else if(newValue == "SuperK") {
			WCSimDetector->SetSuperKGeometry();
		} else if(newValue == "DUSEL_100kton_10inch_40perCent") {
			WCSimDetector->DUSEL_100kton_10inch_40perCent();
		}else if(newValue == "DUSEL_100kton_10inch_HQE_12perCent"){
		  WCSimDetector->DUSEL_100kton_10inch_HQE_12perCent();
		} else if(newValue == "DUSEL_100kton_10inch_HQE_30perCent") {
		  WCSimDetector->DUSEL_100kton_10inch_HQE_30perCent();
		} else if(newValue == "DUSEL_100kton_10inch_HQE_30perCent_Gd") {
		  WCSimDetector->DUSEL_100kton_10inch_HQE_30perCent_Gd();
		} else if(newValue == "DUSEL_150kton_10inch_HQE_30perCent") {
		  WCSimDetector->DUSEL_150kton_10inch_HQE_30perCent(); 
		} else if(newValue == "DUSEL_200kton_10inch_HQE_12perCent") {
		  WCSimDetector->DUSEL_200kton_10inch_HQE_12perCent();	
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


	if(command == PMTSize) {
		G4cout << "SET PMT SIZE" << G4endl;
		if ( newValue == "20inch"){
//			WCSimDetector->Set20inchPMTs();
		}else if (newValue == "10inch"){
//			WCSimDetector->Set10inchPMTs();
		}else
			G4cout << "That PMT size is not defined!" << G4endl;	
	}	

	if (command == WLSPSIZE){
	  G4cout << "SET WLSP SIZE" << G4endl;
	  if (newValue == "0"){
	    WCSimDetector->setWLSP_size(0);
	  }else if (newValue == "1"){
	    WCSimDetector->setWLSP_size(1);
	  }else{
	    WCSimDetector->setWLSP_size(1);
	  }
	}

	if(command == ConstructWLS) {
	 
	  WCSimDetector->setWLS(ConstructWLS->GetNewBoolValue(newValue));
	}

	if(command == ConstructWLSP) {
	  
	  WCSimDetector->setWLSP(ConstructWLSP->GetNewBoolValue(newValue));
	}

	if(command == ConstructLC) {
	  
	  WCSimDetector->setLC(ConstructLC->GetNewBoolValue(newValue));
	}

	if(command == WCConstruct) {
	  WCSimDetector->UpdateGeometry();
	}

}
