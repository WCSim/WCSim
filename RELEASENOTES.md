This file contains the release notes for each version of WCSim. Release notes can also be found at https://github.com/WCSim/WCSim/tags. 
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
