#ifndef WCSimLC_H
#define WCSimLC_H 1

#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Polycone.hh"
#include "G4Polyhedra.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4VisAttributes.hh"
#include "G4NistManager.hh"
#include "G4MaterialPropertiesTable.hh"

class WCSimLC : public G4LogicalVolume {
	public:
		WCSimLC(		  
				const G4String &name, 		 
				G4Material* ma_bulk,
				G4int lc_type 
			   );

		G4double GetHeight();
		G4double GetRadius();
		G4double GetOffset();
		void SetInvisible();

	protected:


		G4double a,t,theta,costh,sinth,f;
		G4double r, rmin, rmax;
		G4float  thick_bulk;
		G4float  thick_refl;
		G4float  thick_back;
		G4float  thick_btw_refl_back;
		G4float  offset_lc;//Position Offset
		G4int n;                                   
		G4int lc_type_internal;                                   

		G4double Winston_z_from_r (double r);
		G4double Tsukada_z_from_r (double radius);

		G4OpticalSurface * op_refl    = NULL;
		G4OpticalSurface * op_back    = NULL;
		G4LogicalVolume  * lo_refl    = NULL;
		G4LogicalVolume  * lo_back    = NULL;

		G4Polycone *so_bulk = NULL;
		G4Polycone *so_refl = NULL;
		G4Polycone *so_back = NULL;

};
#endif
