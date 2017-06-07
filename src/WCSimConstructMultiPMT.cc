#include "WCSimDetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4Polycone.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalSkinSurface.hh"

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

// For MultiPMT: expose have different meaning, namely where to locate the sphere on the blacksheet ID/OD separator
G4LogicalVolume* WCSimDetectorConstruction::ConstructMultiPMT(G4String PMTName, G4String CollectionName)
{
  

  G4bool showme = true;
  
  // For default SK-style without cover.
  // Not possible to have a mPMT without pressure vessel now/yet.
  if(vessel_radius <= 1.*CLHEP::mm && vessel_cyl_height <= 1.*CLHEP::mm){
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

  //  expose = 0.0153*m;          //TF: to compare same size vessels for different size PMTs
  // this is NOT the "expose" of the mPMT

  ////////////////////////////////////////////////////
  /// 1) Outer logical volume: fill with water     ///
  /// (cfr. same starting point as ConstructPMT)   ///
  ////////////////////////////////////////////////////

  // origin on blacksheet wall is origin of spherical top for ID, 
  // change if we want to use the cylindrical part in ID.

  // NEW: possibility of spherical Caps == sphere minus cylinder
  G4double vessel_cap_height = vessel_radius_curv - sqrt(vessel_radius_curv * vessel_radius_curv - vessel_radius*vessel_radius);


  G4double mPMT_zRange_outer[2] = {0,                         // must be zero or it will collide with the blacksheet and mess up the G4Navigator
				   vessel_cyl_height + vessel_cap_height};
  G4double mPMT_RRange_outer[2] = {vessel_radius, 
				   vessel_radius};
  G4double mPMT_rRange_outer[2] = {0., 0.};

  // Although G4Tubs is more natural, Polycone is used to be in control of z position
  // and because z = 0 is position of bottom of cylinder, vs center in G4Tubs
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
  G4Sphere* mPMT_top_sphere_vessel =
    new G4Sphere(    "WCmPMT_tsphere_vessel",
		     0,
		     vessel_radius_curv,
		     0.0*deg,360.0*deg,
		     0.0*deg,90.0*deg);


  G4SubtractionSolid * mPMT_top_cap_vessel;
  //G4LogicalVolume * logic_mPMT_top_sphere_vessel;
  G4double cap_position_offset = 0.*cm;
  cap_position_offset = -1.0* (vessel_radius_curv - vessel_cap_height); //lower position of cap by the piece that we cut off.
  G4LogicalVolume *logic_mPMT_vessel;
  
  // CAP
  if(vessel_radius_curv - vessel_radius > .1*mm){
    G4Box*  solidCutOffTubs = 
      new G4Box(    "cutOffTubs",
		    vessel_radius_curv + 1.*cm,                      // x/2
		    vessel_radius_curv + 1.*cm,                      // y/2
		    vessel_radius_curv - vessel_cap_height);         // z/2
    
    mPMT_top_cap_vessel =
      new G4SubtractionSolid(    "InteriorWCPMT",
				 mPMT_top_sphere_vessel,
				 solidCutOffTubs);
   

    //std::cout << "DEBUG MODE " << vessel_radius_curv - vessel_cap_height << " " <<vessel_cap_height << std::endl;
 
    //cap_position_offset = -1.0* (vessel_radius_curv - vessel_cap_height); //lower position of cap by the piece that we cut off.


    //CAP WO CYLINDER
    if(vessel_cyl_height < 0.01*mm){ 

      /*Acrylic/Water cover of the mPMT*/
      logic_mPMT_vessel =
	new G4LogicalVolume(mPMT_top_cap_vessel,
			    G4Material::GetMaterial(mPMT_outer_material),//"G4_PLEXIGLASS"), 
			    "WCMultiPMT_vessel_top",
			    0,0,0);
      
      
      // Place Vessel Top:
      new G4PVPlacement(0,				                           // its rotation
			G4ThreeVector(0,0,cap_position_offset),                    // its position
			logic_mPMT_vessel,              	                   // its logical volume
			"WCPMT_vessel_top",				           // its name 
			logicWCMultiPMT,			                   // its mother volume
			false,					                   // no boolean os
			0, 
			checkOverlaps);		       
    } else{
      // CAP + CYLINDER
      // Define a cylinder with spherical top 
      // origin is center of upward cylinder
      G4Tubs* mPMT_cylinder_vessel = 
	new G4Tubs("WCmPMT_cyl_vessel",
		   0.,
		   vessel_radius,
		   vessel_cyl_height/2.,
		   0.0*deg,
		   360.0*deg);
      
      
      /* NEW: UnionSolid instead (but should I be worried about performance? Other solutions?)
	 KEEP CODE below in case the material between both volumes becomes different at some stage !!!!!!
	 
	 logic_mPMT_cylinder_vessel =
	 new G4LogicalVolume(    mPMT_cylinder_vessel,
	                         G4Material::GetMaterial(mPMT_outer_material),//"G4_PLEXIGLASS"), 
	                           "WCMultiPMT_cylinder_vessel",
	                            0 ,0,0);
				    

           // Place Cylinder Vessel
           new G4PVPlacement(	0,				         // its rotation
			G4ThreeVector(0,0,vessel_cyl_height/2),    // its position
			logic_mPMT_cylinder_vessel,   	         // its logical volume
			"WCPMT_cyl_vessel",			 // its name 
			logicWCMultiPMT,			 // its mother volume
			false,					 // no boolean os
			0, 
			checkOverlaps);			
      */
      
      // Make union solid: PMTs can extend in cylindrical half (as the support structure)
      //                   to avoid overlaps, needs to be one solid.
      G4UnionSolid *union_vessel = 
	new G4UnionSolid("VesselUnion",mPMT_cylinder_vessel,mPMT_top_cap_vessel,0,G4ThreeVector(0,0,cap_position_offset+vessel_cyl_height/2));
      
      
      logic_mPMT_vessel =
	new G4LogicalVolume(union_vessel,
			    G4Material::GetMaterial(mPMT_outer_material),//"G4_PLEXIGLASS"), 
			    "WCMultiPMT_vessel",
			    0 ,0,0);
      
      // Place Vessel
      new G4PVPlacement(0,				           // its rotation
			G4ThreeVector(0,0,vessel_cyl_height/2),    // its position
			logic_mPMT_vessel,   	                   // its logical volume
			  "WCPMT_vessel",			   // its name 
			logicWCMultiPMT,			   // its mother volume
			false,				   // no boolean os
			0, 
			checkOverlaps);
      
    }
    
  } else {
    // SPHERE WO CYLINDER
    if(vessel_cyl_height < 0.01*mm){ 
      //Acrylic/Water cover of the mPMT      
      logic_mPMT_vessel =
	new G4LogicalVolume(    mPMT_top_sphere_vessel,
				G4Material::GetMaterial(mPMT_outer_material),//"G4_PLEXIGLASS"), 
				"WCMultiPMT_vessel_top",
				0,0,0);
      
      
      // Place Vessel Top:
      new G4PVPlacement(0,				                           // its rotation
			G4ThreeVector(0,0,0),                                      // its position
			logic_mPMT_vessel,   	                   // its logical volume
			"WCPMT_vessel_top",				           // its name 
			logicWCMultiPMT,			                   // its mother volume
			false,					                   // no boolean os
			0, 
			checkOverlaps);		       
    } else{
      // SPHERE + CYLINDER
      // Define a cylinder with spherical top 
      // origin is center of upward cylinder
      G4Tubs* mPMT_cylinder_vessel = 
	new G4Tubs("WCmPMT_cyl_vessel",
		   0.,
		   vessel_radius,
		   vessel_cyl_height/2.,
		   0.0*deg,
		   360.0*deg);
      

      // Make union solid: PMTs can extend in cylindrical half (as the support structure)
      //                   to avoid overlaps, needs to be one solid.
      G4UnionSolid *union_vessel = 
	new G4UnionSolid("VesselUnion",mPMT_cylinder_vessel,mPMT_top_sphere_vessel,0,G4ThreeVector(0,0,vessel_cyl_height/2));
      
      
      logic_mPMT_vessel =
	new G4LogicalVolume(union_vessel,
			    G4Material::GetMaterial(mPMT_outer_material),//"G4_PLEXIGLASS"), 
			    "WCMultiPMT_vessel",
			    0 ,0,0);
      
      // Place Vessel
      new G4PVPlacement(0,				           // its rotation
			G4ThreeVector(0,0,vessel_cyl_height/2),  // its position
			logic_mPMT_vessel,   	                   // its logical volume
			"WCPMT_vessel",			   // its name 
			logicWCMultiPMT,			   // its mother volume
			false,				   // no boolean os
			0, 
			checkOverlaps);
      
    }//end CYLINDER condition
  }//end CAP OR SPHERE condition



  /////////////////////////////////////////////////////////////////////////
  /// 3) This is the area between the outer shell and the inner shell.  ///
  ///    In this space, the PMTs will live.                             ///
  /////////////////////////////////////////////////////////////////////////

    G4bool addPMTBase = false; //ToDo: one parameter for both ConstructMultiPMT and ConstructPMT

  // origin is center of upward cylinder
  G4LogicalVolume *logic_mPMT_container;
  //necessary to make concentric shells because can Mother can only contain parametrized daughters.
  G4double innerR_curv_container = vessel_radius_curv - mPMT_outer_material_d - 54.*mm;  //-expose - dist_pmt_vessel
  G4double outerR_curv_container = vessel_radius_curv - mPMT_outer_material_d;

  //vessel_curv - vessel_height is conserved for all concentric caps !
  G4double innerR_container = sqrt( innerR_curv_container*innerR_curv_container -
				    (vessel_radius_curv - vessel_cap_height)*(vessel_radius_curv - vessel_cap_height) );
  G4double outerR_container = sqrt( outerR_curv_container*outerR_curv_container -
				    (vessel_radius_curv - vessel_cap_height)*(vessel_radius_curv - vessel_cap_height) );
  
  //std::cout << "Inner container radii: Rcurv " << innerR_curv_container << ", R: " << innerR_container << std::endl;
  //std::cout << "Outer container radii: Rcurv " << outerR_curv_container << ", R: " << outerR_container << std::endl;

  if(addPMTBase || nID_PMTs == 1)
    innerR_container = 0;

  G4Sphere* mPMT_top_sphere_container =
    new G4Sphere(    "WCmPMT_tsphere_cont",
		     innerR_curv_container,outerR_curv_container,
		     0.0*deg,360.0*deg,
		     0.0*deg,90.0*deg);
  

  G4SubtractionSolid * mPMT_top_cap_container;
  //G4LogicalVolume * logic_mPMT_top_sphere_container;

  // CAP inner
  if(vessel_radius_curv - vessel_radius > .1*mm){
    G4Box*  solidCutOffContainer = 
      new G4Box(    "cutOffContainer",
		    outerR_curv_container + 1.*cm, 
		    outerR_curv_container + 1.*cm,                     
		    vessel_radius_curv - vessel_cap_height);        
    
    mPMT_top_cap_container =
      new G4SubtractionSolid(    "InteriorWCPMT",
				 mPMT_top_sphere_container,
				 solidCutOffContainer);
    

    // CAP wo CYLINDER inner
    if( vessel_cyl_height < 0.01*mm){
      
      logic_mPMT_container =
	new G4LogicalVolume(    mPMT_top_cap_container,
				G4Material::GetMaterial(mPMT_inner_material),//"SilGel"), //whole area between pressure vessel and support structure is optical gel!
			      "WCMultiPMT_container",
			      0,0,0);

    } else{
      // CAP + CYLINDER inner
      G4Tubs* mPMT_cylinder_container = 
	new G4Tubs("WCmPMT_cyl_container",                    
		   innerR_container,
		   outerR_container,
		   vessel_cyl_height/2.,
		   0.0*deg,
		   360.0*deg);
      
     
      G4UnionSolid *union_container = 
	new G4UnionSolid("ContainerUnion",mPMT_cylinder_container,mPMT_top_cap_container,0,G4ThreeVector(0,0,cap_position_offset+vessel_cyl_height/2));
      

      logic_mPMT_container =
	new G4LogicalVolume(    union_container,
				G4Material::GetMaterial(mPMT_inner_material),//"SilGel"), //whole area between pressure vessel and support structure is optical gel!
				"WCMultiPMT_container",
				0,0,0);
      
    }
  } else{
    //SPHERE wo CYLINDER inner     
    if( vessel_cyl_height < 0.01*mm){
      logic_mPMT_container =
	new G4LogicalVolume(    mPMT_top_sphere_container,
				G4Material::GetMaterial(mPMT_inner_material),//"SilGel"), //whole area between pressure vessel and support structure is optical gel!
				"WCMultiPMT_container",
				0,0,0);
           
      
    }else{
      // SPHERE w CYLINDER inner
      G4Tubs* mPMT_cylinder_container = 
	new G4Tubs("WCmPMT_cyl_container",                    
		   innerR_container,
		   outerR_container,
		   vessel_cyl_height/2.,
		   0.0*deg,
		   360.0*deg);
      
      G4UnionSolid *union_container = 
	new G4UnionSolid("ContainerUnion",mPMT_cylinder_container,mPMT_top_sphere_container,0,G4ThreeVector(0,0,vessel_cyl_height/2));

      logic_mPMT_container =
	new G4LogicalVolume(    union_container,
				G4Material::GetMaterial(mPMT_inner_material),//"SilGel"), //whole area between pressure vessel and support structure is optical gel!
				"WCMultiPMT_container",
				0,0,0);
      
    }
  }


  /////////////////////////////////////////////////////
  /// 4) Fill the mPMT vessel with single (ID) PMTs ///
  /////////////////////////////////////////////////////

  /*
   * ToDo : Also have an alternate filling option according to some array. BUT do that
   * in the parametrization somehow (eg. looping over an array or so)
   */

  // individual PMTs z=0 is offset by position_z_offset (see ConstructPMT) for n>1
  G4double pmtDistance = innerR_curv_container; // Inner radius od the DOM 
  pmtDistance -= (vessel_radius_curv - mPMT_outer_material_d - 54.*mm); 
  G4cout << "Distance from the Z axis and corrected for z0 of individual PMTs = " <<  pmtDistance << " mm" << G4endl;
  
  G4LogicalVolume* logicWCPMT = ConstructPMT(PMTName, CollectionName);

  G4int NbOfTotPmt = 0;
  if(nID_PMTs == 1){

    /////////////////////////////////////////
    /// For single PMT WITH acrylic cover ///
    /////////////////////////////////////////

    double single_pmt_z_offset = 0.;
    // raise/lower PMT to be inside the container
    if(vessel_cyl_height < 0.01*mm)
      single_pmt_z_offset = -cap_position_offset;
    else 
      single_pmt_z_offset = -vessel_cyl_height/2;
    // singlePMT: OK now!
    new G4PVPlacement(0,
		      G4ThreeVector(0, 0, single_pmt_z_offset),  
		      logicWCPMT,                       // its logical volume
		      "pmt",
		      logic_mPMT_container,        // Mother logical volume,
		      false,
		      0,
		      checkOverlaps);  
    
    
    //Place Container
    new G4PVPlacement(0,			            // its rotation
		      G4ThreeVector(0,0,0),	            // its position
		      logic_mPMT_container,      // its logical volume
		      "WCPMT_container",		    // its name 
		      logic_mPMT_vessel,	    // its mother volume
		      false,			            // no boolean os
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
    
      if(showme){
	logicWCMultiPMT->SetVisAttributes(VisAttGrey); 
	
	logic_mPMT_vessel->SetVisAttributes(WCPMTVisAtt5);    
	logic_mPMT_container->SetVisAttributes(VisAttYellow); 
      }

      // Keep track of already created mPMT logical volumes in same map as for PMTs
      PMTLogicalVolumes[key] = logicWCMultiPMT;


    return logicWCMultiPMT;
  }
  else{
    NbOfTotPmt = FillCircles();
    if(NbOfTotPmt != nID_PMTs){
      G4cerr << "ERROR: Requested " <<  nID_PMTs << " PMTs but only acquired " << NbOfTotPmt << 
	" PMT from config file " << G4endl;
      return NULL;
    }
    // OLDER version:
    // CountPMT() needs number of PMTs for circle 1
    // This is the only free param of the OLDER fill-PMT alg 
    //NbOfTotPmt = CountPMT(nID_PMTs); // Total number of PMTs in the circles, defines vNiC and vAlpha
    
  }
  
  //Fill top sphere (or let the parametrisation figure out the whole filling? Yes!)
  // raise/lower PMT to be inside the container
  double pmt_z_offset = 0.;
  if(vessel_cyl_height >= 0.01*mm){
    pmt_z_offset = cap_position_offset+vessel_cyl_height/2; // and same for inner structure
  }
  G4VPVParameterisation* pmtParam_id =
    new WCSimMultiPMTParameterisation(
				      nID_PMTs,	        // NoPMTs
				      pmtDistance,	// Radius of PMT translated positions on sphere
				      vNiC,		// # PMTs in each circle
				      vAlpha,		// Their tilt angle
				      vCircle,          // Circle number
				      vAzimOffset,      // The offset in azimuth angle of first PMT in that circle
				      pmt_z_offset);    // Position offsets of circles, should become array!

  
  // dummy value : kZAxis -- modified by parameterised volume
  
  // Mother Volume can only have ONE parametrization, 
  // hence need to cut in pieces OR parametrize all at once.
  // TF: cut in pieces is better for different OD/ID types. But different parametrization objects needed?

  
  G4VPhysicalVolume* hemisphere = 
    new G4PVParameterised("pmt",                            // their name
			  logicWCPMT,                       // their logical volume
			  logic_mPMT_container,             // Mother logical volume
			  kZAxis,                           // Are placed along this axis
			  NbOfTotPmt,                       // Number of PMTs (replica's)
			  pmtParam_id,                      // The parametrisation
			  true);                            // checking overlaps
  
  
  // Need a 4mm tolerance : still not perfect though because overlaps with mother volume happen still..
  if(hemisphere->CheckOverlaps(1000,4,1)){
    G4cerr << "Hemisphere has overlapping PMTs: Retry" << G4endl;
    //return NULL; 
  }
  
    /* TESTING single case
  G4RotationMatrix* rotm = new G4RotationMatrix();
  rotm->rotateY(-36.*CLHEP::degree);
  new G4PVPlacement(	rotm,				             // its rotation
			G4ThreeVector(0,0,pmt_z_offset),                        // its position
			logicWCPMT,     	             // its logical volume
			"test1",		             // its name 
			logic_mPMT_container,       		     // its mother volume
			false,				             // no boolean os
			0, 
			checkOverlaps);			
  */



  
  // Place Container:
  /*
  new G4PVPlacement(	0,				             // its rotation
			G4ThreeVector(0,0,0),                        // its position
			logic_mPMT_top_sphere_container,   	     // its logical volume
			"WCPMT_top_container",		             // its name 
			logic_mPMT_top_sphere_vessel,		     // its mother volume
			false,				             // no boolean os
			0, 
			checkOverlaps);			
  */
  new G4PVPlacement(	0,				             // its rotation
			G4ThreeVector(0,0,0),                        // its position
			logic_mPMT_container,   	             // its logical volume
			"WCPMT_container",		             // its name 
			logic_mPMT_vessel,       		     // its mother volume
			false,				             // no boolean os
			0, 
			checkOverlaps);			
  




  ///////////////////////////////////////////////////////////////
  /// 5) Cover the surface with black material/paint/...sheet ///
  ///////////////////////////////////////////////////////////////

  // Note: no union solid needed here as no object lives in this volume
  
  if(!addPMTBase){
    /////
    //Defines a cylinder
    // origin is in the center of upward cylinder
    if(vessel_cyl_height > 0){
      G4Tubs* inner_cylinder = 
	new G4Tubs("inn_cyl",                    
		   0.,
		   innerR_container,
		   vessel_cyl_height/2,
		   0.0*deg,
		   360.0*deg);
      
      G4LogicalVolume *logic_mPMT_cylinder_inner =
	new G4LogicalVolume(    inner_cylinder,
				G4Material::GetMaterial("Blacksheet"), 
				"WCMultiPMT_inner_cyl",
				0,0,0);
      
      
      //Place Inner Cylinder:
	new G4PVPlacement(	0,       				// its rotation
				G4ThreeVector(0,0,0),           	// its position
				logic_mPMT_cylinder_inner,   	        // its logical volume
				"WCPMT_inner_cyl",			// its name 
				logic_mPMT_vessel,        // its mother volume
				false,					// no boolean os
				0, 
				checkOverlaps);			
      
      new G4LogicalSkinSurface("FoamSurfaceCyl",logic_mPMT_cylinder_inner, OpGelFoamSurface);
      G4VisAttributes* VisAttRed = new G4VisAttributes(G4Colour(1.0,0.,0.));
      VisAttRed->SetForceSolid(true); 
      logic_mPMT_cylinder_inner->SetVisAttributes(VisAttRed);

    }
    G4Sphere* inner_top_sphere =
      new G4Sphere( "WCmPMT_tsphere",
		    0,innerR_curv_container,
		    0.0*deg,360.0*deg,
		    0.0*deg,90.0*deg);
    


    G4SubtractionSolid * inner_top_cap;
    G4LogicalVolume * logic_mPMT_top_sphere_inner;
    if(vessel_radius_curv - vessel_radius > .1*mm){
      
      G4Box*  solidCutOffInner = 
	new G4Box(    "cutOffInner",
		      innerR_curv_container + 1.*cm,                      // x/2
		      innerR_curv_container + 1.*cm,                      // y/2
		      vessel_radius_curv - vessel_cap_height);         // z/2
      
      inner_top_cap =
	new G4SubtractionSolid(    "InteriorWCPMT",
				   inner_top_sphere,
				   solidCutOffInner);

      
      logic_mPMT_top_sphere_inner =
	new G4LogicalVolume(    inner_top_cap,
				G4Material::GetMaterial("Blacksheet"), 
				"WCMultiPMT_inner_top",
				0,0,0);
            
      
    } else{
      logic_mPMT_top_sphere_inner =
	new G4LogicalVolume(    inner_top_sphere,
				G4Material::GetMaterial("Blacksheet"), 
				"WCMultiPMT_inner_top",
				0,0,0); 
   }
    
      
    //Place Inner Structure
      new G4PVPlacement(	0,       				// its rotation
				G4ThreeVector(0,0,pmt_z_offset), 	// its position
				logic_mPMT_top_sphere_inner,   	        // its logical volume
				"WCPMT_inner_top",			// its name 
				logic_mPMT_vessel,        // its mother volume
				false,					// no boolean os
				0, 
				checkOverlaps);			
  
    new G4LogicalSkinSurface("FoamSurfaceTop",logic_mPMT_top_sphere_inner, OpGelFoamSurface);
    
    G4VisAttributes* VisAttRed = new G4VisAttributes(G4Colour(1.0,0.,0.));
    VisAttRed->SetForceSolid(true); 
    logic_mPMT_top_sphere_inner->SetVisAttributes(VisAttRed);
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
  
  
  if(showme){
    logicWCMultiPMT->SetVisAttributes(VisAttGrey); 
    logic_mPMT_vessel->SetVisAttributes(WCPMTVisAtt5);    
    logic_mPMT_container->SetVisAttributes(VisAttYellow); 

  }

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
  //Fill vAlpha, vNiC, vCircle, vAzimOffset
  std::vector<float> vEta;
  vEta.clear();
  
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
  G4cout << "Min viewing angle eta from input file for mPMT filling is : " << min_eta << G4endl;

  getline(config,line);
  std::istringstream value3(line);
  while(getline(value3, item, ' ')){
    vAlpha.push_back(atof(item.c_str())*CLHEP::deg);
  }
    
  //for(int i = 0; i < vNiC.size(); i++) {
  // std::cout << "test " << vNiC[i] << " " << vAlpha[i]/CLHEP::deg << " " << vEta[i]/CLHEP::deg << std::endl;
  //}

  /// NEW: optionally add an azimuth offset line for the first PMT of each circle (by default 0)
  // necessary for hexagonal orientation
  getline(config,line);
  if(!line.empty()){
    std::istringstream value4(line);
    while(getline(value4, item, ' ')){
      vAzimOffset.push_back(atof(item.c_str())*CLHEP::deg);
    }
  }
  else {
    for(size_t i = 0; i < vNiC.size();i++){
	vAzimOffset.push_back(0.*CLHEP::deg);
    }
    assert(vAzimOffset.size() == vAlpha.size());
  }
  
  // Total number of pmt in the circles
  G4int TotPmt = 0;
  for(size_t i = 0; i < vNiC.size();i++){
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
  G4double theta_min = 13.*CLHEP::pi/180; //atan((vessel_radius_curv)/id_spacing)
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
    alphaNext = CLHEP::pi/2;
    NiCNext = 1;
    vAlpha.push_back(alphaNext);
    vNiC.push_back(NiCNext);
    G4cout << "Circle n. " << NoCircle << " - Number of PMTs: " << vNiC[NoCircle-1] << " - alpha: " << vAlpha[NoCircle-1]*180/3.141592 << " - eta " << fEta*180/3.141592 << G4endl;
    std::cout << "Circle n. " << NoCircle << " - Number of PMTs: " << vNiC[NoCircle-1] << " - alpha: " << vAlpha[NoCircle-1]*180/3.141592 << " - eta " << fEta*180/3.141592 << std::endl;
    NoCircle++;  
  }
  
  

  // Total number of pmt in the circles
  G4int TotPmt = 0;
  for(size_t i = 0; i < vNiC.size();i++){
    TotPmt+=vNiC[i];
    for(int j = 0; j < vNiC[i]; j++){
      vCircle.push_back(i); //number circles internally between 0 and N-1
    }
  }
 
  std::cout << "Total number of pmt: " << TotPmt << std::endl;
  std::cout << "Percentage of covered hemispherical surface = " << TotPmt*(1-std::cos(fEta))*100 << "%" << std::endl;
  std::cout << "Percentage of covered hemispherical surface above theta_min = " << TotPmt*(1-std::cos(fEta))*100/(1-std::cos(CLHEP::pi/2-theta_min)) << "%" << std::endl;
  std::cout << "Test: vNic: " << vNiC.size() << " vAlpha: " << vAlpha.size() << " vCircle: " << vCircle.size() << std::endl;

  return TotPmt;
}

 


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//NoPmt is number of PMT is first circle
G4double WCSimDetectorConstruction::ComputeEta(G4int NoPmt)
{
  // From eq. 11 in Meas. Sci. Technol. 10 (1999) 1015–1019, where a_{c-1} = n_{c-1} = 0.
  G4double cospi = std::cos(CLHEP::pi/NoPmt);
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
  G4int		NiCLocal  = std::floor(CLHEP::pi/arccos); //eq. 7
  return NiCLocal;
}
