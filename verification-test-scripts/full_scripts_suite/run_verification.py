#!/usr/bin/env python                                                                                                                                                                                                                          

import os, argparse, sys, subprocess
import generate_mac_files as gmf

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

def SourceWCSim(setupscript, wcsimdir = os.getcwd()):
        RunCommand('source ' + setupscript)
        os.environ['WCSIMDIR'] = wcsimdir
        os.environ['PATH'] = wcsimdir + '/bin/' + os.environ['G4SYSTEM'] + '/:' + wcsimdir + '/rootwc/' + ':' + os.environ['PATH']
        os.environ['LD_LIBRARY_PATH'] = wcsimdir + ':' + os.environ['LD_LIBRARY_PATH']
        #print os.environ['WCSIMDIR']
        #print os.environ['PATH']
        #print os.environ['LD_LIBRARY_PATH']
    
        
def grabcode(args):
    MkDirOrCheckIsEmpty(args.clean_dir)
    MkDirOrCheckIsEmpty(args.new_dir)
    def CheckoutAndCompile(dirpath, repopath, repobranch, setupscript):
        os.chdir(dirpath)
        RunCommand('git clone ' + repopath)
        os.chdir('WCSim')
        RunCommand('git checkout ' + repobranch)
        SourceWCSim(setupscript)
        RunCommand('make rootcint')
        RunCommand('make')
        os.chdir(origdir)
    CheckoutAndCompile(args.clean_dir, 'git@github.com:WCSim/WCSim.git', 'develop', args.setup_script)
    CheckoutAndCompile(args.new_dir, args.new_repo, args.new_branch, args.setup_script)

    
def run(args):
    clean_work_dir = args.work_dir + '/clean/'
    new_work_dir = args.work_dir + '/new/'
    #MkDirOrCheckIsEmpty(args.work_dir)
    MkDirOrCheckIsEmpty(clean_work_dir)
    MkDirOrCheckIsEmpty(new_work_dir)
    def RunWCSim(runoption, dirpath, wcsimdirpath, setupscript):
        os.chdir(dirpath)
        SourceWCSim(setupscript, wcsimdirpath + '/WCSim/')
        gmf.main(runoption.split())
        os.chdir(origdir)
    run_options = []
    if args.low_energy:
        #this will take the default options for geometry, dark noise, PMT parameters, etc, and run 1000 events for each of 4 electron particle gun configurations
        run_options.append('--NEvents 1000 --GunParticle e- --GunDirection 1,0,0 --GunPosition 0,0,0 --GunEnergy 5,10,20,50 --batchmode '+args.batchmode)
    for runoption in run_options:
        RunWCSim(runoption, clean_work_dir, args.clean_dir, args.setup_script)
        RunWCSim(runoption, new_work_dir, args.work_dir, args.setup_script)

        
def analyse(args):
    pass


def compare(args):
    pass


parser = argparse.ArgumentParser(description='Analyse a directory of WCSim output files')
parser.add_argument('--setup-script', required=True, help='The setup script to source ROOT, GEANT, etc. This SHOULDN"T setup WCSim')
parser.add_argument('--clean-dir', type=str, default='./wcsim-clean', help='Empty (or not yet created) directory to download the official develop branch into. Default ./wcsim-clean')
parser.add_argument('--new-dir', type=str, default='./wcsim-new', help='Empty (or not yet created) directory to download the branch you want to merge. Default ./wcsim-new')
subparsers = parser.add_subparsers(help='')

parser_grabcode = subparsers.add_parser('grabcode', help='grabcode - download & compile fresh versions of WCSim')
parser_grabcode.add_argument('--new-repo', type=str, required=True, help='The repository to checkout the new code from. E.g. git@github.com:tdealtry/WCSim.git')
parser_grabcode.add_argument('--new-branch', type=str, required=True, help='The branch to checkout the new code from. E.g. feature/issue10')
parser_grabcode.set_defaults(func=grabcode)

parser_run = subparsers.add_parser('run', help='run - run WCSim with identical options in the two fresh copies of WCSim')
parser_run.add_argument('--batchmode', type=str, default='local', choices=gmf.BatchChoices, help='Where to submit the jobs. Choices: '+gmf.ListAsString(gmf.BatchChoices)+' (default local)')
parser_run.add_argument('--work-dir', type=str, default='./wcsim-work', help='Empty (or not yet created) directory to save the output of WCSim to. Default ./wcsim-work')
parser_run.add_argument('--low-energy', action='store_true', help='Run the low energy verification?')
parser_run.set_defaults(func=run)

parser_analyse = subparsers.add_parser('analyse', help='analyse help')
parser_analyse.set_defaults(func=analyse)

parser_compare = subparsers.add_parser('compare', help='compare help')
parser_compare.set_defaults(func=compare)

args = parser.parse_args()
args.func(args)

print args

