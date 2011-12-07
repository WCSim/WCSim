#include "WCSimDetectorMessenger.hh"

#include "WCSimDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"


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
			 "DUSEL_200kton_12inch_HQE_12perCent\n"
			 "DUSEL_200kton_12inch_HQE_10perCent\n"
			 "DUSEL_200kton_12inch_HQE_13perCent\n"
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
			   "DUSEL_200kton_12inch_HQE_12perCent "
			   "DUSEL_200kton_12inch_HQE_10perCent "
			   "DUSEL_200kton_12inch_HQE_13perCent "
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

  WLSPSHAPE = new G4UIcmdWithAString("/WCSim/WLSPSHAPE", this);
  WLSPSHAPE->SetGuidance("circle, square and clover\n");
  WLSPSHAPE->SetParameterName("WLSPSHAPE",false);
  WLSPSHAPE->SetCandidates("circle "
			   "square "
			   "clover ");
  WLSPSHAPE->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  
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

  PMT_Timing_Var = new G4UIcmdWithAString("/WCSim/PMT_Timing_Var", this);
  PMT_Timing_Var->SetGuidance("Set the PMT configuration.");
  PMT_Timing_Var->SetGuidance("Available options are:\n"
			      "on\n"
			      "off\n");
  PMT_Timing_Var->SetParameterName("PMT_Timing_Var", false);
  PMT_Timing_Var->SetCandidates("on "
				"off ");
  PMT_Timing_Var->AvailableForStates(G4State_PreInit, G4State_Idle);

  
  PMTCollEff = new G4UIcmdWithAString("/WCSim/PMTCollEff", this);
  PMTCollEff->SetGuidance("Set the PMT configuration.");
  PMTCollEff->SetGuidance("Available options are:\n"
			  "on\n"
			  "off\n");
  PMTCollEff->SetParameterName("PMTCollEff", false);
  PMTCollEff->SetCandidates("on "
			    "off ");
  PMTCollEff->AvailableForStates(G4State_PreInit, G4State_Idle);

  
  PMTCollEff_Method = new G4UIcmdWithAString("/WCSim/PMTCollEff_Method", this);
  PMTCollEff_Method->SetGuidance("Set the PMT Collection Efficiency configuration.");
  PMTCollEff_Method->SetGuidance("Available options are:\n"
			  "1\n"
			  "2\n");
  PMTCollEff_Method->SetParameterName("PMTCollEff_Method", false);
  PMTCollEff_Method->SetCandidates("1 "
			    "2 ");
  PMTCollEff_Method->AvailableForStates(G4State_PreInit, G4State_Idle);

  
  LCoffset = new G4UIcmdWithADouble("/WCSim/LCoffset",this);
  LCoffset->SetGuidance("LCoffset");
  LCoffset->SetDefaultValue(1e9);
  LCoffset->AvailableForStates(G4State_Idle);

  LC_rmin = new G4UIcmdWithADouble("/WCSim/LC_rmin",this);
  LC_rmin->SetGuidance("LC_rmin");
  LC_rmin->SetDefaultValue(1e9);
  LC_rmin->AvailableForStates(G4State_Idle);

  LC_rmax = new G4UIcmdWithADouble("/WCSim/LC_rmax",this);
  LC_rmax->SetGuidance("LC_rmax");
  LC_rmax->SetDefaultValue(1e9);
  LC_rmax->AvailableForStates(G4State_Idle);

  LC_a = new G4UIcmdWithADouble("/WCSim/LC_a",this);
  LC_a->SetGuidance("LC_a");
  LC_a->SetDefaultValue(1e9);
  LC_a->AvailableForStates(G4State_Idle);

  LC_b = new G4UIcmdWithADouble("/WCSim/LC_b",this);
  LC_b->SetGuidance("LC_b");
  LC_b->SetDefaultValue(1e9);
  LC_b->AvailableForStates(G4State_Idle);

  LC_d = new G4UIcmdWithADouble("/WCSim/LC_d",this);
  LC_d->SetGuidance("LC_d");
  LC_d->SetDefaultValue(1e9);
  LC_d->AvailableForStates(G4State_Idle);

  WLSP_offset = new G4UIcmdWithADouble("/WCSim/WLSP_offset",this);
  WLSP_offset->SetGuidance("WLSP_offset");
  WLSP_offset->SetDefaultValue(1e9);
  WLSP_offset->AvailableForStates(G4State_Idle);
  
  WLSP_outradius = new G4UIcmdWithADouble("/WCSim/WLSP_outradius",this);
  WLSP_outradius->SetGuidance("WLSP_outradius");
  WLSP_outradius->SetDefaultValue(1e9);
  WLSP_outradius->AvailableForStates(G4State_Idle);

  LC_reflectivity = new G4UIcmdWithADouble("/WCSim/LC_reflectivity",this);
  LC_reflectivity->SetGuidance("LC_reflectivity");
  LC_reflectivity->SetDefaultValue(0.9);
  LC_reflectivity->AvailableForStates(G4State_Idle);

  WLSP_reflectivity = new G4UIcmdWithADouble("/WCSim/WLSP_reflectivity",this);
  WLSP_reflectivity->SetGuidance("WLSP_reflectivity");
  WLSP_reflectivity->SetDefaultValue(0.9);
  WLSP_reflectivity->AvailableForStates(G4State_Idle);

  LC_material = new G4UIcmdWithAString("/WCSim/LC_material", this);
  LC_material->SetGuidance("Set the PMT Collection Efficiency configuration.");
  LC_material->SetGuidance("Available options are:\n"
			  "1\n"
			  "2\n"
			  "3\n"
			  "4\n"
			  "5\n");
  LC_material->SetParameterName("LC_material", false);
  LC_material->SetCandidates("1 "
			     "2 "
			     "3 "
			     "4 "
			     "5 ");
  LC_material->AvailableForStates(G4State_PreInit, G4State_Idle);


  WLSP_material = new G4UIcmdWithAString("/WCSim/WLSP_material", this);
  WLSP_material->SetGuidance("Set the PMT Collection Efficiency configuration.");
  WLSP_material->SetGuidance("Available options are:\n"
			  "1\n"
			  "2\n"
			  "3\n"
			  "4\n"
			  "5\n");
  WLSP_material->SetParameterName("WLSP_material", false);
  WLSP_material->SetCandidates("1 "
			     "2 "
			     "3 "
			     "4 "
			     "5 ");
  WLSP_material->AvailableForStates(G4State_PreInit, G4State_Idle);

  WCConstruct = new G4UIcmdWithoutParameter("/WCSim/Construct", this);
  WCConstruct->SetGuidance("Update detector construction with new settings.");
}

