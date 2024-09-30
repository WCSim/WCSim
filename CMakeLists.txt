# Minimum cmake verison 3.1 required for the variable CMAKE_CXX_STANDARD
cmake_minimum_required(VERSION 3.1)

# ##############################################################################
# setup #
# ##############################################################################

set(PROJECT_NAME WCSim)

# Add a WCSimRoot library installation (simpler dependencies, used in fitqun)
option (WCSim_WCSimRoot_only
        "Allows to only install the WCSimROOT library (file format); no need for G4!"
        OFF
)
MESSAGE(STATUS "WCSim_WCSimRoot_only: ${WCSim_WCSimRoot_only}")
if (${WCSim_WCSimRoot_only})
    MESSAGE(STATUS "Will install only WCSimRoot library; disabling Geant4 package search!")
    set(PROJECT_NAME WCSimRoot)
endif ()

set(PROJECT_VERSION 1.12.16)

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})

# From https://blog.kitware.com/cmake-and-the-default-build-type/
# Set a default build type to Release if none was specified
# This has to be before Scarab PackageBuilder since the package builder defines the default mode to DEBUG...
set(default_build_type "Release")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
            STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
            "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()

set(PBUILDER ON)
list(APPEND CMAKE_MODULE_PATH
        ${PROJECT_SOURCE_DIR}/cmake)
include(PackageBuilder)

pbuilder_prepare_project()

set(CMAKE_CXX_STANDARD 14) # Enable c++14 standard


########
# HepMC3
########
if (NOT ${WCSim_WCSimRoot_only}) # only looking for HepMC3 if WCSim_WCSimRoot_only is false
  find_package(HepMC3 REQUIRED)
  if (NOT HepMC3_FOUND)
    message(FATAL "Unable to find HepMC3")
  endif (NOT HepMC3_FOUND)
  include_directories (${HEPMC3_INCLUDE_DIR})
  LIST(APPEND PUBLIC_EXT_LIBS ${HEPMC3_LIBRARIES})
endif ()

######
# ROOT
######
find_package(ROOT COMPONENTS CONFIG REQUIRED)
if (ROOT_FOUND)

else (ROOT_FOUND)
    message(FATAL "Unable to find ROOT")
endif (ROOT_FOUND)

include(${ROOT_USE_FILE})
include_directories (${ROOT_INCLUDE_DIR})
LIST(APPEND PUBLIC_EXT_LIBS ${ROOT_LIBRARIES})

# Manual way to set the CXX standard if ROOT uses c++17 standard
# Not very gracious...
cmake_policy(SET CMP0057 NEW)
string(REPLACE " " ";" TMP_ROOT_FLAGS ${ROOT_CXX_FLAGS})
if("-std=c++20" IN_LIST TMP_ROOT_FLAGS)
    set(CMAKE_CXX_STANDARD 20)
elseif ("-std=c++17" IN_LIST TMP_ROOT_FLAGS)
    set(CMAKE_CXX_STANDARD 17)
elseif("-std=c++14" IN_LIST TMP_ROOT_FLAGS)
    set(CMAKE_CXX_STANDARD 14)
elseif("-std=c++11" IN_LIST TMP_ROOT_FLAGS)
    set(CMAKE_CXX_STANDARD 11)
else()
    set(CMAKE_CXX_STANDARD 17)
endif ()

MESSAGE(STATUS "Current CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")



if (NOT ${WCSim_WCSimRoot_only}) # only looking for G4 if WCSim_WCSimRoot_only is false
    #----------------------------------------------------------------------------
    # Find Geant4 package, activating all available UI and Vis drivers by default
    # You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
    # to build a batch mode only executable
    option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
    if(WITH_GEANT4_UIVIS)
        find_package(Geant4 REQUIRED ui_all vis_all)
    else()
        find_package(Geant4 REQUIRED)
    endif()
    LIST(APPEND PUBLIC_EXT_LIBS ${Geant4_LIBRARIES})
    #----------------------------------------------------------------------------
    # Setup Geant4 include directories and compile definitions
    # Setup include directory for this project
    #
    include(${Geant4_USE_FILE})  ## NOT needed for Dict
endif()


#########
# C++ Standard and flags
#########

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.7)
    set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=c++11 -Wno-overloaded-virtual")
else()
    set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

# Set flag for git version to be used as c++ preprocessor macro
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGIT_HASH=\"\\\"`cd ${PROJECT_SOURCE_DIR};git describe --always --long --tags --dirty`\\\"\"")

