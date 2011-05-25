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


#include "G4SDManager.hh"
#include "WCSimWCSD.hh"
#include "WCSimTuningParameters.hh" //jl145

G4float WCSimDetectorConstruction::GetPMTQE(G4float PhotonWavelength, G4int flag, G4float low_wl, G4float high_wl, G4float ratio){
  // XQ  08/17/10
  // Decide to include the QE in the WCSim detector 
  // rathe than hard coded into the StackingAction
  // This way, one can load this QE in both the StackingAction
  // and the sensitive detector.

  // flag
  // flag == 0 return the maximum QE for all wave length
  // flag == 1 return the actual QE for the wave length


  // low_wl and high_wl 
  // remove any optical photons outside the range
  // basically return QE = 0
  
  // ratio, fudge factor to increase QE for certain purpose

  // return 0 for wavelenght outside the range
  if (flag==1){
    if (PhotonWavelength <= low_wl || PhotonWavelength >= high_wl || PhotonWavelength <=280 || PhotonWavelength >=660){
      return 0;
    }
  }else if (flag==0){
    if (PhotonWavelength <= low_wl || PhotonWavelength >= high_wl){
      return 0;
    }
  }
  
  //define the wavelength and QE for different detectors
  const G4float wavelength[20] = 
    { 280., 300., 320., 340., 360., 380., 400., 420., 440., 460.,  
      480., 500., 520., 540., 560., 580., 600., 620., 640., 660.};
  
  // (JF) This is the QE for the SuperK 20" tubes.
  const G4float SKQE[20] =
    { 0.00, .0139, .0854, .169, .203, .206, .211, .202, .188, .167, 
      .140,  .116, .0806, .0432, .0265, .0146, .00756, .00508, .00158, 0.00};
  const G4float SK_maxQE = 0.211;
  
  // (JF) Normal 10 inch quantum efficiency (Data from Hamamatsu/IceCube)
  // These numbers were read off a graph by hand. I will replace them if
  // I get better numbers. These should be close though.
  const G4float TenInchQE[20] =
    { 0.00, .0375, .13, .195, .22, .23, .24, .24, .225, .205,
      .18, .16, .14, .085, .065, .05, .035, .02, .005, 0.0};

  const G4float TenInch_maxQE = 0.24;
  
  // (JF) High QE tubes (Data from Hamamatsu/IceCube)
  // (JF) The data point was origionally copied wrong,
  // the QE at 420 nm should be 0.3168 instead of 0.3368
  // fixed on 11/15/2010
  const G4float TenInchHQE[20] =
    { 0.00, .0502, .2017, .2933, .3306, .3396, .3320, .3168, .2915, .2655, 
      .2268,  .1971, .1641, .1102, .0727, .0499, .0323, .0178, .0061, 0.00};
  
  const G4float TenInch_maxHQE = 0.3396;
  
  G4double wavelengthQE = 0;

  if (flag == 1){
    //MF: off by one bug fix.
    for (int i=0; i<=18; i++){
      if ( PhotonWavelength <= wavelength[i+1]){
	if (GetPMTName()=="10inchHQE"){
	  wavelengthQE = TenInchHQE[i] + 
	    (TenInchHQE[i+1]-TenInchHQE[i])/(wavelength[i+1]-wavelength[i])*
	    (PhotonWavelength - wavelength[i]);
	}else if(GetPMTName()=="10inch"){
	  wavelengthQE = TenInchQE[i] + 
	    (TenInchQE[i+1]-TenInchQE[i])/(wavelength[i+1]-wavelength[i])*
	    (PhotonWavelength - wavelength[i]);
	}else if(GetPMTName()=="20inch"){
	  wavelengthQE = SKQE[i] + 
	    (SKQE[i+1]-SKQE[i])/(wavelength[i+1]-wavelength[i])*
	    (PhotonWavelength - wavelength[i]);
	}else
	  {G4cerr << "PMTName is not defined" << G4endl;}
	break;
      }
    }
  }else if (flag == 0){
    if (GetPMTName()=="10inchHQE"){
      wavelengthQE = TenInch_maxHQE;
    }else if(GetPMTName()=="10inch"){
      wavelengthQE = TenInch_maxQE;
    }else if(GetPMTName()=="20inch"){
      wavelengthQE = SK_maxQE;
    }else
      {G4cerr << "PMTName is not defined" << G4endl;}
    
  }
  wavelengthQE = wavelengthQE *ratio;
  
  return wavelengthQE;
}


void WCSimDetectorConstruction::SetSuperKGeometry()
{
  WCPMTName             ="20inch";
  WCPMTRadius           =.254*m;  
  WCPMTExposeHeight     =.18*m; 
  WCPMTRadiusEff        =  WCPMTExposeHeight ;
  WCIDDiameter          = 33.7*m; //inner detector diameter
  WCIDHeight            = 36.1*m; //"" "" height
  WCBarrelPMTOffset     = 0.109*m; //offset from vertical
  WCBarrelNumPMTHorizontal  = 150; 
  WCBarrelNRings        = 17.;
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3; 
  WCCapPMTSpacing       = 0.707*m; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = 16.9*m;
  WCPMTGlassThickness   = .4*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  DaddLC=false; 
  DaddWLSP=false;
}

void WCSimDetectorConstruction::DUSEL_100kton_10inch_40perCent()
{
  WCPMTName             = "10inch";
  WCPMTRadiusEff        = 0.275*m;
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
							     2*(WCBarrelPMTOffset+WCPMTRadiusEff))/(pi*WCIDDiameter)))/
                                    WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  DaddLC=false; 
  DaddWLSP=false;
}

void WCSimDetectorConstruction::DUSEL_100kton_10inch_HQE_12perCent()
{
  WCPMTName             = "10inchHQE";
  WCPMTRadiusEff        = 0.275*m;
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
							     2*(WCBarrelPMTOffset+WCPMTRadiusEff))/(pi*WCIDDiameter)))/
                                    WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  DaddLC=true; 
  DaddWLSP=true;
}

void WCSimDetectorConstruction::DUSEL_100kton_10inch_HQE_30perCent()
{
  WCPMTName             = "10inchHQE";
  WCPMTRadiusEff        = 0.275*m;
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
							     2*(WCBarrelPMTOffset+WCPMTRadiusEff))/(pi*WCIDDiameter)))/
				 WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  DaddLC=false; 
  DaddWLSP=false;
}

void WCSimDetectorConstruction::DUSEL_100kton_10inch_HQE_30perCent_Gd()
{
  WCPMTName             = "10inchHQE";
  WCPMTRadiusEff        = 0.275*m;
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
							     2*(WCBarrelPMTOffset+WCPMTRadiusEff))/(pi*WCIDDiameter)))/
				 WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = true;
  DaddLC=false; 
  DaddWLSP=false;
}

void WCSimDetectorConstruction::DUSEL_150kton_10inch_HQE_30perCent()
{
  WCPMTName             = "10inchHQE";
  WCPMTRadiusEff        = 0.275*m;
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
							     2*(WCBarrelPMTOffset+WCPMTRadiusEff))/(pi*WCIDDiameter)))/
				 WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  DaddLC=false; 
  DaddWLSP=false;
}

void WCSimDetectorConstruction::DUSEL_200kton_10inch_HQE_12perCent()
{
  WCPMTName             = "10inchHQE";
  WCPMTRadiusEff        = 0.275*m;
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
							     2*(WCBarrelPMTOffset+WCPMTRadiusEff))/(pi*WCIDDiameter)))/
				 WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCPMTGlassThickness   = .55*cm;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
  DaddLC=true; 
  DaddWLSP=true;
}


//aah

void WCSimDetectorConstruction::SetMailBox100kTGeometry()  // This should setup a 100kT (metric) Fiducial Volume
{
	// PMT stuff
  WCPMTName               = "10inch";//normal QE 10 inch tube
  WCPMTRadius				= .127*m; 
  WCPMTExposeHeight		= WCPMTRadius - 0.01*m;
  WCPMTRadiusEff        =  WCPMTExposeHeight ;
  WCPMTGlassThickness		= .55*cm;
  WCPMTPercentCoverage	= 10;	//% coverage
  WCBlackSheetThickness	= 2.0*cm;
  //Tank and Cavern Dimensions
  WC_MB_Tank_Airgap		=2.0*m;	//Arbitrary here, but this is just the height of air between the top of the outer Veto_thickness and the beginning of the Cavern Dome
  WC_MB_Dome_Height_fraction		=0.2;	//fraction should be <=0.5 
  WC_MB_Veto_Thickness		=2.0*m;	// This is just the spacing (in water) between the outer side of the Blacksheet and the beginning of the fiducial volume
  WC_MB_Buffer_Thickness	=2.5*m; //Distance between wall and beginning of the Fiducial volume
  //Fiducial VOlume Stuff--these numbers are taken from table 9.1 of DUSEL LCAB Report #2, 7/2009, adjusted to Fiducial Volume (F.V.) and 2.5m gap between wall and F.V.
  WC_MB_Fid_Length          = (45.3333*m+2*WC_MB_Veto_Thickness);// strictly speaking this isn't the fiducial length anymore, but increased by the veto thickness
  WC_MB_Fid_Width           = (40.0*m+2*WC_MB_Veto_Thickness);
  WC_MB_Fid_Depth           = (55.0*m+2*WC_MB_Veto_Thickness);	// how deep is the bottom layer of tubes at. Tank depth is a little more.
  //	WC_MB_Fid_Length          = 1.0*m+2*WC_MB_Veto_Thickness;// tiny test detector!
  //	WC_MB_Fid_Width           = 1.000*m+2*WC_MB_Veto_Thickness;// tiny test detector!
  //	WC_MB_Fid_Depth           = 1.0*m+2*WC_MB_Veto_Thickness;// tiny test detector!
  WCAddGd               = false;
  DaddLC=false; 
  DaddWLSP=false;
}
void WCSimDetectorConstruction::SetMailBox150kTGeometry_10inch_HQE_30perCent()  // This should setup a 150kT (metric) Fiducial Volume
{
  // PMT stuff
  WCPMTName               = "10inchHQE";//high qe 10 " tube
  WCPMTRadius				= .127*m; 
  WCPMTExposeHeight		= WCPMTRadius - 0.01*m;
  WCPMTRadiusEff        =  WCPMTExposeHeight ;
  WCPMTGlassThickness		= .55*cm;
  WCPMTPercentCoverage	= 30;	//% coverage
  WCBlackSheetThickness	= 2.0*cm;
  //Tank and Cavern Dimensions
  WC_MB_Tank_Airgap		=2.0*m;	//Arbitrary here, but this is just the height of air between the top of the outer Veto_thickness and the beginning of the Cavern Dome
  WC_MB_Dome_Height_fraction	=0.2;//fraction should be <=0.5
  WC_MB_Veto_Thickness	=2.0*m;// This is just the spacing (in water) between the outer side of the Blacksheet and the beginning of the fiducial volume
  WC_MB_Buffer_Thickness	=2.5*m; //Distance between wall and beginning of the Fiducial volume
  //Fiducial VOlume Stuff--these numbers are taken from table 9.1 of DUSEL LCAB Report #2, 7/2009, adjusted to Fiducial Volume (F.V.) and 2.5m gap between wall and F.V.
  WC_MB_Fid_Length		= (68.0*m+2*WC_MB_Veto_Thickness);	// strictly speaking this isn't the fiducial length anymore, but increased by the veto thickness
  WC_MB_Fid_Width			= (40.0*m+2*WC_MB_Veto_Thickness);
  WC_MB_Fid_Depth         = (55.0*m+2*WC_MB_Veto_Thickness);	// how deep is the bottom layer of tubes at. Tank depth is a little more.
  WCAddGd               = false;
  DaddLC=false; 
  DaddWLSP=false;
}

void WCSimDetectorConstruction::SetMailBox150kTGeometry_10inch_40perCent()  // This should setup a 150kT (metric) Fiducial Volume
{
  // PMT stuff
  WCPMTName               = "10inch";//normal qe 10 inch tube
  WCPMTRadius				= .127*m; 
  WCPMTExposeHeight		= WCPMTRadius - 0.01*m;
  WCPMTRadiusEff        =  WCPMTExposeHeight ;
  WCPMTGlassThickness		= .55*cm;
  WCPMTPercentCoverage	= 40;	//% coverage
  WCBlackSheetThickness	= 2.0*cm;
  //Tank and Cavern Dimensions
  WC_MB_Tank_Airgap		=2.0*m;	//Arbitrary here, but this is just the height of air between the top of the outer Veto_thickness and the beginning of the Cavern Dome
  WC_MB_Dome_Height_fraction	=0.2;//fraction should be <=0.5
  WC_MB_Veto_Thickness	=2.0*m;// This is just the spacing (in water) between the outer side of the Blacksheet and the beginning of the fiducial volume
  WC_MB_Buffer_Thickness	=2.5*m; //Distance between wall and beginning of the Fiducial volume
  //Fiducial VOlume Stuff--these numbers are taken from table 9.1 of DUSEL LCAB Report #2, 7/2009, adjusted to Fiducial Volume (F.V.) and 2.5m gap between wall and F.V.
  WC_MB_Fid_Length		= (68.0*m+2*WC_MB_Veto_Thickness);	// strictly speaking this isn't the fiducial length anymore, but increased by the veto thickness
  WC_MB_Fid_Width			= (40.0*m+2*WC_MB_Veto_Thickness);
  WC_MB_Fid_Depth         = (55.0*m+2*WC_MB_Veto_Thickness);	// how deep is the bottom layer of tubes at. Tank depth is a little more.
  WCAddGd               = false;
  DaddLC=false; 
  DaddWLSP=false;
}

void WCSimDetectorConstruction::SetMailBox300kTGeometry()    // This should setup a 300kT (metric) Fiducial Volume
{
  // PMT stuff
  WCPMTName               = "10inch";//normal qe 10 inch tube
  WCPMTRadius				=.127*m;
  WCPMTExposeHeight		=WCPMTRadius - 0.01*m;
  WCPMTRadiusEff        =  WCPMTExposeHeight ;
  WCPMTGlassThickness		= .55*cm;
  WCPMTPercentCoverage	= 10;	//% coverage
  WCBlackSheetThickness	= 2.0*cm;
  //Tank and Cavern Dimensions
  WC_MB_Tank_Airgap		=2.0*m;	//Arbitrary here, but this is just the height of air between the top of the outer Veto_thickness and the beginning of the Cavern Dome
  WC_MB_Dome_Height_fraction	=0.2;//fraction should be <=0.5
  WC_MB_Veto_Thickness	=2.0*m;// This is just the spacing (in water) between the outer side of the Blacksheet and the beginning of the fiducial volume
  WC_MB_Buffer_Thickness	=2.5*m; //Distance between wall and beginning of the Fiducial volume
  //Fiducial VOlume Stuff--these numbers are taken from table 9.1 of DUSEL LCAB Report #2, 7/2009, adjusted to Fiducial Volume (F.V.) and 2.5m gap between wall and F.V.
  WC_MB_Fid_Length        =( 136.0*m+2*WC_MB_Veto_Thickness);	//	// strictly speaking this isn't the fiducial length anymore, but increased by the veto thickness
  WC_MB_Fid_Width         = (40.0*m+2*WC_MB_Veto_Thickness);	// this gives a volume of 299.2 metric tons
  WC_MB_Fid_Depth         = (55.0*m+2*WC_MB_Veto_Thickness);	// how deep is the bottom layer of tubes at. Tank depth is a little more.
  WCAddGd               = false;
  DaddLC=false; 
  DaddWLSP=false;
}
//aah





