#!/usr/bin/env python

import glob, os, argparse, sys

parser = argparse.ArgumentParser(description='Analyse a directory of WCSim output files')
parser.add_argument('-m','--macroname', type=str, default='$WCSIMDIR/verification-test-scripts/full_scripts_suite/daq_readfile.C', help='Default: $WCSIMDIR/verification-test-scripts/full_scripts_suite/daq_readfile.C')
parser.add_argument('-d','--dir', type=str, help='Directory to analyse', default='./')
parser.add_argument('-n','--nevents', type=int, help='Max number of events to analyse', default=9999999999)
parser.add_argument('-v','--verbose', action='store_true', help='Run verbosely?')
parser.add_argument('-p','--pdfs', action='store_true', help='Create simple pdfs of (most of) the histograms created?')
parser.add_argument('--histeveryevent', action='store_true', help='Make hit time histograms for every event? SLOW')
parser.add_argument('-f','--onlyonefile', action='store_true', help='Run only one file')
args = parser.parse_args()

def py_bool_to_string(arg):
    if arg:
        return "true"
    else:
        return "false"
verbose        = py_bool_to_string(args.verbose)
histeveryevent = py_bool_to_string(args.histeveryevent)
pdfs           = py_bool_to_string(args.pdfs)

try:
    os.chdir(args.dir)
except:
    print "Cannot cd to directory", args.dir, "exiting..."
    sys.exit(1)
print "cd'ed to", args.dir

for file1 in glob.glob("wcsim_*.root"):
    command = "rootwc '" + args.macroname + "+(\"" + file1 + "\"," + verbose + "," + str(args.nevents) + "," + pdfs + "," + histeveryevent + ")' -l -b -q"
    print command
    os.system(command)
    if args.onlyonefile:
        break
