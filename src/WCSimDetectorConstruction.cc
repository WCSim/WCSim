#include "WCSimDetectorConstruction.hh"
#include "WCSimDetectorMessenger.hh"
#include "WCSimTuningParameters.hh"

#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4VisAttributes.hh"
#include "G4VisExtent.hh"

#include "G4RunManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <tuple>
#include <algorithm>


namespace {
  //-----------------------------------------------------
  // Local helper functions in anonymous namespace
  //-----------------------------------------------------

  // Iterator based interpolation function.
  //   Used in WCSimDetectorConstruction::CreateCombinedPMTQE()
  //   WCSimDetectorConstruction::GetPMTQE() is not quite satisfactory for
  //   interpolating combined QE functions due to its hard-coded limits and
  //   behavior at its end points.
  template <typename X, typename Y>
  Y linterpolate(const X& x,
                 const X* x_begin, const X* x_end, const Y* y_begin){
    auto x_j = std::upper_bound(x_begin, x_end, x);
    if( x_j == x_begin ) {
      return *y_begin;
    } else if( x_j == x_end ) {
      return *(y_begin + (x_end - x_begin) - 1);
    }
    else {
      auto x_i = x_j - 1;
      auto y_j = y_begin + (x_j - x_begin);
      auto y_i = y_j - 1;
      return *y_i + (*y_j - *y_i) * (x - *x_i) / (*x_j - *x_i);
    }
  }
}

std::map<int, G4Transform3D> WCSimDetectorConstruction::tubeIDMap;
std::map<int, std::pair<int, int> > WCSimDetectorConstruction::mPMTIDMap;
std::map<int, G4Transform3D> WCSimDetectorConstruction::tubeIDMap2;
std::map<int, std::pair<int, int> > WCSimDetectorConstruction::mPMTIDMap2;
std::map<int, G4Transform3D> WCSimDetectorConstruction::ODtubeIDMap;
std::map<int, std::pair<int, int> > WCSimDetectorConstruction::mPMTODMap;
//std::map<int, cyl_location>  WCSimDetectorConstruction::tubeCylLocation;

// std::hash is default hash function actually (http://en.cppreference.com/w/cpp/utility/hash)
// with operator() already properly defined.
std::unordered_map<std::string, int, std::hash<std::string> >         
WCSimDetectorConstruction::tubeLocationMap;
std::unordered_map<std::string, int, std::hash<std::string> >         
WCSimDetectorConstruction::tubeLocationMap2;
std::unordered_map<std::string, int, std::hash<std::string> >
  WCSimDetectorConstruction::ODtubeLocationMap;