//PMT logical volume construction.
void WCSimDetectorConstruction::ConstructPMT()
{
  if(addWLS)  G4cout<<"WLS coat is added "<<G4endl;
  if(addWLSP){
    if (DaddWLSP){
      G4cout<<"WLS Plates are added "<<G4endl;
    }else{
      G4cout<<"WLS Plates are not supported in this configuration   "<<G4endl;
    }
  }
  if(addLC){
    if (DaddLC){
      G4cout<<"Winston Cones are added "<<G4endl;
    }else {
      G4cout<<"Winston Cones are not supported in this configuration  "<<G4endl;
    }
  }
  //if (addWLS)
  //  {
    sphereRadius = (WCPMTExposeHeight*WCPMTExposeHeight+ WCPMTRadius*WCPMTRadius)/(2*WCPMTExposeHeight);
    PMTOffset =  sphereRadius - WCPMTExposeHeight;
    
    // the PMT is the top part of a hemisphere. The origin of the coordinate system is
    // at the center of the spere. Thus you have to place the PMTs PMTOffset behind the
    // blacksheet surface.
    G4Box* cutOfTubs = new G4Box("cutOf",
				 sphereRadius+1.*cm,
				 sphereRadius+1.*cm,
				 PMTOffset);
    
    G4Sphere* tmpSolidWCPMT = new G4Sphere("tmpWCPMT",
					   0.0*m,(sphereRadius-WCPMTGlassThickness),
					   0.0*deg,360.0*deg,
					   0.0*deg,90.0*deg);
    
    G4SubtractionSolid* solidWCPMT =  new G4SubtractionSolid("WCPMT",tmpSolidWCPMT,cutOfTubs);

    logicWCPMT = 
      new G4LogicalVolume(solidWCPMT,
			  G4Material::GetMaterial("Air"),
			  "WCPMT",
			  0,0,0);
    
    
    Double_t wls_thickness=0.;
    Double_t glassdim= 90.0*deg;
   
    // G4cout<<"PMT glass dim= "<<glassdim/deg<<" deg "<<G4endl;
    if(addWLS)
      {
	// hard coded WLS film thickness! X.Q. 2011.5.25 
      wls_thickness = 10.*2.54e-3*cm;
       
    G4Sphere* tmpWLSWCPMT = new G4Sphere("tmpWLSWCPMT",
					 (sphereRadius-wls_thickness),
					 sphereRadius,
					 0.0*deg,360.0*deg,
					 0.0*deg,glassdim);
    G4SubtractionSolid* WLSWCPMT = new G4SubtractionSolid("WLSFaceWCPMT",tmpWLSWCPMT,cutOfTubs);
    
    logicWLSFaceWCPMT = 
      new G4LogicalVolume(WLSWCPMT,
			  G4Material::GetMaterial("acrylic_wls_film"),
			  "WLSFaceWCPMT",
			  0,0,0);
      } //end if addWLS

    G4Sphere* tmpGlassWCPMT = new G4Sphere("tmpGlassWCPMT",
					 (sphereRadius-WCPMTGlassThickness),
					 sphereRadius-wls_thickness,
					 0.0*deg,360.0*deg,
					 0.0*deg,glassdim);
    G4SubtractionSolid* glassWCPMT = new G4SubtractionSolid("GlassFaceWCPMT",tmpGlassWCPMT,cutOfTubs);
    
    
    logicGlassFaceWCPMT =
      new G4LogicalVolume(glassWCPMT,
			  G4Material::GetMaterial("Glass"),
			  "glassFaceWCPMT",
			  0,0,0);
    
    
   

    // Set Visual Attributes
    
    G4VisAttributes* PMTVisAtt = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
    //  PMTVisAtt->SetForceWireframe(true);
    
    PMTVisAtt->SetForceSolid(true);
    logicWCPMT->SetVisAttributes(PMTVisAtt);
    //  logicGlassFaceWCPMT->SetVisAttributes(PMTVisAtt);
    if(addWLS) logicWLSFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible); 
    logicGlassFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible); 
    

    if(!debugMode) {
      logicWCPMT->SetVisAttributes(G4VisAttributes::Invisible);
     if(addWLS)  logicWLSFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible); 
      logicGlassFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible); 
    }

    //  } //if addWLS(the if at the begining of the function
}

//K.Z.

#include <G4NistManager.hh>
#include "WCSimLC.hh"

void WCSimDetectorConstruction::ConstructLC()
{
  
  G4NistManager* man = G4NistManager::Instance();
  G4Material* conc_material = man->FindOrBuildMaterial("G4_PLEXIGLASS");
  logiLC = new WCSimLC("LC", conc_material);
  logiLC->SetInvisible();
   
  offset = 5.0 *mm;

 //     logiLC->SetVisAttributes(G4VisAttributes::Invisible); 
}

#include "WCSimWLSP.hh"
void WCSimDetectorConstruction::ConstructWLSP()
{
  
  G4NistManager* man = G4NistManager::Instance();
  G4Material* plat_material = man->FindOrBuildMaterial("G4_PLEXIGLASS");
  G4Material* plate_material;


  if (Plate_size == 0){
    plate_material= man->FindOrBuildMaterial("BC408");
  }else{
    plate_material= man->FindOrBuildMaterial("BC408_high");
  }

  
  logiWLSPLC = new WCSimWLSP_LC("WLSPLC",plat_material,plate_material,Plate_size);
  logiWLSPLC->SetInvisible();
  G4Material* refl_material = man->FindOrBuildMaterial("G4_POLYETHYLENE");
  G4Material* refl1_material = man->FindOrBuildMaterial("Tyvek_wlsp");
  logiWLSPReflector = new WCSimWLSP_Reflector("WLSPReflector",refl_material,refl1_material,Plate_size);
  WLSP_offset=35.0 *mm;//original
  logiWLSPReflector->SetInvisible();

 //     logiWLSPLC->SetVisAttributes(G4VisAttributes::Invisible); 
 //     logiWLSPReflector->SetVisAttributes(G4VisAttributes::Invisible); 
}


