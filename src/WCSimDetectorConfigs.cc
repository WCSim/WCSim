//  -*- mode:c++; tab-width:4;  -*-
#include "WCSimDetectorConstruction.hh"
#include "WCSimPMTObject.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include <cmath>

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

//Turn on the output of DumpDetectorConfigInfo()
//#define VERBOSE_GEOMETRY_CONFIG

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
  WCBarrelNumPMTHorizontal  = 152;
  WCBarrelNRings        = 17.;
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3; 
  WCCapPMTSpacing       = 0.707*m; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;
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
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;
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
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;
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
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;
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
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;
  InitSinglePMT();
}

void HKGeometryNoLongerUsed(G4String & DetectorName)
{
  G4cerr << "**********************************" << G4endl
		 << DetectorName << " is an obsolete HK geometry" << G4endl
		 << "You should use one of:" << G4endl
		 << " HyperK_HybridmPMT_WithOD" << G4endl
		 << " HyperK_HybridmPMT" << G4endl
		 << "If you really want to use " << DetectorName
		 << " then you can comment out the call to HKGeometryNoLongerUsed() in the method "
		 << "WCSimDetectorConstruction::" << DetectorName << "()"
		 << " in the file $WCSIMDIR/src/WCSimDetectorConfigs.cc and recompile, "
		 << "but be careful if comparing to official HK geometries that the number of PMTs are consistent" << G4endl
		 << "Exiting..." << G4endl;
  exit(-1);
}

void WCSimDetectorConstruction::Cylinder_60x74_20inchBandL_14perCent()
{ 
  WCDetectorName = "Cylinder_60x74_20inchBandL_14perCent()";
  HKGeometryNoLongerUsed(WCDetectorName);
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
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;
  InitSinglePMT();
}

void WCSimDetectorConstruction::Cylinder_60x74_20inchBandL_40perCent()
{ 
  WCDetectorName = "Cylinder_60x74_20inchBandL_40perCent";
  HKGeometryNoLongerUsed(WCDetectorName);
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
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;
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
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;
  InitSinglePMT();
}

void WCSimDetectorConstruction::SetHyperKGeometry()
{
  WCDetectorName = "HyperK";
  HKGeometryNoLongerUsed(WCDetectorName);
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine20inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 64.8*m;//70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
  WCIDHeight            = 65.751*m;//54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  if(WCPMTPercentCoverage < -0.1) WCPMTPercentCoverage  = 40.0;
  else if(WCPMTPercentCoverage < 0) WCPMTPercentCoverage = 0.0; //protection against negative coverage
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;
}

//B. Quilain, HK with 10'' PMTs
void WCSimDetectorConstruction::SetHyperK_10inchGeometry()
{
  WCDetectorName = "HyperK_10inch";
  HKGeometryNoLongerUsed(WCDetectorName);
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("PMT10inch", WCIDCollectionName);

  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 64.8*m;//70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
  WCIDHeight            = 65.751*m;//54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  if(WCPMTPercentCoverage < -0.1) WCPMTPercentCoverage  = 40.0;
  else if(WCPMTPercentCoverage < 0) WCPMTPercentCoverage = 0.0; //protection against negative coverage
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;
}

//B. Quilain, HK with 8'' PMTs
void WCSimDetectorConstruction::SetHyperK_8inchGeometry()
{
  WCDetectorName = "HyperK_8inch";
  HKGeometryNoLongerUsed(WCDetectorName);
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("PMT8inch", WCIDCollectionName);

  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 64.8*m;//70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
  WCIDHeight            = 65.751*m;//54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  if(WCPMTPercentCoverage < -0.1) WCPMTPercentCoverage  = 40.0;
  else if(WCPMTPercentCoverage < 0) WCPMTPercentCoverage = 0.0; //protection against negative coverage
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;
}

//B. Quilain, HK with 3'' PMTs
void WCSimDetectorConstruction::SetHyperK_3inchGeometry()
{
  WCDetectorName = "HyperK_3inch";
  HKGeometryNoLongerUsed(WCDetectorName);
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("PMT3inchR12199_02", WCIDCollectionName);

  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 64.8*m;//70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
  WCIDHeight            = 65.751*m;//54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  if(WCPMTPercentCoverage < -0.1) WCPMTPercentCoverage  = 40.0;
  else if(WCPMTPercentCoverage < 0) WCPMTPercentCoverage = 0.0; //protection against negative coverage
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;
}

void WCSimDetectorConstruction::SetHyperK20pcWithODGeometry()
{
  SetHyperKWithODGeometry();
  WCPMTPercentCoverage = 20.0;
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
									/WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
}