WCSimDetectorConstruction::WCSimDetectorConstruction(G4int DetConfig,
						     WCSimTuningParameters* WCSimTuningPars):
  WCSimTuningParams(WCSimTuningPars),
  placeBGOGeometry(false),
  totalNum_mPMTs(0),
  totalNum_mPMTs2(0)
{
	
  // Decide if (only for the case of !1kT detector) should be upright or horizontal
  isUpright = false;
  isEggShapedHyperK  = false;
  isNuPrism  = false;
  isNuPrismBeamTest = false;
  isNuPrismBeamTest_16cShort = false;
  addCDS = false;

  rotateBarrelHalfTower = false;

  useReplica = true;
  pmtPosVar = 0;
  topRadiusChange = 0; midRadiusChange = 0; botRadiusChange = 0;
  readFromTable = false;
  pmt_blacksheet_offset = 0;
  WCBarrelPMTTopOffset = -1;
  WCBarrelPMTBotOffset = -1;

  debugMode = false;

  isODConstructed = false;
  isCombinedPMTCollectionDefined = false;
  odEdited = false;

  isRealisticPlacement = false;

  myConfiguration = DetConfig;

  BGOX = 0.; BGOY = 0.; BGOZ = 0.;

  //-----------------------------------------------------
  // Create Materials
  //-----------------------------------------------------

  ConstructMaterials();

  //-----------------------------------------------------
  // Initialize things related to the tubeID
  //-----------------------------------------------------

  WCSimDetectorConstruction::tubeIDMap.clear();
  WCSimDetectorConstruction::mPMTIDMap.clear();
  WCSimDetectorConstruction::tubeLocationMap.clear();

  WCSimDetectorConstruction::tubeIDMap2.clear();
  WCSimDetectorConstruction::mPMTIDMap2.clear();
  WCSimDetectorConstruction::tubeLocationMap2.clear();

  WCSimDetectorConstruction::ODtubeIDMap.clear();
  WCSimDetectorConstruction::mPMTODMap.clear();
  WCSimDetectorConstruction::ODtubeLocationMap.clear();

  WCSimDetectorConstruction::PMTLogicalVolumes.clear();

  totalNumPMTs = 0;
  totalNumODPMTs = 0;
  totalNumPMTs2 = 0;
  WCPMTExposeHeight= 0.;
  WCPMTExposeHeight2= 0.;
  WCPMTODExposeHeight= 0.;

  WCPMTODRadius = 0.;
  WCODLateralWaterDepth = 0.;
  WCODHeightWaterDepth = 0.;
  WCBlackSheetThickness = 0.;
  WCODDeadSpace = 0.; 
  WCODTyvekSheetThickness = 0.;

  //---------------------------------------------------
  // Need to define defaults for all mPMT parameters 
  // defaults are chosen that they are valid for a SK detector
  //--------------------------------------------------
  vessel_cyl_height = 0.1*mm;   // LATER: when tested, default WITH PMT cover!
  vessel_radius_curv = 0.1*mm;
  vessel_radius = 0.1*mm;

  mPMT_vessel_cyl_height = 0.1*mm;   // LATER: when tested, default WITH PMT cover!
  mPMT_vessel_radius_curv = 0.1*mm;
  mPMT_vessel_radius = 0.1*mm;
  dist_pmt_vessel = 0.*mm;
  orientation = PERPENDICULAR;
  mPMT_ID_PMT = "PMT3inchR12199_02";
  mPMT_OD_PMT = "";
  mPMT_outer_material = "Water";
  mPMT_inner_material = "";
  mPMT_outer_material_d = 0.*CLHEP::mm;
  // Radius of cone at z=reflectorHeight
  id_reflector_height = 0.*CLHEP::mm;
  id_reflector_z_offset = 0.*CLHEP::mm;
  id_reflector_angle = 0.*CLHEP::deg; 
  // parameters related to filling the ID mPMT
  nID_PMTs = 1;   //per mPMT
  nID_PMTs2 = 1;   //per mPMT
  config_file = "";
  fix_nModules = false;
  mPMT_material_pmtAssembly = "Water";
  mPMT_pmt_openingAngle = 0.*CLHEP::deg;
  
  //-----------------------------------------------------
  // Set the default WC geometry.  This can be changed later.
  //-----------------------------------------------------

  //SetTestSinglemPMTGeometry();
  SetSuperKGeometry();
  //SetHyperKGeometry();

  //----------------------------------------------------- 
  // Set whether or not Pi0-specific info is saved
  //-----------------------------------------------------

  SavePi0Info(false);

  //-----------------------------------------------------
  // Set whether or not neutron capture info is saved
  //-----------------------------------------------------

  SaveCaptureInfo(false);
  
  //-----------------------------------------------------
  // Set the default method for implementing the PMT QE
  //-----------------------------------------------------
  // 3 == SensitiveDetector_Only (QE applied at PMT)
  SetPMT_QE_Method(3);

   //default is to use collection efficiency
  SetPMT_Coll_Eff(1);
  // set default visualizer to OGLSX
  SetVis_Choice("OGLSX");

  //----------------------------------------------------- 
  // Make the detector messenger to allow changing geometry
  //-----------------------------------------------------

  messenger = new WCSimDetectorMessenger(this);

  // Get WCSIM_BUILD_DIR
  const char *wcsimdirenv = std::getenv("WCSIM_BUILD_DIR");
  if (!(wcsimdirenv && wcsimdirenv[0])) { // make sure it's non-empty
    G4cout << "Note: WCSIM_BUILD_DIR not set. Exiting" << G4endl;
    exit(-1);
  }
  wcsimdir_path = wcsimdirenv;
}

#include "G4GeometryManager.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

void WCSimDetectorConstruction::UpdateGeometry()
{
 
  
  G4bool geomChanged = true;
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct(), geomChanged);
  // ToDo: need some error catching here for NULL Construct() cases
 
}



