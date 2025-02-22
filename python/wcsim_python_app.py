import ROOT
import cppyy

print("Loading G4 Includes")
cppyy.cppdef("#define WCSIM_CHECK_GEOMETRY_OVERLAPS 0")
cppyy.add_include_path("/Users/stowell/Research/Development/HK/geant4-v10.3.3/install/include/Geant4/")
cppyy.include("G4ios.hh")
cppyy.include("G4RunManager.hh")
cppyy.include("G4UImanager.hh")
cppyy.include("G4UIterminal.hh")
cppyy.include("G4UItcsh.hh")

print("Loading WCSim Includes")
cppyy.add_include_path("include/")
cppyy.include("WCSimDetectorConstruction.hh")
cppyy.include("WCSimPhysicsListFactory.hh")
cppyy.include("WCSimPhysicsListFactoryMessenger.hh")
cppyy.include("WCSimTuningParameters.hh")
cppyy.include("WCSimTuningMessenger.hh")
cppyy.include("WCSimPrimaryGeneratorAction.hh")
cppyy.include("WCSimEventAction.hh")
cppyy.include("WCSimRunAction.hh")
cppyy.include("WCSimStackingAction.hh")
cppyy.include("WCSimTrackingAction.hh")
cppyy.include("WCSimSteppingAction.hh")
cppyy.include("WCSimVisManager.hh")
cppyy.include("WCSimRandomParameters.hh")

print("Loading G4 Libraries")
cppyy.add_library_path("/Users/stowell/Research/Development/HK/geant4-v10.3.3/install/lib")

cppyy.load_library("G4OpenGL") 
cppyy.load_library("G4gl2ps") 
cppyy.load_library("G4Tree") 
cppyy.load_library("G4FR") 
cppyy.load_library("G4GMocren") 
cppyy.load_library("G4visHepRep") 
cppyy.load_library("G4RayTracer") 
cppyy.load_library("G4VRML") 
cppyy.load_library("G4vis_management") 
cppyy.load_library("G4modeling") 
cppyy.load_library("G4interfaces") 
cppyy.load_library("G4persistency") 
cppyy.load_library("G4analysis") 
cppyy.load_library("G4error_propagation") 
cppyy.load_library("G4readout") 
cppyy.load_library("G4physicslists") 
cppyy.load_library("G4run") 
cppyy.load_library("G4event") 
cppyy.load_library("G4tracking") 
cppyy.load_library("G4parmodels") 
cppyy.load_library("G4processes") 
cppyy.load_library("G4digits_hits") 
cppyy.load_library("G4track") 
cppyy.load_library("G4particles") 
cppyy.load_library("G4geometry") 
cppyy.load_library("G4materials") 
cppyy.load_library("G4graphics_reps") 
cppyy.load_library("G4intercoms") 
cppyy.load_library("G4global") 
cppyy.load_library("G4clhep") 
cppyy.load_library("G4zlib")

print("Loading WCSim Libraries")
cppyy.add_library_path("lib/")

cppyy.load_library("WCSimCore")
cppyy.load_library("WCSimRoot")

runManager = cppyy.gbl.G4RunManager()

UI = cppyy.gbl.G4UImanager.GetUIpointer()

tuningpars = cppyy.gbl.WCSimTuningParameters()

UI.ApplyCommand("/control/execute tuning_parameters.mac")

randomparameters = cppyy.gbl.WCSimRandomParameters()
WCSimConfiguration = 2

WCSimdetector = cppyy.gbl.WCSimDetectorConstruction(WCSimConfiguration,tuningpars)

runManager.SetUserInitialization(WCSimdetector)

physFactory = cppyy.gbl.WCSimPhysicsListFactory()

physFactory.InitializeList()
runManager.SetUserInitialization(physFactory)

# visManager = cppyy.gbl.WCSimVisManager()
# visManager.Initialize()

myGeneratorAction = cppyy.gbl.WCSimPrimaryGeneratorAction(WCSimdetector)
runManager.SetUserAction(myGeneratorAction)

myRunAction = cppyy.gbl.WCSimRunAction(WCSimdetector, randomparameters)


# print("FINISHEsD")

tuningpars.SaveOptionsToOutput(myRunAction.GetRootOptions())
physFactory.SaveOptionsToOutput(myRunAction.GetRootOptions())

runManager.SetUserAction(myRunAction)

runManager.SetUserAction(cppyy.gbl.WCSimEventAction(myRunAction, WCSimdetector,
						 myGeneratorAction))

track = cppyy.gbl.WCSimTrackingAction()
runManager.SetUserAction(track)

stack = cppyy.gbl.WCSimStackingAction(WCSimdetector)
runManager.SetUserAction(stack)

step = cppyy.gbl.WCSimSteppingAction(myRunAction,WCSimdetector)
runManager.SetUserAction(step)

runManager.Initialize()

# UI.ApsplyCommand(execommand + G4String(argv[1]))
