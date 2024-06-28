# Welcome to WCSim

WCSim is a very flexible Geant4-based program for developing and
simulating large water Cherenkov detectors.

As of August 2014 WCSim has been moved to GitHub.  It can be found [here](https://github.com/WCSim/WCSim)

WCSim has very few external dependencies.  It relies on only ROOT and Geant4.

There is a mailing list which will send you GitHub push/checkin
notifications [here](https://lists.phy.duke.edu/mailman/listinfo/wcsim-git)

You can follow issues/requests etc by watching the GitHub respository.

## Validation Webpage

WCSim uses GitHub Actions CI to perform build and physics tests for each pull request and commit.
The scripts it runs can be found in [this separate github repository](https://github.com/WCSim/Validation)

Validation results can be found [here](https://wcsim.github.io/Validation/)

## Documentation

> [!CAUTION]
> The WCSim wiki & PDF documentation are outdated.
>
> The doxygen documentation is more correct, but incomplete.

Tutorials and information about the branches and WCSim development model can be
found [on the wiki](https://github.com/WCSim/WCSim/wiki)

More detailed information about the simulation is available in
`doc/DetectorDocumentation.pdf`

Additionally, doxygen documentation is available [here](https://wcsim.github.io/WCSim/annotated.html).
Note that most of the code has no doxygen comments, but all output class member variables are documented. See the WCSimRoot* classes

doxygen documentation can be built locally by running
```bash
cd $WCSIMDIR/ && doxygen WCSim_doxygen_config
```

For admin: A more featured doxygen build can be performed & pushed using an alternative doxygen config file (requires `dot` from `graphviz`)
```bash
git clone -b gh-pages --single-branch git@github.com:WCSim/WCSim.git WCSim-dox
cd WCSim-dox
./get_dox.sh
```
You can also make this featured version locally by
```bash
cd $WCSIMDIR
git clone -b gh-pages --single-branch git@github.com:WCSim/WCSim.git WCSim-dox
doxygen WCSim-dox/WCSim_doxygen_config
```

## Build Instructions

### Dependencies

The automated building testing is currently performed inside the hk-software docker container with the prerequisites:
* Geant4 10.3.3 (installed via CMake)
  * All of the G4 data files are also required to be installed, include hadron xsec, etc.
* ROOT 6.26/04 (installed via CMake)
* gcc 8.5.0
* CMake 3.20.2

And as such this is the most supported version of the software (it is guaranteed to work)

> [!TIP]
> If you just need to read WCSim output files, you can do this without the G4 dependency using the `WCSim_WCSimRoot_only` CMake option (see [here](#wcsim-cmake-build-options)).

Other versions of prerequisite software can be tried, but aren't guaranteed to work. 
* Geant
  * If using a newer version of Geant, it is your responsiblity to assess whether Geant physics changes are significant, relative to the officially supported 4.10.3.3
* ROOT
  * ROOT5 (e.g. v5r34p36 or v5r34p38, or potentially any v5r34) can potentially be made to work. You may need to modify `$WCSIM_BUILD_DIR/src/cmake_install.cmake` to take account of ROOT5 not producing `.pcm` files
  * ROOT6 has been tested on an older version than above (6.20/04)
* CMake 3.1 and above are required by `CMakeLists.txt`, but not all old versions have been tested
* The compiler should support `-std=c++11` (or `-std=c++0x`) e.g. gcc 4.8.5
  * The WCSim CMake step should set your C++ standard to be the same as what was used by the installation of ROOT you are building against

### Build Instructions using CMake

[CMake](cmake.org) is cross-platform software for managing the build process in 
a compiler-independent way.
It is required to build ROOT and Geant4 also through CMake.
Using cmake, builds and source code need to well separated and make
it easier to build many versions of the same software.

A recommended way to set up the directory structure in your own
preferred `WCSIM_HOME`:
- `${WCSIM_HOME}/WCSim` : contains the src dir, typically the cloned or 
  unzipped code from GitHub
- `${WCSIM_HOME}/WCSim_build` : contains directories for each build, eg.
   for each branch you want to test or for different releases, comparing
  debugged versions, etc.
  Each subdirectory like `WCSim_dev` or `WCSim_v1.2` would contain the intermediate files produced
  during compilation and not the finale ones

To compile you need to have `CMakeLists.txt` in the WCSim source dir.
* `mkdir -p ${WCSIM_HOME}/WCSim_build/mydir ; cd ${WCSIM_HOME}/WCSim_build/mydir`
* Set up the dependencies
  ```bash
  source /path/to/thisroot.sh
  source /path/to/geant4.sh
  ```
* `cmake3 ../../WCSim` : this executes the commands in `CMakeLists.txt` and generates
  the Makefiles for both the ROOT library as the main executable.
  * For some OS, `cmake3` is just `cmake`
* `make clean` : if necessary
* `make -j\`nproc\` install` : will
  * first compile `libWCSimRoot.so` which you need for using the ROOT Dict from WCSim (e.g. to read the output files)
  * compile WCSim (`libWCSimCore.so` and the executable `WCSim`)
  * install the software in the build directory (copies things into `bin/`, `include/`, etc.)
* `source bin/this_wcsim.sh` : will setup the environment variables such that you can run WCSim

To recompile:
* Typically just `make install` will be enough and also redo the cmake phase if
  something changed.
* Sometimes you need to `make clean` first.
* When there are problems, try removing `CMakeCache.txt`, and redo the cmake.

Useful cmake commands:
* `make edit_cache` : customize the build.
* `make rebuild_cache` : redo the cmake phase.

#### WCSim cmake build options
* `-DWCSim_Geometry_Overlaps_CHECK=<ON|OFF>` If ON, turns on geometry overlap checking when running the code (slow, but important when setting new detector geometry options). Default: OFF
* `-DWCSim_DEBUG_COMPILE_FLAG=<ON|OFF>` If ON, turns on the gcc debug compiler flag `-g`. Default: OFF
* `-DWCSIM_SAVE_PHOTON_HISTORY_FLAG=<ON|OFF>` If ON, turns on photon scattering/reflection history saving. The data class `WCSimRootCherenkovHitHistory` is used in a similar way as `WCSimRootCherenkovHitTime`. Default: OFF
* `-DWCSim_WCSimRoot_only=<ON|OFF>` If ON, only builds the WCSimRoot library (ignoring the WCSimCore one). This is useful if one wants only to read the WCSim output files (for e.g. reconstruction) and not running any simulation. Default: OFF

#### Build with CMake on sukap:

A script allowing to build WCSim with CMake on sukap is available:
```bash
cd $WCSIMDIR
./setup/env_sukap.sh
./make.sh
```
Note that `./make.sh` should also work on other systems.

This script will read the current WCSim directory name and create a directory to hold the different cmake builds. 
For example, in case your current WCSim directory name is 'WCSim', it will create a '../WCSim-build' directory. It will also create a `../WCSim-install` directory.

It will then create a directory for your current branch which will have the name of the current branch.
For example, in case your current branch name is `hybridPMT`, you will have:
```bash
WCSim/
WCSim-build/
WCSim-build/hybridPMT/
WCSim-install/
WCSim-install/hybridPMT/
```

Note that by default, just `make install` is run.
You can run `make clean` before `make install` by running `./make.sh clean`

### Build Instructions using GNUmakefile
> [!CAUTION]
> `GNUmakefile` is specifically only for use with ROOT5 and is not supported otherwise.
>
> All output that needs to be trusted must use cmake. To discourage you from using this, the GNUmakefile is hidden in a different branch
>
> The result of a build using the `GNUmakefile` will be different to the result of a CMake build. If you are using WCSim as a dependency for other packages, you will have to update their build paths to find WCSim includes/libraries
```
export WCSIMDIR=/path/to/WCSim/
cd $WCSIMDIR
git clone --depth 1 https://github.com/WCSim/WCSim.git -b makefile --single-branch makefiledir
cp makefiledir/GNUmakefile .
make clean_wcsim
make rootcint
make
```

### Using WCSim without building using Docker

Docker allows you to use WCSim without compiling in an OS independant way. The Docker images are hosted on DockerHub and can be used by following the steps below.

> [!IMPORTANT]
> Automated WCSim Dockerfile builds have stopped working. There have been no requests (yet) to restore them.
>
> In the meantime, if you are a Hyper-K collaborator you can use the hk-software image. (This is what the CI uses)
>
> Alternatively, the old `Dockerfile`s are still available in this repository. You are free to use/modify them to build a newer version of WCSim for your use.
> If you do, please share your `Dockerfile` changes via a pull request.

1) Install Docker cross platform instructions can be found at https://www.docker.com/
2) Get the latest HK container image docker pull ghcr.io/hyperk/hk-software:main
3) Run an instance of the container which will have WCSim and all dependencies installed `docker run --name=WCSim -it ghcr.io/hyperk/hk-software:main`
  * Note: only run once or you will make multiple contianers
  * To save data from inside your docker image mount a local folder in the docker image at runtime and then anything placed in that directory will be available in that folder after exit. To do that run the following `docker run --name=WCSim -it -v local_folder_path:docker_mount_path ghcr.io/hyperk/hk-software:main`
4) The WCSim version in the container will probably be out of date. Let's update it
  ```bash
  cd $WCSIMDIR
  git pull origin develop
  cd ../build-Linux_x86_64-gcc_8-python_3.8.13
  make -j`nproc` install
  ```
5) Run WCSim
6) To exit the docker image `exit`

Note: You only need to use the `docker run` command once to create the container. Once created you changes are saved in that container instance and you can start the container at any time with `docker start -i WCSim`

Extra docker commands:
1) See all images `docker images`
2) Delete an image `docker rmi imageID`
3) See all containers `docker ps -a`
4) Delete a container `docker rm ContainerID`

