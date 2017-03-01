//  -*- mode:c++; tab-width:4;  -*-
#include "WCSimDetectorConstruction.hh"
#include "WCSimPMTObject.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

/***********************************************************
 *
 * This file contains the setup functions for various 
 * detector configurations.  These can be set up by 
 * default in WCSimDetectorConstruction.cc or called
 * in mac files by adding them to WCSimDetectorMessenger.cc.
 *
 * Sourcefile for the WCSimDetectorConstruction class
 *
 ***********************************************************/



void WCSimDetectorConstruction::SetSuperKGeometry()
{
  WCDetectorName = "SuperK";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("PMT20inch", WCIDCollectionName);
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight();
  WCPMTRadius = PMT->GetRadius();
  WCIDDiameter          = 33.6815*m; //16.900*2*cos(2*pi*rad/75)*m; //inner detector diameter
  WCIDHeight            = 36.200*m; //"" "" height
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCBarrelNumPMTHorizontal  = 150; 
  WCBarrelNRings        = 17.;
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3; 
  WCCapPMTSpacing       = 0.707*m; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;

  InitSinglePMT();
}


// Note: the actual coverage is 20.27%
void WCSimDetectorConstruction::SuperK_20inchPMT_20perCent()
{
  WCDetectorName = "SuperK_20inchPMT_20perCent";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("PMT20inch", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 33.6815*m; //16.900*2*cos(2*pi*rad/75)*m; //inner detector diameter
  WCIDHeight            = 36.200*m; //"" "" height
  WCBarrelPMTOffset     = 0.0715*m; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3; 
  WCPMTPercentCoverage  = 20.27;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;

  InitSinglePMT();
}


// Note: the actual coverage is 20.27%
void WCSimDetectorConstruction::SuperK_20inchBandL_20perCent()
{
  WCDetectorName = "SuperK_20inchBandL_20perCent";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine20inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 33.6815*m; //16.900*2*cos(2*pi*rad/75)*m; //inner detector diameter
  WCIDHeight            = 36.200*m; //"" "" height
  WCBarrelPMTOffset     = 0.0715*m; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3; 
  WCPMTPercentCoverage  = 20.27;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;

  InitSinglePMT();
}


// Note: the actual coverage is 14.59%
void WCSimDetectorConstruction::SuperK_12inchBandL_15perCent()
{
  WCDetectorName = "SuperK_12inchBandL_15perCent";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine12inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 33.6815*m; //16.900*2*cos(2*pi*rad/75)*m; //inner detector diameter
  WCIDHeight            = 36.200*m; //"" "" height
  WCBarrelPMTOffset     = 0.0715*m; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 14.59;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;

  InitSinglePMT();
}


// Note: the actual coverage is 13.51%
void WCSimDetectorConstruction::SuperK_20inchBandL_14perCent()
{
  WCDetectorName = "SuperK_20inchBandL_14perCent";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine20inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 33.6815*m; //16.900*2*cos(2*pi*rad/75)*m; //inner detector diameter
  WCIDHeight            = 36.200*m; //"" "" height
  WCBarrelPMTOffset     = 0.0715*m; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 13.51;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;

  InitSinglePMT();
}

void WCSimDetectorConstruction::Cylinder_60x74_20inchBandL_14perCent()
{ 
  WCDetectorName = "Cylinder_60x74_20inchBandL_14perCent()";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine20inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 74.0*m;
  WCIDHeight            = 60.0*m;
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 13.51;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;

  InitSinglePMT();
}

void WCSimDetectorConstruction::Cylinder_60x74_20inchBandL_40perCent()
{ 
  WCDetectorName = "Cylinder_60x74_20inchBandL_40perCent";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine20inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 74.0*m;
  WCIDHeight            = 60.0*m;
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 40.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;

  InitSinglePMT();
}

void WCSimDetectorConstruction::Cylinder_12inchHPD_15perCent()
{ 
  WCDetectorName = "Cylinder_12inchHPD_15perCent";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  // cylindrical detector with a height of 100m and a diameter of 69m 
  // with 12" HPD and 14.59% photocoverage
  WCSimPMTObject * PMT = CreatePMTObject("HPD12inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 69.0*m;
  WCIDHeight            = 100.0*m;
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 14.59;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;

  InitSinglePMT();
}

void WCSimDetectorConstruction::SetHyperKGeometry()
{
  WCDetectorName = "HyperK";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine20inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
  WCIDHeight            = 54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 40.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::SetEggShapedHyperKGeometry()
{
  WCDetectorName = "EggShapedHyperK";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCODCollectionName = WCDetectorName + "-glassFaceWCPMT_OD"; 
  WCSimPMTObject * PMT = CreatePMTObject("PMT20inch", WCIDCollectionName);
  WCSimPMTObject * outerPMT = CreatePMTObject("PMT8inch", WCODCollectionName);
  WCPMTName = PMT->GetPMTName();
  innerPMT_Expose = PMT->GetExposeHeight();
  innerPMT_Radius = PMT->GetRadius();
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
  innerPMT_Rpitch   =   990.*mm;
  innerPMT_Apitch   =   990.*mm;

  outerPMT_Name = outerPMT->GetPMTName();
  outerPMT_Expose = outerPMT->GetExposeHeight();
  outerPMT_Radius = outerPMT->GetRadius();
  outerPMT_TopR      = innerPMT_TopR + 900.*mm;
  outerPMT_BotR      = innerPMT_BotR + 900.*mm;
  outerPMT_TopW      = 12394.*mm;
  outerPMT_BotW      = 11319.*mm;
  outerPMT_Height    = innerPMT_Height + 900.*mm;
  outerPMT_TopRpitch = 3. * innerPMT_Rpitch * (outerPMT_TopR/innerPMT_TopR);
  outerPMT_BotRpitch = 3. * innerPMT_Rpitch * (outerPMT_BotR/innerPMT_BotR);
  outerPMT_Apitch    = 2. * innerPMT_Apitch;

  blackSheetThickness = 20.*mm;

  innerPMT_TopN = 0;
  innerPMT_BotN = 0;

  isEggShapedHyperK = true; // Tell DetectorConstruction to build egg-shaped HK geometry

  MatchWCSimAndEggShapedHyperK();
  InitSinglePMT();
}

void WCSimDetectorConstruction::SetEggShapedHyperKGeometry_withHPD()
{
  WCDetectorName = "EggShapedHyperK_withHPD";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCODCollectionName = WCDetectorName + "-glassFaceWCPMT_OD";
  WCSimPMTObject * PMT = CreatePMTObject("HPD20inchHQE", WCIDCollectionName);
  WCSimPMTObject * outerPMT = CreatePMTObject("PMT8inch", WCODCollectionName);
  WCPMTName = PMT->GetPMTName();
  innerPMT_Expose = PMT->GetExposeHeight();
  innerPMT_Radius = PMT->GetRadius();
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
  innerPMT_Rpitch   =   990.*mm;
  innerPMT_Apitch   =   990.*mm;


  outerPMT_Expose = outerPMT->GetExposeHeight();
  outerPMT_Radius = outerPMT->GetRadius();
  outerPMT_TopR      = innerPMT_TopR + 900.*mm;
  outerPMT_BotR      = innerPMT_BotR + 900.*mm;
  outerPMT_TopW      = 12394.*mm;
  outerPMT_BotW      = 11319.*mm;
  outerPMT_Height    = innerPMT_Height + 900.*mm;
  outerPMT_TopRpitch = 3. * innerPMT_Rpitch * (outerPMT_TopR/innerPMT_TopR);
  outerPMT_BotRpitch = 3. * innerPMT_Rpitch * (outerPMT_BotR/innerPMT_BotR);
  outerPMT_Apitch    = 2. * innerPMT_Apitch;

  blackSheetThickness = 20.*mm;

  innerPMT_TopN = 0;
  innerPMT_BotN = 0;

  isEggShapedHyperK = true; // Tell DetectorConstruction to build egg-shaped HK geometry

  MatchWCSimAndEggShapedHyperK();
  InitSinglePMT();
}


/**
 * Transfer egg-shaped HK variables needed elsewhere
 * to their generic WC equivalents.
 * This should be included in all egg-shaped HK configurations.
 */
void WCSimDetectorConstruction::MatchWCSimAndEggShapedHyperK()
{
  WCLength = waterTank_Length;
  WCPosition = 0.;
  WCPMTRadius = innerPMT_Radius;
}

void WCSimDetectorConstruction::SetNuPrismGeometry(G4String PMTType, G4double PMTCoverage, G4double detectorHeight, G4double detectorDiameter, G4double verticalPosition)
{
    WCDetectorName = "NuPRISM";
    WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
    WCSimPMTObject * PMT = CreatePMTObject(PMTType, WCIDCollectionName);
    WCPMTName = PMT->GetPMTName();
    WCPMTExposeHeight = PMT->GetExposeHeight();
    WCPMTRadius = PMT->GetRadius();

	WCIDHeight               = detectorHeight;
    WCIDDiameter             = detectorDiameter;
    WCIDVerticalPosition     = verticalPosition;

	WCBarrelPMTOffset     = WCPMTRadius;
    WCPMTperCellHorizontal = 1.0;
    WCPMTperCellVertical   = 1.0;
    WCPMTPercentCoverage   = PMTCoverage;
    WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage/100.0)/WCPMTRadius);

    WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/WCPMTperCellVertical));
    WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
    WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
    WCBlackSheetThickness = 2.0*cm;
    WCAddGd               = false;

	InitSinglePMT();
}

