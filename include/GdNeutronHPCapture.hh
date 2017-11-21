

///////////////////////////////////////////////////////////////////////////////
//                   Spectrum of radiative neutron capture by Gadolinium            
//                                    version 1.0.0                                
//                                    (Sep.09.2005)                               
                            
//Modified class from original G4NeutronHPCapture class to include 
//the gammas spectrum of radiative neutron capture by Gadolinium.

// Karim Zbiri, April, 2005
///////////////////////////////////////////////////////////////////////////////



#ifndef GdNeutronHPCapture_h
#define GdNeutronHPCapture_h 1

#include "globals.hh"
#include "G4NeutronHPChannel.hh"
#include "G4HadronicInteraction.hh"

class GdNeutronHPCapture : public G4HadronicInteraction
{
  public: 
  
  GdNeutronHPCapture();

  ~GdNeutronHPCapture();

  G4HadFinalState * ApplyYourself(const G4HadProjectile& aTrack, G4Nucleus& aTargetNucleus);

    void BuildPhysicsTable(const G4ParticleDefinition&);
    
  private:
  
  G4double * xSec;
  //G4NeutronHPChannel * theCapture;
      std::vector<G4NeutronHPChannel*>* theCapture;
  G4String dirName;
  G4int numEle;
  G4int it;
  // G4String nomel;

  G4HadFinalState theResult;
};

#endif
