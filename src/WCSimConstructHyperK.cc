//  -*- mode:c++; tab-width:4;  -*-
#include "WCSimDetectorConstruction.hh"

#include "G4SystemOfUnits.hh"

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"

#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"

#include "G4LogicalSkinSurface.hh"

#include "G4VisAttributes.hh"

#include "G4SDManager.hh"
#include "WCSimWCSD.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCSimDetectorConstruction::SetHyperKGeometry()
{
  waterTank_TopR   = 32000.*mm;
  waterTank_BotR   = 30000.*mm;
  waterTank_Height = 48000.*mm;
  waterTank_UpperA =  8000.*mm;
  waterTank_LowerB =  6000.*mm;
  waterTank_Length = 49500.*mm;

  innerPMT_TopR     = 29095.*mm; 
  innerPMT_BotR     = 27095.*mm;
  innerPMT_TopW     = 12038.*mm;
  innerPMT_BotW     = 11004.*mm;
  innerPMT_Height   = 21095.*mm;
  innerPMT_Radius   = 20. * 25.4*mm / 2.;
  innerPMT_Expose   =   180.*mm;
  innerPMT_Rpitch   =   990.*mm;
  innerPMT_Apitch   =   990.*mm;

  outerPMT_TopR      = innerPMT_TopR + 900.*mm;
  outerPMT_BotR      = innerPMT_BotR + 900.*mm;
  outerPMT_TopW      = 12394.*mm;
  outerPMT_BotW      = 11319.*mm;
  outerPMT_Height    = innerPMT_Height + 900.*mm;
  outerPMT_Radius    =  8. * 25.4*mm / 2.;
  outerPMT_Expose    =  outerPMT_Radius - 10.*mm;
  outerPMT_TopRpitch = 3. * innerPMT_Rpitch * (outerPMT_TopR/innerPMT_TopR);
  outerPMT_BotRpitch = 3. * innerPMT_Rpitch * (outerPMT_BotR/innerPMT_BotR);
  outerPMT_Apitch    = 2. * innerPMT_Apitch;

  blackSheetThickness = 20.*mm;

  innerPMT_TopN = 0;
  innerPMT_BotN = 0;

  MatchWCSimAndHyperK();
}

