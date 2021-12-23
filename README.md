# Welcome to WCSim

WCSim is a very flexible GEANT4 based program for developing and
simulating large water Cherenkov detectors.

As of August 2014 WCSim has been moved to GitHub.  It can be found at:

https://github.com/WCSim/WCSim

Tutorials and information about the branches and WCSim development model can be
found on the wiki:

https://github.com/WCSim/WCSim/wiki

WCSim has very few external dependencies.  It relies on only ROOT and Geant4.

There is a mailing list which will send you GitHub push/checkin
notifications here:

https://lists.phy.duke.edu/mailman/listinfo/wcsim-git

You can follow issues/requests etc by watching the GitHub respository.

## Validation Webpage

WCSim uses Travis CI to perform build and physics tests for each pull request and commit.
The scripts it runs can be found at https://github.com/WCSim/Validation

The output can be found at: https://wcsim.github.io/Validation/

## Documentation

More detailed information about the simulation is available in
`doc/DetectorDocumentation.pdf`

doxygen documentation can be built by running
`cd $WCSIMDIR/doc && make`

## Build Instructions

As of the Hyper-Kamiokande hybrid configuration, the following prerequsite software is required
* A `-std=c++0x` or `-std=c++11` compatible compiler. e.g. gcc 4.8.5
* ROOT v5r34p36 or v5r34p38 (all v5r34 probably works). ROOT v6 also works (6.20/04 has been tested)
* GEANT 4.10.03p3 (or later)
  * All of the G4 data files are also required to be installed, include hadron xsec, etc.
* cmake3 (and ROOT & GEANT4 compiled with cmake)
* The environment variable `$WCSIMDIR` should be defined as your WCSim directory (not the build one).
```bash
export WCSIMDIR=`pwd`
```

### Build Instructions using CMake:

CMake is cross-platform software for managing the build process in 
a compiler-independent way (cmake.org). 
It is recommended to build ROOT and GEANT4 also through CMake. The 
latter is very CMake friendly since GEANT 4.9.6, while it started introducing
builds through CMake from 4.9.4 onwards (http://geant4.web.cern.ch/geant4/support/ReleaseNotes4.9.4.html#10.).
Using cmake, builds and source code need to well separated and make
it easier to build many versions of the same software.

A recommended way to set up the directory structure in your own
preferred WCSIM_HOME:
- `${WCSIM_HOME}/WCSim` : contains the src dir, typically the cloned or 
  unzipped code from GitHub
- ${WCSIM_HOME}/WCSim-build : contains directories for each build, eg.
  for each branch you want to test or for different releases, comparing
  debugged versions, etc.
  This directory will contain the executable, the example macros and
  library for ROOT.

To compile you need to have `CMakeLists.txt` in the WCSim source dir.
* `mkdir ${WCSIM_HOME}/WCSim_build/mydir ; cd ${WCSIM_HOME}/WCSim_build/mydir`
* Set up the Geant4_Dir: `export Geant4_DIR=${HOME}/Geant4/install/geant4.9.6.p04`
  (from the make install phase of Geant4)
* `cmake ../../WCSim` : this executes the commands in `CMakeLists.txt` and generates
  the Makefiles for both the ROOT library as the main executable.
* `make clean` : if necessary
* `make` : will first compile the `libWCSimRoot.so` which you need for using
  the ROOT Dict from WCSim (e.g. to read the output files), and then compile WCSim.

To recompile:
* Typically just `make` will be enough and also redo the cmake phase if
  something changed.
* Sometimes you need to `make clean` first.
* When there are problems, try removing `CMakeCache.txt`, and redo the cmake.

Useful cmake commands:
* `make edit_cache` : customize the build.
* `make rebuild_cache` : redo the cmake phase.


#### Build with CMake on sukap:

A script allowing to build WCSim with CMake on sukap is available:
```bash
./make_sukap.sh
```

This script will read the current WCSim directory name and create a directory to hold the different cmake builds. 
For example, in case your current WCSim directory name is 'WCSim', it will create a '../WCSim-build' directory. 

It will then create a directory for your current branch which will have the name of the current branch.
For example, in case your current branch name is `hybridPMT`, you will have:

```bash
WCSim
WCSim-dir
WCSim-dir/hybridPMT
```

Note: the script env_sukap.sh provide PATH toward Geant4 and ROOT directories on sukap. However, the current ROOT directory is pointing to Guillaume Pronost space. The reason is the default ROOT 5.34 available on sukap is not compatible with CMake compilation. This should be modified in the future.

### Using WCSim without building using Docker:

Docker allows you to use WCSim without compiling in an OS independant way. The Docker images are hosted on DockerHub and can be used by following the steps below.

1) Install Docker cross platform instructions can be found at https://www.docker.com/
2) Pull the WCSim image from docker hub by using `docker pull wcsim/wcsim:tag` where `tag` is the tagged version or use the tag `latest` to get the current develop branch 
3) Run the docker image and create a container `docker run --name=WCSim -i -t wcsim/wcsim:tag` this will give you a shell in the container's OS with WCSim already built. 
To save data from inside your docker image mount a local folder in the docker image at runtime and then anything placed in that directory will be available in that folder after exit. To do that run the following `docker run -v local_folder_path:docker_mount_path -i -t wcsim/wcsim:tag`
4) Once you have run the docker image, you will already be in `$WCSIMDIR` and WCSim (and prerequisites) will be setup. Therefore, you can just start running WCSim as normal from this directory
5) To exit the docker image `exit`

(Note: You only need to use the `docker run` command once to create the container. Once created you changes are saved in that container instance and you can start and stop the container at any time with  `docker start WCSim` and `docker stop WCSim`);

Extra docker commands:
1) See all images `docker images`
2) Delete an image `docker rmi imageID`
3) See all containers `docker ps -a`
4) Delete a container `docker rm ContainerID`

Note that this method currently uses make (not cmake) to build

### Using WCSim without building using Singularity

Singularity is a similar container tool with different philosphies. The most important being that you can't run as root. This means that it may be installed and available to use on your local cluster.

You should be able to run the docker container with singularity without any problems. Just to note that `$WCSIMDIR` will be read-only, therefore you should run WCSim elsewhere (if you forget you'll see a nasty seg fault - this is just because of the read-only directory).

Note that this method currently uses make (not cmake) to build

## Running WCSim

To test that WCSim is working, try running the test macro `WCSim.mac`, which runs 10 electrons with 500 MeV of energy in the Super-Kamiokande detector. The command is one of the following, depending on how WCSim was built:

`./bin/Linux-g++/WCSim WCSim.mac`

`./exe/bin/Linux-g++/WCSim WCSim.mac`

`WCSim.mac` is well commented. Take a look inside (and also at other `.mac` files in `/macros/`) for the various options you can use to run WCSim

For the Hyper-K hybrid geometry, see `WCSim_hybrid.mac`

## Analysing the output of WCSim

There are a few example scripts inside `sample-root-scripts/`

* `sample_readfile.C` is a general script that allows you to see (most) event-level variables
* For the hybrid geometry, see `AnalyzeWSHierarchy.c`
  * Compile it using the `Makefile` in `sample-root-scripts/` (i.e. run `make` from that folder)
  * Execute it using `Analyzer.sh` in `sample-root-scripts/` (i.e. run `./Analyzer.sh` from that folder)

## Color Convention for visualization used in `WCSimVismanager.cc`

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
