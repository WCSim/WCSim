# WCSimConfig.cmake

get_filename_component( WCSim_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH )

include( CMakeFindDependencyMacro )
find_dependency( Geant4 REQUIRED )
find_dependency( ROOT REQUIRED )

include("${WCSim_CMAKE_DIR}/WCSim_Library_Targets.cmake")