void WCSimDetectorConstruction::SetDefaultNuPrismGeometry()
{
    SetNuPrismGeometry("PMT8inch", 40.0, 10*m, 6*m, 0*m);
}

// NuPRISM with mPMTs
void WCSimDetectorConstruction::SetNuPrism_mPMTGeometry(G4double PMTCoverage, G4double detectorHeight, G4double detectorDiameter, G4double verticalPosition)
{
    WCDetectorName = "NuPRISM_mPMT";
    WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
	mPMT_ID_PMT = "PMT3inchR12199_02";    //can be changed in macro through mPMT settings.
	mPMT_OD_PMT = "PMT3inchR12199_02";
    WCSimPMTObject * PMT = CreatePMTObject(mPMT_ID_PMT, WCIDCollectionName);
    WCPMTName = PMT->GetPMTName();
    WCPMTExposeHeight = PMT->GetExposeHeight();
    WCPMTRadius = PMT->GetRadius();

	//mPMT params go first because detector depends on it:
	vessel_cyl_height = 40.*CLHEP::mm; // the 50mm should exist only for OD and extends behind the iD wall
	vessel_radius_curv = 328.6*CLHEP::mm;
	vessel_radius = 242.4*CLHEP::mm;
	dist_pmt_vessel = 5*CLHEP::mm;           // Still "work-in-progress"
	orientation = PERPENDICULAR;
	mPMT_outer_material = "G4_PLEXIGLASS";
	mPMT_inner_material = "SilGel";
	mPMT_outer_material_d = 10*CLHEP::mm;

	// Radius of cone at z=reflectorHeight
	id_reflector_height = 7.5*CLHEP::mm;         //7.5mm from KM3Net JINST
	id_reflector_z_offset = 1.1*CLHEP::mm;         //from KM3Net CAD drawings
	id_reflector_angle = CLHEP::pi/4*CLHEP::rad; // Based on KM3Net JINST: 45 deg wrt normal, so 7.5mm xtra
	G4double vessel_tot_height = vessel_radius + vessel_cyl_height;
	
	// parameters related to filling the ID mPMT
	nID_PMTs = 19;
	config_file = "mPMTconfig_19_nuPrism.txt";

	
	// ToDo: adapt to Robert's design!
	WCIDHeight               = detectorHeight;
    WCIDDiameter             = detectorDiameter;
    WCIDVerticalPosition     = verticalPosition;

	WCBarrelPMTOffset     = vessel_tot_height; //WCPMTRadius;
    WCPMTperCellHorizontal = 1.0;
    WCPMTperCellVertical   = 1.0;
    WCPMTPercentCoverage   = PMTCoverage;  
    WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage/100.0)/WCPMTRadius);

    WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/WCPMTperCellVertical));
    WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
    WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
    WCBlackSheetThickness = 2.0*cm;
    WCAddGd               = false;
}








