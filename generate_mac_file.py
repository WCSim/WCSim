#!/usr/bin/env python

"""
Script to create and run a lot of WCSim .mac files

To run e.g. 10000 events in the HK geometry, with the SKIV digitizer, NHits trigger, 0 or 8.4 kHZ dark noise in a window 4000 ns around hits, with 5,10,20,50 MeV electrons

python generate_mac_file.py --WCgeom HyperK --HKwatertanklength 24750 --PMTQEMethod Stacking_Only --SavePi0 false --DAQdigitizer SKIV --DAQtrigger NHits --DAQnhitsthreshold 25 --DarkNoiseRate 0,8.4 --DarkNoiseConvert 1.367 --DarkNoiseMode 0 --DarkNoiseWindow 4000 --GunParticle e- --GunEnergy 5,10,20,50 --GunPosition 0,0,0 --GunDirection 1,0,0 --NEvents 10000

"""

import argparse
import sys
import os

delim_list = lambda s: s.split(',')

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
DAQtrigger_choices = ['NHits', 'SKI_SKDETSIM']

parser = argparse.ArgumentParser(description='Run many WCSim jobs with different options. Use , to delimit multiple options')
parser.add_argument('--vis', action='store_true', help='Turn on the visulation? Not yet implemented')
parser.add_argument('--WCgeom', type=delim_list, required=True, help='The water tank geometry')
parser.add_argument('--HKwatertanklength', type=delim_list, required=True, help='The size of a HK geometry (mm)')
parser.add_argument('--PMTQEMethod', type=delim_list, required=True, help='How the QE is applied')
parser.add_argument('--SavePi0', type=delim_list, required=True, help='Save Pi0 info?')
parser.add_argument('--DAQdigitizer', type=delim_list, required=True, help='Which digitizer class to use?')
parser.add_argument('--DAQtrigger', type=delim_list, required=True, help='Which trigger class to use?')
parser.add_argument('--DAQnhitsthreshold', type=delim_list, required=True, help='What value of the nhits threshold should be used?')
parser.add_argument('--DarkNoiseRate', type=delim_list, required=True, help='Dark noise rate (kHz)')
parser.add_argument('--DarkNoiseConvert', type=delim_list, required=True, help='Convert dark noise frequency before digitization to after digitization by setting suitable factor')
parser.add_argument('--DarkNoiseMode', type=int, required=True, choices=[0,1], help='0: apply noise around hits. 1: apply noise in a specified time window')
parser.add_argument('--DarkNoiseWindow', type=delim_list, required=True, help='Mode 0: Apply dark noise in a window around each hit. Mode 2: Apply dark noise in range x:y (use : as delimeter!)')
parser.add_argument('--GunParticle', type=str, required=True, choices=['e-','e+','mu-','mu+','pi-','pi+','pi0','gamme'], help='Particle gun particle')
parser.add_argument('--GunEnergy', type=delim_list, required=True, help='Particle gun energy (MeV)')
parser.add_argument('--GunPosition', type=delim_list, required=True, help='Particle gun position')
parser.add_argument('--GunDirection', type=delim_list, required=True, help='Particle gun direction')
parser.add_argument('--NEvents', type=int, required=True, help='Number of events per configuration')

args = parser.parse_args()

def check_input_list(arglist, allowed, parser):
    for arg in arglist:
        if arg not in allowed:
            parser.print_help()
            sys.exit(1)
            
check_input_list(args.WCgeom, WCgeom_choices, parser)
check_input_list(args.PMTQEMethod, PMTQEMethod_choices, parser)
check_input_list(args.SavePi0, SavePi0_choices, parser)
check_input_list(args.DAQdigitizer, DAQdigitizer_choices, parser)
check_input_list(args.DAQtrigger, DAQtrigger_choices, parser)
if len(args.GunPosition) != 3 or len(args.GunDirection) != 3:
    print "GunPosition and GunDirection are three vectors. Specify exactly 3 options!"
    parser.print_help()
    sys.exit(1)



