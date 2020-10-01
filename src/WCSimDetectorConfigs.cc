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
//B. Quilain, HK with 20'' PMTs
void WCSimDetectorConstruction::SetHyperK_SKPMTGeometry()
{
  WCDetectorName = "HyperK_SKPMT";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("PMT20inch", WCIDCollectionName);

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

//B. Quilain, HK with 10'' PMTs
void WCSimDetectorConstruction::SetHyperK_10inchGeometry()
{
  WCDetectorName = "HyperK_10inch";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("PMT10inch", WCIDCollectionName);

  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
  WCIDHeight            = 54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 40.0;//10.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

//B. Quilain, HK with 8'' PMTs
void WCSimDetectorConstruction::SetHyperK_8inchGeometry()
{
  WCDetectorName = "HyperK_8inch";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("PMT8inch", WCIDCollectionName);

  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
  WCIDHeight            = 54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 6.4;//40.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

//B. Quilain, HK with 3'' PMTs
void WCSimDetectorConstruction::SetHyperK_3inchGeometry()
{
  WCDetectorName = "HyperK_3inch";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("PMT3inchR12199_02", WCIDCollectionName);

  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
  WCIDHeight            = 54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 1;//40.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}
// B.Quilain, HK with mPMTs
// These are defaults that can be altered through the macros
void WCSimDetectorConstruction::SetHyperK_mPMTGeometry()
{
    WCDetectorName = "HyperK_mPMT";
	hybrid=false;
    WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
	mPMT_ID_PMT = "PMT3inchR14374";    //can be changed in macro through mPMT settings.
	mPMT_OD_PMT = "PMT3inchR14374";
  //mPMT_ID_PMT = "PMT3inchR12199_02";    //can be changed in macro through mPMT settings.
  //mPMT_OD_PMT = "PMT3inchR12199_02";
    WCSimPMTObject * PMT = CreatePMTObject(mPMT_ID_PMT, WCIDCollectionName);
    WCPMTName = PMT->GetPMTName();
    WCPMTExposeHeight = PMT->GetExposeHeight();
    WCPMTRadius = PMT->GetRadius();//Radius of a small PMT, not of a module

	//mPMT params go first because detector depends on it:
	mPMT_vessel_cyl_height = 38.*CLHEP::mm;    //option A, option B would be 277 mm
	mPMT_vessel_radius_curv = 342.*CLHEP::mm;  //needs to include the vessel thickness, as we construct from outside inwards.
	mPMT_vessel_radius = 254.*CLHEP::mm;
	dist_pmt_vessel = 2*CLHEP::mm;      
	orientation = PERPENDICULAR;
	mPMT_outer_material = "G4_PLEXIGLASS";
	mPMT_inner_material = "Air";               // TODO: real air, hence update abs_length
	mPMT_material_pmtAssembly = "SilGel";
	mPMT_outer_material_d = 10*CLHEP::mm;
	
	// Radius of cone at z=reflectorHeight
	id_reflector_height = 0;//6.53*CLHEP::mm;        // for a radius of 7.25mm, for hex: 5.4mm (radius of 6mm)
	id_reflector_z_offset = 0;//4.8*CLHEP::mm;       //from KM3Net CAD drawings
	id_reflector_angle = 0;//48.*CLHEP::deg;         // Need to be remeasured for different PMT curvature 
	mPMT_pmt_openingAngle = 8.7*CLHEP::deg;     // for hex: 8.5deg
	G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;
	
	// parameters related to filling the ID mPMT
	nID_PMTs = 19;
	config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_19_nuPrism_3ring.txt"; // for smaller reflector, use: mPMTconfig_19_nuPrism.txt (hex)

	WCBarrelPMTOffset     = mPMT_vessel_tot_height;
	WCIDVerticalPosition     = 0.;

	WCIDDiameter          = 70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
	WCIDHeight            = 54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
	WCPMTperCellHorizontal= 1;//4;
	WCPMTperCellVertical  = 1;//3;
	WCPMTPercentCoverage  = 30.0;//40.0;
	WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*mPMT_vessel_radius));
	G4cout << "## Number of Horizontal PMT along diameter = " << WCBarrelNumPMTHorizontal << ", diameter = "<< WCIDDiameter << ", coverage = "<< WCPMTPercentCoverage << ", PMT radius = " << WCPMTRadius << ", test = " << WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius) << G4endl;
	WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
	WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
	WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
	WCBlackSheetThickness = 2.0*cm;
	WCAddGd               = false;
}

// B.Quilain, HK with hybrid configuration: mPMTs + B&L
// These are defaults that can be altered through the macros
void WCSimDetectorConstruction::SetHyperK_HybridmPMTGeometry()
{
  //Box&Line
  WCDetectorName = "HyperK_HybridmPMT";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine20inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  nID_PMTs = 1;
  
  //mPMT
  hybrid = true;
  WCIDCollectionName2 = WCDetectorName +"-glassFaceWCPMT2";
  mPMT_ID_PMT = "PMT3inchR14374";    //can be changed in macro through mPMT settings.
  mPMT_OD_PMT = "PMT3inchR14374";
  WCSimPMTObject * PMT2 = CreatePMTObject(mPMT_ID_PMT, WCIDCollectionName2);
  WCPMTName2 = PMT2->GetPMTName();
  WCPMTExposeHeight2 = PMT2->GetExposeHeight();
  WCPMTRadius2 = PMT2->GetRadius();//Radius of a small PMT, not of a module
  nID_PMTs2 = 19;

  //mPMT params go first because detector depends on it:
  mPMT_vessel_cyl_height = 38.*CLHEP::mm;    //option A, option B would be 277 mm
  mPMT_vessel_radius_curv = 342.*CLHEP::mm;  //needs to include the vessel thickness, as we construct from outside inwards.
  mPMT_vessel_radius = 254.*CLHEP::mm;
  dist_pmt_vessel = 2*CLHEP::mm;
  
  orientation = PERPENDICULAR;
  mPMT_outer_material = "G4_PLEXIGLASS";
  mPMT_inner_material = "Air";               // TODO: real air, hence update abs_length
  mPMT_material_pmtAssembly = "SilGel";
  mPMT_outer_material_d = 10*CLHEP::mm;
  // Radius of cone at z=reflectorHeight
  id_reflector_height = 0;//6.53*CLHEP::mm;        // for a radius of 7.25mm, for hex: 5.4mm (radius of 6mm)
  id_reflector_z_offset = 0;//4.8*CLHEP::mm;       //from KM3Net CAD drawings
  id_reflector_angle = 0;//48.*CLHEP::deg;         // Need to be remeasured for different PMT curvature 
  mPMT_pmt_openingAngle = 8.7*CLHEP::deg;     // for hex: 8.5deg
  // parameters related to filling the ID mPMT
  config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_19_nuPrism_3ring.txt"; // for smaller reflector, use: mPMTconfig_19_nuPrism.txt (hex)

  G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;	
 

  WCIDDiameter          = 70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
  WCIDHeight            = 54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
  WCBarrelPMTOffset     = std::max(WCPMTRadius,mPMT_vessel_tot_height); //offset from vertical
  WCIDVerticalPosition     = 0.;

  WCPMTPercentCoverage  = 20.0;//20.0;//40.0;//40.0;  
  WCPMTPercentCoverage2  = 5.;//5.0;//20.0;
  WCPMTperCellHorizontal = round((WCPMTPercentCoverage+WCPMTPercentCoverage2) / (WCPMTPercentCoverage2==0?(WCPMTPercentCoverage+WCPMTPercentCoverage2):WCPMTPercentCoverage2)); 
  WCPMTperCellVertical = WCPMTperCellHorizontal;
  //WCPMTperCellHorizontal= ((int) WCPMTPercentCoverage2/(WCPMTPercentCoverage+WCPMTPercentCoverage2));//Should be in agreement with coverage of PMT 1 & 2: here, 4/5 PMTs are B&L + 1/5 mPMT
  //WCPMTperCellVertical  = ((int) WCPMTPercentCoverage2/(WCPMTPercentCoverage+WCPMTPercentCoverage2));//Should be in agreement with coverage of PMT 1 & 2: here, 4/5 PMTs are B&L + 1/5 mPMT

  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*(WCPMTPercentCoverage+WCPMTPercentCoverage2))/(10.0*std::max(WCPMTRadius,mPMT_vessel_radius)));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - 5*std::max(WCPMTExposeHeight,mPMT_vessel_tot_height);//3.0*cm;//WCIDDiameter/2.0 - std::max(WCPMTRadius,mPMT_vessel_tot_height);//WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

