###############################################################
## L.Anthony 01/12/2021                                      ##
## Readme for WCTE version of WCSim with multiple geometries ##
###############################################################

You can control the version of geometry in nuPRISMBeamTest.mac. 

nuPRISMBeamTest_16cShort_mPMT is the most up to date version of the detector
16cShort = 16 columns 4 rows with dimensions from CAD

nuPRISMBeamTest_mPMT is the original WCTE geometry which fiTQun was first tuned for

The code which defines the geometries is src/WCSimDetectorConfigs.C
Here the size and number of columns and rows are set.

WCSimConstructCylinder.cc also has ability to import CAD models using CADMesh
