#include "WCSimDetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4Polycone.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4SDManager.hh"
#include "WCSimWCSD.hh"

#include "G4SystemOfUnits.hh"

#include "G4LogicalSkinSurface.hh"

//PMT logical volume construction.

WCSimDetectorConstruction::PMTMap_t WCSimDetectorConstruction::PMTLogicalVolumes;

//TF: ToDo: arg should be PMT type so that radius and expose are just generic function calls.
G4LogicalVolume* WCSimDetectorConstruction::ConstructPMT(G4double radius,
                                                         G4double expose)
{
  PMTKey_t key(radius,expose);

  PMTMap_t::iterator it = PMTLogicalVolumes.find(key);
  if (it != PMTLogicalVolumes.end()) {
      //G4cout << "Restore PMT" << G4endl;
      return it->second;
  }

  //G4cout << "Create PMT" << G4endl;

    // Gray wireframe visual style
  //G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
  //WCPMTVisAtt->SetForceWireframe(true);
  G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
  WCPMTVisAtt->SetForceSolid(true);
    

  G4double sphereRadius = (expose*expose+ radius*radius)/(2*expose);
  G4double PMTOffset =  sphereRadius - expose;

  //All components of the PMT are now contained in a single logical volume logicWCPMT.
  //Origin is on the blacksheet, faces positive z-direction.

  //TF: Optional NEW: IF reflector/Winston cone requested, make it here:

  //TODO: Macro option, can be positive and negative, but not smaller than expose
  // Relative to expose
  G4double reflectorHeight = 7.5*CLHEP::mm-expose;   //7.5mm from KM3Net JINST
  // Radius of cone at z=expose+reflectorHeight, relative to PMT radius
  G4double reflectorRadius = 7.5*CLHEP::mm;  //based on KM3Net JINST
  G4double reflectorThickness = 1.*CLHEP::mm;
  G4bool addBase = true;


  /* version without a base 
  G4double PMTHolderZ[2] = {0, std::max(expose,expose + reflectorHeight)};
  G4double PMTHolderR[2] = {radius + reflectorThickness, radius + reflectorThickness + reflectorRadius};
  G4double PMTHolderr[2] = {0,0};
  */
  G4double baseHeight = 97.*CLHEP::mm - expose + 73.*CLHEP::mm; //the latter are the pins
  G4double baseRadius = 26.*CLHEP::mm;
  G4double PMTHolderZ[2] = {-baseHeight+expose, std::max(expose,expose + reflectorHeight)};
  G4double PMTHolderR[2] = {baseRadius, radius + reflectorThickness + reflectorRadius};
  G4double PMTHolderr[2] = {0,0};



  // IF reflectorParams are non-zero, this will be a solid cone instead of cylinder
  G4Polycone* solidWCPMT = 
   new G4Polycone("WCPMT",                    
                  0.0*deg,
                  360.0*deg,
                  2,
                  PMTHolderZ,
                  PMTHolderr, // R Inner
                  PMTHolderR);// R Outer


  G4LogicalVolume* logicWCPMT =
    new G4LogicalVolume(    solidWCPMT,
                            G4Material::GetMaterial("Water"),
                            "WCPMT",
                            0,0,0);

  G4VisAttributes* WCPMTVisAtt2 = new G4VisAttributes(G4Colour(1.0,0.0,0.0));
  WCPMTVisAtt2->SetForceSolid(true);
  logicWCPMT->SetVisAttributes(WCPMTVisAtt2);
  //  logicWCPMT->SetVisAttributes(G4VisAttributes::Invisible);


  //Need a volume to cut away excess behind blacksheet
  G4Box* solidCutOffTubs =
      new G4Box(    "cutOffTubs",
            sphereRadius+1.*cm,
            sphereRadius+1.*cm,
            PMTOffset);


  //Create PMT Interior
  G4Sphere* tmpSolidInteriorWCPMT =
      new G4Sphere(    "tmpInteriorWCPMT",
                       0.0*m,(sphereRadius-WCPMTGlassThickness),
                       0.0*deg,360.0*deg,
                       0.0*deg,90.0*deg);

  G4SubtractionSolid* solidInteriorWCPMT =
      new G4SubtractionSolid(    "InteriorWCPMT",
                    tmpSolidInteriorWCPMT,
                    solidCutOffTubs);

  // "Air" here is not true air, but a modified material
  // with n = 1 and a very short absorption length
  G4LogicalVolume* logicInteriorWCPMT =
    new G4LogicalVolume(    solidInteriorWCPMT,
                    G4Material::GetMaterial("Air"),
                    "InteriorWCPMT",
                    0,0,0);

  G4VPhysicalVolume* physiInteriorWCPMT =
      new G4PVPlacement(0,
                  G4ThreeVector(0, 0, -1.0*PMTOffset),
                  logicInteriorWCPMT,
                  "InteriorWCPMT",
                  logicWCPMT,
                  false,
                  0);

  logicInteriorWCPMT->SetVisAttributes(G4VisAttributes::Invisible);


  //Create PMT Glass Face
  G4Sphere* tmpGlassFaceWCPMT =
      new G4Sphere(    "tmpGlassFaceWCPMT",
                       (sphereRadius-WCPMTGlassThickness),
                       sphereRadius,
                       0.0*deg,360.0*deg,
                       0.0*deg,90.0*deg);
  
  G4SubtractionSolid* solidGlassFaceWCPMT =
      new G4SubtractionSolid(    "glassFaceWCPMT",
                                 tmpGlassFaceWCPMT,
                                 solidCutOffTubs); 

  G4LogicalVolume *logicGlassFaceWCPMT =
    new G4LogicalVolume(    solidGlassFaceWCPMT,
                            G4Material::GetMaterial("Glass"),
                            "glassFaceWCPMT",
                            0,0,0);

  G4VPhysicalVolume* physiGlassFaceWCPMT =
      new G4PVPlacement(0,
                        G4ThreeVector(0, 0, -1.0*PMTOffset),
                        logicGlassFaceWCPMT,
                        "glassFaceWCPMT",
                        logicWCPMT,
                        false,
                        0,
                        checkOverlaps);

  //logicGlassFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible);
  logicGlassFaceWCPMT->SetVisAttributes(WCPMTVisAtt);

  if (!aWCPMT)
  {
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    aWCPMT = new WCSimWCSD( "/WCSim/glassFaceWCPMT",this );
    SDman->AddNewDetector( aWCPMT );
  }
  logicGlassFaceWCPMT->SetSensitiveDetector( aWCPMT );

  PMTLogicalVolumes[key] = logicWCPMT;

  //Add Logical Border Surface
  new G4LogicalBorderSurface("GlassCathodeSurface",
                             physiGlassFaceWCPMT,
                             physiInteriorWCPMT,
                             OpGlassCathodeSurface);


  //TF: Reflector
  G4double reflectorZ[2] = {0, expose + reflectorHeight};
  G4double reflectorR[2] = {radius + reflectorThickness, radius + reflectorThickness + reflectorRadius};
  G4double reflectorr[2] = {radius,radius + reflectorRadius};

  // IF reflectorParams are non-zero, this will be a solid cone instead of cylinder
  G4Polycone* reflectorCone = 
   new G4Polycone("WCPMT",                    
                  0.0*deg,
                  360.0*deg,
                  2,
                  reflectorZ,
                  reflectorr, // R Inner
                  reflectorR);// R Outer

  G4LogicalVolume* logicReflector =
    new G4LogicalVolume(    reflectorCone,
                            G4Material::GetMaterial("Aluminum"), //It actually is Al+ Ag evaporation
                            "reflectorCone",
                            0,0,0);

  G4VisAttributes* WCPMTVisAtt3 = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  WCPMTVisAtt3->SetForceSolid(true);
  logicReflector->SetVisAttributes(WCPMTVisAtt3);

  new G4LogicalSkinSurface("ReflectorLogSkinSurface",logicReflector,ReflectorSkinSurface);
  
  G4VPhysicalVolume* reflectorWCPMT =
      new G4PVPlacement(0,
                        G4ThreeVector(0, 0, 0),
                        logicReflector,
                        "reflectorWCPMT",
                        logicWCPMT,
                        false,
                        0,
                        checkOverlaps);


  return logicWCPMT;
}