//aah
G4LogicalVolume* WCSimDetectorConstruction::ConstructMailboxWC()
{
	debugMode=false;
	//attempt by aah--1/21/10-2/22/10
	
	//Define geometry of world volumes. At this stage, the world volume will just be the cavern of the excavated detector. It is a rectangular parallelepiped, with a circular arc
	//ceiling.
	
	// The size is set by the dimensions of the fiducial volume declared in the set geometry function, with added spacing for veto/dead water regions, and air just above water surface,
	// extending into the cylindrical domed area.
	// The origin (0,0,0) is at the center of the "Fiducial" and "Tank_H2O" volumes. The Tank_H2O volume size includes:
	// 1) the thin PMT volumes that sit just outside the Fiducial Volume's six faces, and
	// 2) a "Veto" region. Whether this Veto is actually a veto, or simply an inactive layer of water which might include the pmt support structures is left open. The "Veto" region is
	//     the same thickness on all six sides.
	//
	// Above the top of the upper "Veto" region, a rectangular "Airgap" is included, to simulate what might be there.
	// Above the "Airgap" is the domed upper part of the cavern. I give a variable, "WC_MB_Dome_Height_fraction" that sets the shape of the Dome. If this variable=0.5, then the dome
	// is just a half cylinder, whose radius is equal to the "WC_MB_Fid_Width" parameter. If the variable is less than 0.5, then the dome is a smaller section of a cylinder, with the
	// chord also equal to the "WC_MB_Fid_Width" setting. You should not choose values >0.5!---that would look pretty odd I think, the dome being a shape which is larger than a halfcylinder.
	
	// The PMT volumes are built as LxW, WxD, and LxD faces. (L=length, W= width, D=Depth). The length is along the "X" axis, the width along the "Y" axis, and depth alond the "Z" axis.
	//Since the PMT as initially constructed is symmetric about the Z axis, and is facing "upward", along the "Z axis, I build each of the 6 faces, initally oriented on the XY plane,
	// (symmetric around the origin). The PMT spacing is determined automatically from the "WCPMTPercentCoverage" value. Currently the surface area of a hemisphere (2*pi*r^2) is used'
	// for the PMT area. The maximum number of PMT's are placed symmetrically (about the origin) in this thin layer. Then the Blacksheet is laid in. The LxW layers are extended over the WxD 
	// and LxD layers to make a cap.Then "strips" of blacksheet are laid in this extended region to "light-seal" the PMT and Fiducial volumes. The WxD face is also extended on the W dimension
	// as to overlap the LxD face, and then strips of blacksheet are added to seal this too. So in the end, no light from outside the PMT volume can get inside and trugger a PMT (I hope!)
	
	// After the faces are constructed (and I also construct a mirror image for the opposite face), the face is rotated (if necessary) and then translated so that the PMT faces are just
	// outside the Fiducial volume and looking inward into the fiducial volume. Both the PMT volume and the Fiducial Volumes are daughter volumes of the Tank_H2O volume.
	
	
	//For the most part, I think I follow the same prescriptions for sensitizing the PMT's, and making the optical surfaces for the glass and blacksheet as was done for the Cylindrical 
	// Detector.
	
	// The Drawing of the detector shows the watertank volume, with the Fiducial volume and PMT volumes inside.
	
	
	WCPosition=0.;//     aah    WCPosition is used as a depth offset in the Cylinder Detector. I set it to "0" here.
	G4double WC_ActiveLayer_Depth  = WCBlackSheetThickness+WCPMTRadius+0.01*m;             //add 1cm extra thickness just in case
	G4double WC_MB_Cavern_length = WC_MB_Fid_Length+2*(WC_MB_Buffer_Thickness-WC_MB_Veto_Thickness);
	G4double WC_MB_Cavern_width = WC_MB_Fid_Width+2*(WC_MB_Buffer_Thickness-WC_MB_Veto_Thickness);
	G4double WC_MB_Tank_depth = WC_MB_Fid_Depth+2*(WC_MB_Buffer_Thickness-WC_MB_Veto_Thickness);      //note that I want the 0,0,0 position oriented in center of water tank, so I won't include airgap here
	G4cout << "Cavern Length= "<< WC_MB_Cavern_length/m << "m,  Cavern_Width= "<< WC_MB_Cavern_width/m << "m, Tank Depth= "<< WC_MB_Tank_depth/m << "m \n";

	WC_MB_Fid_Length -= 2*(WC_ActiveLayer_Depth-WCBlackSheetThickness);	//these 3 lines are to adjust the fiducial size to "fake out" the sensistive volume size so
	WC_MB_Fid_Width -= 2*(WC_ActiveLayer_Depth-WCBlackSheetThickness);	// that it will fit the FV+"veto" with the blacksheet front face 0.5 m from the wall
	WC_MB_Fid_Depth -= 2*(WC_ActiveLayer_Depth-WCBlackSheetThickness);	// and 2m from the FV. I know it is a complicated mess!
	
	
	// now we know the extent of the detector and are able to tune the tolerance
	G4GeometryManager::GetInstance()->SetWorldMaximumExtent(WC_MB_Fid_Length > WC_MB_Fid_Depth ? WC_MB_Fid_Length : WC_MB_Fid_Depth);
	G4cout << "Computed tolerance = "<< G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm<< " mm" << G4endl;			
	
	// Now make the geometry
	G4double WC_MB_Dome_Height=WC_MB_Dome_Height_fraction*WC_MB_Cavern_width;//
	G4Box* WC_MB_tank_H20= 	new G4Box("Tank",	WC_MB_Cavern_length/2.0	,WC_MB_Cavern_width/2.0,	WC_MB_Tank_depth/2.0);//	Note I will add a rectangular airgap above this.
	G4Box*	WC_MB_Tank_Airgap_volume=	new G4Box("Airgap",	WC_MB_Cavern_length/2.0	,	WC_MB_Cavern_width/2.0,		WC_MB_Tank_Airgap/2.0);	//This is airgap above water
	
	WCLength =(WC_MB_Cavern_length > WC_MB_Tank_depth+WC_MB_Tank_Airgap+ WC_MB_Dome_Height? WC_MB_Cavern_length : WC_MB_Tank_depth+WC_MB_Tank_Airgap+WC_MB_Dome_Height);

    //Decide if adding Gd
    G4String water = "Water";
    if (WCAddGd)
    {water = "Doped Water";}
	
	
	//*********************											  
	//This is for calculation of the ceiling volume
	
	G4double phi_ceiling = 2.*atan(2.*WC_MB_Dome_Height/WC_MB_Cavern_width);
	G4double radius_ceiling=		WC_MB_Cavern_width/(2 *sin(phi_ceiling));                   //so this volume is not necessarily a half cylinder (phi=pi)
	G4cout << "phi" << phi_ceiling <<"   radius= "<< radius_ceiling/m<<"m"<<"\n";
	G4cout << "dome extent" << pi-phi_ceiling <<"    "<< pi+phi_ceiling<<"\n";
	G4Tubs* WC_MB_Cavern_ceiling_cyl = new G4Tubs("ceiling_cyl",0.0,radius_ceiling,WC_MB_Cavern_length/2.,pi-phi_ceiling,2*phi_ceiling);
	G4Box* subtract_to_make_ceiling= new G4Box("subtract",radius_ceiling,radius_ceiling,1+WC_MB_Cavern_length/2.);///a little bit longer since I seemed to have some extra stuff left over.
	G4SubtractionSolid*  WC_MB_Cavern_ceiling= new G4SubtractionSolid("Ceiling",WC_MB_Cavern_ceiling_cyl,subtract_to_make_ceiling,0,G4ThreeVector(+WC_MB_Dome_Height,0.,0.));//this is now ceiling, but it needs to be rotated +90 degrees around y axis
	G4Transform3D rotation_dome = G4RotateY3D(pi/2.);
	G4double delta=-radius_ceiling+WC_MB_Dome_Height+WC_MB_Tank_Airgap/2;
	G4cout << "delta= " << delta/m << "m \n";
	G4Translate3D translate_dome(0,0,delta*.999999);//for some reason the dome disappears when I use delta alone. Not sure what is going on here.
	G4UnionSolid* WC_MB_Tank_Airgap_volume_dome=new G4UnionSolid("Airgap and dome",WC_MB_Tank_Airgap_volume,WC_MB_Cavern_ceiling,translate_dome*rotation_dome);//hopefully rotates first, then translates
	//**********************
	
	
	// Now put the cavern together with a union.
	G4ThreeVector ztrans= G4ThreeVector(0.0,0.0,(WC_MB_Tank_depth+WC_MB_Tank_Airgap)/2);  // position measured relative to center of volumes.
	G4UnionSolid* WC_Cavern=new G4UnionSolid("Cavern",WC_MB_tank_H20,WC_MB_Tank_Airgap_volume_dome,0,ztrans);
	
	//Now declare the cavern logical volume with air (I like to breathe).
	G4LogicalVolume* logic_WC_Cavern=new G4LogicalVolume(WC_Cavern,G4Material::GetMaterial("Air"),"Cavern",0,0,0);
	
	G4VisAttributes* showColor = new G4VisAttributes(G4Colour(0.0,1.0,0.0)); // Note that I should make this visible, at least initially.
	if (debugMode)		
		logic_WC_Cavern->SetVisAttributes(showColor);
	else	 
		logic_WC_Cavern->SetVisAttributes(G4VisAttributes::Invisible); //amb79
	
	//Make Logical Water tank. This is just the same volume as  WC_MB_tank_H20, but now filled with water
	G4LogicalVolume* logic_WC_MB_tank_H20= new G4LogicalVolume(WC_MB_tank_H20,G4Material::GetMaterial(water),"Tank",0,0,0);
	
	
	//Make Fiducial Volume.
	G4Box* WC_MB_Fiducial = new G4Box ("Fiducial Box",WC_MB_Fid_Length/2.,WC_MB_Fid_Width/2.,WC_MB_Fid_Depth/2.);
	G4LogicalVolume* logic_WC_MB_Fiducial= new G4LogicalVolume(WC_MB_Fiducial,G4Material::GetMaterial(water),"Fiducial_Log",0,0,0);
	
	//these volumes will be placed into the cavern just before we exit.
	
	
	
	//Now make 6 active Layers which will contain the PMT's and Black plastic as daughter volumes. These volumes are sized to the surface area
	//of six faces of the fiducial volume.
	
	//WC_Active_LxW_minus---nomenclature is that this is the face of dimensions LxW located on the negative Z axis (PMT looking upward)
	//WC_Active_LxW_plus---nomenclature is that this is the face of dimensions LxW located on the positive Z axis (PMTlooking downward)
	//WC_Active_LxD_minus---nomenclature is that this is the face of dimensions LxD located on the negative Y axis (PMTlooking toward +Y)
	//WC_Active_LxD_plus---nomenclature is that this is the face of dimensions LxD located on the positive Y axis (PMTlooking toward -Y)
	//WC_Active_WxD_minus---nomenclature is that this is the face of dimensions WxD located on the negative X axis (PMTlooking toward +X)
	//WC_Active_WxD_plus---nomenclature is that this is the face of dimensions WxD located on the positive X axis (PMTlooking toward -X)
	
	
	// need PMT's first. Just a copy paste from Cylindrical detector
	
	//////////// M Fechner : I need to  declare the PMT  here in order to
	// place the PMTs in the truncated cells
	//-----------------------------------------------------
	// The PMT
	//-----------------------------------------------------
	
	////////////J Felde: The PMT logical volume is now constructed separately 
    //from any specific detector geometry so that both the Cylindrical and 
    //Mailbox geometries
	//  can use the same definition. 
	//K.Zbiri: The PMT volume and the PMT glass are now put in parallel. 
    //The PMT glass is the sensitive volume in this new configuration.
	
	ConstructPMT();
	//That's all!	
	
	//Now calculate the L,W,D offsets and # pmt's from the requested spacing.
	//	G4double WCPMT_surfacearea=2*pi*WCPMTRadius*WCPMTRadius;//this is the surface area of a hemisphere
	G4double WCPMT_crossarea=pi*WCPMTRadius*WCPMTRadius;// this is just the front face crossectional area---this is our standard definition
	G4double WC_MB_PMT_Spacing = sqrt(100.*WCPMT_crossarea/WCPMTPercentCoverage);	// factor 100 to conver % to fraction

	//**************************try to use replica--here I am making the unit PMT Cell  ***replica
	//make fundamental square PMT cell, equal to PMT spacing on large side and depth=WC_ActiveLayer_Depth--I will put PMT and blacksheet in this volume  ***replica
	G4Box* WC_PMT_Cell = new G4Box("PMT_cell",WC_MB_PMT_Spacing/2,WC_MB_PMT_Spacing/2,WC_ActiveLayer_Depth/2);	//***replica
	G4LogicalVolume* logic_WC_PMT_Cell= new G4LogicalVolume(WC_PMT_Cell,G4Material::GetMaterial(water),"logic_PMT_Cell",0,0,0);	//***replica
	G4Box* WC_Blacksheet_Cell = new G4Box("WC_BlackSheet_Cell",WC_MB_PMT_Spacing/2,WC_MB_PMT_Spacing/2,WCBlackSheetThickness/2.);		//blacksheet material to fit in cell ***replica
	G4LogicalVolume* logic_WC_Blacksheet_Cell=new G4LogicalVolume(WC_Blacksheet_Cell,G4Material::GetMaterial("Blacksheet"),"logic_WC_Blacksheet_Cell",0,0,0);	//***replica
	// Lets put this stuff+PMT into the cell		//***replica
	//First the glass					//***replica
	G4VPhysicalVolume* phys_WC_GlassFacePMT_Cell=	//this id is needed in order to enable some surface features	//***replica
	new G4PVPlacement(0,                         // no rotation			//***replica
					  G4ThreeVector(0., 0., -WC_ActiveLayer_Depth/2.+WCBlackSheetThickness-PMTOffset),                   // its position	//***replica//"-PMTOffset" butts glass envelope up to blacksheet-aah 10/11/2010
					  logicGlassFaceWCPMT,                // its logical volume   //***replica
					  "WCPMTGlass",// its name //***replica
					  logic_WC_PMT_Cell,         // its mother volume	//***replica
					  false,                     // no boolean os	//***replica
					  0);   
	// Next the PMT (sensitive volume)	//***replica
	G4VPhysicalVolume* phys_WC_PMT_Cell=	//***replica
	new G4PVPlacement(0,                         // no rotation	//***replica
					  G4ThreeVector(0., 0., -WC_ActiveLayer_Depth/2.+WCBlackSheetThickness-PMTOffset),                   // its position	//***replica//"-PMTOffset" butts glass envelope up to blacksheet-aah 10/11/2010
					  logicWCPMT,                // its logical volume	//***replica
					  "WCPMT",// its name		//***replica
					  logic_WC_PMT_Cell,         // its mother volume	//***replica
					  false,                     // no boolean os		//***replica
					  0);	//***replica
	// Next the blacksheet	//***replica
	G4VPhysicalVolume* phys_WC_Blacksheet_Cell=	//***replica
	new G4PVPlacement(0,                         // no rotation	//***replica
					  G4ThreeVector(0,0,(-WC_ActiveLayer_Depth+WCBlackSheetThickness)/2),    // its position	//***replica
					  logic_WC_Blacksheet_Cell,                // its logical volume	//***replica
					  "LW_Blacksheet", // its name	//***replica
					  logic_WC_PMT_Cell,         // its mother volume	//***replica
					  false,                     // no boolean os	//***replica
					  0);                        // copy #.				  //***replica
	
	// Turn on the special features of the surface	//***replica
	//glass reflections				//***replica
	new G4LogicalBorderSurface("GlassCathodeSurface",	//***replica
							   phys_WC_GlassFacePMT_Cell,phys_WC_PMT_Cell,	//***replica
							   OpGlassCathodeSurface); //***replica
	// Blacksheet reflections	//***replica
	new G4LogicalBorderSurface("WaterBS_LW_PolySurface",	//***replica
							   phys_WC_PMT_Cell,phys_WC_Blacksheet_Cell,	//***replica
							   OpWaterBSSurface);	//***replica
	
	if (!debugMode)	{
		logic_WC_PMT_Cell->SetVisAttributes(G4VisAttributes::Invisible);//*replica
		logic_WC_Blacksheet_Cell->SetVisAttributes(G4VisAttributes::Invisible);//*replica
	}
	//*******************************************************************************	//***replica
	



	//Length dimension
	G4int WC_MB_NumPMT_L		=(1+ int((WC_MB_Fid_Length-2*sphereRadius)/WC_MB_PMT_Spacing));	//# PMT's along length
	G4double WC_MB_PMT_offset_L	=(WC_MB_Fid_Length-(WC_MB_NumPMT_L-1)*WC_MB_PMT_Spacing)/2;		//Offset in length dimension
	//Width
	G4int WC_MB_NumPMT_W		=(1+ int((WC_MB_Fid_Width-2*sphereRadius)/WC_MB_PMT_Spacing));	//# PMT's along width
	G4double WC_MB_PMT_offset_W	=(WC_MB_Fid_Width-(WC_MB_NumPMT_W-1)*WC_MB_PMT_Spacing)/2;	//Offset in width dimension
	//Depth
	G4int WC_MB_NumPMT_D		=(1+ int((WC_MB_Fid_Depth-2*sphereRadius)/WC_MB_PMT_Spacing));	//# PMT's along depth
	G4double WC_MB_PMT_offset_D	=(WC_MB_Fid_Depth-(WC_MB_NumPMT_D-1)*WC_MB_PMT_Spacing)/2;	//Offset in depth dimension
	G4cout << "PMT Length Offset (m) "<< WC_MB_PMT_offset_L/m << "PMT Width Offset (m) " << WC_MB_PMT_offset_W/m<<"PMT Depth Offset (m) "<<WC_MB_PMT_offset_D/m<<"\n";
	
	//WC_Active_LxW---perp to Z axis
	
	G4Box* WC_Active_LxW = new G4Box("LxW",WC_MB_Fid_Length/2+WC_ActiveLayer_Depth,WC_MB_Fid_Width/2+WC_ActiveLayer_Depth,WC_ActiveLayer_Depth/2);	//active volume with PMT's 
	G4LogicalVolume* logic_WC_Active_LxW_minus=new G4LogicalVolume(WC_Active_LxW,G4Material::GetMaterial(water),"LxWActive_minus",0,0,0);
	G4LogicalVolume* logic_WC_Active_LxW_plus=new G4LogicalVolume(WC_Active_LxW,G4Material::GetMaterial(water),"LxWActive_plus",0,0,0);
	G4VPhysicalVolume* phys_WC_Active_LxW_minus;
	G4VPhysicalVolume* phys_WC_ActiveGlass_LxW_minus;
	G4VPhysicalVolume* phys_WC_Active_LxW_plus;
	G4VPhysicalVolume* phys_WC_ActiveGlass_LxW_plus;
	G4Box* WC_BlacksheetLxW = new G4Box("WC_BlackSheetLxW",WC_MB_Fid_Length/2+WC_ActiveLayer_Depth-WCBlackSheetThickness,
										WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness,WCBlackSheetThickness/2.);		//blacksheet material	
	//*************************replica***********************
	
	//need to cutout center rectangle that will fit the replica pmtvolumes (already with blacksheet)   ***replica
	G4Box* WC_BlacksheetLxW_donuthole = new G4Box("WC_BlackSheetLxW_donuthole",WC_MB_NumPMT_L*WC_MB_PMT_Spacing/2, //***replica
												  WC_MB_NumPMT_W*WC_MB_PMT_Spacing/2,WCBlackSheetThickness/2.);				//***replica
	
	G4SubtractionSolid*  WC_BlacksheetLxW_donut= new G4SubtractionSolid("WC_BlacksheetLxW_donut",WC_BlacksheetLxW,WC_BlacksheetLxW_donuthole,0,G4ThreeVector(0.,0.,0.));	//this is now the blacksheet that will suround the PMT volumes  ***replica							
	//***replica
	
	G4LogicalVolume* logic_WC_BlacksheetLxW = new G4LogicalVolume(WC_BlacksheetLxW_donut,G4Material::GetMaterial("Blacksheet"),"LagicBlacksheet_LxW",0,0,0);				 //***replica														
	
	//*********************************replica*******************************



	G4Box* WC_BlacksheetLxW_corner_L= new G4Box("WC_BlackSheetLxW_corner_L",WC_MB_Fid_Length/2+WC_ActiveLayer_Depth,WCBlackSheetThickness/2.,WC_ActiveLayer_Depth/2.);//Blacksheeet corner--to lite-tite the fiducial volume--This should already be oriented correctly
	G4LogicalVolume* logic_WC_BlacksheetLxW_corner_L=new G4LogicalVolume(WC_BlacksheetLxW_corner_L,G4Material::GetMaterial("Blacksheet"),"LagicBlacksheet_LxW",0,0,0);
	G4Box* WC_BlacksheetLxW_corner_W= new G4Box("WC_BlackSheetLxW_corner_W",WCBlackSheetThickness/2.,WC_MB_Fid_Width/2+WC_ActiveLayer_Depth,WC_ActiveLayer_Depth/2.);//Blacksheet corner--lite-tite the fiducial volume--This should already be oriented correctly
	G4LogicalVolume* logic_WC_BlacksheetLxW_corner_W=new G4LogicalVolume(WC_BlacksheetLxW_corner_W,G4Material::GetMaterial("Blacksheet"),"LagicBlacksheet_LxW",0,0,0);
	
	
	G4double xmin=WC_MB_Fid_Length, xmax=-WC_MB_Fid_Length, ymin=WC_MB_Fid_Width, ymax=-WC_MB_Fid_Width;
	G4int icopy=0,num_pmt=0;

	//***replica
	// will build up pmt volume in two steps, one making a row of pmt volumes, then making a rectangle out of the rows.
	
	G4Box *LxW_PMT_Row =	new G4Box("LxW_PMT_Row",WC_MB_NumPMT_L*WC_MB_PMT_Spacing/2,WC_MB_PMT_Spacing/2,WC_ActiveLayer_Depth/2.);
	G4LogicalVolume* logic_LxW_PMT_Row= new G4LogicalVolume(LxW_PMT_Row,G4Material::GetMaterial(water),"logic_LxW_PMT_Row",0,0,0);
	G4Box * LxW_PMT_Volume=new G4Box("LxW_PMT_Volume",WC_MB_NumPMT_L*WC_MB_PMT_Spacing/2,WC_MB_NumPMT_W*WC_MB_PMT_Spacing/2,WC_ActiveLayer_Depth/2);	//this is entire PMT volume
	G4LogicalVolume* logic_LxW_PMT_Volume= new G4LogicalVolume(LxW_PMT_Volume,G4Material::GetMaterial(water),"logic_LxW_PMT_Volume",0,0,0);
	
	G4VPhysicalVolume* physiWC_LxW_PMT_Row = 
	new G4PVReplica("WC_PMT_Row",
					logic_WC_PMT_Cell,
					logic_LxW_PMT_Row,
					kXAxis,
					WC_MB_NumPMT_L,
					WC_MB_PMT_Spacing,
					0.); 
	
	G4VPhysicalVolume* physiWC_LxW_PMT_Volume = 
	new G4PVReplica("WC_LxW_PMT_Volume",
					logic_LxW_PMT_Row,
					logic_LxW_PMT_Volume,
					kYAxis,
					WC_MB_NumPMT_W,
					WC_MB_PMT_Spacing,
					0.); 
	
	//Now we should have PMT's placed in the PMT "Donut_Hole"
	if (!debugMode)	{
		logic_LxW_PMT_Row->SetVisAttributes(G4VisAttributes::Invisible);//*replica
		logic_LxW_PMT_Volume->SetVisAttributes(G4VisAttributes::Invisible);//*replica
	}
	//Now put into face volumes
	phys_WC_Active_LxW_minus =					// put PMT's into bottom face--eventually it will be translated to bottom face
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0.,0.,0.),                   // its position
					  logic_LxW_PMT_Volume,                // its logical volume
					  //"LxW_PMT_minus", // its name
					  "WCPMT",// (JF) replaced above  
					  logic_WC_Active_LxW_minus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // every PMT needs an unique id in this physical volume		??? what to do here!??
	
	
	phys_WC_Active_LxW_plus =					// put PMT's into bottom face--eventually it will be translated to bottom face
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0.,0.,0.),                   // its position
					  logic_LxW_PMT_Volume,                // its logical volume
					  //"LxW_PMT_minus", // its name
					  "WCPMT",// (JF) replaced above  
					  logic_WC_Active_LxW_plus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // every PMT needs an unique id in this physical volume		??? what to do here!??
	
	//end ****replica					

	G4cout << "total on LxW Surface: " << 2*WC_MB_NumPMT_L*WC_MB_NumPMT_W << "\n";	//***replica (modded original)
	G4cout << "Coverage was calculated to be: " << (WC_MB_NumPMT_L*WC_MB_NumPMT_W *WCPMT_crossarea/(WC_MB_Fid_Length*WC_MB_Fid_Width)) << "\n";
	num_pmt+=2*WC_MB_NumPMT_L*WC_MB_NumPMT_W;  //***replica	

	
	//add blacksheet
	
	G4ThreeVector position_blacksheet_LxW =G4ThreeVector(0,0,(-WC_ActiveLayer_Depth+WCBlackSheetThickness)/2) ;//hopefully this puts blacksheet on bottom of volume.
	
	G4VPhysicalVolume*	phys_WC_Active_LxW_blacksheet_minus =
	new G4PVPlacement(0,                         // no rotation
					  position_blacksheet_LxW,    // its position
					  logic_WC_BlacksheetLxW,                // its logical volume
					  "LW_Blacksheet", // its name 
					  logic_WC_Active_LxW_minus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // copy #.
	
	phys_WC_Active_LxW_blacksheet_minus =					//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0,-(WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0),    // its position
					  logic_WC_BlacksheetLxW_corner_L,                // its logical volume
					  "LW_Blacksheet_corner_L", // its name 
					  logic_WC_Active_LxW_minus,         // its mother volume
					  false,                     // no boolean os
					  1);	// copy #.
	phys_WC_Active_LxW_blacksheet_minus =					//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0,(WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0),    // its position opposite side
					  logic_WC_BlacksheetLxW_corner_L,                // its logical volume
					  "LW_Blacksheet_corner_L", // its name 
					  logic_WC_Active_LxW_minus,         // its mother volume
					  false,                     // no boolean os
					  2); // copy #.
	
	phys_WC_Active_LxW_blacksheet_minus =					//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(-(WC_MB_Fid_Length/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0,0),    // its position opposite side
					  logic_WC_BlacksheetLxW_corner_W,                // its logical volume
					  "LW_Blacksheet_corner_W", // its name 
					  logic_WC_Active_LxW_minus,         // its mother volume
					  false,                     // no boolean os
					  3); // copy #.
	
	
	phys_WC_Active_LxW_blacksheet_minus =					//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector((WC_MB_Fid_Length/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0,0),    // its position opposite side
					  logic_WC_BlacksheetLxW_corner_W,                // its logical volume
					  "LW_Blacksheet_corner_W", // its name 
					  logic_WC_Active_LxW_minus,         // its mother volume
					  false,                     // no boolean os
					  4); // copy #.
	
	//	Now do the same for the positive side pmt layer	
	
	G4VPhysicalVolume*	phys_WC_Active_LxW_blacksheet_plus =
	new G4PVPlacement(0,                         // no rotation
					  position_blacksheet_LxW,    // its position
					  logic_WC_BlacksheetLxW,                // its logical volume
					  "LW_Blacksheet", // its name 
					  logic_WC_Active_LxW_plus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // copy #.
	
	
	phys_WC_Active_LxW_blacksheet_plus =	//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0,-(WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0),    // its position opposite side
					  logic_WC_BlacksheetLxW_corner_L,                // its logical volume
					  "LW_Blacksheet_corner_L", // its name 
					  logic_WC_Active_LxW_plus,         // its mother volume
					  false,                     // no boolean os
					  1); 										
	
	phys_WC_Active_LxW_blacksheet_plus =	//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0,(WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0),    // its position opposite side
					  logic_WC_BlacksheetLxW_corner_L,                // its logical volume
					  "LW_Blacksheet_corner_L", // its name 
					  logic_WC_Active_LxW_plus,         // its mother volume
					  false,                     // no boolean os
					  2); 
	
	phys_WC_Active_LxW_blacksheet_plus =	//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(-(WC_MB_Fid_Length/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0,0),    // its position opposite side
					  logic_WC_BlacksheetLxW_corner_W,                // its logical volume
					  "LW_Blacksheet_corner_W", // its name 
					  logic_WC_Active_LxW_plus,         // its mother volume
					  false,                     // no boolean os
					  3); 
	
	
	phys_WC_Active_LxW_blacksheet_plus =	//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector((WC_MB_Fid_Length/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0,0),    // its position opposite side
					  logic_WC_BlacksheetLxW_corner_W,                // its logical volume
					  "LW_Blacksheet_corner_W", // its name 
					  logic_WC_Active_LxW_plus,         // its mother volume
					  false,                     // no boolean os
					  4); 
	
	
	new G4LogicalBorderSurface("WaterBS_LW_PolySurface",
							   phys_WC_Active_LxW_minus,phys_WC_Active_LxW_blacksheet_minus,
							   OpWaterBSSurface);
	new G4LogicalBorderSurface("WaterBS_LW_PolySurface",
							   phys_WC_Active_LxW_plus,phys_WC_Active_LxW_blacksheet_plus,
							   OpWaterBSSurface);																						
	
	
	//visualization if debugMode=true																						
	G4VisAttributes* WC_BlacksheetLxWVisAtt = new G4VisAttributes(G4Colour(0,0.0,1.));
	G4VisAttributes* WC_BlacksheetLxWVisAtt_corner = new G4VisAttributes(G4Colour(1.,0.0,1.));
	if(debugMode){
		logic_WC_BlacksheetLxW->SetVisAttributes(WC_BlacksheetLxWVisAtt);
		logic_WC_BlacksheetLxW_corner_L->SetVisAttributes(WC_BlacksheetLxWVisAtt);
		logic_WC_BlacksheetLxW_corner_W->SetVisAttributes(WC_BlacksheetLxWVisAtt_corner);
	}
	else {
		logic_WC_BlacksheetLxW->SetVisAttributes(G4VisAttributes::Invisible);
		logic_WC_BlacksheetLxW_corner_L->SetVisAttributes(G4VisAttributes::Invisible);
		logic_WC_BlacksheetLxW_corner_W->SetVisAttributes(G4VisAttributes::Invisible);
	}
	
	
	
	//At this stage we should have the bottom layer containing the PMT's and blacksheet defined--but located in the middle of the water tank
	//Now I will make the translation/rotation which will place the top and bottom layers just outsidethe fiducial detector (in the logical watertank volume.
	
	G4Translate3D LWTranslation_plus(0., 0., +(WC_MB_Fid_Depth+WC_ActiveLayer_Depth)/2);				//translation to top of Fiducial surface (+z)
	G4Translate3D LWTranslation_minus(0., 0., -(WC_MB_Fid_Depth+WC_ActiveLayer_Depth)/2);				//Translation to bottom surface
	const G4Transform3D rotation_LxW_plus = G4RotateX3D(pi);												//rotate to looking in -z direction (180 degrees) for top face
	
	//ok, put up the sides!
	// First side is on the negative axis--this just requires a translation
	new G4PVPlacement(LWTranslation_minus  ,           // no rotation , translation only
					  logic_WC_Active_LxW_minus,     // its logical volume				  
					  "LW_minus",        // its name
					  logic_WC_MB_tank_H20,      // its mother  volume, the WC water tank
					  false,           // no boolean operations
					  0);              // copy number 
	
	new G4PVPlacement(LWTranslation_plus*rotation_LxW_plus, // rotate +180 degrees, then translate upward.
					  logic_WC_Active_LxW_plus,     // its logical volume				  
					  "LW_plus",        // its name
					  logic_WC_MB_tank_H20,      // its mother  volume, the WC water tank
					  false,           // no boolean operations
					  1);              // copy number 
	
	
	
	
	
	//Next two faces
	//WC_Active_WxD	(located on -x axis) I will build this as if it were in the x,y plane (like the LW above) , then rotate it +90degrees around the y axis to make the -x side				 
	
	
	G4Box* WC_Active_WxD=new G4Box("WxD",WC_MB_Fid_Depth/2,WC_MB_Fid_Width/2+WC_ActiveLayer_Depth,WC_ActiveLayer_Depth/2);						//active volume--increase width to cover ends to seal with blacksheet
	G4LogicalVolume* logic_WC_Active_WxD_minus = new G4LogicalVolume(WC_Active_WxD,G4Material::GetMaterial(water),"WxDActive_minus",0,0,0);
	G4LogicalVolume* logic_WC_Active_WxD_plus = new G4LogicalVolume(WC_Active_WxD,G4Material::GetMaterial(water),"WxDActive_plus",0,0,0);
	G4Box* WC_BlacksheetWxD = new G4Box("WC_BlackSheetWxD",WC_MB_Fid_Depth/2,WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2.,WCBlackSheetThickness/2.);			//blacksheet material	

	//*********************************************replica*******************************
	//need to cutout center rectangle that will fit the replica pmtvolumes (already with blacksheet)   ***replica
	G4Box* WC_BlacksheetWxD_donuthole = new G4Box("WC_BlackSheetWxD_donuthole",WC_MB_NumPMT_D*WC_MB_PMT_Spacing/2, //***replica
												  WC_MB_NumPMT_W*WC_MB_PMT_Spacing/2,WCBlackSheetThickness/2.);				//***replica
	
	G4SubtractionSolid*  WC_BlacksheetWxD_donut= new G4SubtractionSolid("WC_BlacksheetWxD_donut",WC_BlacksheetWxD,WC_BlacksheetWxD_donuthole,0,G4ThreeVector(0.,0.,0.));	//this is now the blacksheet that will suround the PMT volumes  ***replica							
	//***replica
	
	G4LogicalVolume* logic_WC_BlacksheetWxD = new G4LogicalVolume(WC_BlacksheetWxD_donut,G4Material::GetMaterial("Blacksheet"),"LagicBlacksheet_WxD",0,0,0);				 //***replica														
	
	
	
	//*********************************************replica*******************************
	

