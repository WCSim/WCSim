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


//mPMT logical volume construction.
//A function of the WCSimDetectorConstruction class


// Keep track of created logical volumes (PMTs and mPMTs).
// If already created, call from map!
WCSimDetectorConstruction::PMTMap_t WCSimDetectorConstruction::PMTLogicalVolumes;

// Options: - PMT type (let's do ONE type per multiPMT)
//          - expose: how much the individual PMTs stick out of the blacksheet. 
//            Let's keep this approach for now and make the surface of our multiPMT a black sheet.
//          - type of multiPMT object: NO, specify in vis.mac (h == 0 is sphere)
//          - WinstonCone: NO, specify in vis.mac

// For MultiPMT: expose have different meaning, namely where to locate the sphere on the blacksheet ID/OD separator
//TF: both args are PMT properties, used by ConstructPMT and should be replaced by PMTtype
G4LogicalVolume* WCSimDetectorConstruction::ConstructMultiPMT(G4String PMTName, G4String CollectionName)
{
  

  // For default SK-style without cover.
  // Not possible to have a mPMT without cover now/yet.
  if(cylinder_radius <= 1.*CLHEP::mm && cylinder_height <= 1.*CLHEP::mm){
    return ConstructPMT(PMTName, CollectionName);
  }


  //unique key for mPMT object. Also single PMT with pressure vessel!
  PMTKey_t key(mPMT_ID_PMT + mPMT_OD_PMT,CollectionName);   
  
  // Return pre-created mPMT Logical Volume if it already exists.
  PMTMap_t::iterator it = PMTLogicalVolumes.find(key);
  if (it != PMTLogicalVolumes.end()) {
    //G4cout << "Restore PMT" << G4endl;
    return it->second;
  }
  
  G4cout << "Create mPMT" << G4endl;

  //All components of the PMT are now contained in a single logical volume logicWCPMT.
  //Origin is on the blacksheet, faces positive z-direction.

  WCSimPMTObject *PMT = GetPMTPointer(CollectionName);
  G4double expose =  PMT->GetExposeHeight(); // THIS is the PMT expose, which I'm currently using for pressure vessel construction
  // this is NOT the "expose" of the mPMT

  ////////////////////////////////////////////////////
  /// 1) Outer logical volume: fill with water     ///
  /// (cfr. same starting point as ConstructPMT)   ///
  ////////////////////////////////////////////////////

  // origin on blacksheet wall is origin of spherical top for ID, 
  // change if we want to use the cylindrical part in ID.
  G4double dist_PMTglass_innerPressureVessel = 2.5*CLHEP::mm; // from KM3Net tech drawing. Don't put PMT directly against pressure vessel. Gel helps transition in refractive indices.
  G4double mPMT_zRange_outer[2] = {-cylinder_height - cylinder_radius - mPMT_outer_material_d - expose - dist_PMTglass_innerPressureVessel, 
				   cylinder_radius + mPMT_outer_material_d + expose
				   + dist_PMTglass_innerPressureVessel};
  G4double mPMT_RRange_outer[2] = {cylinder_radius + expose + mPMT_outer_material_d
				   + dist_PMTglass_innerPressureVessel, 
				   cylinder_radius + expose + mPMT_outer_material_d 
				   + dist_PMTglass_innerPressureVessel};
  G4double mPMT_rRange_outer[2] = {0., 0.};
  
  G4Polycone* solidMultiPMT = 
    new G4Polycone("WCmPMT",                    
		   0.0*deg,
		   360.0*deg,
		   2,
		   mPMT_zRange_outer,
		   mPMT_rRange_outer, // R Inner
		   mPMT_RRange_outer);// R Outer
  
  G4LogicalVolume *logicWCMultiPMT =
    new G4LogicalVolume(    solidMultiPMT,
			    G4Material::GetMaterial("Water"), 
			    "WCMultiPMT",
			    0,0,0);



  ////////////////////////////////////////////////
  /// 2)Acrylic/Glass shell of the mPMT vessel ///
  ////////////////////////////////////////////////

  G4double mPMT_zRange_vessel[2] = {0, cylinder_height};
  G4double mPMT_RRange_vessel[2] = {cylinder_radius + expose + mPMT_outer_material_d 
				    + dist_PMTglass_innerPressureVessel, 
				    cylinder_radius + expose + mPMT_outer_material_d
				    + dist_PMTglass_innerPressureVessel};
  G4double mPMT_rRange_vessel[2] = {cylinder_radius + expose + dist_PMTglass_innerPressureVessel, 
				    cylinder_radius + expose + dist_PMTglass_innerPressureVessel};

  //Define a cylinder with spherical top and bottom
  // origin is bottom of upward cylinder
  G4Polycone* mPMT_cylinder_vessel = 
    new G4Polycone("WCmPMT_vessel",                    
		   0.0*deg,
		   360.0*deg,
		   2,
		   mPMT_zRange_vessel,
		   mPMT_rRange_vessel, // R Inner
		   mPMT_RRange_vessel);// R Outer

  G4Sphere* mPMT_top_sphere_vessel =
    new G4Sphere(    "WCmPMT_tsphere_vessel",
		     cylinder_radius + expose + dist_PMTglass_innerPressureVessel,
		     cylinder_radius + expose + dist_PMTglass_innerPressureVessel + mPMT_outer_material_d, 
		     0.0*deg,360.0*deg,
		     0.0*deg,90.0*deg);


  G4Sphere* mPMT_bottom_sphere_vessel =
    new G4Sphere(    "WCmPMT_bsphere_vessel",
		     cylinder_radius + expose + dist_PMTglass_innerPressureVessel, 
		     cylinder_radius + expose + dist_PMTglass_innerPressureVessel + mPMT_outer_material_d,
		     0.0*deg,360.0*deg,
		     90.0*deg,180.0*deg);

  //Add them up:
  G4VSolid* temp_sum_vessel =
    new G4UnionSolid("Cyl+TopSphere_vessel", mPMT_cylinder_vessel, mPMT_top_sphere_vessel,
		     0, G4ThreeVector(0,0,cylinder_height));


  G4VSolid* solidMultiPMT_vessel =
    new G4UnionSolid("WCMultiPMT_vessel", temp_sum_vessel, mPMT_bottom_sphere_vessel);

  //Acrylic/Water cover of the mPMT
  G4LogicalVolume *logicWCMultiPMT_vessel =
    new G4LogicalVolume(    solidMultiPMT_vessel,
			    G4Material::GetMaterial(mPMT_outer_material),//"G4_PLEXIGLASS"), 
			    "WCMultiPMT_vessel",
			    0,0,0);
  
  G4VPhysicalVolume* place_vessel =
    new G4PVPlacement(	0,				         // its rotation
			G4ThreeVector(0,0,-cylinder_height),	 // its position
			logicWCMultiPMT_vessel,   	         // its logical volume
			"WCPMT_vessel",				 // its name 
			logicWCMultiPMT,			 // its mother volume
			false,					 // no boolean os
			0, 
			checkOverlaps);			

  /////////////////////////////////////////////////////////////////////////
  /// 3) This is the area between the outer shell and the inner shell.  ///
  ///    In this space, the PMTs will live.                             ///
  /////////////////////////////////////////////////////////////////////////

  G4bool addPMTBase = false; //ToDo: one parameter for both ConstructMultiPMT and ConstructPMT
  G4double mPMT_zRange[2] = {0, cylinder_height};
  G4double mPMT_RRange[2] = {cylinder_radius + expose + dist_PMTglass_innerPressureVessel,
			     cylinder_radius + expose + dist_PMTglass_innerPressureVessel};
  //NEW: only outer shell as mother volume for parametrization, as I want to put a black sheet in there
  G4double mPMT_rRange[2] = {cylinder_radius,cylinder_radius}; 
  if(addPMTBase || nID_PMTs == 1)
    mPMT_rRange = {0,0}; 

  // origin is bottom of upward cylinder
  G4Polycone* mPMT_cylinder = 
    new G4Polycone("WCmPMT_cyl",                    
		   0.0*deg,
		   360.0*deg,
		   2,
		   mPMT_zRange,
		   mPMT_rRange, // R Inner
		   mPMT_RRange);// R Outer

  G4double radius_min = cylinder_radius;
  if(addPMTBase || nID_PMTs == 1)
    radius_min = 0.;
  G4Sphere* mPMT_top_sphere =
    new G4Sphere(    "WCmPMT_tsphere",
		     radius_min,cylinder_radius+expose + dist_PMTglass_innerPressureVessel, 
		     0.0*deg,360.0*deg,
		     0.0*deg,90.0*deg);


  G4Sphere* mPMT_bottom_sphere =
    new G4Sphere(    "WCmPMT_bsphere",
		     radius_min,cylinder_radius+expose + dist_PMTglass_innerPressureVessel, 
		     0.0*deg,360.0*deg,
		     90.0*deg,180.0*deg);

  //Add them up:
  G4VSolid* temp_sum =
    new G4UnionSolid("Cyl+TopSphere", mPMT_cylinder, mPMT_top_sphere,
		     0, G4ThreeVector(0,0,cylinder_height));


  G4VSolid* multiPMT_container =
    new G4UnionSolid("WCMultiPMT", temp_sum, mPMT_bottom_sphere);

  G4LogicalVolume *logicWCMultiPMT_container =
    new G4LogicalVolume(    multiPMT_container,
			    //G4Material::GetMaterial("Air"),
			    G4Material::GetMaterial("SilGel"), //whole area between pressure vessel and support structure is optical gel!
			    "WCMultiPMT_container",
			    0,0,0);

  /////////////////////////////////////////////////////
  /// 4) Fill the mPMT vessel with single (ID) PMTs ///
  /////////////////////////////////////////////////////

  /*
   * ToDo : Also have an alternate filling option according to some array. BUT do that
   * in the parametrization somehow (eg. looping over an array or so)
   */

  G4double pmtDistance = cylinder_radius; // Inner radius od the DOM 
  G4cout << "Distance from the Z axis = " <<  pmtDistance << " mm" << G4endl;

  G4LogicalVolume* logicWCPMT = ConstructPMT(PMTName, CollectionName);

  G4int NbOfTotPmt = 0;
  if(nID_PMTs == 1){

    /////////////////////////////////////////
    /// For single PMT WITH acrylic cover ///
    /////////////////////////////////////////
    
    G4VPhysicalVolume* singlePMT =
      new G4PVPlacement(0,
			G4ThreeVector(0, 0, cylinder_height),
			logicWCPMT,                       // its logical volume
			"pmt",
			logicWCMultiPMT_container,        // Mother logical volume,
			false,
			0,
			checkOverlaps);  
    
    G4VPhysicalVolume* place_container =
      new G4PVPlacement(0,			            // its rotation
			G4ThreeVector(0,0,-cylinder_height),		// its position
			logicWCMultiPMT_container,   	    // its logical volume
			"WCPMT_container",		    // its name 
			logicWCMultiPMT,		    // its mother volume
			false,			    // no boolean os
			0, 
			checkOverlaps);			
    
    
    /* Set all visualization here for better overview. */
    // Gray wireframe visual style
    G4VisAttributes* VisAttGrey = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
    //VisAttGrey->SetForceWireframe(true);
    VisAttGrey->SetForceSolid(true); 
    G4VisAttributes* WCPMTVisAtt5 = new G4VisAttributes(G4Colour(.0,1.,1.));
    WCPMTVisAtt5->SetForceSolid(true); 
    G4VisAttributes* VisAttYellow = new G4VisAttributes(G4Colour(1.0,1.,0.));
    VisAttYellow->SetForceSolid(true); 
    
    
    //logicWCMultiPMT->SetVisAttributes(VisAttGrey); 
    //logicWCMultiPMT_vessel->SetVisAttributes(WCPMTVisAtt5);  
    //logicWCMultiPMT_container->SetVisAttributes(VisAttYellow); 
    
    return logicWCMultiPMT;
  }
  else{
    NbOfTotPmt = FillCircles();
    if(NbOfTotPmt != nID_PMTs){
      G4cerr << "Requested " <<  nID_PMTs << " PMTs but only acquired " << NbOfTotPmt << 
	" PMT from config file " << G4endl;
      return NULL;
    }
    // OLDER version:
    // CountPMT() needs number of PMTs for circle 1
    // This is the only free param of the OLDER fill-PMT alg 
    //NbOfTotPmt = CountPMT(nID_PMTs); // Total number of PMTs in the circles, defines vNiC and vAlpha
    
  }
  
  //Fill top sphere (or let the parametrisation figure out the whole filling? Yes!)
  // ToDo: if wanted: fill cylindrical part 
  //       fill bottom hemisphere (OD PMT)
  G4VPVParameterisation* pmtParam_id =
    new WCSimMultiPMTParameterisation(
				      nID_PMTs,	        // NoPMTs
				      pmtDistance,	// Z spacing of centers
				      vNiC,		// # PMTs in each circle
				      vAlpha,		// Their tilt angle
				      vCircle,          // Circle number
				      cylinder_height);	// Position offsets of circles, should become array!!!! ToDo, for bottom sphere
  
  // dummy value : kZAxis -- modified by parameterised volume
  
  // Mother Volume can only have ONE parametrization, 
  // hence need to cut in pieces OR parametrize all at once.
  // TF: cut in pieces is better for different OD/ID types. But different parametrization objects needed?
  G4VPhysicalVolume* hemisphere = 
    new G4PVParameterised("pmt",                      // their name
			  logicWCPMT,                 // their logical volume
			  logicWCMultiPMT_container,  // Mother logical volume
			  kZAxis,                     // Are placed along this axis
			  NbOfTotPmt,                 // Number of PMTs (replica's)
			  pmtParam_id,                // The parametrisation
			  true);                      // checking overlaps
  
  // Need a 4mm tolerance 
  if(hemisphere->CheckOverlaps(1000,4,1)){
    G4cerr << "Hemisphere has overlapping PMTs: Retry" << G4endl;
    return NULL; 
  }
  
  G4VPhysicalVolume* place_container =
    new G4PVPlacement(	0,				     // its rotation
			G4ThreeVector(0,0,-cylinder_height), // its position
			logicWCMultiPMT_container,   	     // its logical volume
			"WCPMT_container",		     // its name 
			logicWCMultiPMT,		     // its mother volume
			false,				     // no boolean os
			0, 
			checkOverlaps);			
    

  ///////////////////////////////////////////////////////////////
  /// 5) Cover the surface with black material/paint/...sheet ///
  ///////////////////////////////////////////////////////////////
  
  if(!addPMTBase){
    /////
    //Defines a cylinder
    //G4double mPMT_zRange[2] = {0, cylinder_height};
    G4double thickness = mPMT_inner_material_d; //...everything else should be air
    G4double inner_RRange[2] = {cylinder_radius, cylinder_radius};
    G4double inner_rRange[2] = {cylinder_radius-thickness,cylinder_radius-thickness};
    
    // origin is bottom of upward cylinder
    G4Polycone* inner_cylinder = 
      new G4Polycone("inn_cyl",                    
		     0.0*deg,
		     360.0*deg,
		     2,
		     mPMT_zRange,
		     inner_rRange,   // R Inner
		     inner_RRange);  // R Outer
    
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
			      G4Material::GetMaterial(mPMT_inner_material),//"Blacksheet"), 
			      "WCMultiPMT_inner",
			      0,0,0);
    
    
    G4VPhysicalVolume* place_inner =
      new G4PVPlacement(	0,       				// its rotation
				G4ThreeVector(0,0,-cylinder_height),	// its position
				logicWCMultiPMT_inner,   	        // its logical volume
				"WCPMT_inner",				// its name 
				logicWCMultiPMT,			// its mother volume
				false,					// no boolean os
				0, 
				checkOverlaps);			
  

    G4VisAttributes* VisAttRed = new G4VisAttributes(G4Colour(1.0,0.,0.));
    VisAttRed->SetForceSolid(true); 
    //logicWCMultiPMT_inner->SetVisAttributes(VisAttRed);
  }
  
  /* Set all visualization here for better overview. */
  // Gray wireframe visual style
  G4VisAttributes* VisAttGrey = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
  //VisAttGrey->SetForceWireframe(true);
  VisAttGrey->SetForceSolid(true); 
  G4VisAttributes* WCPMTVisAtt5 = new G4VisAttributes(G4Colour(.0,1.,1.));
  WCPMTVisAtt5->SetForceSolid(true); 
  G4VisAttributes* VisAttYellow = new G4VisAttributes(G4Colour(1.0,1.,0.));
  VisAttYellow->SetForceSolid(true); 
  
  
  //logicWCMultiPMT->SetVisAttributes(VisAttGrey); 
  //logicWCMultiPMT_vessel->SetVisAttributes(WCPMTVisAtt5);  
  //logicWCMultiPMT_container->SetVisAttributes(VisAttYellow); 

  // Keep track of already created mPMT logical volumes in same map as for PMTs
  PMTLogicalVolumes[key] = logicWCMultiPMT;
  
  return logicWCMultiPMT;
}

