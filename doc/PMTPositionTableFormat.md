# PMT position table format
This describes the format for a file containing the PMT positions, set by `/WCSim/PMT/PositionFile` or `/WCSim/PMT/ODPositionFile`.

An example is shown below:
```
#DATASTART
1 1 0 3502.10000000 452.93300000 -2696.00000000 -0.965926 -0.258819 0.00000000 0 1
1 2 0 3259.38000000 1358.80000000 -2696.00000000 -0.965926 -0.258819 0.00000000 0 1
1 3 0 2806.44000000 2143.30000000 -2696.00000000 -0.707107 -0.707107 0.00000000 0 1
1 4 0 2143.30000000 2806.44000000 -2696.00000000 -0.707107 -0.707107 0.00000000 0 1
1 5 0 1358.80000000 3259.38000000 -2696.00000000 -0.258819 -0.965926 0.00000000 0 1
1 6 0 452.93300000 3502.10000000 -2696.00000000 -0.258819 -0.965926 0.00000000 0 1
1 7 0 -452.93300000 3502.10000000 -2696.00000000 0.258819 -0.965926 0.00000000 0 1
1 8 0 -1358.80000000 3259.38000000 -2696.00000000 0.258819 -0.965926 0.00000000 0 1
```

Each line represents the following:
* `#DATASTART`
  * Marks the start of the data.
* Subsequent lines contain PMT entry in the format:\
`Type mPMTId Section X Y Z DirX DirY DirZ Rotation Use`
  * `Type`: 1 = PMT1 (nominal),\
            2 = PMT2 in hybrid configuration,\
            3 = Ex-situ mPMT in WCTE,\
            4 = In-situ mPMT in WCTE, \
            5 = Beam pipe in WCTE,\
            6 = Camera housing in WCTE\
    For OD PMT, this number is dummy
  * `mPMTId`: custom ID to override the default numbering scheme
  * `Section`: 0 = barrel, \
            1 = top cap, \
            2 = top border ring, \
            3 = bottom cap, \
            4 = bottom border ring, \
            10 = OD barrel, \
            11 = OD top cap, \
            13 = OD bottom cap
  * `X Y Z`: PMT positions in mm. Exact values to be tuned to detector geometry at runtime
  * `DirX DirY DirZ`: PMT direction, just for reference
  * `Rotation`: azimuthal rotation of the PMT module
  * `Use`: enable placement (1) or not (0)