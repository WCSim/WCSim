#!/usr/bin/env python                                                                                                                                                                                                                          

import os, argparse, sys, subprocess

def MkDirOrCheckIsEmpty(dirpath):
    if not os.path.isdir(dirpath):
        os.mkdir(dirpath)
    else:
        if os.listdir(dirpath):
            print 'Directory', dirpath, 'is not empty. Exiting...'
            sys.exit(1)

origdir = os.getcwd()

def RunCommand(command):
    try:
        retcode = subprocess.call(command, shell=True)
        if retcode < 0:
            print >>sys.stderr, command, "Child was terminated by signal", -retcode
        else:
            print >>sys.stderr, command, "Child returned", retcode
    except OSError as e:
        print >>sys.stderr, command, "Execution failed:", e

def grabcode(args):
    MkDirOrCheckIsEmpty(args.clean_dir)
    MkDirOrCheckIsEmpty(args.new_dir)
    def CheckoutAndCompile(dirpath, repopath, repobranch, setupscript):
        os.chdir(dirpath)
        RunCommand('git clone ' + repopath)
        os.chdir('WCSim')
        RunCommand('git checkout ' + repobranch)
        RunCommand('source ' + setupscript)
        os.environ['WCSIMDIR'] = os.getcwd()
        os.environ['PATH'] = os.getcwd() + '/bin/' + os.environ['G4SYSTEM'] + '/:' + os.getcwd() + '/rootwc/' + ':' + os.environ['PATH']
        os.environ['LD_LIBRARY_PATH'] = os.getcwd() + ':' + os.environ['LD_LIBRARY_PATH']
        #print os.environ['WCSIMDIR']
        #print os.environ['PATH']
        #print os.environ['LD_LIBRARY_PATH']
        RunCommand('make rootcint')
        RunCommand('make')
        os.chdir(origdir)
    CheckoutAndCompile(args.clean_dir, 'git@github.com:WCSim/WCSim.git', 'develop', args.setup_script)
    CheckoutAndCompile(args.new_dir, args.new_repo, args.new_branch, args.setup_script)


def run(args):
    pass

def analyse(args):
    pass


parser = argparse.ArgumentParser(description='Analyse a directory of WCSim output files')
parser.add_argument('--setup-script', required=True, help='The setup script to source ROOT, GEANT, etc')
subparsers = parser.add_subparsers(help='sub-command help')

parser_grabcode = subparsers.add_parser('grabcode', help='grabcode help')
parser_grabcode.add_argument('--clean-dir', type=str, default='./wcsim-clean', help='Empty (or not yet created) directory to download the official develop branch into')
parser_grabcode.add_argument('--new-dir', type=str, default='./wcsim-new', help='Empty (or not yet created) directory to download the branch you want to merge')
parser_grabcode.add_argument('--new-repo', type=str, required=True, help='The repository to checkout the new code from')
parser_grabcode.add_argument('--new-branch', type=str, required=True, help='The branch to checkout the new code from')
parser_grabcode.set_defaults(func=grabcode)

parser_run = subparsers.add_parser('run', help='run help')
parser_run.set_defaults(func=run)

parser_analyse = subparsers.add_parser('analyse', help='analyse help')
parser_analyse.set_defaults(func=analyse)

args = parser.parse_args()
args.func(args)

print args

