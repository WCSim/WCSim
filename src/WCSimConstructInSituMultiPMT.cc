#include "WCSimDetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Cons.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4Polycone.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4SDManager.hh"
#include "WCSimWCSD.hh"

#include "G4SystemOfUnits.hh"

#include  "WCSimMultiPMTParameterisation.hh"
#include  "G4PVParameterised.hh"

// ex-situ mPMT logical volume construction.
// A function of the WCSimDetectorConstruction class
// dimensions are measured from actual mPMT construction and hard-coded

// 3-inch PMT construction
G4LogicalVolume* WCSimDetectorConstruction::ConstructInSituPMT(G4String PMTName, G4String CollectionName, G4String detectorElement)
{
  G4String keyname = PMTName+"_InSitu-PMT";
  PMTKey_t key(keyname,CollectionName);
  // Return pre-created PMT Logical Volume if it already exists.
  PMTMap_t::iterator it = PMTLogicalVolumes.find(key);
  if (it != PMTLogicalVolumes.end()) {
    return it->second;
  }

  /////////////////////////////////////////
  // From actual measurements
  G4double pmtGlassThickness = 2.0*mm;
  G4double pmtGlassRadius = 53.0*mm;
  G4double pmtCylRadius = 40.15*mm;
  G4double pmtTubeRadius = 26.1*mm;
  G4double pmtCylHeight = 18.0*mm;
  G4double pmtConeHeight = 20.0*mm;
  G4double pmtGlassCut = 34.597*mm;

  G4double reflectorHeight = 6.5*mm*2;
  G4double reflectorThickness = 0.8*mm;
  G4double reflectorR1 = 36.96*mm;
  G4double reflectorR2 = 45.*mm;

  G4cout << "========================================================" << G4endl;
  G4cout << "In-situ PMT construction: " << G4endl;
  G4cout << "Glass radius for PMT " << CollectionName << " : " << pmtGlassRadius/mm << " mm" << G4endl;
  G4cout << "Expose height : " << (pmtGlassRadius-pmtGlassCut)/mm << " mm" << G4endl;
  G4cout << "Cylinder radius : " << pmtCylRadius/mm << " mm" << G4endl;
  G4cout << "Reflector height, R1, R2 : " << reflectorHeight/mm << " " << reflectorR1/mm << " " << reflectorR2/mm << " mm" << G4endl;
  G4cout << "========================================================" << G4endl;

  ///////////////////////////////Defining PMT Volume/////////////////////////////////
  G4double matrxiRadius = 325.603*mm;
  G4double absorberThickness = 1.*mm;
  G4double pmtVolumeHeight = absorberThickness + pmtConeHeight + pmtCylHeight + 23.6*mm;
  G4double zoffset = matrxiRadius-pmtConeHeight/2.-pmtCylHeight- 23.6*mm;
  G4double openingAngle = 8.14*deg;

  G4Sphere *solidWCPMT = new G4Sphere("WCPMT",
                                      matrxiRadius-pmtVolumeHeight,
                                      matrxiRadius,
                                      0.0*deg, 360.0*deg,
                                      0.0, openingAngle);

  G4LogicalVolume *logicWCPMT = new G4LogicalVolume(solidWCPMT,
                                                    G4Material::GetMaterial("SilGel_WCTE"),
                                                    "WCPMT");

  //////////////////////////
  // Visualize logicWCPMT //
  //////////////////////////

  if (Vis_Choice == "RayTracer"){
    // Makes the volume containing the PMT visible, solid, and forces the auxiliary edges to be viewed.
    G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
    WCPMTVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
    WCPMTVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown

    logicWCPMT->SetVisAttributes(WCPMTVisAtt);
  } else{
    G4VisAttributes* WCPMTVisAttGrey = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
    WCPMTVisAttGrey->SetForceSolid(true);
    logicWCPMT->SetVisAttributes(G4VisAttributes::Invisible);
    logicWCPMT->SetVisAttributes(WCPMTVisAttGrey);                   
  }

  // inner glass tube
  G4double glassTubeZ[3] = {-pmtConeHeight/2.,
                            pmtConeHeight/2.,
                            pmtConeHeight/2.+pmtCylHeight};
  G4double glassTubeRmin[3] = { pmtTubeRadius-pmtGlassThickness,
                                pmtCylRadius-pmtGlassThickness,
                                pmtCylRadius-pmtGlassThickness};
  G4double glassTubeRmax[3] = { pmtTubeRadius,
                                pmtCylRadius,
                                pmtCylRadius};
  G4Polycone* solidGlassTube = new G4Polycone("solidGlassTube",
                                              0,
                                              360.*deg,
                                              3,
                                              glassTubeZ,
                                              glassTubeRmin,
                                              glassTubeRmax);
  
  G4LogicalVolume *logicGlassTube = new G4LogicalVolume(solidGlassTube,
                                                        G4Material::GetMaterial("GlassWCTE"),
                                                        "logicInnerGlassTube");
                                        
  new G4PVPlacement(0,
                    G4ThreeVector(0.,0.,zoffset),
                    logicGlassTube,
                    "InnerGlassTube",
                    logicWCPMT,
                    false,
                    0,
                    checkOverlaps);
  
  // air inside glass tube
  G4double airTubeZ[4] =  {-pmtConeHeight/2.,
                            pmtConeHeight/2.,
                            pmtConeHeight/2.,
                            pmtConeHeight/2.+pmtCylHeight};
  G4double airTubeRmin[4] = { 0, 0, 0, 0};
  G4double airTubeRmax[4] = { pmtTubeRadius-pmtGlassThickness-0.1*mm,
                              pmtCylRadius-pmtGlassThickness-0.1*mm,
                              pmtCylRadius-pmtGlassThickness,
                              pmtCylRadius-pmtGlassThickness,
                            };
  G4Polycone* solidAirTube  = new G4Polycone("solidAirTube",
                                              0,
                                              360.*deg,
                                              4,
                                              airTubeZ,
                                              airTubeRmin,
                                              airTubeRmax);

  G4LogicalVolume *innerLogicTube = new G4LogicalVolume(solidAirTube, 
                                                        G4Material::GetMaterial("Air1"),
                                                        "logicInnerAir");
  
  new G4PVPlacement(0,
                    G4ThreeVector(0.,0.,zoffset),
                    innerLogicTube,
                    "InnerAir",
                    logicWCPMT,
                    false,
                    0,
                    checkOverlaps);

  //Reflector material inside the PMT
  G4Cons *solidConeTube = new G4Cons("ConeTube",
                                      pmtTubeRadius-pmtGlassThickness-0.1*mm,
                                      pmtTubeRadius-pmtGlassThickness,
                                      pmtCylRadius-pmtGlassThickness-0.1*mm,
                                      pmtCylRadius-pmtGlassThickness,
                                      pmtConeHeight/2.,
                                      0.*deg, 360.*deg);

  G4LogicalVolume *logicLayer = new G4LogicalVolume(solidConeTube, 
						    G4Material::GetMaterial("Aluminum"), 
						    "logicInnerReflector");

  new G4PVPlacement(0,
                    G4ThreeVector(0.,0.,zoffset),
                    logicLayer,
                    "InnerReflector",
                    logicWCPMT,
                    false,
                    0,
                    checkOverlaps);

  new G4LogicalSkinSurface("ReflectorLogSkinSurface", logicLayer, ReflectorSkinSurfaceWCTE);

  G4VisAttributes *layerAttributes = new G4VisAttributes();
  layerAttributes->SetColor(0.85, 0.85, 0.85, 1.0);
  layerAttributes->SetVisibility(true);
  layerAttributes->SetForceSolid(true);
  logicLayer->SetVisAttributes(layerAttributes);
                  
  // artificial absorber at the bottom of glass tube to kill all photons
  G4Cons* solidAbsorber = new G4Cons("solidAbsorber", 
                                      0.0, 
                                      pmtTubeRadius, 
                                      0.0, 
                                      pmtTubeRadius, 
                                      absorberThickness/2., 
                                      0.0, 360.0*deg);
  
  G4LogicalVolume *logicabsorber = new G4LogicalVolume( solidAbsorber, 
                                                        G4Material::GetMaterial("customAbsorber"),
                                                        "logicabsorber");
  
  new G4PVPlacement(0, 
                    G4ThreeVector(0.,0.,-absorberThickness/2.-pmtConeHeight/2.+zoffset),
                    logicabsorber, 
                    "absorber", 
                    logicWCPMT, 
                    false, 
                    0, 
                    checkOverlaps);

  new G4LogicalSkinSurface("AbsorberSurfaceProperties", logicabsorber, AbsorberSkinSurface);
  
  G4VisAttributes *visAttributes = new G4VisAttributes();
  visAttributes->SetColor(G4Color::Gray());
  visAttributes->SetVisibility(true);
  visAttributes->SetForceSolid(true);
  logicabsorber->SetVisAttributes(visAttributes);

  // Photocathode glass
  G4Box *solidCutOff = new G4Box( "cutOffTubs",
                                  pmtGlassRadius+1.*cm,                                                                     
                                  pmtGlassRadius+1.*cm,                                                                    
                                  pmtGlassCut);

  G4Sphere *pmtGlassSolid = new G4Sphere("pmtglass", 
                                          pmtGlassRadius-pmtGlassThickness, 
                                          pmtGlassRadius, 
                                          0.0*deg, 360.0*deg, 
                                          0.0, 90*deg);  
  
  G4SubtractionSolid *solidGlassFaceWCPMT = new G4SubtractionSolid( CollectionName,                                       
                                                                    pmtGlassSolid,                                   
                                                                    solidCutOff);

  G4LogicalVolume *logicGlassFaceWCPMT = new G4LogicalVolume( solidGlassFaceWCPMT, 
                                                              G4Material::GetMaterial("GlassWCTE"),
                                                              CollectionName);
  
  G4VPhysicalVolume *physiGlassFaceWCPMT = new G4PVPlacement( 0, 
                                                              G4ThreeVector(0.,0.,-pmtGlassCut+pmtConeHeight/2.+pmtCylHeight+zoffset), 
                                                              logicGlassFaceWCPMT, 
                                                              CollectionName, 
                                                              logicWCPMT, 
                                                              false, 
                                                              0, 
                                                              checkOverlaps);

  if (Vis_Choice == "RayTracer"){
    // Blue wireframe visual style
    // Used in the RayTracer visualizer
    G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
    WCPMTVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
    WCPMTVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown
    //logicGlassFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible);

    logicGlassFaceWCPMT->SetVisAttributes(WCPMTVisAtt);
  } else {

    // Gray wireframe visual style
    // used in OGLSX visualizer
    //G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.2,0.2,0.2));  //better for seeing hits
    //WCPMTVisAtt->SetForceWireframe(true);
    G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.0,1.0,0.0)); //better for seeing geometry
    WCPMTVisAtt->SetForceSolid(true);
    //logicGlassFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible);
    logicGlassFaceWCPMT->SetVisAttributes(WCPMTVisAtt);

  }

  // air below photocathode
  G4Sphere *pmtInnerGlassSolid = 
  new G4Sphere("pmtInnerglassSolid",
					      0.*mm,
					      pmtGlassRadius-pmtGlassThickness,
					      0.0*deg, 360.0*deg,
					      0.0, 90*deg);
  G4Box *boxCutOff = 
  new G4Box("cutOffTubs",                                                                            
			       pmtGlassRadius+1.*cm,                                                    
			       pmtGlassRadius+1.*cm,                                             
			       pmtGlassCut);


  G4SubtractionSolid *solidInteriorWCPMT = 
  new G4SubtractionSolid("InteriorWCPMT",     
                          pmtInnerGlassSolid,
                          boxCutOff); 

  G4LogicalVolume *logicInteriorWCPMT = 
  new G4LogicalVolume(solidInteriorWCPMT,
                      G4Material::GetMaterial("Air1"),
                      "InteriorWCPMT");
  
  G4VPhysicalVolume *physiInteriorWCPMT = 
  new G4PVPlacement(0,
                    G4ThreeVector(0.,0.,-pmtGlassCut+pmtConeHeight/2.+pmtCylHeight+zoffset),
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

    logicInteriorWCPMT->SetVisAttributes(WCPMTVisAtt);
  } else {
    // Making the inner portion of the detector invisible for OGLSX visualization
    logicInteriorWCPMT->SetVisAttributes(G4VisAttributes::Invisible);
  }

  //Add Logical Border Surface
  new G4LogicalBorderSurface("GlassCathodeSurface",
                              physiGlassFaceWCPMT,
                              physiInteriorWCPMT,
                              OpGlassCathodeSurface);
  // now it is possible for photons to travel from air to cathode
  new G4LogicalBorderSurface("AirCathodeSurface",
                              physiInteriorWCPMT,
                              physiGlassFaceWCPMT,
                              OpGlassCathodeSurface);

  // Top Reflector
  G4Cons *reflectorSolid = new G4Cons("solidReflector",
                                      reflectorR1,
                                      reflectorR1+reflectorThickness,
                                      reflectorR2,
                                      reflectorR2+reflectorThickness,
                                      reflectorHeight/2.,
                                      0.*deg, 360.*deg);

  G4LogicalVolume *reflectorLogic = new G4LogicalVolume(reflectorSolid,
                                                        G4Material::GetMaterial("Aluminum"), 
                                                        "logicReflector");
  
  new G4PVPlacement(0,
                    G4ThreeVector(0.,0.,39.9*mm+zoffset),
                    reflectorLogic,                                                         
                    "Reflector",                                                       
                    logicWCPMT,                                          
                    false,                                               
                    0,                                               
                    checkOverlaps); 

  new G4LogicalSkinSurface("ReflectorCupSkinSurface",reflectorLogic,ReflectorSkinSurfaceWCTE);

  G4VisAttributes *refAttributes = new G4VisAttributes();
  refAttributes->SetColor(0.85, 0.85, 0.85, 1.0);
  refAttributes->SetVisibility(true);
  refAttributes->SetForceSolid(true);
  reflectorLogic->SetVisAttributes(refAttributes);  

  /////////////////////////
  // Sensitive detector ///
  /////////////////////////

  // Instantiate a new sensitive detector
  // and register this sensitive detector volume with the SD Manager.
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  G4String SDName = "/WCSim/";
  SDName += CollectionName;

  // If there is no such sensitive detector with that SDName yet,
  // make a new one
  if( ! SDman->FindSensitiveDetector(SDName, false) ) {

    aWCPMT = new WCSimWCSD(CollectionName,SDName,this,detectorElement);
    SDman->AddNewDetector( aWCPMT );
  }

  logicGlassFaceWCPMT->SetSensitiveDetector( aWCPMT );


  PMTLogicalVolumes[key] = logicWCPMT;

  return logicWCPMT;
}

