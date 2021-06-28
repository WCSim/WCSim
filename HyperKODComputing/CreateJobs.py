import sys

def CreateJobs(mac='WCSim'):
    script = '#!/bin/sh\n'
    curdir = '/users/zsoldos/hyperk/WCSim'
    script += 'cd {0}\n'.format(curdir)

    script += 'unset G4WORKDIR\n'
    script += 'export G4WORKDIR={0}/exe\n'.format(curdir)
    script += 'unset WCSIMDIR\n'
    script += 'export WCSIMDIR={0}\n'.format(curdir)

    datahome = '/data/zsoldos'
    script += 'source {0}/.cluster/root-v5-34-36/bin/thisroot.sh\n'.format(datahome)

    script += 'source {0}/.cluster/bin/setup_g410.sh\n'.format(datahome)

    script += 'export PYTHONPATH=$PYTHONPATH:/users/zsoldos/hyperk/WCSim/HyperKODComputing/\n'
    script += 'WCSim {0}.mac\n'.format(mac) 
    file = open('{0}.sh'.format(mac), 'w')
    file.write(script)
    file.close()