/*
 * Different approach (more practical) than the one below using CountPMT()
 * We know total number of PMTs and have a minimum alpha.
 * Using findClosestPacking.py to find combination with maximum viewing angle.
 * This results in the number of circles and the number of PMTs per circle.
 * Need to fill the vectors
 */
G4int WCSimDetectorConstruction::FillCircles(void){

  //Read in text file (instead of setting 8 numbers) with configuration
  // and pre-calculated values
  //Fill vAlpha, vNiC, vCircle
  std::vector<float> vEta;
  
  
  std::ifstream config(config_file); 
  if(!config){
    G4cerr << "Can not find mPMT configfile: " << config_file << G4endl;
    return 0;
  }
  std::string line, item;
  getline(config,line);
  std::istringstream value(line);
  while(getline(value, item, ' ')){
    if(atoi(item.c_str()) > 0)
      vNiC.push_back(atoi(item.c_str()));
  }
  getline(config,line);
  std::istringstream value2(line);
  while(getline(value2, item, ' ')){
    vEta.push_back(atof(item.c_str())*CLHEP::deg);
  }
  G4double min_eta = *std::min_element(vEta.begin(),vEta.end());

  getline(config,line);
  std::istringstream value3(line);
  while(getline(value3, item, ' ')){
    vAlpha.push_back(atof(item.c_str())*CLHEP::deg);
  }
    
  //for(int i = 0; i < vNiC.size(); i++) {
  // std::cout << "test " << vNiC[i] << " " << vAlpha[i]/CLHEP::deg << " " << vEta[i]/CLHEP::deg << std::endl;
  //}


 // Total number of pmt in the circles
  G4int TotPmt = 0;
  for(int i = 0; i < vNiC.size();i++){
    TotPmt+=vNiC[i];
    for(int j = 0; j < vNiC[i]; j++){
      vCircle.push_back(i); //number circles internally between 0 and N-1
    }
  }

  //std::cout << "Test: vNic: " << vNiC.size() << " vAlpha: " << vAlpha.size() << " vCircle: " << vCircle.size() << std::endl;

  return TotPmt; 
}





