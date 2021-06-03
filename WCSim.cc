#include "G4ios.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "WCSimDetectorConstruction.hh"
#include "WCSimPhysicsListFactory.hh"
#include "WCSimPhysicsListFactoryMessenger.hh"
#include "WCSimTuningParameters.hh"
#include "WCSimTuningMessenger.hh"
#include "WCSimPrimaryGeneratorAction.hh"
#include "WCSimEventAction.hh"
#include "WCSimRunAction.hh"
#include "WCSimStackingAction.hh"
#include "WCSimTrackingAction.hh"
#include "WCSimSteppingAction.hh"
#include "WCSimVisManager.hh"
#include "WCSimRandomParameters.hh"


#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

void file_exists(const char * filename) {
  bool exists = access(filename, F_OK) != -1;
  if(!exists) {
    G4cerr << filename << " not found or inaccessible. Exiting" << G4endl;
    exit(-1);
  }
}


int main(int argc,char** argv)
{
  // Construct the default run manager
  G4RunManager* runManager = new G4RunManager;

  // get the pointer to the UI manager
  G4UImanager* UI = G4UImanager::GetUIpointer();

  // Set up the tuning parameters that need to be read before the detector
  //  construction is done
  WCSimTuningParameters* tuningpars = new WCSimTuningParameters();

  if(argc != 3){
      G4cout << "WCSim requires 2 mac-files:" << G4endl;
      G4cout << "\tUsage: WCSim {input.mac} {tuning.mac}" << G4endl;
      delete runManager;
      return -1;
  }
  // Get the tuning parameters
  //hack B.Q
  G4cout << "B.Q: Read argv[2], which should contains the tuning parameters" << G4endl;
  G4String fileName2 = argv[2];
  file_exists(fileName2);
  if(fileName2 == "vis.mac"){
    G4cout << "ERROR: Execute without arg for interactive mode" << G4endl;
    //return -1;
  }
  G4String command2 = "/control/execute ";
  UI->ApplyCommand(command2+fileName2);
  //file_exists("macros/tuning_parameters.mac");
  //UI->ApplyCommand("/control/execute macros/tuning_parameters.mac");

  // define random number generator parameters
  WCSimRandomParameters *randomparameters = new WCSimRandomParameters();

  // UserInitialization classes (mandatory)
  enum DetConfiguration {wfm=1,fwm=2};
  G4int WCSimConfiguration = fwm;

  WCSimDetectorConstruction* WCSimdetector = new 
    WCSimDetectorConstruction(WCSimConfiguration,tuningpars);

  runManager->SetUserInitialization(WCSimdetector);

  // Added selectable physics lists 2010-07 by DMW
  // Set up the messenger hooks here, initialize the actual list after loading jobOptions.mac
  WCSimPhysicsListFactory *physFactory = new WCSimPhysicsListFactory();

  // Currently, default physics list is set to FTFP_BERT
  // The custom WCSim physics list option is removed in versions later than WCSim1.6.0
  const char *WCSIMDIR = std::getenv("WCSIMDIR");
  if (!(WCSIMDIR && WCSIMDIR[0])) { // make sure it's non-empty
    WCSIMDIR = "."; // the "default" value
    G4cout << "Note: WCSIMDIR not set, assuming: " << WCSIMDIR << G4endl;
  }
  G4cout << "B.Q: Read" << Form("/control/execute %s/macros/jobOptions.mac",WCSIMDIR) << G4endl;
  file_exists(Form("%s/macros/jobOptions.mac",WCSIMDIR));
  UI->ApplyCommand(Form("/control/execute %s/macros/jobOptions.mac",WCSIMDIR));

  // Initialize the physics factory to register the selected physics.
  physFactory->InitializeList();
  runManager->SetUserInitialization(physFactory);

  // Visualization
  G4VisManager* visManager = new WCSimVisManager;
  visManager->Initialize();

  // Set user action classes
  WCSimPrimaryGeneratorAction* myGeneratorAction = new 
    WCSimPrimaryGeneratorAction(WCSimdetector);
  runManager->SetUserAction(myGeneratorAction);


  WCSimRunAction* myRunAction = new WCSimRunAction(WCSimdetector, randomparameters);

  //save all the options from WCSimTuningParameters & WCSimPhysicsListFactory
  //(set in tuning_parameters.mac & jobOptions*.mac)
  tuningpars->SaveOptionsToOutput(myRunAction->GetRootOptions());
  physFactory->SaveOptionsToOutput(myRunAction->GetRootOptions());

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
    //    G4UIsession* session =  new G4UIterminal(new G4UItcsh);

    //using working example N04 for Qt UI Compatible code
#ifdef G4UI_USE
    G4UIExecutive * ui = new G4UIExecutive(argc,argv);
#ifdef G4VIS_USE
    // Visualization Macro
    UI->ApplyCommand("/control/execute WCSim.mac");
#endif
    ui->SessionStart();
    delete ui;
#endif

    // Start Interactive Mode
    //session->SessionStart();

    //delete session;
  }
  else           // Batch mode
  { 
    G4cout << "WE WILL EXECUTE THE MAC FILE READING SO IN BATCH MODE" << G4endl;

    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    file_exists(fileName);
    if(fileName == "vis.mac"){
      G4cout << "ERROR: Execute without arg for interactive mode" << G4endl;
      //return -1;
    }
    UI->ApplyCommand(command+fileName);

  }

  delete visManager;

  delete runManager;
  return 0;
}