WCSimDetectorConstruction::~WCSimDetectorConstruction(){
  for (unsigned int i=0;i<fpmts.size();i++){
    delete fpmts.at(i);
  }
  fpmts.clear();
  for (unsigned int i=0;i<fpmts2.size();i++){
    delete fpmts2.at(i);
  }
  fpmts2.clear();
  for (unsigned int i=0;i<fODpmts.size();i++){
    delete fODpmts.at(i);
  }
  fODpmts.clear();
}

//G4ThreeVector WCSimDetectorConstruction::GetTranslationFromSettings()
//{
//    TFile* inFile = TFile::Open(fInputSettingsFilename);
//    if (!inFile){
//        G4ThreeVector trans;
//        return trans;
//    }
//
//    fSettingsTree = (TTree*) inFile->Get("Settings");
//    if (!fSettingsTree){
//        G4ThreeVector trans;
//        return trans;
//    }
//
//    double NuBeamAng;
//    double NuIdfdPos[3];
//    for(int i = 0; i < 3; ++i) NuIdfdPos[i] = 0;
//    fSettingsTree->SetBranchAddress("NuIdfdPos",NuIdfdPos); 
//    fSettingsTree->SetBranchAddress("NuBeamAng",&NuBeamAng); 
//
//    fSettingsTree->GetEntry(0);
//
//    G4ThreeVector trans(0, (NuIdfdPos[1] + TMath::Tan(NuBeamAng)*600.0), NuIdfdPos[2] + 600.0);
//
//    inFile->Close();
//    fInputSettingsFilename = "";
//    fSettingsTree = NULL;
//
//    return trans;
//
//}

