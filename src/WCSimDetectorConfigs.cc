//  -*- mode:c++; tab-width:4;  -*-
#include "WCSimDetectorConstruction.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"


void WCSimDetectorConstruction::SetSuperKGeometry()
{
  WCPMTName             ="20inch";
  WCPMTRadius           =.254*m;  
  WCPMTExposeHeight     =.18*m; 
  WCIDDiameter          = 33.6815*m; //16.900*2*cos(2*pi*rad/75)*m; //inner detector diameter
  WCIDHeight            = 36.200*m; //"" "" height
  WCBarrelPMTOffset     = 0.0715*m; //offset from vertical
  WCBarrelNumPMTHorizontal  = 150; 
  WCBarrelNRings        = 17.;
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3; 
  WCCapPMTSpacing       = 0.707*m; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = 16.9*m;
  WCPMTGlassThickness   = .4*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}


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

  // PMT Name specifies behavior
  // Currently the same for inner and outer
  WCPMTName             ="20inch";
  WCPMTGlassThickness   = .55*cm;

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
  WCPMTName             = "10inch";
  WCPMTRadius           = .127*m;
  WCPMTExposeHeight	    = WCPMTRadius - 0.01*m;
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
  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_100kton_10inch_HQE_12perCent()
{
  WCPMTName             = "10inchHQE";
  WCPMTRadius           = .127*m;
  WCPMTExposeHeight	    = WCPMTRadius - 0.01*m;
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
  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_100kton_10inch_HQE_30perCent()
{
  WCPMTName             = "10inchHQE";
  WCPMTRadius           = .127*m;
  WCPMTExposeHeight	    = WCPMTRadius - 0.01*m;
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
  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_100kton_10inch_HQE_30perCent_Gd()
{
  WCPMTName             = "10inchHQE";
  WCPMTRadius           = .127*m;
  WCPMTExposeHeight	    = WCPMTRadius - 0.01*m;
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
  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = true;
}

void WCSimDetectorConstruction::DUSEL_150kton_10inch_HQE_30perCent()
{
  WCPMTName             = "10inchHQE";
  WCPMTRadius           = .127*m;
  WCPMTExposeHeight	    = WCPMTRadius - 0.01*m;
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
  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_200kton_10inch_HQE_12perCent()
{
	WCPMTName             = "10inchHQE";
	WCPMTRadius           = .127*m;
	WCPMTExposeHeight	    = WCPMTRadius - 0.01*m;
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
	WCPMTGlassThickness   = .55*cm;
	WCBlackSheetThickness = 2.0*cm;
	WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_200kton_12inch_HQE_10perCent()
{
	WCPMTName             = "10inchHQE"; //still using QE from 10 inch tube
	WCPMTRadius           = .1524*m;
	WCPMTExposeHeight	    = .118*m;
	WCIDDiameter          = 63.30*m;
	WCIDHeight            = 76.60*m;
	WCBarrelPMTOffset	    = .1537*m;
	WCPMTperCellHorizontal = 1.0;
	WCPMTperCellVertical	 = 1.0;
	WCBarrelNumPMTHorizontal = 232;
	WCBarrelNRings        = 89;
	WCCapPMTSpacing       = .8572*m;
	WCCapEdgeLimit        = 31.424*m;
	WCPMTGlassThickness   = .55*cm; //guess
	WCBlackSheetThickness = 2.0*cm; //excess, should be just as light-tight
	WCAddGd               = false;
}

void WCSimDetectorConstruction::DUSEL_200kton_12inch_HQE_14perCent()
{
	WCPMTName             = "10inchHQE"; //still using QE from 10 inch tube
	WCPMTRadius           = .1524*m;
	WCPMTExposeHeight	    = .118*m;
	WCIDDiameter          = 63.30*m;
	WCIDHeight            = 76.60*m;
	WCBarrelPMTOffset	    = .1951*m;
	WCPMTperCellHorizontal = 1.0;
	WCPMTperCellVertical	 = 1.0;
	WCBarrelNumPMTHorizontal = 274;
	WCBarrelNRings        = 105;
	WCCapPMTSpacing       = .7258*m;
	WCCapEdgeLimit        = 31.624*m;
	WCPMTGlassThickness   = .55*cm; //guess
	WCBlackSheetThickness = 2.0*cm; //excess, should be just as light-tight
	WCAddGd               = false;
}





