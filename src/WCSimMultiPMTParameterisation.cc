#include "WCSimMultiPMTParameterisation.hh"

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SystemOfUnits.hh"

#include "G4PhysicalConstants.hh"

#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WCSimMultiPMTParameterisation::WCSimMultiPMTParameterisation(
        G4int    noPmt,
        G4double apoth,          // Distance from the Z axis
	std::vector<G4int>& vNiC,
	std::vector<G4double>& vAlpha,
	std::vector<G4int>& vCircle)
 : G4VPVParameterisation()
{
   fNoPmt	= noPmt;
   fApothema	= apoth;
   vNiCLocal	= vNiC;
   vAlphaLocal	= vAlpha;
   vCircleLocal = vCircle;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WCSimMultiPMTParameterisation::~WCSimMultiPMTParameterisation()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCSimMultiPMTParameterisation::ComputeTransformation
(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
  // Note: copyNo will start with zero!
  G4RotationMatrix* rotm = new G4RotationMatrix(); // Rotation matrix for each chamber
  G4double angle = ((vNiCLocal[vCircleLocal[copyNo]]-2)*pi/vNiCLocal[vCircleLocal[copyNo]]); // Internal angle of each polygon
  G4ThreeVector origin(0,0,0); // Positioning vector
  const G4double chambSize = fApothema/std::tan(angle/2);

  origin.setRThetaPhi(fApothema,halfpi-vAlphaLocal[vCircleLocal[copyNo]],copyNo*(pi-angle));

  rotm->rotateX(halfpi);
  rotm->rotateY((halfpi-(copyNo*angle)));
  rotm->rotateX((std::pow(-1,copyNo)*vAlphaLocal[vCircleLocal[copyNo]]));
  physVol->SetTranslation(origin);
  physVol->SetRotation(rotm);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/*
void WCSimMultiPMTParameterisation::ComputeDimensions
(G4Tubs& pmtidmulti, const G4int copyNo, const G4VPhysicalVolume*) const
{
  // Note: copyNo will start with zero!

  G4double angle = ((fNoPmt-2)*pi/fNoPmt);
  const G4double chambSize = fApothema/std::tan(angle/2);
  pmtidmulti.SetZHalfLength(chambSize/5);
  pmtidmulti.SetOuterRadius(chambSize);
  G4cout << "Chamber radius = " << chambSize << " cm" << G4endl;

}
*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4int WCSimMultiPMTParameterisation::CountPMT(G4int NoPmt)
{
  G4double fEta = ComputeEta(NoPmt); // Chamber viewing angle

  // First element of vNiC and vAlpha vectors is added manually
  // to allow recursive calculation of the following
  vNiCLocal.push_back(NoPmt);
  vNiCLocal.push_back(NoPmt);
  vAlphaLocal.push_back(-fEta);

  G4int NoCircle = 1;
  G4double alphaNext = ComputeAlpha(vAlphaLocal[NoCircle-1], fEta);
  G4double NiCNext = NoPmt;

  vAlphaLocal.push_back(alphaNext);

  G4cout << "Circle n. " << NoCircle << " - Number of chambers: " << vNiCLocal[NoCircle] << G4endl;
  NoCircle++;

  // Recursive calculation of vector contents
  do
    {
      alphaNext = ComputeAlpha(vAlphaLocal[NoCircle-1], fEta);
      NiCNext = ComputeNiC(alphaNext, fEta);
      if(NiCNext<2){break;} // Prevents negative values of NiC
      vAlphaLocal.push_back(alphaNext);
      vNiCLocal.push_back(NiCNext);
      G4cout << "Circle n. " << NoCircle << " - Number of chambers: " << vNiCLocal[NoCircle] << G4endl;
      NoCircle++;
    } while(NiCNext > 2); // No less than 3 chambers in each circle

  // Total number of pmt in the circles
  G4int TotPmt = 0;
  for(int i=1;i<vNiCLocal.size();i++)
    {
      TotPmt+=vNiCLocal[i];
      for(int j=0;j<vNiCLocal[i];j++){vCircleLocal.push_back(i);}
    }
  G4cout << "Total number of pmt: " << TotPmt << G4endl;
  G4cout << "Percentage of covered hemispherical surface = " << TotPmt*(1-std::cos(fEta))*100 << "%" << G4endl;

  return TotPmt;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double WCSimMultiPMTParameterisation::ComputeEta(G4int NoPmt)
{
  G4double cospi = std::cos(pi/NoPmt);
  G4double etaLocal = std::atan(std::sqrt(1-std::pow(cospi,2)));
  return etaLocal;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double WCSimMultiPMTParameterisation::ComputeAlpha (G4double alphaOfPrevC, G4double Eta)
{
  G4double alphaLocal = alphaOfPrevC + 2*Eta;
  return alphaLocal;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int WCSimMultiPMTParameterisation::ComputeNiC (G4double alphaOfCircle, G4double Eta)
{
  G4double	cos2eta	  = std::pow(std::cos(Eta),2);
  G4double	sin2alpha = std::pow(std::sin(alphaOfCircle),2);
  G4double	cosalpha  = std::cos(alphaOfCircle);
  G4double	arccos	  = std::acos(std::sqrt(cos2eta-sin2alpha)/cosalpha);
  G4int		NiCLocal  = std::floor(pi/arccos);
  return NiCLocal;
}
