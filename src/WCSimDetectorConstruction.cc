#include "WCSimDetectorConstruction.hh"
#include "WCSimDetectorMessenger.hh"
#include "WCSimTuningParameters.hh"

#include "G4Element.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4VisAttributes.hh"

#include "G4RunManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

std::map<int, G4Transform3D> WCSimDetectorConstruction::tubeIDMap;
std::map<int, std::pair<int, int> > WCSimDetectorConstruction::mPMTIDMap;
//std::map<int, cyl_location>  WCSimDetectorConstruction::tubeCylLocation;
//hash_map<std::string, int, hash<std::string> > 
//WCSimDetectorConstruction::tubeLocationMap_old;    //deprecated !!

// std::hash is default hash function actually (http://en.cppreference.com/w/cpp/utility/hash)
// with operator() already properly defined.
std::unordered_map<std::string, int, std::hash<std::string> >         
WCSimDetectorConstruction::tubeLocationMap;

WCSimDetectorConstruction::WCSimDetectorConstruction(G4int DetConfig,WCSimTuningParameters* WCSimTuningPars):WCSimTuningParams(WCSimTuningPars)
{
	
  // Decide if (only for the case of !1kT detector) should be upright or horizontal
  isUpright = false;
  isEggShapedHyperK  = false;
  isNuPrism  = false;

  debugMode = false;

  myConfiguration = DetConfig;

  //-----------------------------------------------------
  // Create Materials
  //-----------------------------------------------------
    
  ConstructMaterials();

  //-----------------------------------------------------
  // Initialize things related to the tubeID
  //-----------------------------------------------------

  WCSimDetectorConstruction::tubeIDMap.clear();
  WCSimDetectorConstruction::mPMTIDMap.clear();
  //WCSimDetectorConstruction::tubeCylLocation.clear();// (JF) Removed
  WCSimDetectorConstruction::tubeLocationMap.clear();
  WCSimDetectorConstruction::PMTLogicalVolumes.clear();
  totalNumPMTs = 0;
  WCPMTExposeHeight= 0.;

  //---------------------------------------------------
  // Need to define defaults for all mPMT parameters 
  // defaults are chosen that they are valid for a SK detector
  //--------------------------------------------------
  vessel_cyl_height = 0.1*mm;   // LATER: when tested, default WITH PMT cover!
  vessel_radius_curv = 0.1*mm;
  vessel_radius = 0.1*mm;
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
  config_file = "";
  fix_nModules = false;
  mPMT_material_pmtAssembly = "Water";
  mPMT_pmt_openingAngle = 0.*CLHEP::deg;
  


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
  SetPMT_QE_Method(1);

   //default is to use collection efficiency
  SetPMT_Coll_Eff(1);
  // set default visualizer to OGLSX
  SetVis_Choice("OGLSX");


  //----------------------------------------------------- 
  // Make the detector messenger to allow changing geometry
  //-----------------------------------------------------

  messenger = new WCSimDetectorMessenger(this);


  // Get WCSIMDIR
  wcsimdir_path = std::getenv("WCSIMDIR");
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
//    float NuBeamAng;
//    float NuIdfdPos[3];
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
  
  //-----------------------------------------------------
  // Create Logical Volumes
  //-----------------------------------------------------

  // First create the logical volumes of the sub detectors.  After they are 
  // created their size will be used to make the world volume.
  // Note the order is important because they rearrange themselves depending
  // on their size and detector ordering.

  G4LogicalVolume* logicWCBox;

  // Select between HyperK and cylinder
  if (isEggShapedHyperK) logicWCBox = ConstructEggShapedHyperK();
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

  std::cout << "position Y = " << position.y() << std::endl;


  G4double expHallLength = 3.*WCLength; //jl145 - extra space to simulate cosmic muons more easily

  G4cout << " expHallLength = " << expHallLength / m << G4endl;
  G4double expHallHalfLength = 0.5*expHallLength;

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
		      0);

  // Reset the tubeID and tubeLocation maps before refilling them
  tubeIDMap.clear();
  mPMTIDMap.clear();
  tubeLocationMap.clear();


  // Traverse and print the geometry Tree
  
  //  TraverseReplicas(physiWCBox, 0, G4Transform3D(), 
  //	   &WCSimDetectorConstruction::PrintGeometryTree) ;
  
  TraverseReplicas(physiWCBox, 0, G4Transform3D(), 
	           &WCSimDetectorConstruction::DescribeAndRegisterPMT) ;
  
  
  TraverseReplicas(physiWCBox, 0, G4Transform3D(), 
		   &WCSimDetectorConstruction::GetWCGeom) ;
  
  DumpGeometryTableToFile();

  

  
  // Return the pointer to the physical experimental hall
  return physiExpHall;
}

WCSimPMTObject *WCSimDetectorConstruction::CreatePMTObject(G4String PMTType, G4String CollectionName)
{
  if (PMTType == "PMT20inch"){
     WCSimPMTObject* PMT = new PMT20inch;
     WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
     return PMT;
  }
  else if (PMTType == "PMT8inch"){
    WCSimPMTObject* PMT = new PMT8inch;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "PMT5inch"){
    WCSimPMTObject* PMT = new PMT5inch;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "PMT3inch"){
    WCSimPMTObject* PMT = new PMT3inch;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "PMT3inchGT"){
    WCSimPMTObject* PMT = new PMT3inchGT;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "PMT3inchR12199_02"){
    WCSimPMTObject* PMT = new PMT3inchR12199_02;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "PMT10inch"){
    WCSimPMTObject* PMT = new PMT10inch;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "PMT10inchHQE"){
    WCSimPMTObject* PMT = new PMT10inchHQE;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "PMT12inchHQE"){
    WCSimPMTObject* PMT = new PMT12inchHQE;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "HPD20inchHQE"){
    WCSimPMTObject* PMT = new HPD20inchHQE;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "HPD12inchHQE"){
    WCSimPMTObject* PMT = new HPD12inchHQE;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "BoxandLine20inchHQE"){
    WCSimPMTObject* PMT = new BoxandLine20inchHQE;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "BoxandLine12inchHQE"){
    WCSimPMTObject* PMT = new BoxandLine12inchHQE;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "PMT4inchR12199_02"){
    WCSimPMTObject* PMT = new PMT4inchR12199_02;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "PMT5inchR12199_02"){
    WCSimPMTObject* PMT = new PMT5inchR12199_02;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }

  else { G4cout << PMTType << " is not a recognized PMT Type. Exiting WCSim." << G4endl; exit(1);}
}

void WCSimDetectorConstruction::SaveOptionsToOutput(WCSimRootOptions * wcopt)
{
  wcopt->SetDetectorName(WCDetectorName);
  wcopt->SetSavePi0(pi0Info_isSaved);
  wcopt->SetPMTQEMethod(PMT_QE_Method);
  wcopt->SetPMTCollEff(PMT_Coll_Eff);
}
