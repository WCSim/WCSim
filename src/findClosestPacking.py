#!/usr/bin/env python
import math
import numpy as np
from scipy.optimize import root, brentq #root needs scipy 0.11.0 (python > 2.7?)

print 'Make sure you have scipy > 0.11.0 or adjust the rootsolver function'
#eq. 11 from Meas. Sci. Technol. 10 (1999) 1015-1019.
def view_angle_func(x,alpha_prev,eta_prev, n_in_circ):
    if np.cos(x*math.pi/180)*np.cos(x*math.pi/180) - np.sin((alpha_prev + x + eta_prev)*math.pi/180)*np.sin((alpha_prev + x + eta_prev)*math.pi/180) < 0:
        return -100
    return math.sqrt(np.cos(x*math.pi/180)*np.cos(x*math.pi/180)
                     - np.sin((alpha_prev + x + eta_prev)*math.pi/180)*
                     np.sin((alpha_prev + x + eta_prev)*math.pi/180))/np.cos((alpha_prev+x+eta_prev)*math.pi/180) - np.cos(math.pi/n_in_circ)


id_spacing = 1.33 # meter (1 PD/1.5m^2)
nPMT = 33
min_angle = 13   # np.arctan( radius + expose + outer_thickness + dist_glass_to_cover/ id_spacing) : (245mm + 15.3mm + 16 mm + 2.5 mm / sqrt(1500 mm))
configs = []
# Circle 1
i = nPMT
while i > 11:
    # Circle 2
    j = nPMT-i
    while j > 0:
        # Circle 3
        k = nPMT-i-j
        while k > 0:
            # Circle 4
            l = nPMT-i-j-k
            while l > 0:
                #'''
                # Circle 5: too slow
                #m = nPMT-i-j-k-l
                #while m > 0:
                for m in [0,1]:
                    if i+j+k+l+m == nPMT and k > 1 and l > 1 and j > 1:
                        configs.append((i,j,k,l,m))
                    #m -= 1
                #'''
                if i+j+k+l == nPMT and k > 1 and j > 1:
                    configs.append((i,j,k,l,0))
                l -= 1
            if i+j+k == nPMT and j > 1:
                configs.append((i,j,k,0,0))
            k -= 1
        if i+j == nPMT:
            configs.append((i,j,0,0,0))
        j -= 1
    if i == nPMT:
        configs.append((i,0,0,0,0))
    i -= 1

###
# Next: for each config: calc eta for each circle
# Take smallest viewing angle per config, take largest over all configs
# Use minimum alpha
# Solution: for nPMTs, nCircles with nPMTs per circle
###
view_angles_conf = []  #only minimum angle per conf
alpha_conf = dict()
eta_conf = dict()
for conf in configs:
    alpha0 = min_angle
    eta0 = 0
    view_angles = []
    alphas = []
    etas = []
    for nPMT_per_circ in conf:
        if nPMT_per_circ > 1:
            sol = root(view_angle_func, 0.01, (alpha0, eta0, nPMT_per_circ))
            #print conf, nPMT_per_circ, sol.x[0]
            alpha0 = alpha0 + eta0 + sol.x[0]
            eta0 = sol.x[0]
            etas.append(eta0)
            alphas.append(alpha0)
            view_angles.append(sol.x[0])
        elif nPMT_per_circ == 1:
            alphas.append(90)
            etas.append(90-eta0-alpha0)
            view_angles.append(90-eta0-alpha0)
            #print conf, nPMT_per_circ, 90-eta0-alpha0
    alpha_conf[conf] = alphas
    eta_conf[conf] = etas
    if len(view_angles) > 1:
        view_angles_conf.append(min(view_angles))
    elif len(view_angles) == 1:
        view_angles_conf.append(view_angles[0])
        
opt_view_angle = max(view_angles_conf)
print "Optimal viewing angle %.2lf " %(opt_view_angle)

print "Total number of PMTs: %i" %(nPMT)
print "Percentage of covered hemispherical surface = %.2lf %%" %(nPMT*(1-np.cos(opt_view_angle*math.pi/180))*100)
print "Percentage of covered hemispherical surface above min. tilt= %.2lf %%" %(nPMT*(1-np.cos(opt_view_angle*math.pi/180))*100/
                                                                               (1-np.cos(math.pi/2 - min_angle*math.pi/180)))




i = 0
candidate = 0
for conf in configs:
    if abs(view_angles_conf[i]-max(view_angles_conf)) < 0.1: #1:
        candidate += 1
        print conf, view_angles_conf[i]
        # Write best one(s) to separate txt file.
        # I want full config, the eta's and the alpha's
        outfile = open('mPMTconfig_%i_%i_%i.txt'%(nPMT,min_angle,candidate),'w')
        for j in conf:
            outfile.write('%i '%j)
        outfile.write('\n')
        for j in eta_conf[conf]:
            outfile.write('%.3lf '%j)
        outfile.write('\n')
        for j in alpha_conf[conf]:
            outfile.write('%.3lf '%j)
        outfile.write('\n')
        
        outfile.close()
    i += 1


