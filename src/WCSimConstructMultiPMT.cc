#include "WCSimDetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4Polycone.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4SDManager.hh"
#include "WCSimWCSD.hh"

#include "G4SystemOfUnits.hh"

//PMT logical volume construction.
//A function of the WCSimDetectorConstruction class


// TF: uncomment here or in ConstructPMT??
//WCSimDetectorConstruction::PMTMap_t WCSimDetectorConstruction::PMTLogicalVolumes;

// Options : - PMT type (let's do ONE type per multiPMT), currently represented by radius, but don't like that
//           - expose: how much the individual PMTs stick out of the blacksheet. Let's keep this approach for now and make the surface of our multiPMT a black sheet.
//           - multiPMT options: get them from vis.mac?? orientation array (\vec{r} pointers?), length
// gives number of PMTs?
//           - type of multiPMT object: NO, specify in vis.mac (h == 0 is sphere)
//           - WinstonCone: TODO
// FOR TESTING purposes: HARD CODED in here, later a) in ConstructCylinder, b) in the macro.
// Macro: user can quickly test multiple options, in ConstructCylinder: limit options and control better

G4LogicalVolume* WCSimDetectorConstruction::ConstructMultiPMT(G4double radius,
							      G4double expose)
{
  /*PMTKey_t key(radius,expose);
  
  PMTMap_t::iterator it = PMTLogicalVolumes.find(key);
    if (it != PMTLogicalVolumes.end()) {
    //G4cout << "Restore PMT" << G4endl;
      return it->second;
      }*/
  
  //G4cout << "Create PMT" << G4endl;
  
  // Gray wireframe visual style
  G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
  WCPMTVisAtt->SetForceWireframe(true);

  //G4double sphereRadius = (expose*expose+ radius*radius)/(2*expose);
  //G4double PMTOffset =  sphereRadius - expose;

  //All components of the PMT are now contained in a single logical volume logicWCPMT.
  //Origin is on the blacksheet, faces positive z-direction.
  


  
  //Define a cylinder with spherical top and bottom
  //

  //Option 1: spherical orientation of single PMTs: KM3Net-style, h_cylinder = 0
  //Option 2: cylindrical orientation: PINGU/Mica-style

  G4double cylinder_radius = 166.*mm;  //TODO:Acquire from vis.mac
  G4double cylinder_height = 453.*mm;  //TODO:Acquire from vis.mac
  
  //Defines a cylinder
  G4double mPMT_zRange[2] = {0, cylinder_height+2*expose};
  G4double mPMT_RRange[2] = {cylinder_radius+expose, cylinder_radius+expose};
  G4double mPMT_rRange[2] = {0,0};
  
  //solids
  G4Polycone* mPMT_cylinder = 
    new G4Polycone("WCmPMT_cyl",                    
		   0.0*deg,
		   360.0*deg,
		   2,
		   mPMT_zRange,
		   mPMT_rRange, // R Inner
		   mPMT_RRange);// R Outer

  G4Sphere* mPMT_top_sphere =
    new G4Sphere(    "WCmPMT_tsphere",
		     0.0*m,radius,
		     0.0*deg,360.0*deg,
		     0.0*deg,90.0*deg);

  G4Sphere* mPMT_bottom_sphere =
    new G4Sphere(    "WCmPMT_bsphere",
		     0.0*m,radius,
		     0.0*deg,360.0*deg,
		     90.0*deg,180.0*deg);

  //Add them up:
  G4VSolid* temp_sum =
    new G4UnionSolid("Cyl+TopSphere", mPMT_cylinder, mPMT_top_sphere,
		     0, G4ThreeVector(0,0,cylinder_height/2+expose));

  G4VSolid* solidMultiPMT =
    new G4UnionSolid("WCMultiPMT", temp_sum, mPMT_bottom_sphere,
		     0, G4ThreeVector(0,0,-cylinder_height/2+expose));

  G4LogicalVolume *logicWCMultiPMT =
    new G4LogicalVolume(    solidMultiPMT,
			    G4Material::GetMaterial("Water"),
			    "WCMultiPMT",
			    0,0,0);
  //needs to be rotated by 90deg for a cylinder during placement!!






  
  
  //Now we place the nPMTs inside this motherobject on a sphere
  const G4int nPMTs = 2;//31;
  // in rotated frame!
  G4ThreeVector pos_offset[nPMTs] = {G4ThreeVector(0,0,cylinder_height/2+cylinder_radius),
				     G4ThreeVector(0,0,cylinder_height/2+cylinder_radius)}; //make this more user friendly/defineable later
  std::pair<G4double,G4double> orientation[nPMTs] = {std::pair<G4double, G4double>(180.*deg,0.*deg),
						     std::pair<G4double, G4double>(0.*deg,0.*deg)}; //theta,phi
  
  //Loop over and place the PMTs.
  for(int p = 0; p < nPMTs; p++){
    
    G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;
    WCPMTRotation->setTheta(orientation[p].first);
    WCPMTRotation->setPhi(orientation[p].second);
    
    //position offsets : TODO : where does the line r = r0 + alpha*vec(r) cross the sphere, ie. ==R
    // For alpha = (R-r0)/vec(r)
    //G4ThreeVector pmt_pos = G4ThreeVector(pos_offset[p], yoffset, 0);     
    
    G4LogicalVolume* logicWCPMT = ConstructPMT(radius, expose);
    
    G4VPhysicalVolume* singlePMT =
      new G4PVPlacement(	WCPMTRotation,				// its rotation
				pos_offset[p],				// its position
				logicWCPMT,				// its logical volume
				"WCMultiPMT",				// its name 
				logicWCMultiPMT,			// its mother volume
				false,					// no boolean os
				p);					// every PMT need a unique id.
    
    std::cout << "Placed " << p << std::endl;
  }
  
  
  
  logicWCMultiPMT->SetVisAttributes(WCPMTVisAtt);  //DEBUG
  //logicWCPMT->SetVisAttributes(G4VisAttributes::Invisible); 
  
  /*
    if (!aWCPMT)
    {
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    aWCPMT = new WCSimWCSD( "/WCSim/glassFaceWCPMT",this );
    SDman->AddNewDetector( aWCPMT );
    }
    logicGlassFaceWCPMT->SetSensitiveDetector( aWCPMT );
    
    //PMTLogicalVolumes[key] = logicWCPMT;
    */
  return logicWCMultiPMT;
}
