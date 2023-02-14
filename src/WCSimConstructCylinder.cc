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

/***********************************************************
 *
 * This file containts the functions which construct a 
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

//#ifndef ACTIVATE_IDPMTS
// #define ACTIVATE_IDPMTS
//#endif

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
  WCBarrelRingNPhi     = (G4int)(WCBarrelNumPMTHorizontal/WCPMTperCellHorizontal); 
  // the part of one ring, that is covered by the regular cells: 
  totalAngle  = 2.0*pi*rad*(WCBarrelRingNPhi*WCPMTperCellHorizontal/WCBarrelNumPMTHorizontal) ;
  // angle per regular cell:
  dPhi        =  totalAngle/ WCBarrelRingNPhi;
  // it's hight:
  barrelCellHeight  = (WCIDHeight-2.*WCBarrelPMTOffset)/WCBarrelNRings;
  // the hight of all regular cells together:
  mainAnnulusHeight = WCIDHeight -2.*WCBarrelPMTOffset -2.*barrelCellHeight;
  
  
  innerAnnulusRadius = WCIDRadius - WCPMTExposeHeight-1.*mm;
  outerAnnulusRadius = WCIDRadius + WCBlackSheetThickness + 1.*mm;//+ Stealstructure etc.
  if(isODConstructed){
    G4double sphereRadius =
        (WCPMTODExposeHeight*WCPMTODExposeHeight+ WCPMTODRadius*WCPMTODRadius)/(2*WCPMTODExposeHeight);
    outerAnnulusRadius =
        WCIDRadius + WCBlackSheetThickness + WCODDeadSpace + // ID Structure
        WCODTyvekSheetThickness + // Tyvek attached to structure
        sphereRadius; // PMT height
  }
  // the radii are measured to the center of the surfaces
  // (tangent distance). Thus distances between the corner and the center are bigger.
  WCLength    = WCIDHeight + 2*2.3*m;	//jl145 - reflects top veto blueprint, cf. Farshid Feyzi
  WCRadius    = (WCIDDiameter/2. + WCBlackSheetThickness + 1.5*m)/cos(dPhi/2.) ; // TODO: OD
  if(isODConstructed){
    WCLength    = WCIDHeight + 2*(WCODHeightWaterDepth + WCBlackSheetThickness + WCODDeadSpace + WCODTyvekSheetThickness);
    WCRadius    = (outerAnnulusRadius + WCODLateralWaterDepth)/cos(dPhi/2.) ;
  }

  // now we know the extend of the detector and are able to tune the tolerance
  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(WCLength > WCRadius ? WCLength : WCRadius);
  G4cout << "Computed tolerance = "
         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
         << " mm" << G4endl;

  //Decide if adding Gd
  water = "Water";
  if (WCAddGd)
  {water = "Doped Water";}


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

  //  std::clog << " qqqq " << " WCRadius " << WCRadius << " WCBarrel radius " << WCRadius+1.*m << " half height "  << .5*WCLength << std::endl;
  
  G4LogicalVolume* logicWCBarrel = 
    new G4LogicalVolume(solidWCBarrel,
			G4Material::GetMaterial(water),
			"WCBarrel",
			0,0,0);

    G4VPhysicalVolume* physiWCBarrel = 
    new G4PVPlacement(0,
		      G4ThreeVector(0.,0.,0.),
		      logicWCBarrel,
		      "WCBarrel",
		      logicWC,
		      false,
	 	      0);

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

    G4VPhysicalVolume *physiCaveTyvek =
        new G4PVPlacement(0,
                          G4ThreeVector(0., 0., 0.),
                          logicCaveTyvek,
						  "CaveBarrelTyvek",
						  logicWCBarrel,
						  false,
                          0);

    G4LogicalSkinSurface *TyvekCaveBarrelSurface = new G4LogicalSkinSurface("TyvekCaveBarrelSurface", logicCaveTyvek, OpWaterTySurface);

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

    G4LogicalSkinSurface *TyvekCaveTopSurface = new G4LogicalSkinSurface("TyvekCaveTopSurface", logicCaveCapsTyvek, OpWaterTySurface);

    G4VisAttributes *CapsCaveTyvekVisAtt = new G4VisAttributes(yellow);
    CapsCaveTyvekVisAtt->SetForceWireframe(true);
    logicCaveCapsTyvek->SetVisAttributes(CapsCaveTyvekVisAtt);
    //logicCaveCapsTyvek->SetVisAttributes(G4VisAttributes::Invisible); //amb79

    G4ThreeVector CaveTyvekPosition(0., 0., WCLength / 2);

    G4VPhysicalVolume *physiTopCaveTyvek =
        new G4PVPlacement(0,
                          CaveTyvekPosition,
						  logicCaveCapsTyvek,
                          "CaveTopTyvek",
						  logicWCBarrel,
                          false,
                          0);


    CaveTyvekPosition.setZ(-CaveTyvekPosition.getZ());

    G4VPhysicalVolume *physiBottomCaveTyvek =
        new G4PVPlacement(0,
                          CaveTyvekPosition,
						  logicCaveCapsTyvek,
                          "CaveBottomTyvek",
						  logicWCBarrel,
                          false,
                          0);


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

 
  G4double mainAnnulusZ[2] = {-mainAnnulusHeight/2., mainAnnulusHeight/2};
  G4double mainAnnulusRmin[2] = {innerAnnulusRadius, innerAnnulusRadius};
  G4double mainAnnulusRmax[2] = {outerAnnulusRadius, outerAnnulusRadius};

  G4Polyhedra* solidWCBarrelAnnulus = new G4Polyhedra("WCBarrelAnnulus",
                                                   0.*deg, // phi start
                                                   totalAngle, 
                                                   (G4int)WCBarrelRingNPhi, //NPhi-gon
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
  G4VPhysicalVolume* physiWCBarrelAnnulus = 
    new G4PVPlacement(0,
		      G4ThreeVector(0.,0.,0.),
		      logicWCBarrelAnnulus,
		      "WCBarrelAnnulus",
		      logicWCBarrel,
		      false,
		      0,true);
if(!debugMode)
   logicWCBarrelAnnulus->SetVisAttributes(G4VisAttributes::Invisible); //amb79
  //-----------------------------------------------------
  // Subdivide the BarrelAnnulus into rings
  //-----------------------------------------------------
  G4double RingZ[2] = {-barrelCellHeight/2.,
                        barrelCellHeight/2.};

  G4Polyhedra* solidWCBarrelRing = new G4Polyhedra("WCBarrelRing",
                                                   0.*deg,//+dPhi/2., // phi start
                                                   totalAngle, //phi end
                                                   (G4int)WCBarrelRingNPhi, //NPhi-gon
                                                   2,
                                                   RingZ,
                                                   mainAnnulusRmin,
                                                   mainAnnulusRmax);

  G4LogicalVolume* logicWCBarrelRing = 
    new G4LogicalVolume(solidWCBarrelRing,
			G4Material::GetMaterial(water),
			"WCBarrelRing",
			0,0,0);

  G4VPhysicalVolume* physiWCBarrelRing = 
    new G4PVReplica("WCBarrelRing",
		    logicWCBarrelRing,
		    logicWCBarrelAnnulus,
		    kZAxis,
		    (G4int)WCBarrelNRings-2,
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
		    (G4int)WCBarrelRingNPhi,
		    dPhi,
                    0.); 

  if(!debugMode)
  	{G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
  	tmpVisAtt->SetForceWireframe(true);// This line is used to give definition to the cells in OGLSX Visualizer
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

  G4Polyhedra* solidWCBarrelCellBlackSheet = new G4Polyhedra("WCBarrelCellBlackSheet",
                                                   -dPhi/2., // phi start
                                                   dPhi, //total phi
                                                   1, //NPhi-gon
                                                   2,
                                                   RingZ,
                                                   annulusBlackSheetRmin,
                                                   annulusBlackSheetRmax);

  logicWCBarrelCellBlackSheet =
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
                      0,true);

  G4LogicalBorderSurface * WaterBSBarrelCellSurface 
    = new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
                                 physiWCBarrelCell,
                                 physiWCBarrelCellBlackSheet, 
                                 OpWaterBSSurface);

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
 // add extra tower if nessecary
 // ---------------------------------------------------------
 
  // we have to declare the logical Volumes 
  // outside of the if block to access it later on 
  G4LogicalVolume* logicWCExtraTowerCell;
  G4LogicalVolume* logicWCExtraBorderCell;
  if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){

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
  			 totalAngle-2.*pi,//+dPhi/2., // phi start
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
    G4VPhysicalVolume* physiWCExtraTower = 
      new G4PVPlacement(0,
			G4ThreeVector(0.,0.,0.),
			logicWCExtraTower,
			"WCExtraTower",
			logicWCBarrel,
			false,
			0,true);
 

    logicWCExtraTower->SetVisAttributes(G4VisAttributes::Invisible);
  //-------------------------------------------
  // subdivide the extra tower into cells  
  //------------------------------------------

    G4Polyhedra* solidWCExtraTowerCell = new G4Polyhedra("WCExtraTowerCell",
			   totalAngle-2.*pi,//+dPhi/2., // phi start
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
		      (G4int)WCBarrelNRings-2,
		      barrelCellHeight);
    logicWCExtraTowerCell->SetVisAttributes(G4VisAttributes::Invisible);
    
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
			   totalAngle-2.*pi,//+dPhi/2., // phi start
			   2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //phi end
		           1, //NPhi-gon
			   2,
			   RingZ,
			   towerBSRmin,
			   towerBSRmax);
    //G4cout << * solidWCTowerBlackSheet << G4endl;
    logicWCTowerBlackSheet =
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
			0,true);

    G4LogicalBorderSurface * WaterBSTowerCellSurface 
      = new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
				   physiWCTowerCell,
				   physiWCTowerBlackSheet, 
				   OpWaterBSSurface);

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

  G4bool WCTopVeto = (WCSimTuningParams->GetTopVeto());

  G4LogicalVolume* logicWCTopVeto;

  if(WCTopVeto){

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

	  G4VPhysicalVolume* physiWCTopVeto =
			new G4PVPlacement(	0,
								G4ThreeVector(0.,0.,WCIDHeight/2
													+1.0*m),
								logicWCTopVeto,
								"WCTopVeto",
								logicWCBarrel,
								false,0,true);

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
	  G4VPhysicalVolume* physiWCTVTyvekBot =
			new G4PVPlacement(	0,
		                  		G4ThreeVector(0.,0.,-0.5*m
													-WCTyvekThickness/2),
								logicWCTVTyvek,
		               			"WCTVTyvekBot",
		          				logicWCTopVeto,
				 				false,0,true);

          G4LogicalSkinSurface *WaterTyTVSurfaceBot = new G4LogicalSkinSurface("WaterTyTVSurfaceBot", logicWCTVTyvek, OpWaterTySurface);
	  //Top
	  G4VPhysicalVolume* physiWCTVTyvekTop =
			new G4PVPlacement(	0,
		                  		G4ThreeVector(0.,0.,0.5*m
													+WCTyvekThickness/2),
								logicWCTVTyvek,
		               			"WCTVTyvekTop",
		          				logicWCTopVeto,
				 				false,0,true);


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

	  G4VPhysicalVolume* physiWCTVTyvekSide =
			new G4PVPlacement(	0,
		                  		G4ThreeVector(0.,0.,0.),
								logicWCTVTyvekSide,
		               			"WCTVTyvekSide",
		          				logicWCTopVeto,
				 				false,0,true);

          G4LogicalSkinSurface *WaterTyTVSurfaceSurface = new G4LogicalSkinSurface("WaterTyTVSurfaceSide", logicWCTVTyvekSide, OpWaterTySurface);
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

  G4LogicalVolume* logicWCPMT = ConstructPMT(WCPMTName, WCIDCollectionName, "tank");


  /*These lines of code will give color and volume to the PMTs if it hasn't been set in WCSimConstructPMT.cc.
I recommend setting them in WCSimConstructPMT.cc. 
If used here, uncomment the SetVisAttributes(WClogic) line, and comment out the SetVisAttributes(G4VisAttributes::Invisible) line.*/
  
  G4VisAttributes* WClogic 
      = new G4VisAttributes(G4Colour(0.4,0.0,0.8));
     WClogic->SetForceSolid(true);
	 WClogic->SetForceAuxEdgeVisible(true);

    //logicWCPMT->SetVisAttributes(WClogic);
	logicWCPMT->SetVisAttributes(G4VisAttributes::Invisible);

  //jl145------------------------------------------------
  // Add top veto PMTs
  //-----------------------------------------------------

  if(WCTopVeto){

	  G4double WCTVPMTSpacing = (WCSimTuningParams->GetTVSpacing())*cm;
	  G4double WCTVEdgeLimit = WCCapEdgeLimit;
	  G4int TVNCell = WCTVEdgeLimit/WCTVPMTSpacing + 2;

	  int icopy = 0;

	  for ( int i = -TVNCell ; i <  TVNCell; i++) {
		for (int j = -TVNCell ; j <  TVNCell; j++)   {

		  G4double xoffset = i*WCTVPMTSpacing + WCTVPMTSpacing*0.5;
		  G4double yoffset = j*WCTVPMTSpacing + WCTVPMTSpacing*0.5;

		  G4ThreeVector cellpos =
		  		G4ThreeVector(	xoffset, yoffset, -0.5*m);

		  if ((sqrt(xoffset*xoffset + yoffset*yoffset) + WCPMTRadius) < WCTVEdgeLimit) {

		    G4VPhysicalVolume* physiCapPMT =
		    		new G4PVPlacement(	0,						// no rotation
		    							cellpos,				// its position
		    							logicWCPMT,				// its logical volume
		    							"WCPMT",				// its name 
		    							logicWCTopVeto,			// its mother volume
		    							false,					// no boolean os
		    							icopy);					// every PMT need a unique id.

		    icopy++;
		  }
		}
	  }

	  G4double WCTVEfficiency = icopy*WCPMTRadius*WCPMTRadius/((WCIDRadius)*(WCIDRadius));
	  G4cout << "Total on top veto: " << icopy << "\n";
	  G4cout << "Coverage was calculated to be: " << WCTVEfficiency << "\n";

  }

  //
  //
  //jl145------------------------------------------------


    ///////////////   Barrel PMT placement
  G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;
  WCPMTRotation->rotateY(90.*deg);

  G4double barrelCellWidth = 2.*WCIDRadius*tan(dPhi/2.);
  G4double horizontalSpacing   = barrelCellWidth/WCPMTperCellHorizontal;
  G4double verticalSpacing     = barrelCellHeight/WCPMTperCellVertical;

  for(G4double i = 0; i < WCPMTperCellHorizontal; i++){
    for(G4double j = 0; j < WCPMTperCellVertical; j++){
      G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius,
						 -barrelCellWidth/2.+(i+0.5)*horizontalSpacing,
						 -barrelCellHeight/2.+(j+0.5)*verticalSpacing);

#ifdef ACTIVATE_IDPMTS
      G4VPhysicalVolume* physiWCBarrelPMT =
	new G4PVPlacement(WCPMTRotation,              // its rotation
			  PMTPosition, 
			  logicWCPMT,                // its logical volume
			  "WCPMT",             // its name
			  logicWCBarrelCell,         // its mother volume
			  false,                     // no boolean operations
			  (int)(i*WCPMTperCellVertical+j),
			  true);                       
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

    G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;
    WCPMTRotation->rotateY(90.*deg);
    WCPMTRotation->rotateX((2*pi-totalAngle)/2.);//align the PMT with the Cell
                                                 
    G4double towerWidth = WCIDRadius*tan(2*pi-totalAngle);

    G4double horizontalSpacing   = towerWidth/(WCBarrelNumPMTHorizontal-WCBarrelRingNPhi*WCPMTperCellHorizontal);
    G4double verticalSpacing     = barrelCellHeight/WCPMTperCellVertical;

    for(G4double i = 0; i < (WCBarrelNumPMTHorizontal-WCBarrelRingNPhi*WCPMTperCellHorizontal); i++){
      for(G4double j = 0; j < WCPMTperCellVertical; j++){
	G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.),
				towerWidth/2.-(i+0.5)*horizontalSpacing,
			       -barrelCellHeight/2.+(j+0.5)*verticalSpacing);
	PMTPosition.rotateZ(-(2*pi-totalAngle)/2.); // align with the symmetry 
	                                            //axes of the cell 

#ifdef ACTIVATE_IDPMTS
	G4VPhysicalVolume* physiWCBarrelPMT =
	  new G4PVPlacement(WCPMTRotation,             // its rotation
			    PMTPosition, 
			    logicWCPMT,                // its logical volume
			    "WCPMT",             // its name
			    logicWCExtraTowerCell,         // its mother volume
			    false,                     // no boolean operations
			    (int)(i*WCPMTperCellVertical+j),
			    true);                       
#endif
		// logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
		// daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
		// this is still the case.
      }
    }

  }


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

    G4double sphereRadius =
        (WCPMTODExposeHeight*WCPMTODExposeHeight+ WCPMTODRadius*WCPMTODRadius)/(2*WCPMTODExposeHeight);
    WCODRadius = outerAnnulusRadius - sphereRadius;

    //-------------------------------------------------------------
    // OD Tyvek Caps
    // ------------------------------------------------------------

    G4Tubs* solidWCODCapsTyvek = new G4Tubs("WCODCapsTyvek",
                                            0,
                                            WCIDRadius,
                                            .5*(WCODTyvekSheetThickness),
                                            0.*deg,
                                            360.*deg);

    G4LogicalVolume* logicWCODCapTyvek =
        new G4LogicalVolume(solidWCODCapsTyvek,
                            G4Material::GetMaterial("Tyvek"),
                            "WCODTopCapTyvek",
                            0,0,0);

    G4VisAttributes* WCCapsODTyvekCellVisAtt =
        new G4VisAttributes(yellow);
    WCCapsODTyvekCellVisAtt->SetForceWireframe(true);

    logicWCODCapTyvek->SetVisAttributes(G4VisAttributes::Invisible);
    //// Uncomment following for TYVEK visualization
    logicWCODCapTyvek->SetVisAttributes(WCCapsODTyvekCellVisAtt);

    G4ThreeVector CapTyvekPosition(0.,0.,(WCIDHeight + 2*WCODDeadSpace)/2);

    G4VPhysicalVolume* physiWCODTopCapsTyvek =
        new G4PVPlacement(0,
                          CapTyvekPosition,
                          logicWCODCapTyvek,
                          "WCODTopCapsTyvek",
                          logicWCBarrel,
                          false,
                          0);

    G4LogicalSkinSurface *WaterTySurfaceTop =
        new G4LogicalSkinSurface("WaterTySurfaceTop", logicWCODCapTyvek, OpWaterTySurface);

    CapTyvekPosition.setZ(-CapTyvekPosition.getZ());

    G4VPhysicalVolume* physiWCODBottomCapsTyvek =
        new G4PVPlacement(0,
                          CapTyvekPosition,
                          logicWCODCapTyvek,
                          "WCODBottomCapsTyvek",
                          logicWCBarrel,
                          false,
                          0);


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

    logicWCBarrelCellODTyvek =
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


    G4VPhysicalVolume* physiWCBarrelCellODTyvek =
        new G4PVPlacement(0,
                          G4ThreeVector(0.,0.,0.),
                          logicWCBarrelCellODTyvek,
                          "WCBarrelCellODTyvek",
                          logicWCBarrelCell,
                          false,
                          0,true);
    G4LogicalSkinSurface *WaterTySurfaceSide =
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
	// qqq
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
    G4cout << "--> WCPMTODPercentCoverageTop : " <<  WCPMTODPercentCoverageTop << G4endl;
    G4cout << "--> WCPMTODPercentCoverageBottom : " <<  WCPMTODPercentCoverageBottom << G4endl;
    G4cout << G4endl;
    // ------------------- //
    // The number of PMTs per cell gives a slightly different coverage so the photocoverage
    // parameter must be changed here so the endcaps will have the same photocoverage as the barrel.
    WCPMTODPercentCoverage = RealODCoverage*100;
    WCODCapPMTSpacing  = (pi*WCIDDiameter/(round(WCIDDiameter*sqrt(pi*WCPMTODPercentCoverage)/(10.0*WCPMTODRadius))));
	if( WCPMTODPercentCoverageTop )
	  WCODCapPMTSpacingTop  = (pi*WCIDDiameter/(round(WCIDDiameter*sqrt(pi*WCPMTODPercentCoverageTop)/(10.0*WCPMTODRadius))));
	if( WCPMTODPercentCoverageBottom )
	  WCODCapPMTSpacingBottom  = (pi*WCIDDiameter/(round(WCIDDiameter*sqrt(pi*WCPMTODPercentCoverageBottom)/(10.0*WCPMTODRadius))));

    if((G4int)WCPMTODperCellHorizontal == 0 && (G4int)WCPMTODperCellVertical == 0){
      ComputeWCODPMT((G4int)NPMTODByCell,&WCPMTODperCellHorizontal,&WCPMTODperCellVertical);
    }

    G4double horizontalODSpacing = barrelODCellWidth/WCPMTODperCellHorizontal;
    G4double verticalODSpacing   = barrelODCellHeight/WCPMTODperCellVertical;

    if(WCODPMTShift > barrelODCellWidth/2. - WCPMTODRadius) WCODPMTShift = 0.*cm;

	int count_OD_barrel_cell=0;

    for(G4double i = 0; i < WCPMTODperCellHorizontal; i++){
      for(G4double j = 0; j < WCPMTODperCellVertical; j++){
        G4ThreeVector Container =  G4ThreeVector(WCODRadius,
                                                 -barrelODCellWidth/2.+(i+0.5)*horizontalODSpacing+((G4int)(std::pow(-1,j))*(G4int)(WCODPMTShift)/2),
                                                 -(barrelCellHeight * (barrelODCellWidth/barrelCellWidth))/2.+(j+0.5)*verticalODSpacing);

		//		std::cout << " qqqq barrel i " << i << " of " << WCPMTODperCellHorizontal << " j " << j << " of " << WCPMTODperCellVertical << " Container (" << Container.x() << ", " << Container.y()
		//				  << ", " << Container.z() << ") r " << sqrt(pow(Container.x(),2) + pow(Container.y(),2)) << std::endl;

        G4VPhysicalVolume* physiWCBarrelWLSPlate =
            new G4PVPlacement(WCPMTODRotation,           // its rotation
                              Container,
                              logicWCODWLSAndPMT,         // its logical volume
                              "WCBarrelCellODContainer",  // its name
                              logicWCBarrelCell,         // its mother volume
                              false,                     // no boolean operations
                              (int)(i*WCPMTODperCellVertical+j),
                              true);

		count_OD_barrel_cell++;
      }
    }

	int count_OD_barrel = count_OD_barrel_cell*WCBarrelRingNPhi*WCBarrelNRings;

	//	std::cout << " count_OD_barrel_cell " << count_OD_barrel_cell << " WCBarrelRingNPhi "<< WCBarrelRingNPhi << " in ring " << WCBarrelRingNPhi*count_OD_barrel_cell << " WCBarrelNRings " << WCBarrelNRings << " in barrel " << count_OD_barrel << std::endl;

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
                                                         totalAngle-2.*pi,//+dPhi/2., // phi start
                                                         2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //phi end
                                                         1, //NPhi-gon
                                                         2,
                                                         RingZ,
                                                         towerODTyvekRmin,
                                                         towerODTyvekRmax);

      logicWCTowerODTyvek =
          new G4LogicalVolume(solidWCTowerODTyvek,
                              G4Material::GetMaterial("Tyvek"),
                              "WCExtraTowerODTyvek",
                              0,0,0);

      G4LogicalSkinSurface *WaterExtraTySurfaceSide =
          new G4LogicalSkinSurface("WaterExtraTySurfaceSide", logicWCTowerODTyvek, OpWaterTySurface);


      logicWCTowerODTyvek->SetVisAttributes(G4VisAttributes::Invisible);
      //// Uncomment following for TYVEK visualization
      logicWCTowerODTyvek->SetVisAttributes(WCBarrelODTyvekCellVisAtt);


      G4VPhysicalVolume* physiWCTowerODTyvek =
          new G4PVPlacement(0,
                            G4ThreeVector(0.,0.,0.),
                            logicWCTowerODTyvek,
                            "WCExtraTowerODTyvek",
                            logicWCExtraTowerCell,
                            false,
                            0,true);

      // PMTs

      G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;
      WCPMTRotation->rotateY(270.*deg);
      WCPMTRotation->rotateX(2*pi - (2*pi-totalAngle)/2.);//align the PMT with the Cell

      G4double towerWidthOD = WCODRadius*tan(2*pi-totalAngle);
      // We don't want the same number of OD PMTs squished horizontally so we scale down the horizontal PMTs by the width of the extra tower
      G4double ratioOfWidths = (double)(WCPMTODperCellHorizontal)*(towerWidthOD/barrelODCellWidth);
      G4int WCPMTODperCellHorizontalExtra = (int)(ratioOfWidths+0.5);
      G4double horizontalODSpacing   = towerWidthOD/(double)WCPMTODperCellHorizontalExtra;
      G4double verticalODSpacing   = barrelODCellHeight/WCPMTODperCellVertical;

      for(G4double i = 0; i < (WCPMTODperCellHorizontalExtra); i++){
        for(G4double j = 0; j < WCPMTODperCellVertical; j++){
			G4ThreeVector Container =  G4ThreeVector((WCODRadius)/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.),
													 -towerWidthOD/2.+(i+0.5)*horizontalODSpacing,
													 -(barrelCellHeight * (WCODRadius/WCIDRadius))/2.+(j+0.5)*verticalODSpacing);

			Container.rotateZ(-(2*pi-totalAngle)/2.); // align with the symmetry

			G4VPhysicalVolume* physiWCExtraBarrelWLSPlate =
					new G4PVPlacement(WCPMTRotation,              // its rotation
									  Container,
									  logicWCODWLSAndPMT,                // its logical volume
									  "WCExtraBarrelCellODContainer",             // its name
									  logicWCExtraTowerCell,         // its mother volume
									  false,                     // no boolean operations
									  (int)(i*WCPMTODperCellVertical+j),
									  true);


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

	int count_OD_top = 0;
	int count_OD_bottom = 0;

    // loop over the cap
    G4int CapNCellTop = (G4int)(WCODCapEdgeLimit/WCODCapPMTSpacing) + 2;
	if( WCPMTODPercentCoverageTop )
	  CapNCellTop = (G4int)(WCODCapEdgeLimit/WCODCapPMTSpacingTop) + 2;
    for ( int i = -CapNCellTop ; i <  CapNCellTop; i++) {
      for (int j = -CapNCellTop ; j <  CapNCellTop; j++) {

        xoffset = i*WCODCapPMTSpacing + WCODCapPMTSpacing*0.5;
        yoffset = j*WCODCapPMTSpacing + WCODCapPMTSpacing*0.5;

		if( WCPMTODPercentCoverageTop ){
		  xoffset = i*WCODCapPMTSpacingTop + WCODCapPMTSpacingTop*0.5;
		  yoffset = j*WCODCapPMTSpacingTop + WCODCapPMTSpacingTop*0.5;
		}

        G4ThreeVector topWLSpos = G4ThreeVector(xoffset,
                                                yoffset,
                                                ((WCIDHeight + 2*WCODDeadSpace)/2)+WCODTyvekSheetThickness);

        if (((sqrt(xoffset*xoffset + yoffset*yoffset) + WCPMTODRadius) < WCODCapEdgeLimit) ) {


		  //		  std::cout << " qqqq cap i " << i << " of " << CapNCellTop << " j " << j << " of " << CapNCellTop << " Container (" << topWLSpos.x() << ", " << topWLSpos.y()
		  //					<< ", " << topWLSpos.z() << ") r " << sqrt(pow(topWLSpos.x(),2) + pow(topWLSpos.y(),2)) << " WCODCapPMTSpacing " << WCODCapPMTSpacing << std::endl;

			G4VPhysicalVolume* physiTopCapWLSPlate =
					new G4PVPlacement(0,                   // its rotation
									  topWLSpos,
									  logicWCODWLSAndPMT,   // its logical volume
									  "WCTopCapContainerOD",// its name
									  logicWCBarrel,       // its mother volume
									  false,               // no boolean operations
									  icopy);


			icopy++;
			count_OD_top ++;
        }
      }
    }

	icopy = 0;
    G4int CapNCellBottom = (G4int)(WCODCapEdgeLimit/WCODCapPMTSpacing) + 2;
	if( WCPMTODPercentCoverageBottom )
	  CapNCellBottom = (G4int)(WCODCapEdgeLimit/WCODCapPMTSpacingBottom) + 2;
    for ( int i = -CapNCellBottom ; i <  CapNCellBottom; i++) {
      for (int j = -CapNCellBottom ; j <  CapNCellBottom; j++) {

        xoffset = i*WCODCapPMTSpacingBottom + WCODCapPMTSpacingBottom*0.5;
        yoffset = j*WCODCapPMTSpacingBottom + WCODCapPMTSpacingBottom*0.5;

        G4ThreeVector topWLSpos = G4ThreeVector(xoffset,
                                                yoffset,
                                                ((WCIDHeight + 2*WCODDeadSpace)/2)+WCODTyvekSheetThickness);

        G4ThreeVector bottomWLSpos = G4ThreeVector(xoffset,
                                                   yoffset,
                                                   -topWLSpos.getZ());

        if (((sqrt(xoffset*xoffset + yoffset*yoffset) + WCPMTODRadius) < WCODCapEdgeLimit) ) {


		  //		  std::cout << " qqqq cap i " << i << " of " << CapNCellBottom << " j " << j << " of " << CapNCellBottom << " Container (" << topWLSpos.x() << ", " << topWLSpos.y()
		  //					<< ", " << topWLSpos.z() << ") r " << sqrt(pow(topWLSpos.x(),2) + pow(topWLSpos.y(),2)) << " WCODCapPMTSpacing " << WCODCapPMTSpacing << std::endl;

			G4VPhysicalVolume* physiBottomCapWLSPlate =
					new G4PVPlacement(WCCapPMTRotation,                             // its rotation
									  bottomWLSpos,
									  logicWCODWLSAndPMT,   // its logical volume
									  "WCBottomCapContainerOD",                // its name
									  logicWCBarrel,                 // its mother volume
									  false,                         // no boolean operations
									  icopy);

			icopy++;
			count_OD_bottom ++;
        }
      }
    }

	std::cout << " count_OD_barrel " << count_OD_barrel << " count_OD_top " << count_OD_top << " count_OD_bottom " << count_OD_bottom << " total " << count_OD_barrel + count_OD_top + count_OD_bottom << std::endl;

    G4cout << "#### OD ####" << "\n";
    G4cout << " total on cap: " << icopy << "\n";
    G4cout << " Coverage was calculated to be: " << (icopy*WCPMTODRadius*WCPMTODRadius/(WCIDRadius*WCIDRadius)) << "\n";
    G4cout << "############" << "\n";

  } // END if isODConstructed

  G4LogicalVolume* logicTopCapAssembly = ConstructCaps(-1);
  G4LogicalVolume* logicBottomCapAssembly = ConstructCaps(1);

  // These lines make the large cap volume invisible to view the caps blacksheets. Need to make invisible for
  // RayTracer
  if (Vis_Choice == "RayTracer"){
    logicBottomCapAssembly->SetVisAttributes(G4VisAttributes::Invisible);
    logicTopCapAssembly->SetVisAttributes(G4VisAttributes::Invisible);}

  G4VPhysicalVolume* physiTopCapAssembly =
      new G4PVPlacement(0,
                        G4ThreeVector(0.,0.,(mainAnnulusHeight/2.+ capAssemblyHeight/2.)),
                        logicTopCapAssembly,
                        "TopCapAssembly",
                        logicWCBarrel,
                        false, 0,true);

  G4VPhysicalVolume* physiBottomCapAssembly =
      new G4PVPlacement(0,
                        G4ThreeVector(0.,0.,(-mainAnnulusHeight/2.- capAssemblyHeight/2.)),
                        logicBottomCapAssembly,
                        "BottomCapAssembly",
                        logicWCBarrel,
                        false, 0,true);

  return logicWC;
}


