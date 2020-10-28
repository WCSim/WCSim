January 23, 2004  K. Scholberg
Modified for WCSim 8/28/09 C. Walter

I deleted info related to 2KM. Here is info on what root uses in the system. 
- CWW

----
The WCSimRootGeom class contains selected geometry information,
right now just WC info: cylinder dimensions and tube positions
and orientations.  A tree with one geometry "event" is included
in every output file.  This is so that the geometry info (which will
be quite volatile during the testing period) will be associated in the same
file as the data generated.

[Note from CWW: A text file is also generated at running with geometry
for the SK event displays etc. Note, the after the geometry is set up
it is traversed and every tube records where it is in a map.  So all
tubes can query their location and orientation etc.  This is done in a
general way and should work automatically for all setups.  This
structure is what is used to write the files or geometry trees.]

Root related files:
./src/WCSimRootEvent.cc, ./include/WCSimRootEvent.hh
  These are the WCSimRootEvent class files 

Root related files:
./src/WCSimRootGeom.cc, ./include/WCSimRootGeom.hh
  These are the WCSimRootGeom class files 

./src/WCSimRootDict.cc, ./src/WCSimRootDict.hh
  These are the Root dictionary files created by rootcint
(they are not in the repository); they are made by 
gmake rootcint

./include/WCSimRootLinkDef.hh
  Needed for rootcint

The sample-root-scrips subdirectory contains some root scripts.

root/sample_readfile.C
  An example Root CINT script to read the wcsim.root output
and print out various quantities in the tree.

root/read_number_of_PMTs.C
  An example script to print out the number of tubes in a geometry.

root/testgeo.C
  An example Root CINT script to read the wcsim.root output
and print out various quantities in the geometry tree.

To make a shared library to be loaded into Root,
in order to manipulate the WCSim Root classes interactively in Root CINT, do:

gmake shared

This will make libWCSimRoot.so with the class dictionary.
