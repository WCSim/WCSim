#!/usr/bin/env python

import glob, os, argparse, sys

parser = argparse.ArgumentParser(description='Analyse a directory of WCSim output files')
parser.add_argument('-c','--compiled-mode', action='store_true', help='Run in compiled mode? Requires rootwc in $PATH')
parser.add_argument('-m','--macroname', type=str, default='$WCSIMDIR/sample-root-scripts/sample_readfile.C', help='Full path to the macro. Default: $WCSIMDIR/sample-root-scripts/sample_readfile.C')
parser.add_argument('-d','--dir', type=str, help='Directory to analyse. Default .', default='./')
parser.add_argument('-v','--verbose', action='store_true', help='Run verbosely?')
parser.add_argument('-f','--onlyonefile', action='store_true', help='Run only one file?')
parser.add_argument('--additional-macro-options', type=str, default='', help='If your macro has more options than just filename & verbosity, specify the remaining arguments here. Recommended to wrap the command in \'\', then "" are dealt with correctly. WARNING: no whitespace allowed!')
args = parser.parse_args()

def py_bool_to_string(arg):
    if arg:
        return "true"
    else:
        return "false"
verbose        = py_bool_to_string(args.verbose)

try:
    os.chdir(args.dir)
except:
    print "Cannot cd to directory", args.dir, "exiting..."
    sys.exit(1)
print "cd'ed to", args.dir

prefix = 'root'
if args.compiled_mode:
    prefix = 'rootwc'
prefix += " '" + args.macroname
if args.compiled_mode:
    prefix += '+'

for file1 in glob.glob("wcsim_*.root"):
    command = prefix + '("' + file1 + '",' + verbose + (',' + args.additional_macro_options if args.additional_macro_options else '') + ")' -l -b -q"
    print command
    os.system(command)
    if args.onlyonefile:
        break
