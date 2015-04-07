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
  WCSimPMTObject * PMT = CreatePMTObject("PMT20inch", "glassFaceWCPMT");
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight();
  WCPMTRadius = PMT->GetRadius();
  WCPMTGlassThickness = PMT->GetPMTGlassThickness();
  WCIDDiameter          = 33.6815*m; //16.900*2*cos(2*pi*rad/75)*m; //inner detector diameter
  WCIDHeight            = 36.200*m; //"" "" height
  WCBarrelPMTOffset     = 0.0715*m; //offset from vertical
  WCBarrelNumPMTHorizontal  = 150; 
  WCBarrelNRings        = 17.;
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3; 
  WCCapPMTSpacing       = 0.707*m; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = 16.9*m;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}


// Note: the actual coverage is 20.27%
void WCSimDetectorConstruction::SuperK_20inchPMT_20perCent()
{
  WCSimPMTObject * PMT = CreatePMTObject("PMT20inch", "glassFaceWCPMT");
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCPMTGlassThickness = PMT->GetPMTGlassThickness();
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
}


// Note: the actual coverage is 20.27%
void WCSimDetectorConstruction::SuperK_20inchBandL_20perCent()
{
	WCSimPMTObject * PMT = CreatePMTObject("BoxandLine20inchHQE", "glassFaceWCPMT");
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCPMTGlassThickness = PMT->GetPMTGlassThickness();
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
}


// Note: the actual coverage is 14.59%
void WCSimDetectorConstruction::SuperK_12inchBandL_15perCent()
{
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine12inchHQE", "glassFaceWCPMT");
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCPMTGlassThickness = PMT->GetPMTGlassThickness();
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
}


// Note: the actual coverage is 13.51%
void WCSimDetectorConstruction::SuperK_20inchBandL_14perCent()
{
	WCSimPMTObject * PMT = CreatePMTObject("BoxandLine20inchHQE", "glassFaceWCPMT");
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCPMTGlassThickness = PMT->GetPMTGlassThickness();
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
}


void WCSimDetectorConstruction::Cylinder_12inchHPD_15perCent()
{
  // cylindrical detector with a height of 100m and a diameter of 69m 
  // with 12" HPD and 14.59% photocoverage
  WCSimPMTObject * PMT = CreatePMTObject("HPD12inchHQE", "glassFaceWCPMT");
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCPMTGlassThickness = PMT->GetPMTGlassThickness();
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
}


void WCSimDetectorConstruction::SetHyperKGeometry()
{
  WCSimPMTObject * PMT = CreatePMTObject("PMT20inch", "glassFaceWCPMT");
  WCPMTName = PMT->GetPMTName();
  innerPMT_Expose = PMT->GetExposeHeight();
  innerPMT_Radius = PMT->GetRadius();
  WCPMTGlassThickness = PMT->GetPMTGlassThickness();
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

  //WCSimPMTObject* outerPMT = new PMT8inch; //currently doesn't store the pointer to this PMT. If we want to read the qpe or QE for the OD in the future, this will need to be stored.
  WCSimPMTObject * outerPMT = CreatePMTObject("PMT8inch", "glassFaceWCPMT_OD");
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

  isHyperK = true; // Tell DetectorConstruction to build HK geometry

  MatchWCSimAndHyperK();
}

void WCSimDetectorConstruction::SetHyperKGeometry_withHPD()
{
  WCSimPMTObject * PMT = CreatePMTObject("HPD20inchHQE", "glassFaceWCPMT");
   WCPMTName = PMT->GetPMTName();
   innerPMT_Expose = PMT->GetExposeHeight();
   innerPMT_Radius = PMT->GetRadius();
   WCPMTGlassThickness = PMT->GetPMTGlassThickness();
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

   //   WCSimPMTObject* outerPMT = new PMT8inch; //currently doesn't store the pointer to this PMT. If we want to read the qpe or QE for the OD in the future, this will need to be stored.
   WCSimPMTObject * outerPMT = CreatePMTObject("PMT8inch", "glassFaceWCPMT_OD");
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
   
   isHyperK = true; // Tell DetectorConstruction to build HK geometry
   
   MatchWCSimAndHyperK();
}


/**
 * Transfer HK variables needed elsewhere
 * to their generic WC equivalents.
 * This should be included in all HK configurations.
 */
void WCSimDetectorConstruction::MatchWCSimAndHyperK()
{
  WCLength = waterTank_Length;
  WCPosition = 0.;
  WCPMTRadius = innerPMT_Radius;
}






/********************************************************************
 *
 * Various LBNE WC Configurations
 *
 ********************************************************************/


