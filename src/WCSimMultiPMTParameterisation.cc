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
	G4double height)
 : G4VPVParameterisation()
{
   fNoPmt	= noPmt;
   fApothema	= apoth;
   vNiCLocal	= vNiC;
   vAlphaLocal	= vAlpha;
   vCircleLocal = vCircle;
   fHeight      = height;
   
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
    
    G4double angle = ((vNiCLocal[vCircleLocal[copy]]-2)*CLHEP::pi/vNiCLocal[vCircleLocal[copy]]); // Internal angle of each polygon
    G4ThreeVector origin(0,0,0); 
    origin.setRThetaPhi(fApothema,CLHEP::halfpi-vAlphaLocal[vCircleLocal[copy]],copy*(CLHEP::pi-angle));
    // TF: Positioning vector, for an offset single hemisphere (for now)
    origin.setZ(origin.getZ()+fHeight);
    
    // For output to txt file for AutoCad/Solidworks
    //G4cout << "PMT " << copy << " (theta,phi): (" << (CLHEP::halfpi-vAlphaLocal[vCircleLocal[copy]])*(180./CLHEP::pi)  << "," <<  origin.getPhi()*(180./CLHEP::pi) << ")" << G4endl;
    // rotation of mother volume wrt daughter, hence minus sign.
    rotm->rotateZ(-origin.getPhi());
    rotm->rotateY(-acos((origin.getZ()-fHeight)/fApothema)); //over Y', origin.getTheta() is incorrect!

    vPMTpos.push_back(origin);
    vPMTorient.push_back(rotm);

  }

  assert(vPMTpos.size() == fNoPmt);
  assert(vPMTorient.size() == fNoPmt);
}
