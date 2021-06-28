from HKODWCSimMacro import HKODWCSimMacro
import random


def Create1TeVUpMuJobs():

    outList = list()

    NbEvts = 1
    TotNbEvts = 10
    NbofJobs = TotNbEvts // NbEvts
    iJob = 0

    while iJob < NbofJobs:
        macFileName = 'DummyJob_{0}'.format(iJob)
        mac = HKODWCSimMacro(outputFileName='DummyOutput_{0}'.format(iJob),
                             macroFileName=macFileName)
        mac.createRandomParameter(ranSeed=random.randint(1, 30000))
        mac.createHeader()
        mac.createGeometry(PMTODRadius=3,
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
                           PMTODperCellHorizontal=3,
                           PMTODperCellVertical=2,
                           PMTODPercentCoverage=0.42,
                           ODPMTShift=0)
        mac.createPMTsOptions()
        mac.createDAQOptions()
        mac.createODDAQOptions()
        mac.createUpGoingMuGenerator(Energy=1,
                                     EnergyUnit='GeV',
                                     EnergySigma=100,
                                     EnergySigmaUnit='MeV')
        mac.createFooter(NbOfEvts=NbEvts)
        iJob += 1

        outList.append(macFileName)

    return outList



# Create1TeVUpMuJobs()
