//  -*- mode:c++; tab-width:4;  -*-
#include "WCSimDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4IntersectionSolid.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4PVReplica.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4AssemblyVolume.hh"
#include "G4SubtractionSolid.hh"
#include "globals.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpticalSurface.hh"
#include "G4UserLimits.hh"
#include "G4ReflectionFactory.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"

#include "WCSimTuningParameters.hh" //jl145

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "G4VSolid.hh" // following 4 .hh required for CADMesh.
#include "G4Trd.hh"
#include "G4NistManager.hh"
#include "CADMesh.hh"


//#define WCSIMCONSTRUCTCYLINDER_VERBOSE
#define DEBUG
//#define MIRROR_WCSIM_DEVELOP_SKIN
/***********************************************************
 *
 * This file contains the functions which construct a 
 * cylindrical WC detector.  It used by both the SK and 
 * LBNE WC detector modes.  It is called in the Construct()
 * method in WCSimDetectorConstruction.cc.
 *
 * Sourcefile for the WCSimDetectorConstruction class
 *
 ***********************************************************/

//-----------------------------------------------------
// Defining colors used for debugging geom
//-----------------------------------------------------

G4Colour  white   (1.0, 1.0, 1.0) ;  // white
G4Colour  black   (0.0, 0.0, 0.0) ;  // black
G4Colour  red     (1.0, 0.0, 0.0) ;  // red
G4Colour  green   (0.0, 1.0, 0.0) ;  // green
G4Colour  blue    (0.0, 0.0, 1.0) ;  // blue
G4Colour  cyan    (0.0, 1.0, 1.0) ;  // cyan
G4Colour  magenta (1.0, 0.0, 1.0) ;  // magenta
G4Colour  yellow  (1.0, 1.0, 0.0) ;  // yellow

#ifndef ACTIVATE_IDPMTS
#define ACTIVATE_IDPMTS
#endif

G4LogicalVolume* WCSimDetectorConstruction::ConstructCylinder()
{
  G4cout << "**** Building Cylindrical Detector ****" << G4endl;

  //-----------------------------------------------------
  // Positions
  //-----------------------------------------------------

  debugMode = false;
  
  WCPosition=0.;//Set the WC tube offset to zero

  WCIDRadius = WCIDDiameter/2.;
  // the number of regular cell in phi direction:
  WCBarrelRingNPhi     = WCBarrelNumPMTHorizontal/WCPMTperCellHorizontal; 
  // the part of one ring, that is covered by the regular cells: 
  totalAngle  = 2.0*pi*rad*WCBarrelRingNPhi*WCPMTperCellHorizontal/WCBarrelNumPMTHorizontal;
  // angle per regular cell:
  dPhi        =  totalAngle/ WCBarrelRingNPhi;

  if(rotateBarrelHalfTower)
    barrelPhiOffset = -dPhi/2; // For WCTE, the barrel is rotated by half a tower for correct alignment with endcaps
  else
    barrelPhiOffset = 0.;

  // it's height:
  barrelCellHeight  = (WCIDHeight-2.*WCBarrelPMTOffset)/WCBarrelNRings;
  // the height of all regular cells together:
  mainAnnulusHeight = WCIDHeight -2.*WCBarrelPMTOffset -2.*barrelCellHeight;
  
  //TF: has to change for mPMT vessel:

  G4cout << WCIDRadius << ", " << WCPMTExposeHeight << ", " << mPMT_vessel_cyl_height << ", " << mPMT_vessel_radius << G4endl;
#ifdef DEBUG
  G4cout << "HYBRID = " << hybrid << G4endl;
#endif
  
  if(hybrid){
	if(mPMT_vessel_cyl_height + mPMT_vessel_radius < 1.*mm)
	  innerAnnulusRadius = WCIDRadius - std::max(WCPMTExposeHeight,WCPMTExposeHeight2)-1.*mm;
	else
	  innerAnnulusRadius = WCIDRadius - std::max(WCPMTExposeHeight,(mPMT_vessel_cyl_height + mPMT_vessel_radius)) -1.*mm;
  }
  else{
	if(mPMT_vessel_cyl_height + mPMT_vessel_radius < 1.*mm)
	  innerAnnulusRadius = WCIDRadius - WCPMTExposeHeight -1.*mm;
	else
	  innerAnnulusRadius = WCIDRadius - (mPMT_vessel_cyl_height + mPMT_vessel_radius) -1.*mm;
  }

  //TF: need to add a Polyhedra on the other side of the outerAnnulusRadius for the OD
  outerAnnulusRadius = WCIDRadius + WCBlackSheetThickness + 1.*mm;//+ Stealstructure etc.
  WCODRadius = WCIDRadius + WCBlackSheetThickness;
  if(isODConstructed){
    const G4double sphereRadius =
	  (WCPMTODExposeHeight*WCPMTODExposeHeight+ WCPMTODRadius*WCPMTODRadius)/(2*WCPMTODExposeHeight);
    WCODRadius = 
	  WCIDRadius + WCBlackSheetThickness + WCODDeadSpace + // ID Structure
	  WCODTyvekSheetThickness;  // Tyvek attached to structure
    outerAnnulusRadius = WCODRadius + sphereRadius;
  }
#ifdef WCSIMCONSTRUCTCYLINDER_VERBOSE
  G4cout
	<< "GEOMCHECK3 WCIDRadius \t" << WCIDRadius << G4endl
	<< "GEOMCHECK3 WCBlackSheetThickness \t" << WCBlackSheetThickness << G4endl
	<< "GEOMCHECK3 WCODDeadSpace \t" << WCODDeadSpace << G4endl
	<< "GEOMCHECK3 WCODTyvekSheetThickness \t" << WCODTyvekSheetThickness << G4endl
	<< "GEOMCHECK3 sphereRadius \t" << outerAnnulusRadius - WCODRadius << G4endl
	<< "GEOMCHECK3 WCPMTODExposeHeight \t" << WCPMTODExposeHeight << G4endl
	<< "GEOMCHECK3 WCPMTODExposeHeight \t" << WCPMTODExposeHeight << G4endl
	<< "GEOMCHECK3 WCPMTODRadius \t" << WCPMTODRadius << G4endl
	<< "GEOMCHECK3 WCPMTODRadius \t" << WCPMTODRadius << G4endl
	<< "GEOMCHECK3 WCPMTODExposeHeight \t" << WCPMTODExposeHeight << G4endl
	;
#endif

  // the radii are measured to the center of the surfaces
  // (tangent distance). Thus distances between the corner and the center are bigger.
  //BQ: Updated with new HK OD size (2020/12/06). Simply assume no tyvek thickness or dead space.
  WCLength    = WCIDHeight + 2*(WCODHeightWaterDepth + WCBlackSheetThickness + WCODDeadSpace + WCODTyvekSheetThickness + 1*mm);
  WCRadius    = (outerAnnulusRadius + WCODLateralWaterDepth)/cos(dPhi/2.) ;
#ifdef WCSIMCONSTRUCTCYLINDER_VERBOSE
  G4cout
	<< "GEOMCHECK2 WCLength \t" << WCLength << G4endl
	<< "GEOMCHECK2 WCIDHeight \t" << WCIDHeight << G4endl
	<< "GEOMCHECK2 WCODHeightWaterDepth \t" << WCODHeightWaterDepth << G4endl
	<< "GEOMCHECK2 WCBlackSheetThickness \t" << WCBlackSheetThickness << G4endl
	<< "GEOMCHECK2 WCODDeadSpace \t" << WCODDeadSpace << G4endl
	<< "GEOMCHECK2 WCODTyvekSheetThickness \t" << WCODTyvekSheetThickness << G4endl
	<< "GEOMCHECK2 WCRadius \t" << WCRadius << G4endl
	<< "GEOMCHECK2 outerAnnulusRadius \t" << outerAnnulusRadius << G4endl
	<< "GEOMCHECK2 WCODLateralWaterDepth \t" << WCODLateralWaterDepth << G4endl
	;
#endif
  //WCLength    = WCIDHeight + (WCODHeight+WCOuterStructure)*2;	//jl145 - reflects top veto blueprint, cf. Farshid Feyzi
  //WCRadius    = (WCIDDiameter/2. + WCBlackSheetThickness + (WCODRadius+WCODHeight)))/cos(dPhi/2.) ; // BQ: Updated with new HK OD size (2020/12/06) 
  //WCLength    = WCIDHeight + 2*2.3*m;	//jl145 - reflects top veto blueprint, cf. Farshid Feyzi
  //WCRadius    = (WCIDDiameter/2. + WCBlackSheetThickness + 1.5*m)/cos(dPhi/2.) ; // TODO: OD

  // now we know the extend of the detector and are able to tune the tolerance
  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(WCLength > WCRadius ? WCLength : WCRadius);
  G4cout << "Computed tolerance = "
         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
         << " mm" << G4endl;

  //Decide if adding Gd
  water = "Water";
  if (WCAddGd)
	{
	  water = "Doped Water";
	  if(!G4Material::GetMaterial("Doped Water", false)) AddDopedWater();
	}

  // Optionally place parts of the detector. Very useful for visualization and debugging 
  // geometry overlaps in detail.
  placeBarrelPMTs = true;
  placeCapPMTs = true;
  placeBorderPMTs = true; 


  //-----------------------------------------------------
  // Volumes
  //-----------------------------------------------------
  // The water barrel is placed in an tubs of air
  
  G4Tubs* solidWC = new G4Tubs("WC",
							   0.0*m,
							   WCRadius+2.*m,
							   .5*WCLength+4.2*m,	//jl145 - per blueprint
							   0.*deg,
							   360.*deg);
  
  G4LogicalVolume* logicWC = 
    new G4LogicalVolume(solidWC,
						G4Material::GetMaterial("Air"),
						"WC",
						0,0,0);
 
 
  G4VisAttributes* showColor = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
  logicWC->SetVisAttributes(showColor);

  logicWC->SetVisAttributes(G4VisAttributes::Invisible); //amb79
  
  //-----------------------------------------------------
  // everything else is contained in this water tubs
  //-----------------------------------------------------
  G4Tubs* solidWCBarrel = new G4Tubs("WCBarrel",
									 0.0*m,
									 WCRadius+1.*m, // add a bit of extra space
									 .5*WCLength,  //jl145 - per blueprint
									 0.*deg,
									 360.*deg);

  //  G4cout << " qqqqqqqqqqqqqqqqqqqq " << " WCRadius " << WCRadius << " WCBarrel radius " << WCRadius+1.*m << " half height "  << .5*WCLength << G4endl;
  
  G4LogicalVolume* logicWCBarrel = 
    new G4LogicalVolume(solidWCBarrel,
						G4Material::GetMaterial(water),
						"WCBarrel",
						0,0,0);

  //G4VPhysicalVolume* physiWCBarrel = 
    new G4PVPlacement(0,
					  G4ThreeVector(0.,0.,0.),
					  logicWCBarrel,
					  "WCBarrel",
					  logicWC,
					  false,
					  0,
					  checkOverlaps); 

  if(isODConstructed) {
    //-----------------------------------------------------
    // Cylinder wall's tyvek
    //-----------------------------------------------------

    G4Tubs *solidCaveTyvek = new G4Tubs("WC",
                                        WCRadius,
                                        WCRadius + WCODTyvekSheetThickness,
                                        .5 * WCLength,  //jl145 - per blueprint
                                        0. * deg,
                                        360. * deg);

    G4LogicalVolume *logicCaveTyvek =
	  new G4LogicalVolume(solidCaveTyvek,
						  G4Material::GetMaterial("Tyvek"),
						  "CaveTyvek",
						  0, 0, 0);

    //G4VPhysicalVolume *physiCaveTyvek =
	new G4PVPlacement(0,
					  G4ThreeVector(0., 0., 0.),
					  logicCaveTyvek,
					  "CaveBarrelTyvek",
					  logicWCBarrel,
					  false,
					  checkOverlaps);

	//G4LogicalSkinSurface *TyvekCaveBarrelSurface =
	new G4LogicalSkinSurface("TyvekCaveBarrelSurface", logicCaveTyvek, OpWaterTySurface);

    G4VisAttributes *showTyvekCave = new G4VisAttributes(green);
    showTyvekCave->SetForceWireframe(true);// This line is used to give definition to the rings in OGLSX Visualizer
    logicCaveTyvek->SetVisAttributes(showTyvekCave);
    //logicCaveTyvek->SetVisAttributes(G4VisAttributes::Invisible); //amb79

    //-----------------------------------------------------
    // Cylinder caps' tyvek
    //-----------------------------------------------------

    G4Tubs *solidCaveCapsTyvek = new G4Tubs("CaveCapsTyvek",
                                            0,
                                            WCRadius,
                                            .5 * (WCODTyvekSheetThickness),
                                            0. * deg,
                                            360. * deg);

    G4LogicalVolume *logicCaveCapsTyvek =
	  new G4LogicalVolume(solidCaveCapsTyvek,
						  G4Material::GetMaterial("Tyvek"),
						  "CaveCapTyvek",
						  0, 0, 0);

    //G4LogicalSkinSurface *TyvekCaveTopSurface =
	new G4LogicalSkinSurface("TyvekCaveTopSurface", logicCaveCapsTyvek, OpWaterTySurface);

    G4VisAttributes *CapsCaveTyvekVisAtt = new G4VisAttributes(yellow);
    CapsCaveTyvekVisAtt->SetForceWireframe(true);
    logicCaveCapsTyvek->SetVisAttributes(CapsCaveTyvekVisAtt);
    //logicCaveCapsTyvek->SetVisAttributes(G4VisAttributes::Invisible); //amb79

    G4ThreeVector CaveTyvekPosition(0., 0., WCLength / 2);

    //G4VPhysicalVolume *physiTopCaveTyvek =
	new G4PVPlacement(0,
					  CaveTyvekPosition,
					  logicCaveCapsTyvek,
					  "CaveTopTyvek",
					  logicWCBarrel,
					  false,
					  checkOverlaps);


    CaveTyvekPosition.setZ(-CaveTyvekPosition.getZ());

    //G4VPhysicalVolume *physiBottomCaveTyvek =
	new G4PVPlacement(0,
					  CaveTyvekPosition,
					  logicCaveCapsTyvek,
					  "CaveBottomTyvek",
					  logicWCBarrel,
					  false,
					  checkOverlaps);


  } // END Tyvek cave
  //-----------------------------------------------------

  // This volume needs to made invisible to view the blacksheet and PMTs with RayTracer
  if (Vis_Choice == "RayTracer")
	{logicWCBarrel->SetVisAttributes(G4VisAttributes::Invisible);} 

  else
	{//{if(!debugMode)
	  G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4VisAttributes::Invisible);
	  tmpVisAtt->SetForceWireframe(true);// This line is used to give definition to the rings in OGLSX Visualizer
	  logicWCBarrel->SetVisAttributes(tmpVisAtt);
	}
	
  //----------------- example CADMesh -----------------------
  //                 L.Anthony 23/06/2022 
  //---------------------------------------------------------
  /*
  auto shape_bunny = CADMesh::TessellatedMesh::FromSTL("/path/to/WCSim/data/bunny.stl");
  
  // set scale
  shape_bunny->SetScale(1);
  G4ThreeVector posBunny = G4ThreeVector(0*m, 0*m, 0*m);
  
  // make new shape a solid
  G4VSolid* solid_bunny = shape_bunny->GetSolid();
  
  G4LogicalVolume* bunny_logical =                                   //logic name
	new G4LogicalVolume(solid_bunny,                                 //solid name
						G4Material::GetMaterial("StainlessSteel"),          //material
						"bunny");                                     //objects name
  // rotate if necessary
  G4RotationMatrix* bunny_rot = new G4RotationMatrix; // Rotates X and Z axes only
  bunny_rot->rotateX(270*deg);                 
  bunny_rot->rotateY(45*deg);
  
  G4VPhysicalVolume* physiBunny =
	new G4PVPlacement(bunny_rot,                       //no rotation
					  posBunny,                    //at position
					  bunny_logical,           //its logical volume
					  "bunny",                //its name
					  logicWCBarrel,                //its mother  volume
					  false,                   //no boolean operation
					  0);                       //copy number
  //                      checkOverlaps);          //overlaps checking
  
  new G4LogicalSkinSurface("BunnySurface",bunny_logical,ReflectorSkinSurface);
  */
	
  //-----------------------------------------------------
  // Form annular section of barrel to hold PMTs 
  //----------------------------------------------------

 
  G4double mainAnnulusZ[2] = {-mainAnnulusHeight/2., mainAnnulusHeight/2};
  G4double mainAnnulusRmin[2] = {innerAnnulusRadius, innerAnnulusRadius};
  G4double mainAnnulusRmax[2] = {outerAnnulusRadius, outerAnnulusRadius};

  G4Polyhedra* solidWCBarrelAnnulus = new G4Polyhedra("WCBarrelAnnulus",
													  barrelPhiOffset, // phi start
													  totalAngle, 
													  WCBarrelRingNPhi, //NPhi-gon
													  2,
													  mainAnnulusZ,
													  mainAnnulusRmin,
													  mainAnnulusRmax);
  
  G4LogicalVolume* logicWCBarrelAnnulus = 
    new G4LogicalVolume(solidWCBarrelAnnulus,
						G4Material::GetMaterial(water),
						"WCBarrelAnnulus",
						0,0,0);
  // G4cout << *solidWCBarrelAnnulus << G4endl; 
  //G4VPhysicalVolume* physiWCBarrelAnnulus = 
    new G4PVPlacement(0,
					  G4ThreeVector(0.,0.,0.),
					  logicWCBarrelAnnulus,
					  "WCBarrelAnnulus",
					  logicWCBarrel,
					  false,
					  0,
					  checkOverlaps);
  if(!debugMode)
	logicWCBarrelAnnulus->SetVisAttributes(G4VisAttributes::Invisible); //amb79

  //-----------------------------------------------------
  // Subdivide the BarrelAnnulus into rings
  //-----------------------------------------------------
  G4double RingZ[2] = {-barrelCellHeight/2.,
					   barrelCellHeight/2.};

  G4Polyhedra* solidWCBarrelRing = new G4Polyhedra("WCBarrelRing",
                                                   barrelPhiOffset,//+dPhi/2., // phi start
                                                   totalAngle, //phi end
                                                   WCBarrelRingNPhi, //NPhi-gon
                                                   2,
                                                   RingZ,
                                                   mainAnnulusRmin,
                                                   mainAnnulusRmax);

  G4LogicalVolume* logicWCBarrelRing = 
    new G4LogicalVolume(solidWCBarrelRing,
						G4Material::GetMaterial(water),
						"WCBarrelRing",
						0,0,0);

  //G4VPhysicalVolume* physiWCBarrelRing = 
    new G4PVReplica("WCBarrelRing",
					logicWCBarrelRing,
					logicWCBarrelAnnulus,
					kZAxis,
					WCBarrelNRings-2,
					barrelCellHeight);

  if(!debugMode)
	{G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(0,0.5,1.));
	  tmpVisAtt->SetForceWireframe(true);// This line is used to give definition to the rings in OGLSX Visualizer
	  logicWCBarrelRing->SetVisAttributes(tmpVisAtt);
	  //If you want the rings on the Annulus to show in OGLSX, then comment out the line below.
	  logicWCBarrelRing->SetVisAttributes(G4VisAttributes::Invisible);
	}
  else {
	G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(0,0.5,1.));
	tmpVisAtt->SetForceWireframe(true);
	logicWCBarrelRing->SetVisAttributes(tmpVisAtt);
  }

  //-----------------------------------------------------
  // Subdivisions of the BarrelRings are cells
  //------------------------------------------------------


  G4Polyhedra* solidWCBarrelCell = new G4Polyhedra("WCBarrelCell",
                                                   -dPhi/2.+0.*deg, // phi start
                                                   dPhi, //total Phi
                                                   1, //NPhi-gon
                                                   2,
                                                   RingZ,
                                                   mainAnnulusRmin,
                                                   mainAnnulusRmax); 
  //G4cout << *solidWCBarrelCell << G4endl; 
  G4LogicalVolume* logicWCBarrelCell = 
    new G4LogicalVolume(solidWCBarrelCell,
						G4Material::GetMaterial(water),
						"WCBarrelCell",
						0,0,0);

  G4VPhysicalVolume* physiWCBarrelCell = 
    new G4PVReplica("WCBarrelCell",
					logicWCBarrelCell,
					logicWCBarrelRing,
					kPhi,
					WCBarrelRingNPhi,
					dPhi,
          barrelPhiOffset); 

  if(!debugMode)
  	{G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
	  tmpVisAtt->SetForceSolid(true);// This line is used to give definition to the cells in OGLSX Visualizer
	  logicWCBarrelCell->SetVisAttributes(tmpVisAtt); 
	  //If you want the columns on the Annulus to show in OGLSX, then comment out the line below.
	  logicWCBarrelCell->SetVisAttributes(G4VisAttributes::Invisible);
	}
  else {
  	G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
  	tmpVisAtt->SetForceWireframe(true);
  	logicWCBarrelCell->SetVisAttributes(tmpVisAtt);
  }

  //-----------------------------------------------------------
  // The Blacksheet, a daughter of the cells containing PMTs,
  // and also some other volumes to make the edges light tight
  //-----------------------------------------------------------

  //-------------------------------------------------------------
  // add barrel blacksheet to the normal barrel cells 
  // ------------------------------------------------------------
  G4double annulusBlackSheetRmax[2] = {(WCIDRadius+WCBlackSheetThickness),
									   WCIDRadius+WCBlackSheetThickness};
  G4double annulusBlackSheetRmin[2] = {(WCIDRadius),
									   WCIDRadius};
  G4cout << WCIDRadius << ", " << WCBlackSheetThickness << ", " << barrelCellHeight/2. << G4endl;
  G4cout << WCIDHeight << ", " << WCBarrelPMTOffset << ", " << WCBarrelNRings << G4endl;
  G4cout << dPhi << ", " << RingZ << G4endl;
  G4cout << mainAnnulusRmin[0] << ", " << mainAnnulusRmax[0] << ", " << mainAnnulusZ[0] << G4endl;
  G4cout << mainAnnulusRmin[1] << ", " << mainAnnulusRmax[1] << ", " << mainAnnulusZ[1] << G4endl;
  
  G4Polyhedra* solidWCBarrelCellBlackSheet = new G4Polyhedra("WCBarrelCellBlackSheet",
															 -dPhi/2., // phi start
															 dPhi, //total phi
															 1, //NPhi-gon
															 2,
															 RingZ,
															 annulusBlackSheetRmin,
															 annulusBlackSheetRmax);

  G4LogicalVolume* logicWCBarrelCellBlackSheet =
    new G4LogicalVolume(solidWCBarrelCellBlackSheet,
                        G4Material::GetMaterial("Blacksheet"),
                        "WCBarrelCellBlackSheet",
						0,0,0);

  G4VPhysicalVolume* physiWCBarrelCellBlackSheet =
    new G4PVPlacement(0,
                      G4ThreeVector(0.,0.,0.),
                      logicWCBarrelCellBlackSheet,
                      "WCBarrelCellBlackSheet",
                      logicWCBarrelCell,
                      false,
                      0,
                      checkOverlaps);

   
  //G4LogicalBorderSurface * WaterBSBarrelCellSurface =
	  new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
								 physiWCBarrelCell,
								 physiWCBarrelCellBlackSheet, 
								 OpWaterBSSurface);

#ifndef MIRROR_WCSIM_DEVELOP_SKIN
  new G4LogicalSkinSurface("BSBarrelCellSkinSurface",logicWCBarrelCellBlackSheet,
						   BSSkinSurface);
