#!/usr/bin/env python

"""
Script to create and run a lot of WCSim .mac files

Run examples:

e.g. 10000 events per configuration  in the SK geometry, with the SKI digitizer, 25 NHits trigger (automatically adjusted for dark noise rate), 0 or 8.4 kHZ dark noise in a window 2000 ns (+/-1000ns) around hits, with 5,10,20,50 MeV electrons using a fixed position & direction
(produces 8 files total)

python generate_mac_files.py --batchmode local --WCgeom SuperK --HKwatertanklength 24750 --PMTQEMethod Stacking_Only --SavePi0 false --DAQdigitizer SKI --DAQtrigger NHits --DAQnhitsthreshold 25 --DAQnhitsignorenoise --DAQnhitswindow 200 --DAQsavefailuresmode 0 --DAQsavefailurestime 250 --DarkNoiseRate 0,8.4 --DarkNoiseConvert 1.367 --DarkNoiseMode 1 --DarkNoiseWindow 2000 --GunParticle e- --GunEnergy 5,10,20,50 --GunPosition 0,0,0 --GunDirection 1,0,0 --NEvents 10000

e.g. the same, but producing the particles at random positions and directions
(produces 8 files total)

python generate_mac_files.py --batchmode local --WCgeom SuperK --HKwatertanklength 24750 --PMTQEMethod Stacking_Only --SavePi0 false --DAQdigitizer SKI --DAQtrigger NHits --DAQnhitsthreshold 25 --DAQnhitsignorenoise --DAQnhitswindow 200 --DAQsavefailuresmode 0 --DAQsavefailurestime 250 --DarkNoiseRate 0,8.4 --DarkNoiseConvert 1.367 --DarkNoiseMode 1 --DarkNoiseWindow 2000 --GunParticle e- --GunEnergy 5,10,20,50 --GunPosition random --GunDirection 4pi --NEvents 10000

"""

import argparse
import shutil
import sys
import os

delim_list = lambda s: s.split(',')
delim_list_str = lambda s: s.split(',') if len(s.split(',')) == 3 else s

WCgeom_choices = ['HyperK', 'SuperK', \
                'SuperK_20inchPMT_20perCent', \
                'SuperK_20inchBandL_20perCent', \
                'SuperK_12inchBandL_15perCent', \
                'SuperK_20inchBandL_14perCent', \
                'Cylinder_12inchHPD_15perCent', \
                'HyperK_withHPD', \
                'DUSEL_100kton_10inch_40perCent', \
                'DUSEL_100kton_10inch_HQE_12perCent', \
                'DUSEL_100kton_10inch_HQE_30perCent', \
                'DUSEL_100kton_10inch_HQE_30perCent_Gd', \
                'DUSEL_150kton_10inch_HQE_30perCent', \
                'DUSEL_200kton_10inch_HQE_12perCent', \
                'DUSEL_200kton_12inch_HQE_10perCent', \
                'DUSEL_200kton_12inch_HQE_14perCent' ]
PMTQEMethod_choices = ['Stacking_Only', 'Stacking_And_SensitiveDetector', 'SensitiveDetector_Only']
SavePi0_choices = ['true', 'false']
DAQdigitizer_choices = ['SKIV', 'SKI', 'SKI_SKDETSIM']
DAQtrigger_choices = ['NHits', 'SKI_SKDETSIM', 'NHits2']
GunPositionChoices = ['center', 'random', 'wall', 'minusx', 'plusx', 'minusz', 'plusz']
GunDirectionChoices = ['towall', 'tocap', '4pi', 'wall']

