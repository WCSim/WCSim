# CMakeLists for src
# Author: M. Guigue
# Date: Jul 27, 2022

# Core library

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)


# If there were other libraries in this package on which this library depends, then they would be put in this variable
#set(PACKAGE_LIBS )
#
#set( CORE_LINKDEF_HEADERFILE WCSimRootLinkDef.hh )
#set( sources ${sources} )
#set( CORE_DICT_PCMFILE ${CMAKE_CURRENT_BINARY_DIR}/CCoreDict_rdict.pcm )
#
###################################################
#set( EVENT_SOURCEFILES ${EVENT_SOURCEFILES} ${CMAKE_CURRENT_BINARY_DIR}/CCoreDict.cxx )
#ROOT_GENERATE_DICTIONARY( CCoreDict ${headers} LINKDEF ${CORE_LINKDEF_HEADERFILE}  OPTIONS -inlineInputHeader )
#
#pbuilder_install_files(${LIB_INSTALL_DIR} ${CORE_DICT_PCMFILE})

# ###########
# # Library #
# ###########

pbuilder_library(
        TARGET WCSimCore
        SOURCES ${sources}
        PROJECT_LIBRARIES ${PACKAGE_LIBS}
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
        ${PROJECT_SOURCE_DIR}/include/WCSimRootTools.hh )

set( CORE_LINKDEF_HEADERFILE ${PROJECT_SOURCE_DIR}/include/WCSimRootLinkDef.hh )
#set( sources ${sources} )
set(sources_root
        WCSimRootEvent.cc
        WCSimRootGeom.cc
        WCSimPmtInfo.cc
        WCSimEnumerations.cc
        WCSimRootOptions.cc
        WCSimRootTools.cc)
set( CORE_LINKDEF_HEADERFILE ${PROJECT_SOURCE_DIR}/include/WCSimRootLinkDef.hh )
set(CORE_DICT_PCMFILE ${CMAKE_CURRENT_BINARY_DIR}/libWCSimRoot_rdict.pcm)
set(CORE_DICT_ROOTMAPFILE ${CMAKE_CURRENT_BINARY_DIR}/libWCSimRoot.rootmap)
set(sources ${sources} ${CMAKE_CURRENT_BINARY_DIR}/G__WCSimRoot.cxx)
#set( CORE_DICT_PCMFILE ${CMAKE_CURRENT_BINARY_DIR}/WCSimDict_rdict.pcm )
#
###################################################
#set( EVENT_SOURCEFILES ${EVENT_SOURCEFILES} ${CMAKE_CURRENT_BINARY_DIR}/CCoreDict.cxx )

ROOT_GENERATE_DICTIONARY(G__WCSimRoot ${dict_headers} LINKDEF ${CORE_LINKDEF_HEADERFILE})
#ROOT_GENERATE_DICTIONARY( WCSimDict ${headers} LINKDEF ${CORE_LINKDEF_HEADERFILE}  OPTIONS -inlineInputHeader )

pbuilder_install_files(${CMAKE_INSTALL_PREFIX}/lib ${CORE_DICT_PCMFILE} ${CORE_DICT_ROOTMAPFILE})
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