void WCSimDetectorConstruction::MatchWCSimAndHyperK()
{
  WCLength = waterTank_Length;
  WCPosition = 0.;
  WCPMTRadius = innerPMT_Radius;

  WCPMTName             ="20inch";
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4LogicalVolume* WCSimDetectorConstruction::ConstructHyperK()
{

  // ------------- Volumes --------------

  checkOverlaps = true;

  // HyperK Volume

  G4LogicalVolume* hyperKLV
    = new G4LogicalVolume(new G4Box("HyperKBox",
                                    waterTank_Height/2.,
                                    waterTank_Height/2.,
                                    waterTank_Length/2.+blackSheetThickness),
                                    FindMaterial("G4_AIR"),
                                    "HyperK");
  hyperKLV->SetVisAttributes(G4VisAttributes::Invisible);

  new G4LogicalSkinSurface("WaterBSSurface",hyperKLV,OpWaterBSSurface);

//  G4VPhysicalVolume* hyperKPV
//    = new G4PVPlacement(0,G4ThreeVector(),hyperKLV,
//                        "HyperK",0,false,0,checkOverlaps);

  // +/- Z Black Sheets

  G4LogicalVolume* blackSheetZLV
    = new G4LogicalVolume(new G4Box("blackSheetZ",
                                    waterTank_Height/2.,
                                    waterTank_Height/2.,
                                    blackSheetThickness/2.),
                                    FindMaterial("G4_POLYETHYLENE"),
                                    "blackSheetZ");
//  blackSheetZLV->SetVisAttributes(new G4VisAttributes(G4Colour(0,0.5,1.)));

  new G4LogicalSkinSurface("WaterBSSurface",blackSheetZLV,OpWaterBSSurface);

  G4double z = waterTank_Length/2.+blackSheetThickness/2.;

  new G4PVPlacement(0,G4ThreeVector(0,0, z),blackSheetZLV,
                        "blackSheetZ",hyperKLV,false,0,checkOverlaps);
  new G4PVPlacement(0,G4ThreeVector(0,0,-z),blackSheetZLV,
                        "blackSheetZ",hyperKLV,false,1,checkOverlaps);

// -----------------------------------------------------------------
//  G4LogicalVolume* pmtLV = ConstructPMT(innerPMT_Radius,innerPMT_Expose);
//  new G4PVPlacement(0,G4ThreeVector(0.,0.,innerPMT_Expose/2.),
//                    pmtLV,"PMT",hyperKLV,false,0,checkOverlaps);
//  return hyperKLV;
// -----------------------------------------------------------------

  // The HyperK Water Tank

  G4VSolid* waterTank_top = ConstructHalf(waterTank_TopR,waterTank_UpperA);
  G4VSolid* waterTank_bot = ConstructHalf(waterTank_BotR,waterTank_LowerB);

  G4RotationMatrix rotm = G4RotationMatrix();
  rotm.rotateZ(180.*degree);

  G4VSolid* waterTank_union
    = new G4UnionSolid("Union",waterTank_top,waterTank_bot,
                       G4Transform3D(rotm,
                                     G4ThreeVector(0,-waterTank_Height/2.,0)));

  waterTankLV 
    = new G4LogicalVolume(waterTank_union,FindMaterial("G4_WATER"),"Tank");
  waterTankLV->SetVisAttributes(G4VisAttributes::Invisible);

  new G4PVPlacement(0,G4ThreeVector(0,waterTank_Height/4.,0),
                    waterTankLV,"Tank",hyperKLV,false,0,checkOverlaps);

  G4RotationMatrix* g4rot;
  G4LogicalVolume* pmtCellLV;

  // Radial PMTs

  pmtCellLV = ConstructRadialPMT(true,  innerPMT_TopR, innerPMT_Height,
                                 waterTank_UpperA, innerPMT_Expose,
                                 innerPMT_Rpitch, innerPMT_Apitch);

  g4rot = new G4RotationMatrix();
  g4rot->rotateY(90.*deg);

  G4double ratio = innerPMT_Radius/innerPMT_TopR;
  G4double r = innerPMT_TopR*std::sqrt(1.-ratio*ratio);
  r -= innerPMT_Expose/2.;

  new G4PVPlacement(g4rot,
                    G4ThreeVector(r,0.,0.),
                    ConstructPMT(innerPMT_Radius,innerPMT_Expose),
                    "PMT",
                    pmtCellLV,
                    false,0,checkOverlaps);

  pmtCellLV = ConstructRadialPMT(false, innerPMT_BotR, innerPMT_Height,
                                 waterTank_LowerB,innerPMT_Expose,
                                 innerPMT_Rpitch, innerPMT_Apitch);

  ratio = innerPMT_Radius/innerPMT_BotR;
  r = innerPMT_BotR*std::sqrt(1.-ratio*ratio);
  r -= innerPMT_Expose/2.;

  new G4PVPlacement(g4rot,
                    G4ThreeVector(r,0.,0.),
                    ConstructPMT(innerPMT_Radius,innerPMT_Expose),
                    "PMT",
                    pmtCellLV,
                    false,0,checkOverlaps);

  pmtCellLV = ConstructRadialPMT(true,  outerPMT_TopR, outerPMT_Height,
                                 waterTank_UpperA, outerPMT_Expose,
                                 outerPMT_TopRpitch, outerPMT_Apitch);

  g4rot = new G4RotationMatrix();
  g4rot->rotateY(-90.*deg);

  G4double phi  = outerPMT_TopRpitch/outerPMT_TopR/3;
  G4double dphi  = innerPMT_Radius/innerPMT_TopR;

  G4double angle = -phi + dphi;

  ratio = outerPMT_Radius/outerPMT_TopR;
  r = outerPMT_TopR*std::sqrt(1.-ratio*ratio);
  // don't understand this reall....
  r -= outerPMT_Expose;

  G4double x = r * std::cos(angle);
  G4double y = r * std::sin(angle);
//  G4double x = (outerPMT_TopR+outerPMT_Expose/2.) * std::cos(angle);
//  G4double y = (outerPMT_TopR+outerPMT_Expose/2.) * std::sin(angle);
           z = -outerPMT_Apitch/4.;

  new G4PVPlacement(g4rot, G4ThreeVector(x,y,z),
                    ConstructPMT(outerPMT_Radius,outerPMT_Expose),
                    "PMT",
                    pmtCellLV,
                    false,0,checkOverlaps);

  angle = phi - dphi;

  x = r * std::cos(angle);
  y = r * std::sin(angle);
//  x = (outerPMT_TopR+outerPMT_Expose/2.) * std::cos(angle);
//  y = (outerPMT_TopR+outerPMT_Expose/2.) * std::sin(angle);
  z = outerPMT_Apitch/4.;

  new G4PVPlacement(g4rot, G4ThreeVector(x,y,z),
                    ConstructPMT(outerPMT_Radius,outerPMT_Expose),
                    "PMT",
                    pmtCellLV,
                    false,0,checkOverlaps);

  pmtCellLV = ConstructRadialPMT(false, outerPMT_BotR, outerPMT_Height,
                                 waterTank_LowerB, outerPMT_Expose,
                                 outerPMT_BotRpitch, outerPMT_Apitch);

  phi  = outerPMT_BotRpitch/outerPMT_BotR/3;
  dphi = innerPMT_Radius/innerPMT_BotR;

  angle = -phi + dphi;

  ratio = outerPMT_Radius/outerPMT_BotR;
  r = outerPMT_BotR*std::sqrt(1.-ratio*ratio);
  // don't understand this reall....
  r -= outerPMT_Expose;

  x = r * std::cos(angle);
  y = r * std::sin(angle);
//  x = (outerPMT_BotR+outerPMT_Expose/2.) * std::cos(angle);
//  y = (outerPMT_BotR+outerPMT_Expose/2.) * std::sin(angle);
  z = -outerPMT_Apitch/4.;

  new G4PVPlacement(g4rot, G4ThreeVector(x,y,z),
                    ConstructPMT(outerPMT_Radius,outerPMT_Expose),
                    "PMT",
                    pmtCellLV,
                    false,0,checkOverlaps);

  angle = phi - dphi;

  x = r * std::cos(angle);
  y = r * std::sin(angle);
//  x = (outerPMT_BotR+outerPMT_Expose/2.) * std::cos(angle);
//  y = (outerPMT_BotR+outerPMT_Expose/2.) * std::sin(angle);
  z = outerPMT_Apitch/4.;

  new G4PVPlacement(g4rot, G4ThreeVector(x,y,z),
                    ConstructPMT(outerPMT_Radius,outerPMT_Expose),
                    "PMT",
                    pmtCellLV,
                    false,0,checkOverlaps);

  // End Wall PMTs

  pmtCellLV = ConstructEndWallPMT();

  g4rot = new G4RotationMatrix();
  g4rot->rotateY(180.*deg);

  new G4PVPlacement(g4rot, G4ThreeVector(0.,0.,-innerPMT_Expose/2.),
                    ConstructPMT(innerPMT_Radius,innerPMT_Expose),
                    "PMT",
                    pmtCellLV,
                    false,0,checkOverlaps);

  // Top and Bottom PMTs

  pmtCellLV = ConstructCeilingPMT(true,  innerPMT_TopW,   innerPMT_Height, 
                                         innerPMT_Apitch, innerPMT_Expose);

  g4rot = new G4RotationMatrix();
  g4rot->rotateX(-90.*deg);

  new G4PVPlacement(g4rot, G4ThreeVector(0.,0.,0.),
                    ConstructPMT(innerPMT_Radius,innerPMT_Expose),
                    "PMT",
                    pmtCellLV,
                    false,0,checkOverlaps);

  pmtCellLV = ConstructCeilingPMT(false, innerPMT_BotW,   innerPMT_Height,
                                         innerPMT_Apitch, innerPMT_Expose);

  new G4PVPlacement(g4rot, G4ThreeVector(0.,0.,0.),
                    ConstructPMT(innerPMT_Radius,innerPMT_Expose),
                    "PMT",
                    pmtCellLV,
                    false,0,checkOverlaps);

  pmtCellLV = ConstructCeilingPMT(true,  outerPMT_TopW,   outerPMT_Height,
                                  outerPMT_Apitch, outerPMT_Expose);

  g4rot = new G4RotationMatrix();
  g4rot->rotateX(90.*deg);

  new G4PVPlacement(g4rot, G4ThreeVector(0.,0.,0.),
                    ConstructPMT(outerPMT_Radius,outerPMT_Expose),
                    "PMT",
                    pmtCellLV,
                    false,0,checkOverlaps);

  pmtCellLV = ConstructCeilingPMT(false, outerPMT_BotW,   outerPMT_Height,
                                  outerPMT_Apitch, outerPMT_Expose);

  new G4PVPlacement(g4rot, G4ThreeVector(0.,0.,0.),
                    ConstructPMT(outerPMT_Radius,outerPMT_Expose),
                    "PMT",
                    pmtCellLV,
                    false,0,checkOverlaps);

  //
  // always return the physical HyperK volume
  //
  return hyperKLV;
}

G4VSolid* WCSimDetectorConstruction::ConstructHalf(G4double waterTank_Radius,
                                                   G4double waterTank_OffSet)
{
  G4double sinPhi = (waterTank_Height/2.)/waterTank_Radius;
  G4double cosPhi = std::sqrt(1.-sinPhi*sinPhi);

  G4double sinPhiHalf = std::sqrt((1.-cosPhi)/2.);
  G4double cosPhiHalf = std::sqrt((1.+cosPhi)/2.);

  G4double phi = std::asin(sinPhi);

  G4Trap* waterTank_trap 
    = new G4Trap("TankTrap",
                 waterTank_Length/2.,0.,0.,
                 waterTank_Height/4.,
                 waterTank_Radius-waterTank_OffSet,
                 waterTank_Radius*cosPhi-waterTank_OffSet,
                 0,
                 waterTank_Height/4.,
                 waterTank_Radius-waterTank_OffSet,
                 waterTank_Radius*cosPhi-waterTank_OffSet,
                 0);

  const G4double epsilon = 1*mm;
  G4VSolid* waterTank_slice
    = new G4SubtractionSolid("TankSlice",
                             new G4Tubs("WaterTubs",
                                        0.,waterTank_Radius,
                                        waterTank_Length/2.,
                                        -phi/2.,phi),
                             new G4Box("WaterBox",
                                       waterTank_Radius*cosPhiHalf,
                                       waterTank_Radius*sinPhiHalf,
                                       waterTank_Length/2.+epsilon));

  G4RotationMatrix rotm = G4RotationMatrix();
  rotm.rotateZ(phi/2.);

  G4VSolid* waterTank_union1
    = new G4UnionSolid("Union1",waterTank_trap,waterTank_slice,
                       G4Transform3D(rotm,G4ThreeVector(-waterTank_OffSet,
                                                        -waterTank_Height/4.,
                                                        0)));

  rotm = G4RotationMatrix();
  rotm.rotateZ(phi/2.);
  rotm.rotateY(180.*degree);

  G4VSolid* waterTank_union2
    = new G4UnionSolid("Union2",waterTank_union1,waterTank_slice,
                       G4Transform3D(rotm,G4ThreeVector(waterTank_OffSet,
                                                        -waterTank_Height/4.,
                                                        0)));
  return waterTank_union2;
}


G4LogicalVolume* WCSimDetectorConstruction::ConstructRadialPMT(G4bool top,
                                                               G4double radius,
                                                               G4double height,
                                                               G4double offset,
                                                               G4double expose,
                                                               G4double pitchR,
                                                               G4double pitchZ)
{
  G4bool inner = false;
  if (height == innerPMT_Height) inner = true;

  G4double sinPhi = height/radius;

  G4double phi = std::asin(sinPhi);
  G4int nphi = (G4int)(radius*phi/pitchR);

  if(radius == innerPMT_TopR){
    nphi = nphi + 1;
    innerPMT_TopN = nphi;
  }
  if(radius == innerPMT_BotR){
    innerPMT_BotN = nphi;
  }
  if(radius == outerPMT_TopR){
    nphi = innerPMT_TopN/3;
  }
  if(radius == outerPMT_BotR){
    nphi = innerPMT_BotN/3;
  }

  phi = nphi * pitchR/radius;

  G4double dphi = pitchR/radius;

  G4double length = waterTank_Length;
  G4int nz = (G4int)(length/pitchZ);
  length = nz * pitchZ;

  G4LogicalVolume* pmtAnnulusLV
    = new G4LogicalVolume(new G4Tubs("PMTAnnulus",
                                     radius-2.*expose,radius,
                                     length/2.,
                                     -phi/2.,phi),
                          FindMaterial("G4_WATER"),
                          "PMTAnnulus");
//  pmtAnnulusLV->SetVisAttributes(G4VisAttributes::Invisible);

  G4LogicalVolume* blackSheetALV = NULL;
  if (inner) {
     blackSheetALV =
       new G4LogicalVolume(new G4Tubs("blackSheetA",
                                     radius,radius+blackSheetThickness,
                                     length/2.,
                                     -phi/2.,phi),
                          FindMaterial("G4_POLYETHYLENE"),
                          "blackSheetA");
//     blackSheetALV->SetVisAttributes(new G4VisAttributes(G4Colour(0,0.5,1.)));
     new G4LogicalSkinSurface("WaterBSSurface",blackSheetALV,OpWaterBSSurface);
  }

  G4LogicalVolume* pmtRingLV
    = new G4LogicalVolume(new G4Tubs("PMTRing",
                                     radius-2.*expose,radius,
                                     pitchZ/2.,
                                     -phi/2.,phi),
                          FindMaterial("G4_WATER"),
                          "PMTRings");
//  pmtRingLV->SetVisAttributes(G4VisAttributes::Invisible);

  G4LogicalVolume* pmtCellLV
    = new G4LogicalVolume(new G4Tubs("PMTCell",
                                     radius-2.*expose,radius,
                                     pitchZ/2.,
                                     -dphi/2.,dphi),
                          FindMaterial("G4_WATER"),
                          "PMTCell");
//  pmtCellLV->SetVisAttributes(G4VisAttributes::Invisible);

//-----------------------------------------------------------------
//
//  G4Tubs* cell_solid = new G4Tubs("Cell",
//                                  radius-2.*expose,radius,
//                                  pitchZ/2.,
//                                  -dphi/2.,dphi);
//  G4LogicalVolume* cellLV = new G4LogicalVolume(cell_solid,
//                                                FindMaterial("G4_WATER"),
//                                                "Cell");
//  G4VisAttributes* red = new G4VisAttributes(G4Colour(255/255.,0/255.,0/255.));
//  cellLV->SetVisAttributes(red);
//  G4RotationMatrix* g4rot = new G4RotationMatrix();
//  G4double angle = -phi/2.+dphi/2.;
//  g4rot->rotateZ(angle);
//  *g4rot = g4rot->inverse();
//  for (G4int i=0; i<nz; i++) {
//  G4int i=0;
//  G4double z = -length/2. + pitchZ/2. + i * pitchZ;
//  new G4PVPlacement(g4rot,G4ThreeVector(0,0,z),cellLV,"Cell",pmtAnnulusLV,
//                    false,i,checkOverlaps);
//  }
//
//-----------------------------------------------------------------

  G4VPhysicalVolume* pmtRingPV
    = new G4PVReplica("PMTRing",
                      pmtRingLV,
                      pmtAnnulusLV,
                      kZAxis,
                      nz,
                      pitchZ);

  new G4PVReplica("PMTCell",
                  pmtCellLV,
                  pmtRingPV,
                  kPhi,
                  nphi,
                  dphi,
                  -phi/2.);

  G4RotationMatrix* rotm = new G4RotationMatrix();
  rotm->rotateZ(-phi/2.);
  if(!top)rotm->rotateZ(180.*degree);

  G4int t = 1;
  if(!top)t = -1;

  new G4PVPlacement(rotm,G4ThreeVector(-t*offset,-waterTank_Height/4.,0),
                pmtAnnulusLV,"PMTAnnulus",waterTankLV,false,0,checkOverlaps);
  if (inner) 
     new G4PVPlacement(rotm,G4ThreeVector(-t*offset,-waterTank_Height/4.,0),
                blackSheetALV,"blackSheetA",waterTankLV,false,0,checkOverlaps);

  rotm = new G4RotationMatrix();
  rotm->rotateZ(phi/2.);
  if(!top)rotm->rotateZ(180.*degree);
  rotm->rotateY(180.*degree);

  new G4PVPlacement(rotm,G4ThreeVector( t* offset,-waterTank_Height/4.,0),
                pmtAnnulusLV,"PMTAnnulus",waterTankLV,false,1,checkOverlaps);
  if (inner)
     new G4PVPlacement(rotm,G4ThreeVector( t* offset,-waterTank_Height/4.,0),
                blackSheetALV,"blackSheetA",waterTankLV,false,1,checkOverlaps);

//  return cellLV;
  return pmtCellLV;
}

G4LogicalVolume* WCSimDetectorConstruction::ConstructEndWallPMT()
{
  G4LogicalVolume* pmtCellLV
    = new G4LogicalVolume(new G4Box("PMTCell",
                                    innerPMT_Apitch/2.,
                                    innerPMT_Apitch/2.,
                                    innerPMT_Apitch/2.),
                          FindMaterial("G4_WATER"),
                          "PMTCell");
  pmtCellLV->SetVisAttributes(G4VisAttributes::Invisible);

  G4RotationMatrix* rotm = new G4RotationMatrix();
  rotm->rotateY(180.*degree);

  G4int i = 0;
  G4double x = innerPMT_Apitch/2.;
  G4ThreeVector xv = G4ThreeVector(x,0.,0.);

  while (x < innerPMT_TopR - waterTank_UpperA)
  
  {
    G4int j = 0;
    G4double y = innerPMT_Apitch/2.;
    G4ThreeVector ov = G4ThreeVector(waterTank_UpperA,0.,0.);
    G4ThreeVector yv = G4ThreeVector(0.,y,0.);

//    while (y < innerPMT_Height - innerPMT_Radius)
    while (y < innerPMT_Height - 2*innerPMT_Expose)
    {
      G4ThreeVector dv = ov + xv + yv;
//      if (dv.mag() > innerPMT_TopR - innerPMT_Radius) break;
      if (dv.mag() > innerPMT_TopR - 2*innerPMT_Expose) break;
      j++;
      y+=innerPMT_Apitch;
      yv = G4ThreeVector(0.,y,0.);
    }
    G4int ny = j;
    j--;
    y-=innerPMT_Apitch;

    G4double yt = y;

    j = 0;
    y = innerPMT_Apitch/2.;
    ov = G4ThreeVector(waterTank_LowerB,0.,0.);
    yv = G4ThreeVector(0.,y,0.);

//    while (y < innerPMT_Height - innerPMT_Radius)
    while (y < innerPMT_Height - 2*innerPMT_Expose)
    {
      G4ThreeVector dv = ov + xv + yv;
//      if (dv.mag() > innerPMT_BotR - innerPMT_Radius) break;
      if (dv.mag() > innerPMT_BotR - 2*innerPMT_Expose) break;
      j++;
      y+=innerPMT_Apitch;
      yv = G4ThreeVector(0.,y,0.);
    }
    j--;
    ny += j;
    y-=innerPMT_Apitch;

    G4double yb = y;

    G4LogicalVolume* pmtSlabLV
      = new G4LogicalVolume(new G4Box("PMTSlab",
                                      innerPMT_Apitch/2.,
                                      (yt+yb)/2.,
                                      innerPMT_Apitch/2.),
                            FindMaterial("G4_WATER"),
                            "PMTSlab");
    pmtSlabLV->SetVisAttributes(G4VisAttributes::Invisible);

    new G4PVReplica("PMTCell",
                    pmtCellLV,
                    pmtSlabLV,
                    kYAxis,
                    ny,
                    innerPMT_Apitch);

    G4double xpos = x-innerPMT_Apitch/2.;
    G4double ypos = (yt-yb)/2. - waterTank_Height/4. ;
    G4double zpos = waterTank_Length/2.-innerPMT_Apitch/2.;

    new G4PVPlacement(0,   G4ThreeVector(xpos,ypos, zpos),
                      pmtSlabLV,"PMTSlab",waterTankLV,false,0,checkOverlaps);
    new G4PVPlacement(rotm,G4ThreeVector(xpos,ypos,-zpos),
                      pmtSlabLV,"PMTSlab",waterTankLV,false,1,checkOverlaps);
    if ( i > 0 ) {
       xpos = -xpos;
       new G4PVPlacement(0,   G4ThreeVector(xpos,ypos, zpos),
                         pmtSlabLV,"PMTSlab",waterTankLV,false,3,checkOverlaps);
       new G4PVPlacement(rotm,G4ThreeVector(xpos,ypos,-zpos),
                         pmtSlabLV,"PMTSlab",waterTankLV,false,4,checkOverlaps);
    }

    i++;
    x+=innerPMT_Apitch;
    xv = G4ThreeVector(x,0.,0.);

  }

  return pmtCellLV;
}

G4LogicalVolume* WCSimDetectorConstruction::ConstructCeilingPMT(G4bool top, 
                                                                G4double width,
                                                                G4double height,
                                                                G4double pitch,
                                                                G4double size)
{
  G4bool inner = false;
  if (height == innerPMT_Height) inner = true;

  G4int nx = (G4int)(2.*width/pitch);

  // ***** to fix the overlap with annulus *****
  nx = nx - 1;

  G4double xlength = nx * pitch;
  G4int nz = (G4int)(waterTank_Length/pitch);
  G4double zlength = nz * pitch;

  G4LogicalVolume* pmtSlabLV
      = new G4LogicalVolume(new G4Box("PMTSlab",
                                      xlength/2.,
                                      size/2.,
                                      zlength/2.),
                            FindMaterial("G4_WATER"),
                            "PMTSlab");
  pmtSlabLV->SetVisAttributes(G4VisAttributes::Invisible);

  G4LogicalVolume* blackSheetYLV = NULL;
  if (inner) {
     blackSheetYLV =
        new G4LogicalVolume(new G4Box("blackSheetY",
                                      xlength/2.,
                                      blackSheetThickness/2.,
                                      zlength/2.),
                            FindMaterial("G4_POLYETHYLENE"),
                            "blackSheetY");
//     blackSheetYLV->SetVisAttributes(new G4VisAttributes(G4Colour(0,0.5,1.)));
     new G4LogicalSkinSurface("WaterBSSurface",blackSheetYLV,OpWaterBSSurface);
  }

  G4LogicalVolume* pmtSliceLV
      = new G4LogicalVolume(new G4Box("PMTSlice",
                                      xlength/2.,
                                      size/2.,
                                      pitch/2.),
                            FindMaterial("G4_WATER"),
                            "PMTSlab");
  pmtSliceLV->SetVisAttributes(G4VisAttributes::Invisible);

  G4LogicalVolume* pmtCellLV
    = new G4LogicalVolume(new G4Box("PMTCell",
                                    pitch/2.,
                                    size/2.,
                                    pitch/2.),
                          FindMaterial("G4_WATER"),
                          "PMTCell");
  pmtCellLV->SetVisAttributes(G4VisAttributes::Invisible);

  G4VPhysicalVolume* pmtSlicePV
    = new G4PVReplica("PMTSlice",
                      pmtSliceLV,
                      pmtSlabLV,
                      kZAxis,
                      nz,
                      pitch);

  new G4PVReplica("PMTCell",
                  pmtCellLV,
                  pmtSlicePV,
                  kXAxis,
                  nx,
                  pitch);

  G4double xpos = 0.0*mm;
  G4double ypos = (height-size/2.) - waterTank_Height/4.;
  G4double zpos = 0.0*mm;

  if (top) {
     new G4PVPlacement(0,G4ThreeVector(xpos,ypos,zpos),
                       pmtSlabLV,"PMTSlab",waterTankLV,false,0,checkOverlaps);
     ypos += size/2. + blackSheetThickness/2.;
     if (inner) new G4PVPlacement(0,G4ThreeVector(xpos,ypos,zpos),
                 blackSheetYLV,"blackSheet",waterTankLV,false,0,checkOverlaps);
  } else {
     ypos = (-height+size/2.) - waterTank_Height/4.;
     G4RotationMatrix* rotm = new G4RotationMatrix();
     rotm->rotateZ(180.*degree);
     new G4PVPlacement(rotm,G4ThreeVector(xpos,ypos,zpos),
                       pmtSlabLV,"PMTSlab",waterTankLV,false,1,checkOverlaps);
     ypos -= size/2. + blackSheetThickness/2.;
     if (inner) new G4PVPlacement(0,G4ThreeVector(xpos,ypos,zpos),
                 blackSheetYLV,"blackSheet",waterTankLV,false,1,checkOverlaps);

  }

  return pmtCellLV;
}

G4Material* WCSimDetectorConstruction::FindMaterial(G4String name)
{
    if (name == "G4_AIR") return FindMaterial("Air");
    if (name == "G4_WATER") return FindMaterial("Water");
    if (name == "G4_POLYETHYLENE") return FindMaterial("Blacksheet");
    G4Material* material = G4Material::GetMaterial(name,true);
    return material;
}
