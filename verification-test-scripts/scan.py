#!/bin/env python3
from math import pi, sin, cos, sqrt
import numpy as np

run = True

base = ''
with open('base.mac') as f:
    while True:
        l = f.readline()
        if not l:
            break
        base += l
print(base)

det_half_z = 3347.1
det_r      = 3258.38
extreme_z = det_half_z - 500 #5m from cap wall
print('extreme z', extreme_z)
extreme_r = det_r - 500 #5m from barrel wall
print('extreme r', extreme_r)

def GetXY(r, theta):
    return r * cos(theta), r*sin(theta)

def ArrayAsString(a):
    s = ''
    for i, x in enumerate(a):
        if i:
            s += ' '
        s += f'{x:5f}'
    return s

nfiles = 0
def MakeFile(pos, direct, label):
    print(label)
    print('pos', pos)
    print('dir', direct)
    with open(label, 'w') as f:
        s = base.replace('POSITION', ArrayAsString(pos)).replace('DIRECTION', ArrayAsString(direct))
        f.write(s)
        print(s)
    global nfiles
    nfiles += 1

###BARREL
# aim at barrel, from 5m away
r = extreme_r
#scan z in steps of ~5m
nz = 12
for iz in range(nz + 1):
    z = -extreme_z + iz * extreme_z * 2 / nz
    #scan around the azimuth in 10 degree steps
    ntheta = 36
    for itheta in range(ntheta):
        theta = -pi + itheta * 2 * pi / ntheta
        x, y = GetXY(r, theta)
        pos    = np.array([x, y, z])
        #we want to go straight into barrel with these,
        # so use z=0 for creating the direction vector
        pos_for_direction = np.array([x, y, 0])
        direct = pos_for_direction / np.linalg.norm(pos_for_direction)
        if run:
            MakeFile(pos, direct, f'scan_barrel_z_i{iz}_azimu_i{itheta}.mac')

###CAP
#aim at cap from 5m away
z = extreme_z
#scan lines or r, with deltaR ~ 5m
nr = 12
for ir in range(nr + 1):
    r = - extreme_r + ir * extreme_r * 2 / nr
    #perform a star, going across lines of x=0, y=0, x=y, x=-y
    for imode in range(4):
        if imode == 0:
            x = r
            y = 0
        elif imode == 1:
            x = 0
            y = r
        elif imode == 2:
            theta = pi/4
            x,y = GetXY(r, theta)
        elif imode == 3:
            theta = 3*pi/4
            x,y = GetXY(r, theta)
        pos = np.array([x, y, z])
        #we want to go straight into the top cap
        direct = np.array([0, 0, 1])
        if run:
            MakeFile(pos, direct, f'scan_topcap_r_i{ir}_mode_i{imode}.mac')
        #swap z for the bottom cap
        pos[2] = -pos[2]
        direct[2] = -direct[2]
        if run:
            MakeFile(pos, direct, f'scan_bottomcap_r_i{ir}_mode_i{imode}.mac')

#CORNER
#we want to aim at the space, ~5m from the space
z = extreme_z
r = extreme_r
#scan around the azimuth in 10 degree steps
ntheta = 36
factor = 1 / sqrt(2)
for itheta in range(ntheta):
    theta = -pi + itheta * 2 * pi / ntheta
    x, y = GetXY(r, theta)
    pos    = np.array([x, y, z])
    #we want to go at 45 degrees
    # but we're in bad coordinates to do that
    #first we can see what direction we need to go in the X-Y plane
    pos_xy = np.array([x, y])
    direct_xy = pos_xy / np.linalg.norm(pos_xy)
    #now we know that the relative ratio between the X-Y plane & Z @ 45 degrees
    # is 1/sqrt(2)
    # so Set up the direction vector like this
    direct_xy *= factor
    direct = np.array([direct_xy[0], direct_xy[1], factor])
    if run:
        MakeFile(pos, direct, f'scan_topcorner_azimu_i{itheta}.mac')
    #swap z for the bottom cap
    pos[2] = -pos[2]
    direct[2] = -direct[2]
    if run:
        MakeFile(pos, direct, f'scan_bottomcorner_azimu_i{itheta}.mac')


print(nfiles, 'total files produced')
