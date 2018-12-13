from fabric import Connection

from Create1TeVUpMuJobs import Create1TeVUpMuJobs


### Create macros for the jobs
def CreateJobs(mac='WCSim'):
    script = '#!/bin/bash -x\n'

    script += 'source ~/.bashrc\n'

    HyperKDir = '/users/zsoldos/hyperk'
    WCSimDir = HyperKDir + '/WCSim'
    script += 'cd {0}\n'.format(HyperKDir)
    script += 'source env-WCSim_cluster.sh\n'
    script += 'cd {0}\n'.format(WCSimDir)
    script += 'WCSim {0}.mac\n'.format(mac)
    file = open('{0}.sh'.format(mac), 'w')
    file.write(script)
    file.close()


### Need a config file with the local available computer
def CreateListFromClusterCfg(file):
    return open(file, 'r').read().split('\n')


# Function to send script on server and run it
def SendScriptAndRun(c,
                     macFile='',
                     pathOnServer='',
                     scriptFile='',
                     pathScriptOnServer=''):
    c.put(macFile, pathOnServer)  # Send macro
    c.put(scriptFile, pathScriptOnServer)  # Send script
    c.run('cd {0} ; source {1}'.format(pathScriptOnServer, scriptFile))  # Execute script


# Create macro files and keep the name inside List
macFiles = Create1TeVUpMuJobs()

# Create list of servers available
serverList = CreateListFromClusterCfg('QMULCluster.cfg')

# Check how much available threads to run jobs
nProc = 8

# Path to WCSIMDIR
WCSimDir = '/users/zsoldos/hyperk/WCSim'

# Create .sh script to be pushed on server
for fileName in macFiles:
    CreateJobs(fileName)

c = Connection(serverList[1])
print('Welcome you\'re on server')
c.run('uname -n')

SendScriptAndRun(c,
                 macFile=macFiles[0] + '.mac', pathOnServer=WCSimDir,
                 scriptFile=macFiles[0] + '.sh', pathScriptOnServer=WCSimDir)

# for host in serverList:
#     c = Connection(host)
