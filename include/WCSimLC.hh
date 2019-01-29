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
		  G4int LCType
  		 );
 
  G4double GetHeight();
  G4double GetRadius();
  G4double GetOffset();
  void SetInvisible();
   
  protected:
 
    
  G4double a,t,theta,costh,sinth,f;
  G4double r,rmin, rmax;
  G4double thickness_mirror;
  G4double LCOffset;
  G4int n;                                   
                 
  G4double Winston_z_from_r (double r);
  G4double Tsukada_z_from_r (double radius);
 


public:
  static  G4OpticalSurface* OpLCMirrorSurface;

};

#endif
