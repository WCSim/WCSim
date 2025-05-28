#ifndef INCLUDE_GEOMETRYSCAN_HH_
#define INCLUDE_GEOMETRYSCAN_HH_

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4SIunits.hh"

/// @brief GeometryScanning Utility Namespace
namespace GeometryScan {

// Namespace Globals for scanning definition
G4double xlow;
G4double xhigh;
G4double xstep;

G4double ylow;
G4double yhigh;
G4double ystep;

G4double zlow;
G4double zhigh;
G4double zstep;

G4double xcenter;
G4double ycenter;
G4double zcenter;

G4double xwidth;
G4double ywidth;
G4double zwidth;

/// @brief Sets GeometryScan constants for center step point
/// @param v ThreeVector of center
void SetScanGeometryBoundaryCenter(const G4ThreeVector& v) {
  xcenter = v.x();
  ycenter = v.y();
  zcenter = v.z();

  xlow = xcenter - xwidth/2;
  ylow = ycenter - ywidth/2;
  zlow = zcenter - zwidth/2;

  xhigh = xcenter + xwidth/2;
  yhigh = ycenter + ywidth/2;
  zhigh = zcenter + zwidth/2;
}

/// @brief Sets GeometryScan constants for scanning grid width
/// @param v ThreeVector of grid widths
void SetScanGeometryBoundaryWidth(const G4ThreeVector& v) {
  xwidth = v.x();
  ywidth = v.y();
  zwidth = v.z();

  xlow = xcenter - xwidth/2;
  ylow = ycenter - ywidth/2;
  zlow = zcenter - zwidth/2;

  xhigh = xcenter + xwidth/2;
  yhigh = ycenter + ywidth/2;
  zhigh = zcenter + zwidth/2;
}

/// @brief Sets GeometryScan constants for step size
/// @param v ThreeVector of step values
void SetScanGeometryStepSize(const G4ThreeVector& v) {
  xstep = v.x();
  ystep = v.y();
  zstep = v.z();
}

/// @brief Geometry Scanning Utility. Uses namespace definitions
///        of limits and step sizes. Outputs to file path.
/// @param filename location of the output file.
void ScanGeometry(std::string filename) {

  // Open new output file
  std::ofstream outFile(filename);
  if (!outFile) {
        std::cerr << "Error opening file!" << std::endl;
        return;
  }


  G4cout << "Scanning geometry low to file. Starting at: "
    << xlow << " " << ylow << " " << zlow << std::endl;

  auto navigator = (G4TransportationManager::GetTransportationManager()
    ->GetNavigatorForTracking());

  G4ThreeVector scan_pos = G4ThreeVector(xlow, ylow, zlow);
  scan_pos[0] = xlow;

  G4ThreeVector direction = G4ThreeVector(0,0,1);

  while (scan_pos.x() < xhigh) {
    std::cout << scan_pos.x() << ", "
          << scan_pos.y() << ", "
          << scan_pos.z()  << std::endl;

    scan_pos += G4ThreeVector(xstep, 0, 0);

    scan_pos[1] = ylow;
    while (scan_pos.y() < yhigh) {
      scan_pos += G4ThreeVector(0, ystep, 0);

      scan_pos[2] = zlow;
      while (scan_pos.z() < zhigh) {
        scan_pos += G4ThreeVector(0, 0, zstep);


      G4VPhysicalVolume* scan_volume = (navigator
      ->LocateGlobalPointAndSetup(scan_pos, &direction, 1, 0));
      if (!scan_volume) continue;

      G4LogicalVolume* logical = scan_volume->GetLogicalVolume();
      if (!logical) continue;

      G4Material* material = scan_volume->GetLogicalVolume()->GetMaterial();
      if (!material) continue;

      G4double density = material->GetDensity() / (g/cm3);

      outFile << scan_pos.x() << ", "
        << scan_pos.y() << ", "
        << scan_pos.z() << ", "
        << density << ", "
        << logical << std::endl;

      }
    }
  }
  outFile.close();
}
}  // namespace GeometryScan
#endif  // INCLUDE_GEOMETRYSCAN_HH_
