# CMakeLists for src
# Author: M. Guigue
# Date: Jul 27, 2022

# Core library

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh ${PROJECT_SOURCE_DIR}/include/*.h)

# ###########
# # Library #
# ###########

pbuilder_library(
        TARGET WCSimCore
        SOURCES ${sources}
        PROJECT_LIBRARIES WCSimRoot
        PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
        PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXT_LIBS}
)


# WCSimRoot library

set (dict_headers
        ${PROJECT_SOURCE_DIR}/include/WCSimRootEvent.hh
        ${PROJECT_SOURCE_DIR}/include/WCSimRootGeom.hh
        ${PROJECT_SOURCE_DIR}/include/WCSimPmtInfo.hh
        ${PROJECT_SOURCE_DIR}/include/WCSimEnumerations.hh
        ${PROJECT_SOURCE_DIR}/include/WCSimRootOptions.hh
        ${PROJECT_SOURCE_DIR}/include/WCSimRootTools.hh
        ${PROJECT_SOURCE_DIR}/include/TJNuBeamFlux.hh
        ${PROJECT_SOURCE_DIR}/include/TNRooTrackerVtx.hh )

set( linkdef_header
        ${PROJECT_SOURCE_DIR}/include/WCSimRootLinkDef.hh )
set(sources_root
        WCSimRootEvent.cc
        WCSimRootGeom.cc
        WCSimPmtInfo.cc
        WCSimEnumerations.cc
        WCSimRootOptions.cc
        WCSimRootTools.cc
	TJNuBeamFlux.cc
	TNRooTrackerVtx.cc)
set( linkdef_header ${PROJECT_SOURCE_DIR}/include/WCSimRootLinkDef.hh )
set(pcm_file ${CMAKE_CURRENT_BINARY_DIR}/libWCSimRoot_rdict.pcm)
set(rootmap_file ${CMAKE_CURRENT_BINARY_DIR}/libWCSimRoot.rootmap)
set(sources ${sources} ${CMAKE_CURRENT_BINARY_DIR}/G__WCSimRoot.cxx)

ROOT_GENERATE_DICTIONARY(G__WCSimRoot ${dict_headers} LINKDEF ${linkdef_header})
pbuilder_install_files(${LIB_INSTALL_DIR} ${pcm_file} ${rootmap_file})
set(sources_root
        ${sources_root}
        G__WCSimRoot.cxx)

pbuilder_library(
        TARGET WCSimRoot
        SOURCES ${sources_root}
        PROJECT_LIBRARIES ${PACKAGE_LIBS}
        PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
        PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXT_LIBS}
)



pbuilder_component_install_and_export(
        COMPONENT Library
        LIBTARGETS WCSimCore WCSimRoot
)

pbuilder_install_headers(${headers})


##############
# Executable
##############

set (exe_sources
        ${PROJECT_SOURCE_DIR}/WCSim.cc
        )
set(exe_libraries
        WCSimCore WCSimRoot
        )

MESSAGE(${PUBLIC_EXT_LIBS})

pbuilder_executables(
        SOURCES ${exe_sources}
        TARGETS_VAR programs
        PROJECT_LIBRARIES ${exe_libraries}
        PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
        PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXT_LIBS}
)

pbuilder_component_install_and_export(
        COMPONENT Executables
        EXETARGETS ${programs}
)
##############
# WCSim compilation options
##############

target_compile_definitions(WCSimCore PRIVATE
        WCSIM_CHECK_GEOMETRY_OVERLAPS=${WCSIM_CHECK_GEOMETRY_OVERLAPS}
)
target_compile_definitions(WCSim PRIVATE
        WCSIM_CHECK_GEOMETRY_OVERLAPS=${WCSIM_CHECK_GEOMETRY_OVERLAPS}
)