#endif
  
  // Change made here to have the if statement contain the !debugmode to be consistent
  // This code gives the Blacksheet its color. 

  if (Vis_Choice == "RayTracer"){

	G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
      = new G4VisAttributes(G4Colour(0.2,0.9,0.2)); // green color
	WCBarrelBlackSheetCellVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
	WCBarrelBlackSheetCellVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown
	if(!debugMode)
	  logicWCBarrelCellBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
	else
	  logicWCBarrelCellBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);}

  else {

	G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
      = new G4VisAttributes(G4Colour(0.2,0.9,0.2));
	if(!debugMode)
	  logicWCBarrelCellBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);
	else
	  logicWCBarrelCellBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);}

  //-----------------------------------------------------------
  // add extra tower if necessary
  // ---------------------------------------------------------
 
  // we have to declare the logical Volumes 
  // outside of the if block to access it later on 
  G4LogicalVolume* logicWCExtraTowerCell = nullptr;
  if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){
	G4cout << "%%%%% ADDING EXTRA TOWER" << G4endl;

	G4cerr << "The extra tower is not correctly implemented. It causes geometry overlaps," << G4endl
		   << " which lead to killed tracks and the incorrect number of hits." << G4endl
		   << "You are strongly recommended to use $WCSIMDIR/sample-root-scripts/calcPhotoCoverage.C" << G4endl
		   << " in order to produce a photocoverage that will give you a valid geometry." << G4endl
		   << "Also be warned that, due to how PMTs are laid out in WCSim, you should be careful" << G4endl
		   << " when using a new geometry, especially if it is a HK hybrid geometry with" << G4endl
		   << " a low number of of mPMTs." << G4endl
		   << "You should check that all ID PMTs are laid out in a sensible pattern for your needs," << G4endl
		   << " and that the number of hits produced is sensible." << G4endl;

    // as the angles between the corners of the main annulus 
    // and the corners extra tower are different, we need to adjust the 
    // tangent distance the surfaces of the extra tower. Otherwise
    // the corners of the main annulus and the extra tower would 
    // not match. 
    G4double extraTowerRmin[2];
    G4double extraTowerRmax[2];
    for(int i = 0; i < 2 ; i++){
      extraTowerRmin[i] = mainAnnulusRmin[i] != 0 ? mainAnnulusRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
      extraTowerRmax[i] = mainAnnulusRmax[i] != 0 ? mainAnnulusRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
    }
    G4Polyhedra* solidWCExtraTower = new G4Polyhedra("WCextraTower",
													 totalAngle-2.*pi+barrelPhiOffset,//+dPhi/2., // phi start
													 2.*pi -  totalAngle // total angle.
													 -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m),
													 // we need this little Gap between the extra tower and the main annulus
													 // to avoid a shared surface. Without the gap the photons stuck
													 // at this place for mare than 25 steps and the howl simulation
													 // crashes.
													 1, //NPhi-gon
													 2,
													 mainAnnulusZ,
													 extraTowerRmin,
													 extraTowerRmax);

    G4LogicalVolume* logicWCExtraTower = 
      new G4LogicalVolume(solidWCExtraTower,
						  G4Material::GetMaterial(water),
						  "WCExtraTower",
						  0,0,0);
    //G4VPhysicalVolume* physiWCExtraTower = 
	new G4PVPlacement(0,
					  G4ThreeVector(0.,0.,0.),
					  logicWCExtraTower,
					  "WCExtraTower",
					  logicWCBarrel,
					  false,
					  0,
					  checkOverlaps);

 

    logicWCExtraTower->SetVisAttributes(G4VisAttributes::Invisible);
	//-------------------------------------------
	// subdivide the extra tower into cells  
	//------------------------------------------

    G4Polyhedra* solidWCExtraTowerCell = new G4Polyhedra("WCExtraTowerCell",
														 totalAngle-2.*pi+barrelPhiOffset,//+dPhi/2., // phi start
														 2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //phi end
														 1, //NPhi-gon
														 2,
														 RingZ,
														 extraTowerRmin,
														 extraTowerRmax); 
    //G4cout << * solidWCExtraTowerCell << G4endl;
    logicWCExtraTowerCell = 
      new G4LogicalVolume(solidWCExtraTowerCell,
						  G4Material::GetMaterial(water),
						  "WCExtraTowerCell",
						  0,0,0);
    G4VPhysicalVolume* physiWCTowerCell = 
      new G4PVReplica("extraTowerCell",
					  logicWCExtraTowerCell,
					  logicWCExtraTower,
					  kZAxis,
					  WCBarrelNRings-2,
					  barrelCellHeight);
    
    G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
  	tmpVisAtt->SetForceSolid(true);// This line is used to give definition to the cells in OGLSX Visualizer
  	//logicWCExtraTowerCell->SetVisAttributes(tmpVisAtt); 
	logicWCExtraTowerCell->SetVisAttributes(G4VisAttributes::Invisible);
	//TF vis.

    //---------------------------------------------
    // add blacksheet to this cells
    //--------------------------------------------

    G4double towerBSRmin[2];
    G4double towerBSRmax[2];
    for(int i = 0; i < 2; i++){
      towerBSRmin[i] = annulusBlackSheetRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
      towerBSRmax[i] = annulusBlackSheetRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
    }
    G4Polyhedra* solidWCTowerBlackSheet = new G4Polyhedra("WCExtraTowerBlackSheet",
														  totalAngle-2.*pi+barrelPhiOffset,//+dPhi/2., // phi start
														  2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //phi end
														  1, //NPhi-gon
														  2,
														  RingZ,
														  towerBSRmin,
														  towerBSRmax);
    //G4cout << * solidWCTowerBlackSheet << G4endl;
    G4LogicalVolume* logicWCTowerBlackSheet =
      new G4LogicalVolume(solidWCTowerBlackSheet,
						  G4Material::GetMaterial("Blacksheet"),
						  "WCExtraTowerBlackSheet",
						  0,0,0);

	G4VPhysicalVolume* physiWCTowerBlackSheet =
      new G4PVPlacement(0,
						G4ThreeVector(0.,0.,0.),
						logicWCTowerBlackSheet,
						"WCExtraTowerBlackSheet",
						logicWCExtraTowerCell,
						false,
						0,
						checkOverlaps);


	//G4LogicalBorderSurface * WaterBSTowerCellSurface =
	new G4LogicalBorderSurface("WaterBSTowerCellSurface",
							   physiWCTowerCell,
							   physiWCTowerBlackSheet, 
							   OpWaterBSSurface);

	new G4LogicalSkinSurface("BSTowerCellSkinSurface",logicWCTowerBlackSheet,
							 BSSkinSurface);

	// These lines add color to the blacksheet in the extratower. If using RayTracer, comment the first chunk and use the second. The Blacksheet should be green.

	if (Vis_Choice == "OGLSX"){

	  G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
		= new G4VisAttributes(G4Colour(0.2,0.9,0.2)); // green color

	  if(!debugMode)
		{logicWCTowerBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);}
	  else
		{logicWCTowerBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);}}
  
	if (Vis_Choice == "RayTracer"){
   
	  G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
		= new G4VisAttributes(G4Colour(0.2,0.9,0.2)); // green color
	  WCBarrelBlackSheetCellVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
	  WCBarrelBlackSheetCellVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown

	  if(!debugMode)
		logicWCTowerBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
	  else
		logicWCTowerBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);}

	else {

	  G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
		= new G4VisAttributes(G4Colour(0.2,0.9,0.2)); // green color

	  if(!debugMode)
		{logicWCTowerBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);}
	  else
		{logicWCTowerBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);}}
  

  }

  //jl145------------------------------------------------
  // Add top veto volume
  //-----------------------------------------------------

  const G4bool WCTopVeto = (WCSimTuningParams->GetTopVeto());

  G4LogicalVolume* logicWCTopVeto = nullptr;

  if(WCTopVeto){

	G4cout << "%%%%% ADDING TOP VETO" << G4endl;
	G4double WCTyvekThickness = 1.0*mm; //completely made up

	G4VSolid* solidWCTopVeto;
	solidWCTopVeto =
	  new G4Tubs(			"WCTopVeto",
							0.0*m,
							WCIDRadius + WCTyvekThickness,
							0.5*m + WCTyvekThickness,
							0.*deg,
							360.*deg);

	logicWCTopVeto = 
	  new G4LogicalVolume(solidWCTopVeto,
						  G4Material::GetMaterial(water),
						  "WCTopVeto",
						  0,0,0);

	//G4VPhysicalVolume* physiWCTopVeto =
	new G4PVPlacement(	0,
						G4ThreeVector(0.,0.,WCIDHeight/2
									  +1.0*m),
						logicWCTopVeto,
						"WCTopVeto",
						logicWCBarrel,
						false,0,
						checkOverlaps);

	//Add the top veto Tyvek
	//-----------------------------------------------------

	G4VSolid* solidWCTVTyvek;
	solidWCTVTyvek =
	  new G4Tubs(			"WCTVTyvek",
							0.0*m,
							WCIDRadius,
							WCTyvekThickness/2,
							0.*deg,
							360.*deg);


	G4LogicalVolume* logicWCTVTyvek =
	  new G4LogicalVolume(solidWCTVTyvek,
						  G4Material::GetMaterial("Tyvek"),
						  "WCTVTyvek",
						  0,0,0);

	//Bottom
	//G4VPhysicalVolume* physiWCTVTyvekBot =
	new G4PVPlacement(	0,
						G4ThreeVector(0.,0.,-0.5*m
									  -WCTyvekThickness/2),
						logicWCTVTyvek,
						"WCTVTyvekBot",
						logicWCTopVeto,
						false,0,
						checkOverlaps);

	//G4LogicalSkinSurface *WaterTyTVSurfaceBot =
	new G4LogicalSkinSurface("WaterTyTVSurfaceBot", logicWCTVTyvek, OpWaterTySurface);

	//Top
	//G4VPhysicalVolume* physiWCTVTyvekTop =
	new G4PVPlacement(	0,
						G4ThreeVector(0.,0.,0.5*m
									  +WCTyvekThickness/2),
						logicWCTVTyvek,
						"WCTVTyvekTop",
						logicWCTopVeto,
						false,0,
						checkOverlaps);


	//Side
	G4VSolid* solidWCTVTyvekSide;
	solidWCTVTyvekSide =
	  new G4Tubs(			"WCTVTyvekSide",
							WCIDRadius,
							WCIDRadius + WCTyvekThickness,
							0.5*m + WCTyvekThickness,
							0.*deg,
							360.*deg);


	G4LogicalVolume* logicWCTVTyvekSide =
	  new G4LogicalVolume(solidWCTVTyvekSide,
						  G4Material::GetMaterial("Tyvek"),
						  "WCTVTyvekSide",
						  0,0,0);

	//G4VPhysicalVolume* physiWCTVTyvekSide =
	new G4PVPlacement(	0,
						G4ThreeVector(0.,0.,0.),
						logicWCTVTyvekSide,
						"WCTVTyvekSide",
						logicWCTopVeto,
						false,0,
						checkOverlaps);

	//G4LogicalSkinSurface *WaterTyTVSurfaceSurface =
	new G4LogicalSkinSurface("WaterTyTVSurfaceSide", logicWCTVTyvekSide, OpWaterTySurface);
  }//WCTopVeto

  //
  //
  //jl145------------------------------------------------


  //////////// M Fechner : I need to  declare the PMT  here in order to
  // place the PMTs in the truncated cells
  //-----------------------------------------------------
  // The PMT
  //-----------------------------------------------------

  ////////////J Felde: The PMT logical volume is now constructed separately 
  // from any specific detector geometry so that any geometry can use the same definition. 
  // K.Zbiri: The PMT volume and the PMT glass are now put in parallel. 
  // The PMT glass is the sensitive volume in this new configuration.

  // TF: Args are set to properties of the class which is somehow global (see the ConstructDetector.hh)
  //     They are set in the WCSimDetectorConfigs and are property of the PMT.
  G4LogicalVolume* logicWCPMT;
  if(nID_PMTs<=1) logicWCPMT = ConstructPMT(WCPMTName, WCIDCollectionName,"tank",nID_PMTs);
  else logicWCPMT = ConstructMultiPMT(WCPMTName, WCIDCollectionName,"tank",nID_PMTs);
  if(!logicWCPMT){
    G4cerr << "Overlapping PMTs in multiPMT" << G4endl;
    return NULL; 
  }
  G4LogicalVolume* logicWCPMT2 = nullptr;
#ifdef DEBUG
  G4cout << "HYBRID2 = " << hybrid << G4endl;
#endif
  if(hybrid){
	G4cout<<"First type of PMT LV is created. Now creating the LV for the second type of PMT"<<G4endl;
	if(nID_PMTs2<=1) logicWCPMT2 = ConstructPMT(WCPMTName2, WCIDCollectionName2,"tankPMT2",nID_PMTs2);
	else logicWCPMT2 = ConstructMultiPMT(WCPMTName2, WCIDCollectionName2,"tankPMT2",nID_PMTs2);
	if(!logicWCPMT2){
	  G4cerr << "Overlapping PMTs in multiPMT" << G4endl;
	  return NULL; 
	}
  }

  //G4LogicalVolume* logicWCPMT = ConstructPMT(WCPMTName, WCIDCollectionName);
  G4String pmtname = "WCMultiPMT";


  /*These lines of code will give color and volume to the PMTs if it hasn't been set in WCSimConstructPMT.cc.
	I recommend setting them in WCSimConstructPMT.cc. 
	If used here, uncomment the SetVisAttributes(WClogic) line, and comment out the SetVisAttributes(G4VisAttributes::Invisible) line.*/
  
  G4VisAttributes* WClogic 
	= new G4VisAttributes(G4Colour(0.4,0.0,0.8));
  WClogic->SetForceSolid(true);
  WClogic->SetForceAuxEdgeVisible(true);

  //logicWCPMT->SetVisAttributes(WClogic);
  //logicWCPMT->SetVisAttributes(G4VisAttributes::Invisible);

  //jl145------------------------------------------------
  // Add top veto PMTs
  //-----------------------------------------------------

  if(WCTopVeto){

	G4double WCTVPMTSpacing = (WCSimTuningParams->GetTVSpacing())*cm;
	G4double WCTVEdgeLimit = WCCapEdgeLimit;
	G4int TVNCell = WCTVEdgeLimit/WCTVPMTSpacing + 2;

	int icopy = 0;

#ifdef DEBUG
	G4cout<<"Debug B.Q: Constructing the cells for veto"<<G4endl;
#endif
	for ( int i = -TVNCell ; i <  TVNCell; i++) {
	  for (int j = -TVNCell ; j <  TVNCell; j++)   {

		G4cout << "Adding Top Veto PMT in TV cell " << i << ", " << j << G4endl;

		G4double xoffset = i*WCTVPMTSpacing + WCTVPMTSpacing*0.5;
		G4double yoffset = j*WCTVPMTSpacing + WCTVPMTSpacing*0.5;

		G4ThreeVector cellpos =
		  G4ThreeVector(	xoffset, yoffset, -0.5*m);

		if ((sqrt(xoffset*xoffset + yoffset*yoffset) + hybrid?std::max(WCPMTRadius,WCPMTRadius2):WCPMTRadius) < WCTVEdgeLimit) {//B.Q: for hybrid configuration

		  G4int horizontalModulo = (i+TVNCell) % WCPMTperCellHorizontal;
		  G4int verticalModulo = (j+TVNCell) % WCPMTperCellVertical;

		  //G4VPhysicalVolume* physiCapPMT =
		  new G4PVPlacement(	0,						// no rotation
								cellpos,				// its position
								(horizontalModulo == verticalModulo && hybrid && WCPMTPercentCoverage2 !=0)?logicWCPMT2:logicWCPMT,				// its logical volume
								pmtname,//"WCPMT",				// its name 
								logicWCTopVeto,			// its mother volume
								false,					// no boolean os
								icopy,					// every PMT need a unique id.
								checkOverlapsPMT);

		  icopy++;
		}
	  }
	}

	G4double WCTVEfficiency = icopy*WCPMTRadius*(hybrid?std::max(WCPMTRadius,WCPMTRadius2):WCPMTRadius)/((WCIDRadius)*(WCIDRadius));
	G4cout << "Total on top veto: " << icopy << "\n";
	G4cout << "Coverage was calculated to be: " << WCTVEfficiency << "\n";

  }//WCTopVeto

  //
  //
  //jl145------------------------------------------------


  ///////////////   Barrel PMT placement
  G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;

  //for multiPMT, for single PMT need to test defaults
  if(orientation == PERPENDICULAR)
	WCPMTRotation->rotateY(90.*deg); //if mPMT: perp to wall
  else if(orientation == VERTICAL)
	WCPMTRotation->rotateY(0.*deg); //if mPMT: vertical/aligned to wall
  else if(orientation == HORIZONTAL)
	WCPMTRotation->rotateX(90.*deg); //if mPMT: horizontal to wall
  
  


  
  



  G4double barrelCellWidth = 2.*WCIDRadius*tan(dPhi/2.);
  G4double horizontalSpacing   = barrelCellWidth/WCPMTperCellHorizontal;
  G4double verticalSpacing     = barrelCellHeight/WCPMTperCellVertical;

  if(placeBarrelPMTs){

#ifdef DEBUG
  	G4cout << "Debug B.Q : Number of PMT horizontal = " << WCPMTperCellHorizontal << ", number of PMT vertical = " << WCPMTperCellVertical << G4endl;
	G4cout << "Debug B.Q : Horizontal spacing = "<< horizontalSpacing << " vertical spacing = " << verticalSpacing << G4endl;
#endif
	
	for(G4long i = 0; i < WCPMTperCellHorizontal; i++){
	  for(G4long j = 0; j < WCPMTperCellVertical; j++){
#ifdef WCSIMCONSTRUCTCYLINDER_VERBOSE
		G4cout << "Adding barrel PMT in cell " << i << ", " << j << G4endl;
#endif	  
		G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius,
												   -barrelCellWidth/2.+(i+0.5)*horizontalSpacing,
												   -barrelCellHeight/2.+(j+0.5)*verticalSpacing);

#ifdef ACTIVATE_IDPMTS
		//G4VPhysicalVolume* physiWCBarrelPMT =
		new G4PVPlacement(WCPMTRotation,              // its rotation
						  PMTPosition, 
						  (i==j && hybrid && WCPMTPercentCoverage2!=0)?logicWCPMT2:logicWCPMT,                // its logical volume
						  pmtname,//"WCPMT",             // its name
						  logicWCBarrelCell,         // its mother volume
						  false,                     // no boolean operations
						  i*WCPMTperCellVertical+j,
						  checkOverlapsPMT);                       
#endif


		// logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
		// daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
		// this is still the case.
	  }
	}

	//-------------------------------------------------------------
	// Add PMTs in extra Tower if necessary
	//------------------------------------------------------------


	if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){

#ifdef DEBUG
	  G4cout << "Debug B.Q: Wish to place extra PMTs" << G4endl;
	  G4cout << "Debug B.Q : WCBarrelRingNPhi = " << WCBarrelRingNPhi << ", WCBarrelNumPMTHorizontal = " << WCBarrelNumPMTHorizontal << G4endl;
#endif
	
	  G4RotationMatrix* WCExtraPMTRotation = new G4RotationMatrix;
	  WCExtraPMTRotation->rotateY(90.*deg);
	  WCExtraPMTRotation->rotateX((2*pi-totalAngle)/2.-barrelPhiOffset);//align the PMT with the Cell
	  //Stopped here: B.Q                                  
	  G4double towerWidth = WCIDRadius*tan(2*pi-totalAngle);

	  G4double horizontalSpacingExtra   = towerWidth/(WCBarrelNumPMTHorizontal-WCBarrelRingNPhi*WCPMTperCellHorizontal);
	  // verticalSpacing is identical to that for normal cell.

	  for(G4long i = 0; i < (WCBarrelNumPMTHorizontal-WCBarrelRingNPhi*WCPMTperCellHorizontal); i++){
		for(G4long j = 0; j < WCPMTperCellVertical; j++){
		  G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.),
													 towerWidth/2.-(i+0.5)*horizontalSpacingExtra,
													 -barrelCellHeight/2.+(j+0.5)*verticalSpacing);
		  PMTPosition.rotateZ(-(2*pi-totalAngle)/2.+barrelPhiOffset); // align with the symmetry 
		  //axes of the cell 

		  //G4cout << "Adding PMT in extra tower " << i << ", " << j << " position: " << PMTPosition << G4endl;
#ifdef ACTIVATE_IDPMTS
		  //G4VPhysicalVolume* physiWCBarrelPMT =
		  new G4PVPlacement(WCExtraPMTRotation,             // its rotation
							PMTPosition, 
							(i==j && hybrid && WCPMTPercentCoverage2!=0)?logicWCPMT2:logicWCPMT,                // its logical volume
							pmtname,             // its name
							logicWCExtraTowerCell,         // its mother volume
							false,                     // no boolean operations
							i*WCPMTperCellVertical+j,
							checkOverlapsPMT);                       
#endif
		  // logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
		  // daughter volumes to the PMTs (e.g. a acryl cover) you have to check, if
		  // this is still the case.
		}
	  }

	}

  }//end if placeBarrelPMTs

  // # -------------------------------------- #
  // ##########################################
  // # Prototype Outer-Detector OD Hyper-K HK #
  // ##########################################
  // # -------------------------------------- #

  // Goal is to create a dedicated ConstructOD() method in charge of defining an OD geometry
  // for any WC detector.
  // Parameters will be :
  // - NPMTs by Cells horizontally and vertically
  // - Coverage
  // - Dead Volume Size
  // - Water Size

  if(isODConstructed){
    //-------------------------------------------------------------
    // OD Tyvek Caps
    // ------------------------------------------------------------

	G4VSolid* solidWCODTopCapTyvek = nullptr;
	G4VSolid* solidWCODBotCapTyvek = nullptr;

	G4double odTopCapZ[4] = {
		(-WCODDeadSpace+1*mm+WCBlackSheetThickness),
		-.5*(WCODTyvekSheetThickness),
		-.5*(WCODTyvekSheetThickness),
		.5*(WCODTyvekSheetThickness)};
	G4double odBotCapZ[4] = {
		-(-WCODDeadSpace+1*mm+WCBlackSheetThickness),
		.5*(WCODTyvekSheetThickness),
		.5*(WCODTyvekSheetThickness),
		-.5*(WCODTyvekSheetThickness)};
	G4double odCapRmin[4] = {WCODRadius-WCODTyvekSheetThickness,
							 WCODRadius-WCODTyvekSheetThickness,
							 0,
							 0};
	G4double odCapRmax[4] = {WCODRadius,WCODRadius,WCODRadius,WCODRadius};

	if(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal){
		solidWCODTopCapTyvek
		= new G4Polyhedra("WCODTopCapTyvek",
							barrelPhiOffset, // phi start
							totalAngle, //phi end
							WCBarrelRingNPhi, //NPhi-gon
							4, // 4 z-planes
							odTopCapZ, //position of the Z planes
							odCapRmin, // min radius at the z planes
							odCapRmax// max radius at the Z planes
							);
		solidWCODBotCapTyvek
		= new G4Polyhedra("WCODBotCapTyvek",
							barrelPhiOffset, // phi start
							totalAngle, //phi end
							WCBarrelRingNPhi, //NPhi-gon
							4, // 4 z-planes
							odBotCapZ, //position of the Z planes
							odCapRmin, // min radius at the z planes
							odCapRmax// max radius at the Z planes
							);
	} else {
		// if there is an extra tower, the cap volume is a union of
		// to polyhedra. We have to unite both parts, because there are 
		// PMTs that are on the border between both parts.
		G4Polyhedra* mainPartTop 
		= new G4Polyhedra("WCODTopCapTyvekMainPart",
							barrelPhiOffset, // phi start
							totalAngle, //phi end
							WCBarrelRingNPhi, //NPhi-gon
							4, // 4 z-planes
							odTopCapZ, //position of the Z planes
							odCapRmin, // min radius at the z planes
							odCapRmax// max radius at the Z planes
							);
		G4Polyhedra* mainPartBot 
		= new G4Polyhedra("WCODBotCapTyvekMainPart",
							barrelPhiOffset, // phi start
							totalAngle, //phi end
							WCBarrelRingNPhi, //NPhi-gon
							4, // 4 z-planes
							odBotCapZ, //position of the Z planes
							odCapRmin, // min radius at the z planes
							odCapRmax// max radius at the Z planes
							);
		G4double extraCapRmin[4]; 
		G4double extraCapRmax[4]; 
		for(int i = 0; i < 4 ; i++){
			extraCapRmin[i] = odCapRmin[i] != 0. ?  odCapRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
			extraCapRmax[i] = odCapRmax[i] != 0. ?  odCapRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
		}
		G4Polyhedra* extraSliceTop
		= new G4Polyhedra("WCODTopCapTyvekExtraSlice",
							totalAngle-2.*pi+barrelPhiOffset, // phi start
							2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //total phi 
							// fortunately there are no PMTs an the gap!
							1, //NPhi-gon
							4, //  z-planes
							odTopCapZ, //position of the Z planes
							extraCapRmin, // min radius at the z planes
							extraCapRmax// max radius at the Z planes
							);
		G4Polyhedra* extraSliceBot
		= new G4Polyhedra("WCODBotCapTyvekExtraSlice",
							totalAngle-2.*pi+barrelPhiOffset, // phi start
							2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //total phi 
							// fortunately there are no PMTs an the gap!
							1, //NPhi-gon
							4, //  z-planes
							odBotCapZ, //position of the Z planes
							extraCapRmin, // min radius at the z planes
							extraCapRmax// max radius at the Z planes
							);

		solidWCODTopCapTyvek =
		new G4UnionSolid("WCODTopCapTyvek", mainPartTop, extraSliceTop);

		solidWCODBotCapTyvek =
		new G4UnionSolid("WCODBotCapTyvek", mainPartBot, extraSliceBot);
	
	}

    // G4Tubs* solidWCODCapsTyvek = new G4Tubs("WCODCapsTyvek",
    //                                         0,
    //                                         WCIDRadius,
    //                                         .5*(WCODTyvekSheetThickness),
    //                                         0.*deg,
    //                                         360.*deg);

    G4LogicalVolume* logicWCODTopCapTyvek =
	  new G4LogicalVolume(solidWCODTopCapTyvek,
						  G4Material::GetMaterial("Tyvek"),
						  "WCODTopCapTyvek",
						  0,0,0);
	G4LogicalVolume* logicWCODBotCapTyvek =
	  new G4LogicalVolume(solidWCODBotCapTyvek,
						  G4Material::GetMaterial("Tyvek"),
						  "WCODBotCapTyvek",
						  0,0,0);

    G4VisAttributes* WCCapsODTyvekCellVisAtt =
	  new G4VisAttributes(yellow);
    WCCapsODTyvekCellVisAtt->SetForceWireframe(true);

    logicWCODTopCapTyvek->SetVisAttributes(G4VisAttributes::Invisible);
	logicWCODBotCapTyvek->SetVisAttributes(G4VisAttributes::Invisible);
    //// Uncomment following for TYVEK visualization
    logicWCODTopCapTyvek->SetVisAttributes(WCCapsODTyvekCellVisAtt);
	logicWCODBotCapTyvek->SetVisAttributes(WCCapsODTyvekCellVisAtt);

    G4ThreeVector CapTyvekPosition(0.,0.,(WCIDHeight + 2*WCODDeadSpace)/2);

    //G4VPhysicalVolume* physiWCODTopCapsTyvek =
	  new G4PVPlacement(0,
						CapTyvekPosition,
						logicWCODTopCapTyvek,
						"WCODTopCapsTyvek",
						logicWCBarrel,
						false,
						0,
						checkOverlaps);

    //G4LogicalSkinSurface *WaterTySurfaceTop =
	  new G4LogicalSkinSurface("WaterTySurfaceTop", logicWCODTopCapTyvek, OpWaterTySurface);

    CapTyvekPosition.setZ(-CapTyvekPosition.getZ());

    //G4VPhysicalVolume* physiWCODBottomCapsTyvek =
	  new G4PVPlacement(0,
						CapTyvekPosition,
						logicWCODBotCapTyvek,
						"WCODBottomCapsTyvek",
						logicWCBarrel,
						false,
						0,
						checkOverlaps);
	  new G4LogicalSkinSurface("WaterTySurfaceBot", logicWCODBotCapTyvek, OpWaterTySurface);


    //-------------------------------------------------------------
    // OD Tyvek Barrel side
    // ------------------------------------------------------------

    G4double annulusODTyvekRmax[2] = {(WCODRadius),
                                      WCODRadius};
    G4double annulusODTyvekRmin[2] = {(WCODRadius-WCODTyvekSheetThickness),
                                      WCODRadius-WCODTyvekSheetThickness};

    G4Polyhedra* solidWCBarrelCellODTyvek =
	  new G4Polyhedra("WCBarrelCellODTyvek",
					  -dPhi/2., // phi start
					  dPhi, //total phi
					  1, //NPhi-gon
					  2,
					  RingZ,
					  annulusODTyvekRmin,
					  annulusODTyvekRmax);

    G4LogicalVolume* logicWCBarrelCellODTyvek =
	  new G4LogicalVolume(solidWCBarrelCellODTyvek,
						  G4Material::GetMaterial("Tyvek"),
						  "WCBarrelCellODTyvek",
						  0,0,0);

    G4VisAttributes* WCBarrelODTyvekCellVisAtt =
	  new G4VisAttributes(yellow);
    WCBarrelODTyvekCellVisAtt->SetForceWireframe(true);
    WCBarrelODTyvekCellVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown

    logicWCBarrelCellODTyvek->SetVisAttributes(G4VisAttributes::Invisible);
    //// Uncomment following for TYVEK visualization
    logicWCBarrelCellODTyvek->SetVisAttributes(WCBarrelODTyvekCellVisAtt);


    //G4VPhysicalVolume* physiWCBarrelCellODTyvek =
	  new G4PVPlacement(0,
						G4ThreeVector(0.,0.,0.),
						logicWCBarrelCellODTyvek,
						"WCBarrelCellODTyvek",
						logicWCBarrelCell,
						false,
						0,
						checkOverlaps);
    //G4LogicalSkinSurface *WaterTySurfaceSide =
	  new G4LogicalSkinSurface("WaterTySurfaceSide", logicWCBarrelCellODTyvek, OpWaterTySurface);

    //-------------------------------------------------------------
    // WLS and OD PMTs Barrel Side
    // ------------------------------------------------------------

    logicWCODWLSAndPMT = ConstructPMTAndWLSPlate(WCPMTODName, WCODCollectionName, "OD");
    // sphereRadius is the size along z of the logicWCODCapTyvek box containing WLS+PMT

    ///////////////   Barrel PMT placement
    G4RotationMatrix* WCPMTODRotation = new G4RotationMatrix;
    WCPMTODRotation->rotateY(270.*deg);

    G4double barrelODCellWidth   = 2.*WCODRadius*tan(dPhi/2.);
    G4double barrelODCellHeight  = barrelCellHeight * (barrelODCellWidth/barrelCellWidth);

    // ------------------- //
    // COMPUTE OD COVERAGE //
    // ------------------- //
    G4double AreaRingOD = WCBarrelRingNPhi * barrelODCellWidth * barrelODCellHeight;
    G4double AreaCellOD = barrelODCellWidth * barrelODCellHeight;
    G4double AreaPMTOD = 3.1415*std::pow(WCPMTODRadius,2);
    G4double NPMTODCovered = (AreaRingOD/AreaPMTOD) * WCPMTODPercentCoverage/100.;
    G4double NPMTODByCellFull = NPMTODCovered/WCBarrelRingNPhi; // NPMT required par cell to achieve ODPercentOverage
    G4double NPMTODByCell = round(NPMTODCovered/WCBarrelRingNPhi); // NPMT required par cell to achieve ODPercentOverage
    G4double RealODCoverage = NPMTODByCell*AreaPMTOD/AreaCellOD;
    // ------ DEBUG ------ //
    G4cout << G4endl;
    G4cout << "AreaRingOD : " << AreaRingOD/m2 << " (m2)" << G4endl;
    G4cout << "AreaCellOD : " << AreaCellOD/m2 << " (m2)" << G4endl;
    G4cout << "AreaPMTOD : " << AreaPMTOD/m2 << " (m2)" << G4endl;
    G4cout << "--> NbPMTODCovered : " << NPMTODCovered << G4endl;
    G4cout << "--> NbPMTODByCellFull : " << NPMTODByCellFull << G4endl;
    G4cout << "--> NbPMTODByCell : " << NPMTODByCell << G4endl;
    G4cout << "--> SuggestedODCoverage : " <<  WCPMTODPercentCoverage/100. << G4endl;
    G4cout << "--> RealODCoverage : " << RealODCoverage << G4endl;
    G4cout << G4endl;
    // ------------------- //
    // The number of PMTs per cell gives a slightly different coverage so the photocoverage
    // parameter must be changed here so the endcaps will have the same photocoverage as the barrel.
    WCPMTODPercentCoverage = RealODCoverage*100;
    WCODCapPMTSpacing  = (pi*WCIDDiameter/(round(WCIDDiameter*sqrt(pi*WCPMTODPercentCoverage)/(10.0*WCPMTODRadius))));

    if(WCPMTODperCellHorizontal == 0 && WCPMTODperCellVertical == 0){
      ComputeWCODPMT((G4int)NPMTODByCell,WCPMTODperCellHorizontal,WCPMTODperCellVertical);
    }

    G4double horizontalODSpacing = barrelODCellWidth/WCPMTODperCellHorizontal;
    G4double verticalODSpacing   = barrelODCellHeight/WCPMTODperCellVertical;

    if(WCODPMTShift > barrelODCellWidth/2. - WCPMTODRadius) WCODPMTShift = 0.*cm;

    for(G4long i = 0; i < WCPMTODperCellHorizontal; i++){
      for(G4long j = 0; j < WCPMTODperCellVertical; j++){
		G4cout << "Adding OD PMT in cell " << i << ", " << j << G4endl;
        G4ThreeVector Container =  G4ThreeVector(WCODRadius,
                                                 -barrelODCellWidth/2.+(i+0.5)*horizontalODSpacing+((G4int)(std::pow(-1,j))*(G4int)(WCODPMTShift)/2),
                                                 -(barrelCellHeight * (barrelODCellWidth/barrelCellWidth))/2.+(j+0.5)*verticalODSpacing);

		//		G4cout << " qqqqqqqqqqqqqqqqqqqqqqqq barrel i " << i << " of " << WCPMTODperCellHorizontal << " j " << j << " of " << WCPMTODperCellVertical << " Container (" << Container.x() << ", " << Container.y()
		//				  << ", " << Container.z() << ") " << G4endl;

        //G4VPhysicalVolume* physiWCBarrelWLSPlate =
		new G4PVPlacement(WCPMTODRotation,           // its rotation
						  Container,
						  logicWCODWLSAndPMT,         // its logical volume
						  "WCBarrelCellODContainer",  // its name
						  logicWCBarrelCell,         // its mother volume
						  false,                     // no boolean operations
						  i*WCPMTODperCellVertical+j,
						  checkOverlapsPMT);


      }
    }


    //-------------------------------------------------------------
    // Add PMTs in extra Tower if necessary
    //------------------------------------------------------------

    // EXTRA TOWER PART
    if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){

      // TYVEK

      G4double towerODTyvekRmin[2];
      G4double towerODTyvekRmax[2];
      for(int i = 0; i < 2; i++){
        towerODTyvekRmin[i] = annulusODTyvekRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
        towerODTyvekRmax[i] = annulusODTyvekRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
      }
      G4Polyhedra* solidWCTowerODTyvek = new G4Polyhedra("WCExtraTowerODTyvek",
                                                         totalAngle-2.*pi+barrelPhiOffset,//+dPhi/2., // phi start
                                                         2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //phi end
                                                         1, //NPhi-gon
                                                         2,
                                                         RingZ,
                                                         towerODTyvekRmin,
                                                         towerODTyvekRmax);

      G4LogicalVolume* logicWCTowerODTyvek =
		new G4LogicalVolume(solidWCTowerODTyvek,
							G4Material::GetMaterial("Tyvek"),
							"WCExtraTowerODTyvek",
							0,0,0);

      //G4LogicalSkinSurface *WaterExtraTySurfaceSide =
	  new G4LogicalSkinSurface("WaterExtraTySurfaceSide", logicWCTowerODTyvek, OpWaterTySurface);


      logicWCTowerODTyvek->SetVisAttributes(G4VisAttributes::Invisible);
      //// Uncomment following for TYVEK visualization
      logicWCTowerODTyvek->SetVisAttributes(WCBarrelODTyvekCellVisAtt);


      //G4VPhysicalVolume* physiWCTowerODTyvek =
	  new G4PVPlacement(0,
						G4ThreeVector(0.,0.,0.),
						logicWCTowerODTyvek,
						"WCExtraTowerODTyvek",
						logicWCExtraTowerCell,
						false,
						0,
						checkOverlaps);

      // PMTs

      G4RotationMatrix* WCExtraODPMTRotation = new G4RotationMatrix;
      WCExtraODPMTRotation->rotateY(270.*deg);
      WCExtraODPMTRotation->rotateX(2*pi - (2*pi-totalAngle)/2.+barrelPhiOffset);//align the PMT with the Cell

      G4double towerWidthOD = WCODRadius*tan(2*pi-totalAngle);
      // We don't want the same number of OD PMTs squished horizontally so we scale down the horizontal PMTs by the width of the extra tower
      G4double ratioOfWidths = (double)(WCPMTODperCellHorizontal)*(towerWidthOD/barrelODCellWidth);
      G4long WCPMTODperCellHorizontalExtra = std::lround(ratioOfWidths);
      G4double horizontalODSpacingExtra   = towerWidthOD/(double)WCPMTODperCellHorizontalExtra;
      // verticalODSpacing is identical to that for normal cell.

      for(G4long i = 0; i < (WCPMTODperCellHorizontalExtra); i++){
        for(G4long j = 0; j < WCPMTODperCellVertical; j++){
		  G4cout << "Adding OD PMT in cell " << i << ", " << j << G4endl;
		  G4ThreeVector Container =  G4ThreeVector((WCODRadius)/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.),
												   -towerWidthOD/2.+(i+0.5)*horizontalODSpacingExtra,
												   -(barrelCellHeight * (WCODRadius/WCIDRadius))/2.+(j+0.5)*verticalODSpacing);

		  Container.rotateZ(-(2*pi-totalAngle)/2.+barrelPhiOffset); // align with the symmetry

		  //G4VPhysicalVolume* physiWCExtraBarrelWLSPlate =
		  new G4PVPlacement(WCExtraODPMTRotation,              // its rotation
							Container,
							logicWCODWLSAndPMT,                // its logical volume
							"WCExtraBarrelCellODContainer",             // its name
							logicWCExtraTowerCell,         // its mother volume
							false,                     // no boolean operations
							i*WCPMTODperCellVertical+j,
							checkOverlapsPMT);


        }
      }

    }

    //---------------------------------------------------------
    // Add top and bottom PMTs
    // -----------------------------------------------------

    G4double xoffset;
    G4double yoffset;
    G4int    icopy = 0;

    G4RotationMatrix* WCCapPMTRotation = new G4RotationMatrix;
    WCCapPMTRotation->rotateY(180.*deg);


    // loop over the cap
    G4int CapNCell = (G4int)(WCODCapEdgeLimit/WCODCapPMTSpacing) + 2;
    for ( int i = -CapNCell ; i <  CapNCell; i++) {
      for (int j = -CapNCell ; j <  CapNCell; j++) {

		G4cout << "Adding OD PMT in top/bottom cap cell " << i << ", " << j << G4endl;

        xoffset = i*WCODCapPMTSpacing + WCODCapPMTSpacing*0.5;
        yoffset = j*WCODCapPMTSpacing + WCODCapPMTSpacing*0.5;

        G4ThreeVector topWLSpos = G4ThreeVector(xoffset,
                                                yoffset,
                                                ((WCIDHeight + 2*WCODDeadSpace)/2)+WCODTyvekSheetThickness);

        G4ThreeVector bottomWLSpos = G4ThreeVector(xoffset,
                                                   yoffset,
                                                   -topWLSpos.getZ());

        if (((sqrt(xoffset*xoffset + yoffset*yoffset) + WCPMTODRadius) < WCODCapEdgeLimit) ) {


		  //		  G4cout << " qqqqqqqqqqqqqqqqqqqqqqqq cap i " << i << " of " << CapNCell << " j " << j << " of " << CapNCell << " Container (" << topWLSpos.x() << ", " << topWLSpos.y()
		  //				  << ", " << topWLSpos.z() << ") " << G4endl;

		  //G4VPhysicalVolume* physiTopCapWLSPlate =
			new G4PVPlacement(0,                   // its rotation
							  topWLSpos,
							  logicWCODWLSAndPMT,   // its logical volume
							  "WCTopCapContainerOD",// its name
							  logicWCBarrel,       // its mother volume
							  false,               // no boolean operations
							  icopy,
							  checkOverlapsPMT);


		  //G4VPhysicalVolume* physiBottomCapWLSPlate =
			new G4PVPlacement(WCCapPMTRotation,                             // its rotation
							  bottomWLSpos,
							  logicWCODWLSAndPMT,   // its logical volume
							  "WCBottomCapContainerOD",                // its name
							  logicWCBarrel,                 // its mother volume
							  false,                         // no boolean operations
							  icopy,
							  checkOverlapsPMT);

		  icopy++;

        }
      }
    }

    G4cout << "#### OD ####" << "\n";
    G4cout << " total on cap: " << icopy << "\n";
    G4cout << " Coverage was calculated to be: " << (icopy*WCPMTODRadius*WCPMTODRadius/(WCIDRadius*WCIDRadius)) << "\n";
    G4cout << "############" << "\n";

  } // END if isODConstructed

  G4LogicalVolume* logicTopCapAssembly = ConstructCaps(true);
  G4LogicalVolume* logicBottomCapAssembly = ConstructCaps(false);

  // These lines make the large cap volume invisible to view the caps blacksheets. Need to make invisible for
  // RayTracer
  if (Vis_Choice == "RayTracer"){
	logicBottomCapAssembly->SetVisAttributes(G4VisAttributes::Invisible);
	logicTopCapAssembly->SetVisAttributes(G4VisAttributes::Invisible);
  } else {
	logicBottomCapAssembly->SetVisAttributes(G4VisAttributes::Invisible);
	logicTopCapAssembly->SetVisAttributes(G4VisAttributes::Invisible);
  }

  //G4VPhysicalVolume* physiTopCapAssembly =
	new G4PVPlacement(0,
					  G4ThreeVector(0.,0.,(mainAnnulusHeight/2.+ capAssemblyHeight/2.)),
					  logicTopCapAssembly,
					  "TopCapAssembly",
					  logicWCBarrel,
					  false, 0,
					  checkOverlaps);

  //G4VPhysicalVolume* physiBottomCapAssembly =
    new G4PVPlacement(0,
					  G4ThreeVector(0.,0.,(-mainAnnulusHeight/2.- capAssemblyHeight/2.)),
					  logicBottomCapAssembly,
					  "BottomCapAssembly",
					  logicWCBarrel,
					  false, 0,
					  checkOverlaps);

  return logicWC;
}


