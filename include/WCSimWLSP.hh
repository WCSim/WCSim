#ifndef WCSimWLSP_H
#define WCSimWLSP_H 1
                                                                                
#include "G4LogicalVolume.hh"
#include "G4Polycone.hh"
#include "G4Tubs.hh"
#include "G4Material.hh"
#include "G4OpticalSurface.hh"
                                                                                
class WCSimWLSP_LC : public G4LogicalVolume {
public:
          WCSimWLSP_LC(		  
		  const G4String &name, 		 
		  G4Material* cone_Material,
		  G4Material* plate_Material,
		  int plate_size
  		 );
 
 
  void SetInvisible();
   
protected:
  G4double LC_z, LC_rmin, LC_rmax, LC_pmin, LC_pmax, bulb_thickness, gap;

public:
  static  G4OpticalSurface*  OpPlasticSurface; //plastic optical surface

};

class WCSimWLSP_Reflector : public G4LogicalVolume {
public:
          WCSimWLSP_Reflector(		  
		  const G4String &name, 		 
		  G4Material* reflector_Material,
		  G4Material* reflect_Material,
		  int plate_size
  		 );
 
 
  void SetInvisible();
   
protected:
  G4double LC_z, LC_rmin, LC_rmax, LC_pmin, LC_pmax, bulb_thickness, gap;

public:
  static  G4OpticalSurface*  OpReflectorSurface;//reflector optical surface
 
};

#endif