void WCSimDetectorConstruction::SetHyperKWithODGeometry()
{
  WCDetectorName = "HyperKWithOD";
  HKGeometryNoLongerUsed(WCDetectorName);
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine20inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 64.8*m; // = 68m - 2*(60cm ID wall + 1m OD)(69m with 0.5m concrete each side)
  WCIDHeight            = 65.8*m; // = 71m - 2*(60cm ID wall + 2m OD)(72m with 0.5m concrete on each side)
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 8;
  WCPMTperCellVertical  = 6;
  WCPMTPercentCoverage  = 40.0;
  WCBarrelNumPMTHorizontal = 288;
  WCBarrelNRings =
      std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;
  //////////////////////////////////////
  // Cave Parameters --- Beta version //
  //////////////////////////////////////
  CaveTyvekSheetThickness  = 1.*mm; // Quite Standard I guess

  ////////////////////////////////////
  // OD Parameters --- Beta version //
  ////////////////////////////////////
  isODConstructed = true;

  // OD Dimensions //
  WCODLateralWaterDepth    = 1.*m;
  WCODHeightWaterDepth     = 2.*m;
  WCODDeadSpace            = 600.*mm;
  WCODTyvekSheetThickness  = 1.*mm; // Quite standard I guess
  WCODWLSPlatesThickness   = 1.*cm; //
  WCODWLSPlatesLength      = 60.*cm; //
  WCODDiameter             = WCIDDiameter + 2*(WCBlackSheetThickness+WCODDeadSpace+WCODTyvekSheetThickness+WCODWLSPlatesThickness);

  // OD PMTs //
  WCODCollectionName = WCDetectorName + "-glassFaceWCPMT_OD";
  WCSimPMTObject *PMTOD = CreatePMTObject("PMT8inch", WCODCollectionName);
  WCPMTODName           = PMTOD->GetPMTName();
  WCPMTODExposeHeight   = PMTOD->GetExposeHeight();
  WCPMTODRadius         = PMTOD->GetRadius();

  // OD Coverage on barrel side //
  WCPMTODperCellHorizontal = 1;
  WCPMTODperCellVertical   = 1;

  // OD Coverage on caps //
  WCPMTODPercentCoverage   = 1.0; //default 1%
  // NOTE : If you set WCPMTODperCellHorizontal=0 and WCPMTODperCellVertical=0,
  // then method ComputeWCODPMT() inside ConstructCylinder will automatically compute
  // the nb of PMTs to put on barrel side according to WCPMTODPercentCoverage

  // Shift between PMTs inside a cell //
  WCODPMTShift = 0.*cm;

  // OD caps //
  // WCODCapPMTSpacing = 100*cm;
  WCODCapPMTSpacing  = (pi*WCIDDiameter/(round(WCIDDiameter*sqrt(pi*WCPMTODPercentCoverage)/(10.0*WCPMTODRadius))));
  WCODCapEdgeLimit = WCIDDiameter/2.0 - WCODWLSPlatesLength/2;

  // TEST combined PMT collection for stacking action
  std::vector<G4String> WCColName;
  WCColName.push_back(WCIDCollectionName);
  WCColName.push_back(WCODCollectionName);
  CreateCombinedPMTQE(WCColName);
  isCombinedPMTCollectionDefined=true;

  // TEST WLS collection for stacking action
  G4String WLSType = "EljenEJ286";
  isWLSFilled = true;
  BuildODWLSCladding = true;
  CreateWLSObject(WLSType);
}

void WCSimDetectorConstruction::SetHyperKGeometry_20perCent()
{
  WCDetectorName = "HyperK_20perCent";
  HKGeometryNoLongerUsed(WCDetectorName);
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
  WCPMTPercentCoverage  = 20.0;
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;
}
// B.Quilain, HK with mPMTs
// These are defaults that can be altered through the macros
void WCSimDetectorConstruction::SetHyperK_mPMTGeometry()
{
    WCDetectorName = "HyperK_mPMT";
	HKGeometryNoLongerUsed(WCDetectorName);
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
	dist_pmt_vessel = 8*CLHEP::mm;      
	orientation = PERPENDICULAR;
	mPMT_outer_material = "G4_PLEXIGLASS";
	mPMT_inner_material = "Air";               // TODO: real air, hence update abs_length
	mPMT_material_pmtAssembly = "SilGel";
	mPMT_outer_material_d = 10*CLHEP::mm;
	
	// Radius of cone at z=reflectorHeight
	id_reflector_height = 0;//9.29*CLHEP::mm;        // for a radius of 7.25mm, for hex: 5.4mm (radius of 6mm)
	id_reflector_z_offset = 0;//6.12*CLHEP::mm;       //from KM3Net CAD drawings
	id_reflector_angle = 0;//46.5*CLHEP::deg;         // Need to be remeasured for different PMT curvature 
	mPMT_pmt_openingAngle = 8.3*CLHEP::deg;     // for hex: 8.5deg
	G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;
	
	// parameters related to filling the ID mPMT
	nID_PMTs = 19;
	config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_19_nuPrism_3ring.txt"; // for smaller reflector, use: mPMTconfig_19_nuPrism.txt (hex)

	WCBarrelPMTOffset     = mPMT_vessel_tot_height;
	WCIDVerticalPosition     = 0.;

	WCIDDiameter          = 64.8*m;//70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
	WCIDHeight            = 65.751*m;//54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
	WCPMTperCellHorizontal= 1;//4;
	WCPMTperCellVertical  = 1;//3;
	if(WCPMTPercentCoverage < -0.1) WCPMTPercentCoverage  = 40.0;
	else if(WCPMTPercentCoverage < 0) WCPMTPercentCoverage = 0.0; //protection against negative coverage
	WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*mPMT_vessel_radius));
	G4cout << "## Number of Horizontal PMT along diameter = " << WCBarrelNumPMTHorizontal << ", diameter = "<< WCIDDiameter << ", coverage = "<< WCPMTPercentCoverage << ", PMT radius = " << WCPMTRadius << ", test = " << WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius) << G4endl;
	WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
	WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
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
  dist_pmt_vessel = 8*CLHEP::mm;
  
  orientation = PERPENDICULAR;
  mPMT_outer_material = "G4_PLEXIGLASS";
  mPMT_inner_material = "Air";               // TODO: real air, hence update abs_length
  mPMT_material_pmtAssembly = "SilGel";
  mPMT_outer_material_d = 10*CLHEP::mm;
  // Radius of cone at z=reflectorHeight
  id_reflector_height = 0;//9.29*CLHEP::mm;        // for a radius of 7.25mm, for hex: 5.4mm (radius of 6mm)
  id_reflector_z_offset = 0;//6.12*CLHEP::mm;       //from KM3Net CAD drawings
  id_reflector_angle = 0;//46.5*CLHEP::deg;         // Need to be remeasured for different PMT curvature 
  mPMT_pmt_openingAngle = 8.3*CLHEP::deg;     // for hex: 8.5deg
  // parameters related to filling the ID mPMT
  config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_19_nuPrism_3ring.txt"; // for smaller reflector, use: mPMTconfig_19_nuPrism.txt (hex)

  G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;	
 

  WCIDDiameter          = 64.8*m;//70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
  WCIDHeight            = 65.751*m;//54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
  WCBarrelPMTOffset     = std::max(WCPMTRadius,mPMT_vessel_tot_height) + 1.*CLHEP::mm; //offset from vertical
  WCIDVerticalPosition     = 0.;

  if(WCPMTPercentCoverage < -0.1) WCPMTPercentCoverage  = 20.2150576375662;
  else if(WCPMTPercentCoverage < 0) WCPMTPercentCoverage = 0.0; //protection against negative coverage
  if(WCPMTPercentCoverage2 < -0.1) WCPMTPercentCoverage2  = 10.1075288187831;
  else if(WCPMTPercentCoverage2 < 0) WCPMTPercentCoverage2 = 0.0; //protection against negative coverage
  WCPMTperCellHorizontal = std::lround((WCPMTPercentCoverage+WCPMTPercentCoverage2) / (WCPMTPercentCoverage2==0?(WCPMTPercentCoverage+WCPMTPercentCoverage2):WCPMTPercentCoverage2)); 
  WCPMTperCellVertical = WCPMTperCellHorizontal;
  //WCPMTperCellHorizontal= ((int) WCPMTPercentCoverage2/(WCPMTPercentCoverage+WCPMTPercentCoverage2));//Should be in agreement with coverage of PMT 1 & 2: here, 4/5 PMTs are B&L + 1/5 mPMT
  //WCPMTperCellVertical  = ((int) WCPMTPercentCoverage2/(WCPMTPercentCoverage+WCPMTPercentCoverage2));//Should be in agreement with coverage of PMT 1 & 2: here, 4/5 PMTs are B&L + 1/5 mPMT

  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*(WCPMTPercentCoverage+WCPMTPercentCoverage2))/(10.0*std::max(WCPMTRadius,mPMT_vessel_radius)));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  //WCCapEdgeLimit        = WCIDDiameter/2.0 - 5*std::max(WCPMTExposeHeight,mPMT_vessel_tot_height);//3.0*cm;//WCIDDiameter/2.0 - std::max(WCPMTRadius,mPMT_vessel_tot_height);//WCPMTRadius;
  WCCapEdgeLimit        = WCIDDiameter/2.0 - std::max(WCPMTRadius,mPMT_vessel_radius);
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