G4LogicalVolume* WCSimDetectorConstruction::ConstructCaps(G4int zflip)
{

  capAssemblyHeight = (WCIDHeight-mainAnnulusHeight)/2+1*mm+WCBlackSheetThickness;

  G4Tubs* solidCapAssembly = new G4Tubs("CapAssembly",
							0.0*m,
							outerAnnulusRadius/cos(dPhi/2.), 
							capAssemblyHeight/2,
							0.*deg,
							360.*deg);

  G4LogicalVolume* logicCapAssembly =
    new G4LogicalVolume(solidCapAssembly,
                        G4Material::GetMaterial(water),
                        "CapAssembly",
                        0,0,0);

  G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4VisAttributes::Invisible);
  tmpVisAtt->SetForceWireframe(true);// This line is used to give definition to the rings in OGLSX Visualizer
  logicCapAssembly->SetVisAttributes(tmpVisAtt);


  //----------------------------------------------------
  // extra rings for the top and bottom of the annulus
  //---------------------------------------------------
  G4double borderAnnulusZ[3] = {-barrelCellHeight/2.*zflip,
                                (-barrelCellHeight/2.+(WCIDRadius-innerAnnulusRadius))*zflip,
				barrelCellHeight/2.*zflip};
  G4double borderAnnulusRmin[3] = { WCIDRadius, innerAnnulusRadius, innerAnnulusRadius};
  G4double borderAnnulusRmax[3] = {outerAnnulusRadius, outerAnnulusRadius,outerAnnulusRadius};
  G4Polyhedra* solidWCBarrelBorderRing = new G4Polyhedra("WCBarrelBorderRing",
                                                   0.*deg, // phi start
                                                   totalAngle,
                                                   (G4int)WCBarrelRingNPhi, //NPhi-gon
                                                   3,
                                                   borderAnnulusZ,
                                                   borderAnnulusRmin,
                                                   borderAnnulusRmax);
  G4LogicalVolume* logicWCBarrelBorderRing =
    new G4LogicalVolume(solidWCBarrelBorderRing,
                        G4Material::GetMaterial(water),
                        "WCBarrelRing",
                        0,0,0);
  //G4cout << *solidWCBarrelBorderRing << G4endl;

  G4VPhysicalVolume* physiWCBarrelBorderRing =
    new G4PVPlacement(0,
                  G4ThreeVector(0.,0.,(capAssemblyHeight/2.- barrelCellHeight/2.)*zflip),
                  logicWCBarrelBorderRing,
                  "WCBarrelBorderRing",
                  logicCapAssembly,
                  false, 0,true);


                  
  if(!debugMode) 
    logicWCBarrelBorderRing->SetVisAttributes(G4VisAttributes::Invisible); 
  //----------------------------------------------------
  // Subdevide border rings into cells
  // --------------------------------------------------
  G4Polyhedra* solidWCBarrelBorderCell = new G4Polyhedra("WCBarrelBorderCell",
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
                        "WCBarrelBorderCell", 
                        0,0,0);
  //G4cout << *solidWCBarrelBorderCell << G4endl;
  G4VPhysicalVolume* physiWCBarrelBorderCell =
    new G4PVReplica("WCBarrelBorderCell",
                    logicWCBarrelBorderCell,
                    logicWCBarrelBorderRing,
                    kPhi,
                    (G4int)WCBarrelRingNPhi,
                    dPhi,
                    0.);

