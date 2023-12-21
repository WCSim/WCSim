//  -*- mode:c++; tab-width:4;  -*-
#include "WCSimDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4IntersectionSolid.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4PVReplica.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4AssemblyVolume.hh"
#include "G4SubtractionSolid.hh"
#include "globals.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpticalSurface.hh"
#include "G4UserLimits.hh"
#include "G4ReflectionFactory.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"

#include "WCSimTuningParameters.hh" //jl145

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "G4VSolid.hh" // following 4 .hh required for CADMesh.
#include "G4Trd.hh"
#include "G4NistManager.hh"
#include "G4AssemblyVolume.hh"

#ifndef DEBUG_RELEASTIC_GEOMETRY
#define DEBUG_RELEASTIC_GEOMETRY 0
#endif

struct RealisticPlacementConfiguration {

    G4ThreeVector EntireDetectOffset; //< Relative offset for

    G4Material* InnerDetectorMaterial;
    G4Material* OuterDetectorMaterial;

    // Black tyvek is place on the wall of the ID
    G4VisAttributes* BlackTyvekVis;
    G4Material* BlackTyvekMaterial;
    G4double BlackTyvekInnerRadius;
    G4double BlackTyvekOuterRadius;
    G4double BlackTyvekBarrelLength;

    // Dead space is the space between ID & OD
    G4VisAttributes* DeadSpaceVis;
    G4Material* DeadSpaceMaterial;
    G4double DeadSpaceInnerRadius;
    G4double DeadSpaceOuterRadius;
    G4double DeadSpaceBarrelLength;

    G4VisAttributes* OuterDetectorVis;
    G4double OuterDetectorInnerRadius;
    G4double OuterDetectorOuterRadius;
    G4double OuterDetectorBarrelLength;

    G4VisAttributes* InnerDetectorVis;
    G4double InnerDetectorInnerRadius;
    G4double InnerDetectorOuterRadius;
    G4double InnerDetectorBarrelLength;

    G4VisAttributes* WhiteTyvekVis;
    G4Material* WhiteTyvekMaterial;
    G4double WhiteTyvekInnerRadius;
    G4double WhiteTyvekOuterRadius;
    G4double WhiteTyvekBarrelLength;

    // Wall tyvek is placed on the inner wall of the water tank
    G4VisAttributes* WallTyvekVis;
    G4Material* WallTyvekMaterial;
    G4double WallTyvekInnerRadius;
    G4double WallTyvekOuterRadius;
    G4double WallTyvekBarrelLength;

    G4VisAttributes* MainWaterTankVis;
    G4Material* MainWaterTankMaterial;
    G4double MainWaterTankRadius;
    G4double MainWaterTankLength;

    G4VisAttributes* RockShellVis;
    G4Material* RockShellMaterial;
    G4double RockShellRadius;
    G4double RockShellLength;

    G4double NFrameCellsPerRow;
    G4double NRowsPerMainBlock;
    G4double NRowsPerBottomBlock;
    G4double NBlocksPerRing;

    G4double RowSeperation;
    int NSpacesInBlock;
    int NBlocksAround;
    G4double CellArcLength;


    void Print(){
      G4cout << "-----------------------------------" << G4endl;
      G4cout << "HK FD Detector Configuration Table (units: mm)" << G4endl;
      G4cout << "-----------------------------------" << G4endl;
      G4cout << "BlackTyvekInnerRadius = " << BlackTyvekInnerRadius << G4endl;
      G4cout << "BlackTyvekOuterRadius = " << BlackTyvekOuterRadius << G4endl;
      G4cout << "DeadSpaceInnerRadius = " << DeadSpaceInnerRadius << G4endl;
      G4cout << "DeadSpaceOuterRadius = " << DeadSpaceOuterRadius << G4endl;
      G4cout << "WhiteTyvekInnerRadius = " << WhiteTyvekInnerRadius << G4endl;
      G4cout << "WhiteTyvekOuterRadius = " << WhiteTyvekOuterRadius << G4endl;
      G4cout << "WallTyvekInnerRadius = " << WallTyvekInnerRadius << G4endl;
      G4cout << "WallTyvekOuterRadius = " << WallTyvekOuterRadius << G4endl;
      G4cout << "MainWaterTankRadius = " << MainWaterTankRadius << G4endl;
      G4cout << "MainWaterTankLength = " << MainWaterTankLength << G4endl;
      G4cout << "RockShellRadius = " << RockShellRadius << G4endl;
      G4cout << "RockShellLength = " << RockShellLength << G4endl;
    }

};

RealisticPlacementConfiguration config;

G4LogicalVolume* BuildAndPlace_SinglePolyhedraTank(
  std::string name,
  G4double start_radius,
  G4double end_radius,
  G4double full_length,
  G4Material* material,
  G4ThreeVector& position,
  G4LogicalVolume* mother,
  G4VisAttributes* vis,
  G4LogicalVolume*& logic,
  G4PVPlacement*& physical
)
{
  // Helper Function to support placement of cylindrical objects
  // with the number of segments matching the number of PMT cells.
  // This means each segment panel is flush with the PMT base.

  G4cout << "Placing Polyhedra Tank Name " << name << " " << mother << G4endl;

  // Basic polyhedra with top and bottom plane and only an outer shell
  G4double zplane[2] = {-0.5*full_length,0.5*full_length};
  G4double rstart[2] = {start_radius,start_radius};
  G4double rend[2] = {end_radius,end_radius};

  // Polyhedra solid object
  G4Polyhedra* solid = new G4Polyhedra(name,
                                        0, // phi start
                                        360*deg,
                                        48*6, //NPhi-gon
                                        2,
                                        zplane,
                                        rstart,
                                        rend);

  // Logical to be placed
  logic = 
      new G4LogicalVolume(solid,
                          material,
                          name,
                          0,0,0);

  // If a mother has been given it means we need to place the logical
  // inside the mother volume.
  if (mother){
    G4cout << "Placing Physical " << logic << " " << mother << G4endl;
    physical = new G4PVPlacement(0,
                          position,
                          logic,
                          name,
                          mother,
                          false,
                          0,
                          true); 
  }

  // Configure some drawing options to speed up QT.
  vis->SetForceLineSegmentsPerCircle(32);
  vis->SetForceWireframe(1);
  logic->SetVisAttributes(vis); 

  G4cout << "LOGICAL DONE --------------- " << logic << G4endl;
  return logic; 
}

int CountLogicalChildren(G4LogicalVolume* mother, G4LogicalVolume* children){
  // Helper function to count how many children of a given type
  // have already been placed inside a mother volume.
  int n = 0;
  for (int i = 0; i < mother->GetNoDaughters(); i++){
      G4VPhysicalVolume* vol = mother->GetDaughter(i);
      if ( vol->GetLogicalVolume() == children ) { n++; }
  }
  return n;
}


