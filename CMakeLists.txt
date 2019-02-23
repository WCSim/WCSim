## Based on CMakeLists.txt from AnaEx02 in examples/extended of Geant4.9.6 ##
#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(WCSim)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -fdiagnostics-color=always")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fdiagnostics-color=always")
# NOTE: Geant4.10.2 changes the way VisualizationManager is instantiated - requires a verbosity argument.  
# To handle the two types of instantiation: this env variable needs to be defined for geant versions > 10.1:
if(DEFINED ENV{GEANT4_10_2})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGEANT4_10_2")
endif()
if(DEFINED ENV{NO_GENIE})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNO_GENIE")
  message(STATUS "GENIE features disabled.")
else()
  message(STATUS "GENIE features enabled.")
endif()
#----------------------------------------------------------------------------
# Find ROOT (required package) in CONFIG mode. Looking for ROOTConfig.cmake.
# Crucial for loading the proper definitions!
#
if(DEFINED ENV{ROOT_PATH})
  list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
  #message(STATUS "looking for root in ${CMAKE_MODULE_PATH}")
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{ROOT_PATH})
  message(STATUS "looking for root in ${CMAKE_MODULE_PATH}")
endif()
find_package(ROOT CONFIG REQUIRED )
#find_package(ROOT MODULE REQUIRED)
if(NOT ROOT_FOUND)
  message(STATUS "ROOT package not found.")  
  return()
else()
  message(STATUS "found ROOT at: ${ROOT_USE_FILE}")
endif()

## Load macros: need to compile ROOT through cmake first and execute bin/thisroot.sh
include(${ROOT_USE_FILE})
if(NOT DEFINED ENV{NO_GENIE})
#----------------------------------------------------------------------------
# GENIE stuff
#

#find_package(CLHEP REQUIRED)

set(_root_internal_libraries
    # for GENIE
    Geom
    EGPythia6
)
set(Root_LIBRARIES ${_root_internal_libraries})

link_directories($ENV{ROOTSYS}/lib)

include_directories($ENV{GENIE}/src)
link_directories($ENV{GENIE}/lib)

set(_genie_internal_libraries
    GAlgorithm GBaryonResonance GBase GBodekYang GCharm GCoh GDfrc GDIS GCrossSections GDecay GElas GElFF GHEP GEVGCore GEVGModules GEVGDrivers GGiBUU GHadronTransp GFragmentation GInteraction GLlewellynSmith GMEC GMessenger GNuGamma GNuE GNtuple GNuclear GNumerical GQPM GPDG GPDF GQEL GRES GRegistry GUtils GGeo GFluxDrivers GVLE GMuELoss GReWeight GNucleonDecay
)

include_directories($ENV{LOG4CPP_FQ_DIR}/include)
link_directories($ENV{LOG4CPP_FQ_DIR}/lib)
set(_log4cpp_internal_libraries log4cpp)

include_directories($ENV{XERCESCROOT}/include)
link_directories($ENV{XERCESCROOT}/lib)
link_directories($ENV{LIBXML2ROOT}/lib)
set(_xerces_internal_libraries xerces-c xml2)

#include_directories($ENV{ROOTSYS}/include)
link_directories($ENV{ROOTSYS}/lib)
#include_directories($ENV{GEANT4_FQ_DIR}/include)

set(Genie_LIBRARIES ${_genie_internal_libraries} ${_log4cpp_internal_libraries} ${_xerces_internal_libraries} )
endif()
#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
# Locate sources and headers for this project
# ## NOT NEEDED FOR DICT
message(STATUS "using Geant4_INCLUDE_DIR: ${Geant4_INCLUDE_DIR}")
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${PROJECT_SOURCE_DIR}/../shared/include 
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Add libraries: need to compile the Dict before linking WCSim !!
# in standard makefile, need to make rootcint anyway before standard make
#

## WCSimRootDict.cc regeneration by rootcint
## Use ROOT 5.34.32 as some issues with PARSE_ARGUMENTS were found in older ROOT versions (ROOT 5.34.11)
ROOT_GENERATE_DICTIONARY(${CMAKE_CURRENT_SOURCE_DIR}/src/WCSimRootDict
${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimRootEvent.hh
${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimRootGeom.hh
${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimPmtInfo.hh 
${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimRootOptions.hh
${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimEnumerations.hh
LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimRootLinkDef.hh
MODULE WCSimRoot
)

## Crucial for reading ROOT classes: make shared object library
add_library(WCSimRoot SHARED
./src/WCSimRootEvent.cc
./src/WCSimRootGeom.cc
./src/WCSimPmtInfo.cc
./src/WCSimEnumerations.cc
./src/WCSimRootDict.cxx
./src/WCSimRootOptions.cc
)
target_link_libraries(WCSimRoot  ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
if(DEFINED ENV{GEANT4_PATH})
  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{GEANT4_PATH})
  message(STATUS "Looking for GEANT4 in = ${CMAKE_PREFIX_PATH}")
endif()
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
message(STATUS "using Geant4: ${Geant4_USE_FILE}")
include(${Geant4_USE_FILE})  ## NOT needed for Dict

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)


#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(WCSim WCSim.cc ${sources} ${headers})
target_link_libraries(WCSim ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${Genie_LIBRARIES} ${Root_LIBRARIES} WCSimRoot Tree)



#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build WCSim. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(WCSIM_SCRIPTS
  macros/jobOptions.mac
  WCSim.mac
  macros/daq.mac
  macros/visOGLSX.mac
  macros/visRayTracer.mac
  macros/tuning_parameters.mac
  WCSimRootDict_rdict.pcm
  )

foreach(_script ${WCSIM_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
#add_custom_target(WCSim DEPENDS WCSim)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
#install(TARGETS WCSim DESTINATION bin)


