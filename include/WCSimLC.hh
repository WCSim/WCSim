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
 
  void SetInvisible();
   

public:
  static  G4OpticalSurface*  our_Mirror_opsurf;

};

#endif
 
