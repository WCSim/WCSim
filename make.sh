#!/bin/bash

cmake3 --version

wcsim_name=${PWD##*/}
branch_name=$(git rev-parse --abbrev-ref HEAD)

wcsim_directory=${PWD}
build_directory=${wcsim_directory}/../${wcsim_name}-build/${ROOT_STR}/${branch_name}
install_directory=${wcsim_directory}/../${wcsim_name}-install/${ROOT_STR}/${branch_name}
# If ROOT_STR is not set, this will have no effect, if ROOT_STR is set, we can manage two or more different ROOT versions

if [ ! -d ${build_directory} ]; then
	
	# Clean G4
	if [ -d ${G4WORKDIR} ]; then
		rm -r ${G4WORKDIR}
	fi
	rm *.o *.a *.so *~ */*~ src/*Dict*
	
	echo "Creating build directory ${build_directory}"
	mkdir -p ${build_directory}

	echo "Creating install directory ${build_directory}"
	mkdir -p ${install_directory}
	
	cd ${build_directory}
	cmake3 -DCMAKE_PREFIX_PATH=${G4INSTALLDIR} -DCMAKE_INSTALL_PREFIX=${install_directory} ${wcsim_directory}
	
#	cp -r ${wcsim_directory}/sample-root-scripts ${build_directory}/.
else 
	cd ${build_directory}
fi


if [ -d ${build_directory} ]; then
	make clean
	make -j7 install
	
	cd ${wcsim_directory}
fi	
