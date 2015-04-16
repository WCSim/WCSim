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
				std::vector<G4int>& vCircle);
  
  virtual ~WCSimMultiPMTParameterisation();
  
  // MultiPMT methods
  G4int	CountPMT(G4int NoPmt);
  G4double	ComputeEta (G4int NoPmt);
  G4double	ComputeAlpha (G4double alphaOfPrevC, G4double Eta);
  G4int	ComputeNiC (G4double alphaOfCircle, G4double Eta);
  
  void ComputeTransformation (const G4int copyNo,
			      G4VPhysicalVolume* physVol) const;
  
  //void ComputeDimensions (G4Tubs & pmtid_multi, const G4int copyNo,   // DOES NOT exist with G4Tubs
  //                         const G4VPhysicalVolume* physVol) const;
  
private:
  
  G4int		fNoPmt;   
  G4double	fApothema;    // The distance from the Z axis
  
  // // MultiPMT data members
  // std::vector<G4int>		vNiC;	        // Nb of Chambers in each circle
  // std::vector<G4double>  	vAlpha;	        // Tilt angle for each circle
  // std::vector<G4int>		vCircle;	// Circle numbers
  
  // Local NiC, Alpha and Circle vectors
  std::vector<G4int>	vNiCLocal;
  std::vector<G4double> vAlphaLocal;
  std::vector<G4int>	vCircleLocal;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