parser = argparse.ArgumentParser(description='Run many WCSim jobs with different options. Use , to delimit multiple options')
parser.add_argument('--batchmode', type=str, required=True, choices=['local','condor'], help='Where to submit the jobs')
parser.add_argument('--vis', action='store_true', help='Turn on the visulation? Not yet implemented')
parser.add_argument('--ReuseDAQFolder', action='store_true', help='Reuse the DAQ folders (i.e. don\'t exit if mkdir fails)')
parser.add_argument('--WCgeom', type=delim_list, required=True, help='The water tank geometry')
parser.add_argument('--HKwatertanklength', type=delim_list, required=True, help='The size of a HK geometry (mm)')
parser.add_argument('--PMTQEMethod', type=delim_list, required=True, help='How the QE is applied')
parser.add_argument('--SavePi0', type=delim_list, required=True, help='Save Pi0 info?')
parser.add_argument('--DAQsavefailuresmode', type=delim_list, required=True, help='Save failed triggers mode')
parser.add_argument('--DAQsavefailurestime', type=delim_list, required=True, help='Failed triggers trigger time')
parser.add_argument('--DAQdigitizer', type=delim_list, required=True, help='Which digitizer class to use?')
parser.add_argument('--DAQtrigger', type=delim_list, required=True, help='Which trigger class to use?')
parser.add_argument('--DAQnhitsthreshold', type=delim_list, required=True, help='What value of the nhits trigger threshold should be used?')
parser.add_argument('--DAQnhitswindow', type=delim_list, required=True, help='What value of the nhits trigger window should be used?')
parser.add_argument('--DAQnhitsignorenoise', action='store_true', help='Adjust the NHits threshold automatically for the dark noise rate?')
parser.add_argument('--DarkNoiseRate', type=delim_list, required=True, help='Dark noise rate (kHz)')
parser.add_argument('--DarkNoiseConvert', type=delim_list, required=True, help='Convert dark noise frequency before digitization to after digitization by setting suitable factor')
parser.add_argument('--DarkNoiseMode', type=int, required=True, choices=[0,1], help='0: apply noise around hits. 1: apply noise in a specified time window')
parser.add_argument('--DarkNoiseWindow', type=delim_list, required=True, help='Mode 0: Apply dark noise in a window around each hit. Mode 2: Apply dark noise in range x:y (use : as delimeter!)')
parser.add_argument('--GunParticle', type=str, required=True, choices=['e-','e+','mu-','mu+','pi-','pi+','pi0','gamma'], help='Particle gun particle')
parser.add_argument('--GunEnergy', type=delim_list, required=True, help='Particle gun energy (MeV)')
parser.add_argument('--GunPosition', type=delim_list_str, required=True, help='Particle gun position')
parser.add_argument('--GunDirection', type=delim_list_str, required=True, help='Particle gun direction')
parser.add_argument('--NEvents', type=int, required=True, help='Number of events per configuration')

args = parser.parse_args()

def check_input_list(arglist, allowed, parser):
    for arg in arglist:
        if arg not in allowed:
            print arg, "is incorrect"
            parser.print_help()
            sys.exit(1)
            
check_input_list(args.WCgeom, WCgeom_choices, parser)
check_input_list(args.PMTQEMethod, PMTQEMethod_choices, parser)
check_input_list(args.SavePi0, SavePi0_choices, parser)
check_input_list(args.DAQdigitizer, DAQdigitizer_choices, parser)
check_input_list(args.DAQtrigger, DAQtrigger_choices, parser)
if (type(args.GunPosition) is list and len(args.GunPosition) != 3) \
    or (type(args.GunDirection) is list and len(args.GunDirection) != 3):
    print "GunPosition and GunDirection are three vectors. Specify exactly 3 options!"
    parser.print_help()
    sys.exit(1)
if (type(args.GunPosition) is str and args.GunPosition not in GunPositionChoices):
    print "GunPosition", args.GunPosition, "not one of", GunPositionChoices
    parser.print_help()
    sys.exit(1)
if (type(args.GunDirection) is str and args.GunDirection not in GunDirectionChoices):
    print "GunDirection", args.GunDirection, "not one of", GunDirectionChoices
    parser.print_help()
    sys.exit(1)
if type(args.GunDirection) != type(args.GunPosition):
    print "Must use consistent GunPosition and GunDirection options (i.e. both 3 vectors or both MakeKin.py str options)"
    parser.print_help()
    sys.exit(1)

origdir = os.getcwd()

