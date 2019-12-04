#!/bin/bash
	
# Note: please modified the following path for your work:

export WCSIM_BASE_DIR=/disk02/usr6/pronost/HK/WCSim_v1.8.0
export G4WORKDIR=/disk02/usr6/pronost/software/geant4-workdir

# Connect a PATH of Geant4.10 for SKG4

#use correct cmake version
alias cmake='/usr/local/cmake-3.4.3/bin/cmake'

############################
# ROOT/CERN
############################

export ROOT_DIR=/disk02/usr6/pronost/software
source ${ROOT_DIR}/root-5.34.38-build/bin/thisroot.sh

alias root='root -l'

export LD_LIBRARY_PATH=${ROOTSYS}/lib:$LD_LIBRARY_PATH

############################
# GEANT4
############################

export GEANT4_DIR=/disk02/usr6/pronost/software
cur_dir=${PWD}
export PATH=$PATH:${G4WORKDIR}/bin/Linux-g++/
export G4SYSTEM=Linux-g++

GEANT4VERSION=geant4.10.03.p03

export G4INSTALL=${GEANT4_DIR}/${GEANT4VERSION}/
export G4INSTALLDIR=${GEANT4_DIR}/${GEANT4VERSION}-install/
cd ${G4INSTALLDIR}/bin
source geant4.sh
cd ${cur_dir}
	
############################
# WCSim
############################

export WCSIMDIR=${WCSIM_BASE_DIR}

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WCSIM_BASE_DIR
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WCSIM_BASE_DIR/src
export ROOT_INCLUDE_PATH=${WCSIM_BASE_DIR}/include
export PATH=$PATH:${WCSIM_BASE_DIR}-build

############################
# BONSAI
############################

export BONSAIDIR=/disk02/usr6/pronost/software/BonsaiHK
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BONSAIDIR