// B.Quilain, HK with hybrid configuration: mPMTs + B&L
// These are defaults that can be altered through the macros
void WCSimDetectorConstruction::SetHyperK_HybridmPMT10PCGeometry()
{
  //Box&Line
  WCDetectorName = "HyperK_HybridmPMT10PC";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine20inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  nID_PMTs = 1;
  
  //mPMT
  hybrid = true;
  WCIDCollectionName2 = WCDetectorName +"-glassFaceWCPMT2";
  mPMT_ID_PMT = "PMT3inchR14374";    //can be changed in macro through mPMT settings.
  mPMT_OD_PMT = "PMT3inchR14374";
  WCSimPMTObject * PMT2 = CreatePMTObject(mPMT_ID_PMT, WCIDCollectionName2);
  WCPMTName2 = PMT2->GetPMTName();
  WCPMTExposeHeight2 = PMT2->GetExposeHeight();
  WCPMTRadius2 = PMT2->GetRadius();//Radius of a small PMT, not of a module
  nID_PMTs2 = 19;
  
  //mPMT params go first because detector depends on it:
  mPMT_vessel_cyl_height = 38.*CLHEP::mm;    //option A, option B would be 277 mm
  mPMT_vessel_radius_curv = 342.*CLHEP::mm;  //needs to include the vessel thickness, as we construct from outside inwards.
  mPMT_vessel_radius = 254.*CLHEP::mm;
  dist_pmt_vessel = 2*CLHEP::mm;
  
  orientation = PERPENDICULAR;
  mPMT_outer_material = "G4_PLEXIGLASS";
  mPMT_inner_material = "Air";               // TODO: real air, hence update abs_length
  mPMT_material_pmtAssembly = "SilGel";
  mPMT_outer_material_d = 10*CLHEP::mm;
  // Radius of cone at z=reflectorHeight
  id_reflector_height = 0;//6.53*CLHEP::mm;        // for a radius of 7.25mm, for hex: 5.4mm (radius of 6mm)
  id_reflector_z_offset = 0;//4.8*CLHEP::mm;       //from KM3Net CAD drawings
  id_reflector_angle = 0;//48.*CLHEP::deg;         // Need to be remeasured for different PMT curvature 
  mPMT_pmt_openingAngle = 8.7*CLHEP::deg;     // for hex: 8.5deg
  // parameters related to filling the ID mPMT
  config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_19_nuPrism_3ring.txt"; // for smaller reflector, use: mPMTconfig_19_nuPrism.txt (hex)

  G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;	
 

  WCIDDiameter          = 70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
  WCIDHeight            = 54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
  WCBarrelPMTOffset     = std::max(WCPMTRadius,mPMT_vessel_tot_height); //offset from vertical
  WCIDVerticalPosition     = 0.;

  WCPMTPercentCoverage  = 20.0;//40.0;//40.0;  
  WCPMTPercentCoverage2  = 10.0;//20.0;
  WCPMTperCellHorizontal = round((WCPMTPercentCoverage+WCPMTPercentCoverage2) / (WCPMTPercentCoverage2==0?(WCPMTPercentCoverage+WCPMTPercentCoverage2):WCPMTPercentCoverage2)); 
  WCPMTperCellVertical = WCPMTperCellHorizontal;
  //WCPMTperCellHorizontal= ((int) WCPMTPercentCoverage2/(WCPMTPercentCoverage+WCPMTPercentCoverage2));//Should be in agreement with coverage of PMT 1 & 2: here, 4/5 PMTs are B&L + 1/5 mPMT
  //WCPMTperCellVertical  = ((int) WCPMTPercentCoverage2/(WCPMTPercentCoverage+WCPMTPercentCoverage2));//Should be in agreement with coverage of PMT 1 & 2: here, 4/5 PMTs are B&L + 1/5 mPMT

  //WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*(WCPMTPercentCoverage+WCPMTPercentCoverage2))/(10.0*std::max(WCPMTRadius,mPMT_vessel_tot_height)));
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*(WCPMTPercentCoverage+WCPMTPercentCoverage2))/(10.0*std::max(WCPMTRadius,mPMT_vessel_radius)));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  //G4cout << "## Number of Horizontal PMT along diameter = " << WCBarrelNumPMTHorizontal << ", diameter = "<< WCIDDiameter << ", coverage = "<< WCPMTPercentCoverage << ", PMT radius = " << WCPMTRadius << ", test = " << WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius) << G4endl;
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - 5*std::max(WCPMTExposeHeight,mPMT_vessel_tot_height);//3.0*cm;//WCIDDiameter/2.0 - std::max(WCPMTRadius,mPMT_vessel_tot_height);//WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

