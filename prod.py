from CreateMacro import *
from CreateJobs import *
from SubmitJobs import *

PMTODR = [3,8]
E = [1,10,100]
PMTHorizontal = [1]
PMTVertical = [2]
PMTShift = [0,0.5,1,1.5,2]

for elR in PMTODR:
    for elE in E:
        for elS in PMTShift:
            name = 'WCSim_{0}inch_{1}GeV_PMTShift{2}'.format(elR,elE,elS)
            mFileName = 'tmp/{0}'.format(name)
            oFileName = 'prodCosmics/{0}'.format(name)
            CreateMacro(PMTODRadius=elR,energy=elE,macroFileName=mFileName,outputFileName=oFileName,ODPMTShift=elS)
                #CreateJobs(mac=mFileName)
            SubmitJobs(script='{0}.mac'.format(mFileName))
