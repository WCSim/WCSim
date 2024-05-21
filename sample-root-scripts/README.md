# Sample ROOT scripts

## Inputs:
- the directory where the library (`libWCSimRoot.so`) resides is assumed to be in the above directory (`../libWCSimRoot.so`) unless the `$WCSIM_BUILD_DIR` environment variable, that defines the directory where the WCSim package is, is set.
  - Alternatively (recommended) use the `rootwc` ROOT wrapper to automatically load all relevant libraries
- the input root file is assumed to be `../wcsim.root`, unless it is given as argument to the macro.

## Files: 
- `sample_readfile.C`
  - A general example macro to show how to access some (but not all) information stored in the output file
  - See `$WCSIMDIR/include/WCSimRoot*.hh` for how to access other information that is not shown in this example
  - Note: there are three arguments in `sample_readfile.C`
    1. The input root filename
	2. The branch you want to use (defaults to `wcsimrootevent`, which stores information for the main PMT type)
	3. Verbose (`true` or `false`). Verbose is `false` by default.
- `read_number_of_PMTs.C`
  - Simply prints how many PMTs, of each type
- `read_PMT.C`
- `testgeo.C`
- `MakeKin.py`
  - Used to create `.kin` files for input. Useful for creating complicated particle guns (e.g. with position, energy, direction with a distribution) when the Geant GPS settings would be too complicated
- `hadd_wcsim_compileMe.C` and `hadd_wcsim_macro.C`
  - Script to allow successful `hadd`-ing of WCSim files. The standard ROOT `hadd` executable cannot be used, due to the complicated `WCSimRoot*` class structures
- `AnalyzeWSHierarchy.c`
- `calcPhotoCoverage.C`
  - Script to generate ID hybrid photocoverage options such that the *extra tower* is not built. Useful for reducing the number of overlaps/killed tracks in your geometry
- `KinSplitter.py`
  - Take an input `.kin` file and combine all input vertices into multiple overlapping WCSim input events, based on the time of each input vertex
  - Creates multiple `.kin` files per input `.kin` file
  - Its main use is for simulating pileup of supernova events from sntools
- `MakeKin.py`
  - Generate particle-gun like `.kin` files for WCSim to read
  - As well as fixed position/direction, randomised position/directions are possible. A few options are implmented
- `nuPRISMconvert.cc`
- `plot_pmts.C`
  - Plot all PMT positions. Useful for checking new geometries (e.g. if you've modified photocoverage options)
- `pmtremove.C`
- `read_OD.C`
- `trigger_OD.C`

