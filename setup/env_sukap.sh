#!/bin/bash

# ROOT version as argument, default value is 6 for ROOT6
ROOT_ARG=6

if [ ! -z "$1" ] ; then
	echo "ROOT version requested to be: ${1}"
	ROOT_ARG=$1
fi

############################
# CMAKE
############################

cmake --version

############################
# ROOT/CERN
############################

if [[ "$ROOT_ARG" == 5 ]] ; then 
	echo "Use ROOT5"
	# Note: official ROOT 5.34 installation on sukap is not compatible with cmake
	#  Temporary use Guillaume Pronost's version. Should change in the future.
	#
	export ROOT_DIR=/home/pronost/software/root-5.34.38-build
	export ROOT_STR=ROOT5
elif [[ "$ROOT_ARG" == 6 ]] ; then
	echo "Use ROOT6"
	# Note: There is no official ROOT 6 installation 
	#  Temporary use Guillaume Pronost's version. Should change in the future.
	export ROOT_DIR=/home/pronost/software/root-6.22.00-build
	export ROOT_STR=ROOT6
else
    echo "Unknown argument: $1"
    echo "Usage: env_sukap.sh [5|6]"
    echo "Defaults to using ROOT 6"
    return
fi
	
source ${ROOT_DIR}/bin/thisroot.sh

alias root='root -l'

############################
# GEANT4
############################

pwd=$PWD
GEANT4VERSION=geant4.10.05.p01
#alternative naming convention used by the share/ folder...
GEANT4VERSION_ALT=Geant4-10.5.1
cd /usr/local/sklib_gcc8/$GEANT4VERSION/bin/
source geant4.sh
cd /usr/local/sklib_gcc8/$GEANT4VERSION/share/$GEANT4VERSION_ALT/geant4make/
source geant4make.sh
echo "Using $GEANT4VERSION"
cd "$pwd"

############################
# WCSim
############################
export BRANCH_NAME=$(git rev-parse --abbrev-ref HEAD)
export WCSIMDIR=$(pwd)-build/$ROOT_STR/$BRANCH_NAME
export LD_LIBRARY_PATH=$WCSIMDIR:$LD_LIBRARY_PATH
export WCSIMREPO=$(pwd)
