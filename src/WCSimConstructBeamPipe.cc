#include "WCSimDetectorConstruction.hh"

#include "G4Box.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4Polycone.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalSkinSurface.hh"

#include "G4SystemOfUnits.hh"


// WCTE beam pipe logical volume construction.
// A function of the WCSimDetectorConstruction class
// dimensions are from the design drawing and hard-coded
G4LogicalVolume* WCSimDetectorConstruction::ConstructBeamPipe()
{
  /////////////////////////////////////////
  // From design drawings
  G4double pipeInnerR = 75*mm;
  G4double pipeOuterR = 77*mm;
  G4double windowThickness = 1.2*mm;
  G4double flangeThickness = 8*mm;
  G4double flangeR = 100*mm;

  G4double window_blacksheet_distance = 188*mm;

  //////////////////////////////////////////////////////////////
  /// 1) Outer logical volume: fill with stainless steel     ///
  //////////////////////////////////////////////////////////////
  G4double pipe_zRange_outer[6] = { 0,                         // start from zero for easier placement in ID
                                    pmt_blacksheet_offset+window_blacksheet_distance-flangeThickness,
                                    pmt_blacksheet_offset+window_blacksheet_distance-flangeThickness,
                                    pmt_blacksheet_offset+window_blacksheet_distance+windowThickness,
                                    pmt_blacksheet_offset+window_blacksheet_distance+windowThickness,
                                    pmt_blacksheet_offset+window_blacksheet_distance+windowThickness+flangeThickness};
  G4double pipe_RRange_outer[6] = { pipeOuterR,
                                    pipeOuterR,
                                    flangeR,
                                    flangeR,
                                    flangeR,
                                    flangeR};
  G4double pipe_rRange_outer[6] = { 0., 
                                    0., 
                                    0., 
                                    0., 
                                    pipeOuterR,
                                    pipeOuterR};

  // Although G4Tubs is more natural, Polycone is used to be in control of z position
  // and because z = 0 is position of bottom of cylinder, vs center in G4Tubs
  G4Polycone* solidBeamPipe = 
    new G4Polycone("BeamPipe",                    
		   0.0*deg,
		   360.0*deg,
		   6,
		   pipe_zRange_outer,
		   pipe_rRange_outer, // R Inner
		   pipe_RRange_outer);// R Outer
  G4LogicalVolume *logicBeamPipe =
    new G4LogicalVolume(    solidBeamPipe,
			    G4Material::GetMaterial("StainlessSteel"), 
			    "BeamPipe",
			    0,0,0);

  new G4LogicalSkinSurface("BeamPipeSkinSurface", logicBeamPipe, BSSkinSurface); // wrapped with black tyvek

  G4VisAttributes *pipeAttributes = new G4VisAttributes();
  pipeAttributes->SetColor(0.85, 0.85, 0.85, 1.0);
  pipeAttributes->SetVisibility(true);
  pipeAttributes->SetForceSolid(true);
  logicBeamPipe->SetVisAttributes(pipeAttributes);

  //////////////////////////////
  /// 2) Air inside pipe     ///
  //////////////////////////////
  G4double air_zRange_outer[6] = {  0,                       
                                    pmt_blacksheet_offset+window_blacksheet_distance };
  G4double air_RRange_outer[6] = {  pipeInnerR,
                                    pipeInnerR };
  G4double air_rRange_outer[6] = {  0., 
                                    0. };
  G4Polycone* solidPipeInterior = 
    new G4Polycone("PipeInterior",                    
		   0.0*deg,
		   360.0*deg,
		   2,
		   air_zRange_outer,
		   air_rRange_outer, // R Inner
		   air_RRange_outer);// R Outer
       
  G4LogicalVolume *logicPipeInterior =
    new G4LogicalVolume(    solidPipeInterior,
			    G4Material::GetMaterial("Air1"), 
			    "BeamPipeInterior");

  new G4PVPlacement(0,
                    G4ThreeVector(0.,0,0),
                    logicPipeInterior,
                    "BeamPipeInterior",
                    logicBeamPipe,
                    false,
                    0,
                    checkOverlaps);

  //////////////////////////////////////////////////
  /// 3) T5 TOF Scintillator                     ///
  //////////////////////////////////////////////////
  
  // -- Materials --
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* scintMaterial = G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE"); // EJ-228 equivalent
  G4Material* vinylMaterial = G4Material::GetMaterial("G4_POLYVINYL_CHLORIDE"); // PVC
  G4Material* mylarMaterial = G4Material::GetMaterial("G4_MYLAR");
  G4Material* airMaterial   = G4Material::GetMaterial("Air1"); // For the gaps inside the assembly

  // Check to ensure materials exist
  if (!scintMaterial || !vinylMaterial || !mylarMaterial) {
      G4cerr << "ERROR: T5 TOF Materials not defined in WCSimConstructMaterials.cc!" << G4endl;
  }

  // -- Dimensions (Layers) --
  // Stack along Z: Vinyl(3mm) + Air(15.75mm) + Mylar(0.05mm) + Scint(6.4mm) + Mylar(0.05mm) + Air(14.75mm) + Vinyl(3mm)
  G4double t_vinyl  = 3.0 * mm;
  G4double t_gap1   = 15.75 * mm;
  G4double t_mylar  = 0.05 * mm;
  G4double t_scint  = 6.4 * mm;
  G4double t_gap2   = 14.75 * mm;

  G4double totalAssemblyThickness = (2*t_vinyl) + t_gap1 + (2*t_mylar) + t_scint + t_gap2; // Should be 43.0 mm
  
  // -- Bar Dimensions (X/Y) --
  const G4int nBars = 8;
  G4double barHeight = 16.25 * mm; // Y dimension
  G4double barLengths[nBars] = {41*mm, 94*mm, 112*mm, 123*mm, 123*mm, 112*mm, 94*mm, 41*mm}; // X dimension

  // -- Visualization Attributes --
  G4VisAttributes *visScint = new G4VisAttributes(G4Colour::Cyan());
  visScint->SetVisibility(true); visScint->SetForceSolid(true);

  G4VisAttributes *visVinyl = new G4VisAttributes(G4Colour::Gray());
  visVinyl->SetVisibility(true); visVinyl->SetForceSolid(true);

  G4VisAttributes *visMylar = new G4VisAttributes(G4Colour::Magenta());
  visMylar->SetVisibility(true); visMylar->SetForceSolid(true);

  // -- Positioning Logic --
  // Starts in Z at: 5 mm from the start of the pipe
  // The start of the beam pipe volume is at z=0 in this Polycone construction.
  G4double z_assembly_start = 5.0 * mm;
  G4double z_assembly_center = z_assembly_start + (totalAssemblyThickness / 2.0);

  // Calculate Start Y to center the stack vertically
  G4double totalStackHeight = nBars * barHeight; 
  G4double startY = -totalStackHeight / 2.0;

  // -- Calculate local Z positions for components inside the assembly --
  // Center of assembly is Z=0. Start is -totalAssemblyThickness/2.
  G4double z0 = -totalAssemblyThickness / 2.0;

  G4double z_pos_vinyl1 = z0 + (t_vinyl/2.0);
  G4double z_pos_mylar1 = z0 + t_vinyl + t_gap1 + (t_mylar/2.0);
  G4double z_pos_scint  = z0 + t_vinyl + t_gap1 + t_mylar + (t_scint/2.0);
  G4double z_pos_mylar2 = z0 + t_vinyl + t_gap1 + t_mylar + t_scint + (t_mylar/2.0);
  G4double z_pos_vinyl2 = z0 + t_vinyl + t_gap1 + t_mylar + t_scint + t_mylar + t_gap2 + (t_vinyl/2.0);


  // -- Loop to construct each bar --
  for(G4int i=0; i<nBars; i++) {
      G4String suffix = "_Bar" + std::to_string(i+1);
      
      G4double halfLenX = barLengths[i] / 2.0;
      G4double halfHgtY = barHeight / 2.0;

      // 1. Create Container Volume (Air)
      G4Box* solidAssembly = new G4Box("T5_AssemblySolid" + suffix, 
                                       halfLenX, 
                                       halfHgtY, 
                                       totalAssemblyThickness/2.0);

      G4LogicalVolume* logicAssembly = new G4LogicalVolume(solidAssembly, 
                                                           airMaterial, 
                                                           "T5_AssemblyLogic" + suffix);
      logicAssembly->SetVisAttributes(G4VisAttributes::Invisible); // Hide container

      // 2. Create Component Solids (All share X/Y dimensions of the container)
      G4Box* sVinyl = new G4Box("T5_Vinyl"+suffix, halfLenX, halfHgtY, t_vinyl/2.0);
      G4Box* sMylar = new G4Box("T5_Mylar"+suffix, halfLenX, halfHgtY, t_mylar/2.0);
      G4Box* sScint = new G4Box("T5_Scint"+suffix, halfLenX, halfHgtY, t_scint/2.0);

      // 3. Create Component Logicals
      G4LogicalVolume* lVinyl = new G4LogicalVolume(sVinyl, vinylMaterial, "T5_Vinyl_Log"+suffix);
      lVinyl->SetVisAttributes(visVinyl);

      G4LogicalVolume* lMylar = new G4LogicalVolume(sMylar, mylarMaterial, "T5_Mylar_Log"+suffix);
      lMylar->SetVisAttributes(visMylar);

      G4LogicalVolume* lScint = new G4LogicalVolume(sScint, scintMaterial, "T5_Scint_Log"+suffix);
      lScint->SetVisAttributes(visScint);

      // 4. Place Components inside Assembly
      // Vinyl 1
      new G4PVPlacement(0, G4ThreeVector(0,0,z_pos_vinyl1), lVinyl, "T5_Vinyl1_Phys"+suffix, logicAssembly, false, 0, checkOverlaps);
      // Mylar 1
      new G4PVPlacement(0, G4ThreeVector(0,0,z_pos_mylar1), lMylar, "T5_Mylar1_Phys"+suffix, logicAssembly, false, 0, checkOverlaps);
      // Scintillator
      new G4PVPlacement(0, G4ThreeVector(0,0,z_pos_scint),  lScint, "T5_Scint_Phys"+suffix,  logicAssembly, false, 0, checkOverlaps);
      // Mylar 2
      new G4PVPlacement(0, G4ThreeVector(0,0,z_pos_mylar2), lMylar, "T5_Mylar2_Phys"+suffix, logicAssembly, false, 1, checkOverlaps);
      // Vinyl 2
      new G4PVPlacement(0, G4ThreeVector(0,0,z_pos_vinyl2), lVinyl, "T5_Vinyl2_Phys"+suffix, logicAssembly, false, 1, checkOverlaps);

      // 5. Place Assembly in Beam Pipe
      // Calculate Y position for this bar (i=0 is bottom)
      G4double yPos = startY + (i * barHeight) + halfHgtY;

      new G4PVPlacement(0,
                        G4ThreeVector(0, yPos, z_assembly_center),
                        logicAssembly,
                        "T5_Assembly_Phys" + suffix,
                        logicPipeInterior, 
                        false,
                        i,
                        checkOverlaps);
  }

  //////////////////////////////
  /// 4) Beam window         ///
  //////////////////////////////
  G4double window_zRange_outer[6] = { pmt_blacksheet_offset+window_blacksheet_distance,                       
                                      pmt_blacksheet_offset+window_blacksheet_distance+windowThickness };
  G4double window_RRange_outer[6] = { pipeOuterR,
                                      pipeOuterR };
  G4double window_rRange_outer[6] = { 0., 
                                      0. };
  G4Polycone* solidBeamWindow = 
    new G4Polycone("BeamWindow",                    
		   0.0*deg,
		   360.0*deg,
		   2,
		   window_zRange_outer,
		   window_rRange_outer, // R Inner
		   window_RRange_outer);// R Outer
       
  G4LogicalVolume *logicBeamWindow =
    new G4LogicalVolume(    solidBeamWindow,
			    G4Material::GetMaterial("StainlessSteel"), 
			    "BeamWindow");

  new G4PVPlacement(0,
                    G4ThreeVector(0.,0,0),
                    logicBeamWindow,
                    "BeamWindow",
                    logicBeamPipe,
                    false,
                    0,
                    checkOverlaps);

  new G4LogicalSkinSurface("BeamWindowSkinSurface", logicBeamWindow, ReflectorSkinSurface); // assume same as reflector (flat 90% reflectivity)

  return logicBeamPipe;
}