// These lines of code below will turn the border rings invisible. 

// used for RayTracer
 if (Vis_Choice == "RayTracer"){

  if(!debugMode){
        G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
        tmpVisAtt->SetForceSolid(true);
        logicWCBarrelBorderCell->SetVisAttributes(tmpVisAtt);
		logicWCBarrelBorderCell->SetVisAttributes(G4VisAttributes::Invisible);}
  else {
        G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
        tmpVisAtt->SetForceWireframe(true);
        logicWCBarrelBorderCell->SetVisAttributes(tmpVisAtt);
		logicWCBarrelBorderCell->SetVisAttributes(G4VisAttributes::Invisible);}}

// used for OGLSX
 else {

  if(!debugMode)
        {logicWCBarrelBorderCell->SetVisAttributes(G4VisAttributes::Invisible);}
  else {
        G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
        tmpVisAtt->SetForceWireframe(true);
        logicWCBarrelBorderCell->SetVisAttributes(tmpVisAtt);}}


  //------------------------------------------------------------
  // add blacksheet to the border cells.
  // We can use the same logical volume as for the normal
  // barrel cells.
  // ---------------------------------------------------------


   G4VPhysicalVolume* physiWCBarrelBorderCellBlackSheet =
    new G4PVPlacement(0,
                      G4ThreeVector(0.,0.,0.),
                      logicWCBarrelCellBlackSheet,
                      "WCBarrelCellBlackSheet",
                      logicWCBarrelBorderCell,
                      false,
                      0,true);

  G4LogicalBorderSurface * WaterBSBarrelBorderCellSurface
    = new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
                                 physiWCBarrelBorderCell,
                                 physiWCBarrelBorderCellBlackSheet,
                                 OpWaterBSSurface);

  // we have to declare the logical Volumes 
  // outside of the if block to access it later on 
  G4LogicalVolume* logicWCExtraTowerCell;
  G4LogicalVolume* logicWCExtraBorderCell;
  G4VPhysicalVolume* physiWCExtraBorderCell;
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
    G4Polyhedra* solidWCExtraBorderCell = new G4Polyhedra("WCspecialBarrelBorderCell",
			   totalAngle-2.*pi,//+dPhi/2., // phi start
			   2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //total phi
			   1, //NPhi-gon
			   3,
			   borderAnnulusZ,
			   extraBorderRmin,
			   extraBorderRmax);

    logicWCExtraBorderCell =
      new G4LogicalVolume(solidWCExtraBorderCell, 
			  G4Material::GetMaterial(water),
			  "WCspecialBarrelBorderCell", 
			  0,0,0);
    //G4cout << *solidWCExtraBorderCell << G4endl;

    physiWCExtraBorderCell =
      new G4PVPlacement(0,
                  G4ThreeVector(0.,0.,(capAssemblyHeight/2.- barrelCellHeight/2.)*zflip),
                  logicWCExtraBorderCell,
                  "WCExtraTowerBorderCell",
                  logicCapAssembly,
                  false, 0,true);

    logicWCExtraBorderCell->SetVisAttributes(G4VisAttributes::Invisible);

    G4VPhysicalVolume* physiWCExtraBorderBlackSheet =
      new G4PVPlacement(0,
			G4ThreeVector(0.,0.,0.),
			logicWCTowerBlackSheet,
			"WCExtraTowerBlackSheet",
			logicWCExtraBorderCell,
			false,
			0,true);

    G4LogicalBorderSurface * WaterBSExtraBorderCellSurface 
      = new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
				   physiWCExtraBorderCell,
				   physiWCExtraBorderBlackSheet, 
				   OpWaterBSSurface);

  }
 //------------------------------------------------------------
 // add caps
 // -----------------------------------------------------------
 
  G4double capZ[4] = { (-WCBlackSheetThickness-1.*mm)*zflip,
                      WCBarrelPMTOffset*zflip,
		      WCBarrelPMTOffset*zflip,
		      (WCBarrelPMTOffset+(WCIDRadius-innerAnnulusRadius))*zflip} ;
  G4double capRmin[4] = {  0. , 0., 0., 0.} ;
  G4double capRmax[4] = {outerAnnulusRadius, outerAnnulusRadius,  WCIDRadius, innerAnnulusRadius};
  G4VSolid* solidWCCap;
  if(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal){
    solidWCCap
      = new G4Polyhedra("WCCap",
			0.*deg, // phi start
			totalAngle, //phi end
			(int)WCBarrelRingNPhi, //NPhi-gon
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
      = new G4Polyhedra("WCCapMainPart",
			0.*deg, // phi start
			totalAngle, //phi end
			(int)WCBarrelRingNPhi, //NPhi-gon
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
      = new G4Polyhedra("WCCapExtraSlice",
			totalAngle-2.*pi, // phi start
			2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //total phi 
			// fortunately there are no PMTs an the gap!
			1, //NPhi-gon
			4, //  z-planes
			capZ, //position of the Z planes
			extraCapRmin, // min radius at the z planes
			extraCapRmax// max radius at the Z planes
			);
     solidWCCap =
        new G4UnionSolid("WCCap", mainPart, extraSlice);

     //G4cout << *solidWCCap << G4endl;
   
  }
  // G4cout << *solidWCCap << G4endl;
  G4LogicalVolume* logicWCCap = 
    new G4LogicalVolume(solidWCCap,
			G4Material::GetMaterial(water),
			"WCCapPolygon",
			0,0,0);

  G4VPhysicalVolume* physiWCCap =
    new G4PVPlacement(0,                           // no rotation
		      G4ThreeVector(0.,0.,(-capAssemblyHeight/2.+1*mm+WCBlackSheetThickness)*zflip),     // its position
		      logicWCCap,          // its logical volume
		      "WCCap",             // its name
		      logicCapAssembly,                  // its mother volume
		      false,                       // no boolean operations
		      0,true);                          // Copy #


// used for RayTracer
 if (Vis_Choice == "RayTracer"){
  if(!debugMode){  
    G4VisAttributes* tmpVisAtt2 = new G4VisAttributes(G4Colour(1,0.5,0.5));
	tmpVisAtt2->SetForceSolid(true);
	logicWCCap->SetVisAttributes(tmpVisAtt2);
    logicWCCap->SetVisAttributes(G4VisAttributes::Invisible);

  } else{
	
	G4VisAttributes* tmpVisAtt2 = new G4VisAttributes(G4Colour(0.6,0.5,0.5));
	tmpVisAtt2->SetForceSolid(true);
    logicWCCap->SetVisAttributes(tmpVisAtt2);}}

// used for OGLSX
 else{
  if(!debugMode){  
    logicWCCap->SetVisAttributes(G4VisAttributes::Invisible);
  } else
	{G4VisAttributes* tmpVisAtt2 = new G4VisAttributes(G4Colour(.6,0.5,0.5));
    tmpVisAtt2->SetForceWireframe(true);
    logicWCCap->SetVisAttributes(tmpVisAtt2);}}

  //---------------------------------------------------------------------
  // add cap blacksheet
  // -------------------------------------------------------------------
  
  G4double capBlackSheetZ[4] = {-WCBlackSheetThickness*zflip, 0., 0., WCBarrelPMTOffset*zflip};
  G4double capBlackSheetRmin[4] = {0., 0., WCIDRadius, WCIDRadius};
  G4double capBlackSheetRmax[4] = {WCIDRadius+WCBlackSheetThickness, 
                                   WCIDRadius+WCBlackSheetThickness,
				   WCIDRadius+WCBlackSheetThickness,
				   WCIDRadius+WCBlackSheetThickness};
  G4VSolid* solidWCCapBlackSheet;
  if(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal){
    solidWCCapBlackSheet
      = new G4Polyhedra("WCCapBlackSheet",
			0.*deg, // phi start
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
      = new G4Polyhedra("WCCapBlackSheetMainPart",
			0.*deg, // phi start
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
      = new G4Polyhedra("WCCapBlackSheetextraSlice",
			totalAngle-2.*pi, // phi start
			2.*pi -  totalAngle -G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/(10.*m), //
			WCBarrelRingNPhi, //NPhi-gon
			4, //  z-planes
			capBlackSheetZ, //position of the Z planes
			extraBSRmin, // min radius at the z planes
			extraBSRmax// max radius at the Z planes
			);
    
     solidWCCapBlackSheet =
        new G4UnionSolid("WCCapBlackSheet", mainPart, extraSlice);
  }
  G4LogicalVolume* logicWCCapBlackSheet =
    new G4LogicalVolume(solidWCCapBlackSheet,
			G4Material::GetMaterial("Blacksheet"),
			"WCCapBlackSheet",
			0,0,0);
  G4VPhysicalVolume* physiWCCapBlackSheet =
    new G4PVPlacement(0,
                      G4ThreeVector(0.,0.,0.),
                      logicWCCapBlackSheet,
                      "WCCapBlackSheet",
                      logicWCCap,
                      false,
                      0,true);
   G4LogicalBorderSurface * WaterBSBottomCapSurface 
      = new G4LogicalBorderSurface("WaterBSCapPolySurface",
                                   physiWCCap,physiWCCapBlackSheet,
                                   OpWaterBSSurface);

   G4VisAttributes* WCCapBlackSheetVisAtt 
      = new G4VisAttributes(G4Colour(0.9,0.2,0.2));
    
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
  
	G4LogicalVolume* logicWCPMT = ConstructPMT(WCPMTName, WCIDCollectionName, "tank");
	
	// If using RayTracer and want to view the detector without caps, comment out the top and bottom PMT's

  G4double xoffset;
  G4double yoffset;
  G4int    icopy = 0;

  G4RotationMatrix* WCCapPMTRotation = new G4RotationMatrix;
  if(zflip==-1){
    WCCapPMTRotation->rotateY(180.*deg);
  }

  // loop over the cap
  G4int CapNCell = WCCapEdgeLimit/WCCapPMTSpacing + 2;
  for ( int i = -CapNCell ; i <  CapNCell; i++) {
    for (int j = -CapNCell ; j <  CapNCell; j++)   {

       
      xoffset = i*WCCapPMTSpacing + WCCapPMTSpacing*0.5;
      yoffset = j*WCCapPMTSpacing + WCCapPMTSpacing*0.5;

      
      G4ThreeVector cellpos = G4ThreeVector(xoffset, yoffset, 0);     
      //      G4double WCBarrelEffRadius = WCIDDiameter/2. - WCCapPMTSpacing;
      //      double comp = xoffset*xoffset + yoffset*yoffset 
      //	- 2.0 * WCBarrelEffRadius * sqrt(xoffset*xoffset+yoffset*yoffset)
      //	+ WCBarrelEffRadius*WCBarrelEffRadius;
      //      if ( (comp > WCPMTRadius*WCPMTRadius) && ((sqrt(xoffset*xoffset + yoffset*yoffset) + WCPMTRadius) < WCCapEdgeLimit) ) {
            if (((sqrt(xoffset*xoffset + yoffset*yoffset) + WCPMTRadius) < WCCapEdgeLimit) ) {
#ifdef ACTIVATE_IDPMTS
	G4VPhysicalVolume* physiCapPMT =
	  new G4PVPlacement(WCCapPMTRotation,
			    cellpos,                   // its position
			    logicWCPMT,                // its logical volume
			    "WCPMT", // its name 
			    logicWCCap,         // its mother volume
			    false,                 // no boolean os
			    icopy);               // every PMT need a unique id.
#endif
 // logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more
    // daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
	// this is still the case.

	icopy++;
      }
    }
  }

  G4cout << "total on cap: " << icopy << "\n";
  G4cout << "Coverage was calculated to be: " << (icopy*WCPMTRadius*WCPMTRadius/(WCIDRadius*WCIDRadius)) << "\n";

    ///////////////   Barrel PMT placement
  G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;
  WCPMTRotation->rotateY(90.*deg);

  G4double barrelCellWidth = 2.*WCIDRadius*tan(dPhi/2.);
  G4double horizontalSpacing   = barrelCellWidth/WCPMTperCellHorizontal;
  G4double verticalSpacing     = (barrelCellHeight-WCBorderPMTOffset)/WCPMTperCellVertical;

  for(G4double i = 0; i < WCPMTperCellHorizontal; i++){
    for(G4double j = 0; j < WCPMTperCellVertical; j++){
      G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius,
						 -barrelCellWidth/2.+(i+0.5)*horizontalSpacing,
						 (-(barrelCellHeight-WCBorderPMTOffset)/2.+(j+0.5)*verticalSpacing)*zflip);
#ifdef ACTIVATE_IDPMTS
     G4VPhysicalVolume* physiWCBarrelBorderPMT =
	new G4PVPlacement(WCPMTRotation,                      // its rotation
			  PMTPosition,
			  logicWCPMT,                // its logical volume
			  "WCPMT",             // its name
			  logicWCBarrelBorderCell,         // its mother volume
			  false,                     // no boolean operations
			  (int)(i*WCPMTperCellVertical+j)
			  ,true);                      // no particular field
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

    G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;
    WCPMTRotation->rotateY(90.*deg);
    WCPMTRotation->rotateX((2*pi-totalAngle)/2.);//align the PMT with the Cell
                                                 
    G4double towerWidth = WCIDRadius*tan(2*pi-totalAngle);

    G4double horizontalSpacing   = towerWidth/(WCBarrelNumPMTHorizontal-WCBarrelRingNPhi*WCPMTperCellHorizontal);
    G4double verticalSpacing     = (barrelCellHeight-WCBorderPMTOffset)/WCPMTperCellVertical;

    for(G4double i = 0; i < (WCBarrelNumPMTHorizontal-WCBarrelRingNPhi*WCPMTperCellHorizontal); i++){
      for(G4double j = 0; j < WCPMTperCellVertical; j++){
	G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.),
				towerWidth/2.-(i+0.5)*horizontalSpacing,
			       (-(barrelCellHeight-WCBorderPMTOffset)/2.+(j+0.5)*verticalSpacing)*zflip);
	PMTPosition.rotateZ(-(2*pi-totalAngle)/2.); // align with the symmetry 
	                                            //axes of the cell 
#ifdef ACTIVATE_IDPMTS
	G4VPhysicalVolume* physiWCBarrelBorderPMT =
	  new G4PVPlacement(WCPMTRotation,                          // its rotation
			    PMTPosition,
			    logicWCPMT,                // its logical volume
			    "WCPMT",             // its name
			    logicWCExtraBorderCell,         // its mother volume
			    false,                     // no boolean operations
			    (int)(i*WCPMTperCellVertical+j)
			    ,true);                        // no particular field
#endif
		// logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
		// daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
		// this is still the case.
      }
    }

  }

  // # -------------------------------------- #
  // ##########################################
  // # Prototype Outer-Detector OD Hyper-K HK #
  // ##########################################
  // # -------------------------------------- #

  if(isODConstructed){

    G4double sphereRadius =
        (WCPMTODExposeHeight*WCPMTODExposeHeight+ WCPMTODRadius*WCPMTODRadius)/(2*WCPMTODExposeHeight);
    WCODRadius = outerAnnulusRadius - sphereRadius;

    //------------------------------------------------------------
    // add ODTyvek to the border cells.
    // We can use the same logical volume as for the normal
    // barrel cells.
    // ---------------------------------------------------------

    G4VPhysicalVolume* physiWCBarrelBorderCellODTyvek =
        new G4PVPlacement(0,
                          G4ThreeVector(0.,0.,0.),
                          logicWCBarrelCellODTyvek,
                          "WCBorderCellODTyvek",
                          logicWCBarrelBorderCell,
                          false,
                          0,true);


    //-------------------------------------------------------------
    // OD BARREL PMTs
    // ------------------------------------------------------------

    ///////////////   Barrel PMT placement
    G4RotationMatrix* WCPMTODRotation = new G4RotationMatrix;
    WCPMTODRotation->rotateY(270.*deg);

    G4double barrelODCellWidth   = 2.*WCODRadius*tan(dPhi/2.);
    G4double barrelODCellHeight  = barrelCellHeight * (barrelODCellWidth/barrelCellWidth);
    G4double horizontalODSpacing = barrelODCellWidth/WCPMTODperCellHorizontal;
    G4double verticalODSpacing   = barrelODCellHeight / WCPMTODperCellVertical;

	int countagain_OD_barrel=0;

    for(G4double i = 0; i < WCPMTODperCellHorizontal; i++){
      for(G4double j = 0; j < WCPMTODperCellVertical; j++){
        G4ThreeVector Container =  G4ThreeVector(WCODRadius,
                                                 -barrelODCellWidth/2.+(i+0.5)*horizontalODSpacing+((G4int)(std::pow(-1,j))*(G4int)(WCODPMTShift)/2),
                                                 -(barrelCellHeight * (barrelODCellWidth/barrelCellWidth))/2.+(j+0.5)*verticalODSpacing);


        G4VPhysicalVolume* physiWCBarrelWLSPlate =
            new G4PVPlacement(WCPMTODRotation,           // its rotation
                              Container,
                              logicWCODWLSAndPMT,         // its logical volume
                              "WCBorderCellODContainer",  // its name
                              logicWCBarrelBorderCell,         // its mother volume
                              false,                     // no boolean operations
                              (int)(i*WCPMTODperCellVertical+j),
                              true);


		countagain_OD_barrel++;

      }
    }

    if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){

      G4VPhysicalVolume* physiWCExtraBorderODTyvek =
          new G4PVPlacement(0,
                            G4ThreeVector(0.,0.,0.),
                            logicWCTowerODTyvek,
                            "WCExtraTowerODTyvek",
                            logicWCExtraBorderCell,
                            false,
                            0,true);

      G4double barrelODCellWidth   = 2.*WCODRadius*tan(dPhi/2.);
      G4double barrelODCellHeight  = barrelCellHeight * (barrelODCellWidth/barrelCellWidth);
      
      G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;
      WCPMTRotation->rotateY(270.*deg);
      WCPMTRotation->rotateX(2*pi - (2*pi-totalAngle)/2.);//align the PMT with the Cell

      G4double towerWidthOD = WCODRadius*tan(2*pi-totalAngle);
      // We don't want the same number of OD PMTs squished horizontally so we scale down the horizontal PMTs by the width of the extra tower
      G4double ratioOfWidths = (double)(WCPMTODperCellHorizontal)*(towerWidthOD/barrelODCellWidth);
      G4int WCPMTODperCellHorizontalExtra = (int)(ratioOfWidths+0.5);
      G4double horizontalODSpacing   = towerWidthOD/(double)WCPMTODperCellHorizontalExtra;
      G4double verticalODSpacing   = barrelODCellHeight/WCPMTODperCellVertical;
      

      for(G4double i = 0; i < (WCPMTODperCellHorizontalExtra); i++){
        for(G4double j = 0; j < WCPMTODperCellVertical; j++){
          G4ThreeVector Container =  G4ThreeVector((WCODRadius)/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.),
													 -towerWidthOD/2.+(i+0.5)*horizontalODSpacing,
													 -(barrelCellHeight * (WCODRadius/WCIDRadius))/2.+(j+0.5)*verticalODSpacing);
          Container.rotateZ(-(2*pi-totalAngle)/2.); // align with the symmetry

          G4VPhysicalVolume* physiWCBarrelPMT =
              new G4PVPlacement(WCPMTRotation,             // its rotation
                                Container,
                                logicWCODWLSAndPMT,                // its logical volume
                                "WCExtraBorderCellODContainer",             // its name
                                logicWCExtraBorderCell,         // its mother volume
                                false,                     // no boolean operations
                                (int)(i*WCPMTODperCellVertical+j),
                                true);

        }
      }

    }

  } // END isODConstructed

  return logicCapAssembly;

}
// This function finds the factors of a number and aims to minimise the difference between multiplied factors. This will result in a more square-like distribution of PMTs per cell.
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
void ComputeWCODPMT(G4int NPMT, G4double *NPMTHorizontal, G4double *NPMTVertical){
  switch (NPMT) {
    case 0:
      G4cout << "N PMTs for OD is equal 0, increase coverage" << G4endl;
      *NPMTHorizontal = 1;
      *NPMTVertical   = 1;
      break;
    case 1:
      *NPMTHorizontal = 1;
      *NPMTVertical   = 1;
      break;
    case 2:
      *NPMTHorizontal = 1;
      *NPMTVertical   = 2;
      break;
    case 3:
      *NPMTHorizontal = 2;
      *NPMTVertical   = 2;
      break;
    case 4:
      *NPMTHorizontal = 2;
      *NPMTVertical   = 2;
      break;
    default:
    G4double factors[2];
    FindNiceFactors(NPMT, factors);
    *NPMTHorizontal = factors[1];
    *NPMTVertical   = factors[0];

      break;
  }
}
