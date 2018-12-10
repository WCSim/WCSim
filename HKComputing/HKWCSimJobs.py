from paramiko import SSHClient

### Create macros for the jobs
def CreateJobs(mac='WCSim'):
    script = '!/bin/bash -x\n'

    wcsimdir = '/data/zsoldos/hyperk'
    script += 'cd {0}\n'.format(wcsimdir)

    script += 'WCSim {0}.mac\n'.format(mac)
    file = open('{0}.sh'.format(mac), 'w')
    file.write(script)
    file.close()

### Need a config file with the local available computer
def openClusterCfg(file):
    return open(file,'r').read().split('\n')


def testMacro(mac='test'):
    script = '#!/bin/sh\n'

    curdir = '/data/zsoldos/hyperk'
    script += 'cd {0}\n'.format(curdir)

    script += 'pwd\n'

    file = open('{0}.sh'.format(mac), 'w')
    file.write(script)
    file.close()

# from HKODWCSimMacro import HKODWCSimMacro

# mac = HKODWCSimMacro()
# mac.createHeader()
# mac.createGeometry()
# mac.createPMTsOptions()
# mac.createDAQOptions()
# mac.createODDAQOptions()
# mac.createSandMuGenerator()
# mac.createFooter()