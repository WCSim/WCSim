from HKWCSimMacro import HKWCSimMacro


class HKODWCSimMacro(HKWCSimMacro):

    def createGeometry(self,
                       PMTODRadius = 3,
                       ODLateralWaterDepth = 1,
                       ODHeightWaterDepth = 2,
                       ODLateralWaterDepthUnit = 'm',
                       ODHeightWaterDepthUnit = 'm',
                       ODDeadSpace = 600,
                       ODDeadSpaceUnit = 'mm',
                       ODTyvekSheetThickness = 1,
                       ODTyvekSheetThicknessUnit = 'mm',
                       ODWLSPlatesThickness = 1,
                       ODWLSPlatesThicknessUnit = 'cm',
                       ODWLSPlatesLength=48,
                       ODWLSPlatesLengthUnit='cm',
                       PMTODperCellHorizontal = 3,
                       PMTODperCellVertical = 2,
                       PMTODPercentCoverage = 0.42,
                       ODPMTShift = 0):

        # HyperK
        self.macro += '/WCSim/WCgeom HyperKWithOD\n'

        self.macro += '/WCSim/HyperKOD/PMTODRadius {0}inch\n'.format(PMTODRadius)
        self.macro += '/WCSim/HyperKOD/ODLateralWaterDepth {0} {1}\n'.format(ODLateralWaterDepth,
                                                                             ODLateralWaterDepthUnit)
        self.macro += '/WCSim/HyperKOD/ODHeightWaterDepth {0} {1}\n'.format(ODHeightWaterDepth,
                                                                            ODHeightWaterDepthUnit)
        self.macro += '/WCSim/HyperKOD/ODDeadSpace {0} {1}\n'.format(ODDeadSpace,
                                                                     ODDeadSpaceUnit)
        self.macro += '/WCSim/HyperKOD/ODTyvekSheetThickness {0} {1}\n'.format(ODTyvekSheetThickness,
                                                                               ODTyvekSheetThicknessUnit)
        self.macro += '/WCSim/HyperKOD/ODWLSPlatesThickness {0} {1}\n'.format(ODWLSPlatesThickness,
                                                                              ODWLSPlatesThicknessUnit)
        self.macro += '/WCSim/HyperKOD/ODWLSPlatesLength {0} {1}\n'.format(ODWLSPlatesLength,
                                                                         ODWLSPlatesLengthUnit)
        self.macro += '/WCSim/HyperKOD/PMTODperCellHorizontal {0}\n'.format(PMTODperCellHorizontal)
        self.macro += '/WCSim/HyperKOD/PMTODperCellVertical {0}\n'.format(PMTODperCellVertical)
        self.macro += '/WCSim/HyperKOD/PMTODPercentCoverage {0}\n'.format(PMTODPercentCoverage)
        self.macro += '/WCSim/HyperKOD/ODPMTShift {0} {1}\n'.format(ODPMTShift, 'm')

        # Update geom
        self.macro += '/WCSim/Construct\n'

    def createODDAQOptions(self):
        # Dark Rate Tank
        DarkRateDetEl = '/DarkRate/SetDetectorElement OD\n'
        DarkRateMode = '/DarkRate/SetDarkMode 1\n'
        DarkRateLow = '/DarkRate/SetDarkLow 0\n'
        DarkRateHigh = '/DarkRate/SetDarkHigh 100000\n'
        DarkRateWindow = '/DarkRate/SetDarkWindow 4000\n'
        DarkRate = '/DarkRate/SetDarkRate 0 kHz\n'
        self.macro += DarkRateDetEl
        self.macro += DarkRateMode
        self.macro += DarkRateLow
        self.macro += DarkRateHigh
        self.macro += DarkRateWindow
        self.macro += DarkRate