// B.Quilain, HK with hybrid configuration: mPMTs + B&L
// These are defaults that can be altered through the macros
void WCSimDetectorConstruction::SetHyperK_HybridmPMT10PCGeometry()
{
  //Box&Line
  WCDetectorName = "HyperK_HybridmPMT10PC";
  HKGeometryNoLongerUsed(WCDetectorName);
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
  dist_pmt_vessel = 8*CLHEP::mm;
  
  orientation = PERPENDICULAR;
  mPMT_outer_material = "G4_PLEXIGLASS";
  mPMT_inner_material = "Air";               // TODO: real air, hence update abs_length
  mPMT_material_pmtAssembly = "SilGel";
  mPMT_outer_material_d = 10*CLHEP::mm;
  // Radius of cone at z=reflectorHeight
  id_reflector_height = 0;//9.29*CLHEP::mm;        // for a radius of 7.25mm, for hex: 5.4mm (radius of 6mm)
  id_reflector_z_offset = 0;//6.12*CLHEP::mm;       //from KM3Net CAD drawings
  id_reflector_angle = 0;//46.5*CLHEP::deg;         // Need to be remeasured for different PMT curvature 
  mPMT_pmt_openingAngle = 8.3*CLHEP::deg;     // for hex: 8.5deg
  // parameters related to filling the ID mPMT
  config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_19_nuPrism_3ring.txt"; // for smaller reflector, use: mPMTconfig_19_nuPrism.txt (hex)

  G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;	
 

  WCIDDiameter          = 64.8*m;//70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
  WCIDHeight            = 65.751*m;//54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
  WCBarrelPMTOffset     = std::max(WCPMTRadius,mPMT_vessel_tot_height); //offset from vertical
  WCIDVerticalPosition     = 0.;


  if(WCPMTPercentCoverage < -0.1) WCPMTPercentCoverage  = 20.0;
  else if(WCPMTPercentCoverage < 0) WCPMTPercentCoverage = 0.0; //protection against negative coverage
  if(WCPMTPercentCoverage2 < -0.1) WCPMTPercentCoverage2  = 10.0;
  else if(WCPMTPercentCoverage2 < 0) WCPMTPercentCoverage2 = 0.0; //protection against negative coverage
  
  //WCPMTPercentCoverage  = 20.0;//40.0;//40.0;  
  //WCPMTPercentCoverage2  = 10.0;//20.0;
  WCPMTperCellHorizontal = std::lround((WCPMTPercentCoverage+WCPMTPercentCoverage2) / (WCPMTPercentCoverage2==0?(WCPMTPercentCoverage+WCPMTPercentCoverage2):WCPMTPercentCoverage2)); 
  WCPMTperCellVertical = WCPMTperCellHorizontal;
  //WCPMTperCellHorizontal= ((int) WCPMTPercentCoverage2/(WCPMTPercentCoverage+WCPMTPercentCoverage2));//Should be in agreement with coverage of PMT 1 & 2: here, 4/5 PMTs are B&L + 1/5 mPMT
  //WCPMTperCellVertical  = ((int) WCPMTPercentCoverage2/(WCPMTPercentCoverage+WCPMTPercentCoverage2));//Should be in agreement with coverage of PMT 1 & 2: here, 4/5 PMTs are B&L + 1/5 mPMT

  //WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*(WCPMTPercentCoverage+WCPMTPercentCoverage2))/(10.0*std::max(WCPMTRadius,mPMT_vessel_tot_height)));
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*(WCPMTPercentCoverage+WCPMTPercentCoverage2))/(10.0*std::max(WCPMTRadius,mPMT_vessel_radius)));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  //G4cout << "## Number of Horizontal PMT along diameter = " << WCBarrelNumPMTHorizontal << ", diameter = "<< WCIDDiameter << ", coverage = "<< WCPMTPercentCoverage << ", PMT radius = " << WCPMTRadius << ", test = " << WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius) << G4endl;
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  //WCCapEdgeLimit        = WCIDDiameter/2.0 - 5*std::max(WCPMTExposeHeight,mPMT_vessel_tot_height);//3.0*cm;//WCIDDiameter/2.0 - std::max(WCPMTRadius,mPMT_vessel_tot_height);//WCPMTRadius;
  WCCapEdgeLimit        = WCIDDiameter/2.0 - std::max(WCPMTRadius,mPMT_vessel_radius);
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