/**
 * Construct an endcap.  The flipz argument indicates the orientation/end:
 * - flipz == false:  Bottom endcap, construct upright
 * - flipz == true:   Top endcap, construct reflected
 */
G4LogicalVolume* WCSimDetectorConstruction::ConstructCaps(G4bool flipz)
{
  // z-orientation multiplier
  const G4int zflip = (flipz ? -1 : 1);
  // substrings for constructing chiral component names
  const G4String oristr = flipz ? "Top" : "Bot";
  const G4String capstr = oristr + G4String("Cap");  // "[Top|Bot]Cap"
  const G4String bbstr  = G4String("Barrel") +
                oristr + G4String("Border");   // "Barrel[Top|Bot]Border"

  capAssemblyHeight = (WCIDHeight-mainAnnulusHeight)/2+1*mm+WCBlackSheetThickness;

  const G4String caname = capstr + G4String("Assembly");  // "[Top|Bot]CapAssembly"
  G4Tubs* solidCapAssembly = new G4Tubs(caname,
										0.0*m,
										outerAnnulusRadius/cos(dPhi/2.), 
										capAssemblyHeight/2,
										0.*deg,
										360.*deg);

  G4LogicalVolume* logicCapAssembly =
    new G4LogicalVolume(solidCapAssembly,
                        G4Material::GetMaterial(water),
                        caname,
                        0,0,0);

  G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4VisAttributes::Invisible);
  tmpVisAtt->SetForceWireframe(true);// This line is used to give definition to the rings in OGLSX Visualizer
  logicCapAssembly->SetVisAttributes(tmpVisAtt);

  //----------------------------------------------------
  // extra rings for the top and bottom of the annulus
  //---------------------------------------------------
  G4double borderAnnulusZ[3] = {
        (-barrelCellHeight/2.-(WCIDRadius-innerAnnulusRadius))*zflip, 
		-barrelCellHeight/2.*zflip,
		barrelCellHeight/2.*zflip};
  G4double borderAnnulusRmin[3] = { WCIDRadius, innerAnnulusRadius, innerAnnulusRadius};
  G4double borderAnnulusRmax[3] = {outerAnnulusRadius, outerAnnulusRadius,outerAnnulusRadius};

  if(std::abs(borderAnnulusZ[2] - borderAnnulusZ[0]) > capAssemblyHeight - WCBlackSheetThickness) {
    G4cerr << "IMPOSSIBLE GEOMETRY:  capAssemblyHeight (" 
          << capAssemblyHeight << ") too small to contain border ring ("
          << std::abs(borderAnnulusZ[2] - borderAnnulusZ[0])
          << ") and endcap blacksheet (" << WCBlackSheetThickness << ")"
          << G4endl;
    /// Should throw an exception.
  }

  const G4String bbrname = G4String("WC") +
                bbstr + G4String("Ring");  // "WCBarrel[Top|Bot]BorderRing"
  G4Polyhedra* solidWCBarrelBorderRing = new G4Polyhedra(bbrname,
														 barrelPhiOffset, // phi start
														 totalAngle,
														 WCBarrelRingNPhi, //NPhi-gon
														 3,
														 borderAnnulusZ,
														 borderAnnulusRmin,
														 borderAnnulusRmax);
  G4LogicalVolume* logicWCBarrelBorderRing =
    new G4LogicalVolume(solidWCBarrelBorderRing,
                        G4Material::GetMaterial(water),
                        bbrname,
                        0,0,0);
  //G4cout << *solidWCBarrelBorderRing << G4endl;

  //G4VPhysicalVolume* physiWCBarrelBorderRing =
  new G4PVPlacement(0,
					G4ThreeVector(0.,0.,(capAssemblyHeight/2.- barrelCellHeight/2.)*zflip),
					logicWCBarrelBorderRing,
					bbrname,
					logicCapAssembly,
					false, 0,
					checkOverlaps);

                  
  if(!debugMode){ 

    tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
  	tmpVisAtt->SetForceSolid(true);// This line is used to give definition to the cells in OGLSX Visualizer
  	//logicWCBarrelBorderRing->SetVisAttributes(tmpVisAtt); 
    logicWCBarrelBorderRing->SetVisAttributes(G4VisAttributes::Invisible); 
	//TF vis.
  }

  //----------------------------------------------------
  // Subdivide border rings into cells
  // --------------------------------------------------
  const G4String bbcname = G4String("WC") +
                bbstr + G4String("Cell");  // "WCBarrel[Top|Bot]BorderCell"
  G4Polyhedra* solidWCBarrelBorderCell = new G4Polyhedra(bbcname,
														 -dPhi/2., // phi start
														 dPhi, //total angle 
														 1, //NPhi-gon
														 3,
														 borderAnnulusZ,
														 borderAnnulusRmin,
														 borderAnnulusRmax);

  G4LogicalVolume* logicWCBarrelBorderCell =
    new G4LogicalVolume(solidWCBarrelBorderCell, 
                        G4Material::GetMaterial(water),
                        bbcname,
                        0,0,0);
  //G4cout << *solidWCBarrelBorderCell << G4endl;
  G4VPhysicalVolume* physiWCBarrelBorderCell =
    new G4PVReplica(bbcname,
                    logicWCBarrelBorderCell,
                    logicWCBarrelBorderRing,
                    kPhi,
                    WCBarrelRingNPhi,
                    dPhi,
                    barrelPhiOffset);

  //-------------------------------------------------------------
  // add barrel blacksheet to the border barrel cells 
  // ------------------------------------------------------------
  G4double RingZ[2] = {borderAnnulusZ[0], borderAnnulusZ[2]};
  G4double annulusBlackSheetRmax[2] = {WCIDRadius+WCBlackSheetThickness,
				       WCIDRadius+WCBlackSheetThickness};
  G4double annulusBlackSheetRmin[2] = {WCIDRadius,
				       WCIDRadius};

  const G4String bbcbsname = bbcname + G4String("BlackSheet");    // "WCBarrel[Top|Bot]BorderCellBlackSheet"
  G4Polyhedra* solidWCBarrelBorderCellBlackSheet = 
        new G4Polyhedra(bbcbsname,
			-dPhi/2., // phi start
			dPhi, //total phi
			1, //NPhi-gon
			2,
			RingZ,
			annulusBlackSheetRmin,
			annulusBlackSheetRmax);

  G4LogicalVolume* logicWCBarrelBorderCellBlackSheet =
    new G4LogicalVolume(solidWCBarrelBorderCellBlackSheet,
                        G4Material::GetMaterial("Blacksheet"),
                        bbcbsname,
						0,0,0);

  // These lines of code below will turn the border rings invisible. 
  // used for RayTracer
  if (Vis_Choice == "RayTracer"){
    G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
        = new G4VisAttributes(G4Colour(0.2,0.9,0.2)); // green color
    WCBarrelBlackSheetCellVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
    WCBarrelBlackSheetCellVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown
	if(!debugMode){
	  tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
	  tmpVisAtt->SetForceSolid(true);
	  logicWCBarrelBorderCell->SetVisAttributes(tmpVisAtt);
	  logicWCBarrelBorderCell->SetVisAttributes(G4VisAttributes::Invisible);
	  logicWCBarrelBorderCellBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
        }
	else {
	  tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
	  tmpVisAtt->SetForceWireframe(true);
	  logicWCBarrelBorderCell->SetVisAttributes(tmpVisAtt);
	  logicWCBarrelBorderCell->SetVisAttributes(G4VisAttributes::Invisible);
	  logicWCBarrelBorderCellBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);
        }
  }
  // used for OGLSX
  else {
    G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
        = new G4VisAttributes(G4Colour(0.2,0.9,0.2));
	if(!debugMode)
	{
	  tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
	  tmpVisAtt->SetForceSolid(true);
	  logicWCBarrelBorderCell->SetVisAttributes(tmpVisAtt);
	  logicWCBarrelBorderCell->SetVisAttributes(G4VisAttributes::Invisible);
	  logicWCBarrelBorderCellBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);
	}
	else {
	  tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
	  //tmpVisAtt->SetForceWireframe(true);
	  tmpVisAtt->SetForceSolid(true);
	  logicWCBarrelBorderCell->SetVisAttributes(tmpVisAtt);
	  logicWCBarrelBorderCellBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
        }
  }


  G4VPhysicalVolume* physiWCBarrelBorderCellBlackSheet =
    new G4PVPlacement(0,
                      G4ThreeVector(0.,0.,0.),
                      logicWCBarrelBorderCellBlackSheet,
                      bbcbsname,
                      logicWCBarrelBorderCell,
                      false,
                      0,
		      checkOverlaps);

  //G4LogicalBorderSurface * WaterBSBarrelBorderCellSurface =
  const G4String bbcwbssname = bbcname + G4String("WaterBSSurface");
  new G4LogicalBorderSurface(bbcwbssname,
							 physiWCBarrelBorderCell,
							 physiWCBarrelBorderCellBlackSheet,
							 OpWaterBSSurface);

  new G4LogicalSkinSurface(bbcwbssname,logicWCBarrelBorderCellBlackSheet,
							BSSkinSurface);

  // we have to declare the logical Volumes 
  // outside of the if block to access it later on 
  G4LogicalVolume* logicWCExtraBorderCell = nullptr;
  G4VPhysicalVolume* physiWCExtraBorderCell = nullptr;
  const G4String etbcname = G4String("WCExtraTower") + oristr +
        G4String("BorderCell");      // "WCExtraTower[Top|Bot]]BorderCell"
  if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){
    //----------------------------------------------
    // also the extra tower need special cells at the 
    // top and th bottom.
    // (the top cell is created later on by reflecting the
    // bottom cell) 
    //---------------------------------------------
    G4double extraBorderRmin[3];
    G4double extraBorderRmax[3];
    for(int i = 0; i < 3; i++){
      extraBorderRmin[i] = borderAnnulusRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
      extraBorderRmax[i] = borderAnnulusRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
    } 
    G4Polyhedra* solidWCExtraBorderCell = new G4Polyhedra(etbcname,
														  totalAngle-2.*pi+barrelPhiOffset,//+dPhi/2., // phi start
														  2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //total phi
														  1, //NPhi-gon
														  3,
														  borderAnnulusZ,
														  extraBorderRmin,
														  extraBorderRmax);

    logicWCExtraBorderCell =
      new G4LogicalVolume(solidWCExtraBorderCell, 
						  G4Material::GetMaterial(water),
						  etbcname,
						  0,0,0);
    //G4cout << *solidWCExtraBorderCell << G4endl;

    physiWCExtraBorderCell =
      new G4PVPlacement(0,
						G4ThreeVector(0.,0.,(capAssemblyHeight/2.- barrelCellHeight/2.)*zflip),
						logicWCExtraBorderCell,
						etbcname,
						logicCapAssembly,
						false, 0,
						checkOverlaps);

    tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
  	tmpVisAtt->SetForceSolid(true);// This line is used to give definition to the cells in OGLSX Visualizer
  	logicWCExtraBorderCell->SetVisAttributes(tmpVisAtt); 
    logicWCExtraBorderCell->SetVisAttributes(G4VisAttributes::Invisible);
    //TF vis.


    //---------------------------------------------
    // add blacksheet to this cells
    //--------------------------------------------
    G4double extraBSRmin[2];
    G4double extraBSRmax[2];
    for(int i = 0; i < 2; i++){
      extraBSRmin[i] = annulusBlackSheetRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
      extraBSRmax[i] = annulusBlackSheetRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
    } 
    const G4String etbcbsname = etbcname +
        G4String("BlackSheet");      // "WCExtraTower[Top|Bot]]BorderCellBlackSheet"
    G4Polyhedra* solidWCExtraBorderBlackSheet = 
      new G4Polyhedra(etbcbsname,
			   totalAngle-2.*pi+barrelPhiOffset,//+dPhi/2., // phi start
			   2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //phi end
			   1, //NPhi-gon
			   2,
			   RingZ,
			   extraBSRmin,
			   extraBSRmax);

    G4LogicalVolume* logicWCExtraBorderBlackSheet =
      new G4LogicalVolume(solidWCExtraBorderBlackSheet, 
			  G4Material::GetMaterial("Blacksheet"),
			  etbcbsname, 
			  0,0,0);

    G4VPhysicalVolume* physiWCExtraBorderBlackSheet =
      new G4PVPlacement(0,
			G4ThreeVector(0.,0.,0.),
			logicWCExtraBorderBlackSheet,
			etbcbsname,
			logicWCExtraBorderCell,
			false,
			0,
			checkOverlaps);

    //G4LogicalBorderSurface * WaterBSExtraBorderCellSurface =
    const G4String etbcwsname = etbcname + G4String("WaterBSSurface");
        new G4LogicalBorderSurface(etbcwsname,
								   physiWCExtraBorderCell,
								   physiWCExtraBorderBlackSheet, 
								   OpWaterBSSurface);
	new G4LogicalSkinSurface(etbcname,logicWCExtraBorderBlackSheet,
							  BSSkinSurface);
  }
  //------------------------------------------------------------
  // add caps
  // -----------------------------------------------------------
  //crucial to match with borderAnnulusZ
  G4double capZ[4] = 
        { (-WCBlackSheetThickness-1.*mm)*zflip,
		  (WCBarrelPMTOffset - (WCIDRadius-innerAnnulusRadius))*zflip,
		  (WCBarrelPMTOffset - (WCIDRadius-innerAnnulusRadius))*zflip,
		  WCBarrelPMTOffset*zflip} ;
  G4double capRmin[4] = {  0. , 0., 0., 0.} ;
  G4double capRmax[4] = {outerAnnulusRadius, outerAnnulusRadius,  WCIDRadius, innerAnnulusRadius};
  const G4String capname = G4String("WC") + capstr;    // "WC[Top|Bot]Cap"
  G4VSolid* solidWCCap = nullptr;

#ifdef DEBUG
  G4cout << "B.Q Cap: " << totalAngle << ", " << WCBarrelRingNPhi << ", " << outerAnnulusRadius << ", " << innerAnnulusRadius << ", " << WCBlackSheetThickness << ", " << zflip << ", " << WCBarrelPMTOffset << G4endl;
