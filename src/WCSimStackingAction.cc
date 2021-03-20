#include "WCSimStackingAction.hh"
#include "WCSimDetectorConstruction.hh"

#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4VProcess.hh"
#include "G4VPhysicalVolume.hh"
#include "Randomize.hh"
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//class WCSimDetectorConstruction;

WCSimStackingAction::WCSimStackingAction(WCSimDetectorConstruction* myDet):DetConstruct(myDet) {;}
WCSimStackingAction::~WCSimStackingAction(){;}


G4ClassificationOfNewTrack WCSimStackingAction::ClassifyNewTrack
(const G4Track* aTrack) 
{
  G4String WCIDCollectionName = DetConstruct->GetIDCollectionName();
  G4ClassificationOfNewTrack classification    = fWaiting;
  G4ParticleDefinition*      particleType      = aTrack->GetDefinition();
  

  // Make sure it is an optical photon
  if( particleType == G4OpticalPhoton::OpticalPhotonDefinition() )
    {
      // TF: cleaned this up a little: no repetition of code.
      // also don't know why CreatorProcess() == NULL needs to have QE applied.
      // use QE method for ALL.
      if( aTrack->GetCreatorProcess() == NULL ||          // eg. particle gun/gps photons
	  ( aTrack->GetCreatorProcess() != NULL && 
	    ((G4VProcess*)(aTrack->GetCreatorProcess()))->GetProcessType() != fOptical) ) {
	
	G4double photonWavelength = (2.0*M_PI*197.3)/(aTrack->GetTotalEnergy()/eV);
	G4double ratio = 1./(1.0-0.25);
	G4double wavelengthQE = 0;
	
	// MF : translated from skdetsim : better to increase the number of photons
	// than to throw in a global factor  at Digitization time !
	
	// XQ: get the maximum QE and multiply it by the ratio
	// only work for the range between 240 nm and 660 nm for now 
	// Even with WLS
	//G4String volumeName        = aTrack->GetVolume()->GetName();

	if (DetConstruct->GetPMT_QE_Method()==1){
	  wavelengthQE  = DetConstruct->GetPMTQE(WCIDCollectionName,photonWavelength,1,240,660,ratio);
	  //wavelengthQE  = DetConstruct->GetPMTQE(volumeName,photonWavelength,1,240,660,ratio);
	}else if (DetConstruct->GetPMT_QE_Method()==2){
	  wavelengthQE  = DetConstruct->GetPMTQE(WCIDCollectionName,photonWavelength,0,240,660,ratio);
	  //wavelengthQE  = DetConstruct->GetPMTQE(volumeName,photonWavelength,0,240,660,ratio);
	}else if (DetConstruct->GetPMT_QE_Method()==3 || DetConstruct->GetPMT_QE_Method() == 4){
	  wavelengthQE = 1.1;
	}
	

	if( G4UniformRand() > wavelengthQE )
	  classification = fKill;
      }
    }
  
  return classification;
}

void WCSimStackingAction::NewStage() {;}
void WCSimStackingAction::PrepareNewEvent() {;}

