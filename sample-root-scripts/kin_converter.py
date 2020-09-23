#!/bin/env python3

import argparse
import sys
from datetime import datetime

ns_conversion = {'ns':1,
                 'us':1E3,
                 'ms':1E6,
                 's':1E9}

parser = argparse.ArgumentParser(description='KinConverter: convert kin files that have multiple vertices into a single event (or multiple overlapping events)')
parser.add_argument('--input-filename','-i',required=True,type=str,help='Input .kin filename. Output filename(s) will be the same as the input filename & path, with [0-9].merge suffix(es) added')
parser.add_argument('--input-time-unit',required=True,choices=ns_conversion.keys(),help='The time unit of the input file')
parser.add_argument('--dark-noise-start', type=int,required=True,help='When to start the simulation (in ns)')
parser.add_argument('--dark-noise-end',type=int,required=True,help='When to end the simulation (in ns)')
parser.add_argument('--event-overlap',type=int,required=True,help='How long (in ns) to overlap')
parser.add_argument('--verbose','--v',type=int,default=0,help='Verbosity level')
#Use either this
parser.add_argument('--fixed-duration',type=int,default=None,help='A fixed duration (in ns) for each event')
#or these - dark rate, ntubes, NHits per MeV, max allowed hits
#TODO add option for windows that can change size depending on how many physics hits are expected
args = parser.parse_args()

ToNS = ns_conversion[args.input_time_unit]

def PrintNS(time):
    for x in ['ns', 'us', 'ms', 's']:
        if time < 1000.0:
            return "%f %s" % (time, x)
        time /= 1000.0

DummyVertex = """$ nuance 0
$ vertex 0 0 0 0
$ track -12 0.00000 0.00000 0.00000 1.00000 -1
$ track 2212 938.27231 0.00000 0.00000 1.00000 -1
$ info 0 0 0
$ track -11 0.511 0 0 0 0
"""

#read a vertex at a time
def GetVertex(seq, group_by, exclude=['']):
    data = []
    for line in seq:
        if isinstance(line, bytes):
            line = line.decode()
        if line.startswith(group_by):
            if data:
                yield data
                data = []
        if line.strip() in exclude:
            continue
        data.append(line)

    if data:
        yield data

#get the time from a vertex
def GetTime(vertex):
    for line in vertex:
        if 'vertex' in line:
            return float(line.split()[-1]) * ToNS

#check that the file is time ordered
def IsTimeOrdered(filename):
    with open(filename, 'r') as fin:
        for i, vertex in enumerate(GetVertex(fin, "$ begin")):
            #skip the header
            if vertex[0].startswith('#'):
                continue
            try:
                last_time = this_time
            except UnboundLocalError:
                last_time = float(GetTime(vertex))
            this_time = float(GetTime(vertex))
            if this_time < last_time:
                print(PrintNS(this_time), "comes before", PrintNS(last_time))
                return False
    print("Is time ordered")
    return True

#Sort the initial file by time
def SortByTime(filename):
    outfilename = args.input_filename + '.temp'
    with open(filename, 'r') as fin, open(outfilename, 'w') as fout:
        print('TODO SortByTime() not yet implemented')
        sys.exit(-1)

#Get the header
def GetHeader(filename, args):
    with open(filename, 'r') as fin:
        for vertex in GetVertex(fin, "$ begin"):
            #if there's no header, return blank
            if vertex[0].startswith("$ begin"):
                return ''
            #return the header as a single string
            header = ''.join(vertex) + ''\
                     '# Split by kin_converter ' + str(datetime.now()) + '\n'\
                     '# --fixed-duration ' + str(args.fixed_duration) + '\n'\
                     '# --event-overlap ' + str(args.event_overlap) + '\n'
            return header

#See if the file is time ordered
if not IsTimeOrdered(args.input_filename):
    #If not, sort it
    SortByTime(args.input_filename)

header = GetHeader(args.input_filename, args)
print(header)


#loop over kin file start/stop times
event_start = args.dark_noise_start
last_event_end = args.dark_noise_end
ievent = 0
file_position = 0
while event_start < last_event_end:
    event_end = event_start + args.fixed_duration
    next_event_start = event_start + args.fixed_duration - args.event_overlap
    print("Event", ievent, "corresponds to range", PrintNS(event_start), PrintNS(event_end))
    with open(args.input_filename, 'rb') as fin, open(args.input_filename + '.%09d' % ievent, 'w') as fout:
        #write the original header
        fout.write(header)
        #write the dark noise range
        fout.write('# Event ' + str(ievent) + '\n')
        fout.write('# /DarkRate/SetDarkLow  ' + str(event_start) + '\n')
        fout.write('# /DarkRate/SetDarkHigh ' + str(event_end) + '\n')
        #and the event start
        fout.write('$ begin\n')
        nvertices = 0
        #skip forward in the file a bit
        if args.verbose:
            print('Skipping to position in file', file_position)
        fin.seek(file_position)
        #loop over the input file
        for i, vertex in enumerate(GetVertex(fin, "$ begin", ["$ begin", "$ end"])):
            #skip the header and any partial vertices we've found from using seek()
            if not vertex[0].startswith('$ nuance'):
                continue
            #get the event time
            time = GetTime(vertex)
            if args.verbose > 1:
                print(PrintNS(time))
                if args.verbose > 2:
                    print("Vertex #{}".format(i))
                    print("".join(vertex))
            if time > event_end:
                break
            if time >= event_start:
                fout.write(''.join(vertex))
                nvertices += 1
            #save the current file position if it is earlier than required for the next event
            # At most, the position will be the '$ begin' line of the first vertex in the next event
            if time < next_event_start:
                file_position = fin.tell()
        #need to add a dummy vertex, else WCSim/Geant4 will complain
        if not nvertices:
            fout.write(DummyVertex)
        #and close the event/file
        fout.write('$ end\n')
        fout.write('$ stop\n')
        print('contains', nvertices, 'vertices')
    #increment for next event
    event_start = next_event_start
    ievent += 1