// B.Quilain, HK with hybrid configuration: mPMTs + B&L
// These are defaults that can be altered through the macros
void WCSimDetectorConstruction::SetHyperK_HybridFakeGeometry()
{
  WCDetectorName = "HyperK_HybridFake";
  HKGeometryNoLongerUsed(WCDetectorName);
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
  dist_pmt_vessel = 8*CLHEP::mm;      
  orientation = PERPENDICULAR;
  mPMT_outer_material = "G4_PLEXIGLASS";
  mPMT_inner_material = "Air";               // TODO: real air, hence update abs_length
  mPMT_material_pmtAssembly = "SilGel";
  mPMT_outer_material_d = 10*CLHEP::mm;
  
  // Radius of cone at z=reflectorHeight
  id_reflector_height = 9.29*CLHEP::mm;        // for a radius of 7.25mm, for hex: 5.4mm (radius of 6mm)
  id_reflector_z_offset = 6.12*CLHEP::mm;       //from KM3Net CAD drawings
  id_reflector_angle = 46.5*CLHEP::deg;         // Need to be remeasured for different PMT curvature 
  mPMT_pmt_openingAngle = 8.3*CLHEP::deg;     // for hex: 8.5deg
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
 

  WCIDDiameter          = 64.8*m;//70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
  WCIDHeight            = 65.751*m;//54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
  WCBarrelPMTOffset     = std::max(WCPMTRadius,mPMT_vessel_tot_height); //offset from vertical
  WCIDVerticalPosition     = 0.;

  if(WCPMTPercentCoverage < -0.1) WCPMTPercentCoverage  = 20.0;
  else if(WCPMTPercentCoverage < 0) WCPMTPercentCoverage = 0.0; //protection against negative coverage
  if(WCPMTPercentCoverage2 < -0.1) WCPMTPercentCoverage2  = 0.0;
  else if(WCPMTPercentCoverage2 < 0) WCPMTPercentCoverage2 = 0.0; //protection against negative coverage
  //WCPMTPercentCoverage  = 20.0;//20.0;//40.0;//40.0;  
  //WCPMTPercentCoverage2  = 0;//5.0;//20.0;
  WCPMTperCellHorizontal = std::lround((WCPMTPercentCoverage+WCPMTPercentCoverage2) / (WCPMTPercentCoverage2==0?(WCPMTPercentCoverage+WCPMTPercentCoverage2):WCPMTPercentCoverage2)); 
  WCPMTperCellVertical = WCPMTperCellHorizontal;
  //WCPMTperCellHorizontal= ((int) WCPMTPercentCoverage2/(WCPMTPercentCoverage+WCPMTPercentCoverage2));//Should be in agreement with coverage of PMT 1 & 2: here, 4/5 PMTs are B&L + 1/5 mPMT
  //WCPMTperCellVertical  = ((int) WCPMTPercentCoverage2/(WCPMTPercentCoverage+WCPMTPercentCoverage2));//Should be in agreement with coverage of PMT 1 & 2: here, 4/5 PMTs are B&L + 1/5 mPMT

  //WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*(WCPMTPercentCoverage+WCPMTPercentCoverage2))/(10.0*std::max(WCPMTRadius,mPMT_vessel_tot_height)));
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*(WCPMTPercentCoverage+WCPMTPercentCoverage2))/(10.0*std::max(WCPMTRadius,mPMT_vessel_radius)));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  //G4cout << "## Number of Horizontal PMT along diameter = " << WCBarrelNumPMTHorizontal << ", diameter = "<< WCIDDiameter << ", coverage = "<< WCPMTPercentCoverage << ", PMT radius = " << WCPMTRadius << ", test = " << WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius) << G4endl;
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  //WCCapEdgeLimit        = WCIDDiameter/2.0 - 5*std::max(WCPMTExposeHeight,mPMT_vessel_tot_height);//3.0*cm;//WCIDDiameter/2.0 - std::max(WCPMTRadius,mPMT_vessel_tot_height);//WCPMTRadius;
  WCCapEdgeLimit        = WCIDDiameter/2.0 - std::max(WCPMTRadius,mPMT_vessel_radius);
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

