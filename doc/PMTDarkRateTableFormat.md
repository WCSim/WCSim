# PMT dark rate table format
This describes the format for a file containing the PMT dark rate, set by `/WCSim/PMT/DarkRateFile`

An example is shown below:
```
#DATASTART
1 10000
2 10000
```

Each line represents the following:
* `#DATASTART`
  * Marks the start of the data.
* Subsequent lines contain PMT entry in the format:\
`PMTId darkrate`
  * `PMTId`: ID of PMT
  * `darkrate`: value in Hz