#make the jobOptions.mac file(s)
DAQfolders = []
for DAQdigitizer in args.DAQdigitizer:
    for DAQtrigger in args.DAQtrigger:
        if DAQtrigger == 'SKI_SKDETSIM' and DAQdigitizer != 'SKI_SKDETSIM':
            continue
        if DAQtrigger != 'SKI_SKDETSIM' and DAQdigitizer == 'SKI_SKDETSIM':
            continue
        for DAQnhitsthreshold in args.DAQnhitsthreshold:
            for DAQnhitswindow in args.DAQnhitswindow:
                for DAQsavefailuresmode in args.DAQsavefailuresmode:
                    for DAQsavefailurestime in args.DAQsavefailurestime:
                            # go to the specific folder for this jobOptions
                            foldername = DAQdigitizer + "_" + DAQtrigger + "_fails" + DAQsavefailuresmode + "_"
                            if DAQsavefailuresmode:
                                foldername = foldername + DAQsavefailurestime + "_"
                            if (DAQtrigger.find("NHits") != -1) or (DAQtrigger.find("SKI_SKDETSIM") == 0):
                                foldername = foldername + "NHits" + str(DAQnhitsthreshold) + "_" + str(DAQnhitswindow)
                            os.chdir(origdir)
                            try:
                                os.mkdir(foldername)
                            except OSError:
                                print "Folder", foldername, "already exists"
                                if not args.ReuseDAQFolder:
                                    print "Exiting..."
                                    sys.exit(1)
                            os.chdir(foldername)
                            DAQfolders.append(foldername)
                            noise_agnostic = 'true' if args.DAQnhitsignorenoise else 'false'
                            #create the jopOptions.mac file
                            text = "/WCSim/physics/list WCSim" + "\n" \
                                "/DAQ/Digitizer " + DAQdigitizer + "\n" \
                                "/DAQ/Trigger " + DAQtrigger + "\n" \
                                "/DAQ/TriggerSaveFailures/Mode " + DAQsavefailuresmode + "\n" \
                                "/DAQ/TriggerSaveFailures/TriggerTime " + DAQsavefailurestime + "\n" \
                                "/DAQ/TriggerNHits/Threshold " + DAQnhitsthreshold + "\n" \
                                "/DAQ/TriggerNHits/Window " + DAQnhitswindow + "\n" \
                                "/DAQ/TriggerNHits/AdjustForNoise " + noise_agnostic + "\n"
                            #create the file
                            f = open('jobOptions.mac', 'w')
                            f.write(text)
                            f.close()
                            #and don't forget the other .mac files
                            shutil.copy2(os.path.expandvars("$WCSIMDIR") + "/jobOptions2.mac", "./")
                            shutil.copy2(os.path.expandvars("$WCSIMDIR") + "/tuning_parameters.mac", "./")
                            #and the exectuable
                            if args.batchmode == 'condor':
                                if not os.path.islink('WCSim'):
                                    os.symlink(os.path.expandvars("$WCSIMDIR") + "/WCSim", "WCSim")

