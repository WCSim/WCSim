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

#include  "WCSimMultiPMTParameterisation.hh"
#include  "G4PVParameterised.hh"

//TODO: move
#include "G4NistManager.hh"

//PMT logical volume construction.
//A function of the WCSimDetectorConstruction class


// TF: uncomment here or in ConstructPMT??
//WCSimDetectorConstruction::PMTMap_t WCSimDetectorConstruction::PMTLogicalVolumes;

// Options : - PMT type (let's do ONE type per multiPMT), currently represented by radius, but don't like that
//           - expose: how much the individual PMTs stick out of the blacksheet. Let's keep this approach for now and make the surface of our multiPMT a black sheet.
//           - type of multiPMT object: NO, specify in vis.mac (h == 0 is sphere)
//           - WinstonCone: TODO

G4LogicalVolume* WCSimDetectorConstruction::ConstructMultiPMT(G4double pmt_radius, //TF: both args are PMT properties, used by ConstructPMT and should be replaced by PMTtype
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
  //WCPMTVisAtt->SetForceWireframe(true);
  WCPMTVisAtt->SetForceSolid(true); //DEBUG

  //All components of the PMT are now contained in a single logical volume logicWCPMT.
  //Origin is on the blacksheet, faces positive z-direction.
  
  //Define a cylinder with spherical top and bottom
  
  G4double mPMT_zRange[2] = {0, cylinder_height};
  G4double mPMT_RRange[2] = {cylinder_radius+expose, cylinder_radius+expose};
  //G4double mPMT_rRange[2] = {0,0};
  //NEW: only outer shell as mother volume for parametrization, as I want to put a black sheet in there
  G4double mPMT_rRange[2] = {cylinder_radius,cylinder_radius}; //-1mm??

  //solids
  // origin is bottom of upward cylinder
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
		     //		     0.0*m,cylinder_radius+expose,
		     cylinder_radius,cylinder_radius+expose, // NEW
		     0.0*deg,360.0*deg,
		     0.0*deg,90.0*deg);

  // gf: I need a logical volume for the parametrisation; 
  // I'll need a logical volume also for mPMT_bottom_sphere
  //G4LogicalVolume*  LogicmPMT_top_sphere=new G4LogicalVolume(mPMT_top_sphere, G4Material::GetMaterial("Blacksheet"),"top_sphere",0,0,0);
  //G4VisAttributes* WCPMTVisAtt3 = new G4VisAttributes(G4Colour(0,0.,1.));
  //WCPMTVisAtt3->SetForceSolid(true); //DEBUG
  //LogicmPMT_top_sphere->SetVisAttributes(WCPMTVisAtt3);  //DEBUG
  // end gf


  G4Sphere* mPMT_bottom_sphere =
    new G4Sphere(    "WCmPMT_bsphere",
		     //		     0.0*m,cylinder_radius+expose,
		     cylinder_radius,cylinder_radius+expose,
		     0.0*deg,360.0*deg,
		     90.0*deg,180.0*deg);

  //Add them up:
  G4VSolid* temp_sum =
    //G4VSolid* solidMultiPMT =
    new G4UnionSolid("Cyl+TopSphere", mPMT_cylinder, mPMT_top_sphere,
		     0, G4ThreeVector(0,0,cylinder_height));


  G4VSolid* solidMultiPMT =
    new G4UnionSolid("WCMultiPMT", temp_sum, mPMT_bottom_sphere);

  G4NistManager *nist_man = G4NistManager::Instance(); //TODO: MOVE to ConstructMaterials
  G4LogicalVolume *logicWCMultiPMT =
    new G4LogicalVolume(    solidMultiPMT,
			    nist_man->FindOrBuildMaterial("G4_PLEXIGLASS"), //Acrylic
			    //G4Material::GetMaterial("Water"), //TODO: define through material_outer
			    "WCMultiPMT",
			    0,0,0);


  ////////////////////////////////////////////////////////////////////////////////////
  /*
    
  //Now we place the nPMTs inside this motherobject on a sphere
  const G4int nPMTs = 2;//31;
  // in rotated frame!
  G4ThreeVector pos_offset[nPMTs] = {G4ThreeVector(0,0,-cylinder_radius),
				     G4ThreeVector(0,0,cylinder_height+cylinder_radius)}; //make this more user friendly/defineable later
  std::pair<G4double,G4double> orientation[nPMTs] = {std::pair<G4double, G4double>(180.*deg,0.*deg),
						     std::pair<G4double, G4double>(0.*deg,0.*deg)}; //theta,phi
  
  //Loop over and place the PMTs.
  for(int p = 0; p < nPMTs; p++){
    
    G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;
    WCPMTRotation->setTheta(orientation[p].first);
    WCPMTRotation->setPhi(orientation[p].second);
    
    G4LogicalVolume* logicWCPMT = ConstructPMT(radius, expose);
    
    //G4VisAttributes* WCsPMTVisAtt = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
    //WCsPMTVisAtt->SetForceWireframe(true);
    //logicWCPMT->SetVisAttributes(WCsPMTVisAtt);

    G4VPhysicalVolume* singlePMT =
      new G4PVPlacement(	WCPMTRotation,				// its rotation
				pos_offset[p],				// its position
				logicWCPMT,				// its logical volume
				"WCPMT",				// its name 
				logicWCMultiPMT,			// its mother volume
				false,					// no boolean os
				p);					// every PMT need a unique id.
    
  }
  

  */  
  // gf
  //WCSimMultiPMTParameterisation* WCSimMultiPMTParameterisation_par;
  
  // for circle 1
  G4int NbOfPmt = 18;//8;//13;
  //G4int NbOfPmt = 4; //debug
  G4double pmtDistance = cylinder_radius; // Inner radius od the DOM 
  G4cout << "Distance from the Z axis = " <<  pmtDistance << " mm" << G4endl;

  // TF, well this is the arg of ConstructMultiPMT, which should be replaced by PMTtype
  //which should actually be set in the macro file, so this should be a void function call.
  G4LogicalVolume* logicWCPMT = ConstructPMT(pmt_radius, expose);

  //The ConstructMultiPMT function gets called multiple times, so only fill the vectors when not empty.
  G4int NbOfTotPmt = 0;
  if(vNiC.size() > 0){
    //TODO: Could also store this value, as vNiC and vAlpha are members of DetectorConfiguration class
    for(int i = 0; i < vNiC.size();i++){
      NbOfTotPmt+=vNiC[i];
    }
  }else
    NbOfTotPmt = CountPMT(NbOfPmt); // Total number of PMTs in the circles, defines vNiC and vAlpha
  
  //Fill top sphere (or let the parametrisation figure out the whole filling? Yes!)
  G4VPVParameterisation* pmtParam_id =
    new WCSimMultiPMTParameterisation(
				      NbOfPmt,	        // NoPMTs
				      pmtDistance,	// Z spacing of centers
				      vNiC,		// Number of PMTs in each circle
				      vAlpha,		// Tilt angle for each circle
				      vCircle,          // Circle number
				      cylinder_height);	// For position offsets of circles, should become array!!!! TODO, for bottom sphere
  
  // dummy value : kZAxis -- modified by parameterised volume
  
  // Mother Volume can only have ONE parametrization, hence need to cut in pieces OR parametrize all at once
  // TF prefers the latter.
  G4VPhysicalVolume* hemisphere = 
    new G4PVParameterised("pmt",                  // their name
			  logicWCPMT,             // their logical volume
			  logicWCMultiPMT,        // Mother logical volume
			  kZAxis,                 // Are placed along this axis
			  NbOfTotPmt,             // Number of PMTs (replica's)
			  pmtParam_id,            // The parametrisation
			  false);                  // checking overlaps
  
  // Need a 5mm tolerance
  if(hemisphere->CheckOverlaps(1000,5,1)){
    G4cout << "Hemisphere has overlapping PMTs: Retry" << G4endl;
    G4LogicalVolume* emptyLogic;
    return emptyLogic;
  }
    
  


  // end gf



  /////////////////////////////////////////////////////////////////////
  // NEW : cover the surface with blacksheet, or something else
  /// TODO: Cover bottom circle (<theta_min) with absorbing black felt)?
  //////////
  
  /////
  //Defines a cylinder
  //G4double mPMT_zRange[2] = {0, cylinder_height};
  G4double thickness = 2.*mm; //a 2 mm thick blacksheet or whatever...
  G4double inner_RRange[2] = {cylinder_radius, cylinder_radius};
  G4double inner_rRange[2] = {cylinder_radius-thickness,cylinder_radius-thickness};
  
  //solids
  // origin is bottom of upward cylinder
  G4Polycone* inner_cylinder = 
    new G4Polycone("inn_cyl",                    
		   0.0*deg,
		   360.0*deg,
		   2,
		   mPMT_zRange,
		   inner_rRange, // R Inner
		   inner_RRange);// R Outer
  
  G4Sphere* inner_top_sphere =
    new G4Sphere(    "WCmPMT_tsphere",
		     cylinder_radius-thickness,cylinder_radius,
		     0.0*deg,360.0*deg,
		     0.0*deg,90.0*deg);

  G4Sphere* inner_bottom_sphere =
    new G4Sphere(    "WCmPMT_bsphere",
		     cylinder_radius-thickness,cylinder_radius,
		     0.0*deg,360.0*deg,
		     90.0*deg,180.0*deg);

  //Add them up:
  G4VSolid* inner_temp_sum =
    //G4VSolid* solidMultiPMT =
    new G4UnionSolid("inner_Cyl+TopSphere", inner_cylinder, inner_top_sphere,
		     0, G4ThreeVector(0,0,cylinder_height));


  G4VSolid* solidMultiPMT_inner =
    new G4UnionSolid("WCMultiPMT_inner", inner_temp_sum, inner_bottom_sphere);
  G4LogicalVolume *logicWCMultiPMT_inner =
    new G4LogicalVolume(    solidMultiPMT_inner,
			    G4Material::GetMaterial("Blacksheet"), //TODO: define through material_inner
			    "WCMultiPMT_inner",
			    0,0,0);
  
  G4VisAttributes* WCPMTVisAtt2 = new G4VisAttributes(G4Colour(1.0,0.,0.));
  WCPMTVisAtt2->SetForceSolid(true); //DEBUG
  logicWCMultiPMT_inner->SetVisAttributes(WCPMTVisAtt2);  //DEBUG
  
  /*
  G4VPhysicalVolume* place_inner =
    new G4PVPlacement(	0,				// its rotation
			G4ThreeVector(0,0,0),		// its position
			logicWCMultiPMT_inner,   	// its logical volume
			"WCPMT_inner",				// its name 
			logicWCMultiPMT,			// its mother volume
			false,					// no boolean os
			0);					// every PMT need a unique id.
  */
  
  
  //logicWCMultiPMT->SetVisAttributes(WCPMTVisAtt);  //DEBUG



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