option (WCSim_DEBUG_COMPILE_FLAG
  "Toggle the gcc debug flag"
  OFF
)
if(WCSim_DEBUG_COMPILE_FLAG STREQUAL ON)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

##############
# WCSim compilation options
##############
# Caution: setting a new default value to this variable requires the recompilation of the full package
option(WCSim_Geometry_Overlaps_CHECK
  "Toggle GEANT4 overlap checking when placing geometry elements"
  OFF
)
set( WCSIM_CHECK_GEOMETRY_OVERLAPS $<IF:$<BOOL:${WCSim_Geometry_Overlaps_CHECK}>,true,false>)

option(WCSIM_SAVE_PHOTON_HISTORY_FLAG
  "Toggle WCSim to save photon scattering and reflection history"
  OFF
)
if(WCSIM_SAVE_PHOTON_HISTORY_FLAG STREQUAL ON)
  add_definitions(-DWCSIM_SAVE_PHOTON_HISTORY)
endif()

#########
# copy extra data files
#########

pbuilder_install_files(${CMAKE_INSTALL_PREFIX}/data
        ${PROJECT_SOURCE_DIR}/data/MuonFlux-HyperK-ThetaPhi.dat
      	${PROJECT_SOURCE_DIR}/data/biasprofile.dat
        ${PROJECT_SOURCE_DIR}/data/theta_cos15deg_FWHM.txt
        ${PROJECT_SOURCE_DIR}/data/DSNBFluxes.json
      	${PROJECT_SOURCE_DIR}/data/LightInjectors.json
      	${PROJECT_SOURCE_DIR}/data/LI_OD_laser_ball_photon_positions.csv
        ${PROJECT_SOURCE_DIR}/data/mPMT_Position_WCTE.txt
        ${PROJECT_SOURCE_DIR}/data/mPMT_Position_WCTE_CDS.txt
        ${PROJECT_SOURCE_DIR}/data/ground_state_spectrum.txt
        ${PROJECT_SOURCE_DIR}/data/first_excited_spectrum.txt
        ${PROJECT_SOURCE_DIR}/data/second_excited_spectrum.txt
        ${PROJECT_SOURCE_DIR}/data/CathodeParameters.txt
        ${PROJECT_SOURCE_DIR}/data/CathodeParameters_SK.txt
        ${PROJECT_SOURCE_DIR}/data/CathodeParameters_KCsCb.txt
        ${PROJECT_SOURCE_DIR}/data/CathodeParameters_RbCsCb.txt
	)

pbuilder_install_files(${CMAKE_INSTALL_PREFIX}/macros
	      #required macros
        ${PROJECT_SOURCE_DIR}/macros/jobOptions.mac
        ${PROJECT_SOURCE_DIR}/macros/tuning_parameters.mac
	      #options used as includes in WCSim.mac and dependents
        ${PROJECT_SOURCE_DIR}/macros/daq.mac
        ${PROJECT_SOURCE_DIR}/macros/geom_hk_mpmt_options.mac
        ${PROJECT_SOURCE_DIR}/macros/geom_hk_od_options.mac
        ${PROJECT_SOURCE_DIR}/macros/mPMT_37pmtHK.mac
        ${PROJECT_SOURCE_DIR}/macros/mPMT_interesting1.mac
        ${PROJECT_SOURCE_DIR}/macros/mPMT_interesting2_SKwCover.mac
        ${PROJECT_SOURCE_DIR}/macros/mPMT_nuPrism1.mac
        ${PROJECT_SOURCE_DIR}/macros/mPMT_nuPrism2.mac
        ${PROJECT_SOURCE_DIR}/macros/mPMT_radon_with_bias.mac
        ${PROJECT_SOURCE_DIR}/macros/mPMT_radon_without_bias.mac
	      #visualisation macros
        ${PROJECT_SOURCE_DIR}/macros/visOGLSX.mac
        ${PROJECT_SOURCE_DIR}/macros/visRayTracer.mac
        ${PROJECT_SOURCE_DIR}/macros/visOGLQT.mac
        ${PROJECT_SOURCE_DIR}/macros/visOGLQT_2.mac
        ${PROJECT_SOURCE_DIR}/macros/visOGLQT-OD.mac
        ${PROJECT_SOURCE_DIR}/macros/vis_original.mac
        #misc macros with different geometries
        # WCTE
        ${PROJECT_SOURCE_DIR}/macros/WCTE.mac
        #nuprism
        ${PROJECT_SOURCE_DIR}/macros/NuPRISM.mac
        #WCTE
        ${PROJECT_SOURCE_DIR}/macros/WCTE.mac
        #HK FD ID-only
        ${PROJECT_SOURCE_DIR}/macros/WCSim_hybrid.mac
        ${PROJECT_SOURCE_DIR}/macros/WCSim_hybrid_injector.mac
        ${PROJECT_SOURCE_DIR}/macros/WCSim_hybrid_vectorfile.mac
        ${PROJECT_SOURCE_DIR}/macros/mPMT_radon_with_bias.mac
        ${PROJECT_SOURCE_DIR}/macros/mPMT_radon_without_bias.mac
        #Old HK FD geometries
        ${PROJECT_SOURCE_DIR}/macros/OD.mac
        ${PROJECT_SOURCE_DIR}/macros/OD_10k.mac
        ${PROJECT_SOURCE_DIR}/macros/wcsim_temp.mac
        ${PROJECT_SOURCE_DIR}/macros/WCSim_1part_HK.mac
        ${PROJECT_SOURCE_DIR}/macros/WCSim_hybrid.mac
        ${PROJECT_SOURCE_DIR}/macros/WCSim_hybrid_vectorfile.mac
        ${PROJECT_SOURCE_DIR}/macros/WCSim_hybrid_injector.mac
        ${PROJECT_SOURCE_DIR}/macros/LIGen.mac
	)