### Using WCSim without building using Singularity

Singularity is a similar container tool with different philosophies. The most important being that you can't run as root. This means that it may be installed and available to use on your local cluster.

You should be able to run the docker container with singularity without any problems. Just to note that `$WCSIMDIR` and `$WCSIM_BUILD_DIR` will be read-only, therefore you should clone, build, and run a new version of WCSim elsewhere (if you forget you'll see a nasty seg fault when running WCSim - this is just because of the read-only directory).

## Running WCSim

To test that WCSim is working (however you installed it), try running the test macro `WCSim.mac`, which runs 10 electrons with 5 MeV of energy in the Super-Kamiokande detector (it should take less than a minute to run).

```bash
cd $WCSIM_BUILD_DIR
WCSim WCSim.mac macros/tuning_parameters.mac
```

`WCSim.mac` is well commented. Take a look inside (and also at other `.mac` files in `/macros/`) for the various options you can use to run WCSim


## Analysing the output of WCSim

There are a few example scripts inside `sample-root-scripts/`

* `sample_readfile.C` is a general script that allows you to see (most) event-level variables
* For the hybrid geometry, see `AnalyzeWSHierarchy.c`
  * Compile it using the `Makefile` in `sample-root-scripts/` (i.e. run `make` from that folder)
  * Execute it using `Analyzer.sh` in `sample-root-scripts/` (i.e. run `./Analyzer.sh` from that folder)
* For info about the other scripts, see the README inside `sample-root-scripts/`

## Color Convention for visualization used in `WCSimVismanager.cc`

* gamma = green
* neutrino = yellow
* electron = blue
* positron = red
* muon = white
* muon+ = silver
* proton = magenta
* neutron = cyan

***
WCSim development is supported by the United States National Science Foundation.
