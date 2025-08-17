// Modified copy of G4OpRayleigh.cc of geant4.10 to implement Raman scattering
// Reference: APPLIED OPTICS / Vol. 37, No. 15 / 20 May 1998
//

#ifndef WCSimOpRaman_h
#define WCSimOpRaman_h 1

/////////////
// Includes
/////////////

#include "globals.hh"
#include "templates.hh"
#include "Randomize.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleMomentum.hh"
#include "G4Step.hh"
#include "G4VDiscreteProcess.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4OpticalPhoton.hh"
#include "G4PhysicsTable.hh"
#include "G4PhysicsOrderedFreeVector.hh"

// Class Description:
// Discrete Process -- Rayleigh scattering of optical photons.
// Class inherits publicly from G4VDiscreteProcess.
// Class Description - End:

/////////////////////
// Class Definition
/////////////////////

class WCSimOpRaman : public G4VDiscreteProcess 
{

public:

        ////////////////////////////////
        // Constructors and Destructor
        ////////////////////////////////
 
        WCSimOpRaman(const G4String& processName = "OpRaman",
                              G4ProcessType type = fOptical);
	~WCSimOpRaman();

private:

        WCSimOpRaman(const WCSimOpRaman &right);

        //////////////
        // Operators
        //////////////

        WCSimOpRaman& operator=(const WCSimOpRaman &right);

public:

        ////////////
        // Methods
        ////////////

        /// Returns true -> 'is applicable' only for an optical photon.
        G4bool IsApplicable(const G4ParticleDefinition& aParticleType);

        void BuildPhysicsTable(const G4ParticleDefinition& aParticleType);
        // Build thePhysicsTable at a right time

        G4double GetMeanFreePath(const G4Track& aTrack,
				 G4double ,
                                 G4ForceCondition* );
        // Returns the mean free path for Raman scattering in water.
        // --- Not yet implemented for other materials! ---

        G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
                                       const G4Step&  aStep);
        // This is the method implementing Raman scattering.

        G4PhysicsTable* GetPhysicsTable() const;
        // Returns the address of the physics table.

        void DumpPhysicsTable() const;
        // Prints the physics table.

private:

        /////////////////////
        // Helper Functions
        /////////////////////

        /// Calculates the mean free paths for a material as a function of 
        /// photon energy
        ///
        /// @param[in] material information
        /// @return the mean free path vector
        G4PhysicsOrderedFreeVector* 
        CalculateRamanMeanFreePaths( const G4Material* material ) const;

        ///////////////////////
        // Class Data Members
        ///////////////////////

protected:

        G4PhysicsTable* thePhysicsTable;
        //  A Physics Table can be either a cross-sections table or
        //  an energy table (or can be used for other specific
        //  purposes).

private:
};

////////////////////
// Inline methods
////////////////////

inline
G4bool WCSimOpRaman::IsApplicable(const G4ParticleDefinition& aParticleType)
{
  return ( &aParticleType == G4OpticalPhoton::OpticalPhoton() );
}

inline
void WCSimOpRaman::DumpPhysicsTable() const

{
        G4int PhysicsTableSize = thePhysicsTable->entries();
        G4PhysicsOrderedFreeVector *v;

        for (G4int i = 0 ; i < PhysicsTableSize ; i++ )
        {
                v = (G4PhysicsOrderedFreeVector*)(*thePhysicsTable)[i];
                v->DumpValues();
        }
}

inline G4PhysicsTable* WCSimOpRaman::GetPhysicsTable() const
{
  return thePhysicsTable;
}


#endif /* WCSimOpRaman_h */