//	G4LogicalVolume* logic_WC_BlacksheetWxD = new G4LogicalVolume(WC_BlacksheetWxD,G4Material::GetMaterial("Blacksheet"),"LagicBlacksheet_WxD",0,0,0);							  
	G4Box* WC_BlacksheetWxD_corner= new G4Box("WC_BlackSheetWxD_corner",WC_MB_Fid_Depth/2,WCBlackSheetThickness/2.,WC_ActiveLayer_Depth/2.);//Blacksheet corner---This should already be oriented correctly
	G4LogicalVolume* logic_WC_BlacksheetWxD_corner=new G4LogicalVolume(WC_BlacksheetWxD_corner,G4Material::GetMaterial("Blacksheet"),"LagicBlacksheet_WxD_corner",0,0,0);
	
	
	
	G4VPhysicalVolume* phys_WC_Active_WxD_plus, * phys_WC_ActiveGlass_WxD_plus;
	G4VPhysicalVolume* phys_WC_Active_WxD_minus, * phys_WC_ActiveGlass_WxD_minus;								  
	
	
	icopy=0;
	xmin=WC_MB_Fid_Depth, xmax=-WC_MB_Fid_Depth, ymin=WC_MB_Fid_Width, ymax=-WC_MB_Fid_Width;
	//***replica
	// will build up pmt volume in two steps, one making a row of pmt volumes, then making a rectangle out of the rows. for this config, "D" runs on X axis, "W" is on Y axis
	
	
	G4Box * WxD_PMT_Row =	new G4Box("WxD_PMT_Row",WC_MB_NumPMT_D*WC_MB_PMT_Spacing/2,WC_MB_PMT_Spacing/2,WC_ActiveLayer_Depth/2.);
	G4LogicalVolume* logic_WxD_PMT_Row= new G4LogicalVolume(WxD_PMT_Row,G4Material::GetMaterial(water),"logic_WxD_PMT_Row",0,0,0);
	G4Box * WxD_PMT_Volume=new G4Box("WxD_PMT_Volume",WC_MB_NumPMT_D*WC_MB_PMT_Spacing/2,WC_MB_NumPMT_W*WC_MB_PMT_Spacing/2,WC_ActiveLayer_Depth/2);	//this is entire PMT volume
	G4LogicalVolume* logic_WxD_PMT_Volume= new G4LogicalVolume(WxD_PMT_Volume,G4Material::GetMaterial(water),"logic_WxD_PMT_Volume",0,0,0);
	
	G4VPhysicalVolume* physiWC_WxD_PMT_Row = 
	new G4PVReplica("WC_PMT_Row",
					logic_WC_PMT_Cell,
					logic_WxD_PMT_Row,
					kXAxis,
					WC_MB_NumPMT_D,
					WC_MB_PMT_Spacing,
					0.); 
	
	G4VPhysicalVolume* physiWC_WxD_PMT_Volume = 
	new G4PVReplica("WC_PMT_Volume",
					logic_WxD_PMT_Row,
					logic_WxD_PMT_Volume,
					kYAxis,
					WC_MB_NumPMT_W,
					WC_MB_PMT_Spacing,
					0.); 
	
	//Now we should have PMT's placed in the PMT "Donut_Hole"
	
	//Now put into face volumes
	phys_WC_Active_WxD_minus =					// put PMT's into bottom face--eventually it will be translated to bottom face
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0.,0.,0.),                   // its position
					  logic_WxD_PMT_Volume,                // its logical volume
					  //"LxW_PMT_minus", // its name
					  "WCPMT",// (JF) replaced above  
					  logic_WC_Active_WxD_minus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // every PMT needs an unique id in this physical volume		??? what to do here!??
	
	
	phys_WC_Active_WxD_plus =					// put PMT's into bottom face--eventually it will be translated to bottom face
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0.,0.,0.),                   // its position
					  logic_WxD_PMT_Volume,                // its logical volume
					  //"LxW_PMT_minus", // its name
					  "WCPMT",// (JF) replaced above  
					  logic_WC_Active_WxD_plus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // every PMT needs an unique id in this physical volume		??? what to do here!??
	
	
	
	if (!debugMode)	{
		logic_WxD_PMT_Row->SetVisAttributes(G4VisAttributes::Invisible);//*replica
		logic_WxD_PMT_Volume->SetVisAttributes(G4VisAttributes::Invisible);//*replica
	}
	
	
	//end ****replica	
	
								
	G4cout << "total on WxD Surface: " << 2*WC_MB_NumPMT_W*WC_MB_NumPMT_D << "\n";//****replica modified original
	G4cout << "Coverage was calculated to be: " << (WC_MB_NumPMT_W*WC_MB_NumPMT_D*WCPMT_crossarea/(WC_MB_Fid_Width*WC_MB_Fid_Depth)) << "\n";// ***replica--modified original
	num_pmt+=2*WC_MB_NumPMT_W*WC_MB_NumPMT_D;  //***replica	
	
	//				G4cout << "halflength, xmin, xmax= " << WC_MB_Fid_Depth/2<<" "<<xmin<<" "<<xmax << "\n";
	//				G4cout << "halfwidth, ymin, ymax=" << WC_MB_Fid_Width/2<<" "<<ymin<<" "<<ymax << "\n";
	//				G4cout << "\n";							
	
	//add blacksheet----on the corners, only need to put it on the +-Width/2 sides since the cap covers the LxW parts	
	
	G4ThreeVector position_blacksheet_WxD =G4ThreeVector(0,0,(-WC_ActiveLayer_Depth+WCBlackSheetThickness)/2) ;//hopefully this puts blacksheet on bottom of volume.
	
	G4VPhysicalVolume*	phys_WC_Active_WxD_blacksheet_minus =
	new G4PVPlacement(0,                         // no rotation
					  position_blacksheet_WxD,    // its position
					  logic_WC_BlacksheetWxD,                // its logical volume
					  "WD_Blacksheet", // its name 
					  logic_WC_Active_WxD_minus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // copy #.
	
	
	
	phys_WC_Active_WxD_blacksheet_minus =//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0,-(WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0),    // its position opposite side
					  logic_WC_BlacksheetWxD_corner,                // its logical volume
					  "LagicBlacksheet_WxD_corner", // its name 
					  logic_WC_Active_WxD_minus,         // its mother volume
					  false,                     // no boolean os
					  1); 
	
	
	phys_WC_Active_WxD_blacksheet_minus =//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0,(WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0),    // its position opposite side
					  logic_WC_BlacksheetWxD_corner,                // its logical volume
					  "LagicBlacksheet_WxD_corner", // its name 
					  logic_WC_Active_WxD_minus,         // its mother volume
					  false,                     // no boolean os
					  2); 																										
	
	
	
	
	
	
	
	G4VPhysicalVolume*	phys_WC_Active_WxD_blacksheet_plus =
	new G4PVPlacement(0,                         // no rotation
					  position_blacksheet_WxD,    // its position
					  logic_WC_BlacksheetWxD,                // its logical volume
					  "WD_Blacksheet", // its name 
					  logic_WC_Active_WxD_plus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // copy #.
	
	
	
	
	phys_WC_Active_WxD_blacksheet_plus =//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0,-(WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0),    // its position opposite side
					  logic_WC_BlacksheetWxD_corner,                // its logical volume
					  "WLagicBlacksheet_WxD_corner", // its name 
					  logic_WC_Active_WxD_plus,         // its mother volume
					  false,                     // no boolean os
					  1); 
	
	
	phys_WC_Active_WxD_blacksheet_plus =//put on the corner blacksheet
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0,(WC_MB_Fid_Width/2+WC_ActiveLayer_Depth-WCBlackSheetThickness/2),0),    // its position opposite side
					  logic_WC_BlacksheetWxD_corner,                // its logical volume
					  "LagicBlacksheet_WxD_corner", // its name 
					  logic_WC_Active_WxD_plus,         // its mother volume
					  false,                     // no boolean os
					  2); 																										
	
	
	new G4LogicalBorderSurface("WaterBS_LW_PolySurface",
							   phys_WC_Active_WxD_minus,phys_WC_Active_WxD_blacksheet_minus,
							   OpWaterBSSurface);
	new G4LogicalBorderSurface("WaterBS_LW_PolySurface",
							   phys_WC_Active_WxD_plus,phys_WC_Active_WxD_blacksheet_plus,
							   OpWaterBSSurface);																						
	
	
	G4VisAttributes* WC_BlacksheetWxDVisAtt = new G4VisAttributes(G4Colour(0.0,0.,1.));
	G4VisAttributes* WC_BlacksheetWxDVisAtt_corner = new G4VisAttributes(G4Colour(1.,0.0,1.));
	if(debugMode){
		logic_WC_BlacksheetWxD->SetVisAttributes(WC_BlacksheetWxDVisAtt);
		logic_WC_BlacksheetWxD_corner->SetVisAttributes(WC_BlacksheetWxDVisAtt_corner);
	}
	else {
		logic_WC_BlacksheetWxD->SetVisAttributes(G4VisAttributes::Invisible);
		logic_WC_BlacksheetWxD_corner->SetVisAttributes(G4VisAttributes::Invisible);
	}
	
	
	//At this stage we should have the bottom layer containing the PMT's and blacksheet defined
	
	//Now I will rotate this volume by +90degrees on Y axis
	const G4Transform3D rotation_WxD_minus = G4RotateY3D(pi/2.);				// minus rotates +90degrees around Y axis
	const G4Transform3D rotation_WxD_plus = G4RotateY3D(-pi/2.);				// plus rotates -90degrees around Y axis
	G4Translate3D WDTranslate_plus( (WC_MB_Fid_Length+WC_ActiveLayer_Depth)/2., 0., 0);		//for +x axis
	G4Translate3D WDTranslate_minus( -(WC_MB_Fid_Length+WC_ActiveLayer_Depth)/2.,0.,0.);	// for -x axis
	G4cout << "-x axis offset for translation" << -((WC_MB_Fid_Length+WC_ActiveLayer_Depth)/2.)/m << "m"<<"\n";
	
	
	//ok, put up the sides!
	// First side is on the negative axis--this requires rotation and translation for this face 
	
	new G4PVPlacement(WDTranslate_minus*rotation_WxD_minus,					// rotate from xy plane to xz plane 
					  logic_WC_Active_WxD_minus,				// its logical volume				  
					  "WD_minus",					// its name
					  logic_WC_MB_tank_H20,			// its mother  volume, the WC water tank
					  false,						// no boolean operations
					  2);							// copy number
	// Ok now the positive face.
	new G4PVPlacement(WDTranslate_plus*rotation_WxD_plus,					// rotate from xy plane to xz plane 
					  logic_WC_Active_WxD_plus,				// its logical volume				  
					  "WD_plus",					// its name
					  logic_WC_MB_tank_H20,			// its mother  volume, the WC water tank
					  false,						// no boolean operations
					  3);							// copy number 
	
	
	
	//Last two faces
	
	//WC_Active_LxD	(located on -y axis) I will build this as if it were in the x,y plane (like the LW above) , then rotate it +90degrees around the x axis to make the -y side				 
	
	
	G4Box* WC_Active_LxD = new G4Box("LxD",WC_MB_Fid_Length/2,WC_MB_Fid_Depth/2,WC_ActiveLayer_Depth/2);//active volume
	G4LogicalVolume* logic_WC_Active_LxD_minus = new G4LogicalVolume(WC_Active_LxD,G4Material::GetMaterial(water),"LxDActive_minus",0,0,0);
	G4LogicalVolume* logic_WC_Active_LxD_plus = new G4LogicalVolume(WC_Active_LxD,G4Material::GetMaterial(water),"LxDActive_plus",0,0,0);
	G4Box* WC_BlacksheetLxD = new G4Box("WC_BlackSheetLxD",WC_MB_Fid_Length/2,WC_MB_Fid_Depth/2,WCBlackSheetThickness/2.);//blacksheet material	

	//*********************************************replica*******************************
	//need to cutout center rectangle that will fit the replica pmtvolumes (already with blacksheet)   ***replica
	G4Box* WC_BlacksheetLxD_donuthole = new G4Box("WC_BlackSheetLxD_donuthole",WC_MB_NumPMT_L*WC_MB_PMT_Spacing/2, //***replica
												  WC_MB_NumPMT_D*WC_MB_PMT_Spacing/2,WCBlackSheetThickness/2.);				//***replica
	
	G4SubtractionSolid*  WC_BlacksheetLxD_donut= new G4SubtractionSolid("WC_BlacksheetLxD_donut",WC_BlacksheetLxD,WC_BlacksheetLxD_donuthole,0,G4ThreeVector(0.,0.,0.));	//this is now the blacksheet that will suround the PMT volumes  ***replica							
	//***replica
	
	G4LogicalVolume* logic_WC_BlacksheetLxD = new G4LogicalVolume(WC_BlacksheetLxD_donut,G4Material::GetMaterial("Blacksheet"),"LagicBlacksheet_LxD",0,0,0);				 //***replica														
	
	
	
	//*********************************************replica*******************************
	

	G4VPhysicalVolume* phys_WC_Active_LxD_plus, * phys_WC_ActiveGlass_LxD_plus;
	G4VPhysicalVolume* phys_WC_Active_LxD_minus, * phys_WC_ActiveGlass_LxD_minus;								  
	
	// Loop over the L,D dimensions to put ion PMT's
	
	icopy=0;
	xmin=WC_MB_Fid_Length, xmax=-WC_MB_Fid_Length, ymin=WC_MB_Fid_Depth, ymax=-WC_MB_Fid_Depth;

	//***replica
	// will build up pmt volume in two steps, one making a row of pmt volumes, then making a rectangle out of the rows. for this config, "D" runs on X axis, "W" is on Y axis
	
	
	G4Box * LxD_PMT_Row =	new G4Box("LxD_PMT_Row",WC_MB_NumPMT_L*WC_MB_PMT_Spacing/2,WC_MB_PMT_Spacing/2,WC_ActiveLayer_Depth/2.);
	G4LogicalVolume* logic_LxD_PMT_Row= new G4LogicalVolume(LxD_PMT_Row,G4Material::GetMaterial(water),"logic_LxD_PMT_Row",0,0,0);
	G4Box * LxD_PMT_Volume=new G4Box("LxD_PMT_Volume",WC_MB_NumPMT_L*WC_MB_PMT_Spacing/2,WC_MB_NumPMT_D*WC_MB_PMT_Spacing/2,WC_ActiveLayer_Depth/2);	//this is entire PMT volume
	G4LogicalVolume* logic_LxD_PMT_Volume= new G4LogicalVolume(LxD_PMT_Volume,G4Material::GetMaterial(water),"logic_LxD_PMT_Volume",0,0,0);
	
	G4VPhysicalVolume* physiWC_LxD_PMT_Row = 
	new G4PVReplica("WC_PMT_Row",
					logic_WC_PMT_Cell,
					logic_LxD_PMT_Row,
					kXAxis,
					WC_MB_NumPMT_L,
					WC_MB_PMT_Spacing,
					0.); 
	
	G4VPhysicalVolume* physiWC_LxD_PMT_Volume = 
	new G4PVReplica("WC_PMT_Volume",
					logic_LxD_PMT_Row,
					logic_LxD_PMT_Volume,
					kYAxis,
					WC_MB_NumPMT_D,
					WC_MB_PMT_Spacing,
					0.); 
	
	//Now we should have PMT's placed in the PMT "Donut_Hole"
	
	//Now put into face volumes
	phys_WC_Active_LxD_minus =					// put PMT's into bottom face--eventually it will be translated to bottom face
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0.,0.,0.),                   // its position
					  logic_LxD_PMT_Volume,                // its logical volume
					  //"LxD_PMT_minus", // its name
					  "WCPMT",// (JF) replaced above  
					  logic_WC_Active_LxD_minus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // every PMT needs an unique id in this physical volume		??? what to do here!??
	
	
	phys_WC_Active_LxD_plus =					// put PMT's into bottom face--eventually it will be translated to bottom face
	new G4PVPlacement(0,                         // no rotation
					  G4ThreeVector(0.,0.,0.),                   // its position
					  logic_LxD_PMT_Volume,                // its logical volume
					  //"LxD_PMT_minus", // its name
					  "WCPMT",// (JF) replaced above  
					  logic_WC_Active_LxD_plus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // every PMT needs an unique id in this physical volume		??? what to do here!??
	
	
	
	if (!debugMode)	{
		logic_LxD_PMT_Row->SetVisAttributes(G4VisAttributes::Invisible);//*replica
		logic_LxD_PMT_Volume->SetVisAttributes(G4VisAttributes::Invisible);//*replica
	}
	
	//end ****replica	
	
	
	//				G4cout << "halflength, xmin, xmax= " << WC_MB_Fid_Length/2<<" "<<xmin<<" "<<xmax << "\n";
	//				G4cout << "halfwidth, ymin, ymax=" << WC_MB_Fid_Depth/2<<" "<<ymin<<" "<<ymax << "\n";
	//				G4cout << "\n";
	G4cout << "total on LxD Surface: " << 2* WC_MB_NumPMT_L* WC_MB_NumPMT_D << "\n";
	G4cout << "Coverage was calculated to be: " << (WC_MB_NumPMT_L* WC_MB_NumPMT_D*WCPMT_crossarea/(WC_MB_Fid_Length*WC_MB_Fid_Depth)) << "\n";
	num_pmt+=2* WC_MB_NumPMT_L* WC_MB_NumPMT_D;
	G4cout << "total number of PMT: " << num_pmt << "\n";	
	
	//add blacksheet
	
	G4ThreeVector position_blacksheet_LxD =G4ThreeVector(0,0,(-WC_ActiveLayer_Depth+WCBlackSheetThickness)/2) ;//hopefully this puts blacksheet on bottom of volume.
	
	G4VPhysicalVolume*	phys_WC_Active_LxD_blacksheet_minus =
	new G4PVPlacement(0,                         // no rotation
					  position_blacksheet_LxD,    // its position
					  logic_WC_BlacksheetLxD,                // its logical volume
					  "LD_Blacksheet", // its name 
					  logic_WC_Active_LxD_minus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // copy #.
	
	G4VPhysicalVolume*	phys_WC_Active_LxD_blacksheet_plus =
	new G4PVPlacement(0,                         // no rotation
					  position_blacksheet_LxD,    // its position
					  logic_WC_BlacksheetLxD,                // its logical volume
					  "LD_Blacksheet", // its name 
					  logic_WC_Active_LxD_plus,         // its mother volume
					  false,                     // no boolean os
					  0);                        // copy #.
	
	
	//Activate the water/blacksheet surfaces in both faces
	new G4LogicalBorderSurface("WaterBS_LW_PolySurface",
							   phys_WC_Active_LxD_minus,phys_WC_Active_LxD_blacksheet_minus,
							   OpWaterBSSurface);
	new G4LogicalBorderSurface("WaterBS_LW_PolySurface",
							   phys_WC_Active_LxD_plus,phys_WC_Active_LxD_blacksheet_plus,
							   OpWaterBSSurface);																						
	
	
	G4VisAttributes* WC_BlacksheetLxDVisAtt = new G4VisAttributes(G4Colour(0.0,0.,1.));
	if(debugMode)
		logic_WC_BlacksheetLxD->SetVisAttributes(WC_BlacksheetLxDVisAtt);
	else
		logic_WC_BlacksheetLxD->SetVisAttributes(G4VisAttributes::Invisible);
	
	
	
	
	
	//At this stage we should have the bottom layer containing the PMT's and blacksheet defined
	
	//Now I will rotate this volume by -90degrees on X axis to make minus plane (PMT's looking inward on -Y axis)
	const G4Transform3D rotation_LxD_minus = G4RotateX3D(-pi/2.);
	const G4Transform3D rotation_LxD_plus = G4RotateX3D(+pi/2.);
	
	
	
	//Now I will make the translation/reflection which will place theplus and minus layers on the +- y sides of the fiducial detector.
	
	G4Translate3D LDTranslate_minus(0., -(WC_MB_Fid_Width+WC_ActiveLayer_Depth)/2.,.0);		// for -y axis
	G4Translate3D LDTranslate_plus(0.,(WC_MB_Fid_Width+WC_ActiveLayer_Depth)/2, 0.);				//put on +y axis
	
	//ok, put up the sides!	
	// First side is on the negative axis--this just requires rotation and translation
	new G4PVPlacement(LDTranslate_minus*rotation_LxD_minus,    // rotate from xy plane to xz plane and put on -y axis--the PMT's should be looking inward
					  logic_WC_Active_LxD_minus,     // its logical volume				  
					  "LD_minus",        // its name
					  logic_WC_MB_tank_H20,      // its mother  volume, the WC water tank
					  false,           // no boolean operations
					  4);              // copy number 
	//Second side is on + axis
	new G4PVPlacement(LDTranslate_plus*rotation_LxD_plus,    // rotate from xy plane to xz plane and put on -y axis--the PMT's should be looking inward
					  logic_WC_Active_LxD_plus,     // its logical volume				  
					  "LD_plus",        // its name
					  logic_WC_MB_tank_H20,      // its mother  volume, the WC water tank
					  false,           // no boolean operations
					  5);              // copy number 											
	
	
	
	
	
	
	// Make sensitive detectors in the six faces
	
	G4SDManager* SDman = G4SDManager::GetSDMpointer();
	
	if (!aWCPMT) 
	{
	  aWCPMT = new WCSimWCSD( "/WCSim/glassFaceWCPMT", this );
		SDman->AddNewDetector( aWCPMT );
		G4cout << "Got to sensitive detector code \n";
	}
	logicGlassFaceWCPMT->SetSensitiveDetector( aWCPMT );
	
	
	//Put Fiducial into WaterTank, and WaterTank into "Cavern"
	new G4PVPlacement(0,G4ThreeVector(),logic_WC_MB_Fiducial,"physiMB_Fiducial",logic_WC_MB_tank_H20,false,0,false);// this should include now the fiducial volume and the pmt volumes around it.	
	
	new G4PVPlacement(0,G4ThreeVector(),logic_WC_MB_tank_H20,"physiMBTank",logic_WC_Cavern,false,0,false);
	
	
	//return the logical volume of the Cavern	
	
	return logic_WC_Cavern;   //whew!
}
//aah

G4LogicalVolume* WCSimDetectorConstruction::ConstructWC()
{

  //-----------------------------------------------------
  // Positions
  //-----------------------------------------------------

  debugMode = false;
  
  WCPosition=0.;//Set the WC tube offset to zero

  G4double WCIDRadius = WCIDDiameter/2.;
  // the number of regular cell in phi direction:
  WCBarrelRingNPhi     = (G4int)(WCBarrelNumPMTHorizontal/WCPMTperCellHorizontal); 
  // the part of one ring, that is covered by the regular cells: 
  G4double totalAngle  = 2.0*pi*rad*(WCBarrelRingNPhi*WCPMTperCellHorizontal/WCBarrelNumPMTHorizontal) ;
  // angle per regular cell:
  G4double dPhi        =  totalAngle/ WCBarrelRingNPhi;
  // it's hight:
  G4double barrelCellHeight  = (WCIDHeight-2.*(WCBarrelPMTOffset+WCPMTRadiusEff+1.*mm))/WCBarrelNRings;
  // the hight of all regular cells together:
  G4double mainAnnulusHeight = WCIDHeight -2.*(WCBarrelPMTOffset+WCPMTRadiusEff+1.*mm) -2.*barrelCellHeight;
  
  
  G4double innerAnnulusRadius = WCIDRadius -1.*mm - WCPMTRadiusEff;//- WCPMTExposeHeight-1.*mm-11.8*cm; 
  G4double outerAnnulusRadius = WCIDRadius + WCBlackSheetThickness + 1.*mm;//+ Stealstructure etc.
  // the radii are measured to the center of the surfaces
  // (tangent distance). Thus distances between the corner and the center are bigger.
  WCLength    = WCIDHeight + 2*2.3*m;	//jl145 - reflects top veto blueprint, cf. Farshid Feyzi
  G4double WCRadius    = (WCIDDiameter/2. + WCBlackSheetThickness + 1.5*m)/cos(dPhi/2.) ; // TODO: OD 
 
  // now we know the extend of the detector and are able to tune the tolerance
  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(WCLength > WCRadius ? WCLength : WCRadius);
  G4cout << "Computed tolerance = "
         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
         << " mm" << G4endl;


//---------------------------------------------
  //The light collector
  //---------------------------------------------
  // if(addLC && DaddLC) ConstructLC(); //adding Winston Cones
//   if(addWLSP && DaddWLSP) ConstructWLSP(); //adding WLS Plates

  ConstructLC(); //adding Winston Cones
  ConstructWLSP(); //adding WLS Plates  
 //--------------------------------------------

  //Decide if adding Gd
  G4String water = "Water";
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

// if(!debugMode)
//    logicWCBarrel->SetVisAttributes(G4VisAttributes::Invisible); 

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
  //----------------------------------------------------
  // extra rings for the top and bottom of the annulus
  //---------------------------------------------------
  G4double borderAnnulusZ[3] = {-barrelCellHeight/2.,
                                -barrelCellHeight/2.,//+(WCIDRadius-innerAnnulusRadius),
				barrelCellHeight/2.};
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
                  G4ThreeVector(0.,0.,(-mainAnnulusHeight/2.- barrelCellHeight/2.)),
                  logicWCBarrelBorderRing,
                  "WCBarrelBorderRing",
                  logicWCBarrel,
                  false, 0,true);


                  
  if(!debugMode) 
    logicWCBarrelBorderRing->SetVisAttributes(G4VisAttributes::Invisible); 
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
  logicWCBarrelRing->SetVisAttributes(G4VisAttributes::Invisible);
else {
        G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(0,0.5,1.));
        tmpVisAtt->SetForceWireframe(true);
        logicWCBarrelRing->SetVisAttributes(tmpVisAtt);
  }

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
  if(!debugMode)
        logicWCBarrelBorderCell->SetVisAttributes(G4VisAttributes::Invisible);
  else {
        G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(1.,0.5,0.5));
        tmpVisAtt->SetForceWireframe(true);
        logicWCBarrelBorderCell->SetVisAttributes(tmpVisAtt);
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
  	logicWCBarrelCell->SetVisAttributes(G4VisAttributes::Invisible);
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
                      0,true);

  G4LogicalBorderSurface * WaterBSBarrelCellSurface 
    = new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
                                 physiWCBarrelCell,
                                 physiWCBarrelCellBlackSheet, 
                                 OpWaterBSSurface);

 
   G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
      = new G4VisAttributes(G4Colour(0.2,0.9,0.2));
      if(debugMode)
        logicWCBarrelCellBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
      else
        logicWCBarrelCellBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);


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
 //-----------------------------------------------------------
 // add extra tower if nessecary
 // ---------------------------------------------------------
 
  // we have to declare the logical Volumes 
  // outside of the if block to access it later on 
  G4LogicalVolume* logicWCExtraTowerCell;
  G4LogicalVolume* logicWCExtraBorderCell;
  G4VPhysicalVolume* physiWCTowerCell;

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
    //    G4VPhysicalVolume* physiWCTowerCell = 
