
This file contains the release notes for each version of WCSim. Release notes can also be found at https://github.com/WCSim/WCSim/tags.


*************************************************************
Recent updates
*************************************************************

Documentation
* Pull request #454 @tdealtry: Remove some outdated documentation files (README.ROOT, Readme_features.txt)

*************************************************************
27/06/2024: Notes for v1.12.13
*************************************************************
Release after changes to WCTE output after #442 adds mPMT geometry behind the radius of the blacksheet

New feature
* Pull request #453 @kmtsui: Allow PMT photocathode tuning parameters & geometry material refractive indices via mac file
* Pull request #438 @guiguem: Adds `-DWCSim_WCSimRoot_only=ON` cmake option to compile lighter-weight WCSimRoot library without Geant4 dependency, for just reading WCSim files

Update
* Pull Request #450 @tdealtry: Removes the `Makefile` from the `develop` branch (if you **need** it, it is available on a separate branch, but it is **not recommended**)
* Pull request #446 @tdealtry: Hard exit when using a PMT QE method other than SensitiveDetector_Only
* Pull request #442 @kmtsui: Adds mPMT geometry behind the radius of the blacksheet (currently used in WCTE geometry only)
* Pull request #439 @guiguem: Use the same C++ standard as was used to compile ROOT

Bug fix
* Pull request #452 @tdealtry: Fix install of HepMC3 via yum (CentOS8 stream is now EOL)
* Pull request #451 @kmtsui: Fix rotation in cosmic muon generator direction/position for IWCD/WCTE geometries
* Pull request #447 #448 #449 @tdealtry: Analysis macros now load library from `$WCSIM_BUILD_DIR/lib/` instead of `$WCSIMDIR/`
* Pull request #443 @tdealtry: Set the true beam track information for the 0th vertex in events with multiple vertices, for muline files
* Pull request #435 @DiegoCostas97: Set the dark rate options correctly in WCTE.mac
* Pull request #432 @tdealtry: Use the same default dark rate time option for all PMT types in WCSim.mac

Documentation
* Pull Request #450 @tdealtry: Improve installation instructions
* Pull request #449 @tdealtry: Improve documentation formatting for the nuance file format
* Pull request #449 @tdealtry: Fix links to G4 information on gun & gps commands
* Pull request #436 @tdealtry: Improve documentation on finding the PMT that created a digitised or true hit

CI
* Pull request #440 #441 @guiguem: Add CI build test for the WCSimRoot library

*************************************************************
18/04/2024: Notes for v1.12.12
*************************************************************

Minor release after #431, after the IWCD geometry change

Update
* Pull request #431 @kmtsui: Update to IWCD geometry to newly proposed size. 3 proposed options for PMT layout are available (A, B, C), with B being the nominal. The old IWCD geometry is also retained

Bug fix
* Pull request #430 @kmtsui: Use G4MultiUnion & G4Voxeliser from Geant4.10.7.2 instead of Geant4.10.5.1, to pickup some bug fixes related to geometry overlaps

*************************************************************
04/04/2024: Notes for v1.12.11
*************************************************************

Minor release after #424, after the WCTE geometry output changed

Update
* Pull request #424 @kmtsui: Allow light to propagate into PMT region that is behind the blacksheet radius. Activated for WCTE geometry only

Bug fix
* Pull request #428 @kmtsui: Fix output file saving of photon history (accessible with build option) which broke with #427

Documentation
* Pull request #423 @tdealtry: Document output file class data members

*************************************************************
27/03/2024: Notes for v1.12.10
*************************************************************

Minor release after #427, removing the superfluous copies of true track information in the output file
Also picking up mac file fixes in WCSim/Validation - so the HK FD CI checks will be really from the HK FD geometry (has been SK since v1.12.8)

New features
* Pull request #387 @DiegoCostas97: Add (optional) AmBe source geometry
* Pull request #387 @DiegoCostas97: New AmBe primary generator, simulating gamma + photon
* Pull request #387 @DiegoCostas97: Addition of creator process to WCSimRootTrack & WCSimRootCherenkovHitTime
* Pull request #427 @tdealtry: `sample_readfile.C` has option to select which PMT branch to analyse

Update
* Pull request #421 @tdealtry: Remove the old build & physics test CI action (calling bash scripts) in favour of the new python version
* Pull request #426 @tdealtry: Turn off creation of the (unfilled) flat tree output file by default
* Pull request #427 @tdealtry: Only store true track information in the 0th trigger of the first PMT type (previously was saved across multiple triggers, and copied into every active PMT branch)

*************************************************************
05/03/2024: Notes for v1.12.9
*************************************************************

Minor release after updates to mPMT aluminium photon propagation & WCTE PMT rotations

Update
* Pull request #419 #420 @tdealtry: Add new HK FD geometry size to MakeKin.py
* Pull request #416 #418 @nickwp: Update the rotations of WCTE PMTs

Bug fix
* Pull request #413 @arturof: Fix the units of the input when using a RooTracker file
* Pull request #417 @kmtsui: Fix PMT copy numbers being duplicated in some circumstances (using non-replic geomtries without a PMT position table)
* Pull request #418 @kmtsui @nickwp: Set small absorption length for Cherenkov photons in aluminium, to prevent Cherenkov photons being killed due to no refractive index being defined. Not defining the refractive index, as this is complex in reality & non-trivial to implement this in Geant without photons travelling faster than `c`, causing the event to be aborted

