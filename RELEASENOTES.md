This file contains the release notes for each version of WCSim. Release notes can also be found at https://github.com/WCSim/WCSim/tags. 

*************************************************************
09/10/2020: Notes for v1.9.0        
*************************************************************

This release includes updates to accommodate the external trigger software (https://github.com/HKDAQ/TriggerApplication) and changes that allow for supernova neutrinos to be more easily simulated. The ability for tools to work in ROOT6 has also been included. 

New Features
* Pull Request [#285](https://github.com/WCSim/WCSim/pull/285): Include compatibilities for ROOT6

Bug Fixes
* Pull Request [#286](https://github.com/WCSim/WCSim/pull/286), [#287](https://github.com/WCSim/WCSim/pull/287): Fix a memory leak when running on large (>1000) event files. 

Updates
* Pull Request [#289](https://github.com/WCSim/WCSim/pull/289), [#277](https://github.com/WCSim/WCSim/pull/277): Allow for multiple vertices to have different information. 
* Pull Request [#288](https://github.com/WCSim/WCSim/pull/288): Allow kinsplitter to create multiple kin files for the same event.  



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

* In v1.5.0, the dark rate was not being set correctly in the case where the default detector was not used (unless the dark rate was explicitly specified in the macro). This release now correctly implements the dark rate based on the photodector type used in the detector configuration, even when the non-default detector configuration is called. 

*************************************************************
02/01/2016: Notes for v1.5.0        
*************************************************************
New Features
* Dark rate can now be added either in a defined time window around each hit, or in a set window of a constant time. 
* Digits with negative times are now saved (this doesn't affect the current default digitizer, but could have implications for future digitizers that get added in).
* MakeKin.py can now make vector files with random vertices for the SuperK and HyperK cylinders. 
* New DAQ-related features can now be controled via macro files. The digitizer and trigger you want to use can be specified (in WCSim.mac), as well as properties of the digitzer and trigger (in daq.mac). 

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
* WCSim exectuable will exit if jobOptions.mac, jobOptions2.mac, or tuning_parameters.mac are not found
* Two cylindrical detectors with a height of 60 m and a diameter of 74 m are now available: one with 14% photocoverage and one with 40% photocoverage. 
* Raytracer visulaizer is now available. A new macro, visRayTracer.mac, is now available to visualize configurations with Raytracer. 

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
* WCSimWCConstructWC is now named WCSimConstructCylinder to explicitly show that this function builds cylinderical geometries and should not be used to construct the HK geometry. 

New Features
* Python script was added to generate kinematics files for simple particles. 
* A (very) basic validation script was added in a new directory called verification-test-scripts. The verification_HitsChargeTime.C script checks the hits, charge, and timing of events against a clean copy of the WCSim code. 
* HPDs were added to the list of possible tubes allowed in WCSim.
* Hyper-K with HPDs was added as a new detector configuration. 
* Updated the dark rate parameters for normal PMTs.
* Added the dark rate comands to novis.mac. Uncommented out the dark rate commands in novis.mac and vis.mac.
* Included the dark rate parameters for the HPDs in vis.mac and novis.mac.
