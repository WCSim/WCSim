//  -*- mode:c++; tab-width:4;  -*-
#include "WCSimDetectorConstruction.hh"
#include "WCSimPMTObject.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "WCSimTuningParameters.hh"

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


G4double WCSimDetectorConstruction::GetPMTQE(G4String CollectionName, G4double PhotonWavelength, G4int flag, G4double low_wl, G4double high_wl, G4double ratio){
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

  // return 0 for wavelength outside the range
  if (flag==1){
    /*ODCOMMENT -- We use photons < 280nm so we need ->*/ //if (PhotonWavelength <= low_wl || PhotonWavelength >= high_wl ){
    if (PhotonWavelength <= low_wl || PhotonWavelength >= high_wl || PhotonWavelength <=280 || PhotonWavelength >=660){
      return 0;
    }
  }else if (flag==0){
    if (PhotonWavelength <= low_wl || PhotonWavelength >= high_wl){
      return 0;
    }
  }

  WCSimPMTObject *PMT;
  PMT = GetPMTPointer(CollectionName);
  G4double *wavelength;
  wavelength = PMT->GetQEWavelength();
  G4double *QE;
  QE = PMT->GetQE();
  G4double maxQE;
  maxQE = PMT->GetmaxQE();
  G4double wavelengthQE = 0;
  int n = PMT->GetNbOfQEDefined();

  //Added by B.Quilain 2018/07/25
  G4double QEFF = 1.0;
  QEFF = WCSimTuningParams->GetQeff();   //defaults in mac: 1.
  //maxQE *= QEFF;
  //for (int i=0; i<20; i++){
  //	*(QE+i) *= QEFF;//QE is an array with 20 entries, see src/WCSimPMTObject.cc
  //}
  //

  if (flag == 1){
    //MF: off by one bug fix.
    for (int i=0; i<n; i++){
	  if ( PhotonWavelength <= *(wavelength+(i+1))){
		wavelengthQE = *(QE+i) + 
		  (*(QE+(i+1))-*(QE+i))/(*(wavelength+(i+1))-*(wavelength+i))*
		  (PhotonWavelength - *(wavelength+i));
      	break;
      }
	   }
  }else if (flag == 0){
	wavelengthQE = maxQE; 
  }
  wavelengthQE = wavelengthQE*ratio*QEFF;

  //Added by B.Quilain 2018/07/25
  //for (int i=0; i<20; i++){
	//*(QE+i) /= QEFF;
  //}
  //
  
  return wavelengthQE;
}


G4double WCSimDetectorConstruction::GetStackingPMTQE(G4double PhotonWavelength, G4int flag, G4double low_wl, G4double high_wl, G4double ratio) {

  if (flag==1){
    if (PhotonWavelength <= low_wl || PhotonWavelength >= high_wl ){
      return 0;
    }
  }else if (flag==0){
    if (PhotonWavelength <= low_wl || PhotonWavelength >= high_wl){
      return 0;
    }
  }

  // Recover combined PMT object
  WCSimBasicPMTObject *PMT;
  PMT = GetBasicPMTObject();

  // Recover optical response of any WLS materials
  WCSimWLSProperties *WLS;
  WLS = GetWLSPointer();

  if(flag==1){
    if(PMT && WLS){
      return (G4double)(std::max(PMT->GetgQE()->Eval(PhotonWavelength,0,"S"),
                                WLS->GetgAbs()->Eval(PhotonWavelength,0,"S")))*ratio;
    } else {
      return (G4double)(PMT->GetgQE()->Eval(PhotonWavelength,0,"S"))*ratio;
    }
  }
  else if (flag==0){
	return PMT->GetmaxQE()*ratio;
  }
  else return 0;

}
