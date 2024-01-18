#include "WCSimDetectorConstruction.hh"
#include "WCSimDetectorMessenger.hh"
#include "WCSimTuningParameters.hh"

#include "G4Material.hh"
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
std::map<int, G4Transform3D> WCSimDetectorConstruction::ODtubeIDMap;
//std::map<int, cyl_location>  WCSimDetectorConstruction::tubeCylLocation;

// std::hash is default hash function actually (http://en.cppreference.com/w/cpp/utility/hash)
// with operator() already properly defined.
std::unordered_map<std::string, int, std::hash<std::string> >
  WCSimDetectorConstruction::tubeLocationMap;
std::unordered_map<std::string, int, std::hash<std::string> >
  WCSimDetectorConstruction::ODtubeLocationMap;

WCSimDetectorConstruction::WCSimDetectorConstruction(G4int DetConfig,WCSimTuningParameters* WCSimTuningPars):WCSimTuningParams(WCSimTuningPars)
{
	
  // Decide if (only for the case of !1kT detector) should be upright or horizontal
  isUpright = false;
  isEggShapedHyperK  = false;

  debugMode = false;

  isODConstructed = false;
  isCombinedPMTCollectionDefined = false;

  myConfiguration = DetConfig;

  //-----------------------------------------------------
  // Create Materials
  //-----------------------------------------------------
    
  ConstructMaterials();

  //-----------------------------------------------------
  // Initialize things related to the tubeID
  //-----------------------------------------------------

  WCSimDetectorConstruction::tubeIDMap.clear();
  WCSimDetectorConstruction::ODtubeIDMap.clear();
  //WCSimDetectorConstruction::tubeCylLocation.clear();// (JF) Removed
  WCSimDetectorConstruction::tubeLocationMap.clear();
  WCSimDetectorConstruction::ODtubeLocationMap.clear();
  WCSimDetectorConstruction::PMTLogicalVolumes.clear();
  totalNumPMTs = 0;
  totalNumODPMTs = 0;
  WCPMTExposeHeight= 0.;
  WCBorderPMTOffset= 0.;
  //-----------------------------------------------------
  // Set the default WC geometry.  This can be changed later.
  //-----------------------------------------------------

  SetSuperKGeometry();
  //SetHyperKGeometry();

  //----------------------------------------------------- 
  // Set whether or not Pi0-specific info is saved
  //-----------------------------------------------------

  SavePi0Info(false);
  
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
}

#include "G4GeometryManager.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

void WCSimDetectorConstruction::UpdateGeometry()
{
 
  
  G4bool geomChanged = true;
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct(), geomChanged);
 
 }



WCSimDetectorConstruction::~WCSimDetectorConstruction(){
  for (unsigned int i=0;i<fpmts.size();i++){
    delete fpmts.at(i);
  }
  fpmts.clear();
  for (unsigned int i=0;i<fODpmts.size();i++){
    delete fODpmts.at(i);
  }
  fODpmts.clear();
}

G4VPhysicalVolume* WCSimDetectorConstruction::Construct()
{  
  G4GeometryManager::GetInstance()->OpenGeometry();

  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  G4LogicalBorderSurface::CleanSurfaceTable();
  G4LogicalSkinSurface::CleanSurfaceTable();
  WCSimDetectorConstruction::PMTLogicalVolumes.clear();

  totalNumPMTs = 0;
  totalNumODPMTs = 0;

  //-----------------------------------------------------
  // Create Logical Volumes
  //-----------------------------------------------------

  // First create the logical volumes of the sub detectors.  After they are 
  // created their size will be used to make the world volume.
  // Note the order is important because they rearrange themselves depending
  // on their size and detector ordering.

  G4LogicalVolume* logicWCBox;
  // Select between egg-shaped HyperK and cylinder
  if (isEggShapedHyperK) logicWCBox = ConstructEggShapedHyperK();
  else logicWCBox = ConstructCylinder();

  G4cout << " WCLength       = " << WCLength/m << " m"<< G4endl;

  //-------------------------------

  // Now make the detector Hall.  The lengths of the subdectors 
  // were set above.

  G4double expHallLength = 3.*WCLength; //jl145 - extra space to simulate cosmic muons more easily

  G4cout << " expHallLength = " << expHallLength / m << G4endl;
  G4double expHallHalfLength = 0.5*expHallLength;

  G4Box* solidExpHall = new G4Box("expHall",
				  expHallHalfLength,
				  expHallHalfLength,
				  expHallHalfLength);
  
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

	  //G4RotationMatrix* rotationMatrix = new G4RotationMatrix;
	  //rotationMatrix->rotateX(90.*deg);
	  //rotationMatrix->rotateZ(90.*deg);

  G4ThreeVector genPosition = G4ThreeVector(0., 0., WCPosition);
  G4VPhysicalVolume* physiWCBox = 
    new G4PVPlacement(0,
		      genPosition,
		      logicWCBox,
		      "WCBox",
		      logicExpHall,
		      false,
		      0);

  // Reset the tubeID and tubeLocation maps before refiling them
  tubeIDMap.clear();
  tubeLocationMap.clear();
  ODtubeIDMap.clear();
  ODtubeLocationMap.clear();


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

  else { G4cout << PMTType << " is not a recognized PMT Type. Exiting WCSim." << G4endl; exit(1);}
}