#endif
  
  if(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal){
    solidWCCap
      = new G4Polyhedra(capname,
						barrelPhiOffset, // phi start
						totalAngle, //phi end
						WCBarrelRingNPhi, //NPhi-gon
						4, // 2 z-planes
						capZ, //position of the Z planes
						capRmin, // min radius at the z planes
						capRmax// max radius at the Z planes
						);
  } else {
	// if there is an extra tower, the cap volume is a union of
	// to polyhedra. We have to unite both parts, because there are 
	// PMTs that are on the border between both parts.
	G4Polyhedra* mainPart 
      = new G4Polyhedra(capname + G4String("MainPart"),
						barrelPhiOffset, // phi start
						totalAngle, //phi end
						WCBarrelRingNPhi, //NPhi-gon
						4, // 2 z-planes
						capZ, //position of the Z planes
						capRmin, // min radius at the z planes
						capRmax// max radius at the Z planes
						);
    G4double extraCapRmin[4]; 
    G4double extraCapRmax[4]; 
    for(int i = 0; i < 4 ; i++){
      extraCapRmin[i] = capRmin[i] != 0. ?  capRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
      extraCapRmax[i] = capRmax[i] != 0. ? capRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
    }
    G4Polyhedra* extraSlice 
      = new G4Polyhedra(capname + G4String("ExtraSlice"),
						totalAngle-2.*pi+barrelPhiOffset, // phi start
						2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //total phi 
						// fortunately there are no PMTs an the gap!
						1, //NPhi-gon
						4, //  z-planes
						capZ, //position of the Z planes
						extraCapRmin, // min radius at the z planes
						extraCapRmax// max radius at the Z planes
						);
	solidWCCap =
	  new G4UnionSolid(capname, mainPart, extraSlice);

	//G4cout << *solidWCCap << G4endl;
   
  }
  // G4cout << *solidWCCap << G4endl;
  G4LogicalVolume* logicWCCap = 
    new G4LogicalVolume(solidWCCap,
						G4Material::GetMaterial(water),
						capname + G4String("Polygon"),
						0,0,0);

  G4VPhysicalVolume* physiWCCap =
    new G4PVPlacement(0,                           // no rotation
					  G4ThreeVector(0.,0.,(-capAssemblyHeight/2.+1*mm+WCBlackSheetThickness)*zflip),     // its position
					  logicWCCap,          // its logical volume
					  capname,             // its name
					  logicCapAssembly,                  // its mother volume
					  false,                       // no boolean operations
					  0,                          // Copy #
					  checkOverlaps);

  // used for RayTracer
  if (Vis_Choice == "RayTracer"){
	if(!debugMode){  
	  tmpVisAtt = new G4VisAttributes(G4Colour(1,0.5,0.5));
	  tmpVisAtt->SetForceSolid(true);
	  logicWCCap->SetVisAttributes(tmpVisAtt);
	  logicWCCap->SetVisAttributes(G4VisAttributes::Invisible);

	} else{
	
	  tmpVisAtt = new G4VisAttributes(G4Colour(0.6,0.5,0.5));
	  tmpVisAtt->SetForceSolid(true);
	  logicWCCap->SetVisAttributes(tmpVisAtt);}}

  // used for OGLSX
  else{
	if(!debugMode){  

	  tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
	  tmpVisAtt->SetForceSolid(true);// This line is used to give definition to the cells in OGLSX Visualizer
	  //logicWCCap->SetVisAttributes(tmpVisAtt); 
	  logicWCCap->SetVisAttributes(G4VisAttributes::Invisible);
	  //TF vis.

	} else
	  {tmpVisAtt = new G4VisAttributes(G4Colour(.6,0.5,0.5));
		tmpVisAtt->SetForceWireframe(true);
		logicWCCap->SetVisAttributes(tmpVisAtt);}}

  //---------------------------------------------------------------------
  // add cap blacksheet
  // -------------------------------------------------------------------
  
  G4double capBlackSheetZ[4] = {-WCBlackSheetThickness*zflip, 0., 0., 
                                (WCBarrelPMTOffset - (WCIDRadius-innerAnnulusRadius)) *zflip};
  G4double capBlackSheetRmin[4] = {0., 0., WCIDRadius, WCIDRadius};
  G4double capBlackSheetRmax[4] = {WCIDRadius+WCBlackSheetThickness, 
                                   WCIDRadius+WCBlackSheetThickness,
                                   WCIDRadius+WCBlackSheetThickness,
                                   WCIDRadius+WCBlackSheetThickness};
  const G4String capbsname = capname + G4String("BlackSheet");

  if(capBlackSheetZ[0] * capBlackSheetZ[3] > 0.) {
    G4cerr << "IMPOSSIBLE GEOMETRY:  z profile array of " << capbsname
           << " should  be monotonic.  Computed values:"
           << "\n    capBlackSheetZ[0] = " << capBlackSheetZ[0]
           << "\n                 Z[1] = " << capBlackSheetZ[1]
           << "\n                 Z[2] = " << capBlackSheetZ[2]
           << "\n                 Z[3] = " << capBlackSheetZ[3]
          << G4endl;
    /// Should throw an exception.
  }

  G4VSolid* solidWCCapBlackSheet = nullptr;
  if(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal){
    solidWCCapBlackSheet
      = new G4Polyhedra(capbsname,
						barrelPhiOffset, // phi start
						totalAngle, //total phi
						WCBarrelRingNPhi, //NPhi-gon
						4, //  z-planes
						capBlackSheetZ, //position of the Z planes
						capBlackSheetRmin, // min radius at the z planes
						capBlackSheetRmax// max radius at the Z planes
						);
    // G4cout << *solidWCCapBlackSheet << G4endl;
  } else { 
    // same as for the cap volume
	G4Polyhedra* mainPart
      = new G4Polyhedra(capbsname + G4String("MainPart"),
						barrelPhiOffset, // phi start
						totalAngle, //phi end
						WCBarrelRingNPhi, //NPhi-gon
						4, //  z-planes
						capBlackSheetZ, //position of the Z planes
						capBlackSheetRmin, // min radius at the z planes
						capBlackSheetRmax// max radius at the Z planes
						);
	G4double extraBSRmin[4];
	G4double extraBSRmax[4];
	for(int i = 0; i < 4 ; i++){
	  extraBSRmin[i] = capBlackSheetRmin[i] != 0. ? capBlackSheetRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
	  extraBSRmax[i] = capBlackSheetRmax[i] != 0. ? capBlackSheetRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
	}
	G4Polyhedra* extraSlice
      = new G4Polyhedra(capbsname + G4String("ExtraSlice"),
						totalAngle-2.*pi+barrelPhiOffset, // phi start
						2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //
						WCBarrelRingNPhi, //NPhi-gon
						4, //  z-planes
						capBlackSheetZ, //position of the Z planes
						extraBSRmin, // min radius at the z planes
						extraBSRmax// max radius at the Z planes
						);
    
	solidWCCapBlackSheet =
	  new G4UnionSolid(capbsname, mainPart, extraSlice);
  }
  G4LogicalVolume* logicWCCapBlackSheet =
    new G4LogicalVolume(solidWCCapBlackSheet,
						G4Material::GetMaterial("Blacksheet"),
						capbsname,
						0,0,0);
  G4VPhysicalVolume* physiWCCapBlackSheet =
    new G4PVPlacement(0,
                      G4ThreeVector(0.,0.,0.),
                      logicWCCapBlackSheet,
                      capbsname,
                      logicWCCap,
                      false,
                      0,
					  checkOverlaps);

  //G4LogicalBorderSurface * WaterBSBottomCapSurface =
  new G4LogicalBorderSurface(capbsname + G4String("WaterBSSurface"),
							 physiWCCap,physiWCCapBlackSheet,
							 OpWaterBSSurface);

  new G4LogicalSkinSurface(capbsname + G4String("BSSkinSurface"),
                           logicWCCapBlackSheet,
			   BSSkinSurface);

  // used for OGLSX
  if (Vis_Choice == "OGLSX"){

	G4VisAttributes* WCCapBlackSheetVisAtt 
	  = new G4VisAttributes(G4Colour(0.9,0.2,0.2));
 
	if(!debugMode)
	  logicWCCapBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);
    else
	  logicWCCapBlackSheet->SetVisAttributes(WCCapBlackSheetVisAtt);
  }//OGLSX

  // used for RayTracer (makes the caps blacksheet yellow)
  if (Vis_Choice == "RayTracer"){

	G4VisAttributes* WCCapBlackSheetVisAtt 
	  = new G4VisAttributes(G4Colour(1.0,1.0,0.0));

	if(!debugMode)
	  //logicWCCapBlackSheet->SetVisAttributes(G4VisAttributes::Invisible); //Use this line if you want to make the blacksheet on the caps invisible to view through
	  logicWCCapBlackSheet->SetVisAttributes(WCCapBlackSheetVisAtt);
    else
	  logicWCCapBlackSheet->SetVisAttributes(WCCapBlackSheetVisAtt);
  }//RayTracer

  //---------------------------------------------------------
  // Add top and bottom PMTs
  // -----------------------------------------------------

  G4LogicalVolume* logicWCPMT;
  if(nID_PMTs<=1) logicWCPMT = ConstructPMT(WCPMTName, WCIDCollectionName,"tank",nID_PMTs);
  else logicWCPMT = ConstructMultiPMT(WCPMTName, WCIDCollectionName,"tank",nID_PMTs);
  G4LogicalVolume* logicWCPMT2 = nullptr;
  if(hybrid){
	if(nID_PMTs2<=1) logicWCPMT2 = ConstructPMT(WCPMTName2, WCIDCollectionName2,"tankPMT2",nID_PMTs2);
	else logicWCPMT2 = ConstructMultiPMT(WCPMTName2, WCIDCollectionName2,"tankPMT2",nID_PMTs2);
  }
  //G4LogicalVolume* logicWCPMT = ConstructPMT(WCPMTName, WCIDCollectionName);
  G4String pmtname = "WCMultiPMT";
 
  // If using RayTracer and want to view the detector without caps, comment out the top and bottom PMT's
  G4double xoffset;
  G4double yoffset;
  G4int    icopy = 0;

  G4RotationMatrix* WCCapPMTRotation = new G4RotationMatrix;
  //if mPMT: perp to wall
  if(orientation == PERPENDICULAR){
	if(zflip==-1){
	  WCCapPMTRotation->rotateY(180.*deg); 
	}
  }
  else if (orientation == VERTICAL)
	WCCapPMTRotation->rotateY(90.*deg);
  else if (orientation == HORIZONTAL)
	WCCapPMTRotation->rotateX(90.*deg);

  // loop over the cap
  if(placeCapPMTs){
	G4int CapNCell = WCCapEdgeLimit/WCCapPMTSpacing + 2;
#ifdef DEBUG
	G4cout << "Debug B.Q, wccap edge = " << WCCapEdgeLimit << ", spacing = " << WCCapPMTSpacing << ", CapNCell = " << CapNCell << ", PMT radius = " << WCPMTRadius << G4endl;
	//		  G4cout << "Debug B.Q, test = " << ((horizontalModulo == verticalModulo) && hybrid && WCPMTPercentCoverage2!=0) << G4endl;
#endif
	for ( int i = -CapNCell ; i <  CapNCell; i++) {
	  for (int j = -CapNCell ; j <  CapNCell; j++)   {

		// Jun. 04, 2020 by M.Shinoki
		// For IWCD (NuPRISM_mPMT Geometry)
		xoffset = i*WCCapPMTSpacing + WCCapPMTSpacing*0.5;
		yoffset = j*WCCapPMTSpacing + WCCapPMTSpacing*0.5;
		// For WCTE (NuPRISMBeamTest_mPMT Geometry)
		if (isNuPrismBeamTest || isNuPrismBeamTest_16cShort){
		  xoffset = i*WCCapPMTSpacing;
		  yoffset = j*WCCapPMTSpacing;
		}
		G4ThreeVector cellpos = G4ThreeVector(xoffset, yoffset, 0);     

		double dcenter = hybrid?std::max(WCPMTRadius,WCPMTRadius2):WCPMTRadius;
		dcenter+=sqrt(xoffset*xoffset + yoffset*yoffset);
		if (dcenter < WCCapEdgeLimit) 

		  // for debugging boundary cases: 
		  // &&  ((sqrt(xoffset*xoffset + yoffset*yoffset) + WCPMTRadius) > (WCCapEdgeLimit-100)) ) 
		  {
		
			//B.Q for Hybrid
			G4int horizontalModulo = (i+CapNCell) % WCPMTperCellHorizontal;
			G4int verticalModulo = (j+CapNCell) % WCPMTperCellVertical;

#ifdef DEBUG
			//G4cout << "Debug B.Q, xoffset = " << xoffset << ", yoffset = " << yoffset << ", distance = " << dcenter  << G4endl;
			//G4cout << "Debug B.Q, xoffset = " << xoffset << ", yoffset = " << yoffset << ", CapNCell = " << CapNCell << ", spacing = " << WCCapPMTSpacing << G4endl;
			//		  G4cout << "Debug B.Q, test = " << ((horizontalModulo == verticalModulo) && hybrid && WCPMTPercentCoverage2!=0) << G4endl;
#endif
#ifdef ACTIVATE_IDPMTS
			//G4cout << "Adding PMT on cap cell " << i << ", " << j << G4endl;
			//G4VPhysicalVolume* physiCapPMT =
			  new G4PVPlacement(WCCapPMTRotation,
								cellpos,// its position
								((horizontalModulo == verticalModulo) && hybrid && WCPMTPercentCoverage2!=0)?logicWCPMT2:logicWCPMT,            // its logical volume
								pmtname, // its name 
								logicWCCap,         // its mother volume
								false,                 // no boolean os
								icopy,               // every PMT need a unique id.
								checkOverlapsPMT);
#endif
			// logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 

			// daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
			// this is still the case.

			//						((i%WCPMTperCellHorizontal == j%WCPMTperCellVertical) && hybrid)?logicWCPMT2:logicWCPMT,            // its logical volume

			icopy++;
		  }
	  }
	}


	G4cout << "total on cap: " << icopy << "\n";
	G4cout << "Coverage was calculated to be: " << (icopy*WCPMTRadius*WCPMTRadius/(WCIDRadius*WCIDRadius)) << "\n";
  }//end if placeCapPMTs

  ///////////////   Barrel PMT placement
  G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;
  if(orientation == PERPENDICULAR)
	WCPMTRotation->rotateY(90.*deg); //if mPMT: perp to wall
  else if(orientation == VERTICAL)
	WCPMTRotation->rotateY(0.*deg); //if mPMT: vertical/aligned to wall
  else if(orientation == HORIZONTAL)
	WCPMTRotation->rotateX(90.*deg); //if mPMT: horizontal to wall
  

  const G4double barrelCellWidth = 2.*WCIDRadius*tan(dPhi/2.);
  const G4double horizontalSpacing   = barrelCellWidth/WCPMTperCellHorizontal;
  const G4double verticalSpacing     = barrelCellHeight/WCPMTperCellVertical;


  if(placeBorderPMTs){
#ifdef DEBUG
  	G4cout << "Debug B.Q : place border pmt, spacing horiz = "  << horizontalSpacing << ", vertical = " << verticalSpacing*zflip << G4endl;
#endif
	
	for(G4long i = 0; i < WCPMTperCellHorizontal; i++){
	  for(G4long j = 0; j < WCPMTperCellVertical; j++){
		G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius,
												   -barrelCellWidth/2.+(i+0.5)*horizontalSpacing,
												   (-barrelCellHeight/2.+(j+0.5)*verticalSpacing)*zflip);

#ifdef ACTIVATE_IDPMTS
#ifdef WCSIMCONSTRUCTCYLINDER_VERBOSE
		G4cout << "Add PMT on barrel cell " << i << ", " << j << G4endl;
#endif
		//G4VPhysicalVolume* physiWCBarrelBorderPMT =
		new G4PVPlacement(WCPMTRotation,                      // its rotation
						  PMTPosition,
						  (i==j && hybrid && WCPMTPercentCoverage2!=0)?logicWCPMT2:logicWCPMT,                // its logical volume
						  pmtname,             // its name
						  logicWCBarrelBorderCell,         // its mother volume
						  false,                     // no boolean operations
						  i*WCPMTperCellVertical+j,
						  checkOverlapsPMT); 
#endif

		// logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
		// daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
		// this is still the case.
	  }
	}
	//-------------------------------------------------------------
	// Add PMTs in extra Tower if necessary
	//------------------------------------------------------------


	if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){

#ifdef DEBUG
	  G4cout << "Debug B.Q : Add extra tower cap "  << G4endl;
#endif
	
	  G4RotationMatrix* WCExtraPMTRotation = new G4RotationMatrix;
	  WCExtraPMTRotation->rotateY(90.*deg);
	  WCExtraPMTRotation->rotateX((2*pi-totalAngle)/2.-barrelPhiOffset);//align the PMT with the Cell
                                                 
	  G4double towerWidth = WCIDRadius*tan(2*pi-totalAngle);

	  G4double horizontalSpacingExtra   = towerWidth/(WCBarrelNumPMTHorizontal-WCBarrelRingNPhi*WCPMTperCellHorizontal);
	  // verticalSpacing is identical to that for normal cell.

	  for(G4long i = 0; i < (WCBarrelNumPMTHorizontal-WCBarrelRingNPhi*WCPMTperCellHorizontal); i++){
		for(G4long j = 0; j < WCPMTperCellVertical; j++){
		  G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.),
													 towerWidth/2.-(i+0.5)*horizontalSpacingExtra,
													 (-(barrelCellHeight/2.)+(j+0.5)*verticalSpacing)*zflip);
		  PMTPosition.rotateZ(-(2*pi-totalAngle)/2.+barrelPhiOffset); // align with the symmetry 
		  //axes of the cell 
#ifdef ACTIVATE_IDPMTS
#ifdef WCSIMCONSTRUCTCYLINDER_VERBOSE
		  G4cout << "Add PMTs in extra tower, cell " << i << ", " << j << G4endl;
#endif
		  //G4VPhysicalVolume* physiWCBarrelBorderPMT =
		  new G4PVPlacement(WCExtraPMTRotation,                          // its rotation
							PMTPosition,
							(i==j && hybrid && WCPMTPercentCoverage2!=0)?logicWCPMT2:logicWCPMT,                 // its logical volume
							pmtname,             // its name
							logicWCExtraBorderCell,         // its mother volume
							false,                     // no boolean operations
							i*WCPMTperCellVertical+j,
							checkOverlapsPMT);
#endif

		  // logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
		  // daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
		  // this is still the case.
		}
	  }
  
	}

  }//end if placeBorderPMTs

  // # -------------------------------------- #
  // ##########################################
  // # Prototype Outer-Detector OD Hyper-K HK #
  // ##########################################
  // # -------------------------------------- #

  if(isODConstructed){

    G4double sphereRadius =
	  (WCPMTODExposeHeight*WCPMTODExposeHeight+ WCPMTODRadius*WCPMTODRadius)/(2*WCPMTODExposeHeight);
    WCODRadius = outerAnnulusRadius - sphereRadius;

    //-------------------------------------------------------------
    // OD Tyvek Barrel side
    // ------------------------------------------------------------
    G4double annulusODTyvekRmax[2] = {WCODRadius,
                                      WCODRadius};
    G4double annulusODTyvekRmin[2] = {WCODRadius-WCODTyvekSheetThickness,
                                      WCODRadius-WCODTyvekSheetThickness};

    const G4String bbcodtname = bbcname +
        G4String("ODTyvek"); // "WCBarrel[Top|Bot]BorderCellODTyvek"
    G4Polyhedra* solidWCBarrelBorderCellODTyvek =
        new G4Polyhedra(bbcodtname,
		        -dPhi/2., // phi start
			dPhi, //total phi
			1, //NPhi-gon
			2,
			RingZ,
			annulusODTyvekRmin,
			annulusODTyvekRmax);

    G4LogicalVolume* logicWCBarrelBorderCellODTyvek =
	  new G4LogicalVolume(solidWCBarrelBorderCellODTyvek,
			      G4Material::GetMaterial("Tyvek"),
	                      bbcodtname,
			      0,0,0);

    new G4LogicalSkinSurface(bbcodtname + G4String("WaterTySurface"),
                             logicWCBarrelBorderCellODTyvek,
                             OpWaterTySurface);

    G4VisAttributes* WCBarrelODTyvekCellVisAtt =
	  new G4VisAttributes(yellow);
    WCBarrelODTyvekCellVisAtt->SetForceWireframe(true);
    WCBarrelODTyvekCellVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown

    logicWCBarrelBorderCellODTyvek->SetVisAttributes(G4VisAttributes::Invisible);
    //// Uncomment following for TYVEK visualization
    logicWCBarrelBorderCellODTyvek->SetVisAttributes(WCBarrelODTyvekCellVisAtt);

    //G4VPhysicalVolume* physiWCBarrelBorderCellODTyvek =
	new G4PVPlacement(0,
					  G4ThreeVector(0.,0.,0.),
					  logicWCBarrelBorderCellODTyvek,
					  bbcodtname,
					  logicWCBarrelBorderCell,
					  false,
					  0,
					  checkOverlaps);


	// OD Tyvek around the ID cap
	G4VSolid* solidWCODCapTyvek = nullptr;
	G4double odCapZ[2] = {
		(-WCBlackSheetThickness-1.*mm)*zflip,
		(WCBarrelPMTOffset - (WCIDRadius-innerAnnulusRadius)) *zflip};
	if(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal){
		solidWCODCapTyvek
		= new G4Polyhedra(capname + G4String("Tyvek"),
							barrelPhiOffset, // phi start
							totalAngle, //phi end
							WCBarrelRingNPhi, //NPhi-gon
							2, // 2 z-planes
							odCapZ, //position of the Z planes
							annulusODTyvekRmin, // min radius at the z planes
							annulusODTyvekRmax// max radius at the Z planes
							);
	} else {
		// if there is an extra tower, the cap volume is a union of
		// to polyhedra. We have to unite both parts, because there are 
		// PMTs that are on the border between both parts.
		G4Polyhedra* mainPart 
		= new G4Polyhedra(capname + G4String("TyvekMainPart"),
							barrelPhiOffset, // phi start
							totalAngle, //phi end
							WCBarrelRingNPhi, //NPhi-gon
							2, // 2 z-planes
							odCapZ, //position of the Z planes
							annulusODTyvekRmin, // min radius at the z planes
							annulusODTyvekRmax// max radius at the Z planes
							);
		G4double extraCapRmin[2]; 
		G4double extraCapRmax[2]; 
		for(int i = 0; i < 2 ; i++){
			extraCapRmin[i] = annulusODTyvekRmin[i] != 0. ? annulusODTyvekRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
			extraCapRmax[i] = annulusODTyvekRmax[i] != 0. ? annulusODTyvekRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
		}
		G4Polyhedra* extraSlice 
		= new G4Polyhedra(capname + G4String("TyvekExtraSlice"),
							totalAngle-2.*pi+barrelPhiOffset, // phi start
							2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //total phi 
							// fortunately there are no PMTs an the gap!
							1, //NPhi-gon
							2, //  z-planes
							odCapZ, //position of the Z planes
							extraCapRmin, // min radius at the z planes
							extraCapRmax// max radius at the Z planes
							);
		solidWCODCapTyvek =
		new G4UnionSolid(capname + G4String("Tyvek"), mainPart, extraSlice);
	}

	G4LogicalVolume* logicWCODCapTyvek = 
		new G4LogicalVolume(solidWCODCapTyvek,
							G4Material::GetMaterial("Tyvek"),
							capname + G4String("TyvekPolygon"),
							0,0,0);

		new G4PVPlacement(0,                           // no rotation
						G4ThreeVector(0.,0.,0.),     // its position
						logicWCODCapTyvek,          // its logical volume
						capname + G4String("Tyvek"),             // its name
						logicWCCap,                  // its mother volume
						false,                       // no boolean operations
						0,                          // Copy #
						checkOverlaps);
	
	new G4LogicalSkinSurface(capname + G4String("WaterTySurface"),
                             logicWCODCapTyvek,
                             OpWaterTySurface);

	logicWCODCapTyvek->SetVisAttributes(G4VisAttributes::Invisible);
    //// Uncomment following for TYVEK visualization
    logicWCODCapTyvek->SetVisAttributes(WCBarrelODTyvekCellVisAtt);

    //-------------------------------------------------------------
    // OD BARREL PMTs
    // ------------------------------------------------------------

    ///////////////   Barrel PMT placement
    G4RotationMatrix* WCPMTODRotation = new G4RotationMatrix;
    WCPMTODRotation->rotateY(270.*deg);

    const G4double barrelODCellWidth   = 2.*WCODRadius*tan(dPhi/2.);
    const G4double barrelODCellHeight  = barrelCellHeight * (barrelODCellWidth/barrelCellWidth);
    G4double horizontalODSpacing = barrelODCellWidth/WCPMTODperCellHorizontal;
    const G4double verticalODSpacing   = barrelODCellHeight / WCPMTODperCellVertical;

    for(G4long i = 0; i < WCPMTODperCellHorizontal; i++){
      for(G4long j = 0; j < WCPMTODperCellVertical; j++){
        G4ThreeVector Container =  G4ThreeVector(WCODRadius,
                                                 -barrelODCellWidth/2.+(i+0.5)*horizontalODSpacing+((G4int)(std::pow(-1,j))*(G4int)(WCODPMTShift)/2),
                                                 (-(barrelCellHeight * (barrelODCellWidth/barrelCellWidth))/2.+(j+0.5)*verticalODSpacing)*zflip);

		G4cout << "Adding OD PMT in barrel in cell" << i << ", " << j << G4endl;
        //G4VPhysicalVolume* physiWCBarrelWLSPlate =
		new G4PVPlacement(WCPMTODRotation,           // its rotation
						  Container,
						  logicWCODWLSAndPMT,         // its logical volume
						  "WCBorderCellODContainer",  // its name
						  logicWCBarrelBorderCell,         // its mother volume
						  false,                     // no boolean operations
						  i*WCPMTODperCellVertical+j,
						  checkOverlapsPMT);



      }
    }

    if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){

      // TYVEK
      G4double towerODTyvekRmin[2];
      G4double towerODTyvekRmax[2];
      for(int i = 0; i < 2; i++){
        towerODTyvekRmin[i] = annulusODTyvekRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
        towerODTyvekRmax[i] = annulusODTyvekRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
      }
      const G4String etbcodtname = etbcname +
        G4String("ODTyvek"); // "WCExtraTower[Top|Bot]]BorderCellODTyvek"
      G4Polyhedra* solidWCExtraBorderCellODTyvek =
        new G4Polyhedra(etbcodtname,
                        totalAngle-2.*pi+barrelPhiOffset,//+dPhi/2., // phi start
                        2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //phi end
                        1, //NPhi-gon
                        2,
                        RingZ,
                        towerODTyvekRmin,
                        towerODTyvekRmax);

      G4LogicalVolume* logicWCExtraBorderCellODTyvek =
	new G4LogicalVolume(solidWCExtraBorderCellODTyvek,
                            G4Material::GetMaterial("Tyvek"),
                            etbcodtname,
                            0,0,0);

      new G4LogicalSkinSurface(etbcodtname + G4String("WaterTySurface"),
                               logicWCExtraBorderCellODTyvek,
                               OpWaterTySurface);

      logicWCExtraBorderCellODTyvek->SetVisAttributes(G4VisAttributes::Invisible);
      //// Uncomment following for TYVEK visualization
      logicWCExtraBorderCellODTyvek->SetVisAttributes(WCBarrelODTyvekCellVisAtt);

      //G4VPhysicalVolume* physiWCExtraBorderODTyvek =
	  new G4PVPlacement(0,
						G4ThreeVector(0.,0.,0.),
						logicWCExtraBorderCellODTyvek,
						etbcodtname,
						logicWCExtraBorderCell,
						false,
						0,
						checkOverlaps);

      // barrelODCellWidth and barrelODCellHeight are identical to those for normal cell

      G4RotationMatrix* WCExtraODPMTRotation = new G4RotationMatrix;
      WCExtraODPMTRotation->rotateY(270.*deg);
      WCExtraODPMTRotation->rotateX(2*pi - (2*pi-totalAngle)/2.+barrelPhiOffset);//align the PMT with the Cell

      G4double towerWidthOD = WCODRadius*tan(2*pi-totalAngle);
      // We don't want the same number of OD PMTs squished horizontally so we scale down the horizontal PMTs by the width of the extra tower
      G4double ratioOfWidths = (double)(WCPMTODperCellHorizontal)*(towerWidthOD/barrelODCellWidth);
      G4long WCPMTODperCellHorizontalExtra = std::lround(ratioOfWidths);
      G4double horizontalODSpacingExtra   = towerWidthOD/(double)WCPMTODperCellHorizontalExtra;
      // verticalODSpacing is identical to that of a normal cell


      for(G4long i = 0; i < (WCPMTODperCellHorizontalExtra); i++){
        for(G4long j = 0; j < WCPMTODperCellVertical; j++){
          G4ThreeVector Container =  G4ThreeVector((WCODRadius)/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.),
												   -towerWidthOD/2.+(i+0.5)*horizontalODSpacingExtra,
												   (-(barrelCellHeight * (WCODRadius/WCIDRadius))/2.+(j+0.5)*verticalODSpacing)*zflip);
          Container.rotateZ(-(2*pi-totalAngle)/2.+barrelPhiOffset); // align with the symmetry

		  G4cout << "Adding OD PMT in extra tower in cell" << i << ", " << j << G4endl;
          //G4VPhysicalVolume* physiWCBarrelPMT =
		  new G4PVPlacement(WCExtraODPMTRotation,             // its rotation
							Container,
							logicWCODWLSAndPMT,                // its logical volume
							"WCExtraBorderCellODContainer",             // its name
							logicWCExtraBorderCell,         // its mother volume
							false,                     // no boolean operations
							i*WCPMTODperCellVertical+j,
							checkOverlapsPMT);

        }
      }

    }

  } // END isODConstructed

  return logicCapAssembly;

}
// This function finds the factors of a number and aims to minimise the difference between multiplied factors. This will result in a more square-like distribution of PMTs per cell.
// ToDo:  Change the implementation to remove the recursion.  It looks like it can run away.
void FindNiceFactors(G4int NPMT, G4double factors[2]){
  bool found = false;
  int difference = 100000;
  for (int i = 2; i<= (int)(NPMT/2); i++){
    if (NPMT % i == 0){
      // Factors found
      found = true;
      double a = i;
      double b = NPMT/i;
      int d = abs(a - b);
      if (d < difference){difference = d; factors[0] = a; factors[1] = b;} // make the difference between multiplied factors as small as possible, for a square rather than rectangle shape.
    }
  }
  if (!found){FindNiceFactors(NPMT-1, factors);}
}
void ComputeWCODPMT(const G4int NPMT, G4long& NPMTHorizontal, G4long& NPMTVertical){
  switch (NPMT) {
  case 0:
	G4cout << "N PMTs for OD is equal 0, increase coverage" << G4endl;
	NPMTHorizontal = 1;
	NPMTVertical   = 1;
	break;
  case 1:
	NPMTHorizontal = 1;
	NPMTVertical   = 1;
	break;
  case 2:
	NPMTHorizontal = 1;
	NPMTVertical   = 2;
	break;
  case 3:
	NPMTHorizontal = 2;
	NPMTVertical   = 2;
	break;
  case 4:
	NPMTHorizontal = 2;
	NPMTVertical   = 2;
	break;
  default:
	G4double factors[2] = {1, 1};
	FindNiceFactors(NPMT, factors);
	NPMTHorizontal = factors[1];
	NPMTVertical   = factors[0];

	break;
  }
}

