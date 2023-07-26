#include "WCSimMultiPMTParameterisation.hh"

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SystemOfUnits.hh"

#include "G4PhysicalConstants.hh"

#include <vector>
#include "assert.h"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WCSimMultiPMTParameterisation::WCSimMultiPMTParameterisation(
        G4int    noPmt,
        G4double apoth,          // Distance from the Z axis
	std::vector<G4int>& vNiC,
	std::vector<G4double>& vAlpha,
	std::vector<G4int>& vCircle,
	std::vector<G4double>& vAzimOffset,
	G4double height)
 : G4VPVParameterisation()
{
  fNoPmt	        = noPmt;
  fApothema	        = apoth;
  vNiCLocal	        = vNiC;
  vAlphaLocal	        = vAlpha;
  vAzimOffsetLocal	= vAzimOffset;
  vCircleLocal          = vCircle;
  fHeight               = height;
   
   //Fill the position and orientation vectors once (and allocation mem once).
   PreCalculateTransform();   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WCSimMultiPMTParameterisation::~WCSimMultiPMTParameterisation()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// This function gets called a lot, every step for every event. Therefore
// limit calculation and MEM allocation. Very MEM leak sensitive, eg. defining
// and calculating RotationMatrix here. Can't change a class member here because it's
// a const function. Hence, pass vector with precalculated RotationMatrix
void WCSimMultiPMTParameterisation::ComputeTransformation
(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
 
  physVol->SetTranslation(vPMTpos[copyNo]);
  physVol->SetRotation(vPMTorient[copyNo]);

}

void WCSimMultiPMTParameterisation::PreCalculateTransform(){

  // Note: copyNo will start with zero!
  for(unsigned int copy = 0; copy < fNoPmt; copy++){
    G4RotationMatrix* rotm = new G4RotationMatrix(); // Rotation matrix for each PMT
    
    // previous version: overly complicated as
    // pi - ((vNiCLocal[vCircleLocal[copy]]-2)*CLHEP::pi/vNiCLocal[vCircleLocal[copy]]) 
    // = 1/(vNiCLocal[vCircleLocal[copy]]) * (pi * vNiCLocal[vCircleLocal[copy]] - vNiCLocal[vCircleLocal[copy]] * pi + 2pi )
    // = 2pi / (vNiCLocal[vCircleLocal[copy]]) --> more logical and intuitive
    //G4double angle = ((vNiCLocal[vCircleLocal[copy]]-2)*CLHEP::pi/vNiCLocal[vCircleLocal[copy]]); // Internal angle of each polygon
    G4double azimuth = 2.*CLHEP::pi / (vNiCLocal[vCircleLocal[copy]]); 
    G4double phi = copy*azimuth + vAzimOffsetLocal[vCircleLocal[copy]]; 
    G4double theta = CLHEP::halfpi-vAlphaLocal[vCircleLocal[copy]];
        
    G4ThreeVector origin(0,0,0); 
    origin.setRThetaPhi(fApothema,theta,phi);  
    
    // Positioning vector, for an offset single hemisphere
    origin.setZ(origin.getZ()+fHeight);

    // rotation of mother volume wrt daughter, hence minus sign.
    rotm->rotateZ(-phi);
    rotm->rotateY(-theta); //over Y'

    //G4cout << "PMT " << copy << " (theta,phi): (" << theta*(180./CLHEP::pi)  << "," <<  phi*(180./CLHEP::pi) << ")" << G4endl;
    //G4cout << "PMT " << copy << " (x,y,z): (" << origin.getX()  << "," <<  origin.getY() << "," << origin.getZ() << ")" << G4endl;
    vPMTpos.push_back(origin);
    vPMTorient.push_back(rotm);



  }

  assert(vPMTpos.size() == fNoPmt);
  assert(vPMTorient.size() == fNoPmt);
}
