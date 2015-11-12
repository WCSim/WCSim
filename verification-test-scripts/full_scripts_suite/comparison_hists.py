#!/usr/bin/env python                                                                                                                                                  

#arguments:
# histogram name
# xmin
# xmax
# rebinning
#(set xmin/xmax to -1 to not zoom on the x axis, or set them both to -9999.99 to zoom automatically)

hists = [ 
    ["h1vtx0", -9999.99, -9999.99, -1],
    ["h1vtx1", -9999.99, -9999.99, -1],
    ["h1vtx2", -9999.99, -9999.99, -1],
    ["h1triggertype", -9999.99, -9999.99, -1],
    ["h1ndigihits", -9999.99, -9999.99, -1],
    ["h1nrawhits", -9999.99, -9999.99, -1],
    ["h1ntubeshitraw", -9999.99, -9999.99, -1],
    ["h1ntubeshitdigi", -9999.99, -9999.99, -1],
    ["h1ndigihitstrigger", -9999.99, -9999.99, -1],
    ["h1digipe",    -9999.99, -9999.99, -1],
    ["h1digitime",  -9999.99, -9999.99, -1],
    ["h1digitime_noise",  -9999.99, -9999.99, -1],
    ["h1digitime_photon",  -9999.99, -9999.99, -1],
    ["h1digitime_mix",  -9999.99, -9999.99, -1],
    ["h1digiplustriggertime", -9999.99, -9999.99, -1],
    ["h1triggertime", -9999.99, -9999.99, -1],
    ["h1digipeperdigi", -9999.99, -9999.99, -1],
    ["h1digitimeperdigi", -9999.99, -9999.99, -1],
    ["h1hittime", -9999.99, -9999.99, -1],
    ["h1hittime_photon", -9999.99, -9999.99, -1],
    ["h1hittime_noise", -9999.99, -9999.99, -1],
]


#arguments:
# stack name
# histogram 1 name
# histogram 2 name
# xmin
# xmax
# rebinning
# legend 1
# legend 2
stacks = [
    ["hSeventALL_hittime", "h1eventALL_hittime_photon", "h1eventALL_hittime_noise", 0, 3000, -1, "photon", "noise"],
    ["hSeventALL_hittime", "h1eventALL_hittime_photon", "h1eventALL_hittime_noise", 0, 3000, 2, "photon", "noise"],
    ["hSeventALL_hittime", "h1eventALL_hittime_photon", "h1eventALL_hittime_noise", 0, 3000, 5, "photon", "noise"]
]