Documentation
* Pull request #411 @tdealtry: Add extra clarification about geometry options
* Pull request #412 @tdealtry: Fix description of how to control dark noise options for "second ID PMT type" (mPMTs in FD geometry)

*************************************************************
02/02/2024: Notes for v1.12.8
*************************************************************

Minor release after updates to WCTE geometry

New feature
* Pull request #408 @kmtsui: Add option to include calibration deployment system (CDS) in WCTE geometry

Bug fix
* Pull request #409 @kmtsui: Add missing class to the old makefile build
* Pull request #408 @kmtsui: Fix overlaps in WCTE geometry

*************************************************************
31/01/2024: Notes for v1.12.7
*************************************************************

Minor release after updates to WCTE geometry

New feature
* Pull request #401 @nickwp: Allow multiple runs of WCSim to be performed in a single WCSim job

Update
* Pull request #405 @nickwp: Update WCTE geometry (tank height, diameter, PMT positions)

Bug fix
* Pull request #402 @kmtsui: Make /WCSim/PMT/ReplicaPlacement=true work correctly when also using /WCSim/PMT/PositionFile
* Pull request #403 @kmtsui @gondiaz: Initalise `odEdited` to `false` to prevent undefined behaviour

*************************************************************
11/01/2024: Notes for v1.12.6
*************************************************************

Minor release to provide tag for tuning new HK FD geometries

New features
* Pull request #399 @patrickstowell: Addition of realistic HK FD geometry (both w/ & w/o OD PMTs)
* Pull request #397 @kmtsui: `-DWCSIM_SAVE_PHOTON_HISTORY_FLAG=ON` runs in mode with new output information about photon history (scatters, reflections)

Bug fix
* Pull request #398 @arturof: Fixes to make cosmic muon generator work correctly (off by 1 histogram binning errors, crashes due to simulating outside of world volume, incomplete detector coverage of muon trajectories)

*************************************************************
07/12/2023: Notes for v1.12.5
*************************************************************

Minor release to update the reference validation plots after #396

Bug fix
* Pull request #396 @kmtsui: Fix PMT overlap in IWCD geom

*************************************************************
29/11/2023: Notes for v1.12.4
*************************************************************

Minor release to update the reference validation plots after #394

Update
* Pull request #394 @kmtsui: Update default IWCD ID diameter from 7.0 to 6.6 m
* Pull request #393 @kmtsui: Allow `WCSimOpticalPhysics` options to be controlled in `jobOptions.mac`

Bug fix
* Pull request #395 @gondiaz: Install `*.hpp` files
* Pull request #392 @nickwp: Parent of the saved track now correctly saved in the true Cherenkov hit (previously erroneously ID of the photon track)

*************************************************************
15/11/2023: Notes for v1.12.3
*************************************************************

Minor release to update the reference validation plots after #390

