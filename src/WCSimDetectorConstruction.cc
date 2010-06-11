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

std::map<int, G4Transform3D> WCSimDetectorConstruction::tubeIDMap;
//std::map<int, cyl_location>  WCSimDetectorConstruction::tubeCylLocation;
hash_map<std::string, int, hash<std::string> > 
WCSimDetectorConstruction::tubeLocationMap;

WCSimDetectorConstruction::WCSimDetectorConstruction(G4int DetConfig,WCSimTuningParameters* WCSimTuningPars):WCSimTuningParams(WCSimTuningPars)
{
	
  // Decide if (only for the case of !1kT detector) should be upright or horizontal
  isUpright = false;
  isMailbox =false;

  debugMode = false;
//-----------------------------------------------------
// Initilize SD pointers
//-----------------------------------------------------

      aWCPMT     = NULL;

  myConfiguration = DetConfig;

  //-----------------------------------------------------
  // Create Materials
  //-----------------------------------------------------
    
  ConstructMaterials();

  //-----------------------------------------------------
  // Initialize things related to the tubeID
  //-----------------------------------------------------

  WCSimDetectorConstruction::tubeIDMap.clear();
  //WCSimDetectorConstruction::tubeCylLocation.clear();// (JF) Removed
  WCSimDetectorConstruction::tubeLocationMap.clear();
  totalNumPMTs = 0;
  WCPMTExposeHeight= 0.;
  //-----------------------------------------------------
  // Set the default WC geometry.  This can be changed later.
  //-----------------------------------------------------

 // SetSuperKGeometry();
  DUSEL_100kton_10inch_40perCent();
  //DUSEL_100kton_10inch_HQE_30perCent();
  //DUSEL_100kton_10inch_HQE_30perCent_Gd();
  //DUSEL_150kton_10inch_HQE_30perCent();
 // SetMailBox300KTGeometry();


  //----------------------------------------------------- 
  // Set whether or not Pi0-specific info is saved
  //-----------------------------------------------------

  SavePi0Info(false);

  //----------------------------------------------------- 
  // Make the detector messenger to allow changing geometry
  //-----------------------------------------------------

  messenger = new WCSimDetectorMessenger(this);
}

#include "G4GeometryManager.hh"
#include "G4LogicalBorderSurface.hh"
void WCSimDetectorConstruction::UpdateGeometry()
{
  G4GeometryManager::GetInstance()->OpenGeometry();

  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  G4LogicalBorderSurface::CleanSurfaceTable();
  
  G4bool geomChanged = true;
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct(), geomChanged);
 
 }

WCSimDetectorConstruction::~WCSimDetectorConstruction(){;}


G4VPhysicalVolume* WCSimDetectorConstruction::Construct()
{  
  totalNumPMTs = 0;
  
  //-----------------------------------------------------
  // Create Logical Volumes
  //-----------------------------------------------------

  // First create the logical volumes of the sub detectors.  After they are 
  // created their size will be used to make the world volume.
  // Note the order is important because they rearrange themselves depending
  // on their size and detector ordering.

  G4LogicalVolume* logicWCBox;
  // Select between cylinder and mailbox
  if (isMailbox) logicWCBox = ConstructMailboxWC();
  else logicWCBox = ConstructWC(); 

  G4cout << " WCLength       = " << WCLength/m << " m"<< G4endl;
  G4cout << " WCRadius       = " << WCRadius<< " m"<< G4endl;
  //-------------------------------

  // Now make the detector Hall.  The lengths of the subdectors 
  // were set above.
 G4Colour  grey    (0.5, 0.5, 0.5) ;

  G4double expHallLength = 1.5*WCLength;

  G4cout << " expHallLength = " << expHallLength / m << G4endl;
  G4double expHallHalfLength = 0.5*expHallLength;

  G4Box* solidExpHall = new G4Box("expHall",
				  expHallHalfLength,
				  expHallHalfLength,
				  expHallHalfLength);
  
  G4LogicalVolume* logicExpHall = 
    new G4LogicalVolume(solidExpHall,
			G4Material::GetMaterial("Rock"),
			"expHall",
			0,0,0);

  // Now set the visualization attributes of the logical volumes.
  //G4VisAttributes* exp_vat= new G4VisAttributes(grey);
  // logicExpHall->SetVisAttributes(grey);

   // logicWCBox->SetVisAttributes(G4VisAttributes::Invisible);

   logicExpHall->SetVisAttributes(G4VisAttributes::Invisible);
   //logicExpHall->SetVisAttributes(exp_vat);
 logicExpHall->SetVisAttributes(grey);

  //-----------------------------------------------------
  // Create and place the physical Volumes
  //-----------------------------------------------------

  // Experimental Hall
  G4VPhysicalVolume* physiExpHall = 
    new G4PVPlacement(0,G4ThreeVector(),
  		      logicExpHall,
  		      "expHall",
  		      0,false,0);

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
