#include "WCSimLC.hh"
#include <vector>
using namespace std;

WCSimLC::WCSimLC(
		const G4String &name, 
		G4Material* ma_bulk,
		G4int lc_type
		) : G4LogicalVolume(new G4Box("temp",10,10,10), ma_bulk, name)
{

	// Setting mirror shape.
	a = 110*2.32;//millimeter;// Min. radius of LC
	t = 50*2.32; //millimeter;// Height of LC
	rmax = 320;  //millimeter;// Max. radius of LC (temporal)
	rmin = a;
	lc_type_internal = lc_type;

	G4cout<<"*WCSimLC* Light collector  enabled."<<G4endl;
	if(lc_type==2){
		G4cout<<"*WCSimLC* Tsukada Mirror enabled."<<G4endl;
		rmax = 320;//millimeter;// Tsukada
	}
	else if (lc_type==1){
		theta = 60*CLHEP::degree;  // or convert as needed
		costh = cos(theta);
		sinth = sin(theta);
		f = 0.5*(t*costh+a*sinth)+0.5*sqrt(a*a+t*t);
		rmax = 139*2.32;//millimeter;// Winston
		G4cout<<"*WCSimLC* Winston Mirror enabled."<<G4endl;
		G4cout<<"*WCSimLC* THETA= "<<theta/CLHEP::degree<<G4endl;
		G4cout<<"*WCSimLC* F= "<<f<<G4endl;
	}

	n = 0;
	thick_bulk=5*mm;// Thickness of main material (e.g. Acryl).
	thick_refl=0.1*mm; // Thickness of relective layer (e.g. Al).
	thick_back=0.1*mm; // Thickness of back layer (e.g. blacksheet?).
	thick_btw_refl_back=0.1*mm; // Thickness between relective layer and back layer.
	offset_lc = 60 *mm;

	r = rmin;

	G4double  rin[70], rout[70], z[70];
	G4double  rin_refl[70];
	G4double  rout_refl[70];
	G4double  rin_back[70];

	while (1)
	{
		rin[n]       = r;
		rout[n]      = rin[n] + thick_bulk + thick_refl + thick_btw_refl_back + thick_back;//millimeter;
		rin_back[n]  = rin[n] + thick_bulk + thick_refl + thick_btw_refl_back;//millimeter;
		rout_refl[n] = rin[n] + thick_bulk + thick_refl;
		rin_refl[n]  = rin[n] + thick_bulk;

		if(lc_type==2){
			z[n] = Tsukada_z_from_r(rin[n]);
		}
		else if (lc_type==1){
			z[n] = Winston_z_from_r(rin[n]);
		}
		r = r + 2;//millimeter;
		if (r > rmax){
			n=n-1;
			break;
		}
		G4cout<<"*Tsukada* n= "<<n<<"; r= "<<rin[n]<<"; z= "<<z[n]<<G4endl;
		n++;
	}

	// Setting optical properties.
	const int NUMENTRIES_list = 60;
	if ( op_refl == NULL )
	{
		G4double ENERGY_list[NUMENTRIES_list] =
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

		G4double REF_refl = 0.9;

		G4double REFLECTIVITY_refl[NUMENTRIES_list] =
		{ REF_refl, REF_refl, REF_refl, REF_refl, REF_refl, REF_refl,
			REF_refl, REF_refl, REF_refl, REF_refl, REF_refl, REF_refl,
			REF_refl, REF_refl, REF_refl, REF_refl, REF_refl, REF_refl,
			REF_refl, REF_refl, REF_refl, REF_refl, REF_refl, REF_refl,
			REF_refl, REF_refl, REF_refl, REF_refl, REF_refl, REF_refl,
			REF_refl, REF_refl, REF_refl, REF_refl, REF_refl, REF_refl,
			REF_refl, REF_refl, REF_refl, REF_refl, REF_refl, REF_refl,
			REF_refl, REF_refl, REF_refl, REF_refl, REF_refl, REF_refl,
			REF_refl, REF_refl, REF_refl, REF_refl, REF_refl, REF_refl,
			REF_refl, REF_refl, REF_refl, REF_refl, REF_refl, REF_refl};

		G4double ABS_acryl = 100. *m;

		G4double ABSLENGTH_acryl[NUMENTRIES_list] =
		{ ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl,
			ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl,
			ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl,
			ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl,
			ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl,
			ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl,
			ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl,
			ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl,
			ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl,
			ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl, ABS_acryl};

		G4double RI_acryl = 1.5;//Refractive index.
		G4double RINDEX_acryl[NUMENTRIES_list] =
		{ RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl,
			RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl,
			RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl,
			RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl,
			RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl,
			RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl,
			RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl,
			RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl,
			RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl,
			RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl, RI_acryl};

		G4double REF_back = 0.0;// Reflectivity of mirror back. Implementation of blacksheet is reffered.
		G4double REFLECTIVITY_back[NUMENTRIES_list] =
		{ 0.055*REF_back, 0.055*REF_back,
			0.055*REF_back, 0.055*REF_back, 0.055*REF_back, 0.055*REF_back,
			0.055*REF_back, 0.055*REF_back, 0.055*REF_back, 0.055*REF_back,
			0.055*REF_back, 0.055*REF_back, 0.055*REF_back, 0.055*REF_back,
			0.055*REF_back, 0.055*REF_back, 0.055*REF_back, 0.055*REF_back,
			0.055*REF_back, 0.055*REF_back, 0.055*REF_back, 0.055*REF_back,
			0.055*REF_back, 0.055*REF_back, 0.055*REF_back, 0.055*REF_back,
			0.055*REF_back, 0.055*REF_back, 0.055*REF_back, 0.055*REF_back,
			0.055*REF_back, 0.057*REF_back, 0.059*REF_back, 0.060*REF_back,
			0.059*REF_back, 0.058*REF_back, 0.057*REF_back, 0.055*REF_back,
			0.050*REF_back, 0.045*REF_back, 0.045*REF_back, 0.045*REF_back,
			0.045*REF_back, 0.045*REF_back, 0.045*REF_back, 0.045*REF_back,
			0.045*REF_back, 0.045*REF_back, 0.045*REF_back, 0.045*REF_back,
			0.045*REF_back, 0.045*REF_back, 0.045*REF_back, 0.045*REF_back,
			0.045*REF_back, 0.045*REF_back, 0.045*REF_back, 0.045*REF_back,
			0.045*REF_back, 0.045*REF_back };
		/*
		   op_refl =  new G4OpticalSurface("Mirror_opsurf");
		   op_refl->SetFinish(polishedfrontpainted); 
		   op_refl->SetModel(glisur);
		   op_refl->SetType(dielectric_metal);
		   op_refl->SetPolish(0.999);              
		   */

		op_refl =  new G4OpticalSurface("Mirror_opsurf");
		op_refl->SetType(dielectric_metal);
		op_refl->SetModel(unified);
		//op_refl->SetFinish(polishedfrontpainted); 
		op_refl->SetFinish(polished); 
		op_refl->SetSigmaAlpha(0);

		G4MaterialPropertiesTable* mp_acryl=
			new G4MaterialPropertiesTable();
		mp_acryl->AddProperty("ABSLENGTH", ENERGY_list, ABSLENGTH_acryl, NUMENTRIES_list);
		mp_acryl->AddProperty("RINDEX",    ENERGY_list, RINDEX_acryl,    NUMENTRIES_list);
		ma_bulk->SetMaterialPropertiesTable(mp_acryl);

		G4MaterialPropertiesTable* mp_refl=
			new G4MaterialPropertiesTable();
		mp_refl->AddProperty("REFLECTIVITY", ENERGY_list, REFLECTIVITY_refl, NUMENTRIES_list);
		op_refl->SetMaterialPropertiesTable( mp_refl );

		G4MaterialPropertiesTable* mp_back=
			new G4MaterialPropertiesTable();

		if ( op_back == NULL )
		{
			op_back =  new G4OpticalSurface("Mirror_opsurf_back");
			op_back->SetType(dielectric_metal);
			op_back->SetModel(unified);
			//op_back->SetFinish(polishedfrontpainted); 
			op_back->SetFinish(polished); 
			op_back->SetSigmaAlpha(0);
			mp_back->AddProperty("REFLECTIVITY", ENERGY_list, REFLECTIVITY_back, NUMENTRIES_list);
			op_back->SetMaterialPropertiesTable( mp_back );
		}
	}

	G4cout<<"N= "<<n<<G4endl;

	so_bulk = new G4Polycone(name+"_bulk", 0, 2*M_PI, n, z, rin,      rout);
	so_refl = new G4Polycone(name+"_refl", 0, 2*M_PI, n, z, rin_refl, rout_refl);
	so_back = new G4Polycone(name+"_back", 0, 2*M_PI, n, z, rin_back, rout);


	G4NistManager* man  = G4NistManager::Instance();
	G4Material* ma_refl = man->FindOrBuildMaterial("G4_Al");

	lo_refl  = new G4LogicalVolume(so_refl, ma_refl, name+"_refl");
	lo_back  = new G4LogicalVolume(so_back, G4Material::GetMaterial("Blacksheet"), name+"_back");

	new G4LogicalSkinSurface(name+"_refl_surf", lo_refl, op_refl);
	new G4LogicalSkinSurface(name+"_back_surf", lo_back, op_back);

	this->SetMaterial(ma_bulk);
	this->SetSolid(so_bulk);
	this->SetName(name+"_bulk");
	new G4PVPlacement(0, G4ThreeVector(0, 0, 0), lo_refl, "LightConeWCPMT_refl", this, false, 0, true);
	new G4PVPlacement(0, G4ThreeVector(0, 0, 0), lo_back, "LightConeWCPMT_back", this, false, 0, true);

	/*
	G4VisAttributes* visAtt= new G4VisAttributes(G4Color(1.0,1.0,0.,1.0));
	visAtt->SetForceSolid(true);     
	*/

}

G4double WCSimLC::GetHeight()
{
    if (lc_type_internal==1){
	return Winston_z_from_r(rmax) - Winston_z_from_r(rmin);
	}
	else if (lc_type_internal==2){
	return Tsukada_z_from_r(rmax) - Tsukada_z_from_r(rmin);
	}
	else {
		G4cerr << "something wrong! in WCSimLC::GetHeight()" << G4endl;
		exit;
	}
}

G4double WCSimLC::GetRadius()
{
	return rmax;
}

G4double WCSimLC::GetOffset()
{
	return offset_lc;
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