G4LogicalVolume* WCSimDetectorConstruction::ConstructCylinderNoReplica()
{
  G4cout << "**** Building Cylindrical Detector without using replica ****" << G4endl;
  ReadGeometryTableFromFile();

  //-----------------------------------------------------
  // Positions
  //-----------------------------------------------------

  debugMode = false;
  
  WCPosition=0.;//Set the WC tube offset to zero

  WCIDRadius = WCIDDiameter/2.;
  // the number of regular cell in phi direction:
  WCBarrelRingNPhi     = WCBarrelNumPMTHorizontal/WCPMTperCellHorizontal; 
  // the part of one ring, that is covered by the regular cells: 
  totalAngle  = 2.0*pi*rad*WCBarrelRingNPhi*WCPMTperCellHorizontal/WCBarrelNumPMTHorizontal;
  // angle per regular cell:
  dPhi        =  totalAngle/ WCBarrelRingNPhi;

  if(rotateBarrelHalfTower)
    barrelPhiOffset = -dPhi/2; // For WCTE, the barrel is rotated by half a tower for correct alignment with endcaps
  else
    barrelPhiOffset = 0.;

  // it's height:
  barrelCellHeight  = (WCIDHeight-2.*WCBarrelPMTOffset)/WCBarrelNRings;
  // the height of all regular cells together:
  mainAnnulusHeight = WCIDHeight -2.*WCBarrelPMTOffset -2.*barrelCellHeight;
  
  //TF: has to change for mPMT vessel:

  G4cout << WCIDRadius << ", " << WCPMTExposeHeight << ", " << mPMT_vessel_cyl_height << ", " << mPMT_vessel_radius << G4endl;
#ifdef DEBUG
  G4cout << "HYBRID = " << hybrid << G4endl;
#endif
  
  if(hybrid){
	if(mPMT_vessel_cyl_height + mPMT_vessel_radius < 1.*mm)
	  innerAnnulusRadius = WCIDRadius - std::max(WCPMTExposeHeight,WCPMTExposeHeight2)-1.*mm;
	else
	  innerAnnulusRadius = WCIDRadius - std::max(WCPMTExposeHeight,(mPMT_vessel_cyl_height + mPMT_vessel_radius)) -1.*mm;
  }
  else{
	if(mPMT_vessel_cyl_height + mPMT_vessel_radius < 1.*mm)
	  innerAnnulusRadius = WCIDRadius - WCPMTExposeHeight -1.*mm;
	else
	  innerAnnulusRadius = WCIDRadius - (mPMT_vessel_cyl_height + mPMT_vessel_radius) -1.*mm;
  }

  //TF: need to add a Polyhedra on the other side of the outerAnnulusRadius for the OD
  outerAnnulusRadius = WCIDRadius + WCBlackSheetThickness + 1.*mm;//+ Stealstructure etc.
  if(isODConstructed){
    const G4double sphereRadius =
	  (WCPMTODExposeHeight*WCPMTODExposeHeight+ WCPMTODRadius*WCPMTODRadius)/(2*WCPMTODExposeHeight);
    WCODRadius = 
	  WCIDRadius + WCBlackSheetThickness + WCODDeadSpace + // ID Structure
	  WCODTyvekSheetThickness;  // Tyvek attached to structure
    outerAnnulusRadius = WCODRadius + sphereRadius;
  }
#ifdef WCSIMCONSTRUCTCYLINDER_VERBOSE
  G4cout
	<< "GEOMCHECK3 WCIDRadius \t" << WCIDRadius << G4endl
	<< "GEOMCHECK3 WCBlackSheetThickness \t" << WCBlackSheetThickness << G4endl
	<< "GEOMCHECK3 WCODDeadSpace \t" << WCODDeadSpace << G4endl
	<< "GEOMCHECK3 WCODTyvekSheetThickness \t" << WCODTyvekSheetThickness << G4endl
	<< "GEOMCHECK3 sphereRadius \t" << outerAnnulusRadius - WCODRadius << G4endl
	<< "GEOMCHECK3 WCPMTODExposeHeight \t" << WCPMTODExposeHeight << G4endl
	<< "GEOMCHECK3 WCPMTODExposeHeight \t" << WCPMTODExposeHeight << G4endl
	<< "GEOMCHECK3 WCPMTODRadius \t" << WCPMTODRadius << G4endl
	<< "GEOMCHECK3 WCPMTODRadius \t" << WCPMTODRadius << G4endl
	<< "GEOMCHECK3 WCPMTODExposeHeight \t" << WCPMTODExposeHeight << G4endl
	;
#endif

  // the radii are measured to the center of the surfaces
  // (tangent distance). Thus distances between the corner and the center are bigger.
  //BQ: Updated with new HK OD size (2020/12/06). Simply assume no tyvek thickness or dead space.
  WCLength    = WCIDHeight + 2*(WCODHeightWaterDepth + WCBlackSheetThickness + WCODDeadSpace + WCODTyvekSheetThickness + 1*mm);
  WCRadius    = (outerAnnulusRadius + WCODLateralWaterDepth)/cos(dPhi/2.) ;
#ifdef WCSIMCONSTRUCTCYLINDER_VERBOSE
  G4cout
	<< "GEOMCHECK2 WCLength \t" << WCLength << G4endl
	<< "GEOMCHECK2 WCIDHeight \t" << WCIDHeight << G4endl
	<< "GEOMCHECK2 WCODHeightWaterDepth \t" << WCODHeightWaterDepth << G4endl
	<< "GEOMCHECK2 WCBlackSheetThickness \t" << WCBlackSheetThickness << G4endl
	<< "GEOMCHECK2 WCODDeadSpace \t" << WCODDeadSpace << G4endl
	<< "GEOMCHECK2 WCODTyvekSheetThickness \t" << WCODTyvekSheetThickness << G4endl
	<< "GEOMCHECK2 WCRadius \t" << WCRadius << G4endl
	<< "GEOMCHECK2 outerAnnulusRadius \t" << outerAnnulusRadius << G4endl
	<< "GEOMCHECK2 WCODLateralWaterDepth \t" << WCODLateralWaterDepth << G4endl
	;
#endif
  //WCLength    = WCIDHeight + (WCODHeight+WCOuterStructure)*2;	//jl145 - reflects top veto blueprint, cf. Farshid Feyzi
  //WCRadius    = (WCIDDiameter/2. + WCBlackSheetThickness + (WCODRadius+WCODHeight)))/cos(dPhi/2.) ; // BQ: Updated with new HK OD size (2020/12/06) 
  //WCLength    = WCIDHeight + 2*2.3*m;	//jl145 - reflects top veto blueprint, cf. Farshid Feyzi
  //WCRadius    = (WCIDDiameter/2. + WCBlackSheetThickness + 1.5*m)/cos(dPhi/2.) ; // TODO: OD
 
  // now we know the extend of the detector and are able to tune the tolerance
  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(WCLength > WCRadius ? WCLength : WCRadius);
  G4cout << "Computed tolerance = "
         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
         << " mm" << G4endl;

  //Decide if adding Gd
  water = "Water";
  if (WCAddGd)
  {
	  water = "Doped Water";
	  if(!G4Material::GetMaterial("Doped Water", false)) AddDopedWater();
  }

  // Optionally place parts of the detector. Very useful for visualization and debugging 
  // geometry overlaps in detail.
  placeBarrelPMTs = true;
  placeCapPMTs = true;
  placeBorderPMTs = true; 


  //-----------------------------------------------------
  // Volumes
  //-----------------------------------------------------
  // The water barrel is placed in an tubs of air
  
  G4Tubs* solidWC = new G4Tubs("WC",
			       0.0*m,
			       WCRadius+2.*m, 
			       .5*WCLength+4.2*m,	//jl145 - per blueprint
			       0.*deg,
			       360.*deg);
  
  G4LogicalVolume* logicWC = 
    new G4LogicalVolume(solidWC,
			G4Material::GetMaterial("Air"),
			"WC",
			0,0,0);
 
 
   G4VisAttributes* showColor = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
   logicWC->SetVisAttributes(showColor);

   logicWC->SetVisAttributes(G4VisAttributes::Invisible); //amb79
  
  //-----------------------------------------------------
  // everything else is contained in this water tubs
  //-----------------------------------------------------
  G4Tubs* solidWCBarrel = new G4Tubs("WCBarrel",
				     0.0*m,
				     WCRadius+1.*m, // add a bit of extra space
				     .5*WCLength,  //jl145 - per blueprint
				     0.*deg,
				     360.*deg);
  
  G4LogicalVolume* logicWCBarrel = 
    new G4LogicalVolume(solidWCBarrel,
			G4Material::GetMaterial(water),
			"WCBarrel",
			0,0,0);

    //G4VPhysicalVolume* physiWCBarrel = 
    new G4PVPlacement(0,
		      G4ThreeVector(0.,0.,0.),
		      logicWCBarrel,
		      "WCBarrel",
		      logicWC,
		      false,
			  0,
			  checkOverlaps); 

  if(isODConstructed) {
    //-----------------------------------------------------
    // Cylinder wall's tyvek
    //-----------------------------------------------------

    G4Tubs *solidCaveTyvek = new G4Tubs("WC",
                                        WCRadius,
                                        WCRadius + WCODTyvekSheetThickness,
                                        .5 * WCLength,  //jl145 - per blueprint
                                        0. * deg,
                                        360. * deg);

    G4LogicalVolume *logicCaveTyvek =
	  new G4LogicalVolume(solidCaveTyvek,
						  G4Material::GetMaterial("Tyvek"),
						  "CaveTyvek",
						  0, 0, 0);

    //G4VPhysicalVolume *physiCaveTyvek =
    new G4PVPlacement(0,
              G4ThreeVector(0., 0., 0.),
              logicCaveTyvek,
              "CaveBarrelTyvek",
              logicWCBarrel,
              false,
              checkOverlaps);

    //G4LogicalSkinSurface *TyvekCaveBarrelSurface =
    new G4LogicalSkinSurface("TyvekCaveBarrelSurface", logicCaveTyvek, OpWaterTySurface);

    G4VisAttributes *showTyvekCave = new G4VisAttributes(green);
    showTyvekCave->SetForceWireframe(true);// This line is used to give definition to the rings in OGLSX Visualizer
    logicCaveTyvek->SetVisAttributes(showTyvekCave);
    //logicCaveTyvek->SetVisAttributes(G4VisAttributes::Invisible); //amb79

    //-----------------------------------------------------
    // Cylinder caps' tyvek
    //-----------------------------------------------------

    G4Tubs *solidCaveCapsTyvek = new G4Tubs("CaveCapsTyvek",
                                            0,
                                            WCRadius,
                                            .5 * (WCODTyvekSheetThickness),
                                            0. * deg,
                                            360. * deg);

    G4LogicalVolume *logicCaveCapsTyvek =
	  new G4LogicalVolume(solidCaveCapsTyvek,
						  G4Material::GetMaterial("Tyvek"),
						  "CaveCapTyvek",
						  0, 0, 0);

    //G4LogicalSkinSurface *TyvekCaveTopSurface =
	  new G4LogicalSkinSurface("TyvekCaveTopSurface", logicCaveCapsTyvek, OpWaterTySurface);

    G4VisAttributes *CapsCaveTyvekVisAtt = new G4VisAttributes(yellow);
    CapsCaveTyvekVisAtt->SetForceWireframe(true);
    logicCaveCapsTyvek->SetVisAttributes(CapsCaveTyvekVisAtt);
    //logicCaveCapsTyvek->SetVisAttributes(G4VisAttributes::Invisible); //amb79

    G4ThreeVector CaveTyvekPosition(0., 0., WCLength / 2);

    //G4VPhysicalVolume *physiTopCaveTyvek =
	  new G4PVPlacement(0,
					  CaveTyvekPosition,
					  logicCaveCapsTyvek,
					  "CaveTopTyvek",
					  logicWCBarrel,
					  false,
					  checkOverlaps);


    CaveTyvekPosition.setZ(-CaveTyvekPosition.getZ());

    //G4VPhysicalVolume *physiBottomCaveTyvek =
	  new G4PVPlacement(0,
					  CaveTyvekPosition,
					  logicCaveCapsTyvek,
					  "CaveBottomTyvek",
					  logicWCBarrel,
					  false,
					  checkOverlaps);


  } // END Tyvek cave
  //-----------------------------------------------------

  // This volume needs to made invisible to view the blacksheet and PMTs with RayTracer
  if (Vis_Choice == "RayTracer")
   {logicWCBarrel->SetVisAttributes(G4VisAttributes::Invisible);} 

  else
   {//{if(!debugMode)
    G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4VisAttributes::Invisible);
	  tmpVisAtt->SetForceWireframe(true);// This line is used to give definition to the rings in OGLSX Visualizer
	  logicWCBarrel->SetVisAttributes(tmpVisAtt);
   }
	
  //-----------------------------------------------------
  // Form annular section of barrel to hold PMTs 
  //----------------------------------------------------

  const int nLayers = 100; // just a large number to avoid compile warning
  // Divide into WCBarrelNRings-2 layers
  G4double mainAnnulusZ[nLayers];
  G4double mainAnnulusRmin[nLayers];
  G4double mainAnnulusRmax[nLayers];
  for (int i=0;i<(G4int)WCBarrelNRings-1;i++)
  {
    mainAnnulusZ[i] =  -mainAnnulusHeight/2 + mainAnnulusHeight*i/((G4int)WCBarrelNRings-2.);
    G4double dr = GetRadiusChange(mainAnnulusZ[i]); // radius change at end-point
    mainAnnulusRmin[i] = innerAnnulusRadius+dr;
    mainAnnulusRmax[i] = outerAnnulusRadius+dr;
  }

  G4Polyhedra* solidWCBarrelAnnulus = new G4Polyhedra("WCBarrelAnnulus",
                                                   barrelPhiOffset, // phi start
                                                   totalAngle, 
                                                   WCBarrelRingNPhi, //NPhi-gon
                                                   WCBarrelNRings-1,
                                                   mainAnnulusZ,
                                                   mainAnnulusRmin,
                                                   mainAnnulusRmax);
  
  G4LogicalVolume* logicWCBarrelAnnulus = 
    new G4LogicalVolume(solidWCBarrelAnnulus,
			G4Material::GetMaterial(water),
			"WCBarrelAnnulus",
			0,0,0);
  // G4cout << *solidWCBarrelAnnulus << G4endl; 
  G4VPhysicalVolume* physiWCBarrelAnnulus = 
    new G4PVPlacement(0,
		      G4ThreeVector(0.,0.,0.),
		      logicWCBarrelAnnulus,
		      "WCBarrelAnnulus",
		      logicWCBarrel,
		      false,
		      0,
			  checkOverlaps);
  if(!debugMode)
  {
    G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(0,0.5,1.));
	  tmpVisAtt->SetForceWireframe(true);// This line is used to give definition to the rings in OGLSX Visualizer
	  logicWCBarrelAnnulus->SetVisAttributes(tmpVisAtt);
	  //If you want the rings on the Annulus to show in OGLSX, then comment out the line below.
	  logicWCBarrelAnnulus->SetVisAttributes(G4VisAttributes::Invisible);
  }
  else {
    G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(0,0.5,1.));
    tmpVisAtt->SetForceWireframe(true);
    logicWCBarrelAnnulus->SetVisAttributes(tmpVisAtt);
  }

  //-----------------------------------------------------------
  // The Blacksheet, a daughter of the cells containing PMTs,
  // and also some other volumes to make the edges light tight
  //-----------------------------------------------------------

  //-------------------------------------------------------------
  // add barrel blacksheet to the normal barrel cells 
  // ------------------------------------------------------------
  G4double annulusBlackSheetRmax[nLayers];
  G4double annulusBlackSheetRmin[nLayers];
  for (int i=0;i<(G4int)WCBarrelNRings-1;i++)
  {
    G4double dr = GetRadiusChange(mainAnnulusZ[i]);
    annulusBlackSheetRmin[i] = WCIDRadius+dr;
    annulusBlackSheetRmax[i] = WCIDRadius+WCBlackSheetThickness+dr;
  }
  G4cout << WCIDRadius << ", " << WCBlackSheetThickness << ", " << barrelCellHeight/2. << G4endl;
  G4cout << WCIDHeight << ", " << WCBarrelPMTOffset << ", " << WCBarrelNRings << G4endl;
  G4cout << dPhi << ", " << G4endl;
  for (int i=0;i<(G4int)WCBarrelNRings-1;i++)
  {
    G4cout << mainAnnulusRmin[i] << ", " << mainAnnulusRmax[i] << ", " << mainAnnulusZ[i] << G4endl;
  }

  G4Polyhedra* solidWCBarrelBlackSheet = new G4Polyhedra("WCBarrelAnnulusBlackSheet",
                                                   barrelPhiOffset, // phi start
                                                   totalAngle, //total phi
                                                   WCBarrelRingNPhi, //NPhi-gon
                                                   WCBarrelNRings-1,
                                                   mainAnnulusZ,
                                                   annulusBlackSheetRmin,
                                                   annulusBlackSheetRmax);

  G4LogicalVolume* logicWCBarrelAnnulusBlackSheet =
    new G4LogicalVolume(solidWCBarrelBlackSheet,
                        G4Material::GetMaterial("Blacksheet"),
                        "WCBarrelAnnulusBlackSheet",
                          0,0,0);

   G4VPhysicalVolume* physiWCBarrelAnnulusBlackSheet =
    new G4PVPlacement(0,
                      G4ThreeVector(0.,0.,0.),
                      logicWCBarrelAnnulusBlackSheet,
                      "WCBarrelAnnulusBlackSheet",
                      logicWCBarrelAnnulus,
                      false,
                      0,
                      checkOverlaps);

   
   //G4LogicalBorderSurface * WaterBSBarrelSurface =
	  new G4LogicalBorderSurface("WaterBSBarrelAnnulusSurface",
								  physiWCBarrelAnnulus,
								  physiWCBarrelAnnulusBlackSheet, 
								  OpWaterBSSurface);
   
   new G4LogicalSkinSurface("BSBarrelAnnulusSkinSurface",logicWCBarrelAnnulusBlackSheet,
							BSSkinSurface);

  // Change made here to have the if statement contain the !debugmode to be consistent
  // This code gives the Blacksheet its color. 

  if (Vis_Choice == "RayTracer"){

    G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
      = new G4VisAttributes(G4Colour(0.2,0.9,0.2)); // green color
    WCBarrelBlackSheetCellVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
	  WCBarrelBlackSheetCellVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown
    if(!debugMode)
    {
      logicWCBarrelAnnulusBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
    }
    else
    {
      logicWCBarrelAnnulusBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }

  else {

    G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
      = new G4VisAttributes(G4Colour(0.2,0.9,0.2));
    if(!debugMode)
    {
      logicWCBarrelAnnulusBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);
    }
    else
    {
      logicWCBarrelAnnulusBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
    }
  }

  //-----------------------------------------------------------
  // add extra tower if necessary
  // ---------------------------------------------------------
  G4LogicalVolume* logicWCExtraTower = nullptr;
  G4double towerBSRmin[nLayers];
  G4double towerBSRmax[nLayers];
  if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){
    G4cout << "%%%%% ADDING EXTRA TOWER" << G4endl;

	  G4cerr << "The extra tower is not correctly implemented. It causes geometry overlaps," << G4endl
		   << " which lead to killed tracks and the incorrect number of hits." << G4endl
		   << "You are strongly recommended to use $WCSIMDIR/sample-root-scripts/calcPhotoCoverage.C" << G4endl
		   << " in order to produce a photocoverage that will give you a valid geometry." << G4endl
		   << "Also be warned that, due to how PMTs are laid out in WCSim, you should be careful" << G4endl
		   << " when using a new geometry, especially if it is a HK hybrid geometry with" << G4endl
		   << " a low number of of mPMTs." << G4endl
		   << "You should check that all ID PMTs are laid out in a sensible pattern for your needs," << G4endl
		   << " and that the number of hits produced is sensible." << G4endl;

    // as the angles between the corners of the main annulus 
    // and the corners extra tower are different, we need to adjust the 
    // tangent distance the surfaces of the extra tower. Otherwise
    // the corners of the main annulus and the extra tower would 
    // not match. 
    G4double extraTowerRmin[nLayers];
    G4double extraTowerRmax[nLayers];
    for(int i = 0; i < (G4int)WCBarrelNRings-1 ; i++){
      extraTowerRmin[i] = mainAnnulusRmin[i] != 0 ? mainAnnulusRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
      extraTowerRmax[i] = mainAnnulusRmax[i] != 0 ? mainAnnulusRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
    }
    G4Polyhedra* solidWCExtraTower = new G4Polyhedra("WCextraTower",
  			totalAngle-2.*pi+barrelPhiOffset,//+dPhi/2., // phi start
			  2.*pi -  totalAngle // total angle.
			  -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m),
        // we need this little Gap between the extra tower and the main annulus
        // to avoid a shared surface. Without the gap the photons stuck
        // at this place for mare than 25 steps and the howl simulation
        // crashes.
	      1, //NPhi-gon
		    WCBarrelNRings-1,
			  mainAnnulusZ,
			  extraTowerRmin,
		    extraTowerRmax);

    logicWCExtraTower = 
      new G4LogicalVolume(solidWCExtraTower,
			  G4Material::GetMaterial(water),
			  "WCExtraTower",
			  0,0,0);
    G4VPhysicalVolume* physiWCExtraTower = 
      new G4PVPlacement(0,
			G4ThreeVector(0.,0.,0.),
			logicWCExtraTower,
			"WCExtraTower",
			logicWCBarrel,
			false,
			0,
			checkOverlaps);

    logicWCExtraTower->SetVisAttributes(G4VisAttributes::Invisible);

    //---------------------------------------------
    // add blacksheet to this cells
    //--------------------------------------------

    for(int i = 0; i < (G4int)WCBarrelNRings-1; i++){
      towerBSRmin[i] = annulusBlackSheetRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
      towerBSRmax[i] = annulusBlackSheetRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
    }
    G4Polyhedra* solidWCTowerBlackSheet = new G4Polyhedra("WCExtraTowerBlackSheet",
			   totalAngle-2.*pi+barrelPhiOffset,//+dPhi/2., // phi start
			   2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //phi end
		     1, //NPhi-gon
			   WCBarrelNRings-1,
			   mainAnnulusZ,
			   towerBSRmin,
			   towerBSRmax);
    //G4cout << * solidWCTowerBlackSheet << G4endl;
    G4LogicalVolume* logicWCTowerBlackSheet =
      new G4LogicalVolume(solidWCTowerBlackSheet,
			  G4Material::GetMaterial("Blacksheet"),
			  "WCExtraTowerBlackSheet",
			    0,0,0);

    G4VPhysicalVolume* physiWCTowerBlackSheet =
      new G4PVPlacement(0,
			G4ThreeVector(0.,0.,0.),
			logicWCTowerBlackSheet,
			"WCExtraTowerBlackSheet",
			logicWCExtraTower,
			false,
			0,
			checkOverlaps);


    //G4LogicalBorderSurface * WaterBSTowerCellSurface = 
    new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
                    physiWCExtraTower,
                    physiWCTowerBlackSheet, 
                    OpWaterBSSurface);

    new G4LogicalSkinSurface("BSTowerSkinSurface",logicWCTowerBlackSheet,
                  BSSkinSurface);

    // These lines add color to the blacksheet in the extratower. If using RayTracer, comment the first chunk and use the second. The Blacksheet should be green.
  
    if (Vis_Choice == "RayTracer"){
    
      G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
        = new G4VisAttributes(G4Colour(0.2,0.9,0.2)); // green color
      WCBarrelBlackSheetCellVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
      WCBarrelBlackSheetCellVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown

      if(!debugMode)
        logicWCTowerBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
      else
        logicWCTowerBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
    }

    else {

      G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
        = new G4VisAttributes(G4Colour(0.2,0.9,0.2)); // green color

      if(!debugMode)
        {logicWCTowerBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);}
      else
        {logicWCTowerBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);}
    }
  

  }

  //jl145------------------------------------------------
  // Add top veto volume
  //-----------------------------------------------------

  G4bool WCTopVeto = (WCSimTuningParams->GetTopVeto());

  G4LogicalVolume* logicWCTopVeto = nullptr;

  if(WCTopVeto){
    G4cout << "%%%%% ADDING TOP VETO" << G4endl;
	  G4double WCTyvekThickness = 1.0*mm; //completely made up

	  G4VSolid* solidWCTopVeto;
	  solidWCTopVeto =
			new G4Tubs(			"WCTopVeto",
								0.0*m,
								WCIDRadius + WCTyvekThickness + GetRadiusChange(WCIDHeight/2),
								0.5*m + WCTyvekThickness,
								0.*deg,
								360.*deg);

	  logicWCTopVeto = 
			new G4LogicalVolume(solidWCTopVeto,
								G4Material::GetMaterial(water),
								"WCTopVeto",
								0,0,0);

	  G4VPhysicalVolume* physiWCTopVeto =
			new G4PVPlacement(	0,
								G4ThreeVector(0.,0.,WCIDHeight/2
													+1.0*m),
								logicWCTopVeto,
								"WCTopVeto",
								logicWCBarrel,
								false,0,
								checkOverlaps);

	  //Add the top veto Tyvek
	  //-----------------------------------------------------

	  G4VSolid* solidWCTVTyvek;
	  solidWCTVTyvek =
			new G4Tubs(			"WCTVTyvek",
								0.0*m,
								WCIDRadius + GetRadiusChange(WCIDHeight/2),
								WCTyvekThickness/2,
								0.*deg,
								360.*deg);


	  G4LogicalVolume* logicWCTVTyvek =
			new G4LogicalVolume(solidWCTVTyvek,
								G4Material::GetMaterial("Tyvek"),
								"WCTVTyvek",
								0,0,0);

	  //Bottom
	  G4VPhysicalVolume* physiWCTVTyvekBot =
			new G4PVPlacement(	0,
		                  		G4ThreeVector(0.,0.,-0.5*m
													-WCTyvekThickness/2),
								logicWCTVTyvek,
		               			"WCTVTyvekBot",
		          				logicWCTopVeto,
				 				false,0,
								checkOverlaps);

	  //G4LogicalBorderSurface * WaterTyTVSurfaceBot =
			new G4LogicalBorderSurface(	"WaterTyTVSurfaceBot",
										physiWCTopVeto,
										physiWCTVTyvekBot,
										OpWaterTySurface);

	  //Top
	  G4VPhysicalVolume* physiWCTVTyvekTop =
			new G4PVPlacement(	0,
		                  		G4ThreeVector(0.,0.,0.5*m
													+WCTyvekThickness/2),
								logicWCTVTyvek,
		               			"WCTVTyvekTop",
		          				logicWCTopVeto,
				 				false,0,
								checkOverlaps);

	  //G4LogicalBorderSurface * WaterTyTVSurfaceTop =
	  new G4LogicalBorderSurface(	"WaterTyTVSurfaceTop",
										physiWCTopVeto,
										physiWCTVTyvekTop,
										OpWaterTySurface);

	  //Side
	  G4VSolid* solidWCTVTyvekSide;
	  solidWCTVTyvekSide =
			new G4Tubs(			"WCTVTyvekSide",
								WCIDRadius+ GetRadiusChange(WCIDHeight/2),
								WCIDRadius + WCTyvekThickness+ GetRadiusChange(WCIDHeight/2),
								0.5*m + WCTyvekThickness,
								0.*deg,
								360.*deg);


	  G4LogicalVolume* logicWCTVTyvekSide =
			new G4LogicalVolume(solidWCTVTyvekSide,
								G4Material::GetMaterial("Tyvek"),
								"WCTVTyvekSide",
								0,0,0);

	  G4VPhysicalVolume* physiWCTVTyvekSide =
			new G4PVPlacement(	0,
		                  		G4ThreeVector(0.,0.,0.),
								logicWCTVTyvekSide,
		               			"WCTVTyvekSide",
		          				logicWCTopVeto,
				 				false,0,
								checkOverlaps);

	  //G4LogicalBorderSurface * WaterTyTVSurfaceSide =
		new G4LogicalBorderSurface(	"WaterTyTVSurfaceSide",
										physiWCTopVeto,
										physiWCTVTyvekSide,
										OpWaterTySurface);
    new G4LogicalSkinSurface("WaterTyTVSurfaceSide", logicWCTVTyvekSide, OpWaterTySurface);
  }

  //
  //
  //jl145------------------------------------------------


  //////////// M Fechner : I need to  declare the PMT  here in order to
  // place the PMTs in the truncated cells
  //-----------------------------------------------------
  // The PMT
  //-----------------------------------------------------

  ////////////J Felde: The PMT logical volume is now constructed separately 
  // from any specific detector geometry so that any geometry can use the same definition. 
  // K.Zbiri: The PMT volume and the PMT glass are now put in parallel. 
  // The PMT glass is the sensitive volume in this new configuration.

  // TF: Args are set to properties of the class which is somehow global (see the ConstructDetector.hh)
  //     They are set in the WCSimDetectorConfigs and are property of the PMT.
  G4LogicalVolume* logicWCPMT;
  if(nID_PMTs<=1) logicWCPMT = ConstructPMT(WCPMTName, WCIDCollectionName,"tank",nID_PMTs);
  else logicWCPMT = ConstructMultiPMT(WCPMTName, WCIDCollectionName,"tank",nID_PMTs);
  if(!logicWCPMT){
    G4cerr << "Overlapping PMTs in multiPMT" << G4endl;
    return NULL; 
  }
  G4LogicalVolume* logicWCPMT2 = nullptr;