G4VPhysicalVolume* WCSimDetectorConstruction::Construct()
{
  G4cout << G4endl << "STARTING WCSimDetectorConstruction::CONSTRUCT" << G4endl;
  G4GeometryManager::GetInstance()->OpenGeometry();

  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  G4LogicalBorderSurface::CleanSurfaceTable();
  G4LogicalSkinSurface::CleanSurfaceTable();
  WCSimDetectorConstruction::PMTLogicalVolumes.clear();
  //TF: for new mPMT (or make this into a function?)
  vNiC.clear();
  vAlpha.clear();
  vCircle.clear();
  vAzimOffset.clear();

  totalNumPMTs = 0;
  totalNumPMTs2 = 0;
  totalNumODPMTs = 0;

  totalNum_mPMTs = 0;
  totalNum_mPMTs2 = 0;

  //-----------------------------------------------------
  // Create Logical Volumes
  //-----------------------------------------------------

  // First create the logical volumes of the sub detectors.  After they are 
  // created their size will be used to make the world volume.
  // Note the order is important because they rearrange themselves depending
  // on their size and detector ordering.

  G4LogicalVolume* logicWCBox;

  // Select between HyperK and cylinder
  if (isRealisticPlacement) logicWCBox = ConstructRealisticPlacement();
  else if (isEggShapedHyperK) logicWCBox = ConstructEggShapedHyperK();
  else if (!useReplica) logicWCBox = ConstructCylinderNoReplica(); 
  else logicWCBox = ConstructCylinder(); 
    
  if(!logicWCBox){
    G4cerr << "Something went wrong in ConstructCylinder" << G4endl;
    return NULL;
  }
  G4cout << " WCLength       = " << WCLength/m << " m"<< G4endl;

  //-------------------------------

  // Now make the detector Hall.  The lengths of the subdectors 
  // were set above.

  G4ThreeVector position(0,0,0);// = GetTranslationFromSettings();

  if(isNuPrism) position.setY(WCIDVerticalPosition);

  G4cout << "position Y = " << position.y() << G4endl;


  G4double expHallLength = 3.*WCLength; //jl145 - extra space to simulate cosmic muons more easily

  G4cout << " expHallLength = " << expHallLength / m << G4endl;
  G4double expHallHalfLength = 0.5*expHallLength;

  // Increase hall size if necessary
  G4VisExtent extent = logicWCBox->GetSolid()->GetExtent();
  G4double max_extent = std::max({fabs(extent.GetXmin()),fabs(extent.GetXmax()),
                                  fabs(extent.GetYmin()),fabs(extent.GetYmax()),
                                  fabs(extent.GetZmin()),fabs(extent.GetZmax())});
  if (expHallHalfLength<max_extent) expHallHalfLength = max_extent;

  G4Box* solidExpHall = new G4Box("expHall",
				  expHallHalfLength + fabs(position.x()),
				  expHallHalfLength + fabs(position.y()),
				  expHallHalfLength + fabs(position.z()));
  
  G4LogicalVolume* logicExpHall = 
    new G4LogicalVolume(solidExpHall,
			G4Material::GetMaterial("Vacuum"),
			"expHall",
			0,0,0);

  // Now set the visualization attributes of the logical volumes.

  //   logicWCBox->SetVisAttributes(G4VisAttributes::Invisible);
  logicExpHall->SetVisAttributes(G4VisAttributes::Invisible);

  //----------------------------------------------------------
  //BGO Calling and Placement - Diego Costas 29/02/2024 
  // Place BGO only if command is set to true
  if (IsBGOGeometrySet()) {
      G4cout << "Placing AmBe source in geometry at (" << BGOX << ", " << BGOY << ", " << BGOZ << "), Y being the vertical axis" << G4endl;
      G4Tubs* solidBGO = new G4Tubs("solidBGO", 0., 2.5*cm, 2.5*cm, 0., 360.*deg);
      G4LogicalVolume* logicBGO = new G4LogicalVolume(solidBGO, BGO, "logicBGO");
      G4ThreeVector BGOpos(BGOX, BGOY, BGOZ);
      if(isNuPrism) // the input position is the final position after rotation
      {
        BGOpos.setY(BGOZ);
        BGOpos.setZ(-BGOY);
      }
      new G4PVPlacement(0, BGOpos, logicBGO, "BGO", logicWCBox, false, 0, false);
  }
  
  //-----------------------------------------------------

  //-----------------------------------------------------
  // Create and place the physical Volumes
  //-----------------------------------------------------

  // Experimental Hall
  G4VPhysicalVolume* physiExpHall = 
    new G4PVPlacement(0,G4ThreeVector(),
  		      logicExpHall,
  		      "expHall",
  		      0,false,0,true);

  // Water Cherenkov Detector (WC) mother volume
  // WC Box, nice to turn on for x and y views to provide a frame:

  G4RotationMatrix* rotationMatrix = new G4RotationMatrix;
  G4ThreeVector genPosition = G4ThreeVector(0., 0., WCPosition);

  if(isNuPrism){
      rotationMatrix->rotateX(90.*deg);
      rotationMatrix->rotateY(0.*deg);
      rotationMatrix->rotateZ(0.*deg);
      genPosition.setX(position.x());
      genPosition.setY(position.y());
      genPosition.setZ(position.z());
  }

  G4Transform3D transform(*rotationMatrix, genPosition);

  G4VPhysicalVolume* physiWCBox = 
    new G4PVPlacement(transform,
		      logicWCBox,
		      "WCBox",
		      logicExpHall,
		      false,
          0,
		      checkOverlaps);

  // Reset the tubeID and tubeLocation maps before refilling them
  tubeIDMap.clear();
  mPMTIDMap.clear();
  tubeLocationMap.clear();
  
  ODtubeIDMap.clear();
  mPMTODMap.clear();
  ODtubeLocationMap.clear();

  tubeIDMap2.clear();
  mPMTIDMap2.clear();
  tubeLocationMap2.clear();


  // Traverse and print the geometry Tree
  
  //  TraverseReplicas(physiWCBox, 0, G4Transform3D(), 
  //	   &WCSimDetectorConstruction::PrintGeometryTree) ;
  
  TraverseReplicas(physiWCBox, 0, G4Transform3D(), 
	           &WCSimDetectorConstruction::DescribeAndRegisterPMT) ;
  
  
  TraverseReplicas(physiWCBox, 0, G4Transform3D(), 
		   &WCSimDetectorConstruction::GetWCGeom) ;
  
  DumpGeometryTableToFile();

  
  G4cout << "FINISHED WCSimDetectorConstruction::CONSTRUCT" << G4endl;
  
  // Return the pointer to the physical experimental hall
  return physiExpHall;
}

