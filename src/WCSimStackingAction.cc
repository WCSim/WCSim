#include "WCSimStackingAction.hh"
#include "WCSimDetectorConstruction.hh"

#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "Randomize.hh"
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

//class WCSimDetectorConstruction;

WCSimStackingAction::WCSimStackingAction(WCSimDetectorConstruction* myDet):DetConstruct(myDet) {;}
WCSimStackingAction::~WCSimStackingAction(){;}


G4ClassificationOfNewTrack WCSimStackingAction::ClassifyNewTrack
(const G4Track* aTrack) 
{
  G4ClassificationOfNewTrack classification    = fWaiting;
  G4ParticleDefinition*      particleType      = aTrack->GetDefinition();

  if( particleType == G4OpticalPhoton::OpticalPhotonDefinition() )
  {
    const G4float wavelength[20] = 
      { 280., 300., 320., 340., 360., 380., 400., 420., 440., 460.,  
	480., 500., 520., 540., 560., 580., 600., 620., 640., 660.};

    // (JF) This is the QE for the SuperK 20" tubes.
    const G4float SKQE[20] =
      { 0.00, .0139, .0854, .169, .203, .206, .211, .202, .188, .167, 
	.140,  .116, .0806, .0432, .0265, .0146, .00756, .00508, .00158, 0.00};

    // (JF) Normal 10 inch quantum efficiency (Data from Hamamatsu/IceCube)
    // These numbers were read off a graph by hand. I will replace them if
    // I get better numbers. These should be close though.
    const G4float TenInchQE[20] =
      { 0.00, .0375, .13, .195, .22, .23, .24, .24, .225, .205,
     .18, .16, .14, .085, .065, .05, .035, .02, .005, 0.0};

	// (JF) High QE tubes (Data from Hamamatsu/IceCube)
	const G4float TenInchHQE[20] =
      { 0.00, .0502, .2017, .2933, .3306, .3396, .3320, .3368, .2915, .2655, 
	.2268,  .1971, .1641, .1102, .0727, .0499, .0323, .0178, .0061, 0.00};

    G4float photonWavelength = (2.0*M_PI*197.3)/(aTrack->GetTotalEnergy()/eV);
    G4float wavelengthQE  = 0.0;
    
    if ( photonWavelength <= 280.0 || photonWavelength >= 660.0)
      return fKill;

    //MF: off by one bug fix.
    for (int i=0; i<=18; i++)
	{
      if ( photonWavelength <= wavelength[i+1])
      {
		if (DetConstruct->GetPMTName()=="10inchHQE")
        {
		wavelengthQE = TenInchHQE[i] + 
	  	(TenInchHQE[i+1]-TenInchHQE[i])/(wavelength[i+1]-wavelength[i])*
	  	(photonWavelength - wavelength[i]);
        }    
        else if(DetConstruct->GetPMTName()=="10inch")
        {
		wavelengthQE = TenInchQE[i] + 
	  	(TenInchQE[i+1]-TenInchQE[i])/(wavelength[i+1]-wavelength[i])*
	  	(photonWavelength - wavelength[i]);
        }
        else if(DetConstruct->GetPMTName()=="20inch")
		{
        wavelengthQE = SKQE[i] + 
	  	(SKQE[i+1]-SKQE[i])/(wavelength[i+1]-wavelength[i])*
	  	(photonWavelength - wavelength[i]);
        }
        else
        {G4cerr << "PMTName is not defined" << G4endl;}

		break;
      }
	}

//     G4cout << photonWavelength << " " << wavelengthQE << G4endl;

//    if( G4UniformRand() > wavelengthQE )
// MF : translated from skdetsim : better to increase the number of photons
// than to throw in a global factor  at Digitization time !
    if( G4UniformRand() > wavelengthQE/(1.0-0.25) )
      classification = fKill;
  }
  
  return classification;
}

void WCSimStackingAction::NewStage() {;}
void WCSimStackingAction::PrepareNewEvent() {;}

