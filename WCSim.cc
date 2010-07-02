#include "G4ios.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4UIGAG.hh"
#include "G4VisExecutive.hh"
#include "G4TrajectoryDrawByParticleID.hh"
#include "WCSimDetectorConstruction.hh"
#include "WCSimPhysicsList.hh"
#include "WCSimPhysicsMessenger.hh"
#include "WCSimTuningParameters.hh"
#include "WCSimTuningMessenger.hh"
#include "WCSimPrimaryGeneratorAction.hh"
#include "WCSimEventAction.hh"
#include "WCSimRunAction.hh"
#include "WCSimStackingAction.hh"
#include "WCSimTrackingAction.hh"
#include "WCSimSteppingAction.hh"
#include "WCSimVisManager.hh"
#include <iostream>

int main(int argc,char** argv)
{
  // Construct the default run manager
  G4RunManager* runManager = new G4RunManager;

  // get the pointer to the UI manager
  G4UImanager* UI = G4UImanager::GetUIpointer();
  
  // Set up the tuning parameters that need to be read before the detector
  //  construction is done
  WCSimTuningParameters* tuningpars = new WCSimTuningParameters();

  // Get the tuning parameters
  UI->ApplyCommand("/control/execute tuning_parameters.mac");

  // UserInitialization classes (mandatory)
  enum DetConfiguration {wfm=1,fwm=2};
  G4int WCSimConfiguration = fwm;

  WCSimDetectorConstruction* WCSimdetector = new 
    WCSimDetectorConstruction(WCSimConfiguration,tuningpars);

  runManager->SetUserInitialization(WCSimdetector);

  runManager->SetUserInitialization(new WCSimPhysicsList);

  //=================================
  // Added by JLR 2005-07-05
  //=================================
  // Choice of hadronic interaction model for 
  // protons & neutrons. This file must be read in
  // by the program BEFORE the runManager is initialized.
  // If file does not exist, default model will be used.
  // Currently, default model is set to BINARY
  UI->ApplyCommand("/control/execute jobOptions.mac");

  // Visualization
  //G4VisManager* visManager = new WCSimVisManager;
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  //create new drawByParticleID model
  G4TrajectoryDrawByParticleID* mymodel = new G4TrajectoryDrawByParticleID;
  
  //Configure model
  mymodel->SetDefault("cyan");
  mymodel->Set("gamma","green");
  mymodel->Set("nu_e","yellow");
  mymodel->Set("nu_mu","yellow");
  mymodel->Set("anti_nu_e","yellow");
  mymodel->Set("anti_nu_mu","yellow");
  mymodel->Set("e-","blue");
  mymodel->Set("mu-","black");
  mymodel->Set("e+","red");
  mymodel->Set("mu+","white");
  mymodel->Set("proton","magenta");
  mymodel->Set("neutron","Grey");

  

  
  visManager->RegisterModel(mymodel);

  // Set user action classes
  WCSimPrimaryGeneratorAction* myGeneratorAction = new 
    WCSimPrimaryGeneratorAction(WCSimdetector);
  runManager->SetUserAction(myGeneratorAction);

  

  WCSimRunAction* myRunAction = new WCSimRunAction(WCSimdetector);
  runManager->SetUserAction(myRunAction);

  runManager->SetUserAction(new WCSimEventAction(myRunAction, WCSimdetector,
						  myGeneratorAction));
  runManager->SetUserAction(new WCSimTrackingAction);

  runManager->SetUserAction(new WCSimStackingAction(WCSimdetector));

  runManager->SetUserAction(new WCSimSteppingAction);

  // Initialize G4 kernel
  runManager->Initialize();

  if (argc==1)   // Define UI terminal for interactive mode  
  { 

    // Start UI Session
    G4UIsession* session =  new G4UIterminal(new G4UItcsh);
    
    // Visualization Macro
    UI->ApplyCommand("/control/execute vis.mac");

    // Start Interactive Mode
    session->SessionStart();
    
    delete session;
  }
  else           // Batch mode
  { 
    G4String command = "/control/execute ";
    G4String fileName = argv[1];

    UI->ApplyCommand(command+fileName);
  }

  delete visManager;

  delete runManager;
  return 0;
}


