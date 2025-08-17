// Modified copy of G4OpRayleigh.cc of geant4.10 to implement Raman scattering
// Reference: APPLIED OPTICS / Vol. 37, No. 15 / 20 May 1998
//

#include "WCSimOpRaman.hh"

#include "G4ios.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "WCSimOpProcessSubType.hh"

/////////////////////////
// Class Implementation
/////////////////////////

        /////////////////
        // Constructors
        /////////////////

WCSimOpRaman::WCSimOpRaman(const G4String& processName, G4ProcessType type)
           : G4VDiscreteProcess(processName, type)
{
        SetProcessSubType(fOpRaman);

        thePhysicsTable = NULL;

        if (verboseLevel>0) {
           G4cout << GetProcessName() << " is created " << G4endl;
        }
}

        ////////////////
        // Destructors
        ////////////////

WCSimOpRaman::~WCSimOpRaman()
{
        if (thePhysicsTable) {
           thePhysicsTable->clearAndDestroy();
           delete thePhysicsTable;
        }
}

        ////////////
        // Methods
        ////////////

// PostStepDoIt
// -------------
//
G4VParticleChange*
WCSimOpRaman::PostStepDoIt(const G4Track& aTrack, const G4Step& aStep)
{
        aParticleChange.Initialize(aTrack);

        const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();

        if (verboseLevel>0) {
                G4cout << "Raman Scattering Photon!" << G4endl;
                G4cout << "Old Momentum Direction: "
                       << aParticle->GetMomentumDirection() << G4endl;
                G4cout << "Old Polarization: "
                       << aParticle->GetPolarization() << G4endl;
        }

        G4double cosTheta;
        G4ThreeVector OldMomentumDirection, NewMomentumDirection;
        G4ThreeVector OldPolarization, NewPolarization;

        G4double rand, constant;
        G4double CosTheta, SinTheta, SinPhi, CosPhi, unit_x, unit_y, unit_z;

        do {
           // Try to simulate the scattered photon momentum direction
           // w.r.t. the initial photon momentum direction

           CosTheta = G4UniformRand();
           SinTheta = std::sqrt(1.-CosTheta*CosTheta);
           // consider for the angle 90-180 degrees
           if (G4UniformRand() < 0.5) CosTheta = -CosTheta;

           // simulate the phi angle
           rand = twopi*G4UniformRand();
           SinPhi = std::sin(rand);
           CosPhi = std::cos(rand);

           // start constructing the new momentum direction
           unit_x = SinTheta * CosPhi;
           unit_y = SinTheta * SinPhi;
           unit_z = CosTheta;
           NewMomentumDirection.set (unit_x,unit_y,unit_z);

           // Rotate the new momentum direction into global reference system
           OldMomentumDirection = aParticle->GetMomentumDirection();
           OldMomentumDirection = OldMomentumDirection.unit();
           NewMomentumDirection.rotateUz(OldMomentumDirection);
           NewMomentumDirection = NewMomentumDirection.unit();

           // calculate the new polarization direction
           // The new polarization needs to be in the same plane as the new
           // momentum direction and the old polarization direction
           OldPolarization = aParticle->GetPolarization();
           constant = -NewMomentumDirection.dot(OldPolarization);

           NewPolarization = OldPolarization + constant*NewMomentumDirection;
           NewPolarization = NewPolarization.unit();

           // There is a corner case, where the Newmomentum direction
           // is the same as oldpolariztion direction:
           // random generate the azimuthal angle w.r.t. Newmomentum direction
           if (NewPolarization.mag() == 0.) {
              rand = G4UniformRand()*twopi;
              NewPolarization.set(std::cos(rand),std::sin(rand),0.);
              NewPolarization.rotateUz(NewMomentumDirection);
           } else {
              // There are two directions which are perpendicular
              // to the new momentum direction
              if (G4UniformRand() < 0.5) NewPolarization = -NewPolarization;
           }
	  
           // simulate according to the distribution cos^2(theta)
           cosTheta = NewPolarization.dot(OldPolarization);
          // Loop checking, 13-Aug-2015, Peter Gumplinger
        } while (std::pow(cosTheta,2) < G4UniformRand());

        aParticleChange.ProposePolarization(NewPolarization);
        aParticleChange.ProposeMomentumDirection(NewMomentumDirection);

        // Shift photon wavelength
        // 1/lambda' = 1/lambda - 3400cm-1
        G4double NewEnergy = aParticle->GetTotalEnergy() - h_Planck * c_light * (3400./cm);
        aParticleChange.ProposeEnergy(NewEnergy);

        if (verboseLevel>0) {
                G4cout << "New Polarization: " 
                     << NewPolarization << G4endl;
                G4cout << "Polarization Change: "
                     << *(aParticleChange.GetPolarization()) << G4endl;  
                G4cout << "New Momentum Direction: " 
                     << NewMomentumDirection << G4endl;
                G4cout << "Momentum Change: "
                     << *(aParticleChange.GetMomentumDirection()) << G4endl; 
        }

        return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
}