WCSimPMTObject *WCSimDetectorConstruction::CreatePMTObject(G4String PMTType, G4String CollectionName)
{
  WCSimPMTObject* PMT = nullptr;
  if (PMTType == "PMT20inch"){
    PMT = new PMT20inch;
  }
  else if (PMTType == "PMT8inch"){
    PMT = new PMT8inch;
  }
  else if (PMTType == "PMT5inch"){
    PMT = new PMT5inch;
  }
  else if (PMTType == "PMT3inch"){
    PMT = new PMT3inch;
  }
  else if (PMTType == "PMT3inch_ETEL9302B"){
    PMT = new PMT3inch_ETEL9302B;
  }
  else if (PMTType == "PMT3inchGT"){
    PMT = new PMT3inchGT;
  }
  else if (PMTType == "PMT3inchR12199_02"){
    PMT = new PMT3inchR12199_02;
  }
  else if (PMTType == "PMT3inchR14374"){
    PMT = new PMT3inchR14374;
  }
  else if (PMTType == "PMT10inch"){
    PMT = new PMT10inch;
  }
  else if (PMTType == "PMT10inchHQE"){
    PMT = new PMT10inchHQE;
  }
  else if (PMTType == "PMT12inchHQE"){
    PMT = new PMT12inchHQE;
  }
  else if (PMTType == "HPD20inchHQE"){
    PMT = new HPD20inchHQE;
  }
  else if (PMTType == "HPD12inchHQE"){
    PMT = new HPD12inchHQE;
  }
  else if (PMTType == "BoxandLine20inchHQE"){
    PMT = new BoxandLine20inchHQE;
  }
  else if (PMTType == "BoxandLine12inchHQE"){
    PMT = new BoxandLine12inchHQE;
  }
  else if (PMTType == "PMT4inchR12199_02"){
    PMT = new PMT4inchR12199_02;
  }
  else if (PMTType == "PMT5inchR12199_02"){
    PMT = new PMT5inchR12199_02;
  }
  else if (PMTType == "PMT5inch"){
    PMT = new PMT5inch;
  }
  else if (PMTType == "PMT3inchR14374_WCTE"){
    PMT = new PMT3inchR14374_WCTE;
  }

  if(PMT == nullptr) {
    G4cout << PMTType << " is not a recognized PMT Type. Exiting WCSim." << G4endl;
    exit(1);
  }
  WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
  return PMT;
}

void WCSimDetectorConstruction::SaveOptionsToOutput(WCSimRootOptions * wcopt)
{
  wcopt->SetGeomHasOD(false);
  wcopt->SetDetectorName(WCDetectorName);
  wcopt->SetSavePi0(pi0Info_isSaved);
  wcopt->SetPMTQEMethod(PMT_QE_Method);
  wcopt->SetPMTCollEff(PMT_Coll_Eff);
  wcopt->SetGeomHasOD(isODConstructed);
}

//A function to recalculate the dimensions of the HKOD tank if the parameters are changed
void WCSimDetectorConstruction::UpdateODGeo()
{
  WCODCollectionName = WCDetectorName + "-glassFaceWCPMT_OD";
  WCODDiameter = WCIDDiameter + 2*(WCBlackSheetThickness+WCODDeadSpace+WCODTyvekSheetThickness+WCODWLSPlatesThickness);

  WCODCapPMTSpacing  = (pi*WCIDDiameter/(round(WCIDDiameter*sqrt(pi*WCPMTODPercentCoverage)/(10.0*WCPMTODRadius))));
  if( WCPMTODPercentCoverageTop )
    WCODCapPMTSpacingTop  = (pi*WCIDDiameter/(round(WCIDDiameter*sqrt(pi*WCPMTODPercentCoverageTop)/(10.0*WCPMTODRadius))));
  if( WCPMTODPercentCoverageBottom )
    WCODCapPMTSpacingBottom  = (pi*WCIDDiameter/(round(WCIDDiameter*sqrt(pi*WCPMTODPercentCoverageBottom)/(10.0*WCPMTODRadius))));
  WCODCapEdgeLimit = std::min(WCIDDiameter/2.0 - WCPMTODRadius, WCIDDiameter/2.0 - WCODWLSPlatesLength/2);

  std::vector<G4String> WCColName;
  WCColName.push_back(WCIDCollectionName);
  WCColName.push_back(WCODCollectionName);
  CreateCombinedPMTQE(WCColName);
}

