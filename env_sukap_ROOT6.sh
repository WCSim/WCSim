#!/bin/bash

############################
# ROOT/CERN
############################
# Note: official ROOT 5.34 installation on sukap is not compatible with cmake
#  Temporary use Guillaume Pronost's version. Should change in the future.
#
#export ROOT_DIR=/disk02/usr6/pronost/software/root-5.34.38-build
# Note: There is no official ROOT 6 installation 
#  Temporary use Guillaume Pronost's version. Should change in the future.
#  WARNING: This version is quite slow currently, investigation on-going
export ROOT_DIR=/disk02/usr6/pronost/software/root6-build
source ${ROOT_DIR}/bin/thisroot.sh

alias root='root -l'

############################
# GEANT4
############################

pwd=$PWD
cd /usr/local/geant4.10/geant4.10.03.p02/bin/
source geant4.sh
cd /usr/local/geant4.10/geant4.10.03.p02/share/Geant4-10.3.2/geant4make/
source geant4make.sh
echo "Connect a PATH of Geant4.10"
cd "$pwd"

############################
# WCSim
############################
export BRANCH_NAME=$(git rev-parse --abbrev-ref HEAD)
export WCSIMDIR=$(pwd)-build/ROOT6/$BRANCH_NAME
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WCSIMDIR