#ifdef DEBUG
  G4cout << "HYBRID2 = " << hybrid << G4endl;
#endif
  if(hybrid){
    G4cout<<"First type of PMT LV is created. Now creating the LV for the second type of PMT"<<G4endl;
    if(nID_PMTs2<=1) logicWCPMT2 = ConstructPMT(WCPMTName2, WCIDCollectionName2,"tankPMT2",nID_PMTs2);
    else logicWCPMT2 = ConstructMultiPMT(WCPMTName2, WCIDCollectionName2,"tankPMT2",nID_PMTs2);
    if(!logicWCPMT2){
      G4cerr << "Overlapping PMTs in multiPMT" << G4endl;
      return NULL; 
    }
  }

  //G4LogicalVolume* logicWCPMT = ConstructPMT(WCPMTName, WCIDCollectionName);
  G4String pmtname = "WCMultiPMT";


  /*These lines of code will give color and volume to the PMTs if it hasn't been set in WCSimConstructPMT.cc.
  I recommend setting them in WCSimConstructPMT.cc. 
  If used here, uncomment the SetVisAttributes(WClogic) line, and comment out the SetVisAttributes(G4VisAttributes::Invisible) line.*/
  
  G4VisAttributes* WClogic 
      = new G4VisAttributes(G4Colour(0.4,0.0,0.8));
  WClogic->SetForceSolid(true);
	WClogic->SetForceAuxEdgeVisible(true);

  //logicWCPMT->SetVisAttributes(WClogic);
	//logicWCPMT->SetVisAttributes(G4VisAttributes::Invisible);

  //jl145------------------------------------------------
  // Add top veto PMTs
  //-----------------------------------------------------

  if(WCTopVeto){

	  G4double WCTVPMTSpacing = (WCSimTuningParams->GetTVSpacing())*cm;
	  G4double WCTVEdgeLimit = WCCapEdgeLimit;
	  G4int TVNCell = WCTVEdgeLimit/WCTVPMTSpacing + 2;

	  int icopy = 0;

#ifdef DEBUG
	G4cout<<"Debug B.Q: Constructing the cells for veto"<<G4endl;
#endif
	  for ( int i = -TVNCell ; i <  TVNCell; i++) {
		for (int j = -TVNCell ; j <  TVNCell; j++)   {

      G4cout << "Adding Top Veto PMT in TV cell " << i << ", " << j << G4endl;

		  G4double xoffset = i*WCTVPMTSpacing + WCTVPMTSpacing*0.5;
		  G4double yoffset = j*WCTVPMTSpacing + WCTVPMTSpacing*0.5;

		  G4ThreeVector cellpos =
		  		G4ThreeVector(	xoffset, yoffset, -0.5*m);

		  if ((sqrt(xoffset*xoffset + yoffset*yoffset) + hybrid?std::max(WCPMTRadius,WCPMTRadius2):WCPMTRadius) < WCTVEdgeLimit) {//B.Q: for hybrid configuration

        G4int horizontalModulo = (i+TVNCell) % WCPMTperCellHorizontal;
		    G4int verticalModulo = (j+TVNCell) % WCPMTperCellVertical;

		    //G4VPhysicalVolume* physiCapPMT =
		    		new G4PVPlacement(	0,						// no rotation
		    							cellpos,				// its position
		    							(horizontalModulo == verticalModulo && hybrid && WCPMTPercentCoverage2 !=0)?logicWCPMT2:logicWCPMT,				// its logical volume
		    							pmtname,//"WCPMT",				// its name 
		    							logicWCTopVeto,			// its mother volume
		    							false,					// no boolean os
		    							icopy,					// every PMT need a unique id.
										  checkOverlapsPMT);

		    icopy++;
		  }
		}
	  }

	  G4double WCTVEfficiency = icopy*WCPMTRadius*(hybrid?std::max(WCPMTRadius,WCPMTRadius2):WCPMTRadius)/((WCIDRadius)*(WCIDRadius));
	  G4cout << "Total on top veto: " << icopy << "\n";
	  G4cout << "Coverage was calculated to be: " << WCTVEfficiency << "\n";

  }

  //
  //
  //jl145------------------------------------------------


  ///////////////   Barrel PMT placement

  if(placeBarrelPMTs){

#ifdef DEBUG
  	G4cout << "Debug B.Q : Number of PMT horizontal = " << WCPMTperCellHorizontal << ", number of PMT vertical = " << WCPMTperCellVertical << G4endl;
#endif

    G4int copyNo = 0;

    if (readFromTable) // place PMTs according to table input
    {
      for (G4int i=0; i<nPMTsRead; i++ )
      {
        if (!pmtUse[i]) continue; // skip PMT
        if (pmtSection[i]!=0) continue; // only place barrel PMT

        G4RotationMatrix* PMTRotation = new G4RotationMatrix;
        if(orientation == PERPENDICULAR)
          PMTRotation->rotateY(90.*deg); //if mPMT: perp to wall
        else if(orientation == VERTICAL)
          PMTRotation->rotateY(0.*deg); //if mPMT: vertical/aligned to wall
        else if(orientation == HORIZONTAL)
          PMTRotation->rotateX(90.*deg); //if mPMT: horizontal to wall

        G4double pmtPhi = atan2(pmtPos[i].y(),pmtPos[i].x());
        if (pmtPhi<barrelPhiOffset) pmtPhi += 2*pi;
        G4int iphi = (pmtPhi-barrelPhiOffset)/dPhi;
        G4bool inExtraTower = (iphi >= WCBarrelRingNPhi);
        G4double phi_offset = !inExtraTower ? (iphi+0.5)*dPhi+barrelPhiOffset 
                                            : barrelPhiOffset-(2*pi-totalAngle)/2.;
        PMTRotation->rotateX(-phi_offset);//align the PMT with the Cell

        // inclined barrel wall
        G4int iz = (pmtPos[i].z()+mainAnnulusHeight/2.)/barrelCellHeight;
        G4double dth = atan((annulusBlackSheetRmin[iz+1]-annulusBlackSheetRmin[iz])/(mainAnnulusZ[iz+1]-mainAnnulusZ[iz]));
        if(orientation == PERPENDICULAR)
          PMTRotation->rotateY(-dth); 
        else if(orientation == VERTICAL)
          PMTRotation->rotateY(-dth); 

        // ID radius is changed
        G4double newZ = pmtPos[i].z() + G4RandGauss::shoot(0,pmtPosVar);
        G4double newR = annulusBlackSheetRmin[iz]+(annulusBlackSheetRmin[iz+1]-annulusBlackSheetRmin[iz])*(newZ-mainAnnulusZ[iz])/(mainAnnulusZ[iz+1]-mainAnnulusZ[iz]);
        G4double newPhi = pmtPhi-phi_offset;
        G4ThreeVector PMTPosition =  G4ThreeVector(newR,
                    newR*tan(newPhi) + G4RandGauss::shoot(0,pmtPosVar),
                    newZ);
        PMTRotation->rotateZ(pmtRotaton[i]); 
        G4cout<<"Barrel PMT ID = "<<i<<", Position = "<<pmtPos[i].x()<<" "<<pmtPos[i].y()<<" "<<pmtPos[i].z()<<", Rotation = "<<pmtRotaton[i]<<G4endl;

        PMTPosition.rotateZ(phi_offset);  // align with the symmetry 
                                          //axes of the cell 

#ifdef ACTIVATE_IDPMTS
        //G4VPhysicalVolume* physiWCBarrelPMT =
        new G4PVPlacement(PMTRotation,              // its rotation
              PMTPosition, 
              (hybrid && pmtType[i]==2)?logicWCPMT2:logicWCPMT,                // its logical volume
              pmtname,//"WCPMT",             // its name
              !inExtraTower ? logicWCBarrelAnnulus : logicWCExtraTower,         // its mother volume
              false,                     // no boolean operations
              pmtmPMTId[i],
              checkOverlapsPMT);             
#endif                            
        // logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
        // daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
        // this is still the case.
        copyNo++;
      }
    }
    else // Use auto-positioning algorithm
    {
      for (int iz=0;iz<(G4int)WCBarrelNRings-2;iz++)
      {
        G4double z_offset = -mainAnnulusHeight/2.+barrelCellHeight*(iz+0.5);

        // slight different cell width at different z
        G4double barrelCellWidth = (annulusBlackSheetRmin[iz+1]+annulusBlackSheetRmin[iz])*tan(dPhi/2.);
        G4double horizontalSpacing   = barrelCellWidth/WCPMTperCellHorizontal;
        G4double verticalSpacing     = barrelCellHeight/WCPMTperCellVertical;

#ifdef DEBUG
        G4cout << "Debug : iz = " << iz <<" Horizontal spacing = "<< horizontalSpacing << " vertical spacing = " << verticalSpacing << G4endl;
#endif
        
        for (int iphi=0;iphi<(G4int)WCBarrelRingNPhi;iphi++)
        {
          G4double phi_offset = (iphi+0.5)*dPhi+barrelPhiOffset;

          for(G4double i = 0; i < WCPMTperCellHorizontal; i++){
            for(G4double j = 0; j < WCPMTperCellVertical; j++){

              G4RotationMatrix* PMTRotation = new G4RotationMatrix;
              if(orientation == PERPENDICULAR)
                PMTRotation->rotateY(90.*deg); //if mPMT: perp to wall
              else if(orientation == VERTICAL)
                PMTRotation->rotateY(0.*deg); //if mPMT: vertical/aligned to wall
              else if(orientation == HORIZONTAL)
                PMTRotation->rotateX(90.*deg); //if mPMT: horizontal to wall
              PMTRotation->rotateX(-phi_offset);//align the PMT with the Cell
              // inclined barrel wall
              G4double dth = atan((annulusBlackSheetRmin[iz+1]-annulusBlackSheetRmin[iz])/(mainAnnulusZ[iz+1]-mainAnnulusZ[iz]));
              if(orientation == PERPENDICULAR)
                PMTRotation->rotateY(-dth); 
              else if(orientation == VERTICAL)
                PMTRotation->rotateY(-dth); 

#ifdef WCSIMCONSTRUCTCYLINDER_VERBOSE
              G4cout << "Adding barrel PMT in iz = "<< iz << " iphi = " << iphi << " cell " << i << ", " << j << G4endl;
#endif	
              G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius,
                    -barrelCellWidth/2.+(i+0.5)*horizontalSpacing + G4RandGauss::shoot(0,pmtPosVar),
                    -barrelCellHeight/2.+(j+0.5)*verticalSpacing + z_offset + G4RandGauss::shoot(0,pmtPosVar));

              // ID radius is changed
              G4double newR = annulusBlackSheetRmin[iz]+(annulusBlackSheetRmin[iz+1]-annulusBlackSheetRmin[iz])*(PMTPosition.z()-mainAnnulusZ[iz])/(mainAnnulusZ[iz+1]-mainAnnulusZ[iz]);
              PMTPosition.setX(newR);

              PMTPosition.rotateZ(phi_offset);  // align with the symmetry 
                                                //axes of the cell 
#ifdef ACTIVATE_IDPMTS
              //G4VPhysicalVolume* physiWCBarrelPMT =
                new G4PVPlacement(PMTRotation,              // its rotation
                      PMTPosition, 
                      (i==j && hybrid && WCPMTPercentCoverage2!=0)?logicWCPMT2:logicWCPMT,                // its logical volume
                      pmtname,//"WCPMT",             // its name
                      logicWCBarrelAnnulus,         // its mother volume
                      false,                     // no boolean operations
                      copyNo++,
                      checkOverlapsPMT);             
#endif                            

              // logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
              // daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
              // this is still the case.

            }
          }
        }
      }

      //-------------------------------------------------------------
      // Add PMTs in extra Tower if necessary
      //------------------------------------------------------------


      if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){

#ifdef DEBUG
        G4cout << "Debug B.Q: Wish to place extra PMTs" << G4endl;
        G4cout << "Debug B.Q : WCBarrelRingNPhi = " << WCBarrelRingNPhi << ", WCBarrelNumPMTHorizontal = " << WCBarrelNumPMTHorizontal << G4endl;
#endif

        copyNo = 0;
        for (int iz=0;iz<WCBarrelNRings-2;iz++)
        {
          G4double z_offset = -mainAnnulusHeight/2.+barrelCellHeight*(iz+0.5);

          G4double towerWidth = (annulusBlackSheetRmin[iz+1]+annulusBlackSheetRmin[iz])/2.*tan(2*pi-totalAngle);
          G4double horizontalSpacingExtra   = towerWidth/(WCBarrelNumPMTHorizontal-WCBarrelRingNPhi*WCPMTperCellHorizontal);
          G4double verticalSpacing     = barrelCellHeight/WCPMTperCellVertical;


          for(G4double i = 0; i < (WCBarrelNumPMTHorizontal-WCBarrelRingNPhi*WCPMTperCellHorizontal); i++){
            for(G4double j = 0; j < WCPMTperCellVertical; j++){

              G4RotationMatrix* WCExtraPMTRotation = new G4RotationMatrix;
              if(orientation == PERPENDICULAR)
                WCExtraPMTRotation->rotateY(90.*deg); //if mPMT: perp to wall
              else if(orientation == VERTICAL)
                WCExtraPMTRotation->rotateY(0.*deg); //if mPMT: vertical/aligned to wall
              else if(orientation == HORIZONTAL)
                WCExtraPMTRotation->rotateX(90.*deg); //if mPMT: horizontal to wall
              WCExtraPMTRotation->rotateX((2*pi-totalAngle)/2.-barrelPhiOffset);//align the PMT with the Cell
              G4double dth = atan((towerBSRmin[iz+1]-towerBSRmin[iz])/(mainAnnulusZ[iz+1]-mainAnnulusZ[iz]));
              if(orientation == PERPENDICULAR)
                WCExtraPMTRotation->rotateY(-dth); 
              else if(orientation == VERTICAL)
                WCExtraPMTRotation->rotateY(-dth); 

              G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.),
                    towerWidth/2.-(i+0.5)*horizontalSpacingExtra + G4RandGauss::shoot(0,pmtPosVar),
                        -barrelCellHeight/2.+(j+0.5)*verticalSpacing+z_offset + G4RandGauss::shoot(0,pmtPosVar));

              G4double newR = towerBSRmin[iz]+(towerBSRmin[iz+1]-towerBSRmin[iz])*(PMTPosition.z()-mainAnnulusZ[iz])/(mainAnnulusZ[iz+1]-mainAnnulusZ[iz]);
              PMTPosition.setX(newR);

              PMTPosition.rotateZ(-(2*pi-totalAngle)/2.+barrelPhiOffset); // align with the symmetry axes of the cell 
#ifdef ACTIVATE_IDPMTS
              //G4VPhysicalVolume* physiWCBarrelPMT =
                new G4PVPlacement(WCExtraPMTRotation,             // its rotation
                      PMTPosition, 
                      (i==j && hybrid && WCPMTPercentCoverage2!=0)?logicWCPMT2:logicWCPMT,                // its logical volume
                      pmtname,             // its name
                      logicWCExtraTower,         // its mother volume
                      false,                     // no boolean operations
                      copyNo++,
                      checkOverlapsPMT);                       
#endif            
                // logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
                // daughter volumes to the PMTs (e.g. a acryl cover) you have to check, if
                // this is still the case.
            }
          }

        }
      }
    }
  }//end if placeBarrelPMTs

  // # -------------------------------------- #
  // ##########################################
  // # Prototype Outer-Detector OD Hyper-K HK #
  // ##########################################
  // # -------------------------------------- #

  // Goal is to create a dedicated ConstructOD() method in charge of defining an OD geometry
  // for any WC detector.
  // Parameters will be :
  // - NPMTs by Cells horizontally and vertically
  // - Coverage
  // - Dead Volume Size
  // - Water Size

  if(isODConstructed){
    //-------------------------------------------------------------
    // OD Tyvek Caps
    // ------------------------------------------------------------

    G4VSolid* solidWCODTopCapTyvek = nullptr;
    G4VSolid* solidWCODBotCapTyvek = nullptr;

    G4double odTopCapZ[4] = {
      (-WCODDeadSpace+1*mm+WCBlackSheetThickness),
      -.5*(WCODTyvekSheetThickness),
      -.5*(WCODTyvekSheetThickness),
      .5*(WCODTyvekSheetThickness)};
    G4double odBotCapZ[4] = {
      -(-WCODDeadSpace+1*mm+WCBlackSheetThickness),
      .5*(WCODTyvekSheetThickness),
      .5*(WCODTyvekSheetThickness),
      -.5*(WCODTyvekSheetThickness)};
    G4double odCapRmin[4] = {WCODRadius-WCODTyvekSheetThickness,
                WCODRadius-WCODTyvekSheetThickness,
                0,
                0};
    G4double odCapRmax[4] = {WCODRadius,WCODRadius,WCODRadius,WCODRadius};

    if(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal){
      solidWCODTopCapTyvek
      = new G4Polyhedra("WCODTopCapTyvek",
                barrelPhiOffset, // phi start
                totalAngle, //phi end
                WCBarrelRingNPhi, //NPhi-gon
                4, // 4 z-planes
                odTopCapZ, //position of the Z planes
                odCapRmin, // min radius at the z planes
                odCapRmax// max radius at the Z planes
                );
      solidWCODBotCapTyvek
      = new G4Polyhedra("WCODBotCapTyvek",
                barrelPhiOffset, // phi start
                totalAngle, //phi end
                WCBarrelRingNPhi, //NPhi-gon
                4, // 4 z-planes
                odBotCapZ, //position of the Z planes
                odCapRmin, // min radius at the z planes
                odCapRmax// max radius at the Z planes
                );
    } else {
      // if there is an extra tower, the cap volume is a union of
      // to polyhedra. We have to unite both parts, because there are 
      // PMTs that are on the border between both parts.
      G4Polyhedra* mainPartTop 
      = new G4Polyhedra("WCODTopCapTyvekMainPart",
                barrelPhiOffset, // phi start
                totalAngle, //phi end
                WCBarrelRingNPhi, //NPhi-gon
                4, // 4 z-planes
                odTopCapZ, //position of the Z planes
                odCapRmin, // min radius at the z planes
                odCapRmax// max radius at the Z planes
                );
      G4Polyhedra* mainPartBot 
      = new G4Polyhedra("WCODBotCapTyvekMainPart",
                barrelPhiOffset, // phi start
                totalAngle, //phi end
                WCBarrelRingNPhi, //NPhi-gon
                4, // 4 z-planes
                odBotCapZ, //position of the Z planes
                odCapRmin, // min radius at the z planes
                odCapRmax// max radius at the Z planes
                );
      G4double extraCapRmin[4]; 
      G4double extraCapRmax[4]; 
      for(int i = 0; i < 4 ; i++){
        extraCapRmin[i] = odCapRmin[i] != 0. ?  odCapRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
        extraCapRmax[i] = odCapRmax[i] != 0. ?  odCapRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
      }
      G4Polyhedra* extraSliceTop
      = new G4Polyhedra("WCODTopCapTyvekExtraSlice",
                totalAngle-2.*pi+barrelPhiOffset, // phi start
                2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //total phi 
                // fortunately there are no PMTs an the gap!
                1, //NPhi-gon
                4, //  z-planes
                odTopCapZ, //position of the Z planes
                extraCapRmin, // min radius at the z planes
                extraCapRmax// max radius at the Z planes
                );
      G4Polyhedra* extraSliceBot
      = new G4Polyhedra("WCODBotCapTyvekExtraSlice",
                totalAngle-2.*pi+barrelPhiOffset, // phi start
                2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //total phi 
                // fortunately there are no PMTs an the gap!
                1, //NPhi-gon
                4, //  z-planes
                odBotCapZ, //position of the Z planes
                extraCapRmin, // min radius at the z planes
                extraCapRmax// max radius at the Z planes
                );

      solidWCODTopCapTyvek =
      new G4UnionSolid("WCODTopCapTyvek", mainPartTop, extraSliceTop);

      solidWCODBotCapTyvek =
      new G4UnionSolid("WCODBotCapTyvek", mainPartBot, extraSliceBot);
    
    }

    G4LogicalVolume* logicWCODTopCapTyvek =
	  new G4LogicalVolume(solidWCODTopCapTyvek,
						  G4Material::GetMaterial("Tyvek"),
						  "WCODTopCapTyvek",
						  0,0,0);
	  G4LogicalVolume* logicWCODBotCapTyvek =
	  new G4LogicalVolume(solidWCODBotCapTyvek,
						  G4Material::GetMaterial("Tyvek"),
						  "WCODBotCapTyvek",
						  0,0,0);

    G4VisAttributes* WCCapsODTyvekCellVisAtt =
	  new G4VisAttributes(yellow);
    WCCapsODTyvekCellVisAtt->SetForceWireframe(true);

    logicWCODTopCapTyvek->SetVisAttributes(G4VisAttributes::Invisible);
	  logicWCODBotCapTyvek->SetVisAttributes(G4VisAttributes::Invisible);
    //// Uncomment following for TYVEK visualization
    logicWCODTopCapTyvek->SetVisAttributes(WCCapsODTyvekCellVisAtt);
	  logicWCODBotCapTyvek->SetVisAttributes(WCCapsODTyvekCellVisAtt);

    G4ThreeVector CapTyvekPosition(0.,0.,(WCIDHeight + 2*WCODDeadSpace)/2);

    //G4VPhysicalVolume* physiWCODTopCapsTyvek =
	  new G4PVPlacement(0,
						CapTyvekPosition,
						logicWCODTopCapTyvek,
						"WCODTopCapsTyvek",
						logicWCBarrel,
						false,
						0,
						checkOverlaps);

    //G4LogicalSkinSurface *WaterTySurfaceTop =
	  new G4LogicalSkinSurface("WaterTySurfaceTop", logicWCODTopCapTyvek, OpWaterTySurface);

    CapTyvekPosition.setZ(-CapTyvekPosition.getZ());

    //G4VPhysicalVolume* physiWCODBottomCapsTyvek =
	  new G4PVPlacement(0,
						CapTyvekPosition,
						logicWCODBotCapTyvek,
						"WCODBottomCapsTyvek",
						logicWCBarrel,
						false,
						0,
						checkOverlaps);
	  new G4LogicalSkinSurface("WaterTySurfaceBot", logicWCODBotCapTyvek, OpWaterTySurface);


    //-------------------------------------------------------------
    // OD Tyvek Barrel side
    // ------------------------------------------------------------
    G4double annulusZ[2] = {-mainAnnulusHeight/2., mainAnnulusHeight/2};
    G4double annulusODTyvekRmax[2] = {(WCODRadius),
                                      WCODRadius};
    G4double annulusODTyvekRmin[2] = {(WCODRadius-WCODTyvekSheetThickness),
                                      WCODRadius-WCODTyvekSheetThickness};

    G4Polyhedra* solidWCBarrelODTyvek =
	  new G4Polyhedra("WCBarrelODTyvek",
					  barrelPhiOffset, // phi start
					  totalAngle, //total phi
					  WCBarrelRingNPhi, //NPhi-gon
					  2,
					  annulusZ,
					  annulusODTyvekRmin,
					  annulusODTyvekRmax);

    G4LogicalVolume* logicWCBarrelODTyvek =
	  new G4LogicalVolume(solidWCBarrelODTyvek,
						  G4Material::GetMaterial("Tyvek"),
						  "WCBarrelODTyvek",
						  0,0,0);

    G4VisAttributes* WCBarrelODTyvekCellVisAtt =
	  new G4VisAttributes(yellow);
    WCBarrelODTyvekCellVisAtt->SetForceWireframe(true);
    WCBarrelODTyvekCellVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown

    logicWCBarrelODTyvek->SetVisAttributes(G4VisAttributes::Invisible);
    //// Uncomment following for TYVEK visualization
    logicWCBarrelODTyvek->SetVisAttributes(WCBarrelODTyvekCellVisAtt);


    //G4VPhysicalVolume* physiWCBarrelCellODTyvek =
	  new G4PVPlacement(0,
						G4ThreeVector(0.,0.,0.),
						logicWCBarrelODTyvek,
						"WCBarrelCellODTyvek",
						logicWCBarrelAnnulus,
						false,
						0,
						checkOverlaps);
    //G4LogicalSkinSurface *WaterTySurfaceSide =
	  new G4LogicalSkinSurface("WaterTySurfaceSide", logicWCBarrelODTyvek, OpWaterTySurface);

    //-------------------------------------------------------------
    // WLS and OD PMTs Barrel Side
    // ------------------------------------------------------------

    logicWCODWLSAndPMT = ConstructPMTAndWLSPlate(WCPMTODName, WCODCollectionName, "OD");
    // sphereRadius is the size along z of the logicWCODCapTyvek box containing WLS+PMT

    ///////////////   Barrel PMT placement
    // G4RotationMatrix* WCPMTODRotation = new G4RotationMatrix;
    // WCPMTODRotation->rotateY(270.*deg);

    G4double barrelCellWidth = 2.*WCIDRadius*tan(dPhi/2.);
    G4double barrelODCellWidth   = 2.*WCODRadius*tan(dPhi/2.);
    G4double barrelODCellHeight  = barrelCellHeight * (barrelODCellWidth/barrelCellWidth);

    // ------------------- //
    // COMPUTE OD COVERAGE //
    // ------------------- //
    G4double AreaRingOD = WCBarrelRingNPhi * barrelODCellWidth * barrelODCellHeight;
    G4double AreaCellOD = barrelODCellWidth * barrelODCellHeight;
    G4double AreaPMTOD = 3.1415*std::pow(WCPMTODRadius,2);
    G4double NPMTODCovered = (AreaRingOD/AreaPMTOD) * WCPMTODPercentCoverage/100.;
    G4double NPMTODByCellFull = NPMTODCovered/WCBarrelRingNPhi; // NPMT required par cell to achieve ODPercentOverage
    G4double NPMTODByCell = round(NPMTODCovered/WCBarrelRingNPhi); // NPMT required par cell to achieve ODPercentOverage
    G4double RealODCoverage = NPMTODByCell*AreaPMTOD/AreaCellOD;
    // ------ DEBUG ------ //
    G4cout << G4endl;
    G4cout << "AreaRingOD : " << AreaRingOD/m2 << " (m2)" << G4endl;
    G4cout << "AreaCellOD : " << AreaCellOD/m2 << " (m2)" << G4endl;
    G4cout << "AreaPMTOD : " << AreaPMTOD/m2 << " (m2)" << G4endl;
    G4cout << "--> NbPMTODCovered : " << NPMTODCovered << G4endl;
    G4cout << "--> NbPMTODByCellFull : " << NPMTODByCellFull << G4endl;
    G4cout << "--> NbPMTODByCell : " << NPMTODByCell << G4endl;
    G4cout << "--> SuggestedODCoverage : " <<  WCPMTODPercentCoverage/100. << G4endl;
    G4cout << "--> RealODCoverage : " << RealODCoverage << G4endl;
    G4cout << G4endl;
    // ------------------- //
    // The number of PMTs per cell gives a slightly different coverage so the photocoverage
    // parameter must be changed here so the endcaps will have the same photocoverage as the barrel.
    WCPMTODPercentCoverage = RealODCoverage*100;
    WCODCapPMTSpacing  = (pi*WCIDDiameter/(round(WCIDDiameter*sqrt(pi*WCPMTODPercentCoverage)/(10.0*WCPMTODRadius))));

    if(WCPMTODperCellHorizontal == 0 && WCPMTODperCellVertical == 0){
      ComputeWCODPMT((G4int)NPMTODByCell,WCPMTODperCellHorizontal,WCPMTODperCellVertical);
    }

    G4double horizontalODSpacing = barrelODCellWidth/WCPMTODperCellHorizontal;
    G4double verticalODSpacing   = barrelODCellHeight/WCPMTODperCellVertical;

    if(WCODPMTShift > barrelODCellWidth/2. - WCPMTODRadius) WCODPMTShift = 0.*cm;

    G4int odcopyNo = 0;
    for (int iz=0;iz<(G4int)WCBarrelNRings-2;iz++)
    {
      G4double z_offset = -mainAnnulusHeight/2.+barrelCellHeight*(iz+0.5);

      for (int iphi=0;iphi<WCBarrelRingNPhi;iphi++)
      {
        G4double phi_offset = (iphi+0.5)*dPhi+barrelPhiOffset;

        G4RotationMatrix* WCPMTODRotation = new G4RotationMatrix;
        WCPMTODRotation->rotateY(270.*deg);
        WCPMTODRotation->rotateX(phi_offset);//align the PMT with the Cell

        for(G4long i = 0; i < WCPMTODperCellHorizontal; i++){
          for(G4long j = 0; j < WCPMTODperCellVertical; j++){

            G4cout << "Adding OD PMT in iz = "<< iz <<" iphi = "<< iphi << " cell " << i << ", " << j << G4endl;

            G4ThreeVector Container =  G4ThreeVector(WCODRadius,
                                                    -barrelODCellWidth/2.+(i+0.5)*horizontalODSpacing+((G4int)(std::pow(-1,j))*(G4int)(WCODPMTShift)/2),
                                                    -(barrelCellHeight * (barrelODCellWidth/barrelCellWidth))/2.+(j+0.5)*verticalODSpacing + z_offset);

            Container.rotateZ(phi_offset);  // align with the symmetry axes of the cell 

        //		G4cout << " qqqqqqqqqqqqqqqqqqqqqqqq barrel i " << i << " of " << WCPMTODperCellHorizontal << " j " << j << " of " << WCPMTODperCellVertical << " Container (" << Container.x() << ", " << Container.y()
        //				  << ", " << Container.z() << ") " << G4endl;

            //G4VPhysicalVolume* physiWCBarrelWLSPlate =
            new G4PVPlacement(WCPMTODRotation,           // its rotation
                  Container,
                  logicWCODWLSAndPMT,         // its logical volume
                  "WCBarrelCellODContainer",  // its name
                  logicWCBarrelAnnulus,         // its mother volume
                  false,                     // no boolean operations
                  odcopyNo++,
                  checkOverlapsPMT);

          }
        }
      }
    }


    //-------------------------------------------------------------
    // Add PMTs in extra Tower if necessary
    //------------------------------------------------------------

    // EXTRA TOWER PART
    if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){

      // TYVEK

      G4double towerODTyvekRmin[2];
      G4double towerODTyvekRmax[2];
      for(int i = 0; i < 2; i++){
        towerODTyvekRmin[i] = annulusODTyvekRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
        towerODTyvekRmax[i] = annulusODTyvekRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
      }
      G4Polyhedra* solidWCTowerODTyvek = new G4Polyhedra("WCExtraTowerODTyvek",
                                                         totalAngle-2.*pi+barrelPhiOffset,//+dPhi/2., // phi start
                                                         2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //phi end
                                                         1, //NPhi-gon
                                                         2,
                                                         annulusZ,
                                                         towerODTyvekRmin,
                                                         towerODTyvekRmax);

      G4LogicalVolume* logicWCTowerODTyvek =
		new G4LogicalVolume(solidWCTowerODTyvek,
							G4Material::GetMaterial("Tyvek"),
							"WCExtraTowerODTyvek",
							0,0,0);

      //G4LogicalSkinSurface *WaterExtraTySurfaceSide =
	  new G4LogicalSkinSurface("WaterExtraTySurfaceSide", logicWCTowerODTyvek, OpWaterTySurface);


      logicWCTowerODTyvek->SetVisAttributes(G4VisAttributes::Invisible);
      //// Uncomment following for TYVEK visualization
      logicWCTowerODTyvek->SetVisAttributes(WCBarrelODTyvekCellVisAtt);


      //G4VPhysicalVolume* physiWCTowerODTyvek =
	  new G4PVPlacement(0,
						G4ThreeVector(0.,0.,0.),
						logicWCTowerODTyvek,
						"WCExtraTowerODTyvek",
						logicWCExtraTower,
						false,
						0,
						checkOverlaps);

      // PMTs

      G4RotationMatrix* WCExtraODPMTRotation = new G4RotationMatrix;
      WCExtraODPMTRotation->rotateY(270.*deg);
      WCExtraODPMTRotation->rotateX(2*pi - (2*pi-totalAngle)/2.+barrelPhiOffset);//align the PMT with the Cell

      G4double towerWidthOD = WCODRadius*tan(2*pi-totalAngle);
      // We don't want the same number of OD PMTs squished horizontally so we scale down the horizontal PMTs by the width of the extra tower
      G4double ratioOfWidths = (double)(WCPMTODperCellHorizontal)*(towerWidthOD/barrelODCellWidth);
      G4long WCPMTODperCellHorizontalExtra = std::lround(ratioOfWidths);
      G4double horizontalODSpacingExtra   = towerWidthOD/(double)WCPMTODperCellHorizontalExtra;
      // verticalODSpacing is identical to that for normal cell.

      odcopyNo = 0;
      for (int iz=0;iz<(G4int)WCBarrelNRings-2;iz++)
      {
        G4double z_offset = -mainAnnulusHeight/2.+barrelCellHeight*(iz+0.5);
        for(G4long i = 0; i < (WCPMTODperCellHorizontalExtra); i++){
          for(G4long j = 0; j < WCPMTODperCellVertical; j++){
            G4cout << "Adding OD PMT in iz = "<< iz << " cell " << i << ", " << j << G4endl;
            G4ThreeVector Container =  G4ThreeVector((WCODRadius)/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.),
                                -towerWidthOD/2.+(i+0.5)*horizontalODSpacingExtra,
                                -(barrelCellHeight * (WCODRadius/WCIDRadius))/2.+(j+0.5)*verticalODSpacing+z_offset);

            Container.rotateZ(-(2*pi-totalAngle)/2.+barrelPhiOffset); // align with the symmetry

            //G4VPhysicalVolume* physiWCExtraBarrelWLSPlate =
            new G4PVPlacement(WCExtraODPMTRotation,              // its rotation
                    Container,
                    logicWCODWLSAndPMT,                // its logical volume
                    "WCExtraBarrelCellODContainer",             // its name
                    logicWCExtraTower,         // its mother volume
                    false,                     // no boolean operations
                    odcopyNo++,
                    checkOverlapsPMT);


          }
        }
      }

    }

    //---------------------------------------------------------
    // Add top and bottom PMTs
    // -----------------------------------------------------

    G4double xoffset;
    G4double yoffset;
    G4int    icopy = 0;

    G4RotationMatrix* WCCapPMTRotation = new G4RotationMatrix;
    WCCapPMTRotation->rotateY(180.*deg);


    // loop over the cap
    G4int CapNCell = (G4int)(WCODCapEdgeLimit/WCODCapPMTSpacing) + 2;
    for ( int i = -CapNCell ; i <  CapNCell; i++) {
      for (int j = -CapNCell ; j <  CapNCell; j++) {

		    G4cout << "Adding OD PMT in top/bottom cap cell " << i << ", " << j << G4endl;

        xoffset = i*WCODCapPMTSpacing + WCODCapPMTSpacing*0.5;
        yoffset = j*WCODCapPMTSpacing + WCODCapPMTSpacing*0.5;

        G4ThreeVector topWLSpos = G4ThreeVector(xoffset,
                                                yoffset,
                                                ((WCIDHeight + 2*WCODDeadSpace)/2)+WCODTyvekSheetThickness);

        G4ThreeVector bottomWLSpos = G4ThreeVector(xoffset,
                                                   yoffset,
                                                   -topWLSpos.getZ());

        if (((sqrt(xoffset*xoffset + yoffset*yoffset) + WCPMTODRadius) < WCODCapEdgeLimit) ) {


		  //		  G4cout << " qqqqqqqqqqqqqqqqqqqqqqqq cap i " << i << " of " << CapNCell << " j " << j << " of " << CapNCell << " Container (" << topWLSpos.x() << ", " << topWLSpos.y()
		  //				  << ", " << topWLSpos.z() << ") " << G4endl;

          //G4VPhysicalVolume* physiTopCapWLSPlate =
          new G4PVPlacement(0,                   // its rotation
                    topWLSpos,
                    logicWCODWLSAndPMT,   // its logical volume
                    "WCTopCapContainerOD",// its name
                    logicWCBarrel,       // its mother volume
                    false,               // no boolean operations
                    icopy,
                    checkOverlapsPMT);


          //G4VPhysicalVolume* physiBottomCapWLSPlate =
          new G4PVPlacement(WCCapPMTRotation,                             // its rotation
                    bottomWLSpos,
                    logicWCODWLSAndPMT,   // its logical volume
                    "WCBottomCapContainerOD",                // its name
                    logicWCBarrel,                 // its mother volume
                    false,                         // no boolean operations
                    icopy,
                    checkOverlapsPMT);

          icopy++;

        }
      }
    }

    G4cout << "#### OD ####" << "\n";
    G4cout << " total on cap: " << icopy << "\n";
    G4cout << " Coverage was calculated to be: " << (icopy*WCPMTODRadius*WCPMTODRadius/(WCIDRadius*WCIDRadius)) << "\n";
    G4cout << "############" << "\n";

  } // END if isODConstructed

  G4LogicalVolume* logicTopCapAssembly = ConstructCapsNoReplica(true);
  G4LogicalVolume* logicBottomCapAssembly = ConstructCapsNoReplica(false);

 // These lines make the large cap volume invisible to view the caps blacksheets. Need to make invisible for 
 // RayTracer
  if (Vis_Choice == "RayTracer"){
    logicBottomCapAssembly->SetVisAttributes(G4VisAttributes::Invisible);
    logicTopCapAssembly->SetVisAttributes(G4VisAttributes::Invisible);
  } else {
    logicBottomCapAssembly->SetVisAttributes(G4VisAttributes::Invisible);
    logicTopCapAssembly->SetVisAttributes(G4VisAttributes::Invisible);
  }

  //G4VPhysicalVolume* physiTopCapAssembly =
  new G4PVPlacement(0,
                  G4ThreeVector(0.,0.,(mainAnnulusHeight/2.+capAssemblyHeight/2.)),
                  logicTopCapAssembly,
                  "TopCapAssembly",
                  logicWCBarrel,
                  false, 0,
				          checkOverlaps);

  //G4VPhysicalVolume* physiBottomCapAssembly =
  new G4PVPlacement(0,
                  G4ThreeVector(0.,0.,(-mainAnnulusHeight/2.-capAssemblyHeight/2.)),
                  logicBottomCapAssembly,
                  "BottomCapAssembly",
                  logicWCBarrel,
                  false, 0,
				          checkOverlaps);

  return logicWC;
}