void SetNestedVisAttributes(G4LogicalVolume* logic, G4VisAttributes* vis){
  // Helper function to iterate through all nested volumes and fix the
  // vis attributes to a chosen value. This is needede because sometimes
  // the PMTs have high segment objects left in their drawing setup and
  // itt can crash Qt viewers when several thousand are drawn.

  // The geometry is a set of nested cylinders instead of sheets of material
  // placed inside a monotholic water volume (think russian nesting dolls).
  // This means each geometry is light tight. Somewhat confusingly it means to make the
  // ID tyvek we place a solid cylinder/block of Black Sheet, and then place another 
  // cylinder/block of Water inside of it.

  // Our Hierarchy is as follows
  // RockShell (WC)
  // - OD WCBarrel 
  // - - Wall Tyvek 
  // - - - OD Water Space
  // - - - - OD PMTs
  // - - - - White Tyvek 
  // - - - - - Dead Space
  // - - - - - - Black Sheet
  // - - - - - - - Water Inner
  // - - - - - - - - ID PMTs

  // In reverse order starting from the bottom these are
  // ID PMTS -> Inner Detector PMTS (logicWCPMT and logicWCPMT2)
  // Water Inner -> Water mass of the inner detector
  // Black Sheet -> ID Black Sheeting
  // Deadspace -> Elecetronics region between ID and OD
  // White Tyvek -> Tyvek in the OD that lines the deadspace
  // OD 



  int ndaughters = logic->GetNoDaughters();
  
  // Set vis of the mother
  logic->SetVisAttributes(vis);
  
  // Iterate through all children
  for (int i = 0; i < ndaughters; i++){
    G4VPhysicalVolume* vol = logic->GetDaughter(i);
    vol->GetLogicalVolume()->SetVisAttributes(vis);
    SetNestedVisAttributes(vol->GetLogicalVolume(), vis);
  }
}


