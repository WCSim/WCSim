import sys
import math

################################################################################
###################### Formatting the macro file ###############################
################################################################################

def CreateMacro(PMTODRadius=3,ODLateralWaterDepth=1,ODHeightWaterDepth=2,ODDeadSpace=600,ODTyvekSheetThickness=2,ODWLSPlatesThickness=1,PMTODperCellHorizontal=1,PMTODperCellVertical=1,PMTODPercentCoverage=1,ODPMTShift=0,macroFileName='prod',outputFileName='OD.root',energy=1):
    # Sample setup macro with no visualization
    macro = '/run/verbose 0\n'
    macro += '/tracking/verbose 0\n'
    macro += '/hits/verbose 0\n'
    # HyperK
    macro = '/WCSim/WCgeom HyperK\n'

    # Vary OD Dimensions and PMTs number
    macro += '/WCSim/HyperKOD/PMTODRadius {0}inch\n'.format(PMTODRadius)
    macro += '/WCSim/HyperKOD/ODLateralWaterDepth {0} {1}\n'.format(ODLateralWaterDepth, 'm')
    macro += '/WCSim/HyperKOD/ODHeightWaterDepth {0} {1}\n'.format(ODHeightWaterDepth, 'm')
    macro += '/WCSim/HyperKOD/ODDeadSpace {0} {1}\n'.format(ODDeadSpace, 'mm')
    macro += '/WCSim/HyperKOD/ODTyvekSheetThickness {0} {1}\n'.format(ODTyvekSheetThickness, 'mm')
    macro += '/WCSim/HyperKOD/ODWLSPlatesThickness {0} {1}\n'.format(ODWLSPlatesThickness, 'cm')
    macro += '/WCSim/HyperKOD/PMTODperCellHorizontal {0}\n'.format(PMTODperCellHorizontal)
    macro += '/WCSim/HyperKOD/PMTODperCellVertical {0}\n'.format(PMTODperCellVertical)
    macro += '/WCSim/HyperKOD/PMTODPercentCoverage {0}\n'.format(PMTODPercentCoverage)
    macro += '/WCSim/HyperKOD/ODPMTShift {0} {1}\n'.format(ODPMTShift, 'm')

    # Update geom
    update = '/WCSim/Construct\n'
    macro += update

    # PMT QE
    PMTQEMethod = '/WCSim/PMTQEMethod Stacking_Only\n'
    macro += PMTQEMethod

    # PMT Collection efficiency
    PMTCollEff = '/WCSim/PMTCollEff on\n'
    macro += PMTCollEff

    # Save Pi0 info
    SavePi0 = '/WCSim/SavePi0 false\n'
    macro += SavePi0

    # Choose trigger and digitizer
    Digi = '/DAQ/Digitizer SKI\n'
    macro += Digi
    Trig = '/DAQ/Trigger NoTrig\n'
    macro += Trig

    # Grab DAQ options
    DAQ = '/control/execute macros/daq.mac\n'
    macro += DAQ

    # Dark Rate Tank
    DarkRateDetEl = '/DarkRate/SetDetectorElement tank\n'
    macro += DarkRateDetEl
    DarkRateMode = '/DarkRate/SetDarkMode 1\n'
    macro += DarkRateMode
    DarkRateLow = '/DarkRate/SetDarkLow 0\n'
    macro += DarkRateLow
    DarkRateHigh = '/DarkRate/SetDarkHigh 100000\n'
    macro += DarkRateHigh
    DarkRateWindow = '/DarkRate/SetDarkWindow 4000\n'
    macro += DarkRateWindow

    # Dark Rate OD
    DarkRateDetEl = '/DarkRate/SetDetectorElement OD\n'
    macro += DarkRateDetEl
    DarkRateMode = '/DarkRate/SetDarkMode 1\n'
    macro += DarkRateMode
    DarkRateLow = '/DarkRate/SetDarkLow 0\n'
    macro += DarkRateLow
    DarkRateHigh = '/DarkRate/SetDarkHigh 100000\n'
    macro += DarkRateHigh
    DarkRateWindow = '/DarkRate/SetDarkWindow 4000\n'
    macro += DarkRateWindow

    ### Cosmic Muons generator
    #Gen = '/mygen/generator cosmics\n'
    #macro += Gen

    ### GPS generator
    Gen = '/mygen/generator gps\n'
    Gen += '/gps/particle mu-\n'
    Gen += '/gps/pos/type Plane\n'
    Gen += '/gps/pos/shape Rectangle\n'
    Gen += '/gps/pos/centre -50 0 0 m\n'
    Gen += '/gps/pos/halfy 10 m\n'

    Gen += '/gps/ene/type Gauss\n'
    Gen += '/gps/ene/mono {0} GeV\n'.format(energy)
    Gen += '/gps/ene/sigma 100 MeV\n'

    Gen += '/gps/direction 1 0 0\n'

    macro += Gen

### Name output file
    output = '/WCSimIO/RootFile {0}.root\n'.format(outputFileName)
    macro += output

### run
    run = '/run/beamOn 500\n'
    exit = 'exit\n'
    macro += run + exit

    file = open('{0}.mac'.format(macroFileName), 'w')
    file.write(macro)
    file.close()
