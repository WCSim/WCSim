## Welcome to WCSim

WCSim is a very flexible GEANT4 based program for developing and
simulating large water Cherenkov detectors.

As of August 2014 WCSim has been moved to GitHub.  It can be found at:

https://github.com/WCSim

Tutorials and information about the branches and WCSim development model can be
found on the wiki:

https://github.com/WCSim/WCSim/wiki

WCSim has very few external dependencies.  It relies on only ROOT and Geant4.

There is a mailing list which will send you GitHub push/checkin
notifications here:

https://lists.phy.duke.edu/mailman/listinfo/wcsim-git

You can follow issues/requests etc by watching the GitHub respository.

## Current notes and how to build

Build Instructions:

You should have a recent and working version of ROOT and GEANT4.
(Known to work with GEANT 4.9.4.p01 and ROOT v5.28.00)  You also need all of the G4
data files including hadron xsecs etc.  Those are the only
requirements.  The code should work with gcc 4.4.7.

To compile: 
* make clean 
* make rootcint
* make

If you want to use these libraries with an external program then also do:
* make shared      [ For root programs]
* make libWCSim.a  [ Also necessary for the event display?]

More detailed information about the simulation is available in
doc/DetectorDocumentation.pdf.

## Color Convention for visualization used in WCSimVismanager.cc

* gamma = green
* neutrino = yellow
* electron = blue
* muon = black
* positron = red
* muon+ = white
* proton = magneta
* neutron = grey

```
WCSim development is supported by the United States National Science Foundation.
```