// Trevor Towstego: detector with single module
// ToDo: check defaults for mPMT.
void WCSimDetectorConstruction::SetTestSinglemPMTGeometry()
{
  WCDetectorName = "TestSinglemPMT";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  
  mPMT_ID_PMT = "PMT3inchR12199_02";
  mPMT_OD_PMT = "PMT8inch";
  
  WCSimPMTObject * PMT = CreatePMTObject(mPMT_ID_PMT, WCIDCollectionName);
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight();
  WCPMTRadius = PMT->GetRadius();
  
  vessel_cyl_height = 0.;
  vessel_radius_curv = 270.*CLHEP::mm;
  vessel_radius = 225.*CLHEP::mm;
  dist_pmt_vessel = 5*CLHEP::mm;
  orientation = PERPENDICULAR;
  mPMT_outer_material = "G4_PLEXIGLASS";
  mPMT_inner_material = "SilGel";
  mPMT_outer_material_d = 0.03*2*vessel_radius*CLHEP::mm;
  id_reflector_height = 7.5*CLHEP::mm;
  id_reflector_z_offset = 1.1*CLHEP::mm; //4.8*CLHEP::mm
  id_reflector_angle = CLHEP::pi/4*CLHEP::rad;
  G4double vessel_tot_height = vessel_radius + vessel_cyl_height;
  nID_PMTs = 33;
  config_file = "mPMTconfig_33_13_1.txt";
  
  WCIDDiameter             = 10.0*m;
  WCIDHeight               = 30.0*m;
  WCBarrelPMTOffset        = vessel_tot_height;
  WCBarrelNumPMTHorizontal = 3;
  WCBarrelNRings           = 3;
  WCPMTperCellHorizontal   = 1;
  WCPMTperCellVertical     = 1;
  WCCapEdgeLimit           = WCIDDiameter/2.0 - vessel_tot_height;
  WCCapPMTSpacing          = WCIDDiameter*2.0;
  WCBlackSheetThickness    = 2.0*cm;
  WCAddGd                  = false;
}


