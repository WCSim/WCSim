# PackageBuilder.cmake
# Author: Noah Oblath
#
# Default settings and macros for PackageBuilder builds
#
# Parts of this script are based on work done by Sebastian Voecking and Marco Haag in the Kasper package
#
# Requires: CMake v3.12 or better (FindPython3)

# CMake policies
cmake_policy( SET CMP0011 NEW )
cmake_policy( SET CMP0012 NEW ) # how if-statements work
cmake_policy( SET CMP0042 NEW ) # rpath on mac os x
cmake_policy( SET CMP0048 NEW ) # version in project()

include ( PackageBuilderMacros )

include( CMakeParseArguments ) # required until cmake v3.5, when this was added as a built-in command

# check if this is a stand-alone build
set( PBUILDER_STANDALONE FALSE CACHE INTERNAL "Flag for whether or not this is a stand-alone build" )
set( PBUILDER_CHILD_NAME_EXTENSION "${PROJECT_NAME}" CACHE INTERNAL "Submodule library name modifier" )
if( ${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR} )
    set( PBUILDER_STANDALONE TRUE )
    
    if( CMAKE_GENERATOR MATCHES ".*(Make|Ninja).*" AND NOT CMAKE_BUILD_TYPE )
  		set( CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel" FORCE )
  		message( STATUS "CMAKE_BUILD_TYPE not specified. Using ${CMAKE_BUILD_TYPE} build" )
    endif()

    # option to force linking when using g++
    if( CMAKE_COMPILER_IS_GNUCXX )
        option( GCC_FORCE_LINKING "Fix linker errors with some GCC versions by adding the --no-as-needed flag" ON )
        if( GCC_FORCE_LINKING )
            set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed" )
        endif( GCC_FORCE_LINKING )
    endif( CMAKE_COMPILER_IS_GNUCXX )
endif( ${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR} )

# define a variable pointing to the directory containing this file
set( PBUILDER_DIR ${CMAKE_CURRENT_LIST_DIR} )

# preprocessor defintion for debug build
if( "${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG" )
    add_definitions(-D${PROJECT_NAME}_DEBUG )
else( "${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG" )
    remove_definitions(-D${PROJECT_NAME}_DEBUG )    
endif( "${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG" )

message( STATUS "Build type: ${CMAKE_BUILD_TYPE}" )

# Setup the default install prefix
# This gets set to the binary directory upon first configuring.
# If the user changes the prefix, but leaves the flag OFF, then it will remain as the user specified.
# If the user wants to reset the prefix to the default (i.e. the binary directory), then the flag should be set ON.
if( NOT DEFINED SET_INSTALL_PREFIX_TO_DEFAULT )
    set( SET_INSTALL_PREFIX_TO_DEFAULT ON )
endif( NOT DEFINED SET_INSTALL_PREFIX_TO_DEFAULT )
if( SET_INSTALL_PREFIX_TO_DEFAULT )
    set( CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR} CACHE PATH "Install prefix" FORCE )
    set( SET_INSTALL_PREFIX_TO_DEFAULT OFF CACHE BOOL "Reset default install path when when configuring" FORCE )
endif( SET_INSTALL_PREFIX_TO_DEFAULT )

# install subdirectories
set( INCLUDE_INSTALL_SUBDIR "include/${PROJECT_NAME}" CACHE PATH "Install subdirectory for headers" )
if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
    set( LIB_INSTALL_SUBDIR "bin" CACHE PATH "Install subdirectory for libraries" )
else( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
    set( LIB_INSTALL_SUBDIR "lib" CACHE PATH "Install subdirectory for libraries" )
endif( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
set( PACKAGE_CONFIG_SUBDIR "${LIB_INSTALL_SUBDIR}/cmake/${PROJECT_NAME}" CACHE PATH "Install subdirectory for CMake config files" )
set( BIN_INSTALL_SUBDIR "bin" CACHE PATH "Install subdirectory for binaries" )
set( CONFIG_INSTALL_SUBDIR "config" CACHE PATH "Install subdirectory for config files" )
set( DATA_INSTALL_SUBDIR "data" CACHE PATH "Install subdirectory for data files" )

set( INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${INCLUDE_INSTALL_SUBDIR}" )
set( LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_SUBDIR}" )
set( PACKAGE_CONFIG_PREFIX "${CMAKE_INSTALL_PREFIX}/${PACKAGE_CONFIG_SUBDIR}" )
set( BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_SUBDIR}" )
set( CONFIG_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CONFIG_INSTALL_SUBDIR}" )
set( DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_SUBDIR}" )

#if( NOT DEFINED TOP_PROJECT_INCLUDE_INSTALL_DIR )
if( PBUILDER_STANDALONE )
    # this is for standalone builds, but has to be done down here rather than in the standalone if block above
    set( TOP_PROJECT_INCLUDE_INSTALL_DIR "${INCLUDE_INSTALL_DIR}" CACHE INTERNAL "Top-project include installation path" )
    set( TOP_PROJECT_INCLUDE_INSTALL_SUBDIR "${INCLUDE_INSTALL_SUBDIR}" CACHE INTERNAL "Top-project include installation subdirectory" )
    message( STATUS "TOP_PROJECT_INCLUDE_INSTALL_DIR being set to ${TOP_PROJECT_INCLUDE_INSTALL_DIR}" )

    set( TOP_PROJECT_CMAKE_CONFIG_DIR "${PACKAGE_CONFIG_PREFIX}" CACHE INTERNAL "Top-project CMake config installation path" )
    message( STATUS "TOP_PROJECT_CMAKE_CONFIG_DIR being set to ${TOP_PROJECT_CMAKE_CONFIG_DIR}" )
    set( ${PROJECT_NAME}_CMAKE_CONFIG_DIR "${PACKAGE_CONFIG_PREFIX}" CACHE INTERNAL "${PROJECT_NAME} CMake config installation path" )
    message( STATUS "${PROJECT_NAME}_CMAKE_CONFIG_DIR being set to ${${PROJECT_NAME}_CMAKE_CONFIG_DIR}" )
endif()


# flag for building test programs
option( ${PROJECT_NAME}_ENABLE_TESTING "Turn on or off the building of test programs" OFF )

# flag for building executables (other than test programs)
# this is particularly useful if a project is used multiple times and installed in a general location, where executables would overwrite each other.
option( ${PROJECT_NAME}_ENABLE_EXECUTABLES "Turn on or off the building of executables (other than test programs)" ON )

# default version of C++
# acceptable values are any used by the CXX_STANDARD property of your CMake
set( CMAKE_CXX_STANDARD 11 )

# build shared libraries
set( BUILD_SHARED_LIBS ON )

# turn on RPATH for Mac OSX
set( CMAKE_MACOSX_RPATH ON )

# add the library install directory to the rpath
SET(CMAKE_INSTALL_RPATH "${LIB_INSTALL_DIR}" )

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

set( LIB_POSTFIX )
set( INC_PREFIX )

# in windows, disable the min and max macros
if( WIN32 )
    # disable the min and max macros
    add_definitions( -DNOMINMAX )
	# libraries and linking
	set( CMAKE_SHARED_LIBRARY_PREFIX "" )
	set( CMAKE_IMPORT_LIBRARY_PREFIX "" )
	set( CMAKE_LINK_DEF_FILE_FLAG "" )
	add_definitions( -DBOOST_ALL_DYN_LINK )
endif( WIN32 )

# default package name is the project name
set( ${PROJECT_NAME}_PACKAGE_NAME "${PROJECT_NAME}" )

# Full project name, expanded according to the submodule hierarchy
pbuilder_expand_lib_name( ${PROJECT_NAME} )
set( ${PROJECT_NAME}_FULL_PROJECT_NAME ${FULL_LIB_NAME} )
message( STATUS "Full project name: ${PROJECT_NAME}_FULL_PROJECT_NAME = ${${PROJECT_NAME}_FULL_PROJECT_NAME}" )

# if git is used, get the commit SHA1
find_package( Git )
if( GIT_FOUND )
    # check whether this is a git repo
    execute_process( COMMAND ${GIT_EXECUTABLE} rev-parse --is-inside-git-dir WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE IS_GIT_REPO )
    if( IS_GIT_REPO )
        execute_process( COMMAND ${GIT_EXECUTABLE} rev-parse -q HEAD  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE ${PROJECT_NAME}_GIT_COMMIT  OUTPUT_STRIP_TRAILING_WHITESPACE )
        execute_process( COMMAND ${GIT_EXECUTABLE} describe --tags --long  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE ${PROJECT_NAME}_GIT_DESCRIBE  OUTPUT_STRIP_TRAILING_WHITESPACE )
        execute_process( COMMAND ${GIT_EXECUTABLE} remote get-url origin WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE GIT_ORIGIN OUTPUT_STRIP_TRAILING_WHITESPACE )
        if( GIT_ORIGIN )
            message( STATUS "Git origin: ${GIT_ORIGIN}")
            string( REGEX MATCH "[a-zA-Z0-9_-]+/[a-zA-Z0-9_-]+$" GIT_PACKAGE ${GIT_ORIGIN} )
        else( GIT_ORIGIN )
            execute_process( COMMAND ${GIT_EXECUTABLE} rev-parse --show-toplevel WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE TOP_DIR OUTPUT_STRIP_TRAILING_WHITESPACE )
            execute_process( COMMAND basename ${TOP_DIR} OUTPUT_VARIABLE GIT_PACKAGE )
        endif( GIT_ORIGIN )
        message( STATUS "Git package: ${GIT_PACKAGE}" )

        # override package name with git package
        set( ${PROJECT_NAME}_PACKAGE_NAME "${GIT_PACKAGE}" )
    endif( IS_GIT_REPO )
endif( GIT_FOUND )
# check if we set the _PACKAGE_NAME variable; if we didn't, then we'll use an alternate setting
if( NOT ${PROJECT_NAME}_PACKAGE_NAME )
    set( ${PROJECT_NAME}_PACKAGE_NAME "${PROJECT_NAME}" )
endif()

# define the variables to describe the package (will go in the [ProjectName]Config.hh file)
set( ${PROJECT_NAME}_PACKAGE_STRING "${PROJECT_NAME} ${${PROJECT_NAME}_VERSION}" )
