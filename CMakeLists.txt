# Minimum cmake verison 3.1 required for the variable CMAKE_CXX_STANDARD
cmake_minimum_required(VERSION 3.1)

# ##############################################################################
# setup #
# ##############################################################################

set(PROJECT_NAME WCSim)
set(PROJECT_VERSION 1.0)

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
LIST(APPEND PUBLIC_EXT_LIBS ${Geant4_LIBRARIES})

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})  ## NOT needed for Dict

#########
# sources
#########

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.7)
    set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=c++11")
else()
    set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

# Set flag for git version to be used as c++ preprocessor macro
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGIT_HASH=\"\\\"`cd ${PROJECT_SOURCE_DIR};git describe --always --long --tags --dirty`\\\"\"")

# data and macros copy

pbuilder_install_files(${CMAKE_INSTALL_PREFIX}/data
        ${PROJECT_SOURCE_DIR}/data/MuonFlux-HyperK-ThetaPhi.dat)

pbuilder_install_files(${CMAKE_INSTALL_PREFIX}/macros
        ${PROJECT_SOURCE_DIR}/macros/jobOptions.mac
        ${PROJECT_SOURCE_DIR}/macros/daq.mac
        ${PROJECT_SOURCE_DIR}/macros/visOGLSX.mac
        ${PROJECT_SOURCE_DIR}/macros/visRayTracer.mac
        ${PROJECT_SOURCE_DIR}/macros/visOGLQt.mac
        ${PROJECT_SOURCE_DIR}/macros/tuning_parameters.mac
        ${PROJECT_SOURCE_DIR}/macros/OD.mac
        ${PROJECT_SOURCE_DIR}/macros/OD_10k.mac
        )

pbuilder_install_files(${CMAKE_INSTALL_PREFIX}
        ${PROJECT_SOURCE_DIR}/WCSim.mac
        )

pbuilder_install_files(${CMAKE_INSTALL_PREFIX}/bin/rootwc
        ${PROJECT_SOURCE_DIR}/rootwc/loadincs.C
        ${PROJECT_SOURCE_DIR}/rootwc/loadlibs.C
        ${PROJECT_SOURCE_DIR}/rootwc/rootwc
        ${PROJECT_SOURCE_DIR}/rootwc/rootwc.C
        )

configure_file(cmake/this_wcsim.sh.in this_wcsim.sh)
pbuilder_install_files(${BIN_INSTALL_DIR} ${CMAKE_CURRENT_BINARY_DIR}/this_wcsim.sh)

add_custom_target(install_${PROJECT_NAME}
        /include
)

# Library
include_directories (
        ${ROOT_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/include
)

add_subdirectory (src)



configure_file(${PROJECT_SOURCE_DIR}/WCSimConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/WCSimConfig.cmake @ONLY)
pbuilder_do_package_config()

# data and macros copy

pbuilder_install_files(${CMAKE_INSTALL_PREFIX}/data
        ${PROJECT_SOURCE_DIR}/data/MuonFlux-HyperK-ThetaPhi.dat)

pbuilder_install_files(${CMAKE_INSTALL_PREFIX}/macros
        ${PROJECT_SOURCE_DIR}/macros/jobOptions.mac
        ${PROJECT_SOURCE_DIR}/macros/daq.mac
        ${PROJECT_SOURCE_DIR}/macros/visOGLSX.mac
        ${PROJECT_SOURCE_DIR}/macros/visRayTracer.mac
        ${PROJECT_SOURCE_DIR}/macros/visOGLQt.mac
        ${PROJECT_SOURCE_DIR}/macros/tuning_parameters.mac
        ${PROJECT_SOURCE_DIR}/macros/OD.mac
        ${PROJECT_SOURCE_DIR}/macros/OD_10k.mac
        )

pbuilder_install_files(${CMAKE_INSTALL_PREFIX}
        ${PROJECT_SOURCE_DIR}/WCSim.mac
        )

pbuilder_install_files(${CMAKE_INSTALL_PREFIX}/bin/rootwc
        ${PROJECT_SOURCE_DIR}/rootwc/loadincs.C
        ${PROJECT_SOURCE_DIR}/rootwc/loadlibs.C
        ${PROJECT_SOURCE_DIR}/rootwc/rootwc
        ${PROJECT_SOURCE_DIR}/rootwc/rootwc.C
        )

configure_file(cmake/this_wcsim.sh.in this_wcsim.sh)
pbuilder_install_files(${BIN_INSTALL_DIR} ${CMAKE_CURRENT_BINARY_DIR}/this_wcsim.sh)

#add_custom_target(install_${PROJECT_NAME}
#        "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install
#        COMMENT "Installing ${PROJECT_NAME}")
