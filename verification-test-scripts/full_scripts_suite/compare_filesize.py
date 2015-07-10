#!/usr/bin/env python

import glob, os, argparse, sys, time, re
from datetime import datetime
from datetime import timedelta

parser = argparse.ArgumentParser(description='Analyse a directory of WCSim output files')
parser.add_argument('-d','--dir1', type=str, help='First directory to analyse', default='./')
parser.add_argument('-i','--dir2', type=str, help='Second directory to analyse', default='./')
args = parser.parse_args()

try:
    os.chdir(args.dir1)
except:
    print "Cannot cd to directory", args.dir1, "exiting..."
    sys.exit(1)
print "cd'ed to", args.dir1

fout  = open("filesizes.txt", "w")
fout2 = open("filespeed.txt", "w")
for file1 in glob.glob("wcsim*.root"):
    file2 = "../" + args.dir2 + '/' + file1
    filestub = file1.split('_')[2] + file1.split('_')[-1].rsplit('.',1)[0]
    if not os.path.isfile(file2):
        print file2, "not found. Continuing"
        file2 = file2.replace("DarkNoiseM0","DarkNoiseM1")
        if not os.path.isfile(file2):
            print file2, "not found. Continuing"
            continue
    command = 'rootwc -l -b -q \'$TDEALTRYREPO/hyperkdaq/wcsim/filesize/printSizes.C("' + file1 + '","wcsimT",true)\' > tempsize.txt \n' \
        'rootwc -l -b -q \'$TDEALTRYREPO/hyperkdaq/wcsim/filesize/printSizes.C("' + file2 + '","wcsimT",true)\' >> tempsize.txt'
    print command
    os.system(command)
    #get the results
    time.sleep(1)
    f = open("tempsize.txt","r")
    sizes = []
    for line in f.readlines():
        if line.startswith("TREESIZE"):
            sizes.append(int(line.split()[1]))
    f.close()

    #fout.write(file1)
    #fout2.write(file1)
    energy = file1.split('_')[1]
    energy = re.split("[a-zA-Z]", energy)[0]

    result = "{3} & {0:.2f} & {1:.2f} & {2:.2f}\\% \\\\ \n".format(sizes[0] / 1E6, sizes[1] / 1E6, ((sizes[1] / float(sizes[0])) * 100) - 100, energy)
    fout.write(result)
    
    mac1 = file1.replace(".root",".mac")
    mac2 = file2.replace(".root",".mac")
    #print datetime.fromtimestamp(os.path.getmtime(file1)), datetime.fromtimestamp(os.path.getmtime(mac1))
    tdelta1 = datetime.fromtimestamp(os.path.getmtime(file1)) - datetime.fromtimestamp(os.path.getmtime(mac1))
    #print datetime.fromtimestamp(os.path.getmtime(file2)), datetime.fromtimestamp(os.path.getmtime(mac2))
    tdelta2 = datetime.fromtimestamp(os.path.getmtime(file2)) - datetime.fromtimestamp(os.path.getmtime(mac2))
    def total_seconds(td):
        return (td.microseconds + (td.seconds + td.days * 24 * 3600) * 10**6) / 10**6
    ts1 = total_seconds(tdelta1)
    ts2 = total_seconds(tdelta2)
    result = "{3} & {0} & {1} & {2:.2f}\\% \\\\ \n".format(ts1, ts2, ((ts2 / float(ts1)) * 100) - 100, energy)
    fout2.write(result)

fout.close()
