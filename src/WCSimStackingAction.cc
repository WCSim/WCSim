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

      // XQ: get the maximum QE and multiply it by the ratio
      // only work for the range between 240 nm and 660 nm for now 
      // Even with WLS
	G4double wavelengthQE = 0;
	G4double ratio = 1./(1.0-0.25);
      if(aTrack->GetCreatorProcess()==NULL) {
	if(!DetConstruct->GetIsCombinedPMTCollectionDefined())
	  wavelengthQE  = DetConstruct->GetPMTQE(WCIDCollectionName,photonWavelength,1,200,660,ratio);
	else
	  wavelengthQE  = DetConstruct->GetStackingPMTQE(photonWavelength,1,200,660,ratio);
	if( G4UniformRand() > wavelengthQE )
	  classification = fKill;
      }
      else if (((G4VProcess*)(aTrack->GetCreatorProcess()))->GetProcessType()!=3)
	{
	  if (DetConstruct->GetPMT_QE_Method() == 1){
	    if(!DetConstruct->GetIsCombinedPMTCollectionDefined())
	      wavelengthQE  = DetConstruct->GetPMTQE(WCIDCollectionName,photonWavelength,1,200,660,ratio);
	    else
	      wavelengthQE  = DetConstruct->GetStackingPMTQE(photonWavelength,1,200,660,ratio);
	  }else if (DetConstruct->GetPMT_QE_Method() == 2){
	    if(!DetConstruct->GetIsCombinedPMTCollectionDefined())
	      wavelengthQE  = DetConstruct->GetPMTQE(WCIDCollectionName,photonWavelength,0,200,660,ratio);
	    else
	      wavelengthQE  = DetConstruct->GetStackingPMTQE(photonWavelength,0,200,660,ratio);
	  }else if (DetConstruct->GetPMT_QE_Method() == 3 || DetConstruct->GetPMT_QE_Method() == 4){
	    wavelengthQE = 1.1;
	  }
	}//Geometry with OD
	
	if( G4UniformRand() > wavelengthQE )
	  classification = fKill;
      }//aTrack->GetCreatorProcess() == NULL || (aTrack->GetCreatorProcess()))->GetProcessType() != fOptical
    }//optical photon

  return classification;
}

void WCSimStackingAction::NewStage() {;}
void WCSimStackingAction::PrepareNewEvent() {;}
