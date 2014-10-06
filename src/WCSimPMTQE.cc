//  -*- mode:c++; tab-width:4;  -*-
#include "WCSimDetectorConstruction.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

/***********************************************************
 *
 * This file contains the PMT QE as a function of 
 * wavelength for variety of PMTs.  It is used in both the
 * StackingAction and the sensitive detectors so it was 
 * put here (in WCSimDetectorConstruction) so both pieces of 
 * code would have access to it.
 *
 * Sourcefile for the WCSimDetectorConstruction class
 *
 ***********************************************************/


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




