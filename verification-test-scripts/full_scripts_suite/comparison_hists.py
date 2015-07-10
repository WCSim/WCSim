#!/usr/bin/env python                                                                                                                                                  

#arguments:
# histogram name
# xmin
# xmax
# rebinning

hists = [ 
    ["h1nhits", 0, 400, -1],
    ["h1nhits", 0, 100, -1],
    ["h1nhits", 1500, 2500, 73],
    ["h1nhits", 250, 700, -1],
    ["h1nhitstrigger", 0, 400, -1],
    ["h1time",  500, 2000, -1],
    ["h1time2",  600, 2400, -1],
    ["h1pe",    0, 500, -1],
    ["h1pe",    0, 100, -1],
    ["h1pe",    3500, 5000, 73],
    ["h1pe",    350, 750, -1],
    ["h1peperdigi", 0, 10, -1],
    ["h1timeperdigi", 600, 1100, -1],
    ["h1triggertime", 0,300, -1],
    ["h1triggertime", 0,300, 5],
    ["h1inttime", 1,400, -1],
    ["h1inttimenoise", 1,400, -1],
    ["h1inttimephoton", 1,400, -1],
    ["h1inttimemix", 1,400, -1],
    ["h1eventALL_hittime_photon", 0, 400, -1],
    ["h1eventALL_hittime_noise", -1, -1, -1]
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
