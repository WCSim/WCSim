#include "WCSimDetectorConstruction.hh"

#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4Cons.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalSkinSurface.hh"

#include "G4SystemOfUnits.hh"


// WCTE camera housing logical volume construction.
// A function of the WCSimDetectorConstruction class
// dimensions are from the design drawing and hard-coded
G4LogicalVolume* WCSimDetectorConstruction::ConstructCameraHousing()
{
  /////////////////////////////////////////
  // From design drawings
  G4double domeR = 83*mm;
  G4double domeThickness = 6*mm; // rougly estimation from drawing
  G4double cylinderR = 254*mm/2.;
  G4double cylinderH = 162*mm;
  G4double dome_cylinder_intesection = 2*mm;

  //////////////////////////////////////////////////////////////
  /// 1) Outer logical volume: fill with air                 ///
  //////////////////////////////////////////////////////////////
  G4Sphere *solidDome = new G4Sphere("chDome",
                                      0,
                                      domeR,
                                      0.0*deg, 360.0*deg,
                                      0.0, 90*deg);
  
  G4Cons *solidCylinder = new G4Cons("chCylinder",
                                     0,
                                     cylinderR,
                                     0,
                                     cylinderR,
                                     cylinderH/2,
                                     0.*deg, 360.*deg);

  G4UnionSolid *solidCH = new G4UnionSolid("CameraHousing",
                                            solidDome,
                                            solidCylinder,
                                            0,
                                            G4ThreeVector(0.,0.,-cylinderH/2+dome_cylinder_intesection));

  G4LogicalVolume *logicCH =
    new G4LogicalVolume(    solidCH,
			    G4Material::GetMaterial("Air1"), 
			    "CameraHousing");

  //////////////////////////////////////////////////////////////
  /// 2) Upper dome: fill with acrylic                       ///
  //////////////////////////////////////////////////////////////
  G4Sphere *solidDomeS = new G4Sphere("chDomeShell",
                                      domeR-domeThickness,
                                      domeR,
                                      0.0*deg, 360.0*deg,
                                      0.0, 90*deg);
  G4SubtractionSolid *solidDomeCut = new G4SubtractionSolid("CameraHousingDome",                                       
                                                            solidDomeS,                                   
                                                            solidCylinder,
                                                            0,
                                                            G4ThreeVector(0.,0.,-cylinderH/2+dome_cylinder_intesection));
  G4LogicalVolume *logicDome =
    new G4LogicalVolume(    solidDomeCut,
			    G4Material::GetMaterial("G4_PLEXIGLASS"), 
			    "CameraHousingDome");

  new G4PVPlacement(0,
                    G4ThreeVector(0.,0,0),
                    logicDome,
                    "CameraHousingDome",
                    logicCH,
                    false,
                    0,
                    checkOverlaps);

  G4VisAttributes *domeAttributes = new G4VisAttributes();
  domeAttributes->SetColor(1.0, 1.0, 1.0, 0.5);
  domeAttributes->SetVisibility(true);
  domeAttributes->SetForceSolid(true);
  logicDome->SetVisAttributes(domeAttributes);

  //////////////////////////////////////////////////////////////
  /// 3) Lower cylinder: fill with stainless steel           ///
  //////////////////////////////////////////////////////////////
  G4LogicalVolume *logicCyldiner =
    new G4LogicalVolume(    solidCylinder,
			    G4Material::GetMaterial("StainlessSteel"), 
			    "CameraHousingCylinder");

  new G4PVPlacement(0,
                    G4ThreeVector(0.,0,-cylinderH/2+dome_cylinder_intesection),
                    logicCyldiner,
                    "CameraHousingCylinder",
                    logicCH,
                    false,
                    0,
                    checkOverlaps);

  new G4LogicalSkinSurface("chCylinderSkinSurface", logicCyldiner, BSSkinSurface); // assume same as blacksheet

  G4VisAttributes *cylinderAttributes = new G4VisAttributes();  
  cylinderAttributes->SetColor(0.2, 0.2, 0.2, 1.0);  
  cylinderAttributes->SetVisibility(true); 
  cylinderAttributes->SetForceSolid(true);  
  logicCyldiner->SetVisAttributes(cylinderAttributes);

  return logicCH;
}