G4LogicalVolume* WCSimDetectorConstruction::ConstructRealisticPlacement()
{
  // Main Realistic HKFD Geometry Construction Function.
  // This uses a port of the existing ConstructCylinder parameters
  // to build a new nested geometry where the ID is placeed as a child
  // inside volumes higher in the geometry hieerarchy to make fully enclosed
  // and light tight volumes. PMTs are then placed in the corresponding
  // volumes using G4Assembly classes.

    G4cout << "**** Building Realistic HK Placement Detector ****" << G4endl;

    // Instantiate NIST manager so we can use all the materials from
    // WCSimConstructMaterials
    G4NistManager* nist = G4NistManager::Instance();

    // *************************
    // Legacy WCSim Code Translation
    // *************************
    // WCSim has lots of calculations that fill DetectorConstruction Variables
    // To avoid having to search through the structure ALL variables needed 
    // are translated to the the config structure at the start.
    // Future versions should just use hard copies of the config structure.

    // Legacy WCSim Values
    WCLength  = 70*m;        // Length of the ID
    WCIDRadius = 64.8*m/2;   // Radius of the inner detector from black sheet to black sheet
    WCIDHeight = 65.751*m;   // Height of the inner detector from black sheet to black sheet
    WCODLateralWaterDepth    = 1.*m;  // Distance between the OD inner and outer barrel tyvek (i.e. width of the OD on the sides)
    WCODHeightWaterDepth     = 2.*m; // Distance between the OD inner cap and outer cap tyvek (i.e. height between bottom of the OD and inner frame)
    WCODDeadSpace            = 600.*mm; // Dead space between the OD inner tyvek, and the ID black sheet.
    WCODTyvekSheetThickness  = 1.*mm; // Thickness of the OD Tyvek
    WCBlackSheetThickness = 2*cm; // Thickness of the ID black sheet. Here this was changed from 2cm, likely a typo in legacy.

    // *************************
    // Configuration Filling
    // *************************
    
    config.NSpacesInBlock = 48;  // Number of possible PMT spaces in a single row in an integration block
    config.CellArcLength = 706.84*mm; // Length of a PMT cell from strut to strut
    config.RowSeperation = config.CellArcLength*2; // Each PMT 'row' is double height, so row seperation is 2xcell size
    config.NBlocksAround = 6; // Number of G4Assembly blocks that are stamped around the detector (rotating around phi)

    // Now we fill the configuration variables for all volumes
    config.InnerDetectorVis = new G4VisAttributes(true, G4Colour(0.0,0.0,1.0,1.0));
    config.InnerDetectorVis->SetForceSolid(1);
    config.InnerDetectorMaterial = G4Material::GetMaterial("Water");
    config.InnerDetectorInnerRadius = 0;
    config.InnerDetectorOuterRadius = WCIDRadius;
    config.InnerDetectorBarrelLength = WCIDHeight;

    config.BlackTyvekVis = new G4VisAttributes(true, G4Colour(0.0,1.0,0.0,1.0));
    config.BlackTyvekVis->SetLineWidth(2);
    config.BlackTyvekVis->SetForceAuxEdgeVisible(0);
    config.BlackTyvekMaterial = G4Material::GetMaterial("Tyvek");
    config.BlackTyvekInnerRadius = WCIDRadius;
    config.BlackTyvekOuterRadius = WCIDRadius + WCBlackSheetThickness;
    config.BlackTyvekBarrelLength = WCIDHeight + 2*WCBlackSheetThickness;

    config.DeadSpaceVis = new G4VisAttributes(true, G4Colour(0.0,0.0,0.0,1.0));
    config.DeadSpaceVis->SetForceSolid(1);
    config.DeadSpaceMaterial = G4Material::GetMaterial("Water");
    config.DeadSpaceInnerRadius = config.BlackTyvekOuterRadius;
    config.DeadSpaceOuterRadius = config.BlackTyvekOuterRadius + WCODDeadSpace;
    config.DeadSpaceBarrelLength = config.BlackTyvekBarrelLength + 2*WCODDeadSpace;

    config.WhiteTyvekVis = new G4VisAttributes(true, G4Colour(1.0,1.0,1.0,1.0));
    config.WhiteTyvekVis->SetForceSolid(1);
    config.WhiteTyvekVis->SetLineWidth(2);
    config.WhiteTyvekVis->SetForceAuxEdgeVisible(0);
    config.WhiteTyvekMaterial = G4Material::GetMaterial("Tyvek");
    config.WhiteTyvekInnerRadius = config.DeadSpaceOuterRadius;
    config.WhiteTyvekOuterRadius = config.DeadSpaceOuterRadius + WCODTyvekSheetThickness;
    config.WhiteTyvekBarrelLength = config.DeadSpaceBarrelLength + 2*WCODTyvekSheetThickness;

    config.OuterDetectorVis = new G4VisAttributes(false, G4Colour(0.5,0.5,1.0,0.05));
    config.OuterDetectorMaterial = G4Material::GetMaterial("Water");
    config.OuterDetectorInnerRadius = config.WhiteTyvekOuterRadius;
    config.OuterDetectorOuterRadius = config.WhiteTyvekOuterRadius + WCODLateralWaterDepth;
    config.OuterDetectorBarrelLength = config.WhiteTyvekBarrelLength + 2*WCODHeightWaterDepth;

    config.WallTyvekVis = new G4VisAttributes(false, G4Colour(1.0,0.0,0.0,1.0));
    config.WallTyvekVis->SetForceWireframe(1);
    config.WallTyvekVis->SetLineWidth(0);
    config.WallTyvekVis->SetForceAuxEdgeVisible(0);
    config.WallTyvekMaterial = G4Material::GetMaterial("Tyvek");
    config.WallTyvekInnerRadius = config.OuterDetectorOuterRadius;
    config.WallTyvekOuterRadius = config.OuterDetectorOuterRadius + WCODTyvekSheetThickness;
    config.WallTyvekBarrelLength = config.OuterDetectorBarrelLength + 2*WCODTyvekSheetThickness;

    config.MainWaterTankVis = new G4VisAttributes(false, G4Colour(0.0,0.1,0.2,0.5));
    config.MainWaterTankRadius = config.WallTyvekOuterRadius+5*mm;
    config.MainWaterTankLength = config.WallTyvekBarrelLength+5*mm;
    config.MainWaterTankMaterial = G4Material::GetMaterial("Water");

    config.RockShellVis = new G4VisAttributes(false, G4Colour(0.5,0.5,1.0,0.1));
    config.RockShellMaterial = G4Material::GetMaterial("Rock");
    config.RockShellRadius = config.MainWaterTankRadius + 30*cm;
    config.RockShellLength = config.MainWaterTankLength + 30*cm;

    config.NFrameCellsPerRow = 48;
    config.NRowsPerMainBlock = 16;
    config.NRowsPerBottomBlock = 12;
    config.NBlocksPerRing = 6;

    config.Print();

    // *************************
    // Shell Hierarchy Construction
    // *************************
    G4ThreeVector CENTRAL_POS = G4ThreeVector();

    // All placements are treated as nested logical volumes to ensure there are no
    // gaps in the geometry.

    // 1. Rock (WC)
    // This is actually a rock solid, moved it instead of air.
    G4LogicalVolume* rockShellLogic;
    G4PVPlacement* rockShellPhysical = NULL;
    G4LogicalVolume* rockShellLogic2 = BuildAndPlace_SinglePolyhedraTank(
      "WC", // Forced to keep these bad naming conventions?
      0.0,
      config.RockShellRadius,
      config.RockShellLength,
      config.RockShellMaterial,
      CENTRAL_POS,
      NULL, // Don't place for now.
      config.RockShellVis,
      rockShellLogic,
      rockShellPhysical
    );

    G4cout << "ROCK SHELL : " << rockShellLogic << " " << rockShellLogic2 << " " << rockShellPhysical << G4endl;

    // 2. OD (WCBarrel)
    G4LogicalVolume* MainWaterTankLogic;
    G4PVPlacement* MainWaterTankPhysical;
    BuildAndPlace_SinglePolyhedraTank(
      "WCBarrel", // Forced to keep these bad naming conventions?
      0.0,
      config.MainWaterTankRadius,
      config.MainWaterTankLength,
      config.MainWaterTankMaterial,
      CENTRAL_POS,
      rockShellLogic,
      config.MainWaterTankVis,
      MainWaterTankLogic,
      MainWaterTankPhysical
    );

    G4cout << "Water Tank : " << MainWaterTankLogic << " " << MainWaterTankPhysical << G4endl;


    // 3. White Tyvek Shell
    G4LogicalVolume* WallTyvekLogic;
    G4PVPlacement* WallTyvekPhysical;
    BuildAndPlace_SinglePolyhedraTank(
      "WallTyvek",
      0.0,
      config.WallTyvekOuterRadius,
      config.WallTyvekBarrelLength,
      config.WallTyvekMaterial,
      CENTRAL_POS,
      MainWaterTankLogic,
      config.WallTyvekVis,
      WallTyvekLogic,
      WallTyvekPhysical
    );

    // 4. OuterDetectr
    G4LogicalVolume* OuterDetectorLogic;
    G4PVPlacement* OuterDetectorPhysical;
    BuildAndPlace_SinglePolyhedraTank(
      "OuterDetector",
      0.0,
      config.OuterDetectorOuterRadius,
      config.OuterDetectorBarrelLength,
      config.OuterDetectorMaterial,
      CENTRAL_POS,
      WallTyvekLogic,
      config.OuterDetectorVis,
      OuterDetectorLogic,
      OuterDetectorPhysical
    );

    // 5. WhiteTyvek
    G4LogicalVolume* WhiteTyvekLogic;
    G4PVPlacement* WhiteTyvekPhysical;
    BuildAndPlace_SinglePolyhedraTank(
      "WhiteTyvek",
      0.0,
      config.WhiteTyvekOuterRadius,
      config.WhiteTyvekBarrelLength,
      config.WhiteTyvekMaterial,
      CENTRAL_POS,
      OuterDetectorLogic,
      config.WhiteTyvekVis,
      WhiteTyvekLogic,
      WhiteTyvekPhysical
    );

    // 6. Dead Space
    G4LogicalVolume* DeadSpaceLogic;
    G4PVPlacement* DeadSpacePhysical;
    BuildAndPlace_SinglePolyhedraTank(
      "DeadSpace",
      0.0,
      config.DeadSpaceOuterRadius,
      config.DeadSpaceBarrelLength,
      config.DeadSpaceMaterial,
      CENTRAL_POS,
      WhiteTyvekLogic,
      config.DeadSpaceVis,
      DeadSpaceLogic,
      DeadSpacePhysical
    );

    // 7. BlackTyvek
    G4LogicalVolume* BlackTyvekLogic;
    G4PVPlacement* BlackTyvekPhysical;
    BuildAndPlace_SinglePolyhedraTank(
      "BlackTyvek",
      0.0,
      config.BlackTyvekOuterRadius,
      config.BlackTyvekBarrelLength,
      config.BlackTyvekMaterial,
      CENTRAL_POS,
      DeadSpaceLogic,
      config.BlackTyvekVis,
      BlackTyvekLogic,
      BlackTyvekPhysical
    );

    // 8. InnerDetector
    G4LogicalVolume* InnerDetectorLogic;
    G4PVPlacement* InnerDetectorPhysical;

    BuildAndPlace_SinglePolyhedraTank(
      "InnerDetector",
      0.0,
      config.InnerDetectorOuterRadius,
      config.InnerDetectorBarrelLength,
      config.InnerDetectorMaterial,
      CENTRAL_POS,
      BlackTyvekLogic,
      config.InnerDetectorVis,
      InnerDetectorLogic,
      InnerDetectorPhysical
    );

    // Optional inner phantom for creating a new logical away from the PMT tracking one
    // G4LogicalVolume* InnerDetectorPhantomLogic;
    // G4PVPlacement* InnerDetectorPhantomPhysical;
    // BuildAndPlace_SinglePolyhedraTank(
    //   "InnerDetectorPhantom",
    //   0.0,
    //   config.InnerDetectorOuterRadius-1*m,
    //   config.InnerDetectorBarrelLength-2*m,
    //   config.InnerDetectorMaterial,
    //   CENTRAL_POS,
    //   InnerDetectorLogic,
    //   config.InnerDetectorVis,
    //   InnerDetectorPhantomLogic,
    //   InnerDetectorPhantomPhysical
    // );


    // *************************
    // Dummy PMT Construction
    // *************************
    // We use construction assembly units to build the detector but 
    // we need to replace each physical volume with different copyNo
    // after the placement, so here we create some dummy PMTs that
    // represent each possible PMT. We place these first using 
    // assembly volumes as its quicker.

    // Temporary PMT Logics to place
    G4Material* pmt_mat = nist->FindOrBuildMaterial("G4_AIR");
    auto pmt20_dummy_solid = new G4Sphere("pmt20",0, 508*mm/2, 0, pi,0,pi);
    auto pmt20_dummy_logic = new G4LogicalVolume(pmt20_dummy_solid, pmt_mat, "pmt20");
    G4VisAttributes* pmt20_dummy_colour = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
    pmt20_dummy_colour->SetForceLineSegmentsPerCircle(12);
    pmt20_dummy_colour->SetForceAuxEdgeVisible(1);
    pmt20_dummy_colour->SetForceWireframe(1);
    pmt20_dummy_colour->SetLineWidth(3);
    pmt20_dummy_logic->SetVisAttributes(pmt20_dummy_colour);

    auto pmtmulti_dummy_solid = new G4Sphere("pmtMulti",0, 508*mm/2, 0, pi,0,pi);
    auto pmtmulti_dummy_logic = new G4LogicalVolume(pmtmulti_dummy_solid, pmt_mat, "pmtMulti");
    G4VisAttributes* pmtmulti_dummy_colour = new G4VisAttributes(G4Colour(1.0,0.0,0.0));
    pmtmulti_dummy_colour->SetForceLineSegmentsPerCircle(12);
    pmtmulti_dummy_colour->SetForceAuxEdgeVisible(1);
    pmtmulti_dummy_colour->SetForceWireframe(1);
    pmtmulti_dummy_colour->SetLineWidth(3);
    pmtmulti_dummy_logic->SetVisAttributes(pmtmulti_dummy_colour);

    auto pmtod_dummy_solid = new G4Sphere("pmtod",0, 200*mm/2, 0, pi,0,pi);
    auto pmtod_dummy_logic = new G4LogicalVolume(pmtod_dummy_solid, pmt_mat, "pmtod");
    G4VisAttributes* pmtod_dummy_colour = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
    pmtod_dummy_colour->SetForceLineSegmentsPerCircle(12);
    pmtod_dummy_colour->SetForceAuxEdgeVisible(1);
    pmtod_dummy_colour->SetForceWireframe(1);
    pmtod_dummy_colour->SetLineWidth(3);
    pmtod_dummy_logic->SetVisAttributes(pmtod_dummy_colour);


    // *************************
    // Barrel Assembly Placement
    // *************************
    // We have to make several nested assemblies to fill the full detector.
    // Diagrams of each are kept with their respective components.
    
    // The hierarchy of assemblies are.
    // - Barrel Side/Bottom Block
    // - - PMT_only_row
    // - - - PMT_only_two_cell
    // - - - - pmt_offset_placement
    // - - PMT_mPMT1_row
    // - - - PMT_only_two_cell
    // - - - - pmt_offset_placement
    // - - - PMT_hybrid_three_cell
    // - - - - pmt_offset_placement
    // - - - - multipmt_offset_placement
    // - - PMT_mPMT2_row
    // - - - PMT_only_two_cell
    // - - - - pmt_offset_placement
    // - - - PMT_hybrid_three_cell
    // - - - - pmt_offset_placement
    // - - - - multipmt_offset_placement

    // Six barrel side/bottom blocks are placed rotated around the barrel.
    // Each PMT_only or PMT_mPMT row has frame cells (possible PMT holes) across.

    // pmt_offset_placement/multipmt_offset_placement/pmtod_offset_placement
    // ************************

    // First make a single cell assembly, this will put the PMT flush against the wall
    // Since assembly volumes just expand relative to their center point the means
    // all nested volumes will also have the PMT againt the tyvek.

    G4ThreeVector pmt_central_position = G4ThreeVector(config.InnerDetectorOuterRadius-59.62*mm,0.0,0.0);
    G4ThreeVector mpmt_central_position = G4ThreeVector(config.InnerDetectorOuterRadius-59.62*mm,0.0,0.0);
    G4ThreeVector pmt_central_offset = G4ThreeVector(0.0,0.0,0.0); // -> Can be used for relative offset!
    G4RotationMatrix* pmt_central_rotation = new G4RotationMatrix;
    pmt_central_rotation->rotateX(90*deg);
    pmt_central_rotation->rotateZ(270*deg);

    G4RotationMatrix* multipmt_central_rotation = new G4RotationMatrix;
    multipmt_central_rotation->rotateX(90*deg);
    multipmt_central_rotation->rotateZ(270*deg);

    // For the OD PMT we instead need to make it face outwards from the white tyvek in the OD.
    G4ThreeVector odpmt_central_position = G4ThreeVector(config.WhiteTyvekOuterRadius+2*cm,0.0,0.0);
    G4RotationMatrix* pmtod_central_rotation = new G4RotationMatrix;
    pmtod_central_rotation->rotateY(270*deg);

    auto pmt20_offset_placement = new G4AssemblyVolume();
    pmt20_offset_placement->AddPlacedVolume(pmt20_dummy_logic, pmt_central_position, pmt_central_rotation);

    auto pmtmulti_offset_placement = new G4AssemblyVolume();
    pmtmulti_offset_placement->AddPlacedVolume(pmtmulti_dummy_logic, mpmt_central_position, multipmt_central_rotation);

    auto pmtod_offset_placement = new G4AssemblyVolume();
    pmtod_offset_placement->AddPlacedVolume(pmtod_dummy_logic, odpmt_central_position, pmtod_central_rotation);


    // PMT_only_two_cell/PMT_hybrid_three_cell/PMT_od_central_cell
    // ************************

    // Now our job is to make a cell. The following diagrams show locations (#,O,M,F) = (None, 20inch, multiPMT, OD)
    //
    // PMT_only_two_cell
    // _______
    // |     |
    // | # O |
    // |     |
    // | O # |
    // _______

    // PMT_hybrid_three_cell
    // _______
    // |     |
    // | # O |
    // |     |
    // | O M |
    // _______

    // PMT_od_central_cell
    // _______
    // |     |
    // | # # |
    // |     |
    // | #M# |
    // _______

    G4double RowSeperation = config.RowSeperation;

    int NSpacesInBlock = config.NSpacesInBlock;
    int NSegments = config.NBlocksAround * NSpacesInBlock;
    G4double phi_offset = twopi / float(NSegments);

    G4cout << "PHI OFFSET : " << phi_offset/2 << G4endl;

    // First we build PMT_only_two_cell
    G4RotationMatrix* rotation_low = new G4RotationMatrix;
    G4RotationMatrix* rotation_high = new G4RotationMatrix;

    rotation_high->rotateZ(phi_offset/2); //-phi_offset/2);
    rotation_low->rotateZ(3*phi_offset/2);//align the PMT with the Cell

    G4ThreeVector position_low = G4ThreeVector(0,0.0,-RowSeperation/4);
    G4ThreeVector position_high = G4ThreeVector(0,0.0,RowSeperation/4);

    auto frame_block_assembly = new G4AssemblyVolume();
    frame_block_assembly->AddPlacedAssembly(pmt20_offset_placement, position_high, rotation_high );
    // frame_block_assembly->AddPlacedAssembly(pmtmulti_offset_placement, position_high, rotation_low );
    frame_block_assembly->AddPlacedAssembly(pmt20_offset_placement, position_low, rotation_low  );
    // frame_block_assembly->AddPlacedAssembly(pmtmulti_offset_placement, position_low, rotation_high );

    // Then we repeatt but also add mPMT for PMT_hybrid_three_cell
    auto frame_block_assembly_withpmt = new G4AssemblyVolume();  
    frame_block_assembly_withpmt->AddPlacedAssembly(pmt20_offset_placement, position_high, rotation_high  );
    // frame_block_assembly_withpmt->AddPlacedAssembly(pmtmulti_offset_placement, position_high, rotation_low  );
    frame_block_assembly_withpmt->AddPlacedAssembly(pmt20_offset_placement, position_low, rotation_low  );
    frame_block_assembly_withpmt->AddPlacedAssembly(pmtmulti_offset_placement, position_low, rotation_high  );


    // If MPMT in each block
//     Processing read_PMT.C...
// 19826 9927
// 13122 13128
// MEAN : 45967 41412 0.900907
// 19826 9927
// 13122 13128
// MEAN2: 46358 42099 0.908128
// (int) 0
// root [1] .q


    // Finally we build an OD assembly
    auto frame_block_assembly_odpmt = new G4AssemblyVolume();
    G4RotationMatrix* odsteprotation = new G4RotationMatrix;
    G4ThreeVector posod = G4ThreeVector(0,0.0,-RowSeperation/2);
    frame_block_assembly_odpmt->AddPlacedAssembly(pmtod_offset_placement, posod, odsteprotation);

    // PMT_mPMT1_rows
    // ************************

    // Now we make the rows. Based on integration diagrams these are:
    // Main Barrel Block
    // #O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O
    // O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#
    // #O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O
    // O#OMO#O#O#O#O#OMO#O#O#O#O#OMO#O#O#O#O#OMO#O#O#O#
    // #O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O
    // O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#
    // #O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O
    // O#O#O#O#OMO#O#O#O#O#OMO#O#O#O#O#OMO#O#O#O#O#OMO#
    // #O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O
    // O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#
    // #O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O
    // O#OMO#O#O#O#O#OMO#O#O#O#O#OMO#O#O#O#O#OMO#O#O#O#
    // #O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O
    // O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#
    // #O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O
    // O#O#O#O#OMO#O#O#O#O#OMO#O#O#O#O#OMO#O#O#O#O#OMO#

    // Bottom Barrel Block
    // #O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O
    // O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#
    // #O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O
    // O#OMO#O#O#O#O#OMO#O#O#O#O#OMO#O#O#O#O#OMO#O#O#O#
    // #O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O
    // O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#
    // #O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O
    // O#O#O#O#OMO#O#O#O#O#OMO#O#O#O#O#OMO#O#O#O#O#OMO#
    // #O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O
    // O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#
    // #O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O#O
    // O#OMO#O#O#O#O#OMO#O#O#O#O#OMO#O#O#O#O#OMO#O#O#O#

    // OD Barrel Block
    // Todo: Add Diagram for OD Barrel Block

    // The blocks above can be split into 3 different types of rows
    // for both the OD and the ID based on the index that
    // a specific OD or mPMT is added.

    // Make the Three types of block rows
    auto block_row_nomultipmts = new G4AssemblyVolume();
    auto block_row_index1multipmts = new G4AssemblyVolume();
    auto block_row_index4multipmts = new G4AssemblyVolume();

    auto block_row_odzeroindex = new G4AssemblyVolume();
    auto block_row_odoffsetindex = new G4AssemblyVolume();
    auto block_row_odfourindex = new G4AssemblyVolume();

    // Everything is still based on the central position
    // and rotated around the geometry.
    G4ThreeVector cell_pos = G4ThreeVector(0.0,0.0,0.0);
    auto cell_block_rotation = new G4RotationMatrix();


    // Sweep through angular positions in the row
    for (int i = 0; i < NSpacesInBlock/2; i++){
      cell_block_rotation->rotateZ(phi_offset*2);
      
      // Simple row only ID pmt
      block_row_nomultipmts->AddPlacedAssembly(frame_block_assembly, cell_pos, cell_block_rotation);

      // Hybrid mPMT rows for ID index 1
      if ((i-1) % 6 != 0){
        block_row_index1multipmts->AddPlacedAssembly(frame_block_assembly, cell_pos, cell_block_rotation);
      } else {
        block_row_index1multipmts->AddPlacedAssembly(frame_block_assembly_withpmt, cell_pos, cell_block_rotation);
      }

      // Hybrid mPMT rows for ID index 4
      if ((i-4) % 6 != 0){
        block_row_index4multipmts->AddPlacedAssembly(frame_block_assembly, cell_pos, cell_block_rotation);
      } else {
        block_row_index4multipmts->AddPlacedAssembly(frame_block_assembly_withpmt, cell_pos, cell_block_rotation);
      }

      // Pattern for OD PMTs
      if (i % 2 == 0) block_row_odzeroindex->AddPlacedAssembly(frame_block_assembly_odpmt, cell_pos, cell_block_rotation);
      if ((i-3) % 4 == 0) block_row_odoffsetindex->AddPlacedAssembly(frame_block_assembly_odpmt, cell_pos, cell_block_rotation);
      if ((i-1) % 4 == 0) block_row_odfourindex->AddPlacedAssembly(frame_block_assembly_odpmt, cell_pos, cell_block_rotation);

    }

    // Barrel Block Row Placements
    // ************************

    // Now we have the rows we need to build the final stamper blocks that can be imprinted.
    // The bottom block is slightly shorter so we need 4 in total.

    // Make the barrel block 
    auto block_assembly = new G4AssemblyVolume();
    auto block_bottom = new G4AssemblyVolume();

    auto odblock_assembly = new G4AssemblyVolume();
    auto odblock_bottom = new G4AssemblyVolume();

    G4RotationMatrix* block_rot = new G4RotationMatrix();

    // Hard coded row choices for each of the PMT placements.
    // This is because the integration diagrams alternate between
    // a row with no multipmts and ones with them spaced at different offsets.
    for (int i = 0; i < 8; i++){
      G4ThreeVector block_pos = G4ThreeVector(0.0,0.0,(-i-0.5)*RowSeperation);

      // ID Block
      if (i == 0) block_assembly->AddPlacedAssembly(block_row_nomultipmts, block_pos, block_rot);
      if (i == 1) block_assembly->AddPlacedAssembly(block_row_index1multipmts, block_pos, block_rot);
      if (i == 2) block_assembly->AddPlacedAssembly(block_row_nomultipmts, block_pos, block_rot);
      if (i == 3) block_assembly->AddPlacedAssembly(block_row_index4multipmts, block_pos, block_rot);
      if (i == 4) block_assembly->AddPlacedAssembly(block_row_nomultipmts, block_pos, block_rot);
      if (i == 5) block_assembly->AddPlacedAssembly(block_row_index1multipmts, block_pos, block_rot);
      if (i == 6) block_assembly->AddPlacedAssembly(block_row_nomultipmts, block_pos, block_rot);
      if (i == 7) block_assembly->AddPlacedAssembly(block_row_index4multipmts, block_pos, block_rot);

      // ID Bottom Block
      if (i == 0) block_bottom->AddPlacedAssembly(block_row_nomultipmts, block_pos, block_rot);
      if (i == 1) block_bottom->AddPlacedAssembly(block_row_index1multipmts, block_pos, block_rot);
      if (i == 2) block_bottom->AddPlacedAssembly(block_row_nomultipmts, block_pos, block_rot);
      if (i == 3) block_bottom->AddPlacedAssembly(block_row_index4multipmts, block_pos, block_rot);
      if (i == 4) block_bottom->AddPlacedAssembly(block_row_nomultipmts, block_pos, block_rot);
      if (i == 5) block_bottom->AddPlacedAssembly(block_row_index1multipmts, block_pos, block_rot);

      // OD Block
      if (i == 0) odblock_assembly->AddPlacedAssembly(block_row_odzeroindex, block_pos, block_rot);
      if (i == 1) odblock_assembly->AddPlacedAssembly(block_row_odoffsetindex, block_pos, block_rot);
      if (i == 2) odblock_assembly->AddPlacedAssembly(block_row_odzeroindex, block_pos, block_rot);
      if (i == 3) odblock_assembly->AddPlacedAssembly(block_row_odfourindex, block_pos, block_rot);
      if (i == 4) odblock_assembly->AddPlacedAssembly(block_row_odzeroindex, block_pos, block_rot);
      if (i == 5) odblock_assembly->AddPlacedAssembly(block_row_odoffsetindex, block_pos, block_rot);
      if (i == 6) odblock_assembly->AddPlacedAssembly(block_row_odzeroindex, block_pos, block_rot);
      if (i == 7) odblock_assembly->AddPlacedAssembly(block_row_odfourindex, block_pos, block_rot);

      // OD Bottom Block
      if (i == 0) odblock_bottom->AddPlacedAssembly(block_row_odzeroindex, block_pos, block_rot);
      if (i == 1) odblock_bottom->AddPlacedAssembly(block_row_odoffsetindex, block_pos, block_rot);
      if (i == 2) odblock_bottom->AddPlacedAssembly(block_row_odzeroindex, block_pos, block_rot);
      if (i == 3) odblock_bottom->AddPlacedAssembly(block_row_odfourindex, block_pos, block_rot);
      if (i == 4) odblock_bottom->AddPlacedAssembly(block_row_odoffsetindex, block_pos, block_rot);
      if (i == 5) odblock_bottom->AddPlacedAssembly(block_row_odfourindex, block_pos, block_rot);

    }

    // Final Imprint
    // ************************
    G4double imprint_spacing = 8*RowSeperation;

    // Inprint Columns first then rings
    for (int i = 0; i < 5; i++){
      G4RotationMatrix* imprint_rot = new G4RotationMatrix();
      G4ThreeVector imprint_pos = G4ThreeVector(0.0,0.0,-i*imprint_spacing + WCIDHeight/2-70*cm);
      for (int j = 0; j < 6; j++){
        block_assembly->MakeImprint(InnerDetectorLogic, imprint_pos, imprint_rot);
        odblock_assembly->MakeImprint(OuterDetectorLogic, imprint_pos, imprint_rot);
        imprint_rot->rotateZ( twopi / 6 );
      }
    }

    G4RotationMatrix* imprint_rot_bottom = new G4RotationMatrix();
    G4ThreeVector imprint_pos_bottom = G4ThreeVector(0.0,0.0,-5*imprint_spacing + WCIDHeight/2-70*cm);
    for (int j = 0; j < 6; j++){
      block_bottom->MakeImprint(InnerDetectorLogic, imprint_pos_bottom, imprint_rot_bottom);
      odblock_bottom->MakeImprint(OuterDetectorLogic, imprint_pos_bottom, imprint_rot_bottom);
      imprint_rot_bottom->rotateZ( twopi / 6 );
    }

    int pmt20_count_barrel    = CountLogicalChildren(InnerDetectorLogic, pmt20_dummy_logic);
    int pmtmulti_count_barrel = CountLogicalChildren(InnerDetectorLogic, pmtmulti_dummy_logic);
    int pmtod_count_barrel    = CountLogicalChildren(OuterDetectorLogic, pmtod_dummy_logic);

    // ------------------------
    // End Caps
    // ------------------------

    // End Cap Assembly
    // -----------------
    G4AssemblyVolume* endcap_assembly = new G4AssemblyVolume();
    G4AssemblyVolume* endcap_assembly_od = new G4AssemblyVolume();

    // We place the cap pmts in a grid, with a maximum radial distance
    G4double cap_offset_x = 706*mm; // Same as barrel PMT spacing
    G4double cap_offset_y = 706*mm; // Same as barrel PMT spacing
    G4double edgeminimum = 32322*mm - 220*mm; // Remove expose height from barrel

    G4RotationMatrix* endcapmtprotation = new G4RotationMatrix();
    endcapmtprotation->rotateX(180*deg);

    // Add Nested 20inch PMTS first, these are alternating grid pattern
    for (int i = -100; i < 100; i++ ){
      for (int j = -100; j < 100; j++ ){
        G4ThreeVector pmtpos = G4ThreeVector( i*cap_offset_x, j*cap_offset_y, 0.0 );

        if (i % 2 == 0 and (j-1) % 2 == 0) continue;
        if (j % 2 == 0 and (i-1) % 2 == 0) continue;
        if (pmtpos.mag() > edgeminimum) continue;

        endcap_assembly->AddPlacedVolume(pmt20_dummy_logic, pmtpos, endcapmtprotation);
      }
    }


    // Add the MultiPMTs, this is quite a complex pattern.
    for (int i = -100; i < 100; i++ ){
      for (int j = -100; j < 100; j++ ){
        G4ThreeVector pmtpos = G4ThreeVector( (i+1)*cap_offset_x, j*cap_offset_y, 0.0 );

        if (i % 2 == 1) continue;
        if (j % 2 == 1) continue;

        if (!(i % 6 == 0 && j % 6 == 0 && !(i % 12 == 0 && j % 12 == 0))) continue;

        if (i == 0 && j % 42 == 0 ) continue;
        if (j == 0 && i % 42 == 0 ) continue;

        if (pmtpos.mag() > edgeminimum) continue;

        endcap_assembly->AddPlacedVolume(pmtmulti_dummy_logic, pmtpos, endcapmtprotation);
      }
    }

    // OD Cap
    for (int i = -100; i < 100; i++ ){
      for (int j = -100; j < 100; j++ ){

        G4ThreeVector pmtpos = G4ThreeVector( i*cap_offset_x, j*cap_offset_y, 0.0 );

        if (!(i % 2 == 0)) continue;
        if (!(j % 3 == 0)) continue;

        bool valid = false;
        if ((i % 2 == 0 && ((j-3) % 6 == 0) && ((i) % 4 == 0))) valid = true;
        if ((j % 3 == 0 && ((j) % 6 == 0) && ((i-2) % 4 == 0))) valid = true;
        if (!valid) continue;

        if (pmtpos.mag() > edgeminimum) continue;

        endcap_assembly_od->AddPlacedVolume(pmtod_dummy_logic, pmtpos, endcapmtprotation);
      }
    }

    // Now we can place the end caps, Top first.
    // -----------------
    G4ThreeVector topcappos = G4ThreeVector(0.0,0.0,config.InnerDetectorBarrelLength/2);
    G4RotationMatrix* topcaprot = new G4RotationMatrix();
    endcap_assembly->MakeImprint( InnerDetectorLogic, topcappos, topcaprot);

    G4ThreeVector topcapposod = G4ThreeVector(0.0,0.0,config.WhiteTyvekBarrelLength/2);
    G4RotationMatrix* topcaprotod = new G4RotationMatrix();
    endcap_assembly_od->MakeImprint( OuterDetectorLogic, topcapposod, topcaprotod);

    int pmt20_count_barrel_and_top    = CountLogicalChildren(InnerDetectorLogic, pmt20_dummy_logic);
    int pmtmulti_count_barrel_and_top = CountLogicalChildren(InnerDetectorLogic, pmtmulti_dummy_logic);
    int pmtod_count_barrel_and_top    = CountLogicalChildren(OuterDetectorLogic, pmtod_dummy_logic);

    int pmt20_count_top = pmt20_count_barrel_and_top - pmt20_count_barrel;
    int pmtmulti_count_top = pmtmulti_count_barrel_and_top - pmtmulti_count_barrel;
    int pmtod_count_top = pmtod_count_barrel_and_top - pmtod_count_barrel;

    // Now cap the other side
    // -----------------
    G4ThreeVector botcappos = G4ThreeVector(0.0,0.0,-config.InnerDetectorBarrelLength/2);
    G4RotationMatrix* botcaprot = new G4RotationMatrix();
    botcaprot->rotateX(180*deg);
    endcap_assembly->MakeImprint( InnerDetectorLogic, botcappos, botcaprot);

    G4ThreeVector botcapposod = G4ThreeVector(0.0,0.0,-config.WhiteTyvekBarrelLength/2);
    G4RotationMatrix* botcaprotod = new G4RotationMatrix();
    botcaprotod->rotateX(180*deg);
    endcap_assembly_od->MakeImprint( OuterDetectorLogic, botcapposod, botcaprotod);

    // Do some final sumation
    int pmt20_count_total    = CountLogicalChildren(InnerDetectorLogic, pmt20_dummy_logic);
    int pmtmulti_count_total = CountLogicalChildren(InnerDetectorLogic, pmtmulti_dummy_logic);
    int pmtod_count_total    = CountLogicalChildren(OuterDetectorLogic, pmtod_dummy_logic);

    int pmt20_count_bottom = pmt20_count_total - pmt20_count_barrel_and_top;
    int pmtmulti_count_bottom = pmtmulti_count_total - pmtmulti_count_barrel_and_top;
    int pmtod_count_bottom = pmtod_count_total - pmtod_count_barrel_and_top;

    // We are finished with the assembly, check the totals! 
    G4cout << "" << G4endl;
    G4cout << "        Barrel  Top  Bottom  Total" << G4endl;
    G4cout << "IDPMT " << pmt20_count_barrel << " " << pmt20_count_top << " " << pmt20_count_bottom << " " << pmt20_count_total << G4endl;
    G4cout << "ODPMT " << pmtod_count_barrel << " " << pmtod_count_top << " " << pmtod_count_bottom << " " << pmtod_count_total << G4endl;
    G4cout << "mPMT  " << pmtmulti_count_barrel << " " << pmtmulti_count_top << " " << pmtmulti_count_bottom << " " << pmtmulti_count_total << G4endl;
    G4cout << "" << G4endl;

    // -------------------------------------
    // ID PMT Creation (copied from WCSIM)
    // -------------------------------------
    G4LogicalVolume* logicWCPMT;
    if(nID_PMTs<=1) logicWCPMT = ConstructPMT(WCPMTName, WCIDCollectionName,"tank",nID_PMTs);
    else logicWCPMT = ConstructMultiPMT(WCPMTName, WCIDCollectionName,"tank",nID_PMTs);
    if(!logicWCPMT){
      G4cerr << "Overlapping PMTs in multiPMT" << G4endl;
      return NULL; 
    }
    G4LogicalVolume* logicWCPMT2 = nullptr;
  #ifdef DEBUG
    G4cout << "HYBRID2 = " << hybrid << G4endl;
  #endif
    if(hybrid){
      G4cout<<"First type of PMT LV is created. Now creating the LV for the second type of PMT"<<G4endl;
      if(nID_PMTs2<=1) logicWCPMT2 = ConstructPMT(WCPMTName2, WCIDCollectionName2,"tankPMT2",nID_PMTs2);
      else logicWCPMT2 = ConstructMultiPMT(WCPMTName2, WCIDCollectionName2,"tankPMT2",nID_PMTs2);
      if(!logicWCPMT2){
        G4cerr << "Overlapping PMTs in multiPMT" << G4endl;
        return NULL; 
      }
    }

    G4String pmtname = "WCMultiPMT";

    G4VisAttributes* invisible= new G4VisAttributes(G4Colour(0.0,1.0,0.0));
    invisible->SetForceLineSegmentsPerCircle(24);
    invisible->SetForceAuxEdgeVisible(0);
    invisible->SetForceWireframe(1);
    invisible->SetVisibility(0);
    invisible->SetForceAuxEdgeVisible(0);

    G4VisAttributes* colored= new G4VisAttributes(G4Colour(0.0,1.0,0.0));
    colored->SetForceLineSegmentsPerCircle(24);
    colored->SetForceAuxEdgeVisible(0);
    colored->SetForceWireframe(1);
    colored->SetVisibility(1);
    colored->SetForceAuxEdgeVisible(0);
    colored->SetDaughtersInvisible(1);

     G4VisAttributes* colored2= new G4VisAttributes(G4Colour(1.0,0.0,0.0));
    colored2->SetForceLineSegmentsPerCircle(24);
    colored2->SetForceAuxEdgeVisible(0);
    colored2->SetForceWireframe(1);
    colored2->SetVisibility(1);
    colored2->SetForceAuxEdgeVisible(0);
    colored2->SetDaughtersInvisible(1);

    SetNestedVisAttributes(logicWCPMT, invisible );
    SetNestedVisAttributes(logicWCPMT2, invisible );

    logicWCPMT->GetDaughter(0)->GetLogicalVolume()->SetVisAttributes(colored);     
    logicWCPMT2->GetDaughter(0)->GetLogicalVolume()->SetVisAttributes(colored2);  

    // -------------------------------------
    // ID PMT Placement
    // -------------------------------------
    // Now that the dummies are placed, we need to swap them out
    // for real PMT logicals. This is also needed because copy no
    // needs to update for each one.

  // Possibe the ordering?
    int ndaughters = InnerDetectorLogic->GetNoDaughters();
    int copyno = 0;
    std::vector<G4Transform3D> positions;

    // Option remove oof percantage of PMTs.
    int odpmt_x_in_every_100 = 100; //atoi(std::string(getenv("ODPMT_IN_EVERY")).c_str());
    int pmt20_x_in_every_100 = 100; //atoi(std::string(getenv("IDPMT_IN_EVERY")).c_str());
    int mpmt_x_in_every_100 = 100; //atoi(std::string(getenv("MPMT_IN_EVERY")).c_str());

    int removed = 0;
    int pmt_throw_count = 0;
    int mpmt_throw_count = 0;

    for (int i = 0; i < ndaughters; i++){
      G4VPhysicalVolume* vol = InnerDetectorLogic->GetDaughter(i-removed);
      G4Transform3D aTransform = G4Transform3D(*(vol->GetObjectRotation()), vol->GetObjectTranslation());

      // Skip non-PMTS
      if (vol->GetLogicalVolume() != pmt20_dummy_logic) continue;
      pmt_throw_count++;

      InnerDetectorLogic->RemoveDaughter(vol);

      if (G4UniformRand()*100 < pmt20_x_in_every_100){

        if ( vol->GetLogicalVolume() == pmt20_dummy_logic ) {

          new G4PVPlacement(
              aTransform,
              logicWCPMT,              // pmt20
              pmtname,       // its name
              InnerDetectorLogic,       // its mother volume
              false,                    // no boolean operations
              copyno++,
              false
          );   

        }

      }
      delete vol;
      removed++;
    }

    ndaughters = InnerDetectorLogic->GetNoDaughters();
    removed = 0;
    for (int i = 0; i < ndaughters; i++){
      G4VPhysicalVolume* vol = InnerDetectorLogic->GetDaughter(i-removed);
      G4Transform3D aTransform = G4Transform3D(*(vol->GetObjectRotation()), vol->GetObjectTranslation());

      // Skip non-PMTS
      if (vol->GetLogicalVolume() != pmtmulti_dummy_logic) continue;
      mpmt_throw_count++;

      InnerDetectorLogic->RemoveDaughter(vol);
  
      if (G4UniformRand()*100 < mpmt_x_in_every_100){

        if ( vol->GetLogicalVolume() == pmtmulti_dummy_logic ) {

            new G4PVPlacement(
                aTransform,
                logicWCPMT2,              // pmt20
                "WCPMT",       // its name
                InnerDetectorLogic,       // its mother volume
                false,                    // no boolean operations
                copyno++,
                false
            );   
        } 
      }
      delete vol;
      removed++;
    }

    ndaughters = InnerDetectorLogic->GetNoDaughters();
    // -------------------------------------
    // OD PMT Creation and Placement
    // -------------------------------------

    // Reapeat the re-placement for the OD
    logicWCODWLSAndPMT = ConstructPMTAndWLSPlate(WCPMTODName, WCODCollectionName, "OD");

    logicWCODWLSAndPMT->SetVisAttributes(pmtmulti_dummy_colour);        

    copyno = 0;
    removed = 0;
    ndaughters = OuterDetectorLogic->GetNoDaughters();
    for (int i = 0; i < ndaughters; i++){
      G4VPhysicalVolume* vol = OuterDetectorLogic->GetDaughter(i-removed);
      G4Transform3D aTransform = G4Transform3D(*(vol->GetObjectRotation()), vol->GetObjectTranslation());

      // Skip non-PMTS
      if (vol->GetLogicalVolume() != pmtod_dummy_logic) continue;

      OuterDetectorLogic->RemoveDaughter(vol);

      if (G4UniformRand()*100 < odpmt_x_in_every_100){
        if ( vol->GetLogicalVolume() == pmtod_dummy_logic ) {
          if (isODConstructed){
            new G4PVPlacement(
                aTransform,
                logicWCODWLSAndPMT,              // pmt20
                "WCBorderCellODContainer",       // its name
                OuterDetectorLogic,       // its mother volume
                false,                    // no boolean operations
                copyno++,
                false
            );   
          }
        }
      }
      delete vol;
      removed++;
    }
    
    G4cout << "Final Outer N Daugters : " << OuterDetectorLogic->GetNoDaughters() << G4endl;
    G4cout << "Final Inner N Daugters : " << InnerDetectorLogic->GetNoDaughters() << G4endl;

    // -------------------------------------
    // Optical Surfaces
    // -------------------------------------
    // Because we made a nice nested hierarchy above
    // we only have three surfaces to add to make sure light 
    // propogation is handled correctly.
    
    // Nested structure means we should just need one skinsurfaces for all the tyveks.
    new G4LogicalSkinSurface("WallTyvekSurface",WallTyvekLogic,OpWaterTySurface);
    new G4LogicalSkinSurface("WhiteTyvekSurface",WhiteTyvekLogic,OpWaterTySurface);
    new G4LogicalSkinSurface("BlackTyvekSurface",BlackTyvekLogic,OpWaterBSSurface);

    new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
                InnerDetectorPhysical,
                BlackTyvekPhysical, 
                OpWaterBSSurface);

    new G4LogicalBorderSurface("WaterBSBarrelCellSurface2",
                OuterDetectorPhysical,
                WhiteTyvekPhysical, 
                OpWaterTySurface);

    new G4LogicalBorderSurface("WaterBSBarrelCellSurface3",
                OuterDetectorPhysical,
                WallTyvekPhysical, 
                OpWaterTySurface);
    
    int pmt20_count_final    = CountLogicalChildren(InnerDetectorLogic, pmt20_dummy_logic);
    int pmtmulti_count_final = CountLogicalChildren(InnerDetectorLogic, pmtmulti_dummy_logic);
    int pmtod_count_final    = CountLogicalChildren(OuterDetectorLogic, pmtod_dummy_logic);

    G4cout << "Final Values : " << pmt20_count_final << " " << pmtmulti_count_final << " " << pmtod_count_final << G4endl;

    G4cout << "FINISHED Realistic Placement" << G4endl;

    return rockShellLogic;
}