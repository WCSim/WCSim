
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


using namespace std;

// int Plate_size = 0;//Plate size and dye concentration
//                    //0 gives a light collector that increases the light to the pmt by 30%
//                    //1 gives a light collector that increases the light to the pmt by the maximum amount

G4OpticalSurface*  WCSimWLSP_LC::OpPlasticSurface = NULL;
G4OpticalSurface*  WCSimWLSP_Reflector::OpReflectorSurface = NULL;

WCSimWLSP_LC::WCSimWLSP_LC(
      const G4String &name, 
      G4Material* cone_Material,    
      G4Material* plate_material, 
      int Plate_size    
      ) : G4LogicalVolume(new G4Box("temp",10,10,10), cone_Material, name)
{
  
    LC_z = (0.010/2)*m; //Plate thickness, since cylinders (G4Tubs) are                 	                 
                    //defined by a half height, I divide by 2.
 

     LC_rmin = (10.0 * 0.0254/2)*m; //inner radius of disk, PMT diameter==>Original

    //  LC_rmin = 0.97*(10.0 * 0.0254/2)*m; //optimized==>TO TRY with extended rmax

if (Plate_size == 0)
{
    LC_rmax = ((20.0 * 25.4)/2)*mm; //outer radius of disk, our prototypes are 20" diameter==>Original
 }
 else
   {
     LC_rmax = 1.265*((20.0 * 25.4)/2)*mm; //Extended,max=1.27==>TO TRY with the optimized rmin
   }
 
 LC_pmin = 0.0;    //Starting and ending polar angle for LC
 LC_pmax = twopi;  //For a full disk these need to be 0 and 2pi
 //I have these here so that at some point I can
 //look into LCs with less rotational symmetry.
 //This will give a different set of optical paths               	              
 //for light to reach the PMT.

 bulb_thickness = 0.003*m; //thickness of PMT's glass, estimate
 gap = 0.001*m;
 
 
 
//------------End of WLS Plate Optical Properties-------------------
// The LC
//	
 G4Tubs* LC_tube = new G4Tubs("LC1",LC_rmin,LC_rmax,LC_z,LC_pmin,LC_pmax);
 
 
 SetSolid(LC_tube);
 SetMaterial(plate_material);
 SetName(name+"_plastic");

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
      int Plate_size
      ) : G4LogicalVolume(new G4Box("temp",10,10,10), reflector_Material, name)
{
    LC_z = (0.010/2)*m; //Plate thickness, since cylinders (G4Tubs) are                 	                 
                    //defined by a half height, I divide by 2.

      LC_rmin = (10.0 * 0.0254/2)*m; //inner radius of disk, PMT diameter

if (Plate_size == 0)
{
    LC_rmax = ((20.0 * 25.4)/2)*mm; //outer radius of disk, our prototypes are 20" diameter==>Original
}
else
{
    LC_rmax = 1.265*((20.0 * 25.4)/2)*mm; //Extended,max=1.27==>TO TRY with the optimized rmin
}



  LC_pmin = 0.0;    //Starting and ending polar angle for LC
  LC_pmax = twopi;  //For a full disk these need to be 0 and 2pi
                      //I have these here so that at some point I can
                      //look into LCs with less rotational symmetry.
                      //This will give a different set of optical paths               	              
                     //for light to reach the PMT.

  bulb_thickness = 0.003*m; //thickness of PMT's glass, estimate
  gap = 0.001*m;




 G4Tubs* reflector_tube = new G4Tubs("Reflector",LC_rmax+0.0*mm,LC_rmax+1.0*mm,LC_z,LC_pmin,LC_pmax);

 
 SetSolid(reflector_tube);
 SetMaterial(reflect_Material);
 SetName(name+"_plastic");
 
 
// Optical properties for Tyvek surface

 if (OpReflectorSurface == NULL){

   G4double Rsigma_alpha = 0.2;  //0.2 for tyvek
   //G4OpticalSurface* 
   OpReflectorSurface = new G4OpticalSurface("ReflectorSurface");
      
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
   
   G4MaterialPropertiesTable* ReST1 = new G4MaterialPropertiesTable();
   
   ReST1->AddProperty("RINDEX", REphoton, RRefractiveIndex, Rnum);
   ReST1->AddProperty("SPECULARSPIKECONSTANT", REphoton, RSpecularSpike,   Rnum);
   ReST1->AddProperty("SPECULARLOBECONSTANT", REphoton, RSpecularLobe,   Rnum);
   ReST1->AddProperty("BACKSCATTERCONSTANT",   REphoton, RBackscatter,     Rnum);
   ReST1->AddProperty("REFLECTIVITY", REphoton, RReflectivity, Rnum);
   ReST1->AddProperty("EFFICIENCY", REphoton, REfficiency, Rnum);
   
   OpReflectorSurface->SetMaterialPropertiesTable(ReST1);
 }

G4LogicalSkinSurface* ReflectorSurface = 
   new G4LogicalSkinSurface("WLSP_ReflectorSurface", this, OpReflectorSurface);




}// end of the WCSimWSLP_Reflector Constructor


void WCSimWLSP_Reflector::SetInvisible()
{
  this->SetVisAttributes(G4VisAttributes::Invisible);
 
}