physiWCTowerCell = 
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
			0,true);

    G4LogicalBorderSurface * WaterBSTowerCellSurface 
      = new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
				   physiWCTowerCell,
				   physiWCTowerBlackSheet, 
				   OpWaterBSSurface);

   
	if(debugMode)
	  logicWCTowerBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
	else
	  logicWCTowerBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);
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

    G4VPhysicalVolume* physiWCExtraBorderCell =
      new G4PVPlacement(0,
                  G4ThreeVector(0.,0.,(-mainAnnulusHeight/2.- barrelCellHeight/2.)),
                  logicWCExtraBorderCell,
                  "WCExtraTowerBorderCell",
                  logicWCBarrel,
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
 
  G4double capZ[4] = { -WCBlackSheetThickness-1.*mm,
                      WCBarrelPMTOffset,
		      WCBarrelPMTOffset,
		      WCBarrelPMTOffset+(WCIDRadius-innerAnnulusRadius)} ;
  G4double capRmin[4] = {  0. , 0., 0., 0.} ;
  //  G4double capRmax[4] = {outerAnnulusRadius, outerAnnulusRadius,  WCIDRadius, innerAnnulusRadius};
  G4double capRmax[4] = {outerAnnulusRadius, outerAnnulusRadius, outerAnnulusRadius, outerAnnulusRadius};
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
		      G4ThreeVector(0.,0.,-WCIDHeight/2.),     // its position
		      logicWCCap,          // its logical volume
		      "WCCap",             // its name
		      logicWCBarrel,                  // its mother volume
		      false,                       // no boolean operations
		      0,true);                          // Copy #


 
  if(debugMode){  
    G4VisAttributes* tmpVisAtt2 = new G4VisAttributes(G4Colour(.6,0.5,0.5));
   tmpVisAtt2->SetForceWireframe(true);
    logicWCCap->SetVisAttributes(tmpVisAtt2);
  } else
    logicWCCap->SetVisAttributes(G4VisAttributes::Invisible);


  //---------------------------------------------------------------------
  // add cap blacksheet
  // -------------------------------------------------------------------
  
  G4double capBlackSheetZ[4] = {-WCBlackSheetThickness, 0., 0., WCBarrelPMTOffset};
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
    if(debugMode)
        logicWCCapBlackSheet->SetVisAttributes(WCCapBlackSheetVisAtt);
    else
        logicWCCapBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);


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

	  G4LogicalBorderSurface * WaterTyTVSurfaceBot =
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
				 				false,0,true);

	  G4LogicalBorderSurface * WaterTyTVSurfaceTop =
			new G4LogicalBorderSurface(	"WaterTyTVSurfaceTop",
										physiWCTopVeto,
										physiWCTVTyvekTop,
										OpWaterTySurface);

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

	  G4LogicalBorderSurface * WaterTyTVSurfaceSide =
			new G4LogicalBorderSurface(	"WaterTyTVSurfaceSide",
										physiWCTopVeto,
										physiWCTVTyvekSide,
										OpWaterTySurface);

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
    //from any specific detector geometry so that both the Cylindrical and 
    //Mailbox geometries
  //  can use the same definition. 
  //K.Zbiri: The PMT volume and the PMT glass are now put in parallel. 
    //The PMT glass is the sensitive volume in this new configuration.

  ConstructPMT();

  
  //---------------------------------------------------------
  // Add top and bottom PMTs
  // -----------------------------------------------------
  
  G4double xoffset;
  G4double yoffset;
  G4int    icopy = 0;

  // loop over the cap
  G4int CapNCell = WCCapEdgeLimit/WCCapPMTSpacing + 2;
  for ( int i = -CapNCell ; i <  CapNCell; i++) {
    for (int j = -CapNCell ; j <  CapNCell; j++)   {

       
      xoffset = i*WCCapPMTSpacing + WCCapPMTSpacing*0.5;
      yoffset = j*WCCapPMTSpacing + WCCapPMTSpacing*0.5;

      
      G4ThreeVector cellpos = G4ThreeVector(xoffset, yoffset, -1.0*PMTOffset);     
      //      G4double WCBarrelEffRadius = WCIDDiameter/2. - WCCapPMTSpacing;
      //      double comp = xoffset*xoffset + yoffset*yoffset 
      //	- 2.0 * WCBarrelEffRadius * sqrt(xoffset*xoffset+yoffset*yoffset)
      //	+ WCBarrelEffRadius*WCBarrelEffRadius;
      //      if ( (comp > WCPMTRadius*WCPMTRadius) && ((sqrt(xoffset*xoffset + yoffset*yoffset) + WCPMTRadius) < WCCapEdgeLimit) ) {
            if (((sqrt(xoffset*xoffset + yoffset*yoffset) + WCPMTRadius) < WCCapEdgeLimit) ) {

 if(addLC && DaddLC) {

	  G4ThreeVector LCpos = G4ThreeVector(xoffset, yoffset, offset);
	  G4VPhysicalVolume* physiCapLC=
	    new G4PVPlacement(0,                         
			      LCpos,                   
			      logiLC,         
			      "WCCapLC", 
			      logicWCCap,         
			      false,                 
			      icopy
			      );            
 }

 if(addWLSP && DaddWLSP) {

   G4ThreeVector WLSPCone_pos = G4ThreeVector(xoffset, yoffset, WLSP_offset);
   G4ThreeVector WLSPRefl_pos = G4ThreeVector(xoffset, yoffset, WLSP_offset);
 
G4VPhysicalVolume* physiCapWLSPRefl=
	    new G4PVPlacement(0,                         
			      WLSPRefl_pos,                   
			      logiWLSPReflector,         
			      "WCCapWLSPRef", 
			      logicWCCap,         
			      false,                 
			      icopy
			      );      
 
 // G4cout<<"logiWLSPReflector Material: "<< logiWLSPReflector->GetMaterial()->GetName()  <<G4endl;


G4VPhysicalVolume* physiCapWLSPCone=
	    new G4PVPlacement(0,                         
			      WLSPCone_pos,                   
			      logiWLSPLC,         
			      "WCCapWLSPLC", 
			      logicWCCap,         
			      false,                 
			      icopy
			      );   
 G4LogicalBorderSurface* CapWLSPPlasticSurface = new G4LogicalBorderSurface("WLSP_PlasticSurface",
							    //  physiCapWLSPCone,physiWCCap,
							    physiCapWLSPCone,physiWCBarrel,
 							    WCSimWLSP_LC::OpPlasticSurface);   
 //  G4cout<<"logiWLSPLC Material: "<< logiWLSPLC->GetMaterial()->GetName()  <<G4endl;

  
 }
if (addWLS){
	  G4VPhysicalVolume* physiCapPMTWLS =
	  new G4PVPlacement(0,                         // no rotation
			    cellpos,                   // its position
			    logicWLSFaceWCPMT,         // its logical volume
			    "WCPMTWLS", // its name 
			    logicWCCap,         // its mother volume
			    false,                 // no boolean os
			    icopy );          // every PMT need a unique id.
			    //  , true);
	}

	G4VPhysicalVolume* physiCapPMTGlass =
	  new G4PVPlacement(0,                         // no rotation
			    cellpos,                   // its position
			    logicGlassFaceWCPMT,         // its logical volume
			    "WCPMTGlass", // its name 
			    logicWCCap,         // its mother volume
			    false,                 // no boolean os
			    icopy );          // every PMT need a unique id.
			    //   , true);

	G4VPhysicalVolume* physiCapPMT =
	  new G4PVPlacement(0,                         // no rotation
			    cellpos,                   // its position
			    logicWCPMT,                // its logical volume
			    "WCPMT", // its name 
			    logicWCCap,         // its mother volume
			    false,                 // no boolean os
			    icopy );          // every PMT need a unique id.
			    // , true);
	
	G4LogicalBorderSurface *  GlassCathodeCapSurface
	  = new G4LogicalBorderSurface("GlassCathodeSurface",
				  physiCapPMTGlass,physiCapPMT,
				   OpGlassCathodeSurface); 
 // logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
    // daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
	// this is still the case.

	icopy++;
      }
    }
  }

  G4cout << "total on cap: " << icopy << "\n";
  G4cout << "Coverage was calculated to be: " << (icopy*WCPMTRadius*WCPMTRadius/(WCIDRadius*WCIDRadius)) << "\n";


  //jl145------------------------------------------------
  // Add top veto PMTs
  //-----------------------------------------------------

  if(WCTopVeto){

	  G4double WCTVPMTSpacing = (WCSimTuningParams->GetTVSpacing())*cm;
	  G4double WCTVEdgeLimit = WCCapEdgeLimit;
	  G4int TVNCell = WCTVEdgeLimit/WCTVPMTSpacing + 2;

	  icopy = 0;

	  for ( int i = -TVNCell ; i <  TVNCell; i++) {
		for (int j = -TVNCell ; j <  TVNCell; j++)   {

		  xoffset = i*WCTVPMTSpacing + WCTVPMTSpacing*0.5;
		  yoffset = j*WCTVPMTSpacing + WCTVPMTSpacing*0.5;

		  G4ThreeVector cellpos =
		  		G4ThreeVector(	xoffset, yoffset,
								-1.0*PMTOffset-0.5*m);

		  if ((sqrt(xoffset*xoffset + yoffset*yoffset) + WCPMTRadius) < WCTVEdgeLimit) {

		    G4VPhysicalVolume* physiCapPMTGlass =
		    		new G4PVPlacement(	0,						// no rotation
		    							cellpos,				// its position
		    							logicGlassFaceWCPMT,	// its logical volume
		    							"WCPMTGlass",			// its name 
		    							logicWCTopVeto,			// its mother volume
		    							false,					// no boolean os
		    							icopy);					// every PMT need a unique id.

		    G4VPhysicalVolume* physiCapPMT =
		    		new G4PVPlacement(	0,						// no rotation
		    							cellpos,				// its position
		    							logicWCPMT,				// its logical volume
		    							"WCPMT",				// its name 
		    							logicWCTopVeto,			// its mother volume
		    							false,					// no boolean os
		    							icopy);					// every PMT need a unique id.

		    G4LogicalBorderSurface* GlassCathodeCapSurface =
		    		new G4LogicalBorderSurface("GlassCathodeSurface",
		    									physiCapPMTGlass,physiCapPMT,
		    									OpGlassCathodeSurface);
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
      G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius+PMTOffset,
						 -barrelCellWidth/2.+(i+0.5)*horizontalSpacing,
						 -barrelCellHeight/2.+(j+0.5)*verticalSpacing);

   G4ThreeVector LCPosition;
   G4ThreeVector WLSPCone_position;
   G4ThreeVector WLSPRefl_position;

   if(addLC && DaddLC) {	
      LCPosition =  G4ThreeVector(WCIDRadius-offset,
				  -barrelCellWidth/2.+(i+0.5)*horizontalSpacing,
				  -barrelCellHeight/2.+(j+0.5)*verticalSpacing); 
        
	G4VPhysicalVolume* physiWCBarrelLC=
	  new G4PVPlacement(WCPMTRotation,                      // its rotation
			   LCPosition, 
			    logiLC,            // its logical volume
			    "WCLC",            // its name
			    logicWCBarrelCell,         // its mother volume
			    false,                     // no boolean operations
			    (int)(i*WCPMTperCellVertical+j),
			    true); 
      }

 if(addWLSP && DaddWLSP) {

    WLSPCone_position = G4ThreeVector(WCIDRadius-WLSP_offset,
				  -barrelCellWidth/2.+(i+0.5)*horizontalSpacing,
				  -barrelCellHeight/2.+(j+0.5)*verticalSpacing); 

    WLSPRefl_position = G4ThreeVector(WCIDRadius-WLSP_offset,
				  -barrelCellWidth/2.+(i+0.5)*horizontalSpacing,
				  -barrelCellHeight/2.+(j+0.5)*verticalSpacing); 

 G4VPhysicalVolume* physiWCBarrelWLSPRefl=
	    new G4PVPlacement(WCPMTRotation,                         
			      WLSPRefl_position,                   
			      logiWLSPReflector,         
			      "WCWLSPRef", 
			      logicWCBarrelCell,         
			      false,                 
			      (int)(i*WCPMTperCellVertical+j),
			      true);      
  
G4VPhysicalVolume* physiWCBarrelWLSPCone=
	    new G4PVPlacement(WCPMTRotation,                         
			      WLSPCone_position,                   
			      logiWLSPLC,         
			      "WCWLSPLC", 
			      logicWCBarrelCell,         
			      false,                 
			      (int)(i*WCPMTperCellVertical+j),
			       true);      
 G4LogicalBorderSurface* BarrelWLSPPlasticSurface = new G4LogicalBorderSurface("WLSP_PlasticSurface",
 							    physiWCBarrelWLSPCone,physiWCBarrelCell,
 							    WCSimWLSP_LC::OpPlasticSurface);   
  
  }
  
  if (addWLS){
	G4VPhysicalVolume* physiWCBarrelPMTWLS =
	new G4PVPlacement(WCPMTRotation,                      // its rotation
			  PMTPosition, 
			  logicWLSFaceWCPMT,            // its logical volume
			  "WCPMTWLS",            // its name
			  logicWCBarrelCell,         // its mother volume
			  false,                     // no boolean operations
			  (int)(i*WCPMTperCellVertical+j),
			  true);              
      }

     G4VPhysicalVolume* physiWCBarrelPMTGlass =
	new G4PVPlacement(WCPMTRotation,                      // its rotation
			  PMTPosition, 
			  logicGlassFaceWCPMT,            // its logical volume
			  "WCPMTGlass",            // its name
			  logicWCBarrelCell,         // its mother volume
			  false,                     // no boolean operations
			  (int)(i*WCPMTperCellVertical+j),
			  true);                       
			                         
  
      G4VPhysicalVolume* physiWCBarrelPMT =
	new G4PVPlacement(WCPMTRotation,              // its rotation
			  PMTPosition, 
			  logicWCPMT,                // its logical volume
			  "WCPMT",             // its name
			  logicWCBarrelCell,         // its mother volume
			  false,                     // no boolean operations
			  (int)(i*WCPMTperCellVertical+j),
			  true);                       

      G4LogicalBorderSurface *  GlassCathodeBarrelSurface
	  = new G4LogicalBorderSurface("GlassCathodeSurface",
				      physiWCBarrelPMTGlass,physiWCBarrelPMT,
				       OpGlassCathodeSurface);
      
if(addLC && DaddLC) {
		 LCPosition =  G4ThreeVector(WCIDRadius-offset,
					   -barrelCellWidth/2.+(i+0.5)*horizontalSpacing,
					   -barrelCellHeight/2.+(j+0.5)*verticalSpacing);
  
	G4VPhysicalVolume* physiWCBarrelBorderLC=
	  new G4PVPlacement(WCPMTRotation,                      // its rotation
			   LCPosition, 
			    logiLC,            // its logical volume
			    "WCLC",            // its name
			   logicWCBarrelBorderCell ,         // its mother volume
			    false,                     // no boolean operations
			    (int)(i*WCPMTperCellVertical+j+WCPMTperCellVertical*WCPMTperCellVertical),
			    true); 
      }

if(addWLSP && DaddWLSP) {

    WLSPCone_position = G4ThreeVector(WCIDRadius-WLSP_offset,
				  -barrelCellWidth/2.+(i+0.5)*horizontalSpacing,
				  -barrelCellHeight/2.+(j+0.5)*verticalSpacing); 

    WLSPRefl_position = G4ThreeVector(WCIDRadius-WLSP_offset,
				  -barrelCellWidth/2.+(i+0.5)*horizontalSpacing,
				  -barrelCellHeight/2.+(j+0.5)*verticalSpacing); 

G4VPhysicalVolume* physiWCBarrelBorderWLSPRefl=
	    new G4PVPlacement(WCPMTRotation,                         
			      WLSPRefl_position,                   
			      logiWLSPReflector,         
			      "WCWLSPRef", 
			      logicWCBarrelBorderCell,         
			      false,                 
			      (int)(i*WCPMTperCellVertical+j+WCPMTperCellVertical*WCPMTperCellVertical),
			      true);      
  
 G4VPhysicalVolume* physiWCBarrelBorderWLSPCone=
	    new G4PVPlacement(WCPMTRotation,                         
			      WLSPCone_position,                   
			      logiWLSPLC,         
			      "WCWLSPLC", 
			      logicWCBarrelBorderCell,         
			      false,                 
			      (int)(i*WCPMTperCellVertical+j+WCPMTperCellVertical*WCPMTperCellVertical),
			       true);      
G4LogicalBorderSurface* BarrelBorderWLSPPlasticSurface = new G4LogicalBorderSurface("WLSP_PlasticSurface",
 							    physiWCBarrelBorderWLSPCone,physiWCBarrelBorderCell,
 							    WCSimWLSP_LC::OpPlasticSurface);   
  
 
  }
  
  if (addWLS){
	G4VPhysicalVolume* physiWCBarrelBorderPMTWLS =
	new G4PVPlacement(WCPMTRotation,                        // its rotation
			  PMTPosition,
			  logicWLSFaceWCPMT,            // its logical volume
			  "WCPMTWLS",             // its name
			  logicWCBarrelBorderCell,        // its mother volume
			  false,                     // no boolean operations
			  (int)(i*WCPMTperCellVertical+j+WCPMTperCellVertical*WCPMTperCellVertical)
			  ,true);                     // no particular field
      }

 G4VPhysicalVolume* physiWCBarrelBorderPMTGlass =
	new G4PVPlacement(WCPMTRotation,                        // its rotation
			  PMTPosition,
			  logicGlassFaceWCPMT,            // its logical volume
			  "WCPMTGlass",             // its name
			  logicWCBarrelBorderCell,        // its mother volume
			  false,                     // no boolean operations
			  (int)(i*WCPMTperCellVertical+j+WCPMTperCellVertical*WCPMTperCellVertical)
			  ,true);                     // no particular field

     G4VPhysicalVolume* physiWCBarrelBorderPMT =
	new G4PVPlacement(WCPMTRotation,                      // its rotation
			  PMTPosition,
			  logicWCPMT,                // its logical volume
			  "WCPMT",             // its name
			  logicWCBarrelBorderCell,         // its mother volume
			  false,                     // no boolean operations
			  (int)(i*WCPMTperCellVertical+j+WCPMTperCellVertical*WCPMTperCellVertical)
			  ,true);                      // no particular field

	G4LogicalBorderSurface *  GlassCathodeBarrelBorderSurface
	    = new G4LogicalBorderSurface("GlassCathodeBarrelSurface",
			 physiWCBarrelBorderPMTGlass,physiWCBarrelBorderPMT,
					 OpGlassCathodeSurface);
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
	G4ThreeVector PMTPosition =  G4ThreeVector(WCIDRadius/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.)+PMTOffset,
				towerWidth/2.-(i+0.5)*horizontalSpacing,
			       -barrelCellHeight/2.+(j+0.5)*verticalSpacing);
	PMTPosition.rotateZ(-(2*pi-totalAngle)/2.); // align with the symmetry 
	                                            //axes of the cell 

	G4ThreeVector LCPosition ;
	G4ThreeVector WLSPCone_position;
	G4ThreeVector WLSPRefl_position;

	if(addLC && DaddLC) {  
	    LCPosition =  G4ThreeVector(WCIDRadius/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.)-offset,
					towerWidth/2.-(i+0.5)*horizontalSpacing,
					-barrelCellHeight/2.+(j+0.5)*verticalSpacing);

	    LCPosition.rotateZ(-(2*pi-totalAngle)/2.); 
     
	G4VPhysicalVolume* physiWCBarrelLC=
	  new G4PVPlacement(WCPMTRotation,                      // its rotation
			   LCPosition, 
			    logiLC,            // its logical volume
			    "WCLC",            // its name
			   logicWCExtraTowerCell,         // its mother volume
			    false,                     // no boolean operations
			    (int)(i*WCPMTperCellVertical+j+50),
			    true); 
      }

if(addWLSP && DaddWLSP) {

    WLSPCone_position =  G4ThreeVector(WCIDRadius/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.)-WLSP_offset,
					towerWidth/2.-(i+0.5)*horizontalSpacing,
					-barrelCellHeight/2.+(j+0.5)*verticalSpacing);

    WLSPRefl_position =  G4ThreeVector(WCIDRadius/cos(dPhi/2.)*cos((2.*pi-totalAngle)/2.)-WLSP_offset,
					towerWidth/2.-(i+0.5)*horizontalSpacing,
					-barrelCellHeight/2.+(j+0.5)*verticalSpacing);

    WLSPCone_position.rotateZ(-(2*pi-totalAngle)/2.); 
    WLSPRefl_position.rotateZ(-(2*pi-totalAngle)/2.); 

G4VPhysicalVolume* physiWCBarrelWLSPRefl=
	    new G4PVPlacement(WCPMTRotation,                         
			      WLSPRefl_position,                   
			      logiWLSPReflector,         
			      "WCWLSPRef", 
			      logicWCExtraTowerCell,         
			      false,                 
			      (int)(i*WCPMTperCellVertical+j+50),
			      true);      
  
 G4VPhysicalVolume* physiWCBarrelWLSPCone=
	    new G4PVPlacement(WCPMTRotation,                         
			      WLSPCone_position,                   
			      logiWLSPLC,         
			      "WCWLSPLC", 
			      logicWCExtraTowerCell,         
			      false,                 
			      (int)(i*WCPMTperCellVertical+j+50),
			       true);      
G4LogicalBorderSurface* BarrelWLSPPlasticSurface = new G4LogicalBorderSurface("WLSP_PlasticSurface",
 							    physiWCBarrelWLSPCone,physiWCTowerCell,
 							    WCSimWLSP_LC::OpPlasticSurface);   
 
  
  }
  
	if (addWLS){
	  G4VPhysicalVolume* physiWCBarrelPMTWLS =
	  new G4PVPlacement(WCPMTRotation,             // its rotation
			    PMTPosition, 
			    logicWLSFaceWCPMT,         // its logical volume
			    "WCPMTWLS",             // its name
			    logicWCExtraTowerCell,         // its mother volume
			    false,                     // no boolean operations
			    (int)(i*WCPMTperCellVertical+j+50), //TODO: the 
			                              //+50 is to have 
			                             //an unique id for each 
			                             //PMT. You should this 
			                             //by a
						    // variable to make sure, 
			                            //that the value is
						    // big enough even 
			                            //for gigantic cells.
			    true);                       
	}


	G4VPhysicalVolume* physiWCBarrelPMTGlass =
	  new G4PVPlacement(WCPMTRotation,             // its rotation
			    PMTPosition, 
			    logicGlassFaceWCPMT,         // its logical volume
			    "WCPMTGlass",             // its name
			    logicWCExtraTowerCell,         // its mother volume
			    false,                     // no boolean operations
			    (int)(i*WCPMTperCellVertical+j+50), //TODO: the 
			                              //+50 is to have 
			                             //an unique id for each 
			                             //PMT. You should this 
			                             //by a
						    // variable to make sure, 
			                            //that the value is
						    // big enough even 
			                            //for gigantic cells.
			    true);                       

	G4VPhysicalVolume* physiWCBarrelPMT =
	  new G4PVPlacement(WCPMTRotation,             // its rotation
			    PMTPosition, 
			    logicWCPMT,                // its logical volume
			    "WCPMT",             // its name
			    logicWCExtraTowerCell,         // its mother volume
			    false,                     // no boolean operations
			    (int)(i*WCPMTperCellVertical+j+50), //TODO: the +50 
			                                         //is to have 
			                                         //an unique 
			                                        // id for each 
			                                        //PMT. You 
			                                       //should this by a
								// variable to 
			                                       //make sure, 
			                                      //that the value is
							     // big enough even 
			                                   //for gigantic cells.
			    true);                       

	G4LogicalBorderSurface *  GlassCathodeBarrelSurface
	    = new G4LogicalBorderSurface("GlassCathodeSurface",
					 physiWCBarrelPMTGlass,physiWCBarrelPMT,
					 OpGlassCathodeSurface);
