#include "WCSimWCSD.hh"
#include "G4ParticleTypes.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"
#include "G4ios.hh"

#include <sstream>

#include "WCSimDetectorConstruction.hh"
#include "WCSimTrackInformation.hh"

WCSimWCSD::WCSimWCSD(G4String CollectionName, G4String name,WCSimDetectorConstruction* myDet)
:G4VSensitiveDetector(name)
{
  // Place the name of this collection on the list.  We can have more than one
  // in principle.  CollectionName is a vector.

  // Note there is some sort of problem here.  If I use the name
  // Which has a "/" in it, I can find this collection later using 
  // GetCollectionID()

  collectionName.insert(CollectionName);
  
  fdet = myDet;
  
  HCID = -1;
}

WCSimWCSD::~WCSimWCSD() {}

void WCSimWCSD::Initialize(G4HCofThisEvent* HCE)
{
  // Make a new hits collection. With the name we set in the constructor
  hitsCollection = new WCSimWCHitsCollection
    (SensitiveDetectorName,collectionName[0]);

  // This is a trick.  We only want to do this once.  When the program
  // starts HCID will equal -1.  Then it will be set to the pointer to
  // this collection.

  
  // Get the Id of the "0th" collection
  if (HCID<0){
    HCID =  GetCollectionID(0); 
  }  
  // Add it to the Hit collection of this event.
  HCE->AddHitsCollection( HCID, hitsCollection );  

  // Initilize the Hit map to all tubes not hit.
  PMTHitMap.clear();
  // Trick to access the static maxPE variable.  This will go away with the 
  // variable.

  WCSimWCHit* newHit = new WCSimWCHit();
  newHit->SetMaxPe(0);
  delete newHit;
}

