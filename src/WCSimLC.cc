
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
      G4Material* bulkMaterial    
      ) : G4LogicalVolume(new G4Box("temp",10,10,10), bulkMaterial, name)
{


a = 110*millimeter;   // smallest radius of LC
t = 50*millimeter;
theta = 60*degree;  // or convert as needed
costh = cos(theta);
sinth = sin(theta);
f = 0.5*(t*costh+a*sinth)+0.5*sqrt(a*a+t*t);
 G4cout<<"THETA= "<<theta/degree<<G4endl;
 G4cout<<"F= "<<f<<G4endl;

rmin = a;
rmax = 139*millimeter;
n = 0;

r = rmin;

  G4double  rin[23], rout[23],z[23];

while (r < rmax) {
  rin[n] = r;
  rout[n] = rin[n] + 1*millimeter;
  z[n] = Winston_z_from_r(rin[n]);
  //G4cout<<"n= "<<n<<"; r= "<<rin[n]<<"; z= "<<z[n]<<G4endl;
 
  r = r + 4.*(2.636*millimeter-r*0.0181);
  n++;
}
 
  if ( our_Mirror_opsurf == NULL )        {
    
    our_Mirror_opsurf =  new G4OpticalSurface("Mirror_opsurf");
    our_Mirror_opsurf->SetFinish(polishedfrontpainted); 
    our_Mirror_opsurf->SetModel(glisur);
    our_Mirror_opsurf->SetType(dielectric_metal);
    our_Mirror_opsurf->SetPolish(0.999);              
    G4MaterialPropertiesTable* propMirror=
      new G4MaterialPropertiesTable();
    propMirror->AddProperty("REFLECTIVITY", new G4MaterialPropertyVector());
    propMirror->AddEntry("REFLECTIVITY", twopi*hbarc/(800.0e-9*m), 0.9);
    propMirror->AddEntry("REFLECTIVITY", twopi*hbarc/(200.0e-9*m), 0.9);
    our_Mirror_opsurf->SetMaterialPropertiesTable( propMirror );
  }
 
  G4cout<<"N= "<<n<<G4endl;

 G4Polycone *conc_solid = new G4Polycone(name+"_plastic", 0, 2*pi, n, z, 
					  rin,
					  rout);

SetSolid(conc_solid);
  SetName(name+"_plastic");
  
  new G4LogicalSkinSurface(name+"_conc_surface",this, our_Mirror_opsurf);

 
  G4VisAttributes* visAtt= new G4VisAttributes(G4Color(0.8,0.5,0.5,1.0));
  visAtt->SetForceSolid(true);     
  
}

G4double WCSimLC::GetHeight()
{
    return Winston_z_from_r(rmax) - Winston_z_from_r(rmin);
}

G4double WCSimLC::Winston_z_from_r (double r)
{
  G4double z2;
  
  z2 = (2*f-r*costh)*costh-2*sqrt(f*(f-r*sinth));
  z2 = z2/(sinth*sinth) + t;
  return z2;
}

void WCSimLC::SetInvisible()
{
  this->SetVisAttributes(G4VisAttributes::Invisible);
 
}