void WCSimDetectorConstruction::CreateCombinedPMTQE(const std::vector<G4String> & CollectionName){

  // Show printouts for debugging purposes
  G4cout << G4endl;
  G4cout << " ************************* " << G4endl;
  G4cout << " ** CreateCombinedPMTQE ** " << G4endl;
  G4cout << " ************************* " << G4endl;


  // Parallel vectors for information retrieved from master objects of PMT types
  const unsigned nPMTs = CollectionName.size();
  std::vector<WCSimPMTObject *> PMT;
  PMT.reserve(nPMTs);
  // begin and end pointers of wavelength arrays
  std::vector<std::pair<const G4double *, const G4double *>> wl_be;
  wl_be.reserve(nPMTs);
  // begin and end pointers of QE arrays
  std::vector<std::pair<const G4double *, const G4double *>> qe_be;
  qe_be.reserve(nPMTs);

  // Limit values determined across all PMTs in the collection.
  G4double maxQE = 0.;

  for(unsigned iPMT = 0; iPMT < nPMTs; iPMT++){
    auto aPMT = GetPMTPointer(CollectionName[iPMT]);
    PMT[iPMT] = aPMT;

    const int nwl = aPMT->GetNbOfQEDefined();
    const G4double* awl_b = aPMT->GetQEWavelength();
    wl_be[iPMT] = std::make_pair(awl_b, awl_b + nwl);

    const G4double* aqe_b = aPMT->GetQE();
    qe_be[iPMT] = std::make_pair(aqe_b, aqe_b + nwl);

    // Update limits
    if( iPMT == 0 ){
      maxQE = aPMT->GetmaxQE();
    }
    else {
      maxQE = std::max(maxQE, aPMT->GetmaxQE());
    }

    // Dump QE function for this PMT type
    // Debugging output
    G4cout << G4endl;
    G4cout << "### Recover PMT collection name "
           << CollectionName[iPMT] << G4endl;
    for(int iwl = 0; iwl < nwl; iwl++){
      G4cout << "wavelength[" << *(awl_b + iwl) <<"nm] : " << *(aqe_b + iwl)
             << G4endl;
    }
  }

  // Concatenate WL vec and remove duplicates
  std::map<G4double,G4double> QE;
  std::vector<G4double> qe_interps;
  qe_interps.reserve(nPMTs);

  for(unsigned iPMT = 0; iPMT < nPMTs; iPMT++){
    for(auto pwl = wl_be[iPMT].first; pwl != wl_be[iPMT].second; pwl++){
      // Interpolate QE curves
      qe_interps.clear();
      for(unsigned jPMT = 0; jPMT < nPMTs; jPMT++){
        qe_interps.push_back(linterpolate(*pwl,
                wl_be[jPMT].first, wl_be[jPMT].second, qe_be[jPMT].first));
      }
      QE[*pwl] = *std::max_element(qe_interps.begin(), qe_interps.end());
    }
  }

  // Dump composite QE
  // Debugging output
  G4cout << G4endl;
  G4cout << "### Combined PMT QE function" << G4endl;
  for(auto itr = QE.begin(); itr != QE.end(); itr++){
    G4cout << " ### " << itr->first << "nm : " << itr->second << G4endl;
  }
  G4cout << G4endl;

  // Create a new PMT with an extended QE array containing all PMT collection
  WCSimBasicPMTObject *newPMT = new WCSimBasicPMTObject(QE);
  newPMT->SetmaxQE(maxQE);
  newPMT->DefineQEHist(QE);
  SetBasicPMTObject(newPMT);
}

WCSimWLSProperties *WCSimDetectorConstruction::CreateWLSObject(G4String WLSType){

  if (WLSType == "EljenEJ286"){
    WCSimWLSProperties* WLS = new EljenEJ286;
    WCSimDetectorConstruction::SetWLSPointer(WLS);
    return WLS;
  }else if (WLSType == "Kuraray"){
    WCSimWLSProperties* WLS = new Kuraray;
    WCSimDetectorConstruction::SetWLSPointer(WLS);
    return WLS;
  }else if (WLSType == "Inr"){
    WCSimWLSProperties* WLS = new Inr;
    WCSimDetectorConstruction::SetWLSPointer(WLS);
    return WLS;
  }

  else { G4cout << WLSType << " is not a recognized WLS Type. Exiting WCSim." << G4endl; exit(1);}

}
