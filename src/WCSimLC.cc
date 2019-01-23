#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "WCSimLC.hh"

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

#include <vector>
using namespace std;

G4OpticalSurface * WCSimLC::OpLCMirrorSurface = NULL;

WCSimLC::WCSimLC(
		const G4String &name, 
		G4Material* bulkMaterial,
		G4int LCType
		) : G4LogicalVolume(new G4Box("temp",10,10,10), bulkMaterial, name)
{

	a = 110*2.32;//millimeter;   // smallest radius of LC
	t = 50*2.32;//millimeter;
	theta = 60*CLHEP::degree;  // or convert as needed
	costh = cos(theta);
	sinth = sin(theta);
	f = 0.5*(t*costh+a*sinth)+0.5*sqrt(a*a+t*t);
	G4cout<<"THETA= "<<theta/CLHEP::degree<<G4endl;
	G4cout<<"F= "<<f<<G4endl;

	rmin = a;
	G4cout<<"*WCSimLC* Light collector  enabled."<<G4endl;
	if(LCType==2){
		G4cout<<"*WCSimLC* Tsukada Mirror enabled."<<G4endl;
		rmax = 320;//millimeter;// Tsukada
	}
	else if (LCType==1){
		rmax = 139*2.32;//millimeter;// Winston
	}

	n = 0;
	thickness_mirror=5*mm;
	LCOffset = 60 *mm;

	r = rmin;

	G4double  rin[70], rout[70], z[70];

	while (1)
	{
		rin[n] = r;
		rout[n] = rin[n] + thickness_mirror;//millimeter;
		if(LCType==2){
			z[n] = Tsukada_z_from_r(rin[n]);
		}
		else if (LCType==1){
			z[n] = Winston_z_from_r(rin[n]);
		}
		//r = r + (4.*(2.636-r*0.0181));//millimeter;
		r = r + 2;//millimeter;
		if (r > rmax){
			n=n-1;
			break;
		}
		G4cout<<"*Tsukada* n= "<<n<<"; r= "<<rin[n]<<"; z= "<<z[n]<<G4endl;
		n++;
	}

	if ( OpLCMirrorSurface == NULL )
	{
		/*
		   OpLCMirrorSurface =  new G4OpticalSurface("Mirror_opsurf");
		   OpLCMirrorSurface->SetFinish(polishedfrontpainted); 
		   OpLCMirrorSurface->SetModel(glisur);
		   OpLCMirrorSurface->SetType(dielectric_metal);
		   OpLCMirrorSurface->SetPolish(0.999);              
		   */

		OpLCMirrorSurface =  new G4OpticalSurface("Mirror_opsurf");
		OpLCMirrorSurface->SetType(dielectric_metal);
		OpLCMirrorSurface->SetModel(unified);
		//OpLCMirrorSurface->SetFinish(polishedfrontpainted); 
		OpLCMirrorSurface->SetFinish(polished); 
		OpLCMirrorSurface->SetSigmaAlpha(0);

		G4MaterialPropertiesTable* propMirror=
			new G4MaterialPropertiesTable();

		const int NUMENTRIES_water = 60;
		G4double ENERGY_water[NUMENTRIES_water] =
		{ 1.56962e-09*GeV, 1.58974e-09*GeV, 1.61039e-09*GeV, 1.63157e-09*GeV,
			1.65333e-09*GeV, 1.67567e-09*GeV, 1.69863e-09*GeV, 1.72222e-09*GeV,
			1.74647e-09*GeV, 1.77142e-09*GeV,1.7971e-09*GeV, 1.82352e-09*GeV,
			1.85074e-09*GeV, 1.87878e-09*GeV, 1.90769e-09*GeV, 1.93749e-09*GeV,
			1.96825e-09*GeV, 1.99999e-09*GeV, 2.03278e-09*GeV, 2.06666e-09*GeV,
			2.10169e-09*GeV, 2.13793e-09*GeV, 2.17543e-09*GeV, 2.21428e-09*GeV,
			2.25454e-09*GeV, 2.29629e-09*GeV, 2.33962e-09*GeV, 2.38461e-09*GeV,
			2.43137e-09*GeV, 2.47999e-09*GeV, 2.53061e-09*GeV, 2.58333e-09*GeV,
			2.63829e-09*GeV, 2.69565e-09*GeV, 2.75555e-09*GeV, 2.81817e-09*GeV,
			2.88371e-09*GeV, 2.95237e-09*GeV, 3.02438e-09*GeV, 3.09999e-09*GeV,
			3.17948e-09*GeV, 3.26315e-09*GeV, 3.35134e-09*GeV, 3.44444e-09*GeV,
			3.54285e-09*GeV, 3.64705e-09*GeV, 3.75757e-09*GeV, 3.87499e-09*GeV,
			3.99999e-09*GeV, 4.13332e-09*GeV, 4.27585e-09*GeV, 4.42856e-09*GeV,
			4.59258e-09*GeV, 4.76922e-09*GeV, 4.95999e-09*GeV, 5.16665e-09*GeV,
			5.39129e-09*GeV, 5.63635e-09*GeV, 5.90475e-09*GeV, 6.19998e-09*GeV };

		G4double REF_mirror = 0.9;

		G4double REFLECTIVITY_mirror[NUMENTRIES_water] =
		{ REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror,
			REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror,
			REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror,
			REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror,
			REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror,
			REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror,
			REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror,
			REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror,
			REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror,
			REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror, REF_mirror};

		propMirror->AddProperty("REFLECTIVITY", ENERGY_water, REFLECTIVITY_mirror, NUMENTRIES_water);

		/*
		   propMirror->AddProperty("REFLECTIVITY", new G4MaterialPropertyVector());
		   propMirror->AddEntry("REFLECTIVITY", 2*M_PI*CLHEP::hbarc/(800.0e-9*CLHEP::m), 0.9);
		   propMirror->AddEntry("REFLECTIVITY", 2*M_PI*CLHEP::hbarc/(200.0e-9*CLHEP::m), 0.9);
		   */
		OpLCMirrorSurface->SetMaterialPropertiesTable( propMirror );
	}

	G4cout<<"N= "<<n<<G4endl;

	G4Polycone *conc_solid = new G4Polycone(name+"_plastic", 0, 2*M_PI, n, z, 
			rin,
			rout);

	SetSolid(conc_solid);
	SetName(name+"_plastic");

	new G4LogicalSkinSurface(name+"_conc_surface",this, OpLCMirrorSurface);

	G4VisAttributes* visAtt= new G4VisAttributes(G4Color(1.0,1.0,0.,1.0));
	visAtt->SetForceSolid(true);     

}

