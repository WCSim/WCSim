# Geant4 11 compatibility

## Completed

- Replace deprecated scintillation property names for BGO.
- Remove obsolete FAST/SLOW scintillation properties.
- Remove obsolete YIELDRATIO.
- Reorder WCTE optical property tables into increasing photon energy.

## Remaining

- Run full Hyper-K macro.
- Run WCTE macro.
- Verify BGO/AmBe calibration.

Geant4 11 requires photon energy arrays passed to G4MaterialPropertiesTable::AddProperty() to be in strictly increasing order. Reordered the WCTE glass and silicone gel optical property tables while preserving the energy–property correspondence.