// B.Quilain, HK with hybrid configuration: mPMTs + B&L
// These are defaults that can be altered through the macros
void WCSimDetectorConstruction::SetHyperK_HybridFakeGeometry()
{
  WCDetectorName = "HyperK_HybridFake";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  mPMT_ID_PMT = "PMT3inchR12199_02";    //can be changed in macro through mPMT settings.
  mPMT_OD_PMT = "PMT3inchR12199_02";
  WCSimPMTObject * PMT = CreatePMTObject(mPMT_ID_PMT, WCIDCollectionName);
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight();
  WCPMTRadius = PMT->GetRadius();//Radius of a small PMT, not of a module
  nID_PMTs = 1;
  
  //mPMT params go first because detector depends on it:
  mPMT_vessel_cyl_height = 38.*CLHEP::mm;    //option A, option B would be 277 mm
  mPMT_vessel_radius_curv = 342.*CLHEP::mm;  //needs to include the vessel thickness, as we construct from outside inwards.
  mPMT_vessel_radius = 254.*CLHEP::mm;
  dist_pmt_vessel = 2*CLHEP::mm;      
  orientation = PERPENDICULAR;
  mPMT_outer_material = "G4_PLEXIGLASS";
  mPMT_inner_material = "Air";               // TODO: real air, hence update abs_length
  mPMT_material_pmtAssembly = "SilGel";
  mPMT_outer_material_d = 10*CLHEP::mm;
  
  // Radius of cone at z=reflectorHeight
  id_reflector_height = 6.53*CLHEP::mm;        // for a radius of 7.25mm, for hex: 5.4mm (radius of 6mm)
  id_reflector_z_offset = 4.8*CLHEP::mm;       //from KM3Net CAD drawings
  id_reflector_angle = 48.*CLHEP::deg;         // Need to be remeasured for different PMT curvature 
  mPMT_pmt_openingAngle = 8.7*CLHEP::deg;     // for hex: 8.5deg
  G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;

  //Box&Line
  //WCDetectorName2 = "HyperK_HybridmPMT";
  WCIDCollectionName2 = WCDetectorName +"-glassFaceWCPMT2";
  WCSimPMTObject * PMT2 = CreatePMTObject("BoxandLine20inchHQE", WCIDCollectionName2);
  WCPMTName2           = PMT2->GetPMTName();
  WCPMTExposeHeight2   = PMT2->GetExposeHeight();
  WCPMTRadius2         = PMT2->GetRadius();
  nID_PMTs2 = 1;
  //WCPMTVesselCycHeight = 0;
  //WCPMTVesselRadiusCurv = 0;
  //WCPMTVesselRadius = 0;
  //WCPMTDistPMTVessel = 0;

  //WCDetectorName2 = "HyperK_HybridmPMT";
  //WCIDCollectionName2 = WCDetectorName +"-glassFaceWCPMT";
  //WCSimPMTObject * PMT2 = CreatePMTObject("BoxandLine20inchHQE", WCIDCollectionName2);
  //WCPMTName2           = PMT2->GetPMTName();
  //WCPMTExposeHeight2   = PMT2->GetExposeHeight();
  //WCPMTRadius2         = PMT2->GetRadius();
  //nID_PMTs = 1;
 

  WCIDDiameter          = 70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
  WCIDHeight            = 54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
  WCBarrelPMTOffset     = std::max(WCPMTRadius,mPMT_vessel_tot_height); //offset from vertical
  WCIDVerticalPosition     = 0.;

  WCPMTPercentCoverage  = 20.0;//20.0;//40.0;//40.0;  
  WCPMTPercentCoverage2  = 0;//5.0;//20.0;
  WCPMTperCellHorizontal = round((WCPMTPercentCoverage+WCPMTPercentCoverage2) / (WCPMTPercentCoverage2==0?(WCPMTPercentCoverage+WCPMTPercentCoverage2):WCPMTPercentCoverage2)); 
  WCPMTperCellVertical = WCPMTperCellHorizontal;
  //WCPMTperCellHorizontal= ((int) WCPMTPercentCoverage2/(WCPMTPercentCoverage+WCPMTPercentCoverage2));//Should be in agreement with coverage of PMT 1 & 2: here, 4/5 PMTs are B&L + 1/5 mPMT
  //WCPMTperCellVertical  = ((int) WCPMTPercentCoverage2/(WCPMTPercentCoverage+WCPMTPercentCoverage2));//Should be in agreement with coverage of PMT 1 & 2: here, 4/5 PMTs are B&L + 1/5 mPMT

  //WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*(WCPMTPercentCoverage+WCPMTPercentCoverage2))/(10.0*std::max(WCPMTRadius,mPMT_vessel_tot_height)));
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*(WCPMTPercentCoverage+WCPMTPercentCoverage2))/(10.0*std::max(WCPMTRadius,mPMT_vessel_radius)));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  //G4cout << "## Number of Horizontal PMT along diameter = " << WCBarrelNumPMTHorizontal << ", diameter = "<< WCIDDiameter << ", coverage = "<< WCPMTPercentCoverage << ", PMT radius = " << WCPMTRadius << ", test = " << WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius) << G4endl;
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - 5*std::max(WCPMTExposeHeight,mPMT_vessel_tot_height);//3.0*cm;//WCIDDiameter/2.0 - std::max(WCPMTRadius,mPMT_vessel_tot_height);//WCPMTRadius;
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
    SetNuPrismGeometry("PMT8inch", 40.0, 10*m, 8*m, 0*m);
}

