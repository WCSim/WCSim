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




### Full scripts suite

An automated way to go from .mac file creation & job submission through to WCSim output analysis and histogram comparison. Steps are:
 * Creating a series of .mac files
 * Submitting jobs (either locally, or to a cluster)
 * Running a 'sample_readfile.C' style macro
 * Comparing the outputs of the 'sample_readfile.C' macro, either 
 * a) Identical files in different directories (e.g. checking the results before/after a change)
 * b) Different files in the same/different directory (e.g. checking how a new feature compares to an old feature)

## run_verification.py

This script has 3 run modes:
1. grabcode - checkout the appropriate branch of the 'new' code, and a clean copy of the current official develop code, and compile them
2. run - call generate_mac_files.py (with some defaults)
3. analyse - call analyse_dir.py and plot_lots.py

## generate_mac_files.py

This script will generate a series of .mac files for a particle gun for the set of options given, and submit the jobs to run (either locally, or on a cluster)
Specify position/direction with either 3 vectors, or the allowed options of $WCSIMDIR/sample-root-scripts/MakeKin.py
Creates subdirectories for each set of 'jobOptions.mac' options specified, and uses filenames to distinguish 'novis.mac' options
Note: not all possible options are included yet, but can be easily added

## analyse_dir.py

This script will run a given .C analysis script over all files in a given directory
The default macro is sample_readfile.C

## plot_lots.py

This script will glob over all files in a given directory, and create histograms comparing multiple other files using compare_lots.C
The other files either come from:
* The same directory; use --difference to specify differences e.g. noise rate
* A different directory
The 'multiple' files are specified with --diffplots. e.g. _0e-?_10e-:50e-_ will compare 0, 10, and 50 MeV electrons with identical other options
The histograms to compare are read from the file comparison_hists.py

## plot.py

Very similar to plot_lots.py, but only allows a 1 vs 1 comparison. Has a couple of extra run options:
* filesize - compares filesize and running time between 2 files (note runtime will only be valid if you used the 'local' runmode in generate_mac_files.py)
* compare_stacks - creates stacked histogram comparisons (currently only works for 2)
* compare - calls compare.C, which is very similar to compare_lots.C, just for exactly 2 files

### Usage

Setting up
* $WCSIMDIR must be setup to point to the folder where WCSim.cc resides
* $PATH should contain where rootwc lives e.g.:
export PATH=${WCSIMDIR}/rootwc:${PATH}
* The scripts use the argparse module, available in python 2.7. It is included in the directory for convenience

Running
* Run generate_mac_files.py to create WCSim output files
* Run analyse_dir.py to create the relevant histograms in an output file that can be compared
* If comparing histograms or stacks, modify comparison_hists.py to choose which histograms/stacks to compare
* If comparing histograms, run plot_lots.py to make the histograms
* If comparing speeds/sizes or stacks, run plot.py

### Example

Compare 5, 10, 50 MeV electrons for the SKDETSIM-like and new-style digitisation routines, with/without noise, submitting WCSim jobs locally
* python $WCSIMDIR/verification-test-scripts/full_scripts_suite/generate_mac_files.py --batchmode local --WCgeom SuperK --HKwatertanklength 24750 --PMTQEMethod Stacking_Only --SavePi0 false --DAQdigitizer SKI,SKI_SKDETSIM --DAQtrigger NHits,SKI_SKDETSIM --DAQnhitsthreshold 25 --DAQnhitsignorenoise --DAQnhitswindow 200 --DAQsavefailuresmode 0 --DAQsavefailurestime 250  --DarkNoiseRate 0,8.4 --DarkNoiseConvert 1.367 --DarkNoiseMode 1 --DarkNoiseWindow 2000 --GunParticle e- --GunEnergy 5,10,50 --GunPosition 0,0,0 --GunDirection 1,0,0 --DAQnhitsignorenoise --NEvents 2000
* python $WCSIMDIR/verification-test-scripts/full_scripts_suite/analyse_dir.py --dir .
* Compare like-for-like
 * python $WCSIMDIR/verification-test-scripts/full_scripts_suite/plot_lots.py --dir1 SKI_NHits_fails0_250_NHits25_200 --dir2 SKI_SKDETSIM_SKI_SKDETSIM_fails0_250_NHits25_200 --legend 'New_code:Old_code' --mode compare_lots
* Compare 5,10,50 MeV on the same plot
 * python $WCSIMDIR/verification-test-scripts/full_scripts_suite/plot_lots.py --dir1 SKI_NHits_fails0_250_NHits25_200 --dir2 SKI_SKDETSIM_SKI_SKDETSIM_fails0_250_NHits25_200 --legend 'New_5MeV:Old_5MeV:New_10MeV:Old_10MeV:New_10MeV:Old_10MeV' --mode compare_lots --diffplots _5e-:_10e-,_50e- --required_filename_part _5e-
* Compare speed/filesize
 * python /home/other/tdealtry/Documents/myWCSIM/WCSim/verification-test-scripts/full_scripts_suite/plot.py --dir1 SKI_NHits_fails0_250_NHits25_200 --dir2 SKI_SKDETSIM_SKI_SKDETSIM_fails0_250_NHits25_200 --legend 'New_code,Old_code' --mode filesize
