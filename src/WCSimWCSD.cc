#include "WCSimWCSD.hh"

#include "WCSimSteppingAction.hh"
#include "WCSimDetectorConstruction.hh"
#include "WCSimTrackInformation.hh"

#include "G4ParticleTypes.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"
#include "G4ios.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <sstream>


WCSimWCSD::WCSimWCSD(G4String CollectionName,
                     G4String name,
                     WCSimDetectorConstruction* myDet,
                     G4String inDetectorElement)
     :G4VSensitiveDetector(name), detectorElement(inDetectorElement)
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

  // Initialize the Hit map to all tubes not hit.
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
  
  G4int parentSavedTrackID = -1;
  G4float photonStartTime;
  G4ThreeVector photonStartPos;
  G4ThreeVector photonStartDir;
  parentSavedTrackID = aStep->GetTrack()->GetParentID();
  photonStartTime = aStep->GetTrack()->GetGlobalTime() - aStep->GetTrack()->GetLocalTime(); // current time minus elapsed time of track
  photonStartPos = aStep->GetTrack()->GetVertexPosition();
  photonStartDir = aStep->GetTrack()->GetVertexMomentumDirection();


  G4int    trackID           = aStep->GetTrack()->GetTrackID();
  G4String volumeName        = aStep->GetTrack()->GetVolume()->GetName();
  
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

  G4String WCCollectionName;
  if(detectorElement=="tank") WCCollectionName = fdet->GetIDCollectionName();
  else if (detectorElement=="tankPMT2") WCCollectionName = fdet->GetIDCollectionName2();
  else if (detectorElement=="OD") WCCollectionName = fdet->GetODCollectionName();

  //= fdet->GetIDCollectionName();
  //WCSimPMTObject *PMT = fdet->GetPMTPointer(WCIDCollectionName);//B.Q
  //G4cout << "PMT associated to collection = " << PMT->GetPMTName() << G4endl;
  //if(volumeName == ) WCIDCollectionName = fdet->GetIDCollectionName();
  //G4String WCIDCollectionName2 = fdet->GetIDCollectionName2();

  // M Fechner : too verbose
  //  if (aStep->GetTrack()->GetTrackStatus() == fAlive)G4cout << "status is fAlive\n";
  if ((aStep->GetTrack()->GetTrackStatus() == fAlive )
      &&(particleDefinition == G4OpticalPhoton::OpticalPhotonDefinition()))
    return false;
  
  // TF: Problem: photons can go through the sensitive detector (glass)
  // and be killed in the next volume by Absorption, eg. in the BlackSheet, but will then
  // still be counted as hits here. So, either recode as extended/optical/LXe cathode implementation
  // or easier: make sure they cross the cathode, as the logical Boundary "cathode" can't be a
  // sensitive detector (I think).
  
  G4StepPoint        *postStepPoint = aStep->GetPostStepPoint();
  G4VPhysicalVolume  *postVol = postStepPoint->GetPhysicalVolume();
  //if (thePhysical)  G4cout << " thePrePV:  " << thePhysical->GetName()  << G4endl;
  //if (postVol) G4cout << " thePostPV: " << postVol->GetName() << G4endl;
  
  //Optical Photon must pass through glass into PMT interior!
  // What about the other way around? TF: current interior won't keep photons alive like in reality
  // Not an issue yet, because then interior needs to be a sensitive detector, when postStepPoint is the glass.
  if(postVol->GetName() != "InteriorWCPMT")
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

  // Debug:
  //G4cout << "================================================" << G4endl;
  //G4cout << tubeTag.str() << G4endl;
  //G4cout << "================================================" << G4endl;

  // Get the tube ID from the tubeTag
  G4int replicaNumber;
  if(detectorElement=="tank") replicaNumber = WCSimDetectorConstruction::GetTubeID(tubeTag.str());
  else if(detectorElement=="tankPMT2") replicaNumber = WCSimDetectorConstruction::GetTubeID2(tubeTag.str());
  else if(detectorElement=="OD") replicaNumber = WCSimDetectorConstruction::GetODTubeID(tubeTag.str());
  else G4cout << "detectorElement not defined..." << G4endl;

  G4double theta_angle = 0.;
  G4double effectiveAngularEfficiency = 0.;

  //XQ Add the wavelength there
  G4double  wavelength = (2.0*M_PI*197.3)/( aStep->GetTrack()->GetTotalEnergy()/eV);
  G4double ratio = 1.;
  G4double maxQE = 0.;
  G4double photonQE = 0.;
  if (fdet->GetPMT_QE_Method()==1 || fdet->GetPMT_QE_Method() == 4){
    photonQE = 1.1;
  }else if (fdet->GetPMT_QE_Method()==2){
    // maxQE = fdet->GetPMTQE(WCIDCollectionName,wavelength,0,200,700,ratio);
    maxQE = fdet->GetPMTQE(WCCollectionName,wavelength,0,240,660,ratio);
    photonQE = fdet->GetPMTQE(volumeName, wavelength,1,240,660,ratio);
    photonQE = photonQE/maxQE;
  }else if (fdet->GetPMT_QE_Method() == 3){
    ratio = 1./(1.-0.25);
    photonQE = fdet->GetPMTQE(WCCollectionName, wavelength,1,240,660,ratio);
  }
  
  if (G4UniformRand() <= photonQE){
    
     G4double local_x = localPosition.x();
     G4double local_y = localPosition.y();
     G4double local_z = localPosition.z();
     theta_angle = acos(fabs(local_z)/sqrt(pow(local_x,2)+pow(local_y,2)+pow(local_z,2)))/3.1415926*180.;
     effectiveAngularEfficiency = fdet->GetPMTCollectionEfficiency(theta_angle, volumeName);

     if (G4UniformRand() <= effectiveAngularEfficiency || fdet->UsePMT_Coll_Eff()==0){
       //Retrieve the pointer to the appropriate hit collection. 
       //Since volumeName is the same as the SD name, this works. 
       G4SDManager* SDman = G4SDManager::GetSDMpointer();
       G4RunManager* Runman = G4RunManager::GetRunManager();
       G4int collectionID = SDman->GetCollectionID(volumeName);
       const G4Event* currentEvent = Runman->GetCurrentEvent();
       G4HCofThisEvent* HCofEvent = currentEvent->GetHCofThisEvent();
       hitsCollection = (WCSimWCHitsCollection*)(HCofEvent->GetHC(collectionID));

       // mark the track as having produced a hit
       if(!trackinfo)
           trackinfo = new WCSimTrackInformation();
       trackinfo->SetProducesHit(true);

       // If this tube hasn't been hit add it to the collection
       if (PMTHitMap[replicaNumber] == 0)
       //if (PMTHitMap.find(replicaNumber) == PMTHitMap.end())  TF attempt to fix
	 {
	   WCSimWCHit* newHit = new WCSimWCHit();
	   newHit->SetTubeID(replicaNumber);
	   //newHit->SetTubeType(volumeName);//B. Quilain
	   newHit->SetEdep(energyDeposition); 
	   newHit->SetLogicalVolume(thePhysical->GetLogicalVolume());
	   
	   G4AffineTransform aTrans = theTouchable->GetHistory()->GetTopTransform();
	   newHit->SetRot(aTrans.NetRotation());
	   
	   aTrans.Invert();
	   newHit->SetPos(aTrans.NetTranslation());
	   // Set the hitMap value to the collection hit number
	   PMTHitMap[replicaNumber] = hitsCollection->insert( newHit );
	   (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPe(hitTime);
     (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddTrackID(trackID);
	   (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddParentID(parentSavedTrackID);
	   (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPhotonStartTime(photonStartTime);
	   (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPhotonStartPos(photonStartPos);
	   (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPhotonEndPos(worldPosition);
	   (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPhotonStartDir(photonStartDir);
	   (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPhotonEndDir(worldDirection);
	   
	   //     if ( particleDefinition != G4OpticalPhoton::OpticalPhotonDefinition() )
	   //       newHit->Print();
	     
	 }
       else {
	 (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPe(hitTime);
   (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddTrackID(trackID);
	 (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddParentID(parentSavedTrackID);
	 (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPhotonStartTime(photonStartTime);
	 (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPhotonStartPos(photonStartPos);
	 (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPhotonEndPos(worldPosition);
	 (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPhotonStartDir(photonStartDir);
	 (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPhotonEndDir(worldDirection);
	 
       }
     }
  }

  return true;
}

void WCSimWCSD::EndOfEvent(G4HCofThisEvent* HCE)
{

 
  if (verboseLevel>1) 
  { 
    //Need to specify which collection in case multiple geometries are built

    G4String WCIDCollectionName = fdet->GetIDCollectionName();
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    G4int collectionID = SDman->GetCollectionID(WCIDCollectionName);
    hitsCollection = (WCSimWCHitsCollection*)HCE->GetHC(collectionID);

    G4int numHits = hitsCollection->entries();

    G4cout << "There are " << numHits << " hits in the "<<detectorElement<<" : "<< G4endl;
    for (G4int i=0; i < numHits; i++) {
      G4cout<<"ihit ID = "<<i<<G4endl;
      (*hitsCollection)[i]->Print();
    }

    //Added by B. Quilain for the hybrid version
    G4cout<<"Tube hit list finalized"<<G4endl;
    G4cout<<"Geometry is hybrid = "<<fdet->GetHybridPMT()<<G4endl;
    if (fdet->GetHybridPMT()) { //TD fdet->2019/07/13
      G4String WCIDCollectionName2 = fdet->GetIDCollectionName2();
      G4int collectionID2 = SDman->GetCollectionID(WCIDCollectionName2);
      hitsCollection2 = (WCSimWCHitsCollection*)HCE->GetHC(collectionID2);
    
      G4int numHits2 = hitsCollection2->entries();

      G4cout << "There are " << numHits2 << " tubes hit in the WC: " << G4endl;
      for (G4int i=0; i < numHits2; i++){
	G4cout<<"ihit ID = "<<i<<G4endl;
	(*hitsCollection2)[i]->Print();
      }
    }
      /*
    {
      if(abs((*hitsCollection)[i]->GetTubeID() - 1584)  < 5){
	  G4cout << (*hitsCollection)[i]->GetTubeID() << G4endl;
	  (*hitsCollection)[i]->Print();
      }
      }*/

    /* Detailed debug:
    G4cout << "Through mPMTLV " << WCSimSteppingAction::n_photons_through_mPMTLV << G4endl;
    G4cout << "Through Acrylic " << WCSimSteppingAction::n_photons_through_acrylic << G4endl;
    G4cout << "Through Gel " << WCSimSteppingAction::n_photons_through_gel << G4endl;
    G4cout << "On Blacksheet " << WCSimSteppingAction::n_photons_on_blacksheet << G4endl;
    G4cout << "On small PMT " << WCSimSteppingAction::n_photons_on_smallPMT << G4endl;
    */
  } 
}