G4bool WCSimWCSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{ 
  G4StepPoint*       preStepPoint = aStep->GetPreStepPoint();
  G4TouchableHandle  theTouchable = preStepPoint->GetTouchableHandle();
  G4VPhysicalVolume* thePhysical  = theTouchable->GetVolume();


  //XQ 3/30/11 try to get the local position try to add the position and direction
  G4ThreeVector worldPosition = preStepPoint->GetPosition();
  G4ThreeVector localPosition = theTouchable->GetHistory()->GetTopTransform().TransformPoint(worldPosition);
  G4ThreeVector worldDirection = preStepPoint->GetMomentumDirection();
  G4ThreeVector localDirection = theTouchable->GetHistory()->GetTopTransform().TransformAxis(worldDirection);

  

  WCSimTrackInformation* trackinfo 
    = (WCSimTrackInformation*)(aStep->GetTrack()->GetUserInformation());
  G4int primParentID;
  if (trackinfo)
    primParentID = trackinfo->GetPrimaryParentID();
  else // if there is no trackinfo, then it is a primary particle!
    primParentID = aStep->GetTrack()->GetTrackID();

  G4int    trackID           = aStep->GetTrack()->GetTrackID();
  G4String volumeName        = aStep->GetTrack()->GetVolume()->GetName();
  
  //XQ Add the wavelength there
  G4float  wavelength = (2.0*M_PI*197.3)/( aStep->GetTrack()->GetTotalEnergy()/eV);
  
  G4double energyDeposition  = aStep->GetTotalEnergyDeposit();
  G4double hitTime           = aStep->GetPreStepPoint()->GetGlobalTime();

  G4ParticleDefinition *particleDefinition = 
    aStep->GetTrack()->GetDefinition();

  if ( particleDefinition != G4OpticalPhoton::OpticalPhotonDefinition() 
       && energyDeposition == 0.0) 
    return false;
  // MF : I don't see why other particles should register hits
  // they don't in skdetsim. 
  if ( particleDefinition != G4OpticalPhoton::OpticalPhotonDefinition())
    return false;
  G4String WCIDCollectionName = fdet->GetIDCollectionName();
  // M Fechner : too verbose
  //  if (aStep->GetTrack()->GetTrackStatus() == fAlive)G4cout << "status is fAlive\n";
  if ((aStep->GetTrack()->GetTrackStatus() == fAlive )
      &&(particleDefinition == G4OpticalPhoton::OpticalPhotonDefinition()))
    return false;

  //  if ( particleDefinition ==  G4OpticalPhoton::OpticalPhotonDefinition() ) 
  // G4cout << volumeName << " hit by optical Photon! " << G4endl;
    
  // Make the tubeTag string based on the replica numbers
  // See WCSimDetectorConstruction::DescribeAndRegisterPMT() for matching
  // tag construction.

  std::stringstream tubeTag;

  // Start tubeTag with mother to distinguish different PMT hierarchies
//  G4LogicalVolume *theMother = thePhysical->GetMotherLogical();
//  if (theMother != NULL)
//    tubeTag << theMother->GetName() << ":";

//  tubeTag << thePhysical->GetName(); 
  for (G4int i = theTouchable->GetHistoryDepth()-1 ; i >= 0; i--){
    tubeTag << ":" << theTouchable->GetVolume(i)->GetName();
    tubeTag << "-" << theTouchable->GetCopyNumber(i);
  }
  //  tubeTag << ":" << theTouchable->GetVolume(i)->GetCopyNo(); 

//  G4cout << tubeTag.str() << G4endl;

  // Get the tube ID from the tubeTag
  G4int replicaNumber = WCSimDetectorConstruction::GetTubeID(tubeTag.str());

    
  G4float theta_angle;
  G4float effectiveAngularEfficiency;


  
  G4float ratio = 1.;
  G4float maxQE;
  G4float photonQE;
  if (fdet->GetPMT_QE_Method()==1){
    photonQE = 1.1;
  }else if (fdet->GetPMT_QE_Method()==2){
    maxQE = fdet->GetPMTQE(WCIDCollectionName,wavelength,0,240,660,ratio);
    photonQE = fdet->GetPMTQE(volumeName, wavelength,1,240,660,ratio);
    photonQE = photonQE/maxQE;
  }else if (fdet->GetPMT_QE_Method()==3){
    ratio = 1./(1.-0.25);
    photonQE = fdet->GetPMTQE(volumeName, wavelength,1,240,660,ratio);
  }
  
  

  if (G4UniformRand() <= photonQE){
    
     G4double local_x = localPosition.x();
     G4double local_y = localPosition.y();
     G4double local_z = localPosition.z();
     theta_angle = acos(fabs(local_z)/sqrt(pow(local_x,2)+pow(local_y,2)+pow(local_z,2)))/3.1415926*180.;
     effectiveAngularEfficiency = fdet->GetPMTCollectionEfficiency(theta_angle, volumeName);
     if (G4UniformRand() <= effectiveAngularEfficiency || fdet->UsePMT_Coll_Eff()==0){
       //Retrieve the pointer to the appropriate hit collection. Since volumeName is the same as the SD name, this works. 
       G4SDManager* SDman = G4SDManager::GetSDMpointer();
       G4RunManager* Runman = G4RunManager::GetRunManager();
       G4int collectionID = SDman->GetCollectionID(volumeName);
       const G4Event* currentEvent = Runman->GetCurrentEvent();
       G4HCofThisEvent* HCofEvent = currentEvent->GetHCofThisEvent();
       hitsCollection = (WCSimWCHitsCollection*)(HCofEvent->GetHC(collectionID));
      
       // If this tube hasn't been hit add it to the collection
       if (PMTHitMap[replicaNumber] == 0)
	 {
	   WCSimWCHit* newHit = new WCSimWCHit();
	   newHit->SetTubeID(replicaNumber);
	   newHit->SetTrackID(trackID);
	   newHit->SetEdep(energyDeposition); 
	   newHit->SetLogicalVolume(thePhysical->GetLogicalVolume());
	   
	   G4AffineTransform aTrans = theTouchable->GetHistory()->GetTopTransform();
	   newHit->SetRot(aTrans.NetRotation());
	   
	   aTrans.Invert();
	   newHit->SetPos(aTrans.NetTranslation());
	   
	   // Set the hitMap value to the collection hit number
	   PMTHitMap[replicaNumber] = hitsCollection->insert( newHit );
	   (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPe(hitTime);
	   (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddParentID(primParentID);
	   
	   //     if ( particleDefinition != G4OpticalPhoton::OpticalPhotonDefinition() )
	   //       newHit->Print();
	 }
       else {
	 (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPe(hitTime);
	 (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddParentID(primParentID);
	 
       }
     }
  }

  return true;
}

void WCSimWCSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) 
  { 
    G4int numHits = hitsCollection->entries();

    G4cout << "There are " << numHits << " hits in the WC: " << G4endl;
    for (G4int i=0; i < numHits; i++) 
      (*hitsCollection)[i]->Print();
  } 
}