New features
* Pull request [WCSim/WCSim#384 @kmtsui](https://github.com/WCSim/WCSim/pull/384): Addition of mPMT LED primary generator. Note these are for LEDs in standard mPMT modules (not mPMT-LEDs)
* Pull request [WCSim/WCSim#383 @JackFannon](https://github.com/WCSim/WCSim/pull/383): Addition of IBD (inverse beta decay) primary generator for DSNB (diffuse supernova neutrino background) studies

Update
* Pull request [WCSim/WCSim#385 @kmtsui](https://github.com/WCSim/WCSim/pull/385): Updates to PMT placement file. e.g. can now say if it is for 20"/mPMT in hybrid configuration, apply offsets from nominal, etc.
* Pull request [WCSim/WCSim#389 @JackFannon](https://github.com/WCSim/WCSim/pull/389): IBD primary generator action: cross section & selection algorithm updated for accuracy and efficiency
* Pull request [WCSim/WCSim#390 @nickwp](https://github.com/WCSim/WCSim/pull/390): Determination of which true tracks to be saved has been overhauled & simplified. Highlights include option to guarentee that the track (and ancestors of the track) that created a hit are saved

Bug fix
* Pull request [WCSim/WCSim#389 @JackFannon](https://github.com/WCSim/WCSim/pull/389): Vertices now generated across entire z span of detector (rather than half height)


*************************************************************
13/10/2023: Notes for v1.12.2
*************************************************************

Minor release to update the reference validation plots after #380 & #382

New features
* Pull request [WCSim/WCSim#380 @ekneale](https://github.com/WCSim/WCSim/pull/380): Addition of light injector source primary generator - OD devices are currently implmented
* Pull request [WCSim/WCSim#379 @JackFannon](https://github.com/WCSim/WCSim/pull/379): Addition of datatable primary generator - a more compact text-based format

Update
* Pull request [WCSim/WCSim#374 @gondiaz](https://github.com/WCSim/WCSim/pull/374): Restructure the CMake files such that the `WCSIM_CHECK_GEOMETRY_OVERLAPS` option is available for WCSim-dependent tools
* Pull request [WCSim/WCSim#374 @gondiaz](https://github.com/WCSim/WCSim/pull/374): Only read in the cosmics data file if using the cosmics primary generator

Bug fixes
* Pull request [WCSim/WCSim#382 @tdealtry](https://github.com/WCSim/WCSim/pull/382): Correctly set the default RelativeHitTime to `false` (when `true`, offsets the time of each hit by the time of the first hit in the list (may not be time of the earliest hit). Useful for radioactivity studies, where there are long times before a decay occurs)
* Pull request [WCSim/WCSim#380 @ekneale](https://github.com/WCSim/WCSim/pull/380): Set RelativeHitTime for OD PMTs using the .mac file option (in the same way as for ID 20" & mPMTs)
* Pull request [WCSim/WCSim#379 @JackFannon](https://github.com/WCSim/WCSim/pull/379): Fix `WCSimPrimaryGeneratorAction::SetTimeUnit()` bug

*************************************************************
19/09/2023: Notes for v1.12.1
*************************************************************

Minor release to update the reference validation plots after #375

Updates
* Pull request [WCSim/WCSim#375 @kmtsui](https://github.com/WCSim/WCSim/pull/375): Improved WCTE mPMT geometry. Causes some minor changes to physics output for other mPMT geometries, due to a 0.0 to -0.0 precision issue
* Pull request [WCSim/WCSim#376 @tdealtry](https://github.com/WCSim/WCSim/pull/376): Don't fall back to automated geometry building when using invalid PMT position file

Bug fixes
* Pull request [WCSim/WCSim#371 @guiguem](https://github.com/WCSim/WCSim/pull/371): Resolve cmake linkage issue

*************************************************************
21/08/2023: Notes for v1.12.0
*************************************************************

Main change is to reactivate the new WCTE geometry

New features
* Pull Request [WCSim/WCSim#370 @kmtsui](https://github.com/WCSim/WCSim/pull/370): Save extra information (position, time, kinetic energy) as trajectories pass the blacksheet/tyvek/cave wall
* Pull Request [WCSim/WCSim#353 @kmtsui](https://github.com/WCSim/WCSim/pull/353): Add new as-built WCTE geometry
* Pull Request [WCSim/WCSim#353 @kmtsui](https://github.com/WCSim/WCSim/pull/353): Add gamma conversion generator (simulates e+/e- pair with correct kinematics at the position you specify)
* Pull Request [WCSim/WCSim#353 @kmtsui](https://github.com/WCSim/WCSim/pull/353): Add new optional photocathode models that improve the description of photons interacting with the photocathode glass
* Pull Request [WCSim/WCSim#353 @kmtsui](https://github.com/WCSim/WCSim/pull/353): Add ability to specify PMT placement via a file of PMT positions/orientations, rather than using the usual WCSim automated placement
* Pull Request [WCSim/WCSim#353 @kmtsui](https://github.com/WCSim/WCSim/pull/353): Add ability to fluctuate PMT placement based on a Gaussian
* Pull Request [WCSim/WCSim#353 @kmtsui](https://github.com/WCSim/WCSim/pull/353): Add ability to rotate the cap PMT placement by half a barrel column, in order to be able to replicate the real WCTE geometry

Updates
* Pull Request [WCSim/WCSim#359 @P3tru](https://github.com/WCSim/WCSim/pull/359): Remove dependence on `ext/functional` header. Should allow easier compilation on macOS
* Pull Request [WCSim/WCSim#359 @P3tru](https://github.com/WCSim/WCSim/pull/359): Template `ComparisonPassed()` functions

*************************************************************
03/08/2023: Notes for v1.11.1
*************************************************************

Main change is to add the updated IWCD geometry, including an IWCD w/ OD for the first time

Note that the WCTE has been disabled in this code, for now. Please use WCTE/WCSim for the most up-to-date WCTE work

New features
* Pull Request [WCSim/WCSim#355 @kmtsui](https://github.com/WCSim/WCSim/pull/355): New IWCD detector geometries (`IWCD_mPMT`, `IWCD_mPMT_WithOD`)

Updates
* Pull Request [WCSim/WCSim#361 @tdealtry](https://github.com/WCSim/WCSim/pull/361): Suppress the compiler warnings we have. Two are benign, warning of the expected behaviour. One is useless and pervasive, hiding real warnings
* Pull Request [WCSim/WCSim#355 @kmtsui](https://github.com/WCSim/WCSim/pull/355): Improvements in NDigits trigger running time
* Pull Request [WCSim/WCSim#352 @tdealtry](https://github.com/WCSim/WCSim/pull/352): Disable the WCTE geometry in this version of the code
* Pull Request [WCSim/WCSim#332 @federiconova](https://github.com/WCSim/WCSim/pull/332): Add some info about the PMTQEMethod recommended for the OD to the `.mac` file
* Pull Request [WCSim/WCSim#221 @tdealtry](https://github.com/WCSim/WCSim/pull/221): Add some info about the NDigits trigger window to the `.mac` file

Bug fixes
* Pull Request [WCSim/WCSim#355 @kmtsui](https://github.com/WCSim/WCSim/pull/355): Add `G4LogicalSkinSurface` for blacksheet, to prevent photon tracks getting stuck
* Pull Request [WCSim/WCSim#271 @marc1uk](https://github.com/WCSim/WCSim/pull/271): Ensure `SaveFailuresPreWindow` and `NDigitsPreWindow` options are read in correctly
* Pull Request [WCSim/WCSim#157 @JostMigenda](https://github.com/WCSim/WCSim/pull/157): Register model factories correctly

*************************************************************
02/06/2023: Notes for v1.11.0
*************************************************************

This is the first full merge between WCSim/WCSim:develop and nuPRISM/WCSim:nuPRISM/develop for ~7 years.  
There are therefore many changes, and it is inevitable that some will not be fully recognised in this list.  
I have not looked at all at PRs before 2019.  
For that I am sorry

The main feature of this release is the addition of mPMTs (multiple 3" PMTs in a 20"-PMT-like vessel),
 and the creation of associated geometries for IWCD and the HK FD.
 
Note that with this release, the recommended version of Geant4 is now 10.3.3

Default output `.root` file changes
* `fTriggerInfo` in `WCSimRootTrigger` is now a `std::vector<Double_t>` (previously was a `std::vector<Float_t>`)
* Addition of `fParentId` to `WCSimRootTrack`
* Addition of `fmPMTID` and `fmPMT_PMTID` to all of `WCSimRootCherenkovHit`, `WCSimRootCherenkovDigiHit`, and `WCSimRootPMT`
* Addition of `fPhotonStartTime`, `fPhotonStartPos[3]`, `fPhotonEndPos[3]`, `fPhotonStartDir[3]`, and fPhotonEndDir[3]` to `WCSimRootCherenkovHitTime`
* Addition of `TClonesArray` of `WCSimRootCapture` (a new class) to `WCSimRootEvent`
  * `WCSimRootCaptureGamma` is another new class stored within `WCSimRootCapture`
* Addition of two new `TClonesArray` of `WCSimRootPMT` in `WCSimRootGeom`
  1. `fPMTArray2` stores the second ID PMT type information (i.e. mPMT information in HK hybrid FD geometries)
  2. `fODPMTArray` stores the OD PMT information (this used to be stored at the end of `fPMTArray`)

New features
* New script `plot_pmts.C` to plot the PMT distribution
* Pull Request [WCTE/WCSim#10 @kmtsui](https://github.com/WCTE/WCSim/pull/10): Automatically switch Geant4 function arguments between G4.10.1 & 4.10.3
* Pull Request [tdealtry/#2 @spradlin](https://github.com/tdealtry/WCSim/pull/2): Restore interactive running mode of WCSim
* Commit [7663873 @spradlin](https://github.com/spradlin/WCSim/commit/7663873c5d11d4cc1a50e535a185e6ed0124c6a4): Add `-DWCSim_Geometry_Overlaps_CHECK` cmake options
* Pull Request [nuPRISM/#63 @akutsuR](https://github.com/nuPRISM/WCSim/pull/63): Adding parent G4 track id
* Pull Request [nuPRISM/#61 @bquilain](https://github.com/nuPRISM/WCSim/pull/61): Merge of HK FD hybrid ID branch with nuPRISM code. Provides new HK ID-only hybrid FD geometry
* Pull Request [nuPRISM/#57 @nickwp](https://github.com/nuPRISM/WCSim/pull/57): Add tracking of photon start and end directions for true hits
* Pull Request [nuPRISM/#54 @nickwp](https://github.com/nuPRISM/WCSim/pull/54): Add git hash to output settings tree. See also nuPRISM/#55
* Pull Request [nuPRISM/#52 @nickwp](https://github.com/nuPRISM/WCSim/pull/52): Add particle tracking options
* Pull Request [nuPRISM/#48 @mshinoki](https://github.com/nuPRISM/WCSim/pull/48): Add WCTE Geometry
* Addition of light injector simulation for primary particle generation
* Addition of nRooTracker input file reading for generating physics events
* Addition of extra `_flat.root` output file format. Note that this has not been validated during the merge, and could contain incorrect or incomplete information - this file is currently an unofficial solution in WCSim/WCSim

Updates
* The default QE application method is now `SensitiveDetector_Only` (i.e. apply at PMT). It was previously `Stacking_Only` (i.e. apply at optical photon creation). Thank you to @spradlin for finding inconsistencies between the methods
* The default values of PMTs in the HK geometries have been modified such that the *extra tower* is not built. This seems to greatly suppress the number of killed tracks
* Deactivation of some old HK geometries. These can be reactivated if they are really required for studies, but they are no longer supported
* Pull Request [tdealtry/#1 @spradlin](https://github.com/tdealtry/WCSim/pull/1): Correct/suppress most compiler warnings
* Pull Request [#338 @guiguem](https://github.com/WCSim/WCSim/pull/338): Upgrades to cmake installation. See all #341 #342 #344
* Pull Request [bquilain/#17 @JacekHoleczek](https://github.com/bquilain/WCSim/pull/17): Various small building improvements
* Pull Request [bquilain/#14 @gpronost](https://github.com/bquilain/WCSim/pull/14): Change holder of the PMT to E61 design
* Pull Request [bquilain/#12 @pdeperio](https://github.com/bquilain/WCSim/pull/12): Merging of nuPRISM code with HK hybrid code. See also bquilain/#11
* Pull Request [nuPRISM/#49 @nickwp](https://github.com/nuPRISM/WCSim/pull/49): Update IWCD geometry to match solidworks mechanical design

Bug fixes
* Pull Request [tdealtry/#3 @spradlin](https://github.com/tdealtry/WCSim/pull/3): Fix a variety of overlap warnings
* Pull Request [tdealtry/#4 @spradlin](https://github.com/tdealtry/WCSim/pull/4): Fix to loop over integral (rather than floating point) types
* Pull Request [nuPRISM/#47 @jmgwalker](https://github.com/nuPRISM/WCSim/pull/47): Fix detector centre value stored in Settings tree in output file
* Pull Request [nuPRISM/#46 @jmgwalker](https://github.com/nuPRISM/WCSim/pull/46): Fix detector offset bug
* Pull Request [nuPRISM/#45 @jmgwalker](https://github.com/nuPRISM/WCSim/pull/45): Fix bug that was causing seg fault when accessing PDG code from NRooTracker
* Pull Request [nuPRISM/#44 @mscott201](https://github.com/nuPRISM/WCSim/pull/44): Update the event generation to generate events right out to the edge of the E61 ID
* Pull Request [bquilain/#19 @gpronost](https://github.com/bquilain/WCSim/pull/19): Use Flag instead of FirstTime value to determine if new FirstTime needs to be set. See all bquilain/#18
* Pull Request [bquilain/#13 @bquilain](https://github.com/bquilain/WCSim/pull/13): Initialise ROOT randomiser seed

*************************************************************
10/26/2021: Notes for v1.10.0
*************************************************************

The main feature of this release is the OD geometry including WLS plates
Note that with this release, WCSim now requires a C++11 (or C++0x) compatible compiler 

Update
* Pull Request [#218](https://github.com/WCSim/WCSim/pull/218): Solve deprecated `ext/hash_map` header warning (now using C++11’s `unordered_map`)
* Pull Request [#266](https://github.com/WCSim/WCSim/pull/266): Expand `MakeKin.py` to allow random uniform distributions of particle energies & times
* Pull Request [#292](https://github.com/WCSim/WCSim/pull/292): Updated radon model
* Pull Request [#307](https://github.com/WCSim/WCSim/pull/307): Make cmake compilation work with ROOT6 (+ scripts to compile on sukap)
* Pull Request [#212](https://github.com/WCSim/WCSim/pull/212): Modify indices saved to `WCSimRootCherenkovDigiHit::GetPhotonIds()`
* Pull Request [#313](https://github.com/WCSim/WCSim/pull/313): Use `G4cout` instead of `printf`

New feature
* Pull Request [#304](https://github.com/WCSim/WCSim/pull/304): `hadd` scripts for combining WCSim files
* Pull Request [#297](https://github.com/WCSim/WCSim/pull/297): Event generator based on cosmic muon flux
* Pull Request [#297](https://github.com/WCSim/WCSim/pull/297): OD geometry including wavelength shifting plates
* github actions version of CI build and run validation

Bug Fix
* Pull Request [#247](https://github.com/WCSim/WCSim/pull/247): Use `map` features, instead of array-style access/iteration, in `WCSimWCDigi`
* Pull Request [#298](https://github.com/WCSim/WCSim/pull/298): Make NoTrigger trigger work for OD
* Pull Request [#312](https://github.com/WCSim/WCSim/pull/312): ROOT `TRandom3` randomizer initialisation

*************************************************************
04/12/2021: Notes for v1.9.4
*************************************************************

This release includes an update to allow `WCSimRootTrigger`s with more than 150 entries, as well as updates to the Travis automatic validation system execution tests

Update
* Pull Request [#308](https://github.com/WCSim/WCSim/pull/308): Update validation tests
* Pull Request [#309](https://github.com/WCSim/WCSim/pull/309): Dynamically expand `WCSimRootTrigger`s `TObjArray`

*************************************************************
12/02/2020: Notes for v1.9.3       
*************************************************************

This release includes a bug fix in the recently committed update. 

Update
* Pull Request [#302](https://github.com/WCSim/WCSim/pull/302): Typo in inFile command in src/WCSimPrimaryGeneratorAction.cc 

*************************************************************
11/26/2020: Notes for v1.9.2        
*************************************************************

This release includes an update to allow for longer headers in kin files - NOTE: A bug prevents this release from compiling

Update
* Pull Request [#299](https://github.com/WCSim/WCSim/pull/299): Increase buffer size for reading in lines of kin file

*************************************************************
10/02/2020: Notes for v1.9.1        
*************************************************************

This release includes a bug fix in a python helper script and support for Python3. 

Bug Fix
* Pull Request [#293](https://github.com/WCSim/WCSim/pull/293): Fix missing events when using kin_converter.py and upgrade to support Python 3

*************************************************************
08/10/2020: Notes for v1.9.0        
*************************************************************

This release includes changes that allow for supernova neutrinos to be more easily simulated. The ability for tools to work in ROOT6 has also been included. 

New Features
* Pull Request [#285](https://github.com/WCSim/WCSim/pull/285): Include compatibilities for ROOT6
* Pull Request [#284](https://github.com/WCSim/WCSim/pull/285): Include compatibilities for Singularity
* Pull Request [#275](https://github.com/WCSim/WCSim/pull/275): Add the ability to simulate a radon background

Bug Fixes
* Pull Request [#286](https://github.com/WCSim/WCSim/pull/286), [#287](https://github.com/WCSim/WCSim/pull/287): Fix a memory leak when running on large (>1000) event files. 

Updates
* Pull Request [#289](https://github.com/WCSim/WCSim/pull/289), [#277](https://github.com/WCSim/WCSim/pull/277): Allow for multiple vertices to have different information. 
* Pull Request [#288](https://github.com/WCSim/WCSim/pull/288): Allow kinsplitter to create multiple kin files for the same event. 
* Pull Request [#279](https://github.com/WCSim/WCSim/pull/279), [#283](https://github.com/WCSim/WCSim/pull/283): Updates for Docker
* Pull Request [#288](https://github.com/WCSim/WCSim/pull/288): Changes required for supernova simulations (including an increase in the stored time precision - this results in a 20\% increase in file size). 
* Pull Request [#272](https://github.com/WCSim/WCSim/pull/272): Added a different (more direct) way to compare digits

*************************************************************
01/29/2019: Notes for v1.8.0        
*************************************************************

The main updates included in this release include an outer detector volume in the Hyper-Kamiokande detector, an option to include light cones, and updates to accommodate our new trigger simulations. 

New Features
* Pull Request [#210](https://github.com/WCSim/WCSim/pull/210), [#251](https://github.com/WCSim/WCSim/pull/251), [#242](https://github.com/WCSim/WCSim/pull/242), [#252](https://github.com/WCSim/WCSim/pull/252), [#253](https://github.com/WCSim/WCSim/pull/253), [#233](https://github.com/WCSim/WCSim/pull/233): Hyper-K configuration with an outer detector is implemented. 
* Pull Request [#206](https://github.com/WCSim/WCSim/pull/206): New macro folder now contains all the macros.
* Pull Request [#229](https://github.com/WCSim/WCSim/pull/229): Digitization of the OD.
* Pull Request [#205](https://github.com/WCSim/WCSim/pull/205): Add option in macro to switch off QE completely.
* Pull Request [#220](https://github.com/WCSim/WCSim/pull/220): Add a no-trigger trigger option
* Pull Request [#223](https://github.com/WCSim/WCSim/pull/223), [#227](https://github.com/WCSim/WCSim/pull/227), [#238](https://github.com/WCSim/WCSim/pull/238): Changes to allow for Travis validation suite
* Pull Request [#236](https://github.com/WCSim/WCSim/pull/236): Changing travis source docker file
* Pull Request [#228](https://github.com/WCSim/WCSim/pull/228): Allow simulation of multiple primary vertices in an event
* Pull Request [#249](https://github.com/WCSim/WCSim/pull/249): Support OGLSQT visualization 
* Pull Request [#246](https://github.com/WCSim/WCSim/pull/246): Allow multiple instances of dark noise options to be stored
* Pull Request [#231](https://github.com/WCSim/WCSim/pull/231): Add 3" and 5" PMT options
* Pull Request [#263](https://github.com/WCSim/WCSim/pull/263): Light cone option is now included

Bug Fixes
* Pull Request [#145](https://github.com/WCSim/WCSim/pull/145): Fix 0 hits report when running hits/verbose 1
* Pull Request [#193](https://github.com/WCSim/WCSim/pull/193): Digitized hits were being floored to an int (introduced in v1.4.0). This pull request makes digit times a float. 
* Pull Request [#194](https://github.com/WCSim/WCSim/pull/194): Moves hit time smearing back to the PMT hit, rather than to the digitized hits.
* Pull Request [#216](https://github.com/WCSim/WCSim/pull/216): Fix typo in WCSimRootOptions::Print()
* Pull Request [#217](https://github.com/WCSim/WCSim/pull/217): Fix floating point exception error caused by non-optical photons with a total energy of 0 in ProcessHits for which the wavelength calculation didn't make sense. Moved the wavelength calculation down to be only for optical photons.
* Pull Request [#225](https://github.com/WCSim/WCSim/pull/225): A nuance-formatted input file created on Windows will usually have \r\n line endings instead of the Unix default \n. Let’s add \r to the list of ignored characters so those files get parsed correctly.
* Pull Request [#255](https://github.com/WCSim/WCSim/pull/255): Add some statements to the LinkDef
* Pull Request [#265](https://github.com/WCSim/WCSim/pull/265): WCSim files generated using muline option have the vertex incorrectly set to (0,0,0). This fixes it.

Updates
* Pull Request [#166](https://github.com/WCSim/WCSim/pull/166), [#254](https://github.com/WCSim/WCSim/pull/254): Updates related to the verification scripts. 
* Pull Request [#199](https://github.com/WCSim/WCSim/pull/199): Can now check changes to output file sizes and running time in verification scripts. 
* Pull Request [#193](https://github.com/WCSim/WCSim/pull/193): Users can now specify the precision with which to store digitized hits. 
* Pull Request [#204](https://github.com/WCSim/WCSim/pull/204): New RootOptions included in the CMake build options. 
* Pull Request [#207](https://github.com/WCSim/WCSim/pull/207): Change the colour of mu- and mu+ in visualization and add background colour
* Pull Request [#258](https://github.com/WCSim/WCSim/pull/258), [#256](https://github.com/WCSim/WCSim/pull/256): Allow the addition and deletion of Cherenkov digits in WCSimWCTrigger
* Pull Request [#257](https://github.com/WCSim/WCSim/pull/257): Add assignment operators to WCSimRootEvent & WCSimRootTrigger
* Pull Request [#264](https://github.com/WCSim/WCSim/pull/264): Add how to run WCSim to the README
* Pull Request [#259](https://github.com/WCSim/WCSim/pull/259): WCSimWCDigit::SortArrayByHitTime() to sort times_presmear too
* Pull Request [#260](https://github.com/WCSim/WCSim/pull/260): Allow the addition and deletion of tracks in WCSimWCTrigger
* Pull Request [#262](https://github.com/WCSim/WCSim/pull/262), [#267](https://github.com/WCSim/WCSim/pull/267): Changes to the trigger to make it more realistic

*************************************************************
04/27/2017: Notes for v1.7.0        
*************************************************************
New Features
* Pull Request #198: Run options (such as dark noise rate, electronics configuration, detector configuration, etc) is now saved in the output ROOT file. 
* Pull Request #212 (forked from Pull Request #202): WCSim now runs with Geant 4.10.1. Extensive validation was done to ensure the physics output matches what we expect. The custom physics list option was removed and the FTFP_BERT physics list is now the default. 

Bug Fixes
* Pull Request #191: Digits were being saved multiple times if they were in overlapping trigger windows. This is now fixed. 

Updates

* Pull Request #135: G4 General Particle Source now documented in WCSim.mac.
* Pull Request #176: Clean up read_PMT.C by removing duplicate line and unifying output. 
* Pull Request #180: .gitignore file was extended to ignore output and compiled files.

*************************************************************
07/04/2016: Notes for v1.6.0        
*************************************************************
New Features
* Pull Request #125: Update code to compile with cmake. Information is included in the README.md file. 
* Pull Request #151: HyperK detector configuration now reflects the design report specifications (a cylinder with a height of 54.8 m, a diameter of 70.8 m). 
* Pull Request #151: The egg-shaped Hyper-K design is still available, but is now renamed EggShapedHyperK.
* Pull Request #163: Error message is now output if vector file specified in the macro does not exist. 
* Pull Request #170: Some improvements to verification_HitsChargeTime.C, including a check for the same number of events in the two files being compared. 

Bug Fixes
* Pull Request #165: The raw (undigitized) hit times in the output ROOT file were smeared since v1.5.0. This release returns the raw (undigitized) hit times back to their unsmeared times. 
* Pull Request #165: There was a related bug that arose from matching the collection of unsmeared hit times to the smeared times, resulting in some wrong times & parent IDs being saved. This release fixes that bug. 

Updates
* Pull Requests #164 and #183: Swap the remaining instances of cout, cerr, endl for G4cout, G4cerr, G4endl and remove the iostream includes, except in WCSimEnumerations, which is used to analyze the WCSim ROOT files.
* Pull Request #174: Updated references to documentation of root file structure. The defunct link included in some of the sample ROOT macros was changed to refer users to the WCSim DetectorDocumentation.pdf. 

*************************************************************
04/11/2016: Notes for v1.5.1        
*************************************************************
Bug Fixes:

* In v1.5.0, the dark rate was not being set correctly in the case where the default detector was not used (unless the dark rate was explicitly specified in the macro). This release now correctly implements the dark rate based on the photodetector type used in the detector configuration, even when the non-default detector configuration is called. 

*************************************************************
02/01/2016: Notes for v1.5.0        
*************************************************************
New Features
* Dark rate can now be added either in a defined time window around each hit, or in a set window of a constant time. 
* Digits with negative times are now saved (this doesn't affect the current default digitizer, but could have implications for future digitizers that get added in).
* MakeKin.py can now make vector files with random vertices for the SuperK and HyperK cylinders. 
* New DAQ-related features can now be controlled via macro files. The digitizer and trigger you want to use can be specified (in WCSim.mac), as well as properties of the digitzer and trigger (in daq.mac). 

Bug Fixes
* Hit PMTs were not being visualized after a change made in the last release. This was fixed. 
* If you built the same detector configuration twice (for example, if you rebuilt the default configuration) then the program would seg fault. This has now been fixed. 
* The smearing of the hit time is now applied in WCSimWCPMT, rather than in the digitizer after the trigger decisions have already been made. 
* The Enumerations files (WCSimEnumerations.cc and WCSimEnumerations.hh) are now compiled with WCSim to allow for ROOT scripts run in non-compiled mode to access that information. 

Structural Changes
* The digitizer, trigger, and dark noise are now separate pieces of code. Furthermore, the digitizer and trigger now inherit from base classes similar to how the PMTs were implemented in v1.2.0. As a result, users can add their own electronics easily.
* novis.mac has been renamed WCSim.mac and now contains an option to call the visualization commands (visOGLSX.mac for the OGLSX visualization that vis.mac used or visRayTracer.mac for the RayTracer visualization). vis.mac has been removed and visRaytracer.mac is no longer a stand alone macro. 
* All of the digitizer and trigger options are now included in a separate daq.mac file. This file gets called by WCSim.mac. The choice of digitizer and trigger is made in WCSim.mac. 
* Dark rate is now a property of the PMT and is automatically changed when a new PMT is used (rather than having to manually set it in the macro). 

Updates
* doc/DetectorDocumentation.pdf was updated to reflect new DAQ options and the changes made to the default macros. 

*************************************************************
08/03/2015: Notes for v1.4.0        
*************************************************************
New Features
* Multiple sensitive detectors can now be instantiated 
* Different sensitive detectors can have unique hit collections
* WCSim executable will exit if jobOptions.mac, jobOptions2.mac, or tuning_parameters.mac are not found
* Two cylindrical detectors with a height of 60 m and a diameter of 74 m are now available: one with 14% photocoverage and one with 40% photocoverage. 
* Raytracer visualiser is now available. A new macro, visRayTracer.mac, is now available to visualize configurations with Raytracer. 

Deleted Features
* Old DUSEL configurations removed

Structural Changes
* User now instantiates a PMT using CreatePMT("PMTType", "CollectionName")
* Glass thickness in ConstructPMT
* Collection efficiency can be set in the macro files
* HPD parameters to reflect the results from new measurements

Bug Fixes
* The function used for the timing resolution was different from the fitted function for the >20 p.e. region. The correct timing resolution function has now been implemented.
* PMTCollEff is now on by default
* A correction factor of 1/0.73 has been implemented for the Box and Line photosensors. Based on Hamamatsu simulations, the collection efficiency for the R3600 is 73%. We have factors in WCSim that suppress the pe output and so this makes the R3600 simulations correct, but since we now include other photosensors (ex. box and line), we need to correct by the new photosensors by the 1/0.73 factor. The new ratio we expect for the digitized output (Quantum efficiency x Collection efficiency x Hit Efficiency) of B&L to R3600 is: (0.315x0.95x0.856/0.73)/(0.211x1.0x0.717)=2.32. This is a temporary solution until we can properly incorporate the R3600 collection efficiency and retune the factors in WCSim. 

*************************************************************
04/08/2015: Notes for v1.3.0        
*************************************************************
New Features
* Added in 12" HPD photosensor option (same as 20" HPD but with a different geometry)
* New detector configurations added for the Hyper-K design study:  SuperK_20inchPMT_20perCent, SuperK_20inchBandL_20perCent, SuperK_12inchBandL_15perCent, SuperK_20inchBandL_14perCent, Cylinder_12inchHPD_15perCent
* Added 12" and 20" Box and Line photosensor option
* Implemented a collection efficiency parameter to deal with the angular efficiency dependence for Box and Line photosensors. By default, the collection efficiency option is turned off (so the collection efficiency is 100%). To enable the collection efficiency parameter, the user must set `/WCSim/PMTCollEff on` in the macro. If the collection efficiency parameter is enabled, the default collection efficiency in the base class is 100%. To use a different collection efficiency, GetCollectionEfficiencyArray() must be specified when defining a PMTObject in WCSimPMTObject.cc. 
* Added link to the NUANCE format documentation in the comments of WCSimPrimaryActionGenerator.cc

*************************************************************
01/21/2015: Notes for v1.2.0        
*************************************************************
Bug Fixes
* Missing HK variables added so that the HK geometry could be specified as the default and the code won't crash. 
* HasSubEvents() function now returns the proper Boolean value. 
* Dark noise is now added between the start of the trigger window and the start of the signal. 

Structural Changes
* All of the different detector setups were moved into a file called WCSimDetectorConfigs.cc.
* WCSimPMTQE.cc was created and all quantum efficiency info was put there.
* WCSimWCDigitizer was split into a PMT collection (in the newly created WCSimWCPMT file) and a digitizer collection.
* The PMT information was put into a single file called WCSimPMTObject.cc. PMTs now inherit from the newly created WCSimPMTObject class. 
* WCSimWCConstructWC is now named WCSimConstructCylinder to explicitly show that this function builds cylindrical geometries and should not be used to construct the HK geometry. 

New Features
* Python script was added to generate kinematics files for simple particles. 
* A (very) basic validation script was added in a new directory called verification-test-scripts. The verification_HitsChargeTime.C script checks the hits, charge, and timing of events against a clean copy of the WCSim code. 
* HPDs were added to the list of possible tubes allowed in WCSim.
* Hyper-K with HPDs was added as a new detector configuration. 
* Updated the dark rate parameters for normal PMTs.
* Added the dark rate commands to novis.mac. Uncommented out the dark rate commands in novis.mac and vis.mac.
* Included the dark rate parameters for the HPDs in vis.mac and novis.mac.
