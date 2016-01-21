#!/usr/bin/env python

"""
Script to create and run a lot of WCSim .mac files

Run examples:

e.g. 10000 events per configuration in the HyperK geometry, 0 or 8.4 kHZ dark noise, with 5,10,20,50 MeV electrons using a fixed position & direction
(produces 8 files total)

python generate_mac_files.py --batchmode local --WCgeom HyperK --DarkNoiseRate 0,8.4 --GunParticle e- --GunEnergy 5,10,20,50 --GunPosition 0,0,0 --GunDirection 1,0,0 --NEvents 10000

e.g. the same, but producing the particles at random positions and directions
(produces 8 files total)

python generate_mac_files.py --batchmode local --WCgeom HyperK --DarkNoiseRate 0,8.4 --GunParticle e- --GunEnergy 5,10,20,50 --GunPosition random --GunDirection 4pi --NEvents 10000
"""

import argparse
import shutil
import sys
import os
import itertools
from collections import OrderedDict

delim_list = lambda s: list(set(s.split(',')))
delim_list_str = lambda s: s.split(',') if len(s.split(',')) == 3 else s

DAQdigitizer_choices = ['SKI', 'SKI_SKDETSIM']
DAQtrigger_choices = ['NDigits', 'NDigits2', 'SKI_SKDETSIM']
DAQtrigger_ndigits_choices = ['NDigits', 'NDigits2', 'SKI_SKDETSIM']
WCgeom_choices = ['HyperK', \
                      'HyperK_withHPD', \
                      'SuperK', \
                      'SuperK_20inchPMT_20perCent', \
                      'SuperK_20inchBandL_20perCent', \
                      'SuperK_12inchBandL_15perCent', \
                      'SuperK_20inchBandL_14perCent', \
                      'Cylinder_12inchHPD_15perCent', \
                      'Cylinder_60x74_20inchBandL_14perCent', \
                      'Cylinder_60x74_20inchBandL_40perCent']
HKwatertargetlength_choices = ['HyperK', 'HyperK_withHPD']
PMTQEMethod_choices = ['Stacking_Only', 'Stacking_And_SensitiveDetector', 'SensitiveDetector_Only']
PMTCollEff_choices = ['on', 'off']
GunPositionChoices = ['center', 'random', 'wall', 'minusx', 'plusx', 'minusz', 'plusz']
GunDirectionChoices = ['towall', 'tocap', '4pi', 'wall']
GunParticleChoices=['e-','e+','mu-','mu+','pi-','pi+','pi0','gamma','p+','n0']
BatchChoices=['local','condor']

def ListAsString(l):
    return ' '.join(str(o)+',' for o in l)[:-1]