// NuPRISM with mPMTs
// These are defaults that can be altered through the macros
void WCSimDetectorConstruction::SetNuPrism_mPMTGeometry()
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
	mPMT_vessel_cyl_height = 38.*CLHEP::mm;    //option A, option B would be 277 mm
	mPMT_vessel_radius_curv = 342.*CLHEP::mm;  //needs to include the vessel thickness, as we construct from outside inwards.
	mPMT_vessel_radius = 254.*CLHEP::mm;
	dist_pmt_vessel = 2*CLHEP::mm;      
	orientation = PERPENDICULAR;
	mPMT_outer_material = "G4_PLEXIGLASS";
	mPMT_inner_material = "Air";               // TODO: real air, hence update abs_length
	mPMT_material_pmtAssembly = "SilGel";
	mPMT_outer_material_d = 10*CLHEP::mm;
	
	// Radius of cone at z=reflectorHeight
	//id_reflector_height = 0.0*CLHEP::mm;        // for a radius of 7.25mm, for hex: 5.4mm (radius of 6mm)
	//B.Q
	//id_reflector_height = mPMT_ID_reflector_height;
	//id_reflector_angle = mPMT_ID_reflector_angle;
	//id_reflector_height = 8.*CLHEP::mm;        // for a radius of 7.25mm, for hex: 5.4mm (radius of 6mm)
	id_reflector_height = 6.53*CLHEP::mm;        // for a radius of 7.25mm, for hex: 5.4mm (radius of 6mm)
	id_reflector_z_offset = 4.8*CLHEP::mm;       //from KM3Net CAD drawings
	id_reflector_angle = 48.*CLHEP::deg;         // Need to be remeasured for different PMT curvature
	//id_reflector_height = 3.*CLHEP::mm;        // for a radius of 7.25mm, for hex: 5.4mm (radius of 6mm)
	//id_reflector_angle = 0.*CLHEP::deg;         // Need to be remeasured for different PMT curvature
	//End B.Q
	
	mPMT_pmt_openingAngle = 8.7*CLHEP::deg;     // for hex: 8.5deg
	G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;
	
	// parameters related to filling the ID mPMT
	nID_PMTs = 19;
	config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_19_nuPrism_3ring.txt"; // for smaller reflector, use: mPMTconfig_19_nuPrism.txt (hex)

	WCIDHeight               = 10.42*CLHEP::m;
    WCIDDiameter             = 7.42*CLHEP::m;
    WCIDVerticalPosition     = 0.;
	
	WCBarrelPMTOffset     = mPMT_vessel_tot_height;
    WCPMTperCellHorizontal = 2.0; // 2 per phi
    WCPMTperCellVertical   = 1.0;

	// Numbers below are based on R.Henderson's 832 module tank design
    WCBarrelNumPMTHorizontal = 40;
    WCBarrelNRings        = 16;
    WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
	// This is tuned to give a 6-5-5-4-3-1 config in one quarter of the cap
	// And results in 832 mPMT modules as currently in Robert's Design
    WCCapEdgeLimit        = 3.3*m; 
    WCBlackSheetThickness = 2.0*cm;    // deprecate soon.
    WCAddGd               = false;
}

