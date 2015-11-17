#!/usr/bin/env python

import glob, os, argparse, sys

parser = argparse.ArgumentParser(description='Analyse a directory of WCSim output files', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-c','--compiled-mode', action='store_true', help='Run in compiled mode? Requires rootwc in $PATH')
parser.add_argument('-m','--macroname', type=str, default='$WCSIMDIR/sample-root-scripts/sample_readfile.C', help='Full path to the macro')
parser.add_argument('-d','--dir', type=str, help='Directory to analyse', default='./')
parser.add_argument('-v','--verbose', action='store_true', help='Run verbosely?')
parser.add_argument('-f','--onlyonefile', action='store_true', help='Run only one file?')
parser.add_argument('--additional-macro-options', type=str, default='', help='If your macro has more options than just filename & verbosity, specify the remaining arguments here. Recommended to wrap the command in \'\', then "" are dealt with correctly. WARNING: no whitespace allowed!')
parser.add_argument('--batchmode', type=str, default='local', choices=['local','condor'], help='Where to submit the jobs')
parser.add_argument('--notifyuseremail', type=str, default='', help='Specify this to get email notifications about jobs')
args = parser.parse_args()

def py_bool_to_string(arg):
    if arg:
        return "true"
    else:
        return "false"

def main(args_to_parse = None):
    if args.notifyuseremail != "" and (not "@" in args.notifyuseremail or len(args.notifyuseremail.split()) > 1):
        print 'Invalid notifyuseremail - contains whitespace and/or has no @ sign'
        sys.exit(1)

    verbose        = py_bool_to_string(args.verbose)

    try:
        os.chdir(args.dir)
    except:
        print "Cannot cd to directory", args.dir, "exiting..."
        sys.exit(1)
    print "cd'ed to", args.dir

    #create the start of the command (i.e. the macroname, compiled mode/not, the executable to use)
    prefix = 'root'
    if args.compiled_mode:
        prefix = 'rootwc'
    prefix += " '" + args.macroname
    if args.compiled_mode:
        prefix += '+'
    
    filenamestub = 'analysewcsim'
    if args.batchmode == 'condor' and not os.path.islink('rootwc'):
        os.symlink(os.path.expandvars("$WCSIMDIR") + "/rootwc/rootwc", "rootwc")

    #loop through all the relevant files in the directory, and run the analysis script
    for i, file1 in enumerate(glob.glob("wcsim_*.root")):
        #create the arguments passed to the executable
        commandstub = '("' + file1 + '",' + verbose + (',' + args.additional_macro_options if args.additional_macro_options else '') + ")"

        #run in local mode
        if args.batchmode == 'local':
            command = prefix + ('+' if (args.compiled_mode and i == 0) else '') + commandstub + "' -l -b -q"
        #run in condor batch mode
        elif args.batchmode == 'condor':
            filename = filenamestub + '_' + str(i)
            froot = open(filename + '.C', 'w')
            root = 'void ' + filename + '() { \n' \
                '  gROOT->ProcessLine(".L ' + args.macroname + '+"); \n' \
                '  ' + args.macroname.split('/')[-1].rsplit('.',1)[0] + commandstub + '; \n' \
                '}'
            froot.write(root)
            froot.close()
            fcondor = open(filename + '.jdl', 'w')
            condor = ''
            if args.notifyuseremail is not '':
                condor += '' \
                    'notify_user    = ' + args.notifyuseremail + '\n' \
                    'notification   = Always \n'
            condor += '' \
                'executable     = rootwc \n' \
                'universe       = vanilla \n' \
                'arguments      = -b -q ' + filename + '.C \n' \
                'input          = ' + file1 + ' \n' \
                'transfer_input_files = ' + filename + '.C \n' \
                'output         = ' + filename + '.out \n' \
                'error          = ' + filename + '.err \n' \
                'log            = ' + filename + '.log \n' \
                'request_memory = 1000 \n' \
                'getenv         = True \n' \
                'queue 1 \n'
            fcondor.write(condor)
            fcondor.close()
            command = 'condor_submit ' + filename + '.jdl'

        #run the command
        print command
        os.system(command)
        if args.onlyonefile:
            break

if __name__ == "__main__":
    main()
