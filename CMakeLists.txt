## Based on CMakeLists.txt from AnaEx02 in examples/extended of Geant4.9.6 ##
## Modified for ROOT6 support on 2020/11/26 by Guillaume Pronost
#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.8.10 FATAL_ERROR)
project(WCSim)

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.4)
    message(STATUS "GCC version >= 4.4 required!")
    return()
endif()

#----------------------------------------------------------------------------
# Find ROOT (required package) in CONFIG mode. Looking for ROOTConfig.cmake.
# Crucial for loading the proper definitions!
#
find_package(ROOT COMPONENTS CONFIG REQUIRED)
if(NOT ROOT_FOUND)
  message(STATUS "ROOT package not found.") 
  return()
endif()

message(STATUS "ROOT VERSION ${ROOT_VERSION}")

## Load macros: need to compile ROOT through cmake first and execute bin/thisroot.sh
include(${ROOT_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# ## NOT NEEDED FOR DICT
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIRS})

#----------------------------------------------------------------------------
# Add libraries: need to compile the Dict before linking WCSim !!
# in standard makefile, need to make rootcint anyway before standard make
#

## WCSimRootDict.cxx regeneration by rootcint
## Use ROOT 5.34.32 as some issues with PARSE_ARGUMENTS were found in older ROOT versions (ROOT 5.34.11)
if ( ${ROOT_VERSION} GREATER_EQUAL 6 )
	# ROOT6 doesn't accept '/' in target and header names, use include_directories instead 
	# and move the dict source after
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
	ROOT_GENERATE_DICTIONARY(WCSimRootDict 
			WCSimRootEvent.hh 
			WCSimRootGeom.hh 
			WCSimPmtInfo.hh 
			WCSimEnumerations.hh 
			WCSimRootOptions.hh 
			TJNuBeamFlux.hh 
			TNRooTrackerVtx.hh 
			WCSimRootTools.hh 
			LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimRootLinkDef.hh)
	
else()
	# ROOT5 need the full path
	ROOT_GENERATE_DICTIONARY(WCSimRootDict 
			${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimRootEvent.hh 
			${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimRootGeom.hh 
			${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimPmtInfo.hh 
			${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimEnumerations.hh 
			${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimRootOptions.hh 
			${CMAKE_CURRENT_SOURCE_DIR}/include/TJNuBeamFlux.hh 
			${CMAKE_CURRENT_SOURCE_DIR}/include/TNRooTrackerVtx.hh 
			${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimRootTools.hh 
			LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimRootLinkDef.hh)
			
endif()




## Crucial for reading ROOT classes: make shared object library
add_library(WCSimRoot SHARED 
		./src/WCSimRootEvent.cc 
		./src/WCSimRootGeom.cc 
		./src/WCSimPmtInfo.cc 
		./src/WCSimEnumerations.cc 
		./src/WCSimRootOptions.cc 
		./src/TJNuBeamFlux.cc 
		./src/TNRooTrackerVtx.cc 
		./src/WCSimRootTools.cc 
		WCSimRootDict.cxx)
target_link_libraries(WCSimRoot  ${ROOT_LIBRARIES})

# Create libWCSimRootDict.so (needed for ROOT6)
add_custom_command(TARGET WCSimRoot
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E create_symlink libWCSimRoot.so libWCSimRootDict.so)
		
if ( ${ROOT_VERSION} LESS 6 )
	# Create Rootmap (not automatically done for ROOT5)
	add_custom_command(TARGET WCSimRoot
		POST_BUILD
		COMMAND rlibmap -o ${CMAKE_CURRENT_BINARY_DIR}/libWCSimRoot.rootmap -l ${CMAKE_CURRENT_BINARY_DIR}/libWCSimRoot.so -c ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimRootLinkDef.hh)
endif()

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
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

# To use <unordered_map>, ie. use C++11 now from GCC 4.4 onwards support for unordered_maps.
# Had to change G4float to G4double for QE because of double arithmatic inside the array.
if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.7)
    set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=c++11")
else()
    set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

# Set flag for git version to be used as c++ preprocessor macro
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGIT_HASH=\"\\\"`cd ${PROJECT_SOURCE_DIR};git describe --always --long --tags --dirty`\\\"\"")

add_executable(WCSim WCSim.cc ${sources} ${headers})
target_link_libraries(WCSim ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} WCSimRoot Tree)  #add profiler to use gperftools


#----------------------------------------------------------------------------
# Copy required files to the build directory, i.e. the directory in which we
# build WCSim. This is so that we can run the executable directly because it
# relies on these files being in the current working directory and then so
# that we can directly analyze the produced ROOT files, too.
#
if (NOT (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR))
  file(COPY ${PROJECT_SOURCE_DIR}/mPMT-configfiles DESTINATION ${PROJECT_BINARY_DIR} USE_SOURCE_PERMISSIONS)
  file(COPY ${PROJECT_SOURCE_DIR}/macros DESTINATION ${PROJECT_BINARY_DIR} USE_SOURCE_PERMISSIONS)
  # file(COPY ${PROJECT_SOURCE_DIR}/include DESTINATION ${PROJECT_BINARY_DIR} USE_SOURCE_PERMISSIONS)
  set(WCSIM_FILES
    WCSim.mac
    biasprofile.dat
    tuningNominal.mac
    WCSim_hybrid.mac
    WCSim_hybrid.sh
    include/TJNuBeamFlux.hh
    include/TNRooTrackerVtx.hh
    include/WCSimEnumerations.hh
    include/WCSimPmtInfo.hh
    include/WCSimRootEvent.hh
    include/WCSimRootGeom.hh
    include/WCSimRootOptions.hh
    include/WCSimRootTools.hh
    )
  foreach(_file ${WCSIM_FILES})
    configure_file(
      ${PROJECT_SOURCE_DIR}/${_file}
      ${PROJECT_BINARY_DIR}/${_file}
      COPYONLY
      )
  endforeach()
else()
  message(STATUS "Note: building in-place, no need to copy anything.")
endif()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
#add_custom_target(WCSim DEPENDS WCSim)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
#install(TARGETS WCSim DESTINATION bin)

