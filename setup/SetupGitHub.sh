#!/bin/bash

#Check if connected to https or ssh
IsHTTPS=$(git remote -v | grep "https" | wc -l)

if [ "${IsHTTPS}"  -gt 0 ] ; then
	echo "Adding https Upstream to WCSim/WCSim"
	git remote add upstream https://github.com/WCSim/WCSim
else
	echo "Adding ssh Upstream to WCSim/WCSim"
	git remote add upstream git@github.com:WCSim/WCSim.git
fi
	
