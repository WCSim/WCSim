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
* clone a copy of the current code from GitHub into a directory called `WCSim_clean`. Compile the code. 
* Go to `WCSim_clean/verification-test-scripts` and run `electrontest.mac` by typing: `../bin/Linux-g++/WCSim electrontest.mac`. This should produce an output file called `wcsimtest.root` in the `verification-test-scripts` directory of `WCSim_clean`. 
* Go to the `verification-test-scripts` directory of your local WCSim (with your changes you wish to commit) and run the macro `electrontest.mac` in WCSim by typing `../bin/Linux-g++/WCSim electrontest.mac`. This should produce a file called `wcsimtest.root` in your `verification-test-scripts` directory. 
* Still in the `verification-test-scripts` of your local WCSim directory, type: `root verification_HitsChargeTime.C`

You can also use this test to compare any two files from any two directories. To use the script this way type:
* root 
* .x verification_HitsChargeTime.C("/path/to/first_file", "/path/to/second_file")

## verification-test-scripts/complete_comparison.C

This script will test a .root output file from your version of WCSim against the committed version of the code.
All variables within WCSimRootEvent are tested

### Usage
To use the complete_comparison test in the default method, do the following steps:
* clone a copy of the current code from GitHub into a directory called WCSim_clean. Compile the code. 
* Go to WCSim_clean/verification-test-scripts and run electrontest.mac by typing: ../bin/Linux-g++/WCSim electrontest.mac . This should produce an output file called wcsimtest.root in the verification-test-scripts directory of WCSim_clean. 
* Go to the verification-test-scripts directory of your local WCSim (with your changes you wish to commit) and run the macro electrontest.mac in WCSim by typing ../bin/Linux-g++/WCSim electrontest.mac. This should produce a file called wcsimtest.root in your verification-test-scripts directory. 
* Still in the verification-test-scripts of your local WCSim directory, type: root complete_comparison.C

You can also use this test to compare any two files from any two directories. To use the script this way type:
* root 
* .x complete_comparison.C("/path/to/first_file", "/path/to/second_file")

## printSizes.C:

This script will test a .root output file from your version of WCSim against the committed version of the code.
This test prints to the screen the size of the output trees and top-level branches.
The optional `verbose` boolean flag allows sub-branch sizes to be printed.

### Usage
To use the printSizes test in the default method, do the following steps (note that the first 3 steps are equvialent to that for `verification_HitsChargeTime.C`):
* clone a copy of the current code from GitHub into a directory called `WCSim_clean`. Compile the code. 
* Go to `WCSim_clean/verification-test-scripts` and run `electrontest.mac` by typing: `../bin/Linux-g++/WCSim electrontest.mac`. This should produce an output file called `wcsimtest.root` in the `verification-test-scripts` directory of `WCSim_clean`. 
* Go to the `verification-test-scripts` directory of your local WCSim (with your changes you wish to commit) and run the macro `electrontest.mac` in WCSim by typing `../bin/Linux-g++/WCSim electrontest.mac`. This should produce a file called `wcsimtest.root` in your `verification-test-scripts` directory. 
* Still in the `verification-test-scripts` of your local WCSim directory, type: `root printSizes.C`

You can also use this test to compare any two files from any two directories. To use the script this way type:
* `root` 
* `.x printSizes.C("/path/to/first_file", "/path/to/second_file",verbose)`


## `scan.py`
It can sometimes be useful to generate lots of similar mac files with physics starting at different positions (e.g. to find geometry overlaps/gaps).
This script allows you to do so, by setting up multiple mac files that scan
* Particle pointed into the barrel wall (towall == dwall) for various `r` and `theta`
* Particle pointed into the cap, with lines in a star, going across lines of `x=0`, `y=0`, `x=y`, `x=-y`, for both top & bottom caps
* Particle pointed at an angle into the "corner" between cap & barrel, for various azimuthal angles

### Usage
* Setup `base.mac` to include the physics you want (e.g. particle type & energy)
  * You may want to setup `base.mac` to include all options (geometry, dark rate, trigger, electronics, number of events to simulate, etc.)
  * Otherwise, your "main" `.mac` file should include a line like `/control/execute path/to/physics.mac`
* Read `scan.py`. Pay particular note to
  * `det_half_z`, `det_r`, and the `extreme` versions - these tell you the starting positions; make sure they are appropriate for your detector (currently setup for potentially old HK FD dimensions)
  * `nz`, `nr`, `ntheta` specify the resolution of the *imaging*
  * Add additional loops if required
