#!/usr/bin/python

from optparse import OptionParser
import random
from math import pi,sin,cos,sqrt
import sys



pid = {"pi0":111, "pi+":211, "k0l":130, "k0s":310, "k+":321,
       "e+":-11, "mu+":-13, "tau+":-15, 
       "nue":12, "nuebar":-12, 
       "numu":14, "numubar":-14, 
       "nutau":16, "nutaubar":-16,
       "p+":2212, "n0":2112}

for pname, no in pid.items():
    if pname.endswith('+'):
        pid[pname.replace('+','-')] = -1*pid[pname]





parser = OptionParser()
parser.add_option("-N", "--nfiles", dest="nfiles",
                  help="number of files of particles to produce",
                  metavar="#", default=1)                  
parser.add_option("-n", "--npart", dest="npart",
                  help="number of particles to simulate per file", 
                  metavar="#", default=100)
parser.add_option("-t", "--type", dest="type",
                  help="Particle type to be generated",
                  metavar="TYPE",default="mu-")
parser.add_option("-e", "--energy", dest="energy",
                  help="Particle energy to be generated in MeV",
                  metavar="ENERGY",default=1000.0)
parser.add_option("-v", "--vertex", dest="vertname",
                  help="Type of vertex (center*, random, minusx, plusx, minusz, plusz)",
                  default="center")
parser.add_option("-d", "--direction", dest="dirname",
                  help="Type of direction (4pi*, towall, tocap)",
                  default="4pi")

(options, args) = parser.parse_args()

options.vertname = options.vertname.lower()
options.dirname = options.dirname.lower()



nfiles = int(options.nfiles)
npart = int(options.npart)
energy = float(options.energy)


#Define the particle
particle = {"vertex":(0, 0, 0),
            "time":0,
            "type":pid[options.type],
            "energy":energy,
            "direction":(1,0,0)}


randvert = False
if options.vertname == "center":
    randvert = False
elif options.vertname == "random":
    randvert = True
elif options.vertname == "wall":
    print >>sys.stderr, "Wall not implemented yet"
    sys.exit(3)
elif options.vertname == "minusx":
    particle["vertex"] = (-1000,0,0)
elif options.vertname == "plusx":
    particle["vertex"] = (1000,0,0)
elif options.vertname == "minusz":
    particle["vertex"] = (0,0,-1000)
elif options.vertname == "plusz":
    particle["vertex"] = (0,0,1000)
else:
    print >>sys.stderr, "Don't understand vertex",opttions.vertname
    sys.exit(2)

if options.dirname == "towall":
    randdir = False
    particle["direction"] = (1,0,0)
elif options.dirname == "tocap":
    randdir = False
    particle["direction"] = (0,0,1)
elif options.dirname == "4pi":
    randdir = True
elif options.dirname == "wall":
    print >>sys.stderr, "Wall not implemented yet"
    sys.exit(3)
else:
    print >>sys.stderr, "Don't understand direction",options.dirname
    sys.exit(2)





nue=12
nuebar=-12
numu=14
numubar=-14
nutau=16
nutaubar=-16

nu =   {"type":pid["numu"], "energy":energy+1000.0,
        "direction":(1, 0, 0)}
prot = {"type":pid["p+"], "energy":935.9840,
        "direction":(0, 0, 1)}



def partPrint(p, f, recno):
    f.write("$ begin\n")
    f.write("$ nuance 0\n")
    if randvert:
        rad = 3368.15/2. - 20. #cm
        height = 3620.0 - 20. #cm
        while True:
            x = random.uniform(-rad,rad)
            y = random.uniform(-rad,rad)
            if x**2 + y**2 < rad**2: break
        z = random.uniform(-height/2,height/2)
        f.write("$ vertex %.5f %.5f %.5f %.5f\n" % (x, y, z, p["time"]))
    else:
        f.write("$ vertex %.5f %.5f %.5f %.5f\n" % (p["vertex"]+(p["time"],)) )
    printTrack(nu, f, -1)   # "Neutrino" Track
    printTrack(prot, f, -1) # "Target" track
    f.write("$ info 0 0 %i\n" % recno)
    if randdir:
        th = random.random()*2*pi
        u = 1.-2*random.random()
        x = sqrt(1.-u**2)*cos(th)
        y = sqrt(1.-u**2)*sin(th)
        z = u
        p["direction"] = (x, y, z)
        #th = random.random()*pi
        #phi = random.random()*2*pi
        #p["direction"] = (cos(phi)*cos(th), sin(phi)*cos(th), sin(th))
        
    printTrack(p, f)    # Outgoing Particle Track
    f.write("$ end\n")

def printTrack(p, f, code=0):
    f.write("$ track %(type)i %(energy).5f " % p)
    f.write("%.5f %.5f %.5f %i\n" % (p["direction"]+(code,)))


for fileno in range(nfiles):
    typestr = options.type.replace("+","plus").replace("-","minus")
    
    filename="%s_%.0fMeV_%s_%s_%03i.kin" % (typestr, energy, options.vertname, options.dirname, fileno)

    outfile = open(filename, 'w')

    print ("Writing %i particles to " % npart) + filename

    for i in range(npart):
        partPrint(particle, outfile, i)
    outfile.write("$ stop")
    outfile.close()
