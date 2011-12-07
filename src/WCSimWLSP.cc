
#include "WCSimWLSP.hh"

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
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4EllipticalTube.hh"


using namespace std;


G4OpticalSurface*  WCSimWLSP_LC::OpPlasticSurface = NULL;
G4OpticalSurface*  WCSimWLSP_Reflector::OpReflectorSurface = NULL;

WCSimWLSP_LC::WCSimWLSP_LC(
      const G4String &name, 
      G4Material* cone_Material,    
      G4Material* plate_material, 
      //    int Plate_size,
      G4double LC_rmin1,
      G4double LC_rmax1,
      G4String p_shape
      ) : G4LogicalVolume(new G4Box("temp",10,10,10), cone_Material, name)
{
  
    LC_z = (0.010/2)*m; //Plate thickness, since cylinders (G4Tubs) are                 	                 
                    //defined by a half height, I divide by 2.
 
    LC_rmin = LC_rmin1;
    LC_rmax = LC_rmax1;
      
    
 
 LC_pmin = 0.0;    //Starting and ending polar angle for LC
 LC_pmax = twopi;  //For a full disk these need to be 0 and 2pi

 bulb_thickness = 0.003*m; //thickness of PMT's glass, estimate
 gap = 0.001*m;
 
 
 
//------------End of WLS Plate Optical Properties-------------------
// The LC
//	

if(p_shape == "circle")
{
 G4Tubs* LC_tube = new G4Tubs("LC1",LC_rmin,LC_rmax,LC_z,LC_pmin,LC_pmax);

 SetSolid(LC_tube);
 SetMaterial(plate_material);
 SetName(name+"_plastic");
}
if(p_shape == "clover")
{
G4double minor = 2.54* 17.0/2.0 * cm;
G4EllipticalTube* ellip_tube = new G4EllipticalTube("LC1", LC_rmax, minor,LC_z);
G4Tubs* disk = new G4Tubs("disk",0,LC_rmin,LC_z+2.0*mm,LC_pmin,LC_pmax);
G4RotationMatrix* rotat = new G4RotationMatrix;
rotat-> rotateZ(90.0*deg);
G4ThreeVector trans(0.0,0.0,0.0);

G4UnionSolid* ellipses_tube = new G4UnionSolid("2plates",ellip_tube,ellip_tube,rotat,trans);

G4SubtractionSolid* LC_tube = new G4SubtractionSolid("Plate",ellipses_tube,disk);

 SetSolid(LC_tube);
 SetMaterial(plate_material);
 SetName(name+"_plastic");
}
if(p_shape == "square")
{
G4Tubs* disk = new G4Tubs("disk",0,LC_rmin,LC_z+2.0*mm,0,twopi);
G4Box* B = new G4Box("boxes",LC_rmax,LC_rmax,LC_z);
G4SubtractionSolid* LC_tube = new G4SubtractionSolid("Plate",B,disk);

 SetSolid(LC_tube);
 SetMaterial(plate_material);
 SetName(name+"_plastic");
}
 


//	------------- Surfaces --------------
//
// LC
//
//  G4OpticalSurface* 
 OpPlasticSurface = new G4OpticalSurface("PlasticSurface");
 OpPlasticSurface->SetType(dielectric_dielectric);
 OpPlasticSurface->SetFinish(polished);
 OpPlasticSurface->SetModel(glisur);
 

 //  G4LogicalBorderSurface* PlasticSurface = new G4LogicalBorderSurface("WLSP_PlasticSurface",LC_phys,expHall_phys,OpPlasticSurface); //===>moved to constructWC.cc


}// end of the WCSimWSLP_LC Constructor


void WCSimWLSP_LC::SetInvisible()
{
  this->SetVisAttributes(G4VisAttributes::Invisible);
 
}

