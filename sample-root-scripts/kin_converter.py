#!/bin/env python2

import argparse

parser = argparse.ArgumentParser(description='KinConverter: convert kin files that have multiple events into a single event')
parser.add_argument('--infilename','-i',required=True,type=str,help='Input .kin filename. Output filename will be the same as the input filename & path, with a .single suffix added')
args = parser.parse_args()

outfilename = args.infilename + '.single'
foundbegin = False
with open(args.infilename, 'r') as fin, open(outfilename, 'w') as fout:
    for line in fin:
        split = line.split()
        #we only need the first begin
        if 'begin' in split:
            if not foundbegin:
                foundbegin = True
            else:
                continue
        #we only need the last begin...
        elif 'end' in split:
            continue
        #... which occurs just before the stop
        elif 'stop' in split:
            fout.write('$ end\n')
        #write the line
        fout.write(line)