G4int WCSimDetectorConstruction::CountPMT(G4int NoPmt)
{

  G4double fEta = ComputeEta(NoPmt); // PMT viewing angle
  
  // First element of vNiC and vAlpha vectors is added manually
  // to allow recursive calculation of the following
  // TF: not needed:
  /*
  vNiC.push_back(NoPmt);
  vNiC.push_back(NoPmt);
  vAlpha.push_back(-fEta);
  */

  G4int NoCircle = 1;
  G4double alphaNext = fEta;  //+offset;
  G4double alphaPrev = alphaNext;
  G4double NiCNext = NoPmt;

  // Remove the ones where alpha is below the minimum (to look over neighbouring mPMTs)
  // Theta_min = eta + atan(R/r) with R radius of sphere, and r the distance between mPMTs
  G4double theta_min = 13.*pi/180;
  if(alphaNext > theta_min + fEta){
    vAlpha.push_back(alphaNext);
    vNiC.push_back(NoPmt);
    G4cout << "Circle n. " << NoCircle << " - Number of PMTs: " << vNiC[NoCircle-1] << " - alpha: " << vAlpha[NoCircle-1]*180/3.141592 << " - eta " << fEta*180/3.141592 << G4endl;
    NoCircle++;
  }
    
  // Recursive calculation of vector contents
  do
    {
      alphaNext = ComputeAlpha(alphaPrev, fEta);//ComputeAlpha(vAlpha[NoCircle-1], fEta);
      NiCNext = ComputeNiC(alphaNext, fEta);
      if(NiCNext<2){break;} // Prevents negative values of NiC
      if(alphaNext > theta_min +fEta){
	vAlpha.push_back(alphaNext);
	vNiC.push_back(NiCNext);
	G4cout << "Circle n. " << NoCircle << " - Number of PMTs: " << vNiC[NoCircle-1] << " - alpha: " << vAlpha[NoCircle-1]*180/3.141592 << " - eta " << fEta*180/3.141592 << G4endl;
	NoCircle++;
      }
      alphaPrev = alphaNext;
    } while(NiCNext > 2); // No less than 3 detectors in each circle

  // TF: If possible: add top:
  if(vAlpha[NoCircle-1]+2*fEta < 90.){
    alphaNext = pi/2;
    NiCNext = 1;
    vAlpha.push_back(alphaNext);
    vNiC.push_back(NiCNext);
    G4cout << "Circle n. " << NoCircle << " - Number of PMTs: " << vNiC[NoCircle-1] << " - alpha: " << vAlpha[NoCircle-1]*180/3.141592 << " - eta " << fEta*180/3.141592 << G4endl;
    NoCircle++;  
  }
  
  

  // Total number of pmt in the circles
  G4int TotPmt = 0;
  for(int i = 0; i < vNiC.size();i++){
    TotPmt+=vNiC[i];
    for(int j = 0; j < vNiC[i]; j++){
      vCircle.push_back(i); //number circles internally between 0 and N-1
    }
  }

  


  G4cout << "Total number of pmt: " << TotPmt << G4endl;
  G4cout << "Percentage of covered hemispherical surface = " << TotPmt*(1-std::cos(fEta))*100 << "%" << G4endl;
  G4cout << "Percentage of covered hemispherical surface above theta_min = " << TotPmt*(1-std::cos(fEta))*100/(1-std::cos(pi/2-theta_min)) << "%" << G4endl;
  G4cout << "Test: vNic: " << vNiC.size() << " vAlpha: " << vAlpha.size() << " vCircle: " << vCircle.size() << G4endl;

  return TotPmt;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double WCSimDetectorConstruction::ComputeEta(G4int NoPmt)
{
  G4double cospi = std::cos(pi/NoPmt);
  G4double etaLocal = std::atan(std::sqrt(1-std::pow(cospi,2)));
  return etaLocal;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double WCSimDetectorConstruction::ComputeAlpha (G4double alphaOfPrevC, G4double Eta)
{
  G4double alphaLocal = alphaOfPrevC + 2*Eta;
  return alphaLocal;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int WCSimDetectorConstruction::ComputeNiC (G4double alphaOfCircle, G4double Eta)
{
  G4double	cos2eta	  = std::pow(std::cos(Eta),2);
  G4double	sin2alpha = std::pow(std::sin(alphaOfCircle),2);
  G4double	cosalpha  = std::cos(alphaOfCircle);
  G4double	arccos	  = std::acos(std::sqrt(cos2eta-sin2alpha)/cosalpha);
  G4int		NiCLocal  = std::floor(pi/arccos);
  return NiCLocal;
}
