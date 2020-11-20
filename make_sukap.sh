#!/bin/bash

source env_sukap.sh

/usr/local/cmake-3.4.3/bin/cmake --version 

curpath=${PWD}

name=WCSim_hybridPMT

if [ ! -d ../${name}-build ]; then
	
	# Clean G4
	make clean_wcsim
	rm -f src/*Dict*
	
	echo "Creating build directory ../${name}-build"
	mkdir -p ../${name}-build
	
	cd ../${name}-build
	/usr/local/cmake-3.4.3/bin/cmake -DCMAKE_PREFIX_PATH=${G4INSTALLDIR} $curpath
	
	# Add needed directory
	ln -s $curpath/include ../${name}-build/include
else 
	cd ../${name}-build
fi


if [ -d ../${name}-build ]; then
	make clean
	make -j7
	
	cd $curpath
fi	