void WCSimDetectorConstruction::SetTestmPMTGeometry()
{

  WCDetectorName = "TestmPMT";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";

  mPMT_ID_PMT = "PMT3inchR12199_02"; //"BoxandLine20inchHQE";// (combine with nPMT=1);//"PMT3inchR12199_02"; 
  mPMT_OD_PMT = "PMT8inch";         // Only used for the unique string name of mPMT for now!
  WCSimPMTObject * PMT = CreatePMTObject(mPMT_ID_PMT, WCIDCollectionName);
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight(); 
  WCPMTRadius = PMT->GetRadius(); 

                                    
  //mPMT params:
  vessel_cyl_height = 10.*mm;//20.0*mm;//50.*mm;
  vessel_radius_curv = 300.*CLHEP::mm;//260.*mm;//245.*mm;
  vessel_radius = 300.*CLHEP::mm;
  dist_pmt_vessel = 5*CLHEP::mm;    // from KM3Net tech drawing. Don't put PMT directly against pressure vessel. Gel helps transition in refractive indices. Also prevents daughters from sticking out vessel. Smaller will create Overlaps! Avoid Overlaps!!!
  orientation = PERPENDICULAR;
  mPMT_outer_material = "G4_PLEXIGLASS";
  mPMT_inner_material = "SilGel";
  mPMT_outer_material_d = 10.*CLHEP::mm;
  // Radius of cone at z=reflectorHeight
  id_reflector_height = 10.*CLHEP::mm;           //10. > previous 7mm (deprecated number from JINST)
  id_reflector_z_offset = 4.8*CLHEP::mm;         //from KM3Net CAD drawings
  id_reflector_angle = 48*CLHEP::pi/180.*CLHEP::rad; // Based on KM3Net reflector specs

  
  // BarrelPMTOffset/WCCapEdgeLimit needs PMT/mPMT height
  G4double vessel_tot_height = vessel_radius + vessel_cyl_height;
  // parameters related to filling the ID mPMT
  nID_PMTs = 33;
  config_file = "mPMTconfig_33_13_1.txt";//"mPMTconfig_3_13_2.txt"; //mPMTconfig_30_13_3.txt


  WCIDDiameter          = 33.6815*m; //inner detector diameter
  WCIDHeight            = 36.200*m;
  WCBarrelPMTOffset     = vessel_tot_height; //offset from vertical
  WCBarrelNumPMTHorizontal  = 100; 
  WCBarrelNRings        = 3; //all for easier visualization debugging
  WCPMTperCellHorizontal= 2;
  WCPMTperCellVertical  = 2;
  WCCapPMTSpacing       = 8.4*m; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - vessel_tot_height;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;

}