G4double WCSimLC::GetHeight()
{
	return Winston_z_from_r(rmax) - Winston_z_from_r(rmin);
}

G4double WCSimLC::GetRadius()
{
	return rmax;
}

G4double WCSimLC::GetOffset()
{
	return LCOffset;
}

G4double WCSimLC::Winston_z_from_r (double radius)
{
	G4double z2;

	z2 = (2*f-radius*costh)*costh-2*sqrt(f*(f-radius*sinth));
	z2 = z2/(sinth*sinth) + t;
	return z2;
}

/* 2018Oct mirror, by Tsukada-san */
G4double WCSimLC::Tsukada_z_from_r (double radius)
{
	G4double offset = 62.3;
	radius = radius * 3./5.;
	G4double par[5] = {0.0000435759,0.00000830419,-0.00003567787,-0.01320270214,0.00526226472};
	G4double z2;
	z2 =((-par[2]*radius-par[4])/(2*par[1])-sqrt((((par[2]*radius+par[4])*(par[2]*radius+par[4]))/(2*par[1]*2*par[1]))-((par[0]*radius*radius+par[3]*radius+1)/par[1])));
	z2 = z2 * 5./3.;

	return z2 + offset;
}

void WCSimLC::SetInvisible()
{
	this->SetVisAttributes(G4VisAttributes::Invisible);

}