counter = 1
for WCgeom in args.WCgeom:
    for HKwatertanklength in args.HKwatertanklength:
        for PMTQEMethod in args.PMTQEMethod:
            for SavePi0 in args.SavePi0:
                for DAQnhitsthreshold in args.DAQnhitsthreshold:
                    for DAQdigitizer in args.DAQdigitizer:
                        for DAQtrigger in args.DAQtrigger:
                            for DarkNoiseRate in args.DarkNoiseRate:
                                for DarkNoiseConvert in args.DarkNoiseConvert:
                                    for DarkNoiseWindow in args.DarkNoiseWindow:
                                        for GunEnergy in args.GunEnergy:
                                                #DarkNoiseWindow should be treated specially, as it depends on DarkNoiseMode
                                                darknoisewindow = ""
                                                if args.DarkNoiseMode == 0:
                                                    try:
                                                        int(DarkNoiseWindow)
                                                    except ValueError:
                                                        print "For DarkNoiseMode == 0, DarkNoiseWindow should be a single number (not colon separated)"
                                                        sys.exit(1)
                                                    darknoisewindow = "/DarkRate/SetDarkWindow " + DarkNoiseWindow + "\n"
                                                elif args.DarkNoiseMode == 1:
                                                    darknoisewindow = "/DarkRate/SetDarkLow  " + DarkNoiseWindow.split(':')[0] + "\n" \
                                                        "/DarkRate/SetDarkHigh " + DarkNoiseWindow.split(':')[1] + "\n"
                                                else:
                                                    sys.exit(1)

                                                #construct the filename
                                                filenamestub = "wcsim_" + GunEnergy + args.GunParticle + "_" + WCgeom + "_" + HKwatertanklength + "_" + PMTQEMethod + "_SavePi0" + SavePi0 + "_Digi" + DAQdigitizer + "_Trig" + DAQtrigger + "_Thresh" + DAQnhitsthreshold + "_DarkNoiseM" + str(args.DarkNoiseMode) + "R" + DarkNoiseRate + "W" + DarkNoiseWindow
                                                #make the text for the config file
                                                text = "/run/verbose 0 \n" \
                                                    "/tracking/verbose 0 \n" \
                                                    "/hits/verbose 0 \n" \
                                                    "/WCSim/WCgeom " + WCgeom + "\n" \
                                                    "/WCSim/HyperK/waterTank_Length " + HKwatertanklength + "\n" \
                                                    "/WCSim/Construct \n" \
                                                    "/WCSim/PMTQEMethod " + PMTQEMethod + "\n" \
                                                    "/WCSim/SavePi0 " + SavePi0 + "\n" \
                                                    "/DAQ/TriggerNHits/Threshold " + DAQnhitsthreshold + "\n" \
                                                    "/DAQ/Digitizer " + DAQdigitizer + "\n" \
                                                    "/DAQ/Trigger " + DAQtrigger + "\n" \
                                                    "/DarkRate/SetDarkRate " + DarkNoiseRate + " kHz \n" \
                                                    "/DarkRate/SetConvert " + DarkNoiseConvert + "\n" \
                                                    "/DarkRate/SetDarkMode " + str(args.DarkNoiseMode) + "\n" \
                                                    "" + darknoisewindow + "" \
                                                    "/mygen/generator normal " + "\n" \
                                                    "/gun/particle " + args.GunParticle + "\n" \
                                                    "/gun/energy " + GunEnergy + " MeV \n" \
                                                    "/gun/direction " + " ".join(i for i in args.GunDirection) + "\n" \
                                                    "/gun/position " + " ".join(i for i in args.GunPosition) + "\n" \
                                                    "/WCSimIO/RootFile " + filenamestub + ".root" + "\n" \
                                                    "/run/beamOn " + str(args.NEvents) + "\n"

                                                #create the config file
                                                f = open(filenamestub + '.mac', 'w')
                                                f.write(text)
                                                f.close()

                                                #submit the job
                                                command = 'WCSim ' + filenamestub + '.mac &> ' + filenamestub + '.out'
                                                os.system(command)
                                                
                                                print "file:", counter
                                                print text
                                                print command
                                                counter += 1