pbuilder_install_files(${CMAKE_INSTALL_PREFIX}
        ${PROJECT_SOURCE_DIR}/WCSim.mac
        )

pbuilder_install_files_executable(${CMAKE_INSTALL_PREFIX}/bin
        ${PROJECT_SOURCE_DIR}/rootwc/rootwc
        ${PROJECT_SOURCE_DIR}/sample-root-scripts/MakeKin.py
        ${PROJECT_SOURCE_DIR}/sample-root-scripts/KinSplitter.py
        )

pbuilder_install_files(${CMAKE_INSTALL_PREFIX}/bin/rootwc-files
        ${PROJECT_SOURCE_DIR}/rootwc/loadincs.C
        ${PROJECT_SOURCE_DIR}/rootwc/loadlibs.C
        ${PROJECT_SOURCE_DIR}/rootwc/rootwc.C
        )

pbuilder_install_files(${CMAKE_INSTALL_PREFIX}/mPMT-configfiles
        ${PROJECT_SOURCE_DIR}/mPMT-configfiles/mPMTconfig_19_nuPrism.txt
        ${PROJECT_SOURCE_DIR}/mPMT-configfiles/mPMTconfig_19_nuPrism_3ring.txt
        ${PROJECT_SOURCE_DIR}/mPMT-configfiles/mPMTconfig_19_nuPrism_3ring_WCTE.txt
        ${PROJECT_SOURCE_DIR}/mPMT-configfiles/mPMTconfig_30_13_3.txt
        ${PROJECT_SOURCE_DIR}/mPMT-configfiles/mPMTconfig_33_13_1.txt
        ${PROJECT_SOURCE_DIR}/mPMT-configfiles/mPMTconfig_33_50.txt
        ${PROJECT_SOURCE_DIR}/mPMT-configfiles/mPMTconfig_33_50_1.txt
        ${PROJECT_SOURCE_DIR}/mPMT-configfiles/mPMTconfig_34_22.5_1.txt
        ${PROJECT_SOURCE_DIR}/mPMT-configfiles/mPMTconfig_34_22.5_2.txt
        ${PROJECT_SOURCE_DIR}/mPMT-configfiles/mPMTconfig_34_22_1.txt
        ${PROJECT_SOURCE_DIR}/mPMT-configfiles/mPMTconfig_34_22_2.txt
        ${PROJECT_SOURCE_DIR}/mPMT-configfiles/mPMTconfig_37_HK.txt
        )


# Library
include_directories (
        ${ROOT_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/include
)

add_subdirectory (src)


#########
# Configure package and final installation files
#########

if (NOT ${WCSim_WCSimRoot_only})
configure_file(${PROJECT_SOURCE_DIR}/cmake/WCSimConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/WCSimConfig.cmake @ONLY)
else ()
    configure_file(${PROJECT_SOURCE_DIR}/cmake/WCSimRootConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/WCSimRootConfig.cmake @ONLY)
endif()
pbuilder_do_package_config()

configure_file(cmake/this_wcsim.sh.in this_wcsim.sh)
pbuilder_install_files(${BIN_INSTALL_DIR} ${CMAKE_CURRENT_BINARY_DIR}/this_wcsim.sh)

add_custom_target(install_${PROJECT_NAME}
        "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install
        COMMENT "Installing ${PROJECT_NAME}")
