from HKODWCSimMacro import HKODWCSimMacro
import random


def CreateCosmicsJobs():

    outList = list()

    NbEvts = 50
    TotNbEvts = 1000
    NbofJobs = TotNbEvts // NbEvts
    iJob = 0

    while iJob < NbofJobs:
        macFileName = '8inchCosmicsJob_{0}'.format(iJob)
        mac = HKODWCSimMacro(outputFileName='8inchCosmicsOutput_{0}'.format(iJob),
                             macroFileName=macFileName)
        mac.createRandomParameter(ranSeed=random.randint(1, 30000))
        mac.createHeader()
        mac.createGeometry(PMTODRadius=8,
                           ODLateralWaterDepth=100,
                           ODLateralWaterDepthUnit='cm',
                           ODHeightWaterDepth=2,
                           ODHeightWaterDepthUnit='m',
                           ODDeadSpace=600,
                           ODDeadSpaceUnit='mm',
                           ODTyvekSheetThickness=1,
                           ODTyvekSheetThicknessUnit='mm',
                           ODWLSPlatesThickness=1,
                           ODWLSPlatesThicknessUnit='cm',
                           ODWLSPlatesLength=48,
                           ODWLSPlatesLengthUnit='cm',
                           PMTODperCellHorizontal=1,
                           PMTODperCellVertical=2,
                           PMTODPercentCoverage=1.,
                           ODPMTShift=0)
        mac.createPMTsOptions()
        mac.createDAQOptions()
        mac.createODDAQOptions()
        mac.createCosmicsGenerator()
        mac.createFooter(NbOfEvts=NbEvts)
        iJob += 1

        outList.append(macFileName)

    return outList



CreateCosmicsJobs()
