#!/bin/bash

source env_sukap_ROOT5.sh

/usr/local/cmake-3.18.1/bin/cmake --version 

wcsim_name=${PWD##*/}
branch_name=$(git rev-parse --abbrev-ref HEAD)

wcsim_directory=${PWD}
build_directory=${wcsim_directory}/../${wcsim_name}-build/ROOT5/${branch_name}

if [ ! -d ${build_directory} ]; then
	
	# Clean G4
	if [ -d ${G4WORKDIR} ]; then
		rm -r ${G4WORKDIR}
	fi
	rm *.o *.a *.so *~ */*~ *Dict*
	
	echo "Creating build directory ${build_directory}"
	mkdir -p ${build_directory}
	
	cd ${build_directory}
	/usr/local/cmake-3.18.1/bin/cmake -DCMAKE_PREFIX_PATH=${G4INSTALLDIR} ${wcsim_directory}
	
	cp -r ${wcsim_directory}/sample-root-scripts ${build_directory}/.
else 
	cd ${build_directory}
fi


if [ -d ${build_directory} ]; then
	make clean
	make -j7
	
	cd ${wcsim_directory}
fi	