// HK complete FD: mPMTs & B&L ID + OD w/ WLS plates
// These are defaults that can be altered through the macros
void WCSimDetectorConstruction::SetHyperK_HybridmPMT_WithOD_Geometry()
{
  //Box&Line
  WCDetectorName = "HyperK_HybridmPMT_WithOD";
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
  dist_pmt_vessel = 8*CLHEP::mm;
  
  orientation = PERPENDICULAR;
  mPMT_outer_material = "G4_PLEXIGLASS";
  mPMT_inner_material = "Air";               // TODO: real air, hence update abs_length
  mPMT_material_pmtAssembly = "SilGel";
  mPMT_outer_material_d = 10*CLHEP::mm;
  // Radius of cone at z=reflectorHeight
  id_reflector_height = 0;//9.29*CLHEP::mm;        // for a radius of 7.25mm, for hex: 5.4mm (radius of 6mm)
  id_reflector_z_offset = 0;//6.12*CLHEP::mm;       //from KM3Net CAD drawings
  id_reflector_angle = 0;//46.5*CLHEP::deg;         // Need to be remeasured for different PMT curvature 
  mPMT_pmt_openingAngle = 8.3*CLHEP::deg;     // for hex: 8.5deg
  // parameters related to filling the ID mPMT
  config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_19_nuPrism_3ring.txt"; // for smaller reflector, use: mPMTconfig_19_nuPrism.txt (hex)

  G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;	
 

  WCIDDiameter          = 64.8*m;//70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
  WCIDHeight            = 65.751*m;//54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
  WCBarrelPMTOffset     = std::max(WCPMTRadius,mPMT_vessel_tot_height) + 1.*CLHEP::mm; //offset from vertical
  WCIDVerticalPosition     = 0.;

  if(WCPMTPercentCoverage < -0.1) WCPMTPercentCoverage  = 20.2150576375662;
  else if(WCPMTPercentCoverage < 0) WCPMTPercentCoverage = 0.0; //protection against negative coverage
  if(WCPMTPercentCoverage2 < -0.1) WCPMTPercentCoverage2  = 10.1075288187831;
  else if(WCPMTPercentCoverage2 < 0) WCPMTPercentCoverage2 = 0.0; //protection against negative coverage
  WCPMTperCellHorizontal = std::lround((WCPMTPercentCoverage+WCPMTPercentCoverage2) / (WCPMTPercentCoverage2==0?(WCPMTPercentCoverage+WCPMTPercentCoverage2):WCPMTPercentCoverage2)); 
  WCPMTperCellVertical = WCPMTperCellHorizontal;
  //WCPMTperCellHorizontal= ((int) WCPMTPercentCoverage2/(WCPMTPercentCoverage+WCPMTPercentCoverage2));//Should be in agreement with coverage of PMT 1 & 2: here, 4/5 PMTs are B&L + 1/5 mPMT
  //WCPMTperCellVertical  = ((int) WCPMTPercentCoverage2/(WCPMTPercentCoverage+WCPMTPercentCoverage2));//Should be in agreement with coverage of PMT 1 & 2: here, 4/5 PMTs are B&L + 1/5 mPMT

  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*(WCPMTPercentCoverage+WCPMTPercentCoverage2))/(10.0*std::max(WCPMTRadius,mPMT_vessel_radius)));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
									/WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  //WCCapEdgeLimit        = WCIDDiameter/2.0 - 5*std::max(WCPMTExposeHeight,mPMT_vessel_tot_height);//3.0*cm;//WCIDDiameter/2.0 - std::max(WCPMTRadius,mPMT_vessel_tot_height);//WCPMTRadius;
  WCCapEdgeLimit        = WCIDDiameter/2.0 - std::max(WCPMTRadius,mPMT_vessel_radius);
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;

  //////////////////////////////////////
  // Cave Parameters --- Beta version //
  //////////////////////////////////////
  CaveTyvekSheetThickness  = 1.*mm; // Quite Standard I guess

  ////////////////////////////////////
  // OD Parameters --- Beta version //
  ////////////////////////////////////
  isODConstructed = true;

  // OD Dimensions //
  WCODLateralWaterDepth    = 1.*m;
  WCODHeightWaterDepth     = 2.*m;
  WCODDeadSpace            = 600.*mm;
  WCODTyvekSheetThickness  = 1.*mm; // Quite standard I guess
  WCODWLSPlatesThickness   = 1.*cm; //
  WCODWLSPlatesLength      = 60.*cm; //
  WCODDiameter             = WCIDDiameter + 2*(WCBlackSheetThickness+WCODDeadSpace+WCODTyvekSheetThickness+WCODWLSPlatesThickness);

  // OD PMTs //
  WCODCollectionName = WCDetectorName + "-glassFaceWCPMT_OD";
  WCSimPMTObject *PMTOD = CreatePMTObject("PMT8inch", WCODCollectionName);
  WCPMTODName           = PMTOD->GetPMTName();
  WCPMTODExposeHeight   = PMTOD->GetExposeHeight();
  WCPMTODRadius         = PMTOD->GetRadius();

  // OD Coverage on barrel side //
  WCPMTODperCellHorizontal = 1;
  WCPMTODperCellVertical   = 1;

  // OD Coverage on caps //
  WCPMTODPercentCoverage   = 1.0; //default 1%
  // NOTE : If you set WCPMTODperCellHorizontal=0 and WCPMTODperCellVertical=0,
  // then method ComputeWCODPMT() inside ConstructCylinder will automatically compute
  // the nb of PMTs to put on barrel side according to WCPMTODPercentCoverage

  // Shift between PMTs inside a cell //
  WCODPMTShift = 0.*cm;

  // OD caps //
  // WCODCapPMTSpacing = 100*cm;
  WCODCapPMTSpacing  = (pi*WCIDDiameter/(round(WCIDDiameter*sqrt(pi*WCPMTODPercentCoverage)/(10.0*WCPMTODRadius))));
  WCODCapEdgeLimit = WCIDDiameter/2.0 - WCODWLSPlatesLength/2;

  // TEST combined PMT collection for stacking action
  std::vector<G4String> WCColName;
  WCColName.push_back(WCIDCollectionName);
  WCColName.push_back(WCODCollectionName);
  WCColName.push_back(WCIDCollectionName2);
  CreateCombinedPMTQE(WCColName);
  isCombinedPMTCollectionDefined=true;

  // TEST WLS collection for stacking action
  G4String WLSType = "EljenEJ286";
  isWLSFilled = true;
  BuildODWLSCladding = true;
  CreateWLSObject(WLSType);
}

void WCSimDetectorConstruction::SetEggShapedHyperKGeometry()
{
  WCDetectorName = "EggShapedHyperK";
  HKGeometryNoLongerUsed(WCDetectorName);
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
  hybrid = false;

  MatchWCSimAndEggShapedHyperK();
  InitSinglePMT();
}

