
#include "WCSimLC.hh"

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

G4OpticalSurface*  WCSimLC::our_Mirror_opsurf = NULL;

WCSimLC::WCSimLC(
      const G4String &name, 
      G4Material* bulkMaterial,
      G4double LC_rmin1,
      G4double LC_rmax1,
      G4double LC_a1,
      G4double LC_b1,
      G4double LC_d1,
      G4double LC_refl
      ) : G4LogicalVolume(new G4Box("temp",10,10,10), bulkMaterial, name)
{

double rmin= LC_rmin1;
double rmax= LC_rmax1;
int n = 0;

//parameters of ellipse

 double a,b,c,d;
 a = LC_a1;
 b = LC_b1;
 //c = - 572.848*millimeter;
 d = LC_d1;
 
 //we will set c (z position of ellipse center, by requesting that z = 0 at r_min); it is close to above value, but in this way, the match will be exact
 
 c = -b*sqrt(1 - (rmin-d)*(rmin-d)/(a*a));
 
 double z_test;
 z_test = -c - b*sqrt(1 - (rmin-d)*(rmin-d)/(a*a)); 
 //cout << "z_test = " << z_test << endl;
 
 double r = rmin;

 const int ring_number = 1000;
 
 double rin[ring_number], rout[ring_number],z[ring_number];
 
 while (r < rmax) {
    rin[n] = r;
    rout[n] = rin[n] + 1;
    z[n] = -c - b*sqrt(1 - (r-d)*(r-d)/(a*a)); 
    
    //getting the next r, so that the side of the cone to be built is (1 +/- 0.01) mm
    
    double cone_side_length = 5*millimeter; //mm
    double delta_cone_side_length = 0.01*millimeter;//mm
    
    double r_up, z_up;
    
    r_up = r + 0.01*millimeter; //mm
    
    z_up =  -c - b*sqrt(1 - (r_up-d)*(r_up-d)/(a*a)); 
    
    // cout << "r_up = " << r_up << endl;
    //  cout << "z_up = " << z_up << endl;
    
    double calc_cone_side_length;
    double r_step = 0.001*millimeter; //mm
    calc_cone_side_length = sqrt((r_up - r)*(r_up - r) + (z_up - z[n])*(z_up - z[n])*(z_up - z[n]));
    // cout << "calc_cone_side_length = " << calc_cone_side_length << endl;

   
    while (calc_cone_side_length < cone_side_length) {
      if ((cone_side_length - calc_cone_side_length) > delta_cone_side_length) {
	//cout << "r = " << r << "  r_up = " << r_up << " length_diff = " << (cone_side_length - calc_cone_side_length) << endl;
	
	//new value of r
	r_up += r_step;
	
	//new value of z
	z_up =  -c - b*sqrt(1 - (r_up-d)*(r_up-d)/(a*a));
	
	calc_cone_side_length = sqrt((r_up - r)*(r_up - r) + (z_up - z[n])*(z_up - z[n])*(z_up - z[n]));
	
      }
      else {
	//	cout << "r = " << r << "  r_up = " << r_up << " length_diff = " << (cone_side_length - calc_cone_side_length) << endl;
	break;
      }
    }
      
      
    // r = r + 4.*(2.636-r*0.0181);
    
    //  cout << "r = " << r << endl;
    
    r = r_up;
    
    n++;
  }

  //  cout << "n = " << n << endl;

 if ( our_Mirror_opsurf == NULL )        {
    
    our_Mirror_opsurf =  new G4OpticalSurface("Mirror_opsurf");
    our_Mirror_opsurf->SetFinish(polishedfrontpainted); 
    our_Mirror_opsurf->SetModel(glisur);
    our_Mirror_opsurf->SetType(dielectric_metal);
    our_Mirror_opsurf->SetPolish(0.999);              
    G4MaterialPropertiesTable* propMirror=
      new G4MaterialPropertiesTable();
    propMirror->AddProperty("REFLECTIVITY", new G4MaterialPropertyVector());
    propMirror->AddEntry("REFLECTIVITY", twopi*hbarc/(800.0e-9*m), LC_refl);
    propMirror->AddEntry("REFLECTIVITY", twopi*hbarc/(200.0e-9*m), LC_refl);
    our_Mirror_opsurf->SetMaterialPropertiesTable( propMirror );
  }
 
 // G4cout<<"NEW EXTENDED LC :)) "<<G4endl;

 G4Polycone *conc_solid = new G4Polycone(name+"_plastic", 0, 2*pi, n, z, 
					  rin,
					  rout);

SetSolid(conc_solid);
  SetName(name+"_plastic");
  
  new G4LogicalSkinSurface(name+"_conc_surface",this, our_Mirror_opsurf);

 
  G4VisAttributes* visAtt= new G4VisAttributes(G4Color(0.8,0.5,0.5,1.0));
  visAtt->SetForceSolid(true);     
  
}


void WCSimLC::SetInvisible()
{
  this->SetVisAttributes(G4VisAttributes::Invisible);
 
}