// Note: using vessel_radius (mPMT radius) instead of PMT radius for detector construction. Important!
void WCSimDetectorConstruction::Cylinder_60x74_3inchmPMT_14perCent()
{ 
  WCDetectorName = "Cylinder_60x74_3inchmPMT_14perCent";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  mPMT_ID_PMT = "PMT3inchR12199_02";
  mPMT_OD_PMT = "PMT8inch";
  WCSimPMTObject * PMT = CreatePMTObject(mPMT_ID_PMT, WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = 0.254*m;//outer_module_radius;//PMT-T>GetRadius(); //Actually, this name is very misleading, because it should be moduleRadius (eg. PMT + acrylic), this is used in two places in WCConstructCylinder
 


  //mPMT params go first because detector depends on it:
  vessel_cyl_height = 0.*CLHEP::mm; // the 50mm should exist only for OD and extends behind the iD wall
  vessel_radius_curv = 270.*CLHEP::mm;
  vessel_radius = 270.*CLHEP::mm;
  dist_pmt_vessel = 5*CLHEP::mm;
  orientation = PERPENDICULAR;
  mPMT_outer_material = "G4_PLEXIGLASS";
  mPMT_inner_material = "SilGel";
  mPMT_outer_material_d = 0.03*2* vessel_radius *CLHEP::mm; //using NEMO article safety margin of 0.03 for t/D
  // Radius of cone at z=reflectorHeight
  id_reflector_height = 7.5*CLHEP::mm;         //7.5mm from KM3Net JINST
  id_reflector_z_offset = 1.1*CLHEP::mm;         //from KM3Net CAD drawings
  id_reflector_angle = CLHEP::pi/4*CLHEP::rad; // Based on KM3Net JINST: 45 deg wrt normal, so 7.5mm xtra
  G4double vessel_tot_height = vessel_radius + vessel_cyl_height;

  // parameters related to filling the ID mPMT
  nID_PMTs = 33;
  config_file = "mPMTconfig_33_13_1.txt";

  // Note: in principal WCPMTRadius and WCCapEdgeLimit could also use outer_module_radius
  // but then total n mPMT modules change slightly. This configuration wants to have NO overlaps
  // and use the same number of mPMT modules as 20in PMTs, so use same settings.
  // Most important one is WCBarrelNumPMTHorizontal
  // WCBarrelPMTOffset does not change total No. of modules, so use actual outer_module_radius.
  WCIDDiameter          = 74.0*m;
  WCIDHeight            = 60.0*m;
  WCBarrelPMTOffset     = vessel_tot_height; //mPMT cylinder Radius //WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 13.51;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*0.254*m));   //Used same radius as 20in to have same granularity in mPMTs (28% difference otherwise
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - 0.254*m;//outer_module_radius;//WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;


}

// Note: using vessel_radius (mPMT radius) instead of PMT radius for detector construction. Important!
void WCSimDetectorConstruction::Cylinder_60x74_3inchmPMT_40perCent()
{ 
  WCDetectorName = "Cylinder_60x74_3inchmPMT_40perCent";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  mPMT_ID_PMT = "PMT3inchR12199_02";
  mPMT_OD_PMT = "PMT8inch";
  WCSimPMTObject * PMT = CreatePMTObject(mPMT_ID_PMT, WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = 0.254*m;

  //mPMT params:
  vessel_cyl_height = 0.*CLHEP::mm; // the 50mm should exist only for OD and extends behind the iD wall
  vessel_radius_curv = 280.*CLHEP::mm;   
  vessel_radius = 280.*CLHEP::mm;   
  dist_pmt_vessel = 5*CLHEP::mm;
  orientation = PERPENDICULAR;
  mPMT_outer_material = "G4_PLEXIGLASS";
  mPMT_inner_material = "SilGel";
  mPMT_outer_material_d = 0.03*2*vessel_radius*CLHEP::mm; //using NEMO article safety margin of 0.03 for t/D
  // Radius of cone at z=reflectorHeight
  id_reflector_height = 7.5*CLHEP::mm;         //7.5mm from KM3Net JINST
  id_reflector_z_offset = 1.1*CLHEP::mm;         //from KM3Net CAD drawings
  id_reflector_angle = CLHEP::pi/4*CLHEP::rad; // Based on KM3Net JINST: 45 deg wrt normal, so 7.5mm xtra
  G4double vessel_tot_height = vessel_radius + vessel_cyl_height;

  // parameters related to filling the ID mPMT
  nID_PMTs = 34;
  config_file = "mPMTconfig_34_22.5_2.txt"; 


  WCIDDiameter          = 74.0*m;
  WCIDHeight            = 60.0*m;
  WCBarrelPMTOffset     = vessel_tot_height; //mPMT cylinder Radius //WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 40.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*0.254*m));//WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - 0.254*m;//WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;


}

// Uniform distribution of 3" PMTs
void WCSimDetectorConstruction::Cylinder_60x74_3inch_14perCent()
{ 
  WCDetectorName = "Cylinder_60x74_3inch_14perCent";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("PMT3inchR12199_02", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = .04*m; //PMT->GetRadius();
  WCIDDiameter          = 74.0*m;
  WCIDHeight            = 60.0*m;
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 13.51;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));  
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;

  InitSinglePMT();
}

void WCSimDetectorConstruction::Cylinder_60x74_3inch_40perCent()
{ 
  WCDetectorName = "Cylinder_60x74_3inch_40perCent";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("PMT3inchR12199_02", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = .04*m; //PMT->GetRadius();
  WCIDDiameter          = 74.0*m;
  WCIDHeight            = 60.0*m;
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 40.;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));  
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;

  InitSinglePMT();
}



void WCSimDetectorConstruction::InitSinglePMT(){
  
  vessel_cyl_height = 0.*mm;
  vessel_radius_curv = 0.1*mm;
  vessel_radius = 0.1*mm;
  dist_pmt_vessel = 0.*mm;
  orientation = PERPENDICULAR;
  mPMT_ID_PMT = "";
  mPMT_OD_PMT = "";
  mPMT_outer_material = "";
  mPMT_inner_material = "";
  mPMT_outer_material_d = 0.*CLHEP::mm;
  id_reflector_height = 0.*CLHEP::mm;
  id_reflector_z_offset = 0.*CLHEP::mm;
  id_reflector_angle = 0.*CLHEP::rad; 
  nID_PMTs = 1;   
  config_file = "";

}