void WCSimDetectorConstruction::SaveOptionsToOutput(WCSimRootOptions * wcopt)
{
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

void WCSimDetectorConstruction::CreateCombinedPMTQE(std::vector<G4String> CollectionName){

  // Show printouts for debugging purposes
  G4cout << G4endl;
  G4cout << " ************************* " << G4endl;
  G4cout << " ** CreateCombinedPMTQE ** " << G4endl;
  G4cout << " ************************* " << G4endl;

  // Define relevant variable
  // Create array of maps for CollectionName
  std::vector< std::map<G4double,G4double> > QEMap;
  std::vector<G4double> maxQEVec;
  // Need size of QE array
  std::vector<G4int> NbOfWLBins;

  // Recover QE for collection name
  std::vector<WCSimPMTObject*> PMT;
  // F. Nova: here get values from BoxandLine20inchHQE and from PMT8inch
  for(unsigned int iPMT=0;iPMT<CollectionName.size();iPMT++){
    // Access PMT pointer
    PMT.push_back(GetPMTPointer(CollectionName[iPMT]));

    // Recover WL and QE infos
    G4double *wavelength = (PMT[iPMT]->GetQEWavelength());
    G4double *QE = (PMT[iPMT]->GetQE());

    std::map<G4double,G4double> hist;
    G4cout << G4endl;
    G4cout << "### Recover PMT collection name "
           << CollectionName[iPMT] << G4endl;
    for(int iWL=0;iWL<PMT[iPMT]->GetNbOfQEDefined();iWL++){
      hist[wavelength[iWL]]=QE[iWL];
      G4cout << "wavelength[" << wavelength[iWL] <<"nm] : " << QE[iWL] << G4endl;
    }

    QEMap.push_back(hist);
    maxQEVec.push_back(PMT[iPMT]->GetmaxQE());
    NbOfWLBins.push_back(PMT[iPMT]->GetNbOfQEDefined());
  }

  // Concatenate WL vec and remove duplicate
  std::map<G4double,G4double> QE;

  // Recursive algorithm to set new QE for combined PMT collection
  // F. Nova: define QE for a given wavelength as the highest QE in the collection for that wavelength
  // max(QE1, QE, ...), where each QEi is the value of QE for PMT type i
  G4double max_QE = 0.;
  G4cout << G4endl;
  for(unsigned int iCol=0; iCol<QEMap.size();iCol++){
    for(std::map<G4double, G4double>::iterator it=QEMap[iCol].begin(); it!=QEMap[iCol].end(); ++it){
      if(iCol+1<QEMap.size()){
        std::map<G4double, G4double>::iterator foundWL = QEMap[iCol+1].find(it->first);
        if(foundWL == QEMap[iCol+1].end()){
          G4cout << "Undefined QE in next collection" << G4endl;
          G4cout << "Will add it" << G4endl;
          G4cout << " ### " << it->first << "nm : " << it->second << G4endl;
          QE[it->first]=it->second;
        } else {

	        max_QE = std::max(it->second, foundWL->second);
          G4cout << "New QE defined for " << it->first << "nm is "
                 << max_QE << G4endl;
          QE[it->first]=max_QE;
        }
      } else {
        break;
      }
    }
  }

  G4cout << G4endl;

  // Need to make a special case for the last collection
  int iCol = QEMap.size()-1;
  if(iCol>0) {
    for (std::map<G4double, G4double>::iterator it = QEMap[iCol].begin(); it != QEMap[iCol].end(); ++it) {
      std::map<G4double, G4double>::iterator foundWL = QEMap[iCol - 1].find(it->first);
      if (foundWL == QEMap[iCol - 1].end()) {
        G4cout << G4endl;
        G4cout << "Special case for last collection" << G4endl;
        G4cout << " ### " << it->first << "nm : " << it->second << G4endl;
        QE[it->first] = it->second;
      }
    }
  }
  G4cout << G4endl;

  // Let's debug this one last time :
  std::map<G4double, G4double>::iterator itr;
  for(itr = QE.begin(); itr != QE.end(); itr++){
    G4cout << " ### " << itr->first << "nm : " << itr->second << G4endl;
  }
  G4cout << G4endl;

  // Create a new PMT with an extended QE array containing all PMT collection
  WCSimBasicPMTObject *newPMT = new WCSimBasicPMTObject(QE);
  newPMT->SetmaxQE(*std::max_element(maxQEVec.begin(),maxQEVec.end()));
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