/**
 * Construct an endcap.  The flipz argument indicates the orientation/end:
 * - flipz == false:  Bottom endcap, construct upright
 * - flipz == true:   Top endcap, construct reflected
 */
G4LogicalVolume* WCSimDetectorConstruction::ConstructCapsNoReplica(G4bool flipz)
{
  // z-orientation multiplier
  const G4int zflip = (flipz ? -1 : 1);
  // substrings for constructing chiral component names
  const G4String oristr = flipz ? "Top" : "Bot";
  const G4String capstr = oristr + G4String("Cap");  // "[Top|Bot]Cap"
  const G4String bbstr  = G4String("Barrel") +
                oristr + G4String("Border");   // "Barrel[Top|Bot]Border"

  capAssemblyHeight = (WCIDHeight-mainAnnulusHeight)/2+1*mm+WCBlackSheetThickness;

  const G4String caname = capstr + G4String("Assembly");  // "[Top|Bot]CapAssembly"
  G4Tubs* solidCapAssembly = new G4Tubs(caname,
							0.0*m,
              // use the largest radius in cap region
							(outerAnnulusRadius+std::max(GetRadiusChange(-zflip*WCIDHeight/2),GetRadiusChange(-zflip*mainAnnulusHeight/2)))/cos(dPhi/2.), 
							capAssemblyHeight/2,
							0.*deg,
							360.*deg);

  G4LogicalVolume* logicCapAssembly =
    new G4LogicalVolume(solidCapAssembly,
                        G4Material::GetMaterial(water),
                        caname,
                        0,0,0);

  G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4VisAttributes::Invisible);
  tmpVisAtt->SetForceWireframe(true);// This line is used to give definition to the rings in OGLSX Visualizer
  logicCapAssembly->SetVisAttributes(tmpVisAtt);
  
  //----------------------------------------------------
  // extra rings for the top and bottom of the annulus
  //---------------------------------------------------
  G4double borderAnnulusZ[3] = {(-barrelCellHeight/2.-(WCIDRadius-innerAnnulusRadius))*zflip, 
								-barrelCellHeight/2.*zflip,
								barrelCellHeight/2.*zflip};
  // Get the radius at global z 
  G4double borderAnnulusRmin[3] = { WCIDRadius + GetRadiusChange(-zflip*(mainAnnulusHeight/2+barrelCellHeight+(WCIDRadius-innerAnnulusRadius))),
                                    innerAnnulusRadius + GetRadiusChange(-zflip*(mainAnnulusHeight/2+barrelCellHeight)), 
                                    innerAnnulusRadius + GetRadiusChange(-zflip*(mainAnnulusHeight/2)) };

  if(std::abs(borderAnnulusZ[2] - borderAnnulusZ[0]) > capAssemblyHeight - WCBlackSheetThickness) {
    G4cerr << "IMPOSSIBLE GEOMETRY:  capAssemblyHeight (" 
          << capAssemblyHeight << ") too small to contain border ring ("
          << std::abs(borderAnnulusZ[2] - borderAnnulusZ[0])
          << ") and endcap blacksheet (" << WCBlackSheetThickness << ")"
          << G4endl;
    /// Should throw an exception.
  }

  const G4String bbrname = G4String("WC") +
                bbstr + G4String("Ring");  // "WCBarrel[Top|Bot]BorderRing"

  //------------------------------------------------------------
  // add blacksheet to the border cells.
  // ---------------------------------------------------------
  // the part between the barrel ring and cap was missing before
  G4double annulusBlackSheetRmax[3] = { WCIDRadius + GetRadiusChange(-zflip*(mainAnnulusHeight/2+barrelCellHeight+(WCIDRadius-innerAnnulusRadius))) + WCBlackSheetThickness,
                                        WCIDRadius + GetRadiusChange(-zflip*(mainAnnulusHeight/2+barrelCellHeight)) + WCBlackSheetThickness,
                                        WCIDRadius + GetRadiusChange(-zflip*(mainAnnulusHeight/2)) + WCBlackSheetThickness};
  G4double annulusBlackSheetRmin[3] = { WCIDRadius + GetRadiusChange(-zflip*(mainAnnulusHeight/2+barrelCellHeight+(WCIDRadius-innerAnnulusRadius))),
                                        WCIDRadius + GetRadiusChange(-zflip*(mainAnnulusHeight/2+barrelCellHeight)),
                                        WCIDRadius + GetRadiusChange(-zflip*(mainAnnulusHeight/2))};
  const G4String bbbsname = bbrname + G4String("BlackSheet");    // "WCBarrel[Top|Bot]BorderBlackSheet"                                      
  G4Polyhedra* solidWCBarrelBlackSheet = new G4Polyhedra(bbbsname,
                                                   barrelPhiOffset, // phi start
                                                   totalAngle, //total phi
                                                   WCBarrelRingNPhi, //NPhi-gon
                                                   3,
                                                   borderAnnulusZ,
                                                   annulusBlackSheetRmin,
                                                   annulusBlackSheetRmax);

  G4LogicalVolume* logicWCBarrelBorderBlackSheet =
    new G4LogicalVolume(solidWCBarrelBlackSheet,
                        G4Material::GetMaterial("Blacksheet"),
                        bbbsname,
                        0,0,0);

  //G4VPhysicalVolume* physiWCBarrelBorderBlackSheet =
    new G4PVPlacement(0,
                      G4ThreeVector(0.,0.,(capAssemblyHeight/2.- barrelCellHeight/2.)*zflip),
                      logicWCBarrelBorderBlackSheet,
                      bbbsname,
                      logicCapAssembly,
                      false,
                      0,
                      checkOverlaps);
   
  new G4LogicalSkinSurface("BSBarrelBorderSkinSurface",logicWCBarrelBorderBlackSheet,
							BSSkinSurface);

  // Change made here to have the if statement contain the !debugmode to be consistent
  // This code gives the Blacksheet its color. 

  if (Vis_Choice == "RayTracer"){

    G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
      = new G4VisAttributes(G4Colour(0.2,0.9,0.2)); // green color
    WCBarrelBlackSheetCellVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
	  WCBarrelBlackSheetCellVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown
    if(!debugMode)
    {
      logicWCBarrelBorderBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
    }
    else
    {
      logicWCBarrelBorderBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }

  else {

    G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
      = new G4VisAttributes(G4Colour(0.2,0.9,0.2));
    if(!debugMode)
    {
      logicWCBarrelBorderBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);
    }
    else
    {
      logicWCBarrelBorderBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
    }
  }

  const G4String etbcname = G4String("WCExtraTower") + oristr +
        G4String("BorderCell");      // "WCExtraTower[Top|Bot]]BorderCell"
  G4double towerBSRmin[3];
  G4double towerBSRmax[3];
  if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){
    //----------------------------------------------
    // also the extra tower need special cells at the 
    // top and bottom.
    // (the top cell is created later on by reflecting the
    // bottom cell) 
    //---------------------------------------------
    //---------------------------------------------
    // add blacksheet to this cells
    //--------------------------------------------
    for(int i = 0; i < 3; i++){
      towerBSRmin[i] = annulusBlackSheetRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
      towerBSRmax[i] = annulusBlackSheetRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
    }
    const G4String etbcbsname = etbcname +
        G4String("BlackSheet");      // "WCExtraTower[Top|Bot]]BorderCellBlackSheet"
    G4Polyhedra* solidWCExtraBorderBlackSheet = 
      new G4Polyhedra(etbcbsname,
			   totalAngle-2.*pi+barrelPhiOffset,//+dPhi/2., // phi start
			   2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //phi end
		     1, //NPhi-gon
			   3,
			   borderAnnulusZ,
			   towerBSRmin,
			   towerBSRmax);
    G4LogicalVolume* logicWCExtraBorderBlackSheet =
      new G4LogicalVolume(solidWCExtraBorderBlackSheet,
			  G4Material::GetMaterial("Blacksheet"),
			  etbcbsname,
			    0,0,0);

    // G4VPhysicalVolume* physiWCExtraBorderBlackSheet =
      new G4PVPlacement(0,
			G4ThreeVector(0.,0.,(capAssemblyHeight/2.- barrelCellHeight/2.)*zflip),
			logicWCExtraBorderBlackSheet,
			etbcbsname,
			logicCapAssembly,
			false,
			0,
			checkOverlaps);

    new G4LogicalSkinSurface("BSExtraBorderSkinSurface",logicWCExtraBorderBlackSheet,
                  BSSkinSurface);

    // These lines add color to the blacksheet in the extratower. If using RayTracer, comment the first chunk and use the second. The Blacksheet should be green.
  
    if (Vis_Choice == "RayTracer"){
    
      G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
        = new G4VisAttributes(G4Colour(0.2,0.9,0.2)); // green color
      WCBarrelBlackSheetCellVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
      WCBarrelBlackSheetCellVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown

      if(!debugMode)
        logicWCExtraBorderBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
      else
        logicWCExtraBorderBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
    }

    else {

      G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
        = new G4VisAttributes(G4Colour(0.2,0.9,0.2)); // green color

      if(!debugMode)
        {logicWCExtraBorderBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);}
      else
        {logicWCExtraBorderBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);}
    }

  }
 //------------------------------------------------------------
 // add caps
 // -----------------------------------------------------------
  const G4String capname = G4String("WC") + capstr;    // "WC[Top|Bot]Cap"

#ifdef DEBUG
  G4cout << "B.Q Cap: " << totalAngle << ", " << WCBarrelRingNPhi << ", " << outerAnnulusRadius << ", " << innerAnnulusRadius << ", " << WCBlackSheetThickness << ", " << zflip << ", " << WCBarrelPMTOffset << G4endl;
