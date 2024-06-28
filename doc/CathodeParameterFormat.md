# Cathode Parameter format
This describes the format for a file containing the cathode parameters, set by `/WCSim/tuning/PMTCathodePara`

An example is shown below:
```
#DATASTART
2 11.5 6
1.000 3.4 1.7
2.786 3.4 1.7
3.061 3.1 1.6
3.306 2.8 1.5
3.679 2.4 1.4
9.000 2.4 1.4
```

Each line represents the following:
* `#DATASTART`
  * Marks the start of the data.
* `2 11.5 6`
  * The first number is the choice of the photocathode model : 0 = default model, 1 or 2 = new models. \
  If set to 0, all other parameters are irrelevant. \
  Model 1 is copied from [geant4.11](https://ieeexplore.ieee.org/document/9875513), which models the alloy as a thin layer with real refractive index, then calculate reflection and transmission probability, but it cannot handle total internal reflection when n1 < n2. \
  Model 2 is implemented based on [this paper](https://arxiv.org/abs/physics/0408075v1), which models the alloy as a thin layer with real and imaginary refractive indices, then calculate absorption, reflection and transmission probability.
  * The second number sets the photocathode layer thickness in nm.
  * The third sets the number of points of photon energy (rows below) to read refractive indices (limited to <=100)
* `1.000 3.4 1.7` and all lines below
  * The first number is the photon energy in eV.
  * The second and third numbers are the real and imaginary refractive indices of the alloy at that point.