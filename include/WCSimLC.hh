#ifndef WCSimLC_H
#define WCSimLC_H 1
                                                                                
#include "G4LogicalVolume.hh"
#include "G4Polycone.hh"
#include "G4Material.hh"
#include "G4OpticalSurface.hh"
                                                                                
class WCSimLC : public G4LogicalVolume {
public:
          WCSimLC(		  
		  const G4String &name, 		 
		  G4Material* bulkMaterial,
		  G4double LC_rmin1,
		  G4double LC_rmax1,
		  G4double LC_a1,
		  G4double LC_b1,
		  G4double LC_d1,
		  G4double LC_refl
  		 );
 
  void SetInvisible();
   

public:
  static  G4OpticalSurface*  our_Mirror_opsurf;

};

#endif
 