// BuildPhysicsTable for the Raman Scattering process
// --------------------------------------------------------
void WCSimOpRaman::BuildPhysicsTable(const G4ParticleDefinition&)
{
  if (thePhysicsTable) {
     thePhysicsTable->clearAndDestroy();
     delete thePhysicsTable;
     thePhysicsTable = NULL;
  }

  const G4MaterialTable* theMaterialTable = G4Material::GetMaterialTable();
  const G4int numOfMaterials = G4Material::GetNumberOfMaterials();

  thePhysicsTable = new G4PhysicsTable( numOfMaterials );
  
  for( G4int iMaterial = 0; iMaterial < numOfMaterials; iMaterial++ )
  {
      G4Material* material = (*theMaterialTable)[iMaterial];
      G4MaterialPropertiesTable* materialProperties = 
                                       material->GetMaterialPropertiesTable();
      G4PhysicsOrderedFreeVector* raman = NULL;
      if ( materialProperties != NULL ) {
         raman = materialProperties->GetProperty( "RAMAN" );
         if ( raman == NULL ) raman = 
                                   CalculateRamanMeanFreePaths( material );
      }
      thePhysicsTable->insertAt( iMaterial, raman );
  }
}

// GetMeanFreePath()
// -----------------
//
G4double WCSimOpRaman::GetMeanFreePath(const G4Track& aTrack,
                                       G4double ,
                                       G4ForceCondition* )
{
  const G4DynamicParticle* particle = aTrack.GetDynamicParticle();
  const G4double photonMomentum = particle->GetTotalMomentum();
  const G4Material* material = aTrack.GetMaterial();

  G4PhysicsOrderedFreeVector* raman = 
                              static_cast<G4PhysicsOrderedFreeVector*>
                              ((*thePhysicsTable)(material->GetIndex()));
  
  G4double rsLength = DBL_MAX;
  if( raman!= NULL ) rsLength = raman->Value( photonMomentum );
  return rsLength;
}

// CalculateRamanMeanFreePaths()
// --------------------------------
// Private method to compute Raman Scattering Lengths
G4PhysicsOrderedFreeVector* 
WCSimOpRaman::CalculateRamanMeanFreePaths( const G4Material* material ) const
{
  G4MaterialPropertiesTable* materialProperties = 
                                       material->GetMaterialPropertiesTable();

  // Calculation is valid only for "Water" or "Doped Water" (Water + Gadolinium)
  if ( material->GetName() != "Water" && material->GetName() != "Doped Water" )
    return NULL;

  // If the material doesn't have a RINDEX property vector then return
  G4MaterialPropertyVector* rIndex = materialProperties->GetProperty("RINDEX");
  if ( rIndex == NULL ) return NULL;

  G4PhysicsOrderedFreeVector* ramanMeanFreePaths =
                                             new G4PhysicsOrderedFreeVector();

  for( size_t uRIndex = 0; uRIndex < rIndex->GetVectorLength(); uRIndex++ )
  {
     const G4double energy = rIndex->Energy( uRIndex );
     const G4double lambda = h_Planck * c_light / energy;
     const G4double meanFreePath = (3703.7037*m) * pow( lambda / (488*nm) , 5.3);

     if( verboseLevel>0 )
       G4cout << energy/MeV << "MeV\t" << lambda/nm << "nm\t" << meanFreePath/mm << "mm" << G4endl;

     ramanMeanFreePaths->InsertValues( energy, meanFreePath );
  }

  return ramanMeanFreePaths;
}
