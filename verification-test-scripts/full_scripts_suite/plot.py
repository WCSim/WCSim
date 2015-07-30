#!/usr/bin/env python

import glob, os, argparse, sys, time, re
from datetime import datetime
from datetime import timedelta
import comparison_hists as ch

delim_list = lambda s: s.split(',')

modes = ["filesize","compare","compare_stacks","verification_HitsChargeTime"]

parser = argparse.ArgumentParser(description='Analyse a directory of WCSim output files')
parser.add_argument('-d','--dir1', type=str, help='First directory to analyse', default='./')
parser.add_argument('-i','--dir2', type=str, help="Second directory to analyse (don't give to look at 'differnce' files in same directory)", default='')
parser.add_argument('--difference', type=delim_list, help='When running without dir2, look for 2 files in dir1 that are identical but for this difference')
parser.add_argument('--legend', type=delim_list, help='Overwrite the legend labels')
parser.add_argument('-m', '--mode', type=str, help='Run mode. Possibilities: ' + " ".join(m for m in modes), required=True)
args = parser.parse_args()

if args.mode not in modes:
    print "Mode should be one of" + " ".join(m for m in modes)
    sys.exit(1)
if not args.dir2 and not args.difference:
    print "Must provide either a difference or a second directory"
    sys.exit(1)
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
    
try:
    os.chdir(args.dir1)
except:
    print "Cannot cd to directory", args.dir1, "exiting..."
    sys.exit(1)
print "cd'ed to", args.dir1

#construct the string to glob over
glob_prefix = "analysed_wcsim_*"
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
for file1 in glob.glob(globber):
    #construct the 2nd filename
    if args.dir2:
        file2 = "../" + args.dir2 + '/' + file1
        file2temp = file2
    else:
        file2temp = file1
    if args.difference:
        for i in xrange(0, len(args.difference), 2):
            file2temp = file2temp.replace(args.difference[i], args.difference[i+1])
        file2 = file2temp
    #continue if we can't find it
    if not os.path.isfile(file2):
        print file2, "not found. Continuing"
        continue
    filestub = file1.split('_')[2] + file1.split('_')[-1].rsplit('.',1)[0]

    #now we have the 2 files to compare, run the relevant root macro(s)
    if args.mode == "compare":
        if args.legend:
            leg1 = args.legend[0]
            leg2 = args.legend[1]
        elif args.dir2:
            leg1 = args.dir1
            leg2 = args.dir2
        else:
            leg1 = args.difference[0]
            leg2 = args.difference[1]
        hists_analysed = []
        for histname,low,high,rebin in ch.hists:
            hists_analysed.append(histname)
            filestubout = filestub + "_" + str(hists_analysed.count(histname))
            command = 'rootwc -l -b -q \'$WCSIMDIR/verification-test-scripts/full_scripts_suite/compare.C("' + histname + '",' + str(low) + ',' + str(high) + ',"' + file1 + '","' + file2 + '","' + leg1 + '","' + leg2 + '","' + filestubout +'",' + str(rebin) + ')\''
            print command
            os.system(command)

    elif args.mode == "compare_stacks":
        if args.legend:
            leg1 = args.legend[0]
            leg2 = args.legend[1]
        elif args.dir2:
            leg1 = args.dir1
            leg2 = args.dir2
        else:
            leg1 = args.difference[0]
            leg2 = args.difference[1]
        hists_analysed = []
        for stackname,histname1,histname2,low,high,rebin,legx1,legx2 in ch.stacks:
            hists_analysed.append(stackname)
            filestubout = filestub + "_" + str(hists_analysed.count(stackname))
            command = 'rootwc -l -b -q \'$WCSIMDIR/verification-test-scripts/full_scripts_suite/compare_stacks.C("' + stackname + '","' + histname1 + '","' + histname2 + '",' + str(low) + ',' + str(high) + ',"' + file1 + '","' + file2 + '","' + leg1+legx1 + '","' + leg1+legx2 + '","' + leg2+legx1+ '","' + leg2+legx2 + '","' + filestubout +'",' + str(rebin) + ')\''
            print command
            os.system(command)
            
    elif args.mode == "filesize":
        tempsizefile = 'tempsize.txt'
        command = 'rootwc -l -b -q \'$WCSIMDIR/verification-test-scripts/full_scripts_suite/printSizes.C("' + file1 + '","wcsimT",true)\' > ' + tempsizefile + ' \n' \
            'rootwc -l -b -q \'$WCSIMDIR/verification-test-scripts/full_scripts_suite/printSizes.C("' + file2 + '","wcsimT",true)\' >> ' + tempsizefile
        print command
        os.system(command)
        #get the results
        time.sleep(1)
        f = open(tempsizefile,"r")
        sizes = []
        for line in f.readlines():
            if line.startswith("TREESIZE"):
                sizes.append(int(line.split()[1]))
        f.close()
        os.remove(tempsizefile)

        #fout.write(file1)
        #fout2.write(file1)
        energy = file1.split('_')[1]
        energy = re.split("[a-zA-Z]", energy)[0]

        result = "{3} & {0:.2f} & {1:.2f} & {2:.2f}\\% \\\\ \n".format(sizes[0] / 1E6, sizes[1] / 1E6, ((sizes[1] / float(sizes[0])) * 100) - 100, energy)
        fout.write(result)

        mac1 = file1.replace(".root",".mac")
        mac2 = file2.replace(".root",".mac")
        #print datetime.fromtimestamp(os.path.getmtime(file1)), datetime.fromtimestamp(os.path.getmtime(mac1))
        #print datetime.fromtimestamp(os.path.getmtime(file2)), datetime.fromtimestamp(os.path.getmtime(mac2))
        tdelta1 = datetime.fromtimestamp(os.path.getmtime(file1)) - datetime.fromtimestamp(os.path.getmtime(mac1))
        tdelta2 = datetime.fromtimestamp(os.path.getmtime(file2)) - datetime.fromtimestamp(os.path.getmtime(mac2))
        def total_seconds(td):
            return (td.microseconds + (td.seconds + td.days * 24 * 3600) * 10**6) / 10**6
        ts1 = total_seconds(tdelta1)
        ts2 = total_seconds(tdelta2)
        result = "{3} & {0} & {1} & {2:.2f}\\% \\\\ \n".format(ts1, ts2, ((ts2 / float(ts1)) * 100) - 100, energy)
        fout2.write(result)

    elif args.mode == 'verification_HitsChargeTime':
        command = 'rootwc -l -b -q \'$WCSIMDIR/verification-test-scripts/verification_HitsChargeTime.C("' + file1 + '","' + file2 + '")\''
        print command
        os.system(command)


if args.mode == "filesize":
    fout.close()
    fout2.close()
