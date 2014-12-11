# Directory description

This directory contains scripts to test the output of your current version of WCSim with the version on GitHub. 
The purpose of these scripts is to ensure that the changes being committed will not accidentally change the output of the WCSim code. 

# Description of scripts in this directory

## verification_HitsChargeTime.C: 

This script will test a .root output file from your version of WCSim against the committed version of the code. 
This test plots the number of digitized hits, the average charge, and the average time. 
A ks test is printed to the screen. This test also prints to the screen a comparison of the first event in the files to see if the number of hit tubes, number of digitized tubes, and number of pe hit times match. 

### Usage
To use the verification_HitsChargeTime test in the default method, do the following steps:
* clone a copy of the current code from GitHub into a directory called WCSim_clean. Compile the code. 
* Go to WCSim_clean/verification-test-scripts and run electrontest.mac by typing: ../bin/Linux-g++/WCSim electrontest.mac . This should produce an output file called wcsimtest.root in the verification-test-scripts directory of WCSim_clean. 
* Go to the verification-test-scripts directory of your local WCSim (with your changes you wish to commit) and run the macro electrontest.mac in WCSim by typing ../bin/Linux-g++/WCSim electrontest.mac. This should produce a file called wcsimtest.root in your verification-test-scripts directory. 
* Still in the verification-test-scripts of your local WCSim directory, type: root verification_HitsChargeTime.C

You can also use this test to compare any two files from any two directories. To use the script this way type:
* root 
* .x verification_HitsChargeTime.C("/path/to/first_file", "/path/to/second_file")
