# NUANCE MC FORMAT
---

This describes the format for a file containing "nuance format" kinematic
information for passing to WCSim.

An example event is shown below:
```
$ begin
$ nuance 3
$ vertex 811.5 -1030.4 -1681.7 5.86347E+04
$ track 14 1000.0000 1.00000 0.00000 0.00000 -1
$ track 2212 935.9840 -0.03219 -0.24434 0.96915 -1
$ info 2 949000 0.0000E+00
$ track 13 295.6312 0.15021 0.98821 -0.02950 -2
$ track 211 427.5731 0.66875 -0.66698 -0.32849 -2
$ track 2212 1212.7798 0.88658 -0.07320 0.45675 -2
$ track 13 295.6312 0.15021 0.98821 -0.02950 0
$ track 2212 1212.7798 0.88658 -0.07320 0.45675 0
$ track 111 382.1095 -0.73647 -0.35958 -0.57299 0
$ end
```

Each line begins with a text tag, indicating the information it contains.

* `begin`
  * Marks the start of the event.
* `nuance`
  * This line indicates the reaction mode for the event.
* `vertex`
  * The first three numbers are the x, y and z coordinate, in cm.
  * The fourth number is the time, in simulation units normally ns.  To specify
    different units use the command /mygen/time_units in the .mac file.
  * Multiple vertices can used in a single event by repeating the vertex, track, and info
    lines; the nuance line should not be repeated. (see example below)
* `track`
	The track lines contain most of the information. 
	* The first number on the line is the particle code, using Particle
    Data Group/Lund conventions. An exception is that for coherent reactions, an
    ad hoc particle code is assigned for the target nucleus, since the PDG
    system does not encompass nuclei. For nuclei, the code is Z*1000+A, so
    Oxygen is particle code 8016. 
	* The next number is the total energy of the
    particle. By default, this is in MeV. Note that for bound nucleon targets,
    the total energy will be less than the rest mass since the target is
    bound.
	* The third, fourth and fifth numbers are the x, y and z
    direction cosines (direction unit vector).
	* The last number on the line is either -1 (initial state particle), -2 (final state particle
    before interactions) or 0 (the final state particle after interactions).
    Normally your detector simulation should only track particles with a status
    code of 0.	
* `info`
    * The first two numbers are obsolete and/or irrelevant and are retained
    only for compatibility with Super-K software. 
	* The third number is the neutrino flux for this energy and direction. The units are the same as
    whatever the input flux is. This can be useful if you later want to
    re-weight events (after they have passed through detector simulation and
    reconstruction).
* `end`
    * Marks the end of the event.
* `stop`
    * (Not shown above) marks the end of the file.

## WCSim extensions

* Comments `#`
    * In WCSim a `#` character at the start of a line indicates a comment and
    will be ignored. This can be useful for documenting the contents of a file.
* Multiple vertices per event
    * It is possible to have multiple vertices in a single event, thus the format becomes:
```
$ begin
$ nuance 0
$ vertex 1002.14846 -2398.48362 2499.42670 0.00000
$ track 14 1600.00000 1.00000 0.00000 0.00000 -1
$ track 2212 935.98400 0.00000 0.00000 1.00000 -1
$ info 0 0 100
$ track 13 600.00000 0.27937 -0.52291 -0.80531 0
$ nuance 0
$ vertex 3245.31346 -400.50803 -2761.79254 0.00000
$ track 14 1600.00000 1.00000 0.00000 0.00000 -1
$ track 2212 935.98400 0.00000 0.00000 1.00000 -1
$ info 0 0 101
$ track 13 600.00000 -0.57270 0.61429 0.54283 0
$ nuance 0
$ vertex -1940.34611 2221.21080 -2951.44834 0.00000
$ track 14 1600.00000 1.00000 0.00000 0.00000 -1
$ track 2212 935.98400 0.00000 0.00000 1.00000 -1
$ info 0 0 102
$ track 13 600.00000 -0.51716 -0.83284 0.19726 0
$ end
```
   
This document is based on the page: http://neutrino.phy.duke.edu/nuance-format/
      
