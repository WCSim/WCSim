# Light Injector format
This describes the format for the use of a light injector water scan as set in `/WCSim/data/LightInjector.json` and `/WCSim/data/LightInjectorDetails.json`

The LightInjector.json file contains the water profiles of a single light injector of each given type. ID collimator, OD collimator, ID diffuser, OD diffuser, laser ball. In future this should be update so that the water profile for each diffuser is included and assigned correctly.

An example is shown below:
```
"comment": "ID collimator",
"type": "collimator",	
"detector":"ID",
"theta": [-2.0,0.0,2.0,-2.0,0.0,2.0],
"phi": [0,0,0,360,360,360],
"intensity": [1,1,1,1,1,1]
```
Each line represents the following:
* "comment" - Any description of the light injector specified
* "type" - Which type of light injector is specified. Currently this is limited to diffuser, collimator and laserball
* "detector" - Which detector volume the light injector belongs to
* "theta" - The in water scan 'theta' variable. This refers to the rotation of the injector by 180 degrees as it pivots from facing the photosensing unit, to being at 90 degrees to the photosensing unit. An angle of 90 degrees corresponds to the light injector being face on to the photosensing unit.
* "phi" - The in water scan 'phi' variable. This refers to the rotation of the injector by 360 degrees as it spins on its own axis.
* "intensity" - This corresponds to the intensity of light detected at each scan position in phi and theta as measured in the in water scan.

LightInjectorDetails.json specifies the light injector parameters related to their placement in the detector. 

An example is shown below:
```
"comment": "Barrel collimator. Units cm, nm, deg.",
"type": "collimator",
"idx": "0",
"detector":"ID",
"direction": [-1,0,0],
"position": [3220,0,-2465.66],
"wavelength": 400,
"offset": 10,
"photonsfile": ""
```
* "comment" is an optional description of the light injector unit being simulated.
* "type" refers to the type of light injector simulated. The choices currently are diffuser, collimator and laserball. This variable should link to corresponding option in the LightInjector.json file. 
* "detector" determines which detector volume the specified light injector corresponds to. This variable should link to corresponding option in the LightInjector.json file.
* "direction" refers to the direction of the light injector in the simulated detector. It should be an array of 3 elements corresponding to the x, y and z direction respecitvely. Currently they are all simulated to be equally distributed across the tank with 4 light injector positions on the top of the tank, 4 on the bottom and 28 on the barrel. In future this should be updated to the realistic geometry of the tank.
* "position" is the position of the light injector in the tank. It operates in a similar way to the direction variable. It is specified in units of cm
* "wavelength" is the wavelength of the simulated light through the light injector. It is in units of nm.
* "offset" corresponds to the offset of the simualted light injector relative to the tank frame, in cm.
* "photonsfile" provides on option of simulating only photons as specified from a separate file rather than using the water profiles. 