void WCSimDetectorConstruction::SetEggShapedHyperKGeometry_withHPD()
{
  WCDetectorName = "EggShapedHyperK_withHPD";
  HKGeometryNoLongerUsed(WCDetectorName);
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
  hybrid = false;

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


void WCSimDetectorConstruction::SetNuPrismGeometry(G4String PMTType, G4double /*PMTCoverage*/, G4double detectorHeight, G4double detectorDiameter, G4double verticalPosition)
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
    WCPMTperCellHorizontal = 1;
    WCPMTperCellVertical   = 1;
	if(WCPMTPercentCoverage < -0.1) WCPMTPercentCoverage  = 40.0;
	else if(WCPMTPercentCoverage < 0) WCPMTPercentCoverage = 0.0; //protection against negative coverage
	//WCPMTPercentCoverage   = PMTCoverage;
    WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage/100.0)/WCPMTRadius);

    WCBarrelNRings        = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/WCPMTperCellVertical);
     WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal;
    WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
    WCBlackSheetThickness = 2.0*cm;
    WCAddGd               = false;
	hybrid = false;

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
	dist_pmt_vessel = 8*CLHEP::mm;      
	orientation = PERPENDICULAR;
	mPMT_outer_material = "G4_PLEXIGLASS";
	mPMT_inner_material = "Air";               // TODO: real air, hence update abs_length
	mPMT_material_pmtAssembly = "SilGel";
	mPMT_outer_material_d = 10*CLHEP::mm;
	
	// Radius of cone at z=reflectorHeight
	id_reflector_height = 9.29*CLHEP::mm;        // From solidworks CAD drawings
	id_reflector_z_offset = 6.12*CLHEP::mm;
	id_reflector_angle = 46.5*CLHEP::deg;
	mPMT_pmt_openingAngle = 8.3*CLHEP::deg;
	G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;
	
	// parameters related to filling the ID mPMT
	nID_PMTs = 19;
	config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_19_nuPrism_3ring.txt"; // for smaller reflector, use: mPMTconfig_19_nuPrism.txt (hex)

	WCIDHeight               = 10.42*CLHEP::m;
    WCIDDiameter             = 7.42*CLHEP::m;
    WCIDVerticalPosition     = 0.;
	
	WCBarrelPMTOffset     = mPMT_vessel_tot_height;
    WCPMTperCellHorizontal = 2; // 2 per phi
    WCPMTperCellVertical   = 1;

	// Numbers below are based on R.Henderson's 832 module tank design
    WCBarrelNumPMTHorizontal = 40;
    WCBarrelNRings        = 16;
    WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal;
	// This is tuned to give a 6-5-5-4-3-1 config in one quarter of the cap
	// And results in 832 mPMT modules as currently in Robert's Design
    WCCapEdgeLimit        = 3.3*m; 
    WCBlackSheetThickness = 2.0*cm;    // deprecate soon.
    WCAddGd               = false;
	hybrid = false;
}

// WCTE with mPMTs (M.Shinoki)
void WCSimDetectorConstruction::SetNuPrismBeamTest_mPMTGeometry()
{
  WCDetectorName = "NuPRISMBeamTest_mPMT";
  G4cerr << "**********************************" << G4endl
		 << WCDetectorName << " is not supported in this version of WCSim" << G4endl
		 << "Please use code from the WCTE/WCSim repository to simulate WCTE" << G4endl
		 << "Exiting..." << G4endl;
  exit(-1);
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
	dist_pmt_vessel = 8*CLHEP::mm;      
	orientation = PERPENDICULAR;
	mPMT_outer_material = "G4_PLEXIGLASS";
	mPMT_inner_material = "Air";               // TODO: real air, hence update abs_length
	mPMT_material_pmtAssembly = "SilGel";
	mPMT_outer_material_d = 10*CLHEP::mm;
	
	// Radius of cone at z=reflectorHeight
	id_reflector_height = 9.29*CLHEP::mm;        // From solidworks CAD drawings
	id_reflector_z_offset = 6.12*CLHEP::mm;
	id_reflector_angle = 46.5*CLHEP::deg; 
	mPMT_pmt_openingAngle = 8.3*CLHEP::deg;
	//G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;
	
	// parameters related to filling the ID mPMT
	nID_PMTs = 19;
	config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_19_nuPrism_3ring.txt"; // for smaller reflector, use: mPMTconfig_19_nuPrism.txt (hex)

	WCIDHeight               = 3.38*CLHEP::m;
    	WCIDDiameter             = 3.696*CLHEP::m;
    	WCIDVerticalPosition     = 0.;
	
	WCBarrelPMTOffset     = 300.*CLHEP::mm;
    WCPMTperCellHorizontal = 1; // 1 per phi
    WCPMTperCellVertical   = 1;

    WCBarrelNumPMTHorizontal = 18;
    WCBarrelNRings        = 5;
    //WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
    WCCapPMTSpacing       = 0.58*CLHEP::m;
    WCCapEdgeLimit        = 1.551*m; 
    WCBlackSheetThickness = 2.0*cm;    // deprecate soon.
    WCAddGd               = false;
	hybrid = false;
}

