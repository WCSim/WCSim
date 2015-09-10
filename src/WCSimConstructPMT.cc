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
#include "WCSimPMTObject.hh"

#include "G4SystemOfUnits.hh"

#include "G4LogicalSkinSurface.hh"

//PMT logical volume construction.
//WCSimDetectorConstruction::PMTMap_t WCSimDetectorConstruction::PMTLogicalVolumes;

//ToDo: use enum instead of string for PMTtype, and PMTs should be objects of one simple class.
G4LogicalVolume* WCSimDetectorConstruction::ConstructPMT(G4String PMTName, G4String CollectionName)
{
  PMTKey_t key(PMTName,CollectionName);

  // Return pre-created PMT Logical Volume if it already exists.
  PMTMap_t::iterator it = PMTLogicalVolumes.find(key);
  if (it != PMTLogicalVolumes.end()) {
      //G4cout << "Restore PMT" << G4endl;
      return it->second;
  }

  //G4cout << "Create PMT" << G4endl;


  if (Vis_Choice == "RayTracer"){
    // Blue wireframe visual style
    // Used in the RayTracer visualizer
    G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
    WCPMTVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
    WCPMTVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown 
  }
  
  else { 
    // Gray wireframe visual style
    // used in OGLSX visualizer
    //G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
    //WCPMTVisAtt->SetForceWireframe(true);
    G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
    WCPMTVisAtt->SetForceSolid(true);
  }
  
  G4double expose = 0.;
  G4double radius = 0.;
  G4double glassThickness = 0.;
    
  WCSimPMTObject *PMT = GetPMTPointer(CollectionName);
  expose = PMT->GetExposeHeight();
  radius = PMT->GetRadius();                            //r at height = expose
  glassThickness = PMT->GetPMTGlassThickness();

  //sphereRadius R: radius of curvature, based on spherical approx near exposeHeight
  //if radius of spherical cap at exposeHeight = PMTradius r (eg. 20"/2)
  //then from r = sqrt(R*R - (R-expose)*(R-expose)), we get:
  G4double sphereRadius = (expose*expose + radius*radius)/(2*expose);
  G4double PMTOffset =  sphereRadius - expose;

  //All components of the PMT are now contained in a single logical volume logicWCPMT.
  //Origin is on the blacksheet, faces positive z-direction.

  //Optional reflectorCone:
  G4double reflectorRadius = radius + id_reflector_height * tan(id_reflector_angle);
  G4double reflectorThickness = 1.*CLHEP::mm;
  if(reflectorRadius < 1.*CLHEP::mm)
    reflectorThickness = 0.*CLHEP::mm;

  // ToDo: Base is PMT property! Should not hard coded here.
  G4bool addPMTBase = false; 
  G4double baseHeight = 0.;
  G4double baseRadius = 0.;
  if(addPMTBase){
    G4double basePinLength = 28.*CLHEP::mm; //TF measurement: pins + base + transformer.
    baseHeight = 97.*CLHEP::mm - expose + basePinLength; //97mm includes the PMT top as well.
    baseRadius = 26.*CLHEP::mm; //for R121990-02
  } else {
    // version without a base but with optional reflectorCone
    baseHeight = expose;
    baseRadius = radius + reflectorThickness;
  }
  G4double PMTHolderZ[2] = {-baseHeight+expose, 
			    std::max(expose,id_reflector_height)};
  G4double PMTHolderR[2] = {baseRadius, 
			    std::max(radius,reflectorRadius) + reflectorThickness};
  G4double PMTHolderr[2] = {0,0};

  G4Polycone* solidWCPMT = 
   new G4Polycone("WCPMT",                    
                  0.0*deg,
                  360.0*deg,
                  2,
                  PMTHolderZ,
                  PMTHolderr, // R Inner
                  PMTHolderR);// R Outer

  G4Material *material_around_pmt = G4Material::GetMaterial("Water");
  if(id_reflector_height > 0.1*CLHEP::mm && 
     (cylinder_radius > 1.*CLHEP::mm || cylinder_height > 1.*CLHEP::mm)) //or make this a user option? 
    material_around_pmt = G4Material::GetMaterial("SilGel");
  G4LogicalVolume* logicWCPMT =
    new G4LogicalVolume(    solidWCPMT,
                            material_around_pmt,
                            "WCPMT",
                            0,0,0);

  if (Vis_Choice == "RayTracer"){
    // Makes the volume containing the PMT visible, solid, and forces the auxiliary edges to be viewed.
    G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
    WCPMTVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
    WCPMTVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown 
    
    logicWCPMT->SetVisAttributes(WCPMTVisAtt);
  } else{
    G4VisAttributes* WCPMTVisAtt2 = new G4VisAttributes(G4Colour(1.0,0.0,0.0));
    WCPMTVisAtt2->SetForceSolid(true);
    if(addPMTBase) 
      logicWCPMT->SetVisAttributes(WCPMTVisAtt2);
    else
      // Makes the volume containg the PMT invisible for normal visualization
      logicWCPMT->SetVisAttributes(G4VisAttributes::Invisible);
  }

  //Need a volume to cut away excess behind blacksheet
  G4Box* solidCutOffTubs =
      new G4Box(    "cutOffTubs",
            sphereRadius+1.*cm,
            sphereRadius+1.*cm,
            PMTOffset);

  ////////////////////////////
  ///  Create PMT Interior ///
  ////////////////////////////

  G4Sphere* tmpSolidInteriorWCPMT =
      new G4Sphere(    "tmpInteriorWCPMT",
                       0.0*m,(sphereRadius-glassThickness),
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
		  0,
		  checkOverlaps);

if (Vis_Choice == "RayTracer"){
// Adding color and forcing the inner portion of the PMT's to be viewed
  G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  WCPMTVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
  WCPMTVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown 

  logicInteriorWCPMT->SetVisAttributes(WCPMTVisAtt);}

else {
// Making the inner portion of the detector invisible for OGLSX visualization
  logicInteriorWCPMT->SetVisAttributes(G4VisAttributes::Invisible);}

  /////////////////////////////
  /// Create PMT Glass Face ///
  /////////////////////////////

  G4Sphere* tmpGlassFaceWCPMT =
      new G4Sphere(    "tmpGlassFaceWCPMT",
                       (sphereRadius-glassThickness),
                       sphereRadius,
                       0.0*deg,360.0*deg,
                       0.0*deg,90.0*deg);
  
  G4SubtractionSolid* solidGlassFaceWCPMT =
      new G4SubtractionSolid(    CollectionName,
                                 tmpGlassFaceWCPMT,
                                 solidCutOffTubs); 

  G4LogicalVolume *logicGlassFaceWCPMT =
    new G4LogicalVolume(    solidGlassFaceWCPMT,
                            G4Material::GetMaterial("Glass"),
                            CollectionName,
                            0,0,0);

  G4VPhysicalVolume* physiGlassFaceWCPMT =
      new G4PVPlacement(0,
                        G4ThreeVector(0, 0, -1.0*PMTOffset),
                        logicGlassFaceWCPMT,
                        CollectionName,
                        logicWCPMT,
                        false,
                        0,
                        checkOverlaps);

// For either visualization type, logicGlassFaceWCPMT will either be visible or invisible depending on which
// line is commented at the end of the respective if statements

  if (Vis_Choice == "OGLSX")
   { // Gray wireframe visual style
    // used in OGLSX visualizer
  G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
  WCPMTVisAtt->SetForceWireframe(true);
  //logicGlassFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible);
  logicGlassFaceWCPMT->SetVisAttributes(WCPMTVisAtt);}

  if (Vis_Choice == "RayTracer"){
    // Blue wireframe visual style
    // Used in the RayTracer visualizer
  G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  WCPMTVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
  WCPMTVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown 
  //logicGlassFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible);

  logicGlassFaceWCPMT->SetVisAttributes(WCPMTVisAtt);}

  else
   { // Gray wireframe visual style
     // used in OGLSX visualizer
     G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
     WCPMTVisAtt->SetForceWireframe(true);
     //logicGlassFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible);
     logicGlassFaceWCPMT->SetVisAttributes(WCPMTVisAtt);
   }

  //Add Logical Border Surface
  new G4LogicalBorderSurface("GlassCathodeSurface",
                             physiGlassFaceWCPMT,
                             physiInteriorWCPMT,
                             OpGlassCathodeSurface);


  // TF: Optional Reflector
  if(id_reflector_height > 0.1*CLHEP::mm && (reflectorRadius-radius) > -5*CLHEP::mm){
    G4double reflectorZ[2] = {0, id_reflector_height};
    G4double reflectorR[2] = {radius + reflectorThickness, reflectorThickness + reflectorRadius};
    G4double reflectorr[2] = {radius,reflectorRadius};
    
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
  }

  // Instantiate a new sensitive detector and register this sensitive detector volume with the SD Manager. 
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  G4String SDName = "/WCSim/";
  SDName += CollectionName;
  WCSimWCSD* aWCPMT = new WCSimWCSD(CollectionName,SDName,this );
  SDman->AddNewDetector( aWCPMT );
  

  logicGlassFaceWCPMT->SetSensitiveDetector( aWCPMT );

  PMTLogicalVolumes[key] = logicWCPMT;

  return logicWCPMT;
}
