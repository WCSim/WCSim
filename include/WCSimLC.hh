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
		  G4Material* bulkMaterial
  		 );
 
  G4double GetHeight();
  void SetInvisible();
   
  protected:

  
 
    
  G4double a,t,theta,costh,sinth,f;
  G4double r,rmin, rmax;
  G4int n;                                   
                 
  G4double Winston_z_from_r (double r);
 


public:
  static  G4OpticalSurface*  our_Mirror_opsurf;

};

#endif