void WCSimDetectorConstruction::DUSEL_100kton_10inch_40perCent()
{
  WCSimPMTObject * PMT = CreatePMTObject("PMT10inch", "glassFaceWCPMT");
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight();
  WCPMTRadius = PMT->GetRadius();
  WCPMTGlassThickness = PMT->GetPMTGlassThickness();
  WCIDDiameter          = 53.0*m;
  WCIDHeight            = 60.0*m;
  WCBarrelPMTOffset	    = WCPMTRadius;
  WCPMTperCellHorizontal = 4.0;
  WCPMTperCellVertical	 = 3.0;
  WCPMTPercentCoverage	 = 40.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/
                                    (10.0*WCPMTRadius));
  WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-
                                    2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/
                                    WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_100kton_10inch_HQE_12perCent()
{ 
  WCSimPMTObject * PMT = CreatePMTObject("PMT10inchHQE", "glassFaceWCPMT");
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight();
  WCPMTRadius = PMT->GetRadius();
  WCPMTGlassThickness = PMT->GetPMTGlassThickness();
  WCIDDiameter          = 53.0*m;
  WCIDHeight            = 60.0*m;
  WCBarrelPMTOffset	    = WCPMTRadius;
  WCPMTperCellHorizontal = 4.0;
  WCPMTperCellVertical	 = 3.0;
  WCPMTPercentCoverage	 = 12.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/
                                    (10.0*WCPMTRadius));
  WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-
                                    2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/
                                    WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_100kton_10inch_HQE_30perCent()
{
  WCSimPMTObject * PMT = CreatePMTObject("PMT10inchHQE", "glassFaceWCPMT");
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight();
  WCPMTRadius = PMT->GetRadius();
  WCPMTGlassThickness = PMT->GetPMTGlassThickness();
  WCIDDiameter          = 53.0*m;
  WCIDHeight            = 60.0*m;
  WCBarrelPMTOffset	    = WCPMTRadius;
  WCPMTperCellHorizontal = 4.0;
  WCPMTperCellVertical	 = 3.0;
  WCPMTPercentCoverage	 = 30.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/
                                    (10.0*WCPMTRadius));
  WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-
                                    2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/
                                    WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_100kton_10inch_HQE_30perCent_Gd()
{
  WCSimPMTObject * PMT = CreatePMTObject("PMT10inchHQE", "glassFaceWCPMT");
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight();
  WCPMTRadius = PMT->GetRadius();
  WCPMTGlassThickness = PMT->GetPMTGlassThickness();
  WCIDDiameter          = 53.0*m;
  WCIDHeight            = 60.0*m;
  WCBarrelPMTOffset	    = WCPMTRadius;
  WCPMTperCellHorizontal = 4.0;
  WCPMTperCellVertical	 = 3.0;
  WCPMTPercentCoverage	 = 30.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/
                                    (10.0*WCPMTRadius));
  WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-
                                    2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/
                                    WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = true;
}

void WCSimDetectorConstruction::DUSEL_150kton_10inch_HQE_30perCent()
{
  WCSimPMTObject * PMT = CreatePMTObject("PMT10inchHQE", "glassFaceWCPMT");
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight();
  WCPMTRadius = PMT->GetRadius();
  WCPMTGlassThickness = PMT->GetPMTGlassThickness();
  WCIDDiameter          = 64.0*m;
  WCIDHeight            = 60.0*m;
  WCBarrelPMTOffset	    = WCPMTRadius;
  WCPMTperCellHorizontal = 4.0;
  WCPMTperCellVertical	 = 3.0;
  WCPMTPercentCoverage	 = 30.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/
                                    (10.0*WCPMTRadius));
  WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-
                                    2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/
                                    WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_200kton_10inch_HQE_12perCent()
{
  WCSimPMTObject * PMT = CreatePMTObject("PMT10inchHQE", "glassFaceWCPMT");
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight();
  WCPMTRadius = PMT->GetRadius();
  WCPMTGlassThickness = PMT->GetPMTGlassThickness();
  WCIDDiameter          = 62.21*m;
  WCIDHeight            = 79.96*m;
  WCBarrelPMTOffset	    = WCPMTRadius;
  WCPMTperCellHorizontal = 4.0;
  WCPMTperCellVertical	 = 3.0;
  WCPMTPercentCoverage	 = 12.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/
								   (10.0*WCPMTRadius));
  WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-
															 2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))/
								 WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_200kton_12inch_HQE_10perCent()
{
  WCSimPMTObject * PMT = CreatePMTObject("PMT12inchHQE", "glassFaceWCPMT");
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight();
  WCPMTRadius = PMT->GetRadius();
  WCPMTGlassThickness = PMT->GetPMTGlassThickness();
	WCIDDiameter          = 63.30*m;
	WCIDHeight            = 76.60*m;
	WCBarrelPMTOffset	    = .1537*m;
	WCPMTperCellHorizontal = 1.0;
	WCPMTperCellVertical	 = 1.0;
	WCBarrelNumPMTHorizontal = 232;
	WCBarrelNRings        = 89;
	WCCapPMTSpacing       = .8572*m;
	WCCapEdgeLimit        = 31.424*m;
	WCBlackSheetThickness = 2.0*cm; //excess, should be just as light-tight
	WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_200kton_12inch_HQE_14perCent()
{
  WCSimPMTObject * PMT = CreatePMTObject("PMT12inchHQE", "glassFaceWCPMT");
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight();
  WCPMTRadius = PMT->GetRadius();
  WCPMTGlassThickness = PMT->GetPMTGlassThickness();
  WCIDDiameter          = 63.30*m;
  WCIDHeight            = 76.60*m;
  WCBarrelPMTOffset	    = .1951*m;
  WCPMTperCellHorizontal = 1.0;
  WCPMTperCellVertical	 = 1.0;
  WCBarrelNumPMTHorizontal = 274;
  WCBarrelNRings        = 105;
  WCCapPMTSpacing       = .7258*m;
  WCCapEdgeLimit        = 31.624*m;
  WCBlackSheetThickness = 2.0*cm; //excess, should be just as light-tight
  WCAddGd               = false;
}