// mPMT construction
G4LogicalVolume* WCSimDetectorConstruction::ConstructInSituMultiPMT(G4String PMTName, G4String CollectionName, G4String detectorElement)
{

  G4bool showme = true;

  //unique key for mPMT object. 
  G4String keyname =  mPMT_ID_PMT + "_InSitu-mPMT";
  PMTKey_t key(keyname,CollectionName);
  
  // Return pre-created mPMT Logical Volume if it already exists.
  PMTMap_t::iterator it = PMTLogicalVolumes.find(key);
  if (it != PMTLogicalVolumes.end()) {
    return it->second;
  }

  G4cout << "========================================================" << G4endl;
  G4cout<<"In-situ mPMT: Collection Name = "<<CollectionName<<G4endl;
  G4cout << "========================================================" << G4endl;

  G4double domeInnerRadius = 332.*mm;
  G4double domeOuterRadius = 347.*mm;
  G4double domeCut = 235*mm;

  G4double vessel_cylinder_height = 77.785*2*mm;
  G4double vessel_inner_radius = 250.93*mm;
  G4double vessel_outer_radius = 254.*mm;
  G4double dome_height = domeOuterRadius - domeCut;

  // G4double offsetFromBox = 168.97*mm; // offset from original code where world volume is a box

  G4int nIDPMTs = 19;
  
  G4cout << "Create ex-situ mPMT" <<  G4endl;
  G4cout << "Vessel height: " << vessel_cylinder_height << G4endl;
  G4cout << "Vessel radius: " << vessel_outer_radius << G4endl;
  G4cout << "Dome radius curv: " << domeOuterRadius << G4endl;
  G4cout << "Dome height: " << dome_height << G4endl;
  G4cout << "Number of ID PMTs: " << nIDPMTs << G4endl;

  //All components of the PMT are now contained in a single logical volume logicWCPMT.
  //Origin is cylinder bottom, faces positive z-direction.

  ////////////////////////////////////////////////////
  /// 1) Outer logical volume: fill with air     ///
  ////////////////////////////////////////////////////
  G4double mPMT_zRange_outer[2] = {0,                         // start from zero for easier placement in ID
				                           vessel_cylinder_height};
  G4double mPMT_RRange_outer[2] = {vessel_outer_radius, 
				                           vessel_outer_radius};
  G4double mPMT_rRange_outer[2] = {0., 0.};

  // Although G4Tubs is more natural, Polycone is used to be in control of z position
  // and because z = 0 is position of bottom of cylinder, vs center in G4Tubs
  G4Polycone* solidMultiPMT_vessel = 
    new G4Polycone("WCmPMT_vessel",                    
		   0.0*deg,
		   360.0*deg,
		   2,
		   mPMT_zRange_outer,
		   mPMT_rRange_outer, // R Inner
		   mPMT_RRange_outer);// R Outer
  
  G4Sphere* mPMT_top_sphere =
    new G4Sphere(    "WCmPMT_tsphere",
		     0,
		     domeOuterRadius,
		     0.0*deg,360.0*deg,
		     0.0*deg,90.0*deg);
  
  G4Box*  solidCutOffTubs = 
    new G4Box(    "cutOffTubs",
      domeOuterRadius + 1.*cm,                      // x/2
      domeOuterRadius + 1.*cm,                      // y/2
      domeCut);         // z/2

  G4SubtractionSolid * mPMT_top_cap_vessel = new G4SubtractionSolid("WCmPMT_tsphere_vessel",
                                                                    mPMT_top_sphere,
                                                                    solidCutOffTubs);

  G4UnionSolid *solidMultiPMT = 
	  new G4UnionSolid("WCmPMT",solidMultiPMT_vessel,mPMT_top_cap_vessel,0,G4ThreeVector(0,0,vessel_cylinder_height-domeCut));

  G4LogicalVolume *logicWCMultiPMT =
    new G4LogicalVolume(    solidMultiPMT,
			    G4Material::GetMaterial("Air1"), 
			    "WCMultiPMT",
			    0,0,0);



  ////////////////////////////////////////////////
  /// 2)Top acrylic + shell of the mPMT vessel ///
  ////////////////////////////////////////////////
  G4Sphere *domeSphere = 
  new G4Sphere("DomeSphere",
                domeInnerRadius,
                domeOuterRadius,
                0.0*deg, 360.0*deg,
                0.0, 90.*deg);

  G4VSolid *domeSolid = new G4SubtractionSolid("domeSolid",
                                               domeSphere,
                                               solidCutOffTubs);

  G4LogicalVolume *domeLogic = new G4LogicalVolume(domeSolid,
                                                   G4Material::GetMaterial("G4_PLEXIGLASS"),
                                                   "logicDome");
  
  new G4PVPlacement(0,
                    G4ThreeVector(0.,0.,vessel_cylinder_height-domeCut),
                    domeLogic,
                    "physInsituDome",
                    logicWCMultiPMT,
                    false,
                    0,
                    checkOverlaps);

  G4VisAttributes *domeAttributes = new G4VisAttributes();
  domeAttributes->SetColor(1.0, 1.0, 1.0, 0.5);
  domeAttributes->SetVisibility(true);
  domeAttributes->SetForceSolid(true);
  domeLogic->SetVisAttributes(domeAttributes);

  G4Tubs *cylinderSolid = new G4Tubs("cylinderSolid",
				     vessel_inner_radius,
				     vessel_outer_radius,
				     vessel_cylinder_height/2,
				     0.*deg, 360.*deg);

  G4LogicalVolume *logicCylinder = new G4LogicalVolume( cylinderSolid,
                                                        G4Material::GetMaterial("Plastic"),
                                                        "logicCylinder");
  
  new G4PVPlacement(0,
                    G4ThreeVector(0.,0.,vessel_cylinder_height/2),
                    logicCylinder,
                    "physInsituCylinder",
                    logicWCMultiPMT,
                    false,
                    0,
                    checkOverlaps);

  new G4LogicalSkinSurface("cylinderSkinSurface", logicCylinder, BSSkinSurface); // assume same as blacksheet
      
  G4VisAttributes *cylinderAttributes = new G4VisAttributes();  
  cylinderAttributes->SetColor(0.2, 0.2, 0.2, 1.0);  
  cylinderAttributes->SetVisibility(true); 
  cylinderAttributes->SetForceSolid(true);  
  logicCylinder->SetVisAttributes(cylinderAttributes);

  /////////////////////////////////////////////////////////////////////////
  /// 3) This is the area between the outer shell and the inner shell.  ///
  ///    In this space, the PMTs will live.                             ///
  /////////////////////////////////////////////////////////////////////////

  // Matrix, where PMTs are placed as daughters
  G4double matrixRmin = 263.003*mm;
  G4double matrixRmax = 325.603*mm;
  G4double matrixUpshift = 5.7*mm;
  G4Sphere *solidMatrix = 
  new G4Sphere("solidMatrix",
				       matrixRmin,
				       matrixRmax,
				       0.0*deg, 360.0*deg,
				       0.0, 43.4549*deg);
  

  G4LogicalVolume *logicMatrix = new G4LogicalVolume( solidMatrix,
                                                      G4Material::GetMaterial("Plastic"),
                                                      "logicMatrix");
  // delay the matrix placement 

  new G4LogicalSkinSurface("matrixSkinSurface", logicMatrix, BSSkinSurface); // assume same as blacksheet

  // Gel
  G4double gelRmin = 323.5*mm;
  G4Sphere *solidGelSphere = new G4Sphere("solidGelSphere",
                                          gelRmin,
                                          domeInnerRadius,
                                          0.0*deg, 360.0*deg,
                                          0.0, 90.0*deg);

  G4Box *solidGelCutOut = new G4Box("BoxGelCutOut",
                                    domeInnerRadius+1.*cm,
                                    domeInnerRadius+1.*cm,
                                    241.04*mm);

  G4VSolid *solidGelSphereCut = 
  new G4SubtractionSolid("solidGelSphereCut",
                          solidGelSphere,
                          solidGelCutOut);

  G4VSolid *solidGel = new G4SubtractionSolid("solidGel",
                                              solidGelSphereCut,
                                              solidMatrix,
                                              0,
                                              G4ThreeVector(0.,0.,gelRmin-matrixRmax+matrixUpshift));

  G4LogicalVolume *logicGel = new G4LogicalVolume(solidGel,
                                                  G4Material::GetMaterial("SilGel_WCTE"),
                                                  "logicGel");

  new G4PVPlacement(0,
						        G4ThreeVector(0.,0.,vessel_cylinder_height-domeCut),
                    logicGel,
                    "Gel",
                    logicWCMultiPMT,
                    false,
                    0,
                    checkOverlaps);

  G4VisAttributes *gelAttributes = new G4VisAttributes();
  gelAttributes->SetColor(1.0, 1.0, 1.0, 0.5);
  gelAttributes->SetVisibility(true);
  gelAttributes->SetForceSolid(true);
  logicGel->SetVisAttributes(gelAttributes);

  // Seal the bottom with blacksheet
  G4Tubs *solidBS = new G4Tubs("solidBS",
                                0*mm,
                                vessel_inner_radius,
                                1.*mm,
                                0.*deg, 360.*deg);

  G4LogicalVolume *logicBS = new G4LogicalVolume(solidBS,
						                                     G4Material::GetMaterial("Blacksheet"),
                                                 "logicBS");

  new G4PVPlacement(0,
                    G4ThreeVector(0.,0.,1.*mm),
                    logicBS,
                    "physInsituMPMTBS",
                    logicWCMultiPMT,
                    false,
                    0,
                    checkOverlaps);

  new G4LogicalSkinSurface("insituMPMTBSSkinSurface", logicBS, BSSkinSurface); 

  G4VisAttributes* VisAttRed = new G4VisAttributes(G4Colour(1.0,0.,0.));
  VisAttRed->SetForceSolid(true); 
  logicBS->SetVisAttributes(VisAttRed);

  /////////////////////////////////////////////////////
  /// 4) Fill the mPMT matrix with single (ID) PMTs ///
  /////////////////////////////////////////////////////
  
  G4LogicalVolume* logicWCPMT = ConstructInSituPMT(PMTName, CollectionName,detectorElement);
  
  G4double thetaArray[19] =  {0.*deg, 18.1345*deg, 18.1345*deg, 18.1345*deg, 18.1345*deg, 18.1345*deg, 18.1345*deg, 
                              35.3149*deg, 35.3149*deg, 35.3149*deg, 35.3149*deg, 35.3149*deg, 35.3149*deg, 
                              35.3149*deg, 35.3149*deg, 35.3149*deg, 35.3149*deg, 35.3149*deg, 35.3149*deg };
  G4double phiArray[19] = {0.*deg, 0.*deg, 60.*deg, 120.*deg, 180.*deg, -120.*deg, -60*deg, 0.*deg, 30.*deg, 60.*deg, 90.*deg, 120.*deg, 150.*deg, 180.*deg, -150.*deg, -120.*deg, -90.*deg, -60.*deg, -30.*deg};

  // Place the PMTs inside matrix
  for (int i = 0; i < nIDPMTs; i++) {

    G4ThreeVector PMTPosition = {0,0,0};

    G4RotationMatrix* PMTRotation = new G4RotationMatrix;
    PMTRotation->rotateZ(-phiArray[i]);
    PMTRotation->rotateY(-thetaArray[i]);
    
    // Create and place the PMT copy                                                                                    
    new G4PVPlacement(PMTRotation,
                      PMTPosition,
                      logicWCPMT,
                      "pmt",
                      logicMatrix,
                      false,
                      i,
                      checkOverlaps);
    
  }  
 
  // Finally place the matrix
  new G4PVPlacement(0,
                    G4ThreeVector(0.,0., vessel_cylinder_height-domeCut+gelRmin-matrixRmax+matrixUpshift),
                    logicMatrix,
                    "physMatrix",
                    logicWCMultiPMT,
                    false,
                    0,
                    checkOverlaps);  

  /* Set all visualization here for better overview. */
  // Gray wireframe visual style
  G4VisAttributes* VisAttGrey = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
  //VisAttGrey->SetForceWireframe(true);
  VisAttGrey->SetForceSolid(true); 
  G4VisAttributes* WCPMTVisAtt5 = new G4VisAttributes(G4Colour(.0,1.,1.));
  WCPMTVisAtt5->SetForceSolid(true); 
  G4VisAttributes* VisAttYellow = new G4VisAttributes(G4Colour(1.0,1.,0.));
  VisAttYellow->SetForceSolid(true); 
  
  if(showme){
    logicWCMultiPMT->SetVisAttributes(VisAttGrey); 
    logicCylinder->SetVisAttributes(WCPMTVisAtt5);    
  }

  // Keep track of already created mPMT logical volumes in same map as for PMTs
  PMTLogicalVolumes[key] = logicWCMultiPMT;

  return logicWCMultiPMT;
}
