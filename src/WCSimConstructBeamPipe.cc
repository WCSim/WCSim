#include "WCSimDetectorConstruction.hh"

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

  //////////////////////////////
  /// 3) Beam window         ///
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