WCSimDetectorMessenger::~WCSimDetectorMessenger()
{
  delete LCoffset;
  delete LC_rmin;
  delete LC_rmax;
  delete LC_a;
  delete LC_b;
  delete LC_d;
  
  delete LC_material;
  delete WLSP_material;
  delete LC_reflectivity;
  delete WLSP_reflectivity;

  delete WLSP_offset;
  delete WLSP_outradius;

  delete ConstructWLS;
  delete ConstructWLSP;
  delete ConstructLC;
  delete PMTSize;
  delete PMTConfig;
  delete WLSPSHAPE;
  delete SavePi0;
  delete PMTQEMethod;
  delete PMTCollEff;
  delete PMTCollEff_Method;
  delete PMT_Timing_Var;
  delete tubeCmd;
  delete distortionCmd;
  delete WCSimDir;
}

void WCSimDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{    

  if (command == LC_reflectivity){
    G4cout << "LC reflectivity " << LC_reflectivity->GetNewDoubleValue(newValue) << G4endl;
    WCSimDetector->Set_LC_reflectivity(LC_reflectivity->GetNewDoubleValue(newValue));
  }
  if (command == WLSP_reflectivity){
    G4cout << "WLSP reflectivity " << WLSP_reflectivity->GetNewDoubleValue(newValue) << G4endl;
    WCSimDetector->Set_WLSP_reflectivity(WLSP_reflectivity->GetNewDoubleValue(newValue));
  }
  
  if (command == LC_material){
    G4cout << "Set LC Material " << newValue << " ";
    if (newValue == "1"){
      WCSimDetector->SetLC_material(1);
      G4cout << "1";
    }else if (newValue == "2"){
      WCSimDetector->SetLC_material(2);
      G4cout << "2";
    }else if (newValue == "3"){
      WCSimDetector->SetLC_material(3);
      G4cout << "3";
    }else if (newValue == "4"){
      WCSimDetector->SetLC_material(4);
      G4cout << "4";
    }else if (newValue == "5"){
      WCSimDetector->SetLC_material(5);
      G4cout << "5";  
    }else{
      
    }
    G4cout << G4endl;
  }

  if (command == WLSP_material){
    G4cout << "Set WLSP Material " << newValue << " ";
    if (newValue == "1"){
      WCSimDetector->SetWLSP_material(1);
      G4cout << "1";
    }else if (newValue == "2"){
      WCSimDetector->SetWLSP_material(2);
      G4cout << "2";
    }else if (newValue == "3"){
      WCSimDetector->SetWLSP_material(3);
      G4cout << "3";
    }else if (newValue == "4"){
      WCSimDetector->SetWLSP_material(4);
      G4cout << "4";
    }else if (newValue == "5"){
      WCSimDetector->SetWLSP_material(5);
      G4cout << "5";  
    }else{
      
    }
    G4cout << G4endl;
  }


  if (command == LCoffset){
    G4cout << "LCoffset " <<LCoffset->GetNewDoubleValue(newValue) << G4endl;
    WCSimDetector->Set_LCoffset(LCoffset->GetNewDoubleValue(newValue));
  }
  if (command == LC_rmin){
    G4cout << "LC_rmin " <<LC_rmin->GetNewDoubleValue(newValue) << G4endl;
    WCSimDetector->Set_LC_rmin(LC_rmin->GetNewDoubleValue(newValue));
  }
  if (command == LC_rmax){
    G4cout << "LC_rmax " <<LC_rmax->GetNewDoubleValue(newValue) << G4endl;
    WCSimDetector->Set_LC_rmax(LC_rmax->GetNewDoubleValue(newValue));
  }
  if (command == LC_a){
    G4cout << "LC_a " <<LC_a->GetNewDoubleValue(newValue) << G4endl;
    WCSimDetector->Set_LC_a(LC_a->GetNewDoubleValue(newValue));
  }
  if (command == LC_b){
    G4cout << "LC_b " <<LC_b->GetNewDoubleValue(newValue) << G4endl;
    WCSimDetector->Set_LC_b(LC_b->GetNewDoubleValue(newValue));
  }
  if (command == LC_d){
    G4cout << "LC_d " <<LC_d->GetNewDoubleValue(newValue) << G4endl;
    WCSimDetector->Set_LC_d(LC_d->GetNewDoubleValue(newValue));
  }
  if (command == WLSP_offset){
    G4cout << "WLSP_offset " <<WLSP_offset->GetNewDoubleValue(newValue) << G4endl;
    WCSimDetector->Set_WLSP_offset(WLSP_offset->GetNewDoubleValue(newValue));
  }
  if (command == WLSP_outradius){
    G4cout << "WLSP_outradius " <<WLSP_outradius->GetNewDoubleValue(newValue) << G4endl;
    WCSimDetector->Set_WLSP_outradius(WLSP_outradius->GetNewDoubleValue(newValue));
  }

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
		} else if(newValue == "DUSEL_200kton_12inch_HQE_12perCent") {
		  WCSimDetector->DUSEL_200kton_12inch_HQE_12perCent();	
		} else if(newValue == "DUSEL_200kton_12inch_HQE_10perCent") {
		  WCSimDetector->DUSEL_200kton_12inch_HQE_10perCent();	
		} else if(newValue == "DUSEL_200kton_12inch_HQE_13perCent") {
		  WCSimDetector->DUSEL_200kton_12inch_HQE_13perCent();	
		}  else{
		  G4cout << "That geometry choice not defined!" << G4endl;
		}
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

	if (command == PMT_Timing_Var){
	  G4cout << "Set PMT Timing Variation " << newValue << " ";
	  if (newValue == "on"){
	    WCSimDetector->SetPMT_Timing_Var(1);
	    G4cout << "1";
	  }else if (newValue == "off"){
	    WCSimDetector->SetPMT_Timing_Var(0);
	    G4cout << "0";
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




	if (command == PMTCollEff_Method){
	  G4cout << "Set PMT Collection Efficiency Method " << newValue << " ";
	  if (newValue == "1"){
	    WCSimDetector->SetPMT_Coll_Eff_Method(1);
	    G4cout << "1";
	  }else if (newValue == "2"){
	    WCSimDetector->SetPMT_Coll_Eff_Method(2);
	    G4cout << "2";
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

	if (command == WLSPSHAPE){
	  G4cout << "SET WLSP SHAPE " << newValue << G4endl;
	  if (newValue == "circle"){
	    WCSimDetector->setWLSP_shape(0);
	  }else if (newValue == "square"){
	    WCSimDetector->setWLSP_shape(1);
	  }else if (newValue == "clover"){
	    WCSimDetector->setWLSP_shape(2);
	  }else{
	    WCSimDetector->setWLSP_shape(0);
	  }
	}

	if(command == ConstructWLS) {
	 
	  WCSimDetector->setWLS(ConstructWLS->GetNewBoolValue(newValue));
	}

	if(command == ConstructWLSP) {
	  
	  WCSimDetector->setWLSP(ConstructWLSP->GetNewBoolValue(newValue));
	  if (WCSimDetector->getWLSP() && WCSimDetector->getLC()){
	    G4cout << "You can not turn on both WLSP and LC. Turn off both WLSP and LC" << G4endl;
	    WCSimDetector->setWLSP(false);
	    WCSimDetector->setLC(false);
	    exit(0);
	  }
	}

	if(command == ConstructLC) {
	  WCSimDetector->setLC(ConstructLC->GetNewBoolValue(newValue));
	  if (WCSimDetector->getWLSP() && WCSimDetector->getLC()){
	    G4cout << "You can not turn on both WLSP and LC. Turn off both WLSP and LC" << G4endl;
	    WCSimDetector->setWLSP(false);
	    WCSimDetector->setLC(false);
	    exit(0);
	  }
	}

	if(command == WCConstruct) {
	  WCSimDetector->UpdateGeometry();
	}

}