#make the novis.mac analogue
counter = 1
for WCgeom in args.WCgeom:
    for HKwatertanklength in args.HKwatertanklength:
        for PMTQEMethod in args.PMTQEMethod:
            for SavePi0 in args.SavePi0:
                for DAQfolder in DAQfolders:
                    for DarkNoiseRate in args.DarkNoiseRate:
                        for DarkNoiseConvert in args.DarkNoiseConvert:
                            for DarkNoiseWindow in args.DarkNoiseWindow:
                                for GunEnergy in args.GunEnergy:
                                    os.chdir(origdir + "/" + DAQfolder)
                                    #DarkNoiseWindow should be treated specially, as it depends on DarkNoiseMode
                                    darknoisewindow = ""
                                    if args.DarkNoiseMode == 1:
                                        try:
                                            int(DarkNoiseWindow)
                                        except ValueError:
                                            print "For DarkNoiseMode == 1, DarkNoiseWindow should be a single number (not colon separated)"
                                            sys.exit(1)
                                            darknoisewindow = "/DarkRate/SetDarkWindow " + DarkNoiseWindow + "\n"
                                    elif args.DarkNoiseMode == 0:
                                        darknoisewindow = "/DarkRate/SetDarkLow  " + DarkNoiseWindow.split(':')[0] + "\n" \
                                            "/DarkRate/SetDarkHigh " + DarkNoiseWindow.split(':')[1] + "\n"
                                    else:
                                        sys.exit(1)

                                    if type(args.GunPosition) is str:
                                        #if GunPosition and GunDirection are string's
                                        #we need to call MakeKin.py to generate distributions of different positions/directions
                                        command = '$WCSIMDIR/sample-root-scripts/MakeKin.py -N 1 ' \
                                        '-n ' + str(args.NEvents) + ' ' \
                                        '-t ' + args.GunParticle  + ' ' \
                                        '-e ' + GunEnergy  + ' ' \
                                        '-v ' + args.GunPosition  + ' ' \
                                        '-d ' + args.GunDirection
                                        print command
                                        os.system(command)
                                        #now create the .kin filename
                                        kinname = "%s_%.0fMeV__%s_%s_%03i.kin" % (args.GunParticle.replace("+","plus").replace("-","minus"), float(GunEnergy), args.GunPosition, args.GunDirection, 0)
                                        print kinname
                                        #and finally get the .mac options
                                        gunoptions = '/mygen/vecfile ' + kinname + '\n'
                                    else:
                                        #we're using the simple GEANT4 particle gun
                                        gunoptions = "/gun/particle " + args.GunParticle + "\n" \
                                            "/gun/energy " + GunEnergy + " MeV \n" \
                                            "/gun/direction " + " ".join(i for i in args.GunDirection) + "\n" \
                                            "/gun/position " + " ".join(i for i in args.GunPosition) + "\n"
                                        
                                    #the WCSim/Construct line
                                    if WCgeom == 'SuperK':
                                        constructoption = ''
                                    else:
                                        constructoption = "/WCSim/Construct \n"

                                    #construct the filename
                                    filenamestub = "wcsim_" + GunEnergy + args.GunParticle + "_" + WCgeom + "_" + HKwatertanklength + "_" + PMTQEMethod + "_SavePi0" + SavePi0 + "_DarkNoiseM" + str(args.DarkNoiseMode) + "R" + DarkNoiseRate + "W" + DarkNoiseWindow.strip()
                                    #make the text for the config file
                                    text = "/run/verbose 0 \n" \
                                        "/tracking/verbose 0 \n" \
                                        "/hits/verbose 0 \n" \
                                        "/WCSim/WCgeom " + WCgeom + "\n" \
                                        "/WCSim/HyperK/waterTank_Length " + HKwatertanklength + "\n" \
                                        "" + constructoption + "" \
                                        "/WCSim/PMTQEMethod " + PMTQEMethod + "\n" \
                                        "/WCSim/SavePi0 " + SavePi0 + "\n" \
                                        "/DarkRate/SetDarkRate " + DarkNoiseRate + " kHz \n" \
                                        "/DarkRate/SetConvert " + DarkNoiseConvert + "\n" \
                                        "/DarkRate/SetDarkMode " + str(args.DarkNoiseMode) + "\n" \
                                        "" + darknoisewindow + "" \
                                        "/mygen/generator normal " + "\n" \
                                        "" + gunoptions + "" \
                                        "/WCSimIO/RootFile " + filenamestub + ".root" + "\n" \
                                        "/run/beamOn " + str(args.NEvents) + "\n"

                                    #create the config file
                                    f = open(filenamestub + '.mac', 'w')
                                    f.write(text)
                                    f.close()

                                    #print the run information
                                    print "\n\nfile:", counter
                                    print '\n jopOptions.mac:'
                                    ftemp = open('jobOptions.mac', 'r')
                                    for l in ftemp.readlines():
                                        print l,
                                    ftemp.close()
                                    print '\n', filenamestub + '.mac:'
                                    print text

                                    #submit the job
                                    if args.batchmode == 'local':
                                        command = 'WCSim ' + filenamestub + '.mac &> ' + filenamestub + '.out'
                                    elif args.batchmode == 'condor':
                                        fcondor = open(filenamestub + '.jdl', 'w')
                                        condor = '' \
                                          'executable     = WCSim \n' \
                                          'universe       = vanilla \n' \
                                          'arguments      = ' + filenamestub + '.mac \n' \
                                          'output         = ' + filenamestub + '.out \n' \
                                          'error          = ' + filenamestub + '.err \n' \
                                          'log            = ' + filenamestub + '.log \n' \
                                          'request_memory = 1000 \n' \
                                          'queue 1 \n'
                                        fcondor.write(condor)
                                        fcondor.close()
                                        command = 'condor_submit ' + filenamestub + '.jdl'
                                        
                                    os.system(command)
                                    print command
                                    counter += 1
