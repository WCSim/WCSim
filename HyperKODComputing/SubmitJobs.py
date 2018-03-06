import shlex, subprocess

def SubmitJobs(script='tmp/WCSimJob.sh'):
    #command_line = 'qsub -o {0}.out -e {1}.e {2}'.format(script, script, script)
#    command_line = 'qsub -cwd {0}'.format(script)
    command_line = 'WCSim {0}'.format(script)
    args = shlex.split(command_line)
    #print args
    subprocess.Popen(args)