WCSimWLSP_Reflector::WCSimWLSP_Reflector(
      const G4String &name, 
      G4Material* reflector_Material,
      G4Material* reflect_Material,
      // int Plate_size
      G4double LC_rmax1,
      G4String p_shape
      ) : G4LogicalVolume(new G4Box("temp",10,10,10), reflector_Material, name)
{
  LC_z = (0.010/2)*m; //Plate thickness, since cylinders (G4Tubs) are                 	                 
                    //defined by a half height, I divide by 2.

  LC_rmax = LC_rmax1;
  LC_pmin = 0.0;    //Starting and ending polar angle for LC
  LC_pmax = twopi;  //For a full disk these need to be 0 and 2pi

  bulb_thickness = 0.003*m; //thickness of PMT's glass, estimate
  gap = 0.001*m;

G4double thickness = 1.0*mm;

G4RotationMatrix* rotat = new G4RotationMatrix;
rotat-> rotateZ(90.0*deg);
G4ThreeVector trans(0.0,0.0,0.0);


if(p_shape == "circle")
{
 G4Tubs* reflector_tube = new G4Tubs("Reflector",LC_rmax+0.0*mm,LC_rmax+1.0*mm,LC_z,LC_pmin,LC_pmax);

 SetSolid(reflector_tube);
 SetMaterial(reflect_Material);
 SetName(name+"_plastic");
}
if(p_shape == "clover")
{
G4double minor = 2.54* 17.0/2.0 * cm;
G4EllipticalTube* ellip_tube = new G4EllipticalTube("LC1", LC_rmax, minor,LC_z+thickness);

G4UnionSolid* ellipses_tube = new G4UnionSolid("2plates",ellip_tube,ellip_tube,rotat,trans);


G4EllipticalTube* reflect_ellip = new G4EllipticalTube("ref1", LC_rmax + thickness, minor+ thickness,LC_z);

G4UnionSolid* reflect_ellipses = new G4UnionSolid("ref2",reflect_ellip,reflect_ellip,rotat,trans);

G4SubtractionSolid* reflector_tube = new G4SubtractionSolid("Reflector",reflect_ellipses,ellipses_tube);

 SetSolid(reflector_tube);
 SetMaterial(reflect_Material);
 SetName(name+"_plastic");

}
if(p_shape == "square")
{
G4Box* B = new G4Box("boxes",LC_rmax,LC_rmax,LC_z+thickness);
G4Box* Br = new G4Box("boxesr",LC_rmax+thickness,LC_rmax+thickness,LC_z);
G4SubtractionSolid* reflector_tube = new G4SubtractionSolid("Plater",Br,B);

 SetSolid(reflector_tube);
 SetMaterial(reflect_Material);
 SetName(name+"_plastic");
}
 
// Optical properties for Tyvek surface

//if (OpReflectorSurface == NULL){

   G4double Rsigma_alpha = 0.2;  //0.2 for tyvek
   //G4OpticalSurface* 
   OpReflectorSurface = new G4OpticalSurface("ReflectorSurface");
     
   G4MaterialPropertiesTable* ReST1 = new G4MaterialPropertiesTable();

if(p_shape == "circle")
{
   OpReflectorSurface->SetType(dielectric_metal);
   OpReflectorSurface->SetFinish(ground);
   OpReflectorSurface->SetSigmaAlpha(Rsigma_alpha);
   OpReflectorSurface->SetModel(unified);
   
   const G4int Rnum = 2;
   G4double REphoton[Rnum] = {1.0*eV, 7.0*eV};
   
   //Reflector Surface 
   G4double RRefractiveIndex[Rnum] = {1.5, 1.5};
   G4double RSpecularLobe[Rnum] = {0.2,0.2};  //0.2 for tyvek in water
   G4double RSpecularSpike[Rnum]   = {0.0, 0.0};
   G4double RBackscatter[Rnum]     = {0.0, 0.0};
   G4double RReflectivity[Rnum] = {0.9, 0.9};
   G4double REfficiency[Rnum] = {0.1, 0.1};

   ReST1->AddProperty("RINDEX", REphoton, RRefractiveIndex, Rnum);
   ReST1->AddProperty("SPECULARSPIKECONSTANT", REphoton, RSpecularSpike,   Rnum);
   ReST1->AddProperty("SPECULARLOBECONSTANT", REphoton, RSpecularLobe,   Rnum);
   ReST1->AddProperty("BACKSCATTERCONSTANT",   REphoton, RBackscatter,     Rnum);
   ReST1->AddProperty("REFLECTIVITY", REphoton, RReflectivity, Rnum);
   ReST1->AddProperty("EFFICIENCY", REphoton, REfficiency, Rnum);

}else if(p_shape == "clover")
{
   OpReflectorSurface->SetType(dielectric_metal);
   OpReflectorSurface->SetFinish(polished);
   OpReflectorSurface->SetModel(unified);
   
   const G4int Rnum = 2;
   G4double REphoton[Rnum] = {1.0*eV, 7.0*eV};
   
   //Reflector Surface 
   G4double RRefractiveIndex[Rnum] = {1.5, 1.5};
   G4double RSpecularLobe[Rnum] = {0.0,0.0};  //0.2 for tyvek in water
   G4double RSpecularSpike[Rnum]   = {1.0, 1.0};
   G4double RBackscatter[Rnum]     = {0.0, 0.0};
   G4double RReflectivity[Rnum] = {0.9, 0.9};
   G4double REfficiency[Rnum] = {0.1, 0.1};

   ReST1->AddProperty("RINDEX", REphoton, RRefractiveIndex, Rnum);
   ReST1->AddProperty("SPECULARSPIKECONSTANT", REphoton, RSpecularSpike,   Rnum);
   ReST1->AddProperty("SPECULARLOBECONSTANT", REphoton, RSpecularLobe,   Rnum);
   ReST1->AddProperty("BACKSCATTERCONSTANT",   REphoton, RBackscatter,     Rnum);
   ReST1->AddProperty("REFLECTIVITY", REphoton, RReflectivity, Rnum);
   ReST1->AddProperty("EFFICIENCY", REphoton, REfficiency, Rnum);

}else if(p_shape == "square")
{
   OpReflectorSurface->SetType(dielectric_metal);
   OpReflectorSurface->SetFinish(polished);
   OpReflectorSurface->SetModel(unified);
   
   const G4int Rnum = 2;
   G4double REphoton[Rnum] = {1.0*eV, 7.0*eV};
   
   //Reflector Surface 
   G4double RRefractiveIndex[Rnum] = {1.5, 1.5};
   G4double RSpecularLobe[Rnum] = {0.0,0.0};  //0.2 for tyvek in water
   G4double RSpecularSpike[Rnum]   = {1.0, 1.0};
   G4double RBackscatter[Rnum]     = {0.0, 0.0};
   G4double RReflectivity[Rnum] = {0.9, 0.9};
   G4double REfficiency[Rnum] = {0.1, 0.1};

   ReST1->AddProperty("RINDEX", REphoton, RRefractiveIndex, Rnum);
   ReST1->AddProperty("SPECULARSPIKECONSTANT", REphoton, RSpecularSpike,   Rnum);
   ReST1->AddProperty("SPECULARLOBECONSTANT", REphoton, RSpecularLobe,   Rnum);
   ReST1->AddProperty("BACKSCATTERCONSTANT",   REphoton, RBackscatter,     Rnum);
   ReST1->AddProperty("REFLECTIVITY", REphoton, RReflectivity, Rnum);
   ReST1->AddProperty("EFFICIENCY", REphoton, REfficiency, Rnum);
}else
{
   OpReflectorSurface->SetType(dielectric_metal);
   OpReflectorSurface->SetFinish(polished);
   OpReflectorSurface->SetModel(unified);
   
   const G4int Rnum = 2;
   G4double REphoton[Rnum] = {1.0*eV, 7.0*eV};
   
   //Reflector Surface 
   G4double RRefractiveIndex[Rnum] = {1.5, 1.5};
   G4double RSpecularLobe[Rnum] = {0.0,0.0};  //0.2 for tyvek in water
   G4double RSpecularSpike[Rnum]   = {1.0, 1.0};
   G4double RBackscatter[Rnum]     = {0.0, 0.0};
   G4double RReflectivity[Rnum] = {0.9, 0.9};
   G4double REfficiency[Rnum] = {0.1, 0.1};

   ReST1->AddProperty("RINDEX", REphoton, RRefractiveIndex, Rnum);
   ReST1->AddProperty("SPECULARSPIKECONSTANT", REphoton, RSpecularSpike,   Rnum);
   ReST1->AddProperty("SPECULARLOBECONSTANT", REphoton, RSpecularLobe,   Rnum);
   ReST1->AddProperty("BACKSCATTERCONSTANT",   REphoton, RBackscatter,     Rnum);
   ReST1->AddProperty("REFLECTIVITY", REphoton, RReflectivity, Rnum);
   ReST1->AddProperty("EFFICIENCY", REphoton, REfficiency, Rnum);
}

   OpReflectorSurface->SetMaterialPropertiesTable(ReST1);
// }

G4LogicalSkinSurface* ReflectorSurface = 
   new G4LogicalSkinSurface("WLSP_ReflectorSurface", this, OpReflectorSurface);




}// end of the WCSimWSLP_Reflector Constructor


void WCSimWLSP_Reflector::SetInvisible()
{
  this->SetVisAttributes(G4VisAttributes::Invisible);
 
}

