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
(Known to work with GEANT 4.10.1p03 and ROOT v5.28.00)  You also need all of the G4
data files including hadron xsecs etc.  Those are the only
requirements.  The code should work with gcc 4.4.7. For v1.6.0 and earlier, use GEANT 4.9.4.p01.

To compile: 
* make clean 
* make rootcint
* make

If you want to use these libraries with an external program then also do:
* make shared      [ For root programs]
* make libWCSim.a  [ Also necessary for the event display?]

More detailed information about the simulation is available in
doc/DetectorDocumentation.pdf.

Build Instructions using CMake:

CMake is cross-platform software for managing the build process in 
a compiler-independent way (cmake.org). 
It is recommended to build ROOT and GEANT4 also through CMake. The 
latter is very CMake friendly since GEANT 4.9.6, while it started introducing
builds through CMake from 4.9.4 onwards (http://geant4.web.cern.ch/geant4/support/ReleaseNotes4.9.4.html#10.).
Using cmake, builds and source code need to well separated and make
it easier to build many versions of the same software.

A recommended way to set up the directory structure in your own
preferred WCSIM_HOME:
- ${WCSIM_HOME}/WCSim : contains the src dir, typically the cloned or 
  unzipped code from GitHub
- ${WCSIM_HOME}/WCSim_build : contains directories for each build, eg.
  for each branch you want to test or for different releases, comparing
  debugged versions, etc.
  This directory will contain the executable, the example macros and
  library for ROOT.

To compile you need to have CMakeLists.txt in the WCSim source dir.
* mkdir ${WCSIM_HOME}/WCSim_build/mydir ; cd ${WCSIM_HOME}/WCSim_build/mydir
* Set up the Geant4_Dir: export Geant4_DIR=${HOME}/Geant4/install/geant4.9.6.p04 
  (from the make install phase of Geant4)
* cmake ../../WCSim : this executes the commands in CMakeLists.txt and generates
  the Makefiles for both the ROOT library as the main executable.
* make clean : if necessary
* make : will first compile the libWCSimRoot.so which you need for using
  the ROOT Dict from WCSim and then compile WCSim.

To recompile:
* Typically just "make" will be enough and also redo the cmake phase if
  something changed.
* Sometimes you need to "make clean" first.
* When there are problems, try removing CMakeCache.txt, and redo the cmake.

Useful cmake commands:
* make edit_cache : customize the build.
* make rebuild_cache : redo the cmake phase.



## Color Convention for visualization used in WCSimVismanager.cc

* gamma = green
* neutrino = yellow
* electron = blue
* positron = red
* muon = white
* muon+ = silver
* proton = magenta
* neutron = cyan

```
WCSim development is supported by the United States National Science Foundation.
```