if(addLC && DaddLC) {

  	G4VPhysicalVolume* physiWCBarrelBorderLC=
	  new G4PVPlacement(WCPMTRotation,                      // its rotation
			   LCPosition, 
			    logiLC,            // its logical volume
			    "WCLC",            // its name
			   logicWCExtraTowerCell,         // its mother volume
			    false,                     // no boolean operations
			    (int)(i*WCPMTperCellVertical+j+100),
			    true); 
      }	

if(addWLSP && DaddWLSP) {

 G4VPhysicalVolume* physiWCBarrelBorderWLSPRefl=
	    new G4PVPlacement(WCPMTRotation,                         
			      WLSPRefl_position,                   
			      logiWLSPReflector,         
			      "WCWLSPRef", 
			      logicWCExtraTowerCell,         
			      false,                 
			      (int)(i*WCPMTperCellVertical+j+100),
			      true);      
  
G4VPhysicalVolume* physiWCBarrelBorderWLSPCone=
	    new G4PVPlacement(WCPMTRotation,                         
			      WLSPCone_position,                   
			      logiWLSPLC,         
			      "WCWLSPLC", 
			      logicWCExtraTowerCell,         
			      false,                 
			      (int)(i*WCPMTperCellVertical+j+100),
			       true);      
  G4LogicalBorderSurface* BarrelBorderWLSPPlasticSurface = new G4LogicalBorderSurface("WLSP_PlasticSurface",
 							    physiWCBarrelBorderWLSPCone,physiWCTowerCell,
 							    WCSimWLSP_LC::OpPlasticSurface);   
 

  }
  
	
