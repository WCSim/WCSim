#!/usr/bin/env python                                                                                                                                                  

#arguments:
# histogram name
# xmin
# xmax
# rebinning
#(set xmin/xmax to -1 to not zoom on the x axis, or set them both to -9999.99 to zoom automatically)

hists = [ 
    ["h1nhits", -9999.99, -9999.99, -1],
    ["h1nhitstrigger", -9999.99, -9999.99, -1],
    ["h1time",  -9999.99, -9999.99, -1],
    ["h1time2", -9999.99, -9999.99, -1],
    ["h1pe",    -9999.99, -9999.99, -1],
    ["h1peperdigi", -9999.99, -9999.99, -1],
    ["h1timeperdigi", -9999.99, -9999.99, -1],
    ["h1triggertime", -9999.99, -9999.99, -1],
    ["h1inttime", -9999.99, -9999.99, -1],
    ["h1inttimenoise", -9999.99, -9999.99, -1],
    ["h1inttimephoton", -9999.99, -9999.99, -1],
    ["h1inttimemix", -9999.99, -9999.99, -1],
    ["h1eventALL_hittime_photon", -9999.99, -9999.99, -1],
    ["h1eventALL_hittime_noise", -9999.99, -9999.99, -1]
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