// WCTE with mPMTs (M.Shinoki)
void WCSimDetectorConstruction::SetNuPrismBeamTest_16cShort_mPMTGeometry()
{
    WCDetectorName = "NuPRISMBeamTest_16cShort_mPMT";
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
	dist_pmt_vessel = 8*CLHEP::mm;      
	orientation = PERPENDICULAR;
	mPMT_outer_material = "G4_PLEXIGLASS";
	mPMT_inner_material = "Air";               // TODO: real air, hence update abs_length
	mPMT_material_pmtAssembly = "SilGel";
	mPMT_outer_material_d = 10*CLHEP::mm;
	
	// Radius of cone at z=reflectorHeight
	id_reflector_height = 9.29*CLHEP::mm;        // From solidworks CAD drawings
	id_reflector_z_offset = 6.12*CLHEP::mm;
	id_reflector_angle = 46.5*CLHEP::deg; 
	mPMT_pmt_openingAngle = 8.3*CLHEP::deg;
	// G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;
	
	// parameters related to filling the ID mPMT
	nID_PMTs = 19;
	config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_19_nuPrism_3ring.txt"; // for smaller reflector, use: mPMTconfig_19_nuPrism.txt (hex)

	WCIDHeight               = 2.739*CLHEP::m;
	WCIDDiameter             = 3.441*CLHEP::m;
	WCIDVerticalPosition     = 0.;
	
	WCBarrelPMTOffset     = 300.*CLHEP::mm;
    WCPMTperCellHorizontal = 1.0; // 1 per phi
    WCPMTperCellVertical   = 1.0;

    WCBarrelNumPMTHorizontal = 16;
    WCBarrelNRings        = 4;
    //WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
    WCCapPMTSpacing       = 0.58*CLHEP::m;
    WCCapEdgeLimit        = 1.551*m; 
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
	mPMT_vessel_cyl_height = 38.*CLHEP::mm;    //option A, option B would be 277 mm
	mPMT_vessel_radius_curv = 342.*CLHEP::mm;  //needs to include the vessel thickness, as we construct from outside inwards.
	mPMT_vessel_radius = 254.*CLHEP::mm;
	dist_pmt_vessel = 8*CLHEP::mm;
	orientation = PERPENDICULAR;
	mPMT_outer_material = "G4_PLEXIGLASS";
	mPMT_inner_material = "Air";               // TODO: real air, hence update abs_length
	mPMT_material_pmtAssembly = "SilGel";
	mPMT_outer_material_d = 10*CLHEP::mm;   //me: dome thickness
	
	// Radius of cone at z=reflectorHeight
	id_reflector_height = 9.29*CLHEP::mm;        // From solidworks CAD drawings
	id_reflector_z_offset = 6.12*CLHEP::mm;
	id_reflector_angle = 46.5*CLHEP::deg;
	mPMT_pmt_openingAngle = 8.3*CLHEP::deg;
	G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;
	
	// parameters related to filling the ID mPMT
	nID_PMTs = 19;
	config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_19_nuPrism_3ring.txt"; // for smaller reflector, use: mPMTconfig_19_nuPrism.txt (hex)

	WCIDHeight               = 6.0*CLHEP::m;
	WCIDDiameter             = 8.0*CLHEP::m;
    WCIDVerticalPosition     = 0.;
	
	WCBarrelPMTOffset     = mPMT_vessel_tot_height;
    WCPMTperCellHorizontal = 1;
    WCPMTperCellVertical   = 1;

	// Numbers below are based on R.Henderson's 832 module tank design
    WCBarrelNumPMTHorizontal = 40;
    WCBarrelNRings        = 9; // Scaled from 10.42 m tall detector.
    WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal;
	// This is tuned to give a 6-5-5-4-3-1 config in one quarter of the cap
	// And results in 832 mPMT modules as currently in Robert's Design
    WCCapEdgeLimit        = 3.3*m; 
    WCBlackSheetThickness = 2.0*cm;    // deprecate soon.
    WCAddGd               = false;
	hybrid = false;
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
  mPMT_vessel_cyl_height = 38.*CLHEP::mm;
  mPMT_vessel_radius_curv = 300.*CLHEP::mm;
  mPMT_vessel_radius = 254.*CLHEP::mm;
  dist_pmt_vessel = 8*CLHEP::mm;
  orientation = PERPENDICULAR;
  mPMT_outer_material = "G4_PLEXIGLASS";
  mPMT_inner_material = "SilGel";
  mPMT_material_pmtAssembly = "SilGel";
  mPMT_outer_material_d = 10.*CLHEP::mm;
  // Radius of cone at z=reflectorHeight
  id_reflector_height = 10*CLHEP::mm;                //10. > previous 7mm (deprecated number from JINST)
  id_reflector_z_offset = 6.12*CLHEP::mm;            //from KM3Net CAD drawings
  id_reflector_angle = 48*CLHEP::pi/180.*CLHEP::rad; // Based on KM3Net reflector specs
  mPMT_pmt_openingAngle = 8.3*CLHEP::deg;     
  
  // BarrelPMTOffset/WCCapEdgeLimit needs PMT/mPMT height
  G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;
  // parameters related to filling the ID mPMT
  nID_PMTs = 33;
  config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_33_13_1.txt";

  WCIDDiameter             = 10.0*m;
  WCIDHeight               = 30.0*m;
  WCBarrelPMTOffset        = mPMT_vessel_tot_height;
  WCBarrelNumPMTHorizontal = 3;
  WCBarrelNRings           = 3;
  WCPMTperCellHorizontal   = 1;
  WCPMTperCellVertical     = 1;
  WCPMTPercentCoverage     = 10.;
  WCCapEdgeLimit           = WCIDDiameter/2.0 - mPMT_vessel_tot_height;
  WCCapPMTSpacing          = WCIDDiameter*2.0;
  WCBlackSheetThickness    = 2.0*cm;
  WCAddGd                  = false;
  hybrid = false;
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
  mPMT_vessel_cyl_height = 0.*CLHEP::mm; // the 50mm should exist only for OD and extends behind the iD wall
  mPMT_vessel_radius_curv = 270.*CLHEP::mm;
  mPMT_vessel_radius = 270.*CLHEP::mm;
  dist_pmt_vessel = 5*CLHEP::mm;
  orientation = PERPENDICULAR;
  mPMT_outer_material = "G4_PLEXIGLASS";
  mPMT_inner_material = "SilGel";
  mPMT_outer_material_d = 0.03*2* mPMT_vessel_radius *CLHEP::mm; //using NEMO article safety margin of 0.03 for t/D
  // Radius of cone at z=reflectorHeight
  id_reflector_height = 7.5*CLHEP::mm;         //7.5mm from KM3Net JINST
  id_reflector_z_offset = 1.1*CLHEP::mm;         //from KM3Net CAD drawings
  id_reflector_angle = CLHEP::pi/4*CLHEP::rad; // Based on KM3Net JINST: 45 deg wrt normal, so 7.5mm xtra
  G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;

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
  WCBarrelPMTOffset     = mPMT_vessel_tot_height; //mPMT cylinder Radius //WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 13.51;
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*0.254*m));   //Used same radius as 20in to have same granularity in mPMTs (28% difference otherwise
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - 0.254*m;//outer_module_radius;//WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;
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
  mPMT_vessel_cyl_height = 0.*CLHEP::mm; // the 50mm should exist only for OD and extends behind the iD wall
  mPMT_vessel_radius_curv = 280.*CLHEP::mm;   
  mPMT_vessel_radius = 280.*CLHEP::mm;   
  dist_pmt_vessel = 5*CLHEP::mm;
  orientation = PERPENDICULAR;
  mPMT_outer_material = "G4_PLEXIGLASS";
  mPMT_inner_material = "SilGel";
  mPMT_outer_material_d = 0.03*2*mPMT_vessel_radius*CLHEP::mm; //using NEMO article safety margin of 0.03 for t/D
  // Radius of cone at z=reflectorHeight
  id_reflector_height = 7.5*CLHEP::mm;         //7.5mm from KM3Net JINST
  id_reflector_z_offset = 1.1*CLHEP::mm;         //from KM3Net CAD drawings
  id_reflector_angle = CLHEP::pi/4*CLHEP::rad; // Based on KM3Net JINST: 45 deg wrt normal, so 7.5mm xtra
  G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;

  // parameters related to filling the ID mPMT
  nID_PMTs = 34;
  config_file = wcsimdir_path+"/mPMT-configfiles/mPMTconfig_34_22.5_2.txt"; 


  WCIDDiameter          = 74.0*m;
  WCIDHeight            = 60.0*m;
  WCBarrelPMTOffset     = mPMT_vessel_tot_height; //mPMT cylinder Radius //WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 40.0;
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*0.254*m));//WCPMTRadius));
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - 0.254*m;//WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;
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
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));  
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;

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
  WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));  
  WCBarrelNRings           = std::lround((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical);
  WCCapPMTSpacing       = pi*WCIDDiameter/WCBarrelNumPMTHorizontal; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  hybrid = false;

  InitSinglePMT();
}