if (addWLS){
	  G4VPhysicalVolume* physiWCBarrelBorderPMTWLS =
	  new G4PVPlacement(WCPMTRotation,                          // its rotation
			    PMTPosition,
			    logicWLSFaceWCPMT,                // its logical volume
			    "WCPMTWLS",             // its name
			    logicWCExtraBorderCell,         // its mother volume
			    false,                     // no boolean operations
			    (int)(i*WCPMTperCellVertical+j+100)
			    ,true);                        // no particular field
	}

 	G4VPhysicalVolume* physiWCBarrelBorderPMTGlass =
	  new G4PVPlacement(WCPMTRotation,                          // its rotation
			    PMTPosition,
			    logicGlassFaceWCPMT,                // its logical volume
			    "WCPMTGlass",             // its name
			    logicWCExtraBorderCell,         // its mother volume
			    false,                     // no boolean operations
			    (int)(i*WCPMTperCellVertical+j+100)
			    ,true);                        // no particular field

	G4VPhysicalVolume* physiWCBarrelBorderPMT =
	  new G4PVPlacement(WCPMTRotation,                          // its rotation
			    PMTPosition,
			    logicWCPMT,                // its logical volume
			    "WCPMT",             // its name
			    logicWCExtraBorderCell,         // its mother volume
			    false,                     // no boolean operations
			    (int)(i*WCPMTperCellVertical+j+100)
			    ,true);                        // no particular field

	G4LogicalBorderSurface *  GlassCathodeBarreBorderlSurface
	    = new G4LogicalBorderSurface("GlassCathodeBarrelSurface",
					 physiWCBarrelBorderPMTGlass,physiWCBarrelBorderPMT,
					 OpGlassCathodeSurface);
		// logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more 
		// daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
		// this is still the case.
      }
    }

  }

