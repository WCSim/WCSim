This file contains the release notes for each version of WCSim. Release notes can also be found at https://github.com/WCSim/WCSim/tags. 

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
