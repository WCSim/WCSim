# WCSim Python Package
# p.stowell

# Allows Geant4 macros to be started from within python.
# Uses ROOT's cppyy to build automatic bindings.
# There is a ~30s load up time, so not suitable for production.

_debug = False
if (_debug): print("WCSim Python")

# Import PYROOT
if (_debug): print("- importing ROOT")
try:
    import ROOT
except Exception as error:
    print("Failed to import ROOT. We need this for WCSim.")
    print("This needs to have a matching cppyy version to G4.")
    raise(error)

# Import CPPYY explicitly to check we had access to it in ROOT
if (_debug): print("- importing cppyy")
try:
    import cppyy
except Exception as error:
    print("Failed to import cppyy. We need this for WCSim python.")
    print("This needs to have a matching cppyy version to G4.")
    raise(error)

# Get GEANT4 INCLUDE FOLDER
def _get_geant4_dir():
    """Returns the Geant4 include directory by calling geant4-config."""
    import subprocess

    try:
        output = subprocess.check_output(['geant4-config', '--prefix'])
        return output.decode().strip()
    except subprocess.CalledProcessError as e:
        print("Error calling geant4-config:", e)
        return None

def _get_geant4_lib_dir(g4dir):
    """Returns the Geant4 lib directory"""
    import os
    for libdir in ['lib', 'lib64']:
        ld = f'{g4dir}/{libdir}'
        if os.path.isdir(ld):
            return ld
    print('Could not find Geant4 lib directory')
    return None

_GEANT4_DIR = _get_geant4_dir()
if not _GEANT4_DIR:
    raise RuntimeError("Failed to find geant4-prefix dir! Is geant4-config available?")

_GEANT4_LIB_DIR = _get_geant4_lib_dir(_GEANT4_DIR)
if not _GEANT4_LIB_DIR:
    raise RuntimeError("Failed to find geant4 lib dir!")

# Load Minimal G4 Headers
if (_debug): print("- loading G4 Includes")
try:
    cppyy.add_include_path(_GEANT4_DIR + "/include/Geant4/")
    cppyy.include("G4ios.hh")
    cppyy.include("G4RunManager.hh")
    cppyy.include("G4UImanager.hh")
    cppyy.include("G4UIterminal.hh")
    cppyy.include("G4UItcsh.hh")
except Exception as error:
    print("Failed to load minimal Geant4 includes")
    raise(error)

# Get WCSIM PREFIX
import os as _os
WCSIM_BUILD_DIR=_os.environ["WCSIM_BUILD_DIR"]

# Load Minimal WCSim Headers
if (_debug): print("- loading WCSim Includes")
try:
    cppyy.cppdef("#define WCSIM_CHECK_GEOMETRY_OVERLAPS 0")
    cppyy.add_include_path(WCSIM_BUILD_DIR + "/include/")
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
    cppyy.include("GeometryScan.hh")
except Exception as error:
    print("Failed to load WCSim includes. Check WCSIM_BUILD_DIR is set")
    print("Looking inside ", WCSIM_BUILD_DIR + "/include/")
    raise(error)

if (_debug): print("- loading G4 Libraries")
try:
    cppyy.add_library_path(_GEANT4_LIB_DIR)
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
except Exception as error:
    print("Looking inside ", _GEANT4_LIB_DIR)
    print("Failed to load GEANT4 libraries. You may not have compiled with all expected visualisation libraries; If you don't need visualisation, you can comment out loading of e.g. G4OpenGL and G4gl2ps")
    raise(error)

if (_debug): print("- loading WCSim Libraries")
try:
    cppyy.add_library_path(WCSIM_BUILD_DIR + "/lib/")
    cppyy.load_library("WCSimCore")
    cppyy.load_library("WCSimRoot")
except Exception as error:
    raise(error)

# Define handle to allow 
# import WCSim as wc
# wc.g4.G4Box()
g4 = cppyy.gbl

