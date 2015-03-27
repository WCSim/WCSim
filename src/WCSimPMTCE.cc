//  -*- mode:c++; tab-width:4;  -*-
#include "WCSimDetectorConstruction.hh"
#include "WCSimPMTObject.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

/***********************************************************
 *
 * This file contains the rate of any signal output 
 * (include the events which fail to amplify). 
 * It will be necessary to introduce CE as a function of 
 * wavelength for variety of PMTs.  It is used when you set 
 * "/WCSim/PMTCollEff on" in macro file.
 *
 ***********************************************************/


G4float WCSimDetectorConstruction::GetPMTCE(float theta_angle){
  
  WCSimPMTObject *PMT;
  PMT = GetPMTPointer();
  G4float* CE = PMT->GetCE();
  G4float* angle = PMT->GetCEAngle();
  G4double angleCE = 0;

  angleCE = Interpolate_func(theta_angle,10,angle,CE)/100.;
  
  return angleCE;
}

G4float WCSimDetectorConstruction::Interpolate_func(G4float x, G4int ncount, G4float *angle, G4float *quantity){
  // linear interpolate the quantity function versus angle                                                                                                                        
  if (x < *angle || x >=*(angle+ncount-1)){
    return 0;
  }else{
    for (Int_t i=0;i!=ncount;i++){
      if (x>=*(angle+i) && x < *(angle+i+1)){
        return (x-*(angle+i))/(*(angle+i+1)-*(angle+i))* (*(quantity+i+1)) + (*(angle+i+1)-x)/(*(angle+i+1)-*(angle+i)) * (*(quantity+i));
      }
    }
  }
}



