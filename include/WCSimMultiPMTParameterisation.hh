#ifndef WCSimMultiPMTParameterisation_h
#define WCSimMultiPMTParameterisation_h 1

#include "globals.hh"
#include "G4VPVParameterisation.hh"
#include <vector>

class G4VPhysicalVolume;
class G4Tubs;

// Dummy declarations to get rid of warnings ...
class G4Trd;
class G4Trap;
class G4Cons;
class G4Orb;
class G4Sphere;
class G4Ellipsoid;
class G4Torus;
class G4Para;
class G4Hype;
class G4Box;
class G4Polycone;
class G4Polyhedra;

///  A parameterisation that describes a series of boxes along Z.
///
///  The boxes have equal width, & their lengths are a linear equation.
///  They are spaced an equal distance apart, starting from given location.

class WCSimMultiPMTParameterisation : public G4VPVParameterisation
{ 
  public:
  
  WCSimMultiPMTParameterisation(G4int    noPmt, 
				G4double apoth,
				std::vector<G4int>& vNiC,
				std::vector<G4double>& vAlpha,
				std::vector<G4int>& vCircle,
				G4double height);
  
  virtual ~WCSimMultiPMTParameterisation();
  void ComputeTransformation (const G4int copyNo,
			      G4VPhysicalVolume* physVol) const;
  
  // gf va reinserito:
  //void ComputeDimensions (G4Tubs & pmtid_multi, const G4int copyNo,   // DOES NOT exist with G4Tubs
  //                         const G4VPhysicalVolume* physVol) const;
  
private:
  
  G4int		fNoPmt;   
  G4double	fApothema;    // The distance from the Z axis
  
  // Local NiC, Alpha and Circle vectors
  std::vector<G4int>	vNiCLocal;
  std::vector<G4double> vAlphaLocal;
  std::vector<G4int>	vCircleLocal;

  G4double   fHeight;
  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