/*
 * Method based on the number of PMTs in the first circle
 * Then the rest will be filled according to closest packing
 * Result are filled vNiC, vAlpha and vCircle vectors.
 * Return total number of PMTs
 *
 * Downside : - uses same fEta (from first circle) for all.
 *            - assumes the viewing angle eta from the first circle
 *              is the minimum one, which is not always true.
 *            - needs an empirical number of PMTs in circle 1 to start with
 *              and cannot properly deal with a minimum opening angle.
 *            - calculate configuration for each WCSim MC (slow).
 *             => DEPRECATE at some point.
 */
G4int WCSimDetectorConstruction::CountPMT(G4int NoPmt)
{

  G4double fEta = ComputeEta(NoPmt); // PMT viewing angle, viewing_angle (CONVERT TO DEG)
  
  G4int NoCircle = 1;
  G4double alphaNext = fEta; 
  G4double alphaPrev = alphaNext;
  G4double NiCNext = NoPmt;

  // Only fill the ones where alpha is above the minimum (to look over neighbouring mPMTs)
  // Theta_min = eta + atan(R/r) with R radius of sphere, and r the distance between mPMTs
  G4double theta_min = 13.*pi/180; //atan((cylinder_radius+expose+outer_thickness)/id_spacing)
  if(alphaNext > theta_min + fEta){
    vAlpha.push_back(alphaNext);
    vNiC.push_back(NoPmt);
    G4cout << "Circle n. " << NoCircle << " - Number of PMTs: " << vNiC[NoCircle-1] << " - alpha: " << vAlpha[NoCircle-1]*180/3.141592 << " - eta " << fEta*180/3.141592 << G4endl;
    NoCircle++;
  }
    
  // Recursive calculation of vector contents
  do
    {
      alphaNext = ComputeAlpha(alphaPrev, fEta);
      NiCNext = ComputeNiC(alphaNext, fEta);
      if(NiCNext<2){break;} // Prevents negative values of NiC
      if(alphaNext > theta_min +fEta){
	vAlpha.push_back(alphaNext);
	vNiC.push_back(NiCNext);
	G4cout << "Circle n. " << NoCircle << " - Number of PMTs: " << vNiC[NoCircle-1] << " - alpha: " << vAlpha[NoCircle-1]*180/3.141592 << " - eta " << fEta*180/3.141592 << G4endl;
	std::cout << "Circle n. " << NoCircle << " - Number of PMTs: " << vNiC[NoCircle-1] << " - alpha: " << vAlpha[NoCircle-1]*180/3.141592 << " - eta " << fEta*180/3.141592 << std::endl;
	NoCircle++;
      }
      alphaPrev = alphaNext;
    } while(NiCNext > 2); // No less than 3 detectors in each circle

  // TF: If possible, add top:
  if(vAlpha[NoCircle-1]+2*fEta < 90.){
    alphaNext = pi/2;
    NiCNext = 1;
    vAlpha.push_back(alphaNext);
    vNiC.push_back(NiCNext);
    G4cout << "Circle n. " << NoCircle << " - Number of PMTs: " << vNiC[NoCircle-1] << " - alpha: " << vAlpha[NoCircle-1]*180/3.141592 << " - eta " << fEta*180/3.141592 << G4endl;
    std::cout << "Circle n. " << NoCircle << " - Number of PMTs: " << vNiC[NoCircle-1] << " - alpha: " << vAlpha[NoCircle-1]*180/3.141592 << " - eta " << fEta*180/3.141592 << std::endl;
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
 
  std::cout << "Total number of pmt: " << TotPmt << std::endl;
  std::cout << "Percentage of covered hemispherical surface = " << TotPmt*(1-std::cos(fEta))*100 << "%" << std::endl;
  std::cout << "Percentage of covered hemispherical surface above theta_min = " << TotPmt*(1-std::cos(fEta))*100/(1-std::cos(pi/2-theta_min)) << "%" << std::endl;
  std::cout << "Test: vNic: " << vNiC.size() << " vAlpha: " << vAlpha.size() << " vCircle: " << vCircle.size() << std::endl;

  return TotPmt;
}

 


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//NoPmt is number of PMT is first circle
G4double WCSimDetectorConstruction::ComputeEta(G4int NoPmt)
{
  // From eq. 11 in Meas. Sci. Technol. 10 (1999) 1015–1019, where a_{c-1} = n_{c-1} = 0.
  G4double cospi = std::cos(pi/NoPmt);
  G4double etaLocal = std::atan(std::sqrt(1 - cospi*cospi));
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
  G4double	cos2eta	  = std::cos(Eta) * std::cos(Eta);
  G4double	sin2alpha = std::sin(alphaOfCircle) * std::sin(alphaOfCircle);
  G4double	cosalpha  = std::cos(alphaOfCircle);
  G4double	arccos	  = std::acos(std::sqrt(cos2eta-sin2alpha)/cosalpha); //eq. 10
  G4int		NiCLocal  = std::floor(pi/arccos); //eq. 7
  return NiCLocal;
}