parser = argparse.ArgumentParser(description='Run many WCSim jobs with different options. Use , to delimit multiple options', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
#options about how to run this script
parser.add_argument('--onlycreatefiles', action='store_true', help="Do a test run where you create all the files, but don't run WCSim?")
parser.add_argument('--batchmode', type=str, default='local', choices=BatchChoices, help='Where to submit the jobs.')
parser.add_argument('--notifyuseremail', type=str, default='', help='Specify this to get email notifications about jobs')
#options for the .mac files
# geometry
parser.add_argument('--WCgeom', type=delim_list, default='SuperK', help='The water tank geometry. Specify multiple with comma separated list. Choices: '+ListAsString(WCgeom_choices))
parser.add_argument('--HKwatertanklength', type=delim_list, default='49500', help='The size of a HyperK geometry (mm)')
# trigger & digitization
parser.add_argument('--DAQdigitizer', type=delim_list, default='SKI', help='Which digitizer class to use? Specify multiple with comma separated list. Choices: '+ListAsString(DAQdigitizer_choices))
parser.add_argument('--DAQtrigger', type=delim_list, default='NDigits', help='Which trigger class to use? Specify multiple with comma separated list. Choices: '+ListAsString(DAQtrigger_choices))
parser.add_argument('--DAQconstruct', action='store_true', help='Append the /DAQ/Construct line to the end of the DAQ options. TODO remove after PR115 merge')
parser.add_argument('--DAQMultiDigitsPerTrigger', action='store_true', help='Allow multiple digits per trigger window?')
#generic digitizer options
parser.add_argument('--DAQdigideadtime', type=delim_list, default='0', help='What value of the digitizer deadtime should be used (i.e. how long can the digitizer not create new digits)? Specify multiple with comma separated list')
parser.add_argument('--DAQdigiintwindow', type=delim_list, default='200', help='What value of the digitizer integration window should be used (i.e. how long does the digitizer integrate for)? Specify multiple with comma separated list')
#ndigits trigger
parser.add_argument('--DAQndigitsthreshold', type=delim_list, default='25', help='What value of the ndigits trigger threshold should be used (i.e. number of hits/digits)? Specify multiple with comma separated list')
parser.add_argument('--DAQndigitswindow', type=delim_list, default='200', help='What value of the ndigits trigger window should be used (ns)? Specify multiple with comma separated list')
parser.add_argument('--DAQndigitsdontignorenoise', action='store_true', help='Don\'t adjust the NDigits threshold automatically for the dark noise rate?')
parser.add_argument('--DAQndigitssavewindow', type=delim_list, default='-400:+950', help='What value of the pre/post trigger window should digits be saved in (for the ndigits trigger)? Separate pre/post with a ":". Specify multiple pairs with a comma separated list')
#save failures trigger
parser.add_argument('--DAQsavefailuresmode', type=delim_list, default='0', help='Save failed triggers mode. 0: save only events which pass the trigger. 1: save both. 2: save only events which fail the trigger')
parser.add_argument('--DAQsavefailurestime', type=delim_list, default='200', help='For mode 1 & 2, give events which fail the trigger the trigger time')
parser.add_argument('--DAQsavefailuressavewindow', type=delim_list, default='-400:+950', help='What value of the pre/post trigger window should digits be saved in (for the save failures)? Separate pre/post with a ":". Specify multiple pairs with a comma separated list')
#dark noise
parser.add_argument('--DarkNoiseRate', type=delim_list, default='4.2', help='Dark noise rate (kHz). Specify multiple with comma separated list')
parser.add_argument('--DarkNoiseConvert', type=delim_list, default='1.367', help='Convert dark noise frequency before digitization to after digitization by setting suitable factor. Specify multiple with comma separated list')
parser.add_argument('--DarkNoiseMode', type=int, default=1, choices=[0,1], help='0: apply noise in a specified time window. 1: apply noise around hits. Choose exactly one')
parser.add_argument('--DarkNoiseWindow', type=delim_list, default='1500', help=' Mode 0: Apply dark noise in range x:y (use : as delimeter!). Mode 1: Apply dark noise in a window around each hit (+-window/2). Specify multiple (for the same mode) with a comma separated list')
# pmt
parser.add_argument('--PMTQEMethod', type=delim_list, default='Stacking_Only', help='How the QE is applied? Specify multiple with comma separated list. Choices: '+ListAsString(PMTQEMethod_choices))
parser.add_argument('--PMTCollEff', type=delim_list, default='on', help='Turn on/off the PMT collection efficiency? Specify multiple with comma separated list. Choices: '+ListAsString(PMTCollEff_choices))
# other
parser.add_argument('--SavePi0', action='store_true', help='Save Pi0 info?')
# particle gun
parser.add_argument('--GunIsGPS', type=str, choices=['','line','cylinder'], help='Use the G4 General Particle Source? Overridden by specifying MakeKin.py-like options to --GunPosition and --GunDirection')
parser.add_argument('--GunParticle', type=str, default='e-', choices=GunParticleChoices, help='Particle gun particle. Choose exactly one (default e-)')
parser.add_argument('--GunEnergy', type=delim_list, default='500', help='Particle gun energy (MeV). Specify multiple with comma separated list')
parser.add_argument('--GunPosition', type=delim_list_str, default='0,0,0', help='Particle gun position. Either a comma-separated 3 vector OR exactly one of '+ListAsString(GunPositionChoices))
parser.add_argument('--GunDirection', type=delim_list_str, default='1,0,0', help='Particle gun direction. Either a comma-separated 3 vector OR exactly one of '+ListAsString(GunDirectionChoices))
parser.add_argument('--NEvents', type=int, default=10, help='Number of events per configuration')
parser.add_argument('--JobsPerConfig', type=int, default=1, help='Number of jobs per configuration (total events generated = JobsPerConfig * NEvents')

def check_input_list(arglist, allowed, parser):
    #check for options that aren't allowed
    for arg in arglist:
        if arg not in allowed:
            parser.print_help()
            print arg, "is an incorrect option"
            sys.exit(1)

def check_input_pairs(args):
    for arg in args:
        opt_str = arg.split(':')
        opt_int = []
        if len(opt_str) != 2:
            parser.print_help()
            print arg, "must be a (common separated list of) colon-separated monotonically-increasing int pair(s). Not a pair!"
            sys.exit(1)
        for i in opt_str:
            try:
                opt_int.append(int(i))
            except:
                parser.print_help()
                print arg, "must be a (common separated list of) colon-separated monotonically-increasing int pair(s). Not ints:", i
                sys.exit(1)
        if opt_int[1] < opt_int[0]:
            parser.print_help()
            print arg, "must be a (common separated list of) colon-separated monotonically-increasing int pair(s). Not monotonically-increasing"
            sys.exit(1)

def main(args_to_parse = None):

    #parse the arguments and check that they're valid
    args = parser.parse_args(args_to_parse)
    check_input_list(args.WCgeom, WCgeom_choices, parser)
    check_input_list(args.PMTQEMethod, PMTQEMethod_choices, parser)
    check_input_list(args.PMTCollEff, PMTCollEff_choices, parser)
    check_input_list(args.DAQdigitizer, DAQdigitizer_choices, parser)
    check_input_list(args.DAQtrigger, DAQtrigger_choices, parser)
    if (type(args.GunPosition) is list and len(args.GunPosition) != 3) \
        or (type(args.GunDirection) is list and len(args.GunDirection) != 3):
        parser.print_help()
        print "GunPosition and GunDirection are three vectors. Specify exactly 3 options!"
        sys.exit(1)
    if (type(args.GunPosition) is str and args.GunPosition not in GunPositionChoices):
        parser.print_help()
        print "GunPosition", args.GunPosition, "not one of", GunPositionChoices
        sys.exit(1)
    if (type(args.GunDirection) is str and args.GunDirection not in GunDirectionChoices):
        parser.print_help()
        print "GunDirection", args.GunDirection, "not one of", GunDirectionChoices
        sys.exit(1)
    if type(args.GunDirection) != type(args.GunPosition):
        parser.print_help()
        print "Must use consistent GunPosition and GunDirection options (i.e. both 3 vectors or both MakeKin.py str options)"
        sys.exit(1)
    if args.DarkNoiseMode == 0:
        check_input_pairs(args.DarkNoiseWindow)
    elif args.DarkNoiseMode == 1:
        for w in args.DarkNoiseWindow:
            try:
                int(w)
            except:
                parser.print_help()
                print "DarkNoiseWindow must be a (comma separated list of) single int value(s) when running in mode 1"
                sys.exit(1)
    check_input_pairs(args.DAQndigitssavewindow)
    check_input_pairs(args.DAQsavefailuressavewindow)
    if args.notifyuseremail != "" and (not "@" in args.notifyuseremail or len(args.notifyuseremail.split()) > 1):
        print 'Invalid notifyuseremail - contains whitespace and/or has no @ sign'
        sys.exit(1)
    
    #Grab the other .mac files
    shutil.copy2(os.path.expandvars("$WCSIMDIR") + "/jobOptions.mac", "./")
    shutil.copy2(os.path.expandvars("$WCSIMDIR") + "/jobOptions2.mac", "./")
    shutil.copy2(os.path.expandvars("$WCSIMDIR") + "/tuning_parameters.mac", "./")
    #and the exectuable
    if args.batchmode == 'condor':
        if not os.path.islink('WCSim'):
            os.symlink(os.path.expandvars("$WCSIMDIR") + "/bin/" + os.path.expandvars("$G4SYSTEM") + "/WCSim", "WCSim")


    #methods to construct sets of .mac file options
    def ConstructVerbosity(args):
        verboptions = "/run/verbose 0 \n" \
            "/tracking/verbose 0 \n" \
            "/hits/verbose 0 \n"
        return [[verboptions], ['']]

    def ConstructGeometry(args):
        #the detector construction options
        geoms = []
        filestubs = []
        # permutations
        permutationDict = OrderedDict()
        permutationDict['/WCSim/WCgeom']  = [x for x in args.WCgeom]
        permutationDict['/WCSim/HyperK/waterTank_Length'] = [x for x in args.HKwatertanklength]
        # create a list of dictionaries for each permutation of the parameter values
        permutationDictList = [ OrderedDict(zip(permutationDict, v)) for v in itertools.product(*permutationDict.values()) ]
        for pDict in permutationDictList:
            #get the gun
            [gunopts, gunstubs] = ConstructParticleGun(args, pDict['/WCSim/WCgeom'])
            #get the options
            geomoptions = ''
            for k,v in pDict.iteritems():
                #SuperK is the default
                if k == '/WCSim/WCgeom' and v == 'SuperK':
                    break
                #HK length only relevant for some geometries
                if k == '/WCSim/HyperK/waterTank_Length':
                    thisgeom = geomoptions.split()[1].strip()
                    if thisgeom not in HKwatertargetlength_choices:
                        break
                geomoptions += k + ' ' + v + '\n'
            if geomoptions != '':
                geomoptions += "/WCSim/Construct \n"
            for gunopt in gunopts:
                geoms.append(geomoptions + gunopt)
            #assemble the filename
            filestub = pDict['/WCSim/WCgeom']
            if filestub in HKwatertargetlength_choices:
                filestub += '_' + pDict['/WCSim/HyperK/waterTank_Length']
            for gunstub in gunstubs:
                filestubs.append(gunstub + '_' + filestub)
        return [geoms, filestubs]

    def ConstructPMT(args):
        #the PMT behaviour options
        pmts = []
        filestubs = []
        # permutations
        permutationDict = OrderedDict()
        permutationDict['/WCSim/PMTQEMethod'] = [x for x in args.PMTQEMethod]
        permutationDict['/WCSim/PMTCollEff']     = [x for x in args.PMTCollEff]
        # create a list of dictionaries for each permutation of the parameter values
        permutationDictList = [ OrderedDict(zip(permutationDict, v)) for v in itertools.product(*permutationDict.values()) ]
        for pDict in permutationDictList:
            #get the options
            pmtoptions = ''
            for k,v in pDict.iteritems():
                pmtoptions += k + ' ' + v + '\n'
            pmts.append(pmtoptions)
            #assemble the filename
            filestub = pDict['/WCSim/PMTQEMethod'] + '_PMTCollEff_' + pDict['/WCSim/PMTCollEff']
            filestubs.append(filestub)
        return [pmts, filestubs]

    def ConstructDAQ(args):
        #make the DAQ digitizer / trigger options
        daqs = []
        filestubs = []
        # permutations
        permutationDict = OrderedDict()
        permutationDict['/DAQ/Digitizer']                             = [x for x in args.DAQdigitizer]
        permutationDict['/DAQ/Trigger']                               = [x for x in args.DAQtrigger]
        permutationDict['/DAQ/DigitizerOpt/DeadTime']                 = [x for x in args.DAQdigideadtime]
        permutationDict['/DAQ/DigitizerOpt/IntegrationWindow']        = [x for x in args.DAQdigiintwindow]
        permutationDict['/DAQ/TriggerSaveFailures/Mode']              = [x for x in args.DAQsavefailuresmode]
        permutationDict['/DAQ/TriggerSaveFailures/TriggerTime']       = [x for x in args.DAQsavefailurestime]
        permutationDict['/DAQ/TriggerSaveFailures/PreTriggerWindow']  = [x.split(':')[0] for x in args.DAQsavefailuressavewindow]
        permutationDict['/DAQ/TriggerSaveFailures/PostTriggerWindow'] = [x.split(':')[1] for x in args.DAQsavefailuressavewindow]
        permutationDict['/DAQ/MultiDigitsPerTrigger']                 = ['true' if args.DAQMultiDigitsPerTrigger else 'false']
        # create a list of dictionaries for each permutation of the parameter values
        permutationDictList = [ OrderedDict(zip(permutationDict, v)) for v in itertools.product(*permutationDict.values()) ]
        for pDict in permutationDictList:
            #only run if both Trigger & Digitizer are both or neither SKI_SKDETSIM
            if pDict['/DAQ/Digitizer'] == 'SKI_SKDETSIM' and pDict['/DAQ/Trigger'] != 'SKI_SKDETSIM':
                continue
            if pDict['/DAQ/Digitizer'] != 'SKI_SKDETSIM' and pDict['/DAQ/Trigger'] == 'SKI_SKDETSIM':
                continue
            #get the options
            daqoptions = ''
            for k,v in pDict.iteritems():
                daqoptions += k + ' ' + v + '\n'
            #get the filename
            filestub = pDict['/DAQ/Digitizer'] + '_digi' + pDict['/DAQ/DigitizerOpt/DeadTime'] + '_' \
                + pDict['/DAQ/DigitizerOpt/IntegrationWindow'] + '_' \
                + pDict['/DAQ/Trigger'] + '_fails' + pDict['/DAQ/TriggerSaveFailures/Mode']
            if pDict['/DAQ/TriggerSaveFailures/Mode'] != '0':
                filestub += "_" + pDict['/DAQ/TriggerSaveFailures/TriggerTime']
            if pDict['/DAQ/MultiDigitsPerTrigger']:
                filestub += '_multidigipertrig'
            #get the NDigits options/filestubs
            additionaloptions   = []
            if pDict['/DAQ/Trigger'] in DAQtrigger_ndigits_choices:
                additionaloptions.append(ConstructNDigitsTrigger(args))
            #assemble the complete set of options
            permutationDictO = OrderedDict()
            permutationDictF = OrderedDict()
            for i, optionset in enumerate(additionaloptions):
                permutationDictO[i] = optionset[0]
                permutationDictF[i] = optionset[1]
            permutationDictListO = [ OrderedDict(zip(permutationDictO, v)) for v in itertools.product(*permutationDictO.values()) ]
            permutationDictListF = [ OrderedDict(zip(permutationDictF, v)) for v in itertools.product(*permutationDictF.values()) ]
            for pDictO,pDictF in itertools.izip(permutationDictListO, permutationDictListF):
                theseoptions = daqoptions
                thesefile    = filestub
                for (kO,vO), (kF, vF) in itertools.izip(pDictO.iteritems(), pDictF.iteritems()):
                    theseoptions += vO
                    thesefile += '_' + vF
                #append the Construct line TODO remove this
                if args.DAQconstruct:
                    theseoptions += '/DAQ/Construct \n'
                daqs.append(theseoptions)
                filestubs.append(thesefile)
        return [daqs, filestubs]

    def ConstructNDigitsTrigger(args):
        #make the NDigits type trigger options
        commands  = []
        filestubs = []
        # permutations
        permutationDict = OrderedDict()
        permutationDict['/DAQ/TriggerNDigits/Threshold']         = [x for x in args.DAQndigitsthreshold]
        permutationDict['/DAQ/TriggerNDigits/Window']            = [x for x in args.DAQndigitswindow]
        permutationDict['/DAQ/TriggerNDigits/AdjustForNoise']    = ['true' if not args.DAQndigitsdontignorenoise else 'false']
        permutationDict['/DAQ/TriggerNDigits/PreTriggerWindow']  = [x.split(':')[0] for x in args.DAQndigitssavewindow]
        permutationDict['/DAQ/TriggerNDigits/PostTriggerWindow'] = [x.split(':')[1] for x in args.DAQndigitssavewindow]
        # create a list of dictionaries for each permutation of the parameter values
        permutationDictList = [ OrderedDict(zip(permutationDict, v)) for v in itertools.product(*permutationDict.values()) ]
        for pDict in permutationDictList:
            #get the options
            options = ''
            for k,v in pDict.iteritems():
                options += k + ' ' + v + '\n'
            commands.append(options)
            #assemble the filename
            filestub = 'NDigits' + pDict['/DAQ/TriggerNDigits/Threshold'] + '_' + pDict['/DAQ/TriggerNDigits/Window']
            filestubs.append(filestub)
        return [commands, filestubs]

    def ConstructDarkNoise(args):
        noises = []
        filestubs = []
        # permutations
        permutationDict = OrderedDict()
        permutationDict['/DarkRate/SetDarkRate'] = [x for x in args.DarkNoiseRate]
        permutationDict['/DarkRate/SetConvert']  = [x for x in args.DarkNoiseConvert]
        permutationDict['/DarkRate/SetDarkMode'] = [str(args.DarkNoiseMode)]
        if args.DarkNoiseMode == 1:
            permutationDict['/DarkRate/SetDarkWindow'] = [x for x in args.DarkNoiseWindow]
        elif args.DarkNoiseMode == 0:
            permutationDict['/DarkRate/SetDarkLow']  = [x.split(':')[0] for x in args.DarkNoiseWindow]
            permutationDict['/DarkRate/SetDarkHigh'] = [x.split(':')[1] for x in args.DarkNoiseWindow]
        # create a list of dictionaries for each permutation of the parameter values
        permutationDictList = [ OrderedDict(zip(permutationDict, v)) for v in itertools.product(*permutationDict.values()) ]
        for pDict in permutationDictList:
            #get the options
            darkoptions = ''
            for k,v in pDict.iteritems():
                darkoptions += k + ' ' + v + '\n'
            noises.append(darkoptions)
            #assemble the filename
            filestub = 'DarkNoiseM' + pDict['/DarkRate/SetDarkMode'] + 'C' + pDict['/DarkRate/SetConvert'] + 'R' + pDict['/DarkRate/SetDarkRate'] + 'W'
            if args.DarkNoiseMode == 1:
                filestub += pDict['/DarkRate/SetDarkWindow'].strip()
            elif args.DarkNoiseMode == 0:
                filestub += pDict['/DarkRate/SetDarkLow'].strip() + ':' + pDict['/DarkRate/SetDarkHigh'].strip()
            filestubs.append(filestub)
        return [noises, filestubs]

    def ConstructParticleGun(args, geom):
        guns = []
        filestubs = []
        for GunEnergy in args.GunEnergy:
            filestub = ''
            if type(args.GunPosition) is str:
                #if GunPosition and GunDirection are string's
                #we need to call MakeKin.py to generate distributions of different positions/directions
                command = '$WCSIMDIR/sample-root-scripts/MakeKin.py ' \
                    '-N ' + str(args.JobsPerConfig) + ' ' \
                    '-n ' + str(args.NEvents) + ' ' \
                    '-t ' + args.GunParticle  + ' ' \
                    '-e ' + GunEnergy  + ' ' \
                    '-v ' + args.GunPosition  + ' ' \
                    '-d ' + args.GunDirection + ' ' \
                    '-w ' + geom
                print command
                os.system(command)
                for ijob in xrange(args.JobsPerConfig):
                    #now create the .kin filename
                    kinname = "%s_%.1fMeV_%s_%s_%s_%03i.kin" % (args.GunParticle.replace("+","plus").replace("-","minus"), float(GunEnergy), args.GunPosition, args.GunDirection, args.WCgeom[0], ijob)
                    print kinname
                    #and finally get the .mac options
                    gunoptions = '/mygen/vecfile ' + os.getcwd() + '/' + kinname + '\n'
                    guns.append(gunoptions)
                filestub += 'makekin_'
            elif args.GunIsGPS == 'line':
                #we're using the G4 General particle source
                #https://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/ForApplicationDeveloper/html/ch02s07.html
                gunoptions = "/mygen/generator laser \n" \
                    "/gps/particle " + args.GunParticle + "\n" \
                    "/gps/energy " + GunEnergy + " MeV \n" \
                    "/gps/pos/centre " + " ".join(i for i in args.GunPosition) + "\n" \
                    "/gps/pos/type Plane \n" \
                    "/gps/pos/shape Rectangle \n" \
                    "/gps/pos/halfx 3700. cm \n" \
                    "/gps/pos/halfy 0. cm \n" \
                    "/gps/pos/rot1 " + " ".join(i for i in args.GunDirection) + "\n" \
                    "/gps/pos/rot2 " + " ".join(i for i in (args.GunDirection[1:] + args.GunDirection[:1])) + "\n" \
                    "/gps/verbose 2 \n" \
                    "/gps/direction " + " ".join(i for i in args.GunDirection) + "\n"
                guns.append(gunoptions)
                filestub += 'gps_'
            elif args.GunIsGPS == 'cylinder':
                #we're using the G4 General particle source
                #https://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/ForApplicationDeveloper/html/ch02s07.html
                gunoptions = "/mygen/generator laser \n" \
                    "/gps/particle " + args.GunParticle + "\n" \
                    "/gps/energy " + GunEnergy + " MeV \n" \
                    "/gps/pos/centre " + " ".join(i for i in args.GunPosition) + "\n" \
                    "/gps/pos/type Volume \n" \
                    "/gps/pos/shape Cylinder \n" \
                    "/gps/pos/radius 3700. cm \n" \
                    "/gps/pos/halfz 3000. cm \n" \
                    "/gps/pos/rot1 " + " ".join(i for i in args.GunDirection) + "\n" \
                    "/gps/pos/rot2 " + " ".join(i for i in (args.GunDirection[1:] + args.GunDirection[:1])) + "\n" \
                    "/gps/verbose 2 \n" \
                    "/gps/direction " + " ".join(i for i in args.GunDirection) + "\n"
                guns.append(gunoptions)
                filestub += 'gpscyl_'
            else:
                #we're using the simple GEANT4 particle gun
                #http://geant4.web.cern.ch/geant4/G4UsersDocuments/UsersGuides/ForApplicationDeveloper/html/Control/UIcommands/_gun_.html
                gunoptions = "/mygen/generator normal " + "\n" \
                    "/gun/particle " + args.GunParticle + "\n" \
                    "/gun/energy " + GunEnergy + " MeV \n" \
                    "/gun/direction " + " ".join(i for i in args.GunDirection) + "\n" \
                    "/gun/position " + " ".join(i for i in args.GunPosition) + "\n"
                guns.append(gunoptions)
            nappends = 1
            if type(args.GunPosition) is str:
                nappends = args.JobsPerConfig
            for i in xrange(nappends):
                filestubs.append(filestub + GunEnergy + args.GunParticle)
        return [guns, filestubs]

    #construct the .mac options and parts of filenames for the different groups
    tempoptions = []
    tempoptions.append(ConstructVerbosity(args))
    #tempoptions.append(ConstructParticleGun(args)) #depends on the geometry, so called in ConstructGeometry
    tempoptions.append(ConstructGeometry(args))
    tempoptions.append(ConstructPMT(args))
    tempoptions.append(ConstructDAQ(args))
    tempoptions.append(ConstructDarkNoise(args))

    #assemble the complete set of options
    options = []
    filestubs = []
    permutationDictO = OrderedDict()
    permutationDictF = OrderedDict()
    for i, optionset in enumerate(tempoptions):
        permutationDictO[i] = optionset[0]
        permutationDictF[i] = optionset[1]
    permutationDictListO = [ OrderedDict(zip(permutationDictO, v)) for v in itertools.product(*permutationDictO.values()) ]
    permutationDictListF = [ OrderedDict(zip(permutationDictF, v)) for v in itertools.product(*permutationDictF.values()) ]
    for pDictO,pDictF in itertools.izip(permutationDictListO, permutationDictListF):
        theseoptions = ''
        thesefile    = 'wcsim'
        for (kO,vO), (kF, vF) in itertools.izip(pDictO.iteritems(), pDictF.iteritems()):
            theseoptions += vO
            if vF:
                thesefile    += '_' + vF

        looper = xrange(args.JobsPerConfig)
        if type(args.GunPosition) is str:
            #need to get the job ID from the .kin (there are already multiple jobs per config)
            ijob = int(theseoptions[theseoptions.find('.kin')-3:theseoptions.find('.kin')])
            looper = [ijob]
        #loop over jobs
        for ijob in looper:
            #add the final bits to the options
            jobname = thesefile + '.' + str(ijob)
            joboptions = theseoptions + ""\
                "/WCSim/random/seed " + str(ijob + 31415) + "\n" \
                "/WCSimIO/RootFile " + jobname + ".root" + "\n" \
                "/WCSim/SavePi0 " + ("true" if args.SavePi0 else "false") + "\n" \
                "/run/beamOn " + str(args.NEvents) + "\n"
            options.append(joboptions)
            filestubs.append(jobname)
        
    #loop over every options set
    counter = 1
    for text, filenamestub in itertools.izip(options, filestubs):
        #write the novis.mac style file
        f = open(filenamestub + '.mac', 'w')
        f.write(text)
        f.close()

        #print the run information
        print "\n\nfile:", counter
        print '\n', filenamestub + '.mac:'
        print text

        counter += 1

        #Submit the job
        Submit(filenamestub, args)
                                        


def Submit(filenamestub, args):
    #generate the command to run
    if args.batchmode == 'local':
        command = '$WCSIMDIR/bin/$G4SYSTEM/WCSim ' + filenamestub + '.mac &> ' + filenamestub + '.out'
    elif args.batchmode == 'condor':
        fcondor = open(filenamestub + '.jdl', 'w')
        condor = ''
        if args.notifyuseremail is not '':
            condor += '' \
                'notify_user    = ' + args.notifyuseremail + '\n' \
                'notification   = Always \n'
        condor += '' \
            'executable     = WCSim \n' \
            'universe       = vanilla \n' \
            'arguments      = ' + filenamestub + '.mac \n' \
            'output         = ' + filenamestub + '.out \n' \
            'error          = ' + filenamestub + '.err \n' \
            'log            = ' + filenamestub + '.log \n' \
            'request_memory = 1000 \n' \
            'getenv         = True \n' \
            'queue 1 \n'
        fcondor.write(condor)
        fcondor.close()
        command = 'condor_submit ' + filenamestub + '.jdl'

    #run the job
    print command
    if not args.onlycreatefiles:
        os.system(command)
    else:
        print 'test run; not actually running WCSim'
                               
if __name__ == "__main__":
    main()

    print "\n\n\nTODO fix defaults - certain variables should be allowed to be not written in the .mac file (e.g. if DarkRate == -99)"

