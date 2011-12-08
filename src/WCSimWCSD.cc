#include "WCSimWCSD.hh"
#include "G4ParticleTypes.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "Randomize.hh"
#include "G4ios.hh"
#include "G4VProcess.hh"

#include <sstream>

#include "WCSimDetectorConstruction.hh"
#include "WCSimTrackInformation.hh"

WCSimWCSD::WCSimWCSD(G4String name,WCSimDetectorConstruction* myDet)
:G4VSensitiveDetector(name)
{
  // Place the name of this collection on the list.  We can have more than one
  // in principle.  CollectionName is a vector.

  // Note there is some sort of problem here.  If I use the name
  // Which has a "/" in it, I can find this collection later using 
  // GetCollectionID()

  G4String HCname;
  collectionName.insert(HCname="glassFaceWCPMT");
  
  fdet = myDet;
  
  HCID = -1;
}

WCSimWCSD::~WCSimWCSD() {}

void WCSimWCSD::Initialize(G4HCofThisEvent* HCE)
{
  // Make a new hits collection.  With the name we set in the constructor
  hitsCollection = new WCSimWCHitsCollection
    (SensitiveDetectorName,collectionName[0]); 
  
  // This is a trick.  We only want to do this once.  When the program
  // starts HCID will equal -1.  Then it will be set to the pointer to
  // this collection.
  
  // Get the Id of the "0th" collection
  if (HCID<0)
    HCID =  GetCollectionID(0); 
  
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

G4float WCSimWCSD::Interpolate_func(G4float x, G4int ncount, G4float *wave_length, G4float *quantity){
  // linear interpolate the quantity function versus wave_length
  if (x < *wave_length || x >=*(wave_length+ncount-1)){
    return 0;
  }else{
    for (Int_t i=0;i!=ncount;i++){
      if (x>=*(wave_length+i) && x < *(wave_length+i+1)){
	return (x-*(wave_length+i))/(*(wave_length+i+1)-*(wave_length+i))* (*(quantity+i+1)) + (*(wave_length+i+1)-x)/(*(wave_length+i+1)-*(wave_length+i)) * (*(quantity+i));
      }
    }
  }
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

  // M Fechner : too verbose
  //  if (aStep->GetTrack()->GetTrackStatus() == fAlive) cout << "status is fAlive\n";
  if ((aStep->GetTrack()->GetTrackStatus() == fAlive )
      &&(particleDefinition == G4OpticalPhoton::OpticalPhotonDefinition()))
    return false;

  G4float wlsflag = 0.0;
  
  G4Track * theTrack = aStep->GetTrack();
  const G4VProcess* originofphoton = theTrack->GetCreatorProcess();
  
 
  G4String originstring;
  originstring = originofphoton->GetProcessName();
  
  //G4cout << originstring << G4endl;
  
  if(originstring == "OpWLS")
    {
      wlsflag = 1.0;
    }
  



  //  if ( particleDefinition ==  G4OpticalPhoton::OpticalPhotonDefinition() ) 
  // G4cout << volumeName << " hit by optical Photon! " << G4endl;
    
  // Make the tubeTag string based on the replica numbers
   // See WCSimDetectorConstruction::DescribeAndRegisterPMT() for matching
  // tag construction.
#ifndef USE_STRSTREAM
  std::stringstream tubeTag;
#else
  char buffer[100];
  std::ostrstream tubeTag(buffer,100);
#endif
  
  // Start tubeTag with mother to distinguish different PMT hierarchies
  G4LogicalVolume *theMother = thePhysical->GetMotherLogical();
  if (theMother != NULL)
    tubeTag << theMother->GetName() << ":";

  tubeTag << thePhysical->GetName(); 

  for (G4int i = theTouchable->GetHistoryDepth()-1 ; i >= 0; i--)
    tubeTag << ":" << theTouchable->GetCopyNumber(i); 
    //   tubeTag << ":" << theTouchable->GetVolume(i)->GetCopyNo(); 


#ifdef USE_STRSTREAM
  tubeTag << std::ends;
#endif
 
//   G4cout << tubeTag.str() << G4endl;

  // Get the tube ID from the tubeTag
  G4int replicaNumber = WCSimDetectorConstruction::GetTubeID(tubeTag.str());

  
  G4float collection_angle[10]={0,10,20,30,40,50,60,70,80,90};
  G4float collection_eff_db[2][10]={{100,100,99,95,90,85,80,69,35,13}, // 10 inch
				    {100.,99.33,100.,98.89,97.23,95.392,93.52,86.3,55.6,26.25} // Penn's measurement of 12 inch PMT
  };

  // add in position dependence timing variations
  //center of mean shift
  G4float pmt_timing_center[10]={-0.1585,-0.188037,-0.114125,0.0108234,0.151367,-0.0643322,-0.446126,-0.694123,-0.463073,-0.011125};
  //sigma of mean shift, we have to add this part, since in principle we can have different timing in azimuthal angles.
  // Here we did a very naive approximation on this part to speed up. 
  G4float pmt_timing_sigma[10]={0.169304,0.160181,0.180895,0.214357,0.242133,0.364881,0.755061,1.47378,1.6562,1.37735};
  // timing resolution 
  G4float pmt_timing_smear[10]={1.1715,1.16763,1.17955,1.18753,1.20617,1.19932,1.21371,1.38257,1.43092,1.68762};
  
  
  

  G4float theta_angle;
  G4float effectiveAngularEfficiency;

 
  
  G4float ratio = 1.;
  G4float maxQE;
  G4float photonQE;
  // G4cout<<"GetPMT_QE_Method=  "<<fdet->GetPMT_QE_Method()<<G4endl;
  if (fdet->GetPMT_QE_Method()==1){
    photonQE = 1.1;
  }else if (fdet->GetPMT_QE_Method()==2){
    maxQE = fdet->GetPMTQE(wavelength,0,240,660,ratio);
    photonQE = fdet->GetPMTQE(wavelength,1,240,660,ratio);
    photonQE = photonQE/maxQE;
  }else if (fdet->GetPMT_QE_Method()==3){
    ratio = 1./(1.-0.25);
    photonQE = fdet->GetPMTQE(wavelength,1,240,660,ratio);
  }
  G4double qe_flag = 0;
  if (G4UniformRand() <= photonQE){
    
     G4double local_x = localPosition.x();
     G4double local_y = localPosition.y();
     G4double local_z = localPosition.z();
     theta_angle = acos(fabs(local_z)/sqrt(pow(local_x,2)+pow(local_y,2)+pow(local_z,2)))/3.1415926*180.;
     if (fdet->GetPMT_Coll_Eff_Method()==1){
       effectiveAngularEfficiency = Interpolate_func(theta_angle,10,collection_angle,collection_eff_db[0])/100.;
     }else if (fdet->GetPMT_Coll_Eff_Method()==2){
       effectiveAngularEfficiency = Interpolate_func(theta_angle,10,collection_angle,collection_eff_db[1])/100.;
     }else{
       effectiveAngularEfficiency = Interpolate_func(theta_angle,10,collection_angle,collection_eff_db[0])/100.;
     }
     // std::cout << fdet->GetPMT_Coll_Eff_Method() << "\t" << effectiveAngularEfficiency << std::endl;

     G4float pmt_tc,pmt_ts,pmt_tsm,pmt_time_corr; 
     if (fdet->GetPMT_Timing_Var()==1){
       pmt_tc = Interpolate_func(theta_angle,10,collection_angle,pmt_timing_center);
       pmt_ts = Interpolate_func(theta_angle,10,collection_angle,pmt_timing_sigma);
       pmt_tsm = Interpolate_func(theta_angle,10,collection_angle,pmt_timing_smear);
       pmt_time_corr = pmt_tc + G4RandGauss::shoot(0.,1.)*sqrt(pmt_ts*pmt_ts+pmt_tsm+pmt_tsm);
     }else{
       pmt_time_corr = 0.;
     }
     

     if (G4UniformRand() <= effectiveAngularEfficiency || fdet->UsePMT_Coll_Eff()==0){

       qe_flag = 1;
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
	   
	   (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPe(hitTime,pmt_time_corr,wavelength, wlsflag,qe_flag,
								localPosition.x(),
								localPosition.y(),
								localPosition.z(),
								localDirection.x(),
								localDirection.y(),
								localDirection.z()
								);
	   (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddParentID(primParentID);
	   
	   //     if ( particleDefinition != G4OpticalPhoton::OpticalPhotonDefinition() )
	   //       newHit->Print();
	 }
       else {
	 (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPe(hitTime,pmt_time_corr,wavelength, wlsflag,qe_flag,
							      localPosition.x(),
							      localPosition.y(),
							      localPosition.z(),
							      localDirection.x(),
							      localDirection.y(),
							      localDirection.z());
	 (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddParentID(primParentID);
       }
     }
  }
  //else{
    // for test purpose
    // (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPe(hitTime,wavelength,qe_flag);
    //     (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddParentID(primParentID);
  //}

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