# Generic holder for WCSimInstsance
class WCSimPythonInstance:
    def __init__(self):
        self.name = "WCSimPythonInstance"
        self.UI = None

    def apply(self, command):
        self.UI.ApplyCommand(command)

    def execute(self, macro_path):
        self.UI.ApplyCommand(f"/control/execute {macro_path}")

# Global Instance
gWCSimInstance = None

# Python equivalent of standard WCSim.cc 
def build_wcsim_instance(tuning_macro = WCSIM_BUILD_DIR + "/macros/tuning_parameters.mac",
                         job_options = WCSIM_BUILD_DIR + "/macros/jobOptions.mac"):
        
    ins = WCSimPythonInstance()

    ins.run_manager = cppyy.gbl.G4RunManager()
    ins.UI = cppyy.gbl.G4UImanager.GetUIpointer()

    ins.tuning_parameters = cppyy.gbl.WCSimTuningParameters()
    ins.UI.ApplyCommand(f"/control/execute {tuning_macro}")

    ins.random_parameters = cppyy.gbl.WCSimRandomParameters()
    ins.configuration = 2
    ins.detector_construction = cppyy.gbl.WCSimDetectorConstruction(ins.configuration,
                                                        ins.tuning_parameters)
    ins.run_manager.SetUserInitialization(ins.detector_construction)

    ins.physics_list = cppyy.gbl.WCSimPhysicsListFactory()
    ins.UI.ApplyCommand(f"/control/execute {job_options}")
    ins.physics_list.InitializeList()
    ins.run_manager.SetUserInitialization(ins.physics_list)

    ins.generator_action = cppyy.gbl.WCSimPrimaryGeneratorAction(ins.detector_construction)
    ins.run_manager.SetUserAction(ins.generator_action)

    ins.run_action = cppyy.gbl.WCSimRunAction(ins.detector_construction, ins.random_parameters)
    ins.tuning_parameters.SaveOptionsToOutput(ins.run_action.GetRootOptions())
    ins.physics_list.SaveOptionsToOutput(ins.run_action.GetRootOptions())
    ins.run_manager.SetUserAction(ins.run_action)

    ins.event_action = cppyy.gbl.WCSimEventAction(ins.run_action, 
                                            ins.detector_construction,
                                            ins.generator_action)
    ins.run_manager.SetUserAction(ins.event_action)

    ins.track_action = cppyy.gbl.WCSimTrackingAction()
    ins.run_manager.SetUserAction(ins.track_action)

    ins.stack_action = cppyy.gbl.WCSimStackingAction(ins.detector_construction)
    ins.run_manager.SetUserAction(ins.stack_action)

    ins.step_action = cppyy.gbl.WCSimSteppingAction(ins.run_action,ins.detector_construction)
    ins.run_manager.SetUserAction(ins.step_action)

    ins.run_manager.Initialize()
    
    ins.UI.__python_owns__ = False
    ins.event_action.__python_owns__ = False
    ins.physics_list.__python_owns__ = False
    ins.run_manager.__python_owns__ = False
    ins.track_action.__python_owns__ = False
    ins.generator_action.__python_owns__ = False
    ins.random_parameters.__python_owns__ = False
    ins.stack_action.__python_owns__ = False
    ins.tuning_parameters.__python_owns__ = False
    ins.detector_construction.__python_owns__ = False
    ins.run_action.__python_owns__ = False
    ins.step_action.__python_owns__ = False

    gWCSimInstance = ins

    return ins

# Load IPython magic if in jupyter
try:
    # Main Jupyter Load Check    
    from IPython.core.magic import register_cell_magic

    @register_cell_magic
    def wcsim_macro(filename, cell):
        """
        Magic function that saves the content of a cell to a file and calls my_function with the filename.
        Usage: %%save_and_call filename
        """

        if (not gWCSimInstance):
            raise RuntimeError("No WCSim Instance currently setup!")
                
        # Save the cell content to the file
        with open(filename, 'w') as f:
            f.write(cell)

        # Run on the global instance        
        gWCSimInstance.execute(filename)

except:
    pass