void WCSimDetectorConstruction::InitSinglePMT(){
  
  mPMT_vessel_cyl_height = 0.*mm;
  mPMT_vessel_radius_curv = 0.1*mm;
  mPMT_vessel_radius = 0.1*mm;
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

void WCSimDetectorConstruction::DumpDetectorConfigInfo()
{
#ifdef VERBOSE_GEOMETRY_CONFIG
  G4cout
	<< "GEOMCHECK WCDetectorName \t" << WCDetectorName << G4endl
	<< "GEOMCHECK WCIDCollectionName \t" << WCIDCollectionName << G4endl
	<< "GEOMCHECK WCPMTName \t" << WCPMTName << G4endl
	<< "GEOMCHECK WCPMTExposeHeight \t" << WCPMTExposeHeight << G4endl
	<< "GEOMCHECK WCPMTRadius \t" << WCPMTRadius << G4endl
	<< "GEOMCHECK WCIDDiameter \t" << WCIDDiameter << G4endl
	<< "GEOMCHECK WCIDHeight \t" << WCIDHeight << G4endl
	<< "GEOMCHECK WCBarrelPMTOffset \t" << WCBarrelPMTOffset << G4endl
	<< "GEOMCHECK WCPMTperCellHorizontal \t" << WCPMTperCellHorizontal << G4endl
	<< "GEOMCHECK WCPMTperCellVertical \t" << WCPMTperCellVertical << G4endl
	<< "GEOMCHECK WCPMTPercentCoverage \t" << WCPMTPercentCoverage << G4endl
	<< "GEOMCHECK WCBarrelNumPMTHorizontal \t" << WCBarrelNumPMTHorizontal << G4endl
	<< "GEOMCHECK WCBarrelNRings \t" << WCBarrelNRings << G4endl
	<< "GEOMCHECK WCCapPMTSpacing \t" << WCCapPMTSpacing << G4endl
	<< "GEOMCHECK WCCapEdgeLimit \t" << WCCapEdgeLimit << G4endl
	<< "GEOMCHECK WCBlackSheetThickness \t" << WCBlackSheetThickness << G4endl
	<< "GEOMCHECK WCAddGd \t" << WCAddGd << G4endl
	<< "GEOMCHECK CaveTyvekSheetThickness \t" << CaveTyvekSheetThickness << G4endl
	<< "GEOMCHECK isODConstructed \t" << isODConstructed << G4endl
	<< "GEOMCHECK WCODLateralWaterDepth \t" << WCODLateralWaterDepth << G4endl
	<< "GEOMCHECK WCODHeightWaterDepth \t" << WCODHeightWaterDepth << G4endl
	<< "GEOMCHECK WCODDeadSpace \t" << WCODDeadSpace << G4endl
	<< "GEOMCHECK WCODTyvekSheetThickness \t" << WCODTyvekSheetThickness << G4endl
	<< "GEOMCHECK WCODWLSPlatesThickness \t" << WCODWLSPlatesThickness << G4endl
	<< "GEOMCHECK WCODWLSPlatesLength \t" << WCODWLSPlatesLength << G4endl
	<< "GEOMCHECK WCODDiameter \t" << WCODDiameter << G4endl
	<< "GEOMCHECK WCODCollectionName \t" << WCODCollectionName << G4endl
	<< "GEOMCHECK WCPMTODName \t" << WCPMTODName << G4endl
	<< "GEOMCHECK WCPMTODExposeHeight \t" << WCPMTODExposeHeight << G4endl
	<< "GEOMCHECK WCPMTODRadius \t" << WCPMTODRadius << G4endl
	<< "GEOMCHECK WCPMTODperCellHorizontal \t" << WCPMTODperCellHorizontal << G4endl
	<< "GEOMCHECK WCPMTODperCellVertical \t" << WCPMTODperCellVertical << G4endl
	<< "GEOMCHECK WCPMTODPercentCoverage \t" << WCPMTODPercentCoverage << G4endl
	<< "GEOMCHECK WCODPMTShift \t" << WCODPMTShift << G4endl
	<< "GEOMCHECK WCODCapPMTSpacing \t" << WCODCapPMTSpacing << G4endl
	<< "GEOMCHECK WCODCapEdgeLimit \t" << WCODCapEdgeLimit << G4endl
	;
#endif
}
