// Modified copy of G4OpRayleigh.cc of geant4.10 to implement Raman scattering
// Reference: APPLIED OPTICS / Vol. 37, No. 15 / 20 May 1998
//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: G4OpRayleigh.cc 92045 2015-08-14 07:21:23Z gcosmo $
//
// 
////////////////////////////////////////////////////////////////////////
// Optical Photon Rayleigh Scattering Class Implementation
////////////////////////////////////////////////////////////////////////
//
// File:        G4OpRayleigh.cc
// Description: Discrete Process -- Rayleigh scattering of optical
//		photons
// Version:     1.0
// Created:     1996-05-31
// Author:      Juliet Armstrong
// Updated:     2014-10-10 -  This version calculates the Rayleigh scattering   
//              length for more materials than just Water (although the Water
//              default is kept). To do this the user would need to specify the
//              ISOTHERMAL_COMPRESSIBILITY as a material property and
//              optionally an RS_SCALE_LENGTH (useful for testing). Code comes
//              from Philip Graham (Queen Mary University of London).
//              2010-06-11 - Fix Bug 207; Thanks to Xin Qian
//              (Kellogg Radiation Lab of Caltech)
//              2005-07-28 - add G4ProcessType to constructor
//              2001-10-18 by Peter Gumplinger
//              eliminate unused variable warning on Linux (gcc-2.95.2)
//              2001-09-18 by mma
//		>numOfMaterials=G4Material::GetNumberOfMaterials() in BuildPhy
//              2001-01-30 by Peter Gumplinger
//              > allow for positiv and negative CosTheta and force the
//              > new momentum direction to be in the same plane as the
//              > new and old polarization vectors
//              2001-01-29 by Peter Gumplinger
//              > fix calculation of SinTheta (from CosTheta)
//              1997-04-09 by Peter Gumplinger
//              > new physics/tracking scheme
// mail:        gum@triumf.ca
//
////////////////////////////////////////////////////////////////////////

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
