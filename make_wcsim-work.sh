#!/bin/bash

curpath=${PWD}

if [ ! -d ../WCSim-work ]; then
	echo "Creating work directory ../WCSim-work"
	mkdir -p ../WCSim-work
	
	ln -s $curpath/data    						../WCSim-work/data
	ln -s $curpath/include 						../WCSim-work/include
	
	cp $curpath/biasprofile.dat ../WCSim-work/.
	
	ln -s $curpath/../WCSim_v1.8.0-build/geofile.txt 		../WCSim-work/geofile.txt
	#ln -s $curpath/../WCSim_v1.8.0-build/libWCSimRootDict_rdict.pcm ../WCSim-work/libWCSimRootDict_rdict.pcm
	#ln -s $curpath/../WCSim_v1.8.0-build/libWCSimRootDict.rootmap	../WCSim-work/libWCSimRootDict.rootmap
	ln -s $curpath/../WCSim_v1.8.0-build/libWCSimRoot.so		../WCSim-work/libWCSimRoot.so
	
	
	#cp $curpath/src/WCSimRootDict_rdict.pcm			../WCSim-work/WCSimRootDict_rdict.pcm
	
	ln -s /disk02/usr6/pronost/HK/WCSim-work/output			../WCSim-work/output
	ln -s /disk02/usr6/pronost/HK/WCSim-work/macros			../WCSim-work/macros
	ln -s /disk02/usr6/pronost/HK/WCSim-work/analysis		../WCSim-work/analysis
	ln -s /disk02/usr6/pronost/HK/WCSim-work/scripts		../WCSim-work/scripts
fi

