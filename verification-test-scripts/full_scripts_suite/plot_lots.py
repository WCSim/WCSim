#!/usr/bin/env python

import glob, os, argparse, sys, time, re
from datetime import datetime
from datetime import timedelta
import comparison_hists as ch

delim_list = lambda s: s.split(',')

modes = ["compare_lots"]

parser = argparse.ArgumentParser(description='Analyse a directory of WCSim output files')
parser.add_argument('-d','--dir1',  type=str, help='First directory to analyse', default='./')
parser.add_argument('-i','--dir2',  type=str, help="Second directory to analyse (don't give to look at 'differnce' files in same directory)", default='')
parser.add_argument('--difference', type=delim_list, help='When running without dir2, look for 2 files in dir1 that are identical but for this difference')
parser.add_argument('--diffplots',  type=str, help='When running without dir2, look for 2 files in dir1 that are identical but for this difference')
parser.add_argument('--legend',     type=str, help='Overwrite the legend labels')
parser.add_argument('-m', '--mode', type=str, help='Run mode. Possibilities: ' + " ".join(m for m in modes), required=True)
args = parser.parse_args()

if args.mode not in modes:
    print "Mode should be one of " + " ".join(m for m in modes)
    sys.exit(1)

if not args.dir2 and not args.difference:
    print "Must provide either a difference or a second directory"
    sys.exit(1)
"""
if args.difference and len(args.difference) % 2:
    print "Difference should be a comma separated list with an even number of entries"
    sys.exit(1)
if args.legend and len(args.legend) != 2:
    if args.mode == "compare_stacks" and len(args.legend) % 2:
        print "Legend should be a comma separated list with an even number of entries"
        sys.exit(1)
    else:
        print "Legend should be a comma separated list with exactly 2 entries"
        sys.exit(1)
"""
    
try:
    os.chdir(args.dir1)
except:
    print "Cannot cd to directory", args.dir1, "exiting..."
    sys.exit(1)
print "cd'ed to", args.dir1

#construct the string to glob over
glob_prefix = "analysed_wcsim*"
if(args.mode == "filesize"):
    glob_prefix = "wcsim_*"
glob_postfix = "*.root"

globber = glob_prefix + glob_postfix
if not args.dir2:
    globber = glob_prefix + args.difference[0] + glob_postfix

if args.mode == "filesize":
    fout  = open("filesizes.txt", "w")
    fout2 = open("filespeed.txt", "w")
    
#loop over the files in directory
print globber
for file1 in glob.glob(globber):
    #construct the 2nd filename
    if args.dir2:
        file2 = "../" + args.dir2 + '/' + file1
        file2temp = file2
    else:
        file2temp = file1
    files = file1
    if args.difference:
        for i in xrange(0, len(args.difference), 2):
            file2temp = file2temp.replace(args.difference[i], args.difference[i+1])
        file2 = file2temp
    if args.diffplots:
        for diffs in args.diffplots.split('?'):
            orig = diffs.split(':')[0]
            for d in diffs.split(':')[1].split(','):
                file3temp = file2
                print file3temp
                print "Replacing", orig, "with", d
                file3temp = file3temp.replace(orig, d)
                if os.path.isfile(file3temp):
                    files = files + '$' + file3temp
                else:
                    print "Could not find file", file3temp
    else:
        files = files + "$" + file2

    #continue if we can't find it
    if len(files.split('$')) != len(args.legend.split(':')):
        print 'Different number of files and legends found'
        for f in files.split('$'):
            print f
        for l in args.legend.split(':'):
            print l
        sys.exit(1)
        #continue

    filestub = file1.split('_')[2] + file1.split('_')[-1].rsplit('.',1)[0]

    #now we have the files to compare, run the relevant root macro(s)
    if args.mode == "compare_lots":
        """
        if args.legend:
            leg1 = args.legend[0]
            leg2 = args.legend[1]
        elif args.dir2:
            leg1 = args.dir1
            leg2 = args.dir2
        else:
            leg1 = args.difference[0]
            leg2 = args.difference[1]
        """
        hists_analysed = []
        for histname,low,high,rebin in ch.hists:
            hists_analysed.append(histname)
            filestubout = filestub + "_" + str(hists_analysed.count(histname))
            command = 'rootwc -l -b -q \'$WCSIMDIR/verification-test-scripts/full_scripts_suite/compare_lots.C("' + histname + '",' + str(low) + ',' + str(high) + ',"' + files + '","' + args.legend + '","' + filestubout +'",' + str(rebin) + ')\''
            print command
            os.system(command)