// Short version of NuPRISM with mPMTs: 6 m tall ID
// These are defaults that can be altered through the macros
void WCSimDetectorConstruction::SetNuPrismShort_mPMTGeometry()
{
    WCDetectorName = "NuPRISMShort_mPMT";
    WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
	mPMT_ID_PMT = "PMT3inchR12199_02";    //can be changed in macro through mPMT settings.
	mPMT_OD_PMT = "PMT3inchR12199_02";
    WCSimPMTObject * PMT = CreatePMTObject(mPMT_ID_PMT, WCIDCollectionName);
    WCPMTName = PMT->GetPMTName();
    WCPMTExposeHeight = PMT->GetExposeHeight();
    WCPMTRadius = PMT->GetRadius();

	//mPMT params go first because detector depends on it:
	vessel_cyl_height = 38.*CLHEP::mm;    //option A, option B would be 277 mm
	vessel_radius_curv = 342.*CLHEP::mm;  //needs to include the vessel thickness, as we construct from outside inwards.
	vessel_radius = 254.*CLHEP::mm;
	dist_pmt_vessel = 2*CLHEP::mm;      
	orientation = PERPENDICULAR;
	mPMT_outer_material = "G4_PLEXIGLASS";
	mPMT_inner_material = "Air";               // TODO: real air, hence update abs_length
	mPMT_material_pmtAssembly = "SilGel";
	mPMT_outer_material_d = 10*CLHEP::mm;
	
	// Radius of cone at z=reflectorHeight
	id_reflector_height = 6.53*CLHEP::mm;        // for a radius of 7.25mm, for hex: 5.4mm (radius of 6mm)
	id_reflector_z_offset = 4.8*CLHEP::mm;       //from KM3Net CAD drawings
	id_reflector_angle = 48.*CLHEP::deg;         // Need to be remeasured for different PMT curvature 
	mPMT_pmt_openingAngle = 8.7*CLHEP::deg;     // for hex: 8.5deg
	G4double vessel_tot_height = vessel_radius + vessel_cyl_height;
	
	// parameters related to filling the ID mPMT
	nID_PMTs = 19;
	config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_19_nuPrism_3ring.txt"; // for smaller reflector, use: mPMTconfig_19_nuPrism.txt (hex)

	WCIDHeight               = 6.0*CLHEP::m;
	WCIDDiameter             = 8.0*CLHEP::m;
    WCIDVerticalPosition     = 0.;
	
	WCBarrelPMTOffset     = vessel_tot_height;
    WCPMTperCellHorizontal = 2.0; // 2 per phi
    WCPMTperCellVertical   = 1.0;

	// Numbers below are based on R.Henderson's 832 module tank design
    WCBarrelNumPMTHorizontal = 40;
    WCBarrelNRings        = 9; // Scaled from 10.42 m tall detector.
    WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
	// This is tuned to give a 6-5-5-4-3-1 config in one quarter of the cap
	// And results in 832 mPMT modules as currently in Robert's Design
    WCCapEdgeLimit        = 3.3*m; 
    WCBlackSheetThickness = 2.0*cm;    // deprecate soon.
    WCAddGd               = false;
}