#endif

  //---------------------------------------------------------------------
  // add cap blacksheet
  // -------------------------------------------------------------------
  
  G4double capBlackSheetZ[4] = {-WCBlackSheetThickness*zflip, 0., 0., (WCBarrelPMTOffset - (WCIDRadius-innerAnnulusRadius)) *zflip};
  G4double capBlackSheetRmin[4] = { 0., 
                                    0., 
                                    WCIDRadius + GetRadiusChange(-zflip*WCIDHeight/2), 
                                    WCIDRadius + GetRadiusChange(-zflip*(WCIDHeight/2-(WCBarrelPMTOffset - (WCIDRadius-innerAnnulusRadius))))};
  G4double capBlackSheetRmax[4] = { WCIDRadius+WCBlackSheetThickness + GetRadiusChange(-zflip*WCIDHeight/2), 
                                    WCIDRadius+WCBlackSheetThickness + GetRadiusChange(-zflip*WCIDHeight/2),
								                    WCIDRadius+WCBlackSheetThickness + GetRadiusChange(-zflip*WCIDHeight/2),
								                    WCIDRadius+WCBlackSheetThickness + GetRadiusChange(-zflip*(WCIDHeight/2-(WCBarrelPMTOffset - (WCIDRadius-innerAnnulusRadius))))};
  const G4String capbsname = capname + G4String("BlackSheet");

  if(capBlackSheetZ[0] * capBlackSheetZ[3] > 0.) {
    G4cerr << "IMPOSSIBLE GEOMETRY:  z profile array of " << capbsname
           << " should  be monotonic.  Computed values:"
           << "\n    capBlackSheetZ[0] = " << capBlackSheetZ[0]
           << "\n                 Z[1] = " << capBlackSheetZ[1]
           << "\n                 Z[2] = " << capBlackSheetZ[2]
           << "\n                 Z[3] = " << capBlackSheetZ[3]
          << G4endl;
    /// Should throw an exception.
  }

  G4VSolid* solidWCCapBlackSheet = nullptr;
  if(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal){
    solidWCCapBlackSheet
      = new G4Polyhedra(capbsname,
			barrelPhiOffset, // phi start
			totalAngle, //total phi
			WCBarrelRingNPhi, //NPhi-gon
			4, //  z-planes
			capBlackSheetZ, //position of the Z planes
			capBlackSheetRmin, // min radius at the z planes
			capBlackSheetRmax// max radius at the Z planes
			);
    // G4cout << *solidWCCapBlackSheet << G4endl;
  } else { 
    // same as for the cap volume
    G4Polyhedra* mainPart
      = new G4Polyhedra(capbsname + G4String("MainPart"),
			barrelPhiOffset, // phi start
			totalAngle, //phi end
			WCBarrelRingNPhi, //NPhi-gon
			4, //  z-planes
			capBlackSheetZ, //position of the Z planes
			capBlackSheetRmin, // min radius at the z planes
			capBlackSheetRmax// max radius at the Z planes
			);
    G4double extraBSRmin[4];
    G4double extraBSRmax[4];
    for(int i = 0; i < 4 ; i++){
      extraBSRmin[i] = capBlackSheetRmin[i] != 0. ? capBlackSheetRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
      extraBSRmax[i] = capBlackSheetRmax[i] != 0. ? capBlackSheetRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
    }
    G4Polyhedra* extraSlice
    = new G4Polyhedra(capbsname + G4String("ExtraSlice"),
      totalAngle-2.*pi+barrelPhiOffset, // phi start
      2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //
      WCBarrelRingNPhi, //NPhi-gon
      4, //  z-planes
      capBlackSheetZ, //position of the Z planes
      extraBSRmin, // min radius at the z planes
      extraBSRmax// max radius at the Z planes
      );
  
    solidWCCapBlackSheet =
      new G4UnionSolid(capbsname, mainPart, extraSlice);
  }
  G4LogicalVolume* logicWCCapBlackSheet =
    new G4LogicalVolume(solidWCCapBlackSheet,
			G4Material::GetMaterial("Blacksheet"),
			capbsname,
			0,0,0);
  //G4VPhysicalVolume* physiWCCapBlackSheet =
    new G4PVPlacement(0,
                      G4ThreeVector(0.,0.,(-capAssemblyHeight/2.+1*mm+WCBlackSheetThickness)*zflip),
                      logicWCCapBlackSheet,
                      capbsname,
                      logicCapAssembly,
                      false,
                      0,
					  checkOverlaps);
  
  new G4LogicalSkinSurface(capbsname + G4String("BSSkinSurface"),logicWCCapBlackSheet,
						   BSSkinSurface);
      
  // used for OGLSX
  if (Vis_Choice == "OGLSX"){

    G4VisAttributes* WCCapBlackSheetVisAtt 
    = new G4VisAttributes(G4Colour(0.9,0.2,0.2));
  
    if(!debugMode)
          logicWCCapBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);
      else
          logicWCCapBlackSheet->SetVisAttributes(WCCapBlackSheetVisAtt);}

  // used for RayTracer (makes the caps blacksheet yellow)
  if (Vis_Choice == "RayTracer"){

    G4VisAttributes* WCCapBlackSheetVisAtt 
    = new G4VisAttributes(G4Colour(1.0,1.0,0.0));

    if(!debugMode)
        //logicWCCapBlackSheet->SetVisAttributes(G4VisAttributes::Invisible); //Use this line if you want to make the blacksheet on the caps invisible to view through
      logicWCCapBlackSheet->SetVisAttributes(WCCapBlackSheetVisAtt);
      else
          logicWCCapBlackSheet->SetVisAttributes(WCCapBlackSheetVisAtt);}

  //---------------------------------------------------------
  // Add top and bottom PMTs
  // -----------------------------------------------------
    
  G4LogicalVolume* logicWCPMT;
  if(nID_PMTs<=1) logicWCPMT = ConstructPMT(WCPMTName, WCIDCollectionName,"tank",nID_PMTs);
  else logicWCPMT = ConstructMultiPMT(WCPMTName, WCIDCollectionName,"tank",nID_PMTs);
  G4LogicalVolume* logicWCPMT2 = nullptr;
  if(hybrid){
    if(nID_PMTs2<=1) logicWCPMT2 = ConstructPMT(WCPMTName2, WCIDCollectionName2,"tankPMT2",nID_PMTs2);
    else logicWCPMT2 = ConstructMultiPMT(WCPMTName2, WCIDCollectionName2,"tankPMT2",nID_PMTs2);
  }

  G4String pmtname = "WCMultiPMT";
 
  // If using RayTracer and want to view the detector without caps, comment out the top and bottom PMT's
  G4double xoffset;
  G4double yoffset;
  G4int    icopy = 0;

  // loop over the cap
  if(placeCapPMTs){

    if (readFromTable)
    {
      G4int capSection = (zflip==-1) ? 1 : 3;
      for (G4int i=0; i<nPMTsRead; i++ )
      {
        if (!pmtUse[i]) continue; // skip PMT
        if (pmtSection[i]!=capSection) continue; // only place cap PMT

        G4RotationMatrix* WCCapPMTRotation = new G4RotationMatrix;
        //if mPMT: perp to wall
        if(orientation == PERPENDICULAR){
          if(zflip==-1){ // rotation for top cap
            WCCapPMTRotation->rotateY(180.*deg); 
          }
        }
        else if (orientation == VERTICAL)
        WCCapPMTRotation->rotateY(90.*deg);
        else if (orientation == HORIZONTAL)
        WCCapPMTRotation->rotateX(90.*deg);

        xoffset = pmtPos[i].x() + G4RandGauss::shoot(0,pmtPosVar);
        yoffset = pmtPos[i].y() + G4RandGauss::shoot(0,pmtPosVar);
        G4cout<<"Cap PMT ID = "<<i<<", Position = "<<pmtPos[i].x()<<" "<<pmtPos[i].y()<<" "<<pmtPos[i].z()<<", Rotation = "<<pmtRotaton[i]<<G4endl;
        WCCapPMTRotation->rotateZ(pmtRotaton[i]); 
        G4ThreeVector cellpos = G4ThreeVector(xoffset, yoffset, (-capAssemblyHeight/2.+1*mm+WCBlackSheetThickness)*zflip);

#ifdef ACTIVATE_IDPMTS
        //G4VPhysicalVolume* physiCapPMT =
        new G4PVPlacement(WCCapPMTRotation,
                cellpos,                   // its position
                (hybrid && pmtType[i]==2)?logicWCPMT2:logicWCPMT,                // its logical volume
                pmtname, // its name 
                logicCapAssembly,         // its mother volume
                false,                 // no boolean os
                pmtmPMTId[i],               // every PMT need a unique id.
                checkOverlapsPMT);
#endif          
        // logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
        // daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
        // this is still the case.
        icopy++;
      }
    }
    else
    {

      G4int CapNCell = WCCapEdgeLimit/WCCapPMTSpacing + 2;
#ifdef DEBUG
      G4cout << "Debug B.Q, wccap edge = " << WCCapEdgeLimit << ", spacing = " << WCCapPMTSpacing << ", CapNCell = " << CapNCell << ", PMT radius = " << WCPMTRadius << G4endl;
#endif
      for ( int i = -CapNCell ; i <  CapNCell; i++) {
        for (int j = -CapNCell ; j <  CapNCell; j++)   {

          G4RotationMatrix* WCCapPMTRotation = new G4RotationMatrix;
          //if mPMT: perp to wall
          if(orientation == PERPENDICULAR){
            if(zflip==-1){
              WCCapPMTRotation->rotateY(180.*deg); 
            }
          }
          else if (orientation == VERTICAL)
          WCCapPMTRotation->rotateY(90.*deg);
          else if (orientation == HORIZONTAL)
          WCCapPMTRotation->rotateX(90.*deg);

          // Jun. 04, 2020 by M.Shinoki
          // For IWCD (NuPRISM_mPMT Geometry)
          xoffset = i*WCCapPMTSpacing + WCCapPMTSpacing*0.5 + G4RandGauss::shoot(0,pmtPosVar);
          yoffset = j*WCCapPMTSpacing + WCCapPMTSpacing*0.5 + G4RandGauss::shoot(0,pmtPosVar);
          // For WCTE (NuPRISMBeamTest_mPMT Geometry)
          if (isNuPrismBeamTest || isNuPrismBeamTest_16cShort){
            xoffset = i*WCCapPMTSpacing + G4RandGauss::shoot(0,pmtPosVar);
            yoffset = j*WCCapPMTSpacing + G4RandGauss::shoot(0,pmtPosVar);
          }
          G4ThreeVector cellpos = G4ThreeVector(xoffset, yoffset, (-capAssemblyHeight/2.+1*mm+WCBlackSheetThickness)*zflip);     
          
          double dcenter = hybrid?std::max(WCPMTRadius,WCPMTRadius2):WCPMTRadius;
          dcenter+=sqrt(xoffset*xoffset + yoffset*yoffset);
          if (dcenter < WCCapEdgeLimit) 

          // for debugging boundary cases: 
          // &&  ((sqrt(xoffset*xoffset + yoffset*yoffset) + WCPMTRadius) > (WCCapEdgeLimit-100)) ) 
          {
            //B.Q for Hybrid
            G4int horizontalModulo = (i+CapNCell) % WCPMTperCellHorizontal;
            G4int verticalModulo = (j+CapNCell) % WCPMTperCellVertical;
#ifdef ACTIVATE_IDPMTS
            //G4VPhysicalVolume* physiCapPMT =
              new G4PVPlacement(WCCapPMTRotation,
                    cellpos,                   // its position
                    ((horizontalModulo == verticalModulo) && hybrid && WCPMTPercentCoverage2!=0)?logicWCPMT2:logicWCPMT,                // its logical volume
                    pmtname, // its name 
                    logicCapAssembly,         // its mother volume
                    false,                 // no boolean os
                    icopy,               // every PMT need a unique id.
                    checkOverlapsPMT);
#endif          

          // logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
              // daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
            // this is still the case.

            icopy++;
          }
        }
      }
    }


    G4cout << "total on cap: " << icopy << "\n";
    G4cout << "Coverage was calculated to be: " << (icopy*WCPMTRadius*WCPMTRadius/(WCIDRadius*WCIDRadius)) << "\n";
  }//end if placeCapPMTs

  ///////////////   Barrel PMT placement

  if(placeBorderPMTs){

    G4int copyNo = 0;

    if (readFromTable)
    {
      G4int capSection = (zflip==-1) ? 2 : 4;
      for (G4int i=0; i<nPMTsRead; i++ )
      {
        if (!pmtUse[i]) continue; // skip PMT
        if (pmtSection[i]!=capSection) continue; // only place border PMT

        G4double pmtPhi = atan2(pmtPos[i].y(),pmtPos[i].x());
        if (pmtPhi<barrelPhiOffset) pmtPhi += 2*pi;
        G4int iphi = (pmtPhi-barrelPhiOffset)/dPhi;
        G4bool inExtraTower = (iphi >= WCBarrelRingNPhi);
        G4double phi_offset = !inExtraTower ? (iphi+0.5)*dPhi+barrelPhiOffset 
                                            : barrelPhiOffset-(2*pi-totalAngle)/2.;
        G4double dth = atan((borderAnnulusRmin[2]-borderAnnulusRmin[1])/(borderAnnulusZ[2]-borderAnnulusZ[1]));

        G4RotationMatrix* PMTRotation = new G4RotationMatrix;
        if(orientation == PERPENDICULAR)
          PMTRotation->rotateY(90.*deg); //if mPMT: perp to wall
        else if(orientation == VERTICAL)
          PMTRotation->rotateY(0.*deg); //if mPMT: vertical/aligned to wall
        else if(orientation == HORIZONTAL)
          PMTRotation->rotateX(90.*deg); //if mPMT: horizontal to wall
        PMTRotation->rotateX(-phi_offset);//align the PMT with the Cell
        if(orientation == PERPENDICULAR)
          PMTRotation->rotateY(-dth); 
        else if(orientation == VERTICAL)
          PMTRotation->rotateY(-dth); 

        G4double newZ = pmtPos[i].z() + (mainAnnulusHeight/2.+barrelCellHeight/2.)*zflip + G4RandGauss::shoot(0,pmtPosVar);
        G4double newR = annulusBlackSheetRmin[1]+(annulusBlackSheetRmin[2]-annulusBlackSheetRmin[1])*(newZ-borderAnnulusZ[1])/(borderAnnulusZ[2]-borderAnnulusZ[1]);
        G4double newPhi = pmtPhi-phi_offset;

        G4ThreeVector PMTPosition =  G4ThreeVector(newR,
              newR*tan(newPhi) + G4RandGauss::shoot(0,pmtPosVar),
              newZ+(capAssemblyHeight/2.- barrelCellHeight/2.)*zflip);
        PMTRotation->rotateZ(pmtRotaton[i]); 
        G4cout<<"Border ring PMT ID = "<<i<<", Position = "<<pmtPos[i].x()<<" "<<pmtPos[i].y()<<" "<<pmtPos[i].z()<<", Rotation = "<<pmtRotaton[i]<<G4endl;

        PMTPosition.rotateZ(phi_offset);  // align with the symmetry axes of the cell 

#ifdef ACTIVATE_IDPMTS
        //G4VPhysicalVolume* physiWCBarrelBorderPMT =
        new G4PVPlacement(PMTRotation,                      // its rotation
                PMTPosition,
                (hybrid && pmtType[i]==2)?logicWCPMT2:logicWCPMT,                // its logical volume
                pmtname,             // its name
                logicCapAssembly,         // its mother volume
                false,                     // no boolean operations
                pmtmPMTId[i],
                checkOverlapsPMT); 
#endif
        // logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
        // daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
        // this is still the case.
        copyNo++;
      }
    }
    else
    {
      G4double barrelCellWidth = (annulusBlackSheetRmin[1]+annulusBlackSheetRmin[2])*tan(dPhi/2.);
      G4double horizontalSpacing   = barrelCellWidth/WCPMTperCellHorizontal;
      G4double verticalSpacing     = barrelCellHeight/WCPMTperCellVertical;

#ifdef DEBUG
      G4cout << "Debug B.Q : place border pmt, spacing horiz = "  << horizontalSpacing << ", vertical = " << verticalSpacing*zflip << G4endl;
#endif

      for (int iphi=0;iphi<(G4int)WCBarrelRingNPhi;iphi++)
      {
        G4double phi_offset = (iphi+0.5)*dPhi+barrelPhiOffset;
        G4double dth = atan((borderAnnulusRmin[2]-borderAnnulusRmin[1])/(borderAnnulusZ[2]-borderAnnulusZ[1]));

        for(G4double i = 0; i < WCPMTperCellHorizontal; i++){
          for(G4double j = 0; j < WCPMTperCellVertical; j++){

            G4RotationMatrix* PMTRotation = new G4RotationMatrix;
            if(orientation == PERPENDICULAR)
              PMTRotation->rotateY(90.*deg); //if mPMT: perp to wall
            else if(orientation == VERTICAL)
              PMTRotation->rotateY(0.*deg); //if mPMT: vertical/aligned to wall
            else if(orientation == HORIZONTAL)
              PMTRotation->rotateX(90.*deg); //if mPMT: horizontal to wall
            PMTRotation->rotateX(-phi_offset);//align the PMT with the Cell
            if(orientation == PERPENDICULAR)
              PMTRotation->rotateY(-dth); 
            else if(orientation == VERTICAL)
              PMTRotation->rotateY(-dth); 

            G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius,
                  -barrelCellWidth/2.+(i+0.5)*horizontalSpacing + G4RandGauss::shoot(0,pmtPosVar),
                  (-barrelCellHeight/2.+(j+0.5)*verticalSpacing)*zflip + G4RandGauss::shoot(0,pmtPosVar));

            G4double newR = annulusBlackSheetRmin[1]+(annulusBlackSheetRmin[2]-annulusBlackSheetRmin[1])*(PMTPosition.z()-borderAnnulusZ[1])/(borderAnnulusZ[2]-borderAnnulusZ[1]);
            PMTPosition.setX(newR);
            PMTPosition.setZ(PMTPosition.z()+(capAssemblyHeight/2.- barrelCellHeight/2.)*zflip);

            PMTPosition.rotateZ(phi_offset);  // align with the symmetry axes of the cell 
#ifdef ACTIVATE_IDPMTS
#ifdef WCSIMCONSTRUCTCYLINDER_VERBOSE
      G4cout << "Add PMT on barrel iphi = "<< iphi << " cell " << i << ", " << j << G4endl;
#endif
            //G4VPhysicalVolume* physiWCBarrelBorderPMT =
            new G4PVPlacement(PMTRotation,                      // its rotation
                    PMTPosition,
                    (i==j && hybrid && WCPMTPercentCoverage2!=0)?logicWCPMT2:logicWCPMT,                // its logical volume
                    pmtname,             // its name
                    logicCapAssembly,         // its mother volume
                    false,                     // no boolean operations
                    copyNo++,
                    checkOverlapsPMT); 
#endif
            // logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
            // daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
            // this is still the case.
          }
        }
      }
  
      //-------------------------------------------------------------
      // Add PMTs in extra Tower if necessary
      //------------------------------------------------------------
      if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){

#ifdef DEBUG
        G4cout << "Debug B.Q : Add extra tower cap "  << G4endl;
#endif

        G4double dth = atan((towerBSRmin[2]-towerBSRmin[1])/(borderAnnulusZ[2]-borderAnnulusZ[1]));
                                                    
        G4double towerWidth = (annulusBlackSheetRmin[1]+annulusBlackSheetRmin[2])/2.*tan(2*pi-totalAngle);

        G4double horizontalSpacingExtra   = towerWidth/(WCBarrelNumPMTHorizontal-WCBarrelRingNPhi*WCPMTperCellHorizontal);
        // verticalSpacing is identical to that for normal cell.

        for(G4double i = 0; i < (WCBarrelNumPMTHorizontal-WCBarrelRingNPhi*WCPMTperCellHorizontal); i++){
          for(G4double j = 0; j < WCPMTperCellVertical; j++){

            G4RotationMatrix* PMTRotation = new G4RotationMatrix;
            if(orientation == PERPENDICULAR)
              PMTRotation->rotateY(90.*deg); //if mPMT: perp to wall
            else if(orientation == VERTICAL)
              PMTRotation->rotateY(0.*deg); //if mPMT: vertical/aligned to wall
            else if(orientation == HORIZONTAL)
              PMTRotation->rotateX(90.*deg); //if mPMT: horizontal to wall
            PMTRotation->rotateX((2*pi-totalAngle)/2.-barrelPhiOffset);//align the PMT with the Cell
            if(orientation == PERPENDICULAR)
              PMTRotation->rotateY(-dth); 
            else if(orientation == VERTICAL)
              PMTRotation->rotateY(-dth); 

            G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.),
                  towerWidth/2.-(i+0.5)*horizontalSpacingExtra + G4RandGauss::shoot(0,pmtPosVar),
                      (-barrelCellHeight/2.+(j+0.5)*verticalSpacing)*zflip + G4RandGauss::shoot(0,pmtPosVar));

            G4double newR = towerBSRmin[1]+(towerBSRmin[2]-towerBSRmin[1])*(PMTPosition.z()-borderAnnulusZ[1])/(borderAnnulusZ[2]-borderAnnulusZ[1]);
            PMTPosition.setX(newR);
            PMTPosition.setZ(PMTPosition.z()+(capAssemblyHeight/2.- barrelCellHeight/2.)*zflip);

            PMTPosition.rotateZ(-(2*pi-totalAngle)/2.+barrelPhiOffset); // align with the symmetry axes of the cell 
#ifdef ACTIVATE_IDPMTS
#ifdef WCSIMCONSTRUCTCYLINDER_VERBOSE
            G4cout << "Add PMTs in extra tower, cell " << i << ", " << j << G4endl;
#endif          
            //G4VPhysicalVolume* physiWCBarrelBorderPMT =
            new G4PVPlacement(PMTRotation,                          // its rotation
                    PMTPosition,
                    (i==j && hybrid && WCPMTPercentCoverage2!=0)?logicWCPMT2:logicWCPMT,                // its logical volume
                    pmtname,             // its name
                    logicCapAssembly,         // its mother volume
                    false,                     // no boolean operations
                    i*WCPMTperCellVertical+j,
                    checkOverlapsPMT);
#endif
              // logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
              // daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
              // this is still the case.
          }
        }
      
      }
    }
  }//end if placeBorderPMTs

  // # -------------------------------------- #
  // ##########################################
  // # Prototype Outer-Detector OD Hyper-K HK #
  // ##########################################
  // # -------------------------------------- #

  if(isODConstructed){

    G4double sphereRadius =
	  (WCPMTODExposeHeight*WCPMTODExposeHeight+ WCPMTODRadius*WCPMTODRadius)/(2*WCPMTODExposeHeight);
    WCODRadius = outerAnnulusRadius - sphereRadius;

    //-------------------------------------------------------------
    // OD Tyvek Barrel side
    // ------------------------------------------------------------
    G4double annulusODTyvekRmax[2] = {WCODRadius,
                                      WCODRadius};
    G4double annulusODTyvekRmin[2] = {WCODRadius-WCODTyvekSheetThickness,
                                      WCODRadius-WCODTyvekSheetThickness};
    G4double RingZ[2] = {borderAnnulusZ[0], borderAnnulusZ[2]};

    const G4String bbodtname = bbrname +
        G4String("ODTyvek"); // "WCBarrel[Top|Bot]BorderRingODTyvek"
    G4Polyhedra* solidWCBarrelBorderODTyvek =
        new G4Polyhedra(bbodtname,
		        barrelPhiOffset, // phi start
			      totalAngle, //total phi
			      WCBarrelRingNPhi, //NPhi-gon
            2,
            RingZ,
            annulusODTyvekRmin,
            annulusODTyvekRmax);

    G4LogicalVolume* logicWCBarrelBorderODTyvek =
	  new G4LogicalVolume(solidWCBarrelBorderODTyvek,
			      G4Material::GetMaterial("Tyvek"),
	                      bbodtname,
			      0,0,0);

    new G4LogicalSkinSurface(bbodtname + G4String("WaterTySurface"),
                             logicWCBarrelBorderODTyvek,
                             OpWaterTySurface);

    G4VisAttributes* WCBarrelODTyvekCellVisAtt =
	  new G4VisAttributes(yellow);
    WCBarrelODTyvekCellVisAtt->SetForceWireframe(true);
    WCBarrelODTyvekCellVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown

    logicWCBarrelBorderODTyvek->SetVisAttributes(G4VisAttributes::Invisible);
    //// Uncomment following for TYVEK visualization
    logicWCBarrelBorderODTyvek->SetVisAttributes(WCBarrelODTyvekCellVisAtt);

    //G4VPhysicalVolume* physiWCBarrelBorderCellODTyvek =
	  new G4PVPlacement(0,
					  G4ThreeVector(0.,0.,(capAssemblyHeight/2.- barrelCellHeight/2.)*zflip),
					  logicWCBarrelBorderODTyvek,
					  bbodtname,
					  logicCapAssembly,
					  false,
					  0,
					  checkOverlaps);

    // OD Tyvek around the ID cap
    G4VSolid* solidWCODCapTyvek = nullptr;
    G4double odCapZ[2] = {
      (-WCBlackSheetThickness-1.*mm)*zflip,
      (WCBarrelPMTOffset - (WCIDRadius-innerAnnulusRadius)) *zflip};
    if(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal){
      solidWCODCapTyvek
      = new G4Polyhedra(capname + G4String("Tyvek"),
                barrelPhiOffset, // phi start
                totalAngle, //phi end
                WCBarrelRingNPhi, //NPhi-gon
                2, // 2 z-planes
                odCapZ, //position of the Z planes
                annulusODTyvekRmin, // min radius at the z planes
                annulusODTyvekRmax// max radius at the Z planes
                );
    } else {
      // if there is an extra tower, the cap volume is a union of
      // to polyhedra. We have to unite both parts, because there are 
      // PMTs that are on the border between both parts.
      G4Polyhedra* mainPart 
      = new G4Polyhedra(capname + G4String("TyvekMainPart"),
                barrelPhiOffset, // phi start
                totalAngle, //phi end
                WCBarrelRingNPhi, //NPhi-gon
                2, // 2 z-planes
                odCapZ, //position of the Z planes
                annulusODTyvekRmin, // min radius at the z planes
                annulusODTyvekRmax// max radius at the Z planes
                );
      G4double extraCapRmin[2]; 
      G4double extraCapRmax[2]; 
      for(int i = 0; i < 2 ; i++){
        extraCapRmin[i] = annulusODTyvekRmin[i] != 0. ? annulusODTyvekRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
        extraCapRmax[i] = annulusODTyvekRmax[i] != 0. ? annulusODTyvekRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.) : 0.;
      }
      G4Polyhedra* extraSlice 
      = new G4Polyhedra(capname + G4String("TyvekExtraSlice"),
                totalAngle-2.*pi+barrelPhiOffset, // phi start
                2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //total phi 
                // fortunately there are no PMTs an the gap!
                1, //NPhi-gon
                2, //  z-planes
                odCapZ, //position of the Z planes
                extraCapRmin, // min radius at the z planes
                extraCapRmax// max radius at the Z planes
                );
      solidWCODCapTyvek =
      new G4UnionSolid(capname + G4String("Tyvek"), mainPart, extraSlice);
    }

    G4LogicalVolume* logicWCODCapTyvek = 
      new G4LogicalVolume(solidWCODCapTyvek,
                G4Material::GetMaterial("Tyvek"),
                capname + G4String("TyvekPolygon"),
                0,0,0);

      new G4PVPlacement(0,                           // no rotation
              G4ThreeVector(0.,0.,(-capAssemblyHeight/2.+1*mm+WCBlackSheetThickness)*zflip),    // its position
              logicWCODCapTyvek,          // its logical volume
              capname + G4String("Tyvek"),             // its name
              logicCapAssembly,                  // its mother volume
              false,                       // no boolean operations
              0,                          // Copy #
              checkOverlaps);
    
    new G4LogicalSkinSurface(capname + G4String("WaterTySurface"),
                              logicWCODCapTyvek,
                              OpWaterTySurface);

    logicWCODCapTyvek->SetVisAttributes(G4VisAttributes::Invisible);
    //// Uncomment following for TYVEK visualization
    logicWCODCapTyvek->SetVisAttributes(WCBarrelODTyvekCellVisAtt);

    //-------------------------------------------------------------
    // OD BARREL PMTs
    // ------------------------------------------------------------

    ///////////////   Barrel PMT placement
    G4double barrelCellWidth = 2.*WCIDRadius*tan(dPhi/2.);
    const G4double barrelODCellWidth   = 2.*WCODRadius*tan(dPhi/2.);
    const G4double barrelODCellHeight  = barrelCellHeight * (barrelODCellWidth/barrelCellWidth);
    G4double horizontalODSpacing = barrelODCellWidth/WCPMTODperCellHorizontal;
    const G4double verticalODSpacing   = barrelODCellHeight / WCPMTODperCellVertical;

    G4int odcopyNo = 0;
    for (int iphi=0;iphi<WCBarrelRingNPhi;iphi++)
    {
      G4double phi_offset = (iphi+0.5)*dPhi+barrelPhiOffset;

      G4RotationMatrix* WCPMTODRotation = new G4RotationMatrix;
      WCPMTODRotation->rotateY(270.*deg);
      WCPMTODRotation->rotateX(phi_offset);//align the PMT with the Cell

      for(G4long i = 0; i < WCPMTODperCellHorizontal; i++){
        for(G4long j = 0; j < WCPMTODperCellVertical; j++){

          G4cout << "Adding OD PMT in barrel in iphi "<< iphi << " cell" << i << ", " << j << G4endl;

          G4ThreeVector Container =  G4ThreeVector(WCODRadius,
                                                  -barrelODCellWidth/2.+(i+0.5)*horizontalODSpacing+((G4int)(std::pow(-1,j))*(G4int)(WCODPMTShift)/2),
                                                  (-(barrelCellHeight * (barrelODCellWidth/barrelCellWidth))/2.+(j+0.5)*verticalODSpacing)*zflip);
          Container.setZ(Container.z()+(capAssemblyHeight/2.- barrelCellHeight/2.)*zflip);
          Container.rotateZ(phi_offset);  // align with the symmetry axes of the cell 

              //G4VPhysicalVolume* physiWCBarrelWLSPlate =
          new G4PVPlacement(WCPMTODRotation,           // its rotation
                    Container,
                    logicWCODWLSAndPMT,         // its logical volume
                    "WCBorderCellODContainer",  // its name
                    logicCapAssembly,         // its mother volume
                    false,                     // no boolean operations
                    odcopyNo++,
                    checkOverlapsPMT);

        }
      }
    }

    if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){

      // TYVEK
      G4double towerODTyvekRmin[2];
      G4double towerODTyvekRmax[2];
      for(int i = 0; i < 2; i++){
        towerODTyvekRmin[i] = annulusODTyvekRmin[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
        towerODTyvekRmax[i] = annulusODTyvekRmax[i]/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.);
      }
      const G4String etbcodtname = etbcname +
        G4String("ODTyvek"); // "WCExtraTower[Top|Bot]]BorderCellODTyvek"
      G4Polyhedra* solidWCExtraBorderCellODTyvek =
        new G4Polyhedra(etbcodtname,
                        totalAngle-2.*pi+barrelPhiOffset,//+dPhi/2., // phi start
                        2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //phi end
                        1, //NPhi-gon
                        2,
                        RingZ,
                        towerODTyvekRmin,
                        towerODTyvekRmax);

      G4LogicalVolume* logicWCExtraBorderCellODTyvek =
	new G4LogicalVolume(solidWCExtraBorderCellODTyvek,
                            G4Material::GetMaterial("Tyvek"),
                            etbcodtname,
                            0,0,0);

      new G4LogicalSkinSurface(etbcodtname + G4String("WaterTySurface"),
                               logicWCExtraBorderCellODTyvek,
                               OpWaterTySurface);

      logicWCExtraBorderCellODTyvek->SetVisAttributes(G4VisAttributes::Invisible);
      //// Uncomment following for TYVEK visualization
      logicWCExtraBorderCellODTyvek->SetVisAttributes(WCBarrelODTyvekCellVisAtt);

      //G4VPhysicalVolume* physiWCExtraBorderODTyvek =
	  new G4PVPlacement(0,
						G4ThreeVector(0.,0.,(capAssemblyHeight/2.- barrelCellHeight/2.)*zflip),
						logicWCExtraBorderCellODTyvek,
						etbcodtname,
						logicCapAssembly,
						false,
						0,
						checkOverlaps);

      // barrelODCellWidth and barrelODCellHeight are identical to those for normal cell

      G4RotationMatrix* WCExtraODPMTRotation = new G4RotationMatrix;
      WCExtraODPMTRotation->rotateY(270.*deg);
      WCExtraODPMTRotation->rotateX(2*pi - (2*pi-totalAngle)/2.+barrelPhiOffset);//align the PMT with the Cell

      G4double towerWidthOD = WCODRadius*tan(2*pi-totalAngle);
      // We don't want the same number of OD PMTs squished horizontally so we scale down the horizontal PMTs by the width of the extra tower
      G4double ratioOfWidths = (double)(WCPMTODperCellHorizontal)*(towerWidthOD/barrelODCellWidth);
      G4long WCPMTODperCellHorizontalExtra = std::lround(ratioOfWidths);
      G4double horizontalODSpacingExtra   = towerWidthOD/(double)WCPMTODperCellHorizontalExtra;
      // verticalODSpacing is identical to that of a normal cell


      for(G4long i = 0; i < (WCPMTODperCellHorizontalExtra); i++){
        for(G4long j = 0; j < WCPMTODperCellVertical; j++){
          G4ThreeVector Container =  G4ThreeVector((WCODRadius)/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.),
												   -towerWidthOD/2.+(i+0.5)*horizontalODSpacingExtra,
												   (-(barrelCellHeight * (WCODRadius/WCIDRadius))/2.+(j+0.5)*verticalODSpacing)*zflip);
          Container.setZ(Container.z()+(capAssemblyHeight/2.- barrelCellHeight/2.)*zflip);
          Container.rotateZ(-(2*pi-totalAngle)/2.+barrelPhiOffset); // align with the symmetry

		  G4cout << "Adding OD PMT in extra tower in cell" << i << ", " << j << G4endl;
          //G4VPhysicalVolume* physiWCBarrelPMT =
		  new G4PVPlacement(WCExtraODPMTRotation,             // its rotation
							Container,
							logicWCODWLSAndPMT,                // its logical volume
							"WCExtraBorderCellODContainer",             // its name
							logicCapAssembly,         // its mother volume
							false,                     // no boolean operations
							i*WCPMTODperCellVertical+j,
							checkOverlapsPMT);

        }
      }

    }

  } // END isODConstructed

  // Add CDS CAD model in WCTE
  if (isNuPrismBeamTest_16cShort && addCDS && !flipz) // bottom cap instead of top cap, because we have rotation later
  {
    auto shape_CDS = CADMesh::TessellatedMesh::FromSTL(CDSFile);

    // set scale
    shape_CDS->SetScale(1);
    double cds_z_offset = -mainAnnulusHeight/2.-capAssemblyHeight/2.; // logicBottomCapAssembly placement
    cds_z_offset += 2*(-capAssemblyHeight/2.+1*mm+WCBlackSheetThickness); // logicWCCap placement
    cds_z_offset += 132.25*mm + 145*mm + 45.9225*mm; // ad-hoc value to place CDS close to endcap but not overlapping
    G4ThreeVector posCDS = G4ThreeVector(0*m, 0*m,cds_z_offset);
    
    // make new shape a solid
    G4VSolid* solid_CDS = shape_CDS->GetSolid();
    
    G4LogicalVolume* CDS_logical =                                   //logic name
    new G4LogicalVolume(solid_CDS,                                 //solid name
              G4Material::GetMaterial("StainlessSteel"),          //material
              "CDS");                                     //objects name
    // rotate if necessary
    G4RotationMatrix* CDS_rot = new G4RotationMatrix; // Rotates X and Z axes only
    CDS_rot->rotateX(270*deg);                 
    CDS_rot->rotateY(315*deg);
    
    new G4PVPlacement(CDS_rot,                       //rotation
              posCDS,                    //at position
              CDS_logical,           //its logical volume
              "CDS",                //its name
              logicCapAssembly,                //its mother  volume
              false,                   //no boolean operation
              0,                       //copy number
              checkOverlaps);          //overlaps checking
    new G4LogicalSkinSurface("CDSSurface",CDS_logical,BSSkinSurface);
  }

  return logicCapAssembly;

}