// The caps an borderrings aren't nessesary symetric. We have to reflect the bottom 
// cap and border at the x-y plane. 

// add the top annulus border 

  G4Translate3D borderTranslation(0., 0., mainAnnulusHeight/2.+ barrelCellHeight/2);
  G4ReflectZ3D  reflection;
  G4Transform3D borderTransform = borderTranslation*reflection;

  G4PhysicalVolumesPair borderPair = G4ReflectionFactory::Instance()
          ->Place(borderTransform,     // the transformation with reflection
                  "WCBarrelTopRing", // the actual name
                  logicWCBarrelBorderRing,    // the logical volume
                  logicWCBarrel,     // the mother volume
                  false,         // no boolean operation
                  1,             // copy number
                  false);        // no overlap check triggered
		  
   // The ReflectionFactory crates new logical an physical volumes.
   // we have to add the BorderSurfaces to this volumes and make them sensitive.
   
   G4VPhysicalVolume* ring = borderPair.first; 
   G4LogicalVolume *ringLogical = ring->GetLogicalVolume();
   G4LogicalVolume * logicWCPMT_refl_1 = NULL;

   for(G4int i = 0; i <  ringLogical->GetNoDaughters(); i++){ // loop over the daughter an check if they
                                                              // need a surface
     G4VPhysicalVolume *daughterPhysi = ringLogical->GetDaughter(i);

     for(G4int j = 0; j <  daughterPhysi->GetLogicalVolume()->GetNoDaughters(); j++){
       if(daughterPhysi->GetLogicalVolume()->GetDaughter(j)->GetName() == "WCBarrelCellBlackSheet"){
         G4LogicalBorderSurface * WaterBSTopCapSurface 
                = new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
                         daughterPhysi->GetLogicalVolume()->GetDaughter(j),daughterPhysi,
                         OpWaterBSSurface);
	//G4cout << "BS added surface: "<<  WaterBSTopCapSurface->GetVolume1()->GetName() << " and " 
	//  <<  WaterBSTopCapSurface->GetVolume2()->GetName() << G4endl;
       }		
     
        //Add Reflective surface to the reflected LCs
    
       if( daughterPhysi->GetLogicalVolume()->GetDaughter(j)->GetName()  == "WCLC"){
	 
	 new G4LogicalSkinSurface("LC_conc_surface",
				  daughterPhysi->GetLogicalVolume()->GetDaughter(j)->GetLogicalVolume(), 
				  WCSimLC::our_Mirror_opsurf);  	
       }

      if( daughterPhysi->GetLogicalVolume()->GetDaughter(j)->GetName()  == "WCWLSPLC"){
	 
	//	 new G4LogicalSkinSurface("WLSP_PlasticSurface",
	//		  daughterPhysi->GetLogicalVolume()->GetDaughter(j)->GetLogicalVolume(), 
	//		  WCSimWLSP_LC::OpPlasticSurface);  	

G4LogicalBorderSurface *  BarrelBorderWLSPPlasticSurface
	         = new G4LogicalBorderSurface("WLSP_PlasticSurface",
			    daughterPhysi->GetLogicalVolume()->GetDaughter(j),
			    daughterPhysi,
              		    WCSimWLSP_LC::OpPlasticSurface);
//G4cout<<"Refl_1 daughterPhysi  Name= "<<daughterPhysi->GetName()<<G4endl;
//	G4cout << "PMT surface added: " << BarrelBorderWLSPPlasticSurface->GetVolume1()->GetName() << " and " 
//	  << BarrelBorderWLSPPlasticSurface->GetVolume2()->GetName()<< G4endl;	
       }

      if( daughterPhysi->GetLogicalVolume()->GetDaughter(j)->GetName()  == "WCWLSPRef"){
	 
	 new G4LogicalSkinSurface("WLSP_ReflectorSurface",
				  daughterPhysi->GetLogicalVolume()->GetDaughter(j)->GetLogicalVolume(), 
				  WCSimWLSP_Reflector::OpReflectorSurface);  	
       }


     if( daughterPhysi->GetLogicalVolume()->GetDaughter(j)->GetName()  == "WCPMTGlass"){
	   // if you add more daughters to the PMTs, you need to replace this if-statement with a loop
         G4LogicalBorderSurface *  GlassCathodeBarrelBorderSurface
	         = new G4LogicalBorderSurface("GlassCathodeBarrelBorderSurface",
			    daughterPhysi->GetLogicalVolume()->GetDaughter(j),
			    daughterPhysi->GetLogicalVolume()->GetDaughter(j+1),
			    OpGlassCathodeSurface);
	 logicWCPMT_refl_1 = daughterPhysi->GetLogicalVolume()->GetDaughter(j)->GetLogicalVolume(); // store a pointer to the logical
	                                                     // PMT volume to add a sensitive detector
							     // later on.
	
	 //G4cout << "PMT surface added: " << GlassCathodeBarrelBorderSurface->GetVolume1()->GetName() << " and " 
	 //  << GlassCathodeBarrelBorderSurface->GetVolume2()->GetName()<< G4endl;
      }		             
    }
  }
  //------------------------------------------------------
  // if necessary add the missing part to the top border
  //-----------------------------------------------------

  G4LogicalVolume * logicWCPMT_refl_2 = NULL;
  if(!(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal)){
   
   G4PhysicalVolumesPair borderPair = G4ReflectionFactory::Instance()
	    ->Place(borderTransform,     // the transformation with reflection
		    "WCExtraBarrelTopBorderCell", // the actual name
		    logicWCExtraBorderCell,    // the logical volume
		    logicWCBarrel,     // the mother volume
		    false,         // no boolean operation
		    1,             // copy number
		    false);        // no overlap check triggered
     G4VPhysicalVolume* cell = borderPair.first;

     for(G4int i = 0; i <  cell->GetLogicalVolume()->GetNoDaughters(); i++){
       
       G4VPhysicalVolume *daughterPhysi = cell->GetLogicalVolume()->GetDaughter(i);
       
       if(daughterPhysi->GetName() == "WCExtraTowerBlackSheet"){
	 G4LogicalBorderSurface * WaterBSTopCapSurface 
		= new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
			 daughterPhysi,cell,
			 OpWaterBSSurface);
	 //G4cout << "BS added surface: "<<  WaterBSTopCapSurface->GetVolume1()->GetName() << " and " 
	 //   <<  WaterBSTopCapSurface->GetVolume2()->GetName() << G4endl;
       }		
     
  //Add Reflective surface to the reflected LCs
    
       if( daughterPhysi->GetName()  == "WCLC"){	 
	 
	 new G4LogicalSkinSurface("LC_conc_surface",
				  cell->GetLogicalVolume()->GetDaughter(i)->GetLogicalVolume(),
				  WCSimLC::our_Mirror_opsurf);   
       }
       
  if( daughterPhysi->GetName()  == "WCWLSPLC"){	 	 
    /*	 new G4LogicalSkinSurface("WLSP_PlasticSurface",
				  cell->GetLogicalVolume()->GetDaughter(i)->GetLogicalVolume(),
				  WCSimWLSP_LC::OpPlasticSurface);   */

G4LogicalBorderSurface*  BarrelBorderWLSPPlasticSurface  
		 = new G4LogicalBorderSurface("WLSP_PlasticSurface",
			    daughterPhysi,
			    cell,
			    WCSimWLSP_LC::OpPlasticSurface);
// G4cout<<"Refl_2 Cell Name= "<<cell->GetName()<<G4endl;
//	G4cout << "PMT surface added: " << BarrelBorderWLSPPlasticSurface->GetVolume1()->GetName() << " and " 
//	  << BarrelBorderWLSPPlasticSurface->GetVolume2()->GetName()<< G4endl;	
       }
       
 if( daughterPhysi->GetName()  == "WCWLSPRef"){	 	 
	 new G4LogicalSkinSurface("WLSP_ReflectorSurface",
				  cell->GetLogicalVolume()->GetDaughter(i)->GetLogicalVolume(),
				  WCSimWLSP_Reflector::OpReflectorSurface);   
       }
  
        if( daughterPhysi->GetName()  == "WCPMTGlass"){	 
	 G4LogicalBorderSurface *  GlassCathodeBarrelBorderSurface
		 = new G4LogicalBorderSurface("GlassCathodeSurface",
			    daughterPhysi,
			    cell->GetLogicalVolume()->GetDaughter(i+1),
			    OpGlassCathodeSurface);
	 logicWCPMT_refl_2 = daughterPhysi->GetLogicalVolume(); // store a pointer to the logical
	                                                     // PMT volume to add a sensitive detector
							     // later on.

		 // G4cout << "PMT surface added: " << GlassCathodeBarrelBorderSurface->GetVolume1()->GetName() << " and " 
	 //  << GlassCathodeBarrelBorderSurface->GetVolume2()->GetName()<< G4endl;
      }
    }
  }

 // add top cap

  G4Translate3D capTranslation(0., 0., WCIDHeight/2.);
  G4Transform3D capTransform = capTranslation*reflection;

  G4PhysicalVolumesPair capsPair = G4ReflectionFactory::Instance()
          ->Place(capTransform,     // the transformation with reflection
                  "WCCap", // the actual name
                  logicWCCap,    // the logical volume
                  logicWCBarrel,     // the mother volume
                  false,         // no boolean operation
                  1,             // copy number
                  false);        // no overlap check triggered
   G4VPhysicalVolume* cover = capsPair.first;
   
   G4LogicalVolume *coverLogical = cover->GetLogicalVolume();
   G4LogicalVolume * logicWCPMT_refl_3 = NULL;

   for(G4int i = 0; i <  coverLogical->GetNoDaughters(); i++){
   
     G4VPhysicalVolume *daughterPhysi = coverLogical->GetDaughter(i);
     
     if(daughterPhysi->GetName() == "WCCapBlackSheet"){
       G4LogicalBorderSurface * WaterBSTopCapSurface 
                = new G4LogicalBorderSurface("WaterBSCapPolySurface",
                         cover,daughterPhysi,
                         OpWaterBSSurface);
	// G4cout << "BS added surface: "<<  WaterBSTopCapSurface->GetVolume1()->GetName() << " and " 
	//   <<  WaterBSTopCapSurface->GetVolume2()->GetName() << G4endl;
    }	

     //Add Reflective surface to the reflected LCs
     if(daughterPhysi->GetName() == "WCCapLC"){
       
       new G4LogicalSkinSurface("LC_conc_surface",coverLogical->GetDaughter(i)->GetLogicalVolume(), 
				WCSimLC::our_Mirror_opsurf);
     }

    if(daughterPhysi->GetName() == "WCCapWLSPLC"){
       
      //  new G4LogicalSkinSurface("WLSP_PlasticSurface",coverLogical->GetDaughter(i)->GetLogicalVolume(), 
      //			WCSimWLSP_LC::OpPlasticSurface);
      
        G4LogicalBorderSurface* CapWLSPPlasticSurface
	         = new G4LogicalBorderSurface("WLSP_PlasticSurface",
					      // daughterPhysi,cover,
					      daughterPhysi,physiWCBarrel,
					      WCSimWLSP_LC::OpPlasticSurface);  
	//	G4cout<<"refl_3 cover name= "<<physiWCBarrel->GetName()<<G4endl;       
	//	G4cout << "PMT surface added: " << CapWLSPPlasticSurface->GetVolume1()->GetName() << " and " 
	//	  << CapWLSPPlasticSurface->GetVolume2()->GetName()<< G4endl;	                    
    }

   if(daughterPhysi->GetName() == "WCCapWLSPRef"){
       
       new G4LogicalSkinSurface("WLSP_ReflectorSurface",coverLogical->GetDaughter(i)->GetLogicalVolume(), 
				WCSimWLSP_Reflector::OpReflectorSurface);
     }

      if(daughterPhysi->GetName() == "WCPMTGlass"){
        G4LogicalBorderSurface *  GlassCathodeCapBorderSurface
	         = new G4LogicalBorderSurface("GlassCathodeSurface",
		            daughterPhysi,coverLogical->GetDaughter(i+1),
				    OpGlassCathodeSurface);
        logicWCPMT_refl_3 = daughterPhysi->GetLogicalVolume(); // store a pointer to the logical
	                                                     // PMT volume to add a sensitive detector
							     // later on.
	//G4cout << "PMT surface added: " <<GlassCathodeCapBorderSurface ->GetVolume1()->GetName() << " and " 
	//  << GlassCathodeCapBorderSurface->GetVolume2()->GetName()<< G4endl;	                                 
    }
  }
  // Make sensitive detectors

  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  if (!aWCPMT) 
  {
    aWCPMT = new WCSimWCSD( "/WCSim/glassFaceWCPMT",this );
    SDman->AddNewDetector( aWCPMT );
  }
  logicGlassFaceWCPMT->SetSensitiveDetector( aWCPMT );
  // if there are PMTs in the reflected volumes, make them sensitive'
  if(logicWCPMT_refl_1 != NULL) logicWCPMT_refl_1->SetSensitiveDetector( aWCPMT );
  if(logicWCPMT_refl_2 != NULL) logicWCPMT_refl_2->SetSensitiveDetector( aWCPMT );
  if(logicWCPMT_refl_3 != NULL) logicWCPMT_refl_3->SetSensitiveDetector( aWCPMT );


//  return logicWCBox;
  return logicWC;
}




