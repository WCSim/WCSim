#!/bin/bash

#Check if connected to https or ssh
IsHTTPS=$(git remote -v | grep "https" | wc -l)

if [ "${IsHTTPS}"  -gt 0 ] ; then
	echo "Adding https Upstream to bquilain/WCSim"
	git remote add upstream https://github.com/bquilain/WCSim
else
	echo "Adding ssh Upstream to bquilain/WCSim"
	git remote add upstream git@github.com:bquilain/WCSim.git
fi
	
