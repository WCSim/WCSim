#!/bin/bash

source G4ROOTsource.sh

/usr/local/cmake-3.4.3/bin/cmake --version 

curpath=${PWD}

if [ ! -d ../WCSim_v1.8.0-build ]; then

	echo "Creating libWCSimRoot.so"
	rm -f src/*Dict*
	make shared
	rm -f src/*Dict*
	
	echo "Creating build directory ../WCSim_v1.8.0-build"
	mkdir -p ../WCSim_v1.8.0-build
	
	cd ../WCSim_v1.8.0-build
	/usr/local/cmake-3.4.3/bin/cmake -DCMAKE_PREFIX_PATH=${G4INSTALLDIR} $curpath
	
	# Add needed directory and files
	ln -s $curpath/include ../WCSim_v1.8.0-build/include
	cp $curpath/biasprofile.dat ../WCSim_v1.8.0-build/.
else 
	cd ../WCSim_v1.8.0-build
fi


if [ -d ../WCSim_v1.8.0-build ]; then
	rm -f src/*Dict*
	make clean
	make -j7

	cd $curpath
fi	