void WCSimDetectorConstruction::SetTestSinglemPMTGeometry()
{
  WCDetectorName = "TestSinglemPMT";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  
  mPMT_ID_PMT = "PMT3inchR12199_02"; //"BoxandLine20inchHQE";// (combine with nPMT=1);
  mPMT_OD_PMT = "PMT8inch";          // Only used for the unique string name of mPMT for now!
  
  WCSimPMTObject * PMT = CreatePMTObject(mPMT_ID_PMT, WCIDCollectionName);
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight();
  WCPMTRadius = PMT->GetRadius();
  
  //mPMT params
  vessel_cyl_height = 38.*CLHEP::mm;
  vessel_radius_curv = 300.*CLHEP::mm;
  vessel_radius = 254.*CLHEP::mm;
  dist_pmt_vessel = 2*CLHEP::mm;
  orientation = PERPENDICULAR;
  mPMT_outer_material = "G4_PLEXIGLASS";
  mPMT_inner_material = "SilGel";
  mPMT_material_pmtAssembly = "SilGel";
  mPMT_outer_material_d = 10.*CLHEP::mm;
  // Radius of cone at z=reflectorHeight
  id_reflector_height = 10*CLHEP::mm;                //10. > previous 7mm (deprecated number from JINST)
  id_reflector_z_offset = 4.8*CLHEP::mm;            //from KM3Net CAD drawings
  id_reflector_angle = 48*CLHEP::pi/180.*CLHEP::rad; // Based on KM3Net reflector specs
  mPMT_pmt_openingAngle = 8.7*CLHEP::deg;     
  
  // BarrelPMTOffset/WCCapEdgeLimit needs PMT/mPMT height
  G4double vessel_tot_height = vessel_radius + vessel_cyl_height;
  // parameters related to filling the ID mPMT
  nID_PMTs = 33;
  config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_33_13_1.txt";

  WCIDDiameter             = 10.0*m;
  WCIDHeight               = 30.0*m;
  WCBarrelPMTOffset        = vessel_tot_height;
  WCBarrelNumPMTHorizontal = 3;
  WCBarrelNRings           = 3;
  WCPMTperCellHorizontal   = 1;
  WCPMTperCellVertical     = 1;
  WCPMTPercentCoverage     = 10.;
  WCCapEdgeLimit           = WCIDDiameter/2.0 - vessel_tot_height;
  WCCapPMTSpacing          = WCIDDiameter*2.0;
  WCBlackSheetThickness    = 2.0*cm;
  WCAddGd                  = false;
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
  config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_33_13_1.txt";

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
  config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_34_22.5_2.txt"; 


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
