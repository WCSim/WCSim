// Modified from G4OpBoundaryProcess.cc of geant4.10 to implement photocathode physics, which is a thin film of semiconductor alloy coated on glass
// Model the reflection/transmission/absorption processes by the coated layer
// 
// CoatedDielectricDielectric_Model1() : 
// Copy from geant4.11, which is based on https://ieeexplore.ieee.org/document/9875513
// Model the alloy as a thin layer with real refractive index, then calculate reflection and transmission probability
// Cannot handle total internal reflection when n1<n2
// 
// CoatedDielectricDielectric_Model2() : 
// Implementation based on https://arxiv.org/abs/physics/0408075v1
// Model the alloy as a thin layer with real and imaginary refractive indices, then calculate absorption, reflection and transmission probability
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
////////////////////////////////////////////////////////////////////////
// Optical Photon Boundary Process Class Implementation
////////////////////////////////////////////////////////////////////////
//
// File:        G4OpBoundaryProcess.cc
// Description: Discrete Process -- reflection/refraction at
//                                  optical interfaces
// Version:     1.1
// Created:     1997-06-18
// Modified:    1998-05-25 - Correct parallel component of polarization
//                           (thanks to: Stefano Magni + Giovanni Pieri)
//              1998-05-28 - NULL Rindex pointer before reuse
//                           (thanks to: Stefano Magni)
//              1998-06-11 - delete *sint1 in oblique reflection
//                           (thanks to: Giovanni Pieri)
//              1998-06-19 - move from GetLocalExitNormal() to the new 
//                           method: GetLocalExitNormal(&valid) to get
//                           the surface normal in all cases
//              1998-11-07 - NULL OpticalSurface pointer before use
//                           comparison not sharp for: std::abs(cost1) < 1.0
//                           remove sin1, sin2 in lines 556,567
//                           (thanks to Stefano Magni)
//              1999-10-10 - Accommodate changes done in DoAbsorption by
//                           changing logic in DielectricMetal
//              2001-10-18 - avoid Linux (gcc-2.95.2) warning about variables
//                           might be used uninitialized in this function
//                           moved E2_perp, E2_parl and E2_total out of 'if'
//              2003-11-27 - Modified line 168-9 to reflect changes made to
//                           G4OpticalSurface class ( by Fan Lei)
//              2004-02-02 - Set theStatus = Undefined at start of DoIt
//              2005-07-28 - add G4ProcessType to constructor
//              2006-11-04 - add capability of calculating the reflectivity
//                           off a metal surface by way of a complex index 
//                           of refraction - Thanks to Sehwook Lee and John 
//                           Hauptman (Dept. of Physics - Iowa State Univ.)
//              2009-11-10 - add capability of simulating surface reflections
//                           with Look-Up-Tables (LUT) containing measured
//                           optical reflectance for a variety of surface
//                           treatments - Thanks to Martin Janecek and
//                           William Moses (Lawrence Berkeley National Lab.)
//              2013-06-01 - add the capability of simulating the transmission
//                           of a dichronic filter
//              2017-02-24 - add capability of simulating surface reflections
//                           with Look-Up-Tables (LUT) developed in DAVIS
//
// Author:      Peter Gumplinger
// 		adopted from work by Werner Keil - April 2/96
// mail:        gum@triumf.ca
//
////////////////////////////////////////////////////////////////////////

#include "G4ios.hh"
#include "G4PhysicalConstants.hh"
#include "G4OpProcessSubType.hh"

#include "WCSimOpBoundaryProcess.hh"
#include "G4GeometryTolerance.hh"

#include "G4VSensitiveDetector.hh"
#include "G4ParallelWorldProcess.hh"

#include "G4SystemOfUnits.hh"

/////////////////////////
// Class Implementation
/////////////////////////

        //////////////
        // Operators
        //////////////

// WCSimOpBoundaryProcess::operator=(const WCSimOpBoundaryProcess &right)
// {
// }

        /////////////////
        // Constructors
        /////////////////

WCSimOpBoundaryProcess::WCSimOpBoundaryProcess(const G4String& processName,
                                               G4ProcessType type)
             : G4VDiscreteProcess(processName, type)
{
        if ( verboseLevel > 0) {
           G4cout << GetProcessName() << " is created " << G4endl;
        }

        SetProcessSubType(fOpBoundary);

        theStatus = Undefined;
        theModel = glisur;
        theFinish = polished;
        theReflectivity =  1.;
        theEfficiency   =  0.;
        theTransmittance = 0.;

        theSurfaceRoughness = 0.;

        prob_sl = 0.;
        prob_ss = 0.;
        prob_bs = 0.;

        PropertyPointer  = NULL;
        PropertyPointer1 = NULL;
        PropertyPointer2 = NULL;

        Material1 = NULL;
        Material2 = NULL;

        OpticalSurface = NULL;

        kCarTolerance = G4GeometryTolerance::GetInstance()
                        ->GetSurfaceTolerance();

        iTE = iTM = 0;
        thePhotonMomentum = 0.;
        Rindex1 = Rindex2 = 1.;
        cost1 = cost2 = sint1 = sint2 = 0.;

        // Thin film properties
        fCoatedRindex = 1;
        fCoatedRindexIm = 0;
        fCoatedThickness = 0;

        idx = idy = 0;
        DichroicVector = NULL;

        fInvokeSD = true;
}

// WCSimOpBoundaryProcess::WCSimOpBoundaryProcess(const WCSimOpBoundaryProcess &right)
// {
// }

        ////////////////
        // Destructors
        ////////////////

WCSimOpBoundaryProcess::~WCSimOpBoundaryProcess(){}

        ////////////
        // Methods
        ////////////

// PostStepDoIt
// ------------
//

G4VParticleChange*
WCSimOpBoundaryProcess::PostStepDoIt(const G4Track& aTrack, const G4Step& aStep)
{
        theStatus = Undefined;

        aParticleChange.Initialize(aTrack);
        aParticleChange.ProposeVelocity(aTrack.GetVelocity());

        // Get hyperStep from  G4ParallelWorldProcess
        //  NOTE: PostSetpDoIt of this process should be
        //        invoked after G4ParallelWorldProcess!

        const G4Step* pStep = &aStep;

        const G4Step* hStep = G4ParallelWorldProcess::GetHyperStep();
        
        if (hStep) pStep = hStep;

        G4bool isOnBoundary =
                (pStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary);

        if (isOnBoundary) {
           Material1 = pStep->GetPreStepPoint()->GetMaterial();
           Material2 = pStep->GetPostStepPoint()->GetMaterial();
        } else {
           theStatus = NotAtBoundary;
           if ( verboseLevel > 0) BoundaryProcessVerbose();
           return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
        }

        G4VPhysicalVolume* thePrePV  =
                               pStep->GetPreStepPoint() ->GetPhysicalVolume();
        G4VPhysicalVolume* thePostPV =
                               pStep->GetPostStepPoint()->GetPhysicalVolume();

        if ( verboseLevel > 0 ) {
           G4cout << " Photon at Boundary! " << G4endl;
           if (thePrePV)  G4cout << " thePrePV:  " << thePrePV->GetName()  << G4endl;
           if (thePostPV) G4cout << " thePostPV: " << thePostPV->GetName() << G4endl;
        }

        if (aTrack.GetStepLength()<=kCarTolerance/2){
                theStatus = StepTooSmall;
                if ( verboseLevel > 0) BoundaryProcessVerbose();
                return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
        }

        const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();

        thePhotonMomentum = aParticle->GetTotalMomentum();
        OldMomentum       = aParticle->GetMomentumDirection();
        OldPolarization   = aParticle->GetPolarization();

        if ( verboseLevel > 0 ) {
           G4cout << " Old Momentum Direction: " << OldMomentum     << G4endl;
           G4cout << " Old Polarization:       " << OldPolarization << G4endl;
        }

        G4ThreeVector theGlobalPoint = pStep->GetPostStepPoint()->GetPosition();

        G4bool valid;
        //  Use the new method for Exit Normal in global coordinates,
        //    which provides the normal more reliably.

        // ID of Navigator which limits step

        G4int hNavId = G4ParallelWorldProcess::GetHypNavigatorID();
        std::vector<G4Navigator*>::iterator iNav =
                G4TransportationManager::GetTransportationManager()->
                                         GetActiveNavigatorsIterator();
        theGlobalNormal =
                   (iNav[hNavId])->GetGlobalExitNormal(theGlobalPoint,&valid);

        if (valid) {
          theGlobalNormal = -theGlobalNormal;
        }
        else 
        {
          G4ExceptionDescription ed;
          ed << " WCSimOpBoundaryProcess/PostStepDoIt(): "
                 << " The Navigator reports that it returned an invalid normal"
                 << G4endl;
          G4Exception("WCSimOpBoundaryProcess::PostStepDoIt", "OpBoun01",
                      EventMustBeAborted,ed,
                      "Invalid Surface Normal - Geometry must return valid surface normal");
        }

        if (OldMomentum * theGlobalNormal > 0.0) {
#ifdef G4OPTICAL_DEBUG
           G4ExceptionDescription ed;
           ed << " WCSimOpBoundaryProcess/PostStepDoIt(): "
              << " theGlobalNormal points in a wrong direction. "
              << G4endl;
           ed << "    The momentum of the photon arriving at interface (oldMomentum)"
              << " must exit the volume cross in the step. " << G4endl;
           ed << "  So it MUST have dot < 0 with the normal that Exits the new volume (globalNormal)." << G4endl;
           ed << "  >> The dot product of oldMomentum and global Normal is " << OldMomentum*theGlobalNormal << G4endl;
           ed << "     Old Momentum  (during step)     = " << OldMomentum << G4endl;
           ed << "     Global Normal (Exiting New Vol) = " << theGlobalNormal << G4endl;
           ed << G4endl;
           G4Exception("WCSimOpBoundaryProcess::PostStepDoIt", "OpBoun02",
                       EventMustBeAborted,  // Or JustWarning to see if it happens repeatedbly on one ray
                       ed,
                      "Invalid Surface Normal - Geometry must return valid surface normal pointing in the right direction");
#else
           theGlobalNormal = -theGlobalNormal;
#endif
        }

        G4MaterialPropertiesTable* aMaterialPropertiesTable;
            G4MaterialPropertyVector* Rindex;

        aMaterialPropertiesTable = Material1->GetMaterialPropertiesTable();
        if (aMaterialPropertiesTable) {
		    Rindex = aMaterialPropertiesTable->GetProperty("RINDEX");
        }
        else {
                    theStatus = NoRINDEX;
                    if ( verboseLevel > 0) BoundaryProcessVerbose();
                    aParticleChange.ProposeLocalEnergyDeposit(thePhotonMomentum);
                    aParticleChange.ProposeTrackStatus(fStopAndKill);
                    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
        }

        if (Rindex) {
           Rindex1 = Rindex->Value(thePhotonMomentum);
        }
        else {
	        theStatus = NoRINDEX;
                if ( verboseLevel > 0) BoundaryProcessVerbose();
                aParticleChange.ProposeLocalEnergyDeposit(thePhotonMomentum);
                aParticleChange.ProposeTrackStatus(fStopAndKill);
                return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
        }

        theReflectivity =  1.;
        theEfficiency   =  0.;
        theTransmittance = 0.;

        theSurfaceRoughness = 0.;

        theModel = glisur;
        theFinish = polished;

        G4SurfaceType type = dielectric_dielectric;

        Rindex = NULL;
        OpticalSurface = NULL;

        G4LogicalSurface* Surface = NULL;

        Surface = G4LogicalBorderSurface::GetSurface(thePrePV, thePostPV);

        if (Surface == NULL){
          G4bool enteredDaughter= (thePostPV->GetMotherLogical() ==
                                   thePrePV ->GetLogicalVolume());
            if(enteredDaughter){
                Surface = 
                    G4LogicalSkinSurface::GetSurface(thePostPV->GetLogicalVolume());
                if(Surface == NULL)
                Surface =
                        G4LogicalSkinSurface::GetSurface(thePrePV->GetLogicalVolume());
            }
            else {
                Surface =
                    G4LogicalSkinSurface::GetSurface(thePrePV->GetLogicalVolume());
                if(Surface == NULL)
                Surface =
                        G4LogicalSkinSurface::GetSurface(thePostPV->GetLogicalVolume());
            }
        }

        if (Surface) OpticalSurface = 
           dynamic_cast <G4OpticalSurface*> (Surface->GetSurfaceProperty());

        if (OpticalSurface) {

           type      = OpticalSurface->GetType();
           theModel  = OpticalSurface->GetModel();
           theFinish = OpticalSurface->GetFinish();

           aMaterialPropertiesTable = OpticalSurface->
                                        GetMaterialPropertiesTable();

           if (aMaterialPropertiesTable) {

              if (theFinish == polishedbackpainted ||
                  theFinish == groundbackpainted ) {
                  Rindex = aMaterialPropertiesTable->GetProperty("RINDEX");
	          if (Rindex) {
                     Rindex2 = Rindex->Value(thePhotonMomentum);
                  }
                  else {
                     theStatus = NoRINDEX;
                     if ( verboseLevel > 0) BoundaryProcessVerbose();
                     aParticleChange.ProposeLocalEnergyDeposit(thePhotonMomentum);
                     aParticleChange.ProposeTrackStatus(fStopAndKill);
                     return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
                  }
              }

              PropertyPointer =
                      aMaterialPropertiesTable->GetProperty("REFLECTIVITY");
              PropertyPointer1 =
                      aMaterialPropertiesTable->GetProperty("REALRINDEX");
              PropertyPointer2 =
                      aMaterialPropertiesTable->GetProperty("IMAGINARYRINDEX");

              iTE = 1;
              iTM = 1;

              if (PropertyPointer) {

                 theReflectivity =
                          PropertyPointer->Value(thePhotonMomentum);

              } else if (PropertyPointer1 && PropertyPointer2) {

                 CalculateReflectivity();

              }

              PropertyPointer =
              aMaterialPropertiesTable->GetProperty("EFFICIENCY");
              if (PropertyPointer) {
                      theEfficiency =
                      PropertyPointer->Value(thePhotonMomentum);
              }

              PropertyPointer =
              aMaterialPropertiesTable->GetProperty("TRANSMITTANCE");
              if (PropertyPointer) {
                      theTransmittance =
                      PropertyPointer->Value(thePhotonMomentum);
              }

              if (aMaterialPropertiesTable->
                                     ConstPropertyExists("SURFACEROUGHNESS"))
                 theSurfaceRoughness = aMaterialPropertiesTable->
                                         GetConstProperty("SURFACEROUGHNESS");

	      if ( theModel == unified ) {
                 PropertyPointer =
                 aMaterialPropertiesTable->GetProperty("SPECULARLOBECONSTANT");
                 if (PropertyPointer) {
                         prob_sl =
                         PropertyPointer->Value(thePhotonMomentum);
                 } else {
                         prob_sl = 0.0;
                 }

                 PropertyPointer =
                 aMaterialPropertiesTable->GetProperty("SPECULARSPIKECONSTANT");
	         if (PropertyPointer) {
                         prob_ss =
                         PropertyPointer->Value(thePhotonMomentum);
                 } else {
                         prob_ss = 0.0;
                 }

                 PropertyPointer =
                 aMaterialPropertiesTable->GetProperty("BACKSCATTERCONSTANT");
                 if (PropertyPointer) {
                         prob_bs =
                         PropertyPointer->Value(thePhotonMomentum);
                 } else {
                         prob_bs = 0.0;
                 }
              }
           }
           else if (theFinish == polishedbackpainted ||
                    theFinish == groundbackpainted ) {
                      aParticleChange.ProposeLocalEnergyDeposit(thePhotonMomentum);
                      aParticleChange.ProposeTrackStatus(fStopAndKill);
                      return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
           }
        }

        if (type == dielectric_dielectric ) {
           if (theFinish == polished || theFinish == ground ) {

              if (Material1 == Material2){
                 theStatus = SameMaterial;
                 if ( verboseLevel > 0) BoundaryProcessVerbose();
		 return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
	      }
              aMaterialPropertiesTable =
                     Material2->GetMaterialPropertiesTable();
              if (aMaterialPropertiesTable)
                 Rindex = aMaterialPropertiesTable->GetProperty("RINDEX");
              if (Rindex) {
                 Rindex2 = Rindex->Value(thePhotonMomentum);
              }
              else {
                 theStatus = NoRINDEX;
                 if ( verboseLevel > 0) BoundaryProcessVerbose();
                 aParticleChange.ProposeLocalEnergyDeposit(thePhotonMomentum);
                 aParticleChange.ProposeTrackStatus(fStopAndKill);
                 return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
              }
           }
        }

	if (type == dielectric_metal) {

          DielectricMetal();

	}
        else if (type == dielectric_LUT) {

          DielectricLUT();

        }
#if G4VERSION_NUMBER > 1033
        else if (type == dielectric_LUTDAVIS) {

          DielectricLUTDAVIS();

        }
#endif
        else if (type == dielectric_dichroic) {

          DielectricDichroic();

        }
        else if (type == dielectric_dielectric) {

          if ( theFinish == polishedbackpainted ||
               theFinish == groundbackpainted ) {
             DielectricDielectric();
          }
          else {
             G4double rand = G4UniformRand();
             if ( rand > theReflectivity ) {
                if (rand > theReflectivity + theTransmittance) {
                   DoAbsorption();
                } else {
                   theStatus = Transmission;
                   NewMomentum = OldMomentum;
                   NewPolarization = OldPolarization;
                }
             }
             else {
                if ( theFinish == polishedfrontpainted ) {
                   DoReflection();
                }
                else if ( theFinish == groundfrontpainted ) {
                   theStatus = LambertianReflection;
                   DoReflection();
                }
                else {
                   DielectricDielectric();
                }
             }
          }
        }
        else if (type == 101) // brute-force index to get coated surface processes
        {
            CoatedDielectricDielectric_Model1();
        }
        else if (type == 102)
        {
            CoatedDielectricDielectric_Model2();
        }
        else {

          G4cerr << " Error: G4BoundaryProcess: illegal boundary type " << G4endl;
          return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);

        }

        NewMomentum = NewMomentum.unit();
        NewPolarization = NewPolarization.unit();

        if ( verboseLevel > 0) {
           G4cout << " New Momentum Direction: " << NewMomentum     << G4endl;
           G4cout << " New Polarization:       " << NewPolarization << G4endl;
           BoundaryProcessVerbose();
        }

        aParticleChange.ProposeMomentumDirection(NewMomentum);
        aParticleChange.ProposePolarization(NewPolarization);

        if ( theStatus == FresnelRefraction || theStatus == Transmission ) {
           G4MaterialPropertyVector* groupvel =
           Material2->GetMaterialPropertiesTable()->GetProperty("GROUPVEL");
           G4double finalVelocity = groupvel->Value(thePhotonMomentum);
           aParticleChange.ProposeVelocity(finalVelocity);
        }

        if ( theStatus == Detection && fInvokeSD ) InvokeSD(pStep);

        return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
}

void WCSimOpBoundaryProcess::BoundaryProcessVerbose() const
{
        if ( theStatus == Undefined )
                G4cout << " *** Undefined *** " << G4endl;
        if ( theStatus == Transmission )
                G4cout << " *** Transmission *** " << G4endl;
        if ( theStatus == FresnelRefraction )
                G4cout << " *** FresnelRefraction *** " << G4endl;
        if ( theStatus == FresnelReflection )
                G4cout << " *** FresnelReflection *** " << G4endl;
        if ( theStatus == TotalInternalReflection )
                G4cout << " *** TotalInternalReflection *** " << G4endl;
        if ( theStatus == LambertianReflection )
                G4cout << " *** LambertianReflection *** " << G4endl;
        if ( theStatus == LobeReflection )
                G4cout << " *** LobeReflection *** " << G4endl;
        if ( theStatus == SpikeReflection )
                G4cout << " *** SpikeReflection *** " << G4endl;
        if ( theStatus == BackScattering )
                G4cout << " *** BackScattering *** " << G4endl;
        if ( theStatus == PolishedLumirrorAirReflection )
                G4cout << " *** PolishedLumirrorAirReflection *** " << G4endl;
        if ( theStatus == PolishedLumirrorGlueReflection )
                G4cout << " *** PolishedLumirrorGlueReflection *** " << G4endl;
        if ( theStatus == PolishedAirReflection )
                G4cout << " *** PolishedAirReflection *** " << G4endl;
        if ( theStatus == PolishedTeflonAirReflection )
                G4cout << " *** PolishedTeflonAirReflection *** " << G4endl;
        if ( theStatus == PolishedTiOAirReflection )
                G4cout << " *** PolishedTiOAirReflection *** " << G4endl;
        if ( theStatus == PolishedTyvekAirReflection )
                G4cout << " *** PolishedTyvekAirReflection *** " << G4endl;
        if ( theStatus == PolishedVM2000AirReflection )
                G4cout << " *** PolishedVM2000AirReflection *** " << G4endl;
        if ( theStatus == PolishedVM2000GlueReflection )
                G4cout << " *** PolishedVM2000GlueReflection *** " << G4endl;
        if ( theStatus == EtchedLumirrorAirReflection )
                G4cout << " *** EtchedLumirrorAirReflection *** " << G4endl;
        if ( theStatus == EtchedLumirrorGlueReflection )
                G4cout << " *** EtchedLumirrorGlueReflection *** " << G4endl;
        if ( theStatus == EtchedAirReflection )
                G4cout << " *** EtchedAirReflection *** " << G4endl;
        if ( theStatus == EtchedTeflonAirReflection )
                G4cout << " *** EtchedTeflonAirReflection *** " << G4endl;
        if ( theStatus == EtchedTiOAirReflection )
                G4cout << " *** EtchedTiOAirReflection *** " << G4endl;
        if ( theStatus == EtchedTyvekAirReflection )
                G4cout << " *** EtchedTyvekAirReflection *** " << G4endl;
        if ( theStatus == EtchedVM2000AirReflection )
                G4cout << " *** EtchedVM2000AirReflection *** " << G4endl;
        if ( theStatus == EtchedVM2000GlueReflection )
                G4cout << " *** EtchedVM2000GlueReflection *** " << G4endl;
        if ( theStatus == GroundLumirrorAirReflection )
                G4cout << " *** GroundLumirrorAirReflection *** " << G4endl;
        if ( theStatus == GroundLumirrorGlueReflection )
                G4cout << " *** GroundLumirrorGlueReflection *** " << G4endl;
        if ( theStatus == GroundAirReflection )
                G4cout << " *** GroundAirReflection *** " << G4endl;
        if ( theStatus == GroundTeflonAirReflection )
                G4cout << " *** GroundTeflonAirReflection *** " << G4endl;
        if ( theStatus == GroundTiOAirReflection )
                G4cout << " *** GroundTiOAirReflection *** " << G4endl;
        if ( theStatus == GroundTyvekAirReflection )
                G4cout << " *** GroundTyvekAirReflection *** " << G4endl;
        if ( theStatus == GroundVM2000AirReflection )
                G4cout << " *** GroundVM2000AirReflection *** " << G4endl;
        if ( theStatus == GroundVM2000GlueReflection )
                G4cout << " *** GroundVM2000GlueReflection *** " << G4endl;
        if ( theStatus == Absorption )
                G4cout << " *** Absorption *** " << G4endl;
        if ( theStatus == Detection )
                G4cout << " *** Detection *** " << G4endl;
        if ( theStatus == NotAtBoundary )
                G4cout << " *** NotAtBoundary *** " << G4endl;
        if ( theStatus == SameMaterial )
                G4cout << " *** SameMaterial *** " << G4endl;
        if ( theStatus == StepTooSmall )
                G4cout << " *** StepTooSmall *** " << G4endl;
        if ( theStatus == NoRINDEX )
                G4cout << " *** NoRINDEX *** " << G4endl;
        if ( theStatus == Dichroic )
                G4cout << " *** Dichroic Transmission *** " << G4endl;
        if(theStatus == CoatedDielectricReflection)
                G4cout << " *** Coated Dielectric Reflection *** " << G4endl;
        if(theStatus == CoatedDielectricRefraction)
                G4cout << " *** Coated Dielectric Refraction *** " << G4endl;
        if(theStatus == CoatedDielectricFrustratedTransmission)
                G4cout << " *** Coated Dielectric Frustrated Transmission *** " << G4endl;
}

G4ThreeVector
WCSimOpBoundaryProcess::GetFacetNormal(const G4ThreeVector& Momentum,
			            const G4ThreeVector&  Normal ) const
{
        G4ThreeVector FacetNormal;
#if G4VERSION_NUMBER > 1033
        if (theModel == unified || theModel == LUT || theModel== DAVIS) 
#else
        if (theModel == unified || theModel == LUT) 
#endif
        {
           /* This function code alpha to a random value taken from the
           distribution p(alpha) = g(alpha; 0, sigma_alpha)*std::sin(alpha),
           for alpha > 0 and alpha < 90, where g(alpha; 0, sigma_alpha)
           is a gaussian distribution with mean 0 and standard deviation
           sigma_alpha.  */

           G4double alpha;

           G4double sigma_alpha = 0.0;
           if (OpticalSurface) sigma_alpha = OpticalSurface->GetSigmaAlpha();

           if (sigma_alpha == 0.0) return FacetNormal = Normal;

           G4double f_max = std::min(1.0,4.*sigma_alpha);

           G4double phi, SinAlpha, CosAlpha, SinPhi, CosPhi, unit_x, unit_y, unit_z;
           G4ThreeVector tmpNormal;

           do {
              do {
                 alpha = G4RandGauss::shoot(0.0,sigma_alpha);
                 // Loop checking, 13-Aug-2015, Peter Gumplinger
              } while (G4UniformRand()*f_max > std::sin(alpha) || alpha >= halfpi );

              phi = G4UniformRand()*twopi;

              SinAlpha = std::sin(alpha);
              CosAlpha = std::cos(alpha);
              SinPhi = std::sin(phi);
              CosPhi = std::cos(phi);

              unit_x = SinAlpha * CosPhi;
              unit_y = SinAlpha * SinPhi;
              unit_z = CosAlpha;

              FacetNormal.setX(unit_x);
              FacetNormal.setY(unit_y);
              FacetNormal.setZ(unit_z);

              tmpNormal = Normal;

              FacetNormal.rotateUz(tmpNormal);
              // Loop checking, 13-Aug-2015, Peter Gumplinger
           } while (Momentum * FacetNormal >= 0.0);
	}
        else {

           G4double  polish = 1.0;
           if (OpticalSurface) polish = OpticalSurface->GetPolish();

           if (polish < 1.0) {
              do {
                 G4ThreeVector smear;
                 do {
                    smear.setX(2.*G4UniformRand()-1.0);
                    smear.setY(2.*G4UniformRand()-1.0);
                    smear.setZ(2.*G4UniformRand()-1.0);
                    // Loop checking, 13-Aug-2015, Peter Gumplinger
                 } while (smear.mag()>1.0);
                 smear = (1.-polish) * smear;
                 FacetNormal = Normal + smear;
                 // Loop checking, 13-Aug-2015, Peter Gumplinger
              } while (Momentum * FacetNormal >= 0.0);
              FacetNormal = FacetNormal.unit();
           }
           else {
              FacetNormal = Normal;
           }
	}
        return FacetNormal;
}

void WCSimOpBoundaryProcess::DielectricMetal()
{
        G4int n = 0;
        G4double rand, PdotN, EdotN;
        G4ThreeVector A_trans, A_paral;

        do {

           n++;

           rand = G4UniformRand();
           if ( rand > theReflectivity && n == 1 ) {
              if (rand > theReflectivity + theTransmittance) {
                DoAbsorption();
              } else {
                theStatus = Transmission;
                NewMomentum = OldMomentum;
                NewPolarization = OldPolarization;
              }
              break;
           }
           else {

             if (PropertyPointer1 && PropertyPointer2) {
                if ( n > 1 ) {
                   CalculateReflectivity();
                   if ( !G4BooleanRand(theReflectivity) ) {
                      DoAbsorption();
                      break;
                   }
                }
             }

             if ( theModel == glisur || theFinish == polished ) {

                DoReflection();

             } else {

                if ( n == 1 ) ChooseReflection();
                                                                                
                if ( theStatus == LambertianReflection ) {
                   DoReflection();
                }
                else if ( theStatus == BackScattering ) {
                   NewMomentum = -OldMomentum;
                   NewPolarization = -OldPolarization;
                }
                else {

                   if(theStatus==LobeReflection){
                     if ( PropertyPointer1 && PropertyPointer2 ){
                     } else {
                        theFacetNormal =
                            GetFacetNormal(OldMomentum,theGlobalNormal);
                     }
                   }

                   PdotN = OldMomentum * theFacetNormal;
                   NewMomentum = OldMomentum - (2.*PdotN)*theFacetNormal;
                   EdotN = OldPolarization * theFacetNormal;

                   if (sint1 > 0.0 ) {
                      A_trans = OldMomentum.cross(theFacetNormal);
                      A_trans = A_trans.unit();
                   } else {
                      A_trans  = OldPolarization;
                   }
                   A_paral   = NewMomentum.cross(A_trans);
                   A_paral   = A_paral.unit();

                   if(iTE>0&&iTM>0) {
                     NewPolarization = 
                           -OldPolarization + (2.*EdotN)*theFacetNormal;
                   } else if (iTE>0) {
                     NewPolarization = -A_trans;
                   } else if (iTM>0) {
                     NewPolarization = -A_paral;
                   }

                }

             }

             OldMomentum = NewMomentum;
             OldPolarization = NewPolarization;

	   }

          // Loop checking, 13-Aug-2015, Peter Gumplinger
	} while (NewMomentum * theGlobalNormal < 0.0);
}

void WCSimOpBoundaryProcess::DielectricLUT()
{
        G4int thetaIndex, phiIndex;
        G4double AngularDistributionValue, thetaRad, phiRad, EdotN;
        G4ThreeVector PerpendicularVectorTheta, PerpendicularVectorPhi;

        theStatus = WCSimOpBoundaryProcessStatus(G4int(theFinish) + 
                           (G4int(NoRINDEX)-G4int(groundbackpainted)));

        G4int thetaIndexMax = OpticalSurface->GetThetaIndexMax();
        G4int phiIndexMax   = OpticalSurface->GetPhiIndexMax();

        G4double rand;

        do {
           rand = G4UniformRand();
           if ( rand > theReflectivity ) {
              if (rand > theReflectivity + theTransmittance) {
                 DoAbsorption();
              } else {
                 theStatus = Transmission;
                 NewMomentum = OldMomentum;
                 NewPolarization = OldPolarization;
              }
              break;
           }
           else {
              // Calculate Angle between Normal and Photon Momentum
              G4double anglePhotonToNormal = 
                                          OldMomentum.angle(-theGlobalNormal);
              // Round it to closest integer
              G4int angleIncident = G4int(std::floor(180/pi*anglePhotonToNormal+0.5));

              // Take random angles THETA and PHI, 
              // and see if below Probability - if not - Redo
              do {
                 thetaIndex = G4RandFlat::shootInt(thetaIndexMax-1);
                 phiIndex = G4RandFlat::shootInt(phiIndexMax-1);
                 // Find probability with the new indeces from LUT
                 AngularDistributionValue = OpticalSurface -> 
                   GetAngularDistributionValue(angleIncident,
                                               thetaIndex,
                                               phiIndex);
                // Loop checking, 13-Aug-2015, Peter Gumplinger
              } while ( !G4BooleanRand(AngularDistributionValue) );

              thetaRad = (-90 + 4*thetaIndex)*pi/180;
              phiRad = (-90 + 5*phiIndex)*pi/180;
              // Rotate Photon Momentum in Theta, then in Phi
              NewMomentum = -OldMomentum;

              PerpendicularVectorTheta = NewMomentum.cross(theGlobalNormal);
              if (PerpendicularVectorTheta.mag() < kCarTolerance )
                          PerpendicularVectorTheta = NewMomentum.orthogonal();
              NewMomentum =
                 NewMomentum.rotate(anglePhotonToNormal-thetaRad,
                                    PerpendicularVectorTheta);
              PerpendicularVectorPhi = 
                                  PerpendicularVectorTheta.cross(NewMomentum);
              NewMomentum = NewMomentum.rotate(-phiRad,PerpendicularVectorPhi);

              // Rotate Polarization too:
              theFacetNormal = (NewMomentum - OldMomentum).unit();
              EdotN = OldPolarization * theFacetNormal;
              NewPolarization = -OldPolarization + (2.*EdotN)*theFacetNormal;
           }
          // Loop checking, 13-Aug-2015, Peter Gumplinger
        } while (NewMomentum * theGlobalNormal <= 0.0);
}
#if G4VERSION_NUMBER > 1033
void WCSimOpBoundaryProcess::DielectricLUTDAVIS()
{
  G4int angindex, random, angleIncident;
  G4double ReflectivityValue, elevation, azimuth, EdotN;
  G4double anglePhotonToNormal;

  G4int LUTbin = OpticalSurface->GetLUTbins();

  G4double rand = G4UniformRand();

  do {

     anglePhotonToNormal = OldMomentum.angle(-theGlobalNormal);
     angleIncident = G4int(std::floor(180/pi*anglePhotonToNormal+0.5));

     ReflectivityValue = OpticalSurface -> GetReflectivityLUTValue(angleIncident);

     if ( rand > ReflectivityValue ) {

        if ( theEfficiency > 0 ) {
           DoAbsorption();
           break;
        }
        else {

           theStatus = Transmission;

           if (angleIncident <= 0.01) {
              NewMomentum = OldMomentum;
              break;

           }

           do {
              random   = G4RandFlat::shootInt(1,LUTbin+1);
              angindex = (((random*2)-1))+angleIncident*LUTbin*2 + 3640000;

              azimuth  = OpticalSurface -> GetAngularDistributionValueLUT(angindex-1);
              elevation= OpticalSurface -> GetAngularDistributionValueLUT(angindex);

           } while ( elevation == 0 && azimuth == 0);

           NewMomentum = -OldMomentum;

           G4ThreeVector v = theGlobalNormal.cross(-NewMomentum);
           G4ThreeVector vNorm = v/v.mag();
           G4ThreeVector u = vNorm.cross(theGlobalNormal);

           u = u *= (std::sin(elevation) * std::cos(azimuth));
           v = vNorm *= (std::sin(elevation) * std::sin(azimuth));
           G4ThreeVector w = theGlobalNormal *= (std::cos(elevation));
           NewMomentum = G4ThreeVector(u+v+w);

           // Rotate Polarization too:
           theFacetNormal = (NewMomentum - OldMomentum).unit();
           EdotN = OldPolarization * theFacetNormal;
           NewPolarization = -OldPolarization + (2.*EdotN)*theFacetNormal;
        }
     }
     else {

        theStatus = LobeReflection;

        if (angleIncident == 0) {
           NewMomentum = -OldMomentum;
           break;
        }

        do {
           random   = G4RandFlat::shootInt(1,LUTbin+1);
           angindex = (((random*2)-1))+(angleIncident-1)*LUTbin*2;

           azimuth   = OpticalSurface -> GetAngularDistributionValueLUT(angindex-1);
           elevation = OpticalSurface -> GetAngularDistributionValueLUT(angindex);
        } while (elevation == 0 && azimuth == 0);

        NewMomentum = -OldMomentum;

        G4ThreeVector v     = theGlobalNormal.cross(-NewMomentum);
        G4ThreeVector vNorm = v/v.mag();
        G4ThreeVector u     = vNorm.cross(theGlobalNormal);

        u = u *= (std::sin(elevation) * std::cos(azimuth));
        v = vNorm *= (std::sin(elevation) * std::sin(azimuth));
        G4ThreeVector w = theGlobalNormal*=(std::cos(elevation));

        NewMomentum = G4ThreeVector(u+v+w);

        // Rotate Polarization too: (needs revision)
        NewPolarization = OldPolarization;
     }
  } while (NewMomentum * theGlobalNormal <= 0.0);
}
#endif
void WCSimOpBoundaryProcess::DielectricDichroic()
{
        // Calculate Angle between Normal and Photon Momentum
        G4double anglePhotonToNormal = OldMomentum.angle(-theGlobalNormal);

        // Round it to closest integer
        G4double angleIncident = std::floor(180/pi*anglePhotonToNormal+0.5);

        if (!DichroicVector) {
           if (OpticalSurface) DichroicVector = OpticalSurface->GetDichroicVector();
        }


        if (DichroicVector) {
           G4double wavelength = h_Planck*c_light/thePhotonMomentum;
           theTransmittance =
             DichroicVector->Value(wavelength/nm,angleIncident,idx,idy)*perCent;
//            G4cout << "wavelength: " << std::floor(wavelength/nm) 
//                                     << "nm" << G4endl;
//            G4cout << "Incident angle: " << angleIncident << "deg" << G4endl;
//            G4cout << "Transmittance: " 
//                   << std::floor(theTransmittance/perCent) << "%" << G4endl;
        } else {
           G4ExceptionDescription ed;
           ed << " WCSimOpBoundaryProcess/DielectricDichroic(): "
              << " The dichroic surface has no G4Physics2DVector"
              << G4endl;
           G4Exception("WCSimOpBoundaryProcess::DielectricDichroic", "OpBoun03",
                       FatalException,ed,
                       "A dichroic surface must have an associated G4Physics2DVector");
        }

        if ( !G4BooleanRand(theTransmittance) ) { // Not transmitted, so reflect

           if ( theModel == glisur || theFinish == polished ) {
              DoReflection();
           } else {
              ChooseReflection();
              if ( theStatus == LambertianReflection ) {
                 DoReflection();
              } else if ( theStatus == BackScattering ) {
                 NewMomentum = -OldMomentum;
                 NewPolarization = -OldPolarization;
              } else {
                G4double PdotN, EdotN;
                do {
                   if (theStatus==LobeReflection)
                      theFacetNormal = GetFacetNormal(OldMomentum,theGlobalNormal);
                   PdotN = OldMomentum * theFacetNormal;
                   NewMomentum = OldMomentum - (2.*PdotN)*theFacetNormal;
                  // Loop checking, 13-Aug-2015, Peter Gumplinger
                } while (NewMomentum * theGlobalNormal <= 0.0);
                EdotN = OldPolarization * theFacetNormal;
                NewPolarization = -OldPolarization + (2.*EdotN)*theFacetNormal;
              }
           }

        } else {

           theStatus = Dichroic;
           NewMomentum = OldMomentum;
           NewPolarization = OldPolarization;

        }
}

void WCSimOpBoundaryProcess::DielectricDielectric()
{
        G4bool Inside = false;
        G4bool Swap = false;

        G4bool SurfaceRoughnessCriterionPass = 1;
        if (theSurfaceRoughness != 0. && Rindex1 > Rindex2) {
           G4double wavelength = h_Planck*c_light/thePhotonMomentum;
           G4double SurfaceRoughnessCriterion =
             std::exp(-std::pow((4*pi*theSurfaceRoughness*Rindex1*cost1/wavelength),2));
           SurfaceRoughnessCriterionPass = 
                                     G4BooleanRand(SurfaceRoughnessCriterion);
        }

        leap:

        G4bool Through = false;
        G4bool Done = false;

        G4double PdotN, EdotN;

        G4ThreeVector A_trans, A_paral, E1pp, E1pl;
        G4double E1_perp, E1_parl;
        G4double s1, s2, E2_perp, E2_parl, E2_total, TransCoeff;
        G4double E2_abs, C_parl, C_perp;
        G4double alpha;

        do {

           if (Through) {
              Swap = !Swap;
              Through = false;
              theGlobalNormal = -theGlobalNormal;
              G4SwapPtr(Material1,Material2);
              G4SwapObj(&Rindex1,&Rindex2);
           }

           if ( theFinish == polished ) {
              theFacetNormal = theGlobalNormal;
           }
           else {
              theFacetNormal =
                             GetFacetNormal(OldMomentum,theGlobalNormal);
           }

           PdotN = OldMomentum * theFacetNormal;
           EdotN = OldPolarization * theFacetNormal;

           cost1 = - PdotN;
           if (std::abs(cost1) < 1.0-kCarTolerance){
              sint1 = std::sqrt(1.-cost1*cost1);
              sint2 = sint1*Rindex1/Rindex2;     // *** Snell's Law ***
           }
           else {
              sint1 = 0.0;
              sint2 = 0.0;
           }

           if (sint2 >= 1.0) {

              // Simulate total internal reflection

              if (Swap) Swap = !Swap;

              theStatus = TotalInternalReflection;

              if ( !SurfaceRoughnessCriterionPass ) theStatus =
                                                       LambertianReflection;

              if ( theModel == unified && theFinish != polished )
                                                    ChooseReflection();

              if ( theStatus == LambertianReflection ) {
                 DoReflection();
              }
              else if ( theStatus == BackScattering ) {
                 NewMomentum = -OldMomentum;
                 NewPolarization = -OldPolarization;
              }
              else {

                 PdotN = OldMomentum * theFacetNormal;
                 NewMomentum = OldMomentum - (2.*PdotN)*theFacetNormal;
                 EdotN = OldPolarization * theFacetNormal;
                 NewPolarization = -OldPolarization + (2.*EdotN)*theFacetNormal;

              }
           }
           else if (sint2 < 1.0) {

              // Calculate amplitude for transmission (Q = P x N)

              if (cost1 > 0.0) {
                 cost2 =  std::sqrt(1.-sint2*sint2);
              }
              else {
                 cost2 = -std::sqrt(1.-sint2*sint2);
              }

              if (sint1 > 0.0) {
                 A_trans = OldMomentum.cross(theFacetNormal);
                 A_trans = A_trans.unit();
                 E1_perp = OldPolarization * A_trans;
                 E1pp    = E1_perp * A_trans;
                 E1pl    = OldPolarization - E1pp;
                 E1_parl = E1pl.mag();
              }
              else {
                 A_trans  = OldPolarization;
                 // Here we Follow Jackson's conventions and we set the
                 // parallel component = 1 in case of a ray perpendicular
                 // to the surface
                 E1_perp  = 0.0;
                 E1_parl  = 1.0;
              }

              s1 = Rindex1*cost1;
              E2_perp = 2.*s1*E1_perp/(Rindex1*cost1+Rindex2*cost2);
              E2_parl = 2.*s1*E1_parl/(Rindex2*cost1+Rindex1*cost2);
              E2_total = E2_perp*E2_perp + E2_parl*E2_parl;
              s2 = Rindex2*cost2*E2_total;

              if (theTransmittance > 0) TransCoeff = theTransmittance;
              else if (cost1 != 0.0) TransCoeff = s2/s1;
              else TransCoeff = 0.0;

              if ( !G4BooleanRand(TransCoeff) ) {

                 // Simulate reflection

                 if (Swap) Swap = !Swap;

                 theStatus = FresnelReflection;

                 if ( !SurfaceRoughnessCriterionPass ) theStatus =
                                                          LambertianReflection;

                 if ( theModel == unified && theFinish != polished )
                                                     ChooseReflection();

                 if ( theStatus == LambertianReflection ) {
                    DoReflection();
                 }
                 else if ( theStatus == BackScattering ) {
                    NewMomentum = -OldMomentum;
                    NewPolarization = -OldPolarization;
                 }
                 else {

                    PdotN = OldMomentum * theFacetNormal;
                    NewMomentum = OldMomentum - (2.*PdotN)*theFacetNormal;

                    if (sint1 > 0.0) {   // incident ray oblique

                       E2_parl   = Rindex2*E2_parl/Rindex1 - E1_parl;
                       E2_perp   = E2_perp - E1_perp;
                       E2_total  = E2_perp*E2_perp + E2_parl*E2_parl;
                       A_paral   = NewMomentum.cross(A_trans);
                       A_paral   = A_paral.unit();
                       E2_abs    = std::sqrt(E2_total);
                       C_parl    = E2_parl/E2_abs;
                       C_perp    = E2_perp/E2_abs;

                       NewPolarization = C_parl*A_paral + C_perp*A_trans;

                    }

                    else {               // incident ray perpendicular

                       if (Rindex2 > Rindex1) {
                          NewPolarization = - OldPolarization;
                       }
                       else {
                          NewPolarization =   OldPolarization;
                       }

                    }
                 }
              }
              else { // photon gets transmitted

                // Simulate transmission/refraction

                Inside = !Inside;
                Through = true;
                theStatus = FresnelRefraction;

                if (sint1 > 0.0) {      // incident ray oblique

                   alpha = cost1 - cost2*(Rindex2/Rindex1);
                   NewMomentum = OldMomentum + alpha*theFacetNormal;
                   NewMomentum = NewMomentum.unit();
//                   PdotN = -cost2;
                   A_paral = NewMomentum.cross(A_trans);
                   A_paral = A_paral.unit();
                   E2_abs     = std::sqrt(E2_total);
                   C_parl     = E2_parl/E2_abs;
                   C_perp     = E2_perp/E2_abs;

                   NewPolarization = C_parl*A_paral + C_perp*A_trans;

                }
                else {                  // incident ray perpendicular

                   NewMomentum = OldMomentum;
                   NewPolarization = OldPolarization;

                }
              }
           }

           OldMomentum = NewMomentum.unit();
           OldPolarization = NewPolarization.unit();

           if (theStatus == FresnelRefraction) {
              Done = (NewMomentum * theGlobalNormal <= 0.0);
           } 
           else {
              Done = (NewMomentum * theGlobalNormal >= -kCarTolerance);
	   }

        // Loop checking, 13-Aug-2015, Peter Gumplinger
	} while (!Done);

        if (Inside && !Swap) {
          if( theFinish == polishedbackpainted ||
              theFinish == groundbackpainted ) {

              G4double rand = G4UniformRand();
              if ( rand > theReflectivity ) {
                 if (rand > theReflectivity + theTransmittance) {
                    DoAbsorption();
                 } else {
                    theStatus = Transmission;
                    NewMomentum = OldMomentum;
                    NewPolarization = OldPolarization;
                 }
              }
	      else {
                 if (theStatus != FresnelRefraction ) {
                    theGlobalNormal = -theGlobalNormal;
                 }
                 else {
                    Swap = !Swap;
                    G4SwapPtr(Material1,Material2);
                    G4SwapObj(&Rindex1,&Rindex2);
                 }
                 if ( theFinish == groundbackpainted )
                                        theStatus = LambertianReflection;

                 DoReflection();

                 theGlobalNormal = -theGlobalNormal;
                 OldMomentum = NewMomentum;

                 goto leap;
              }
          }
        }
}

// GetMeanFreePath
// ---------------
//
G4double WCSimOpBoundaryProcess::GetMeanFreePath(const G4Track& ,
                                              G4double ,
                                              G4ForceCondition* condition)
{
  *condition = Forced;

  return DBL_MAX;
}

G4double WCSimOpBoundaryProcess::GetIncidentAngle() 
{
  G4double PdotN = OldMomentum * theFacetNormal;
  G4double magP= OldMomentum.mag();
  G4double magN= theFacetNormal.mag();
  G4double incidentangle = pi - std::acos(PdotN/(magP*magN));

  return incidentangle;
}

G4double WCSimOpBoundaryProcess::GetReflectivity(G4double E1_perp,
                                              G4double E1_parl,
                                              G4double incidentangle,
                                              G4double RealRindex,
                                              G4double ImaginaryRindex)
{
  G4complex Reflectivity, Reflectivity_TE, Reflectivity_TM;
  G4complex N1(Rindex1, 0), N2(RealRindex, ImaginaryRindex);
  G4complex CosPhi;

  G4complex u(1,0);           //unit number 1

  G4complex numeratorTE;      // E1_perp=1 E1_parl=0 -> TE polarization
  G4complex numeratorTM;      // E1_parl=1 E1_perp=0 -> TM polarization
  G4complex denominatorTE, denominatorTM;
  G4complex rTM, rTE;

  G4MaterialPropertiesTable* aMaterialPropertiesTable =
                                    Material1->GetMaterialPropertiesTable();
  G4MaterialPropertyVector* aPropertyPointerR =
                      aMaterialPropertiesTable->GetProperty("REALRINDEX");
  G4MaterialPropertyVector* aPropertyPointerI =
                      aMaterialPropertiesTable->GetProperty("IMAGINARYRINDEX");
  if (aPropertyPointerR && aPropertyPointerI) {
     G4double RRindex = aPropertyPointerR->Value(thePhotonMomentum);
     G4double IRindex = aPropertyPointerI->Value(thePhotonMomentum);
     N1 = G4complex(RRindex,IRindex);
  }

  // Following two equations, rTM and rTE, are from: "Introduction To Modern
  // Optics" written by Fowles

  CosPhi=std::sqrt(u-((std::sin(incidentangle)*std::sin(incidentangle))*(N1*N1)/(N2*N2)));

  numeratorTE   = N1*std::cos(incidentangle) - N2*CosPhi;
  denominatorTE = N1*std::cos(incidentangle) + N2*CosPhi;
  rTE = numeratorTE/denominatorTE;

  numeratorTM   = N2*std::cos(incidentangle) - N1*CosPhi;
  denominatorTM = N2*std::cos(incidentangle) + N1*CosPhi;
  rTM = numeratorTM/denominatorTM;

  // This is my calculaton for reflectivity on a metalic surface
  // depending on the fraction of TE and TM polarization
  // when TE polarization, E1_parl=0 and E1_perp=1, R=abs(rTE)^2 and
  // when TM polarization, E1_parl=1 and E1_perp=0, R=abs(rTM)^2

  Reflectivity_TE =  (rTE*conj(rTE))*(E1_perp*E1_perp)
                    / (E1_perp*E1_perp + E1_parl*E1_parl);
  Reflectivity_TM =  (rTM*conj(rTM))*(E1_parl*E1_parl)
                    / (E1_perp*E1_perp + E1_parl*E1_parl);
  Reflectivity    = Reflectivity_TE + Reflectivity_TM;

  do {
     if(G4UniformRand()*real(Reflectivity) > real(Reflectivity_TE))
       {iTE = -1;}else{iTE = 1;}
     if(G4UniformRand()*real(Reflectivity) > real(Reflectivity_TM))
       {iTM = -1;}else{iTM = 1;}
    // Loop checking, 13-Aug-2015, Peter Gumplinger
  } while(iTE<0&&iTM<0);

  return real(Reflectivity);

}

void WCSimOpBoundaryProcess::CalculateReflectivity()
{
  G4double RealRindex =
           PropertyPointer1->Value(thePhotonMomentum);
  G4double ImaginaryRindex =
           PropertyPointer2->Value(thePhotonMomentum);

  // calculate FacetNormal
  if ( theFinish == ground ) {
     theFacetNormal =
               GetFacetNormal(OldMomentum, theGlobalNormal);
  } else {
     theFacetNormal = theGlobalNormal;
  }

  G4double PdotN = OldMomentum * theFacetNormal;
  cost1 = -PdotN;

  if (std::abs(cost1) < 1.0 - kCarTolerance) {
     sint1 = std::sqrt(1. - cost1*cost1);
  } else {
     sint1 = 0.0;
  }

  G4ThreeVector A_trans, A_paral, E1pp, E1pl;
  G4double E1_perp, E1_parl;

  if (sint1 > 0.0 ) {
     A_trans = OldMomentum.cross(theFacetNormal);
     A_trans = A_trans.unit();
     E1_perp = OldPolarization * A_trans;
     E1pp    = E1_perp * A_trans;
     E1pl    = OldPolarization - E1pp;
     E1_parl = E1pl.mag();
  }
  else {
     A_trans  = OldPolarization;
     // Here we Follow Jackson's conventions and we set the
     // parallel component = 1 in case of a ray perpendicular
     // to the surface
     E1_perp  = 0.0;
     E1_parl  = 1.0;
  }

  //calculate incident angle
  G4double incidentangle = GetIncidentAngle();

  //calculate the reflectivity depending on incident angle,
  //polarization and complex refractive

  theReflectivity =
             GetReflectivity(E1_perp, E1_parl, incidentangle,
                                                 RealRindex, ImaginaryRindex);
}

G4bool WCSimOpBoundaryProcess::InvokeSD(const G4Step* pStep)
{
  G4Step aStep = *pStep;

  aStep.AddTotalEnergyDeposit(thePhotonMomentum);

  G4VSensitiveDetector* sd = aStep.GetPostStepPoint()->GetSensitiveDetector();
  if (sd) return sd->Hit(&aStep);
  else return false;
}

void WCSimOpBoundaryProcess::CoatedDielectricDielectric_Model1()
{
  G4MaterialPropertyVector* pp = nullptr;

  G4MaterialPropertiesTable* MPT = Material2->GetMaterialPropertiesTable();
  if((pp = MPT->GetProperty("RINDEX")))
  {
    Rindex2 = pp->Value(thePhotonMomentum);
  }

  MPT = OpticalSurface->GetMaterialPropertiesTable();
  // thin film properties to be defined in WCSimConstructMaterials.cc
  if((pp = MPT->GetProperty("COATEDRINDEX")))
  {
    fCoatedRindex = pp->Value(thePhotonMomentum);
  }
  if(MPT->ConstPropertyExists("COATEDTHICKNESS"))
  {
    fCoatedThickness = MPT->GetConstProperty("COATEDTHICKNESS");
  }
  // allows frustrated transmission through the film or not
  if(MPT->ConstPropertyExists("COATEDFRUSTRATEDTRANSMISSION"))
  {
    fCoatedFrustratedTransmission =
      (G4bool)MPT->GetConstProperty("COATEDFRUSTRATEDTRANSMISSION");
  }

  G4double sintTL;
  G4double wavelength = h_Planck * c_light / thePhotonMomentum;
  G4double PdotN;
  G4double E1_perp, E1_parl;
  G4double s1, E2_perp, E2_parl, E2_total, transCoeff;
  G4double E2_abs, C_parl, C_perp;
  G4double alpha;
  G4ThreeVector A_trans, A_paral, E1pp, E1pl;
  //G4bool Inside  = false;
  //G4bool Swap    = false;
  G4bool through = false;
  G4bool done    = false;

  do {
    if (through)
    {
      //Swap = !Swap;
      through = false;
      theGlobalNormal = -theGlobalNormal;
      G4SwapPtr(Material1, Material2);
      G4SwapObj(&Rindex1, &Rindex2);
    }

    if(theFinish == polished)
    {
      theFacetNormal = theGlobalNormal;
    }
    else
    {
      theFacetNormal = GetFacetNormal(OldMomentum, theGlobalNormal);
    }

    PdotN = OldMomentum * theFacetNormal;
    cost1 = -PdotN;
    sint2 = 0; cost2 = 0.;

    if (std::abs(cost1) < 1.0 - kCarTolerance)
    {
      sint1 = std::sqrt(1. - cost1 * cost1);
      sint2 = sint1 * Rindex1 / Rindex2;
      sintTL = sint1 * Rindex1 / fCoatedRindex;
    } else
    {
      sint1 = 0.0;
      sint2 = 0.0;
      sintTL = 0.0;
    }

    if (sint1 > 0.0)
    {
      A_trans = OldMomentum.cross(theFacetNormal);
      A_trans = A_trans.unit();
      E1_perp = OldPolarization * A_trans;
      E1pp = E1_perp * A_trans;
      E1pl = OldPolarization - E1pp;
      E1_parl = E1pl.mag();
    }
    else
    {
      A_trans = OldPolarization;
      E1_perp = 0.0;
      E1_parl = 1.0;
    }

    s1 = Rindex1 * cost1;

    if (cost1 > 0.0)
    {
      cost2 = std::sqrt(1. - sint2 * sint2);
    }
    else
    {
      cost2 = -std::sqrt(1. - sint2 * sint2);
    }

    transCoeff = 0.0;

    if (sintTL >= 1.0)
    { // --> Angle > Angle Limit
      //Swap = false;
    }
    E2_perp = 2. * s1 * E1_perp / (Rindex1 * cost1 + Rindex2 * cost2);
    E2_parl = 2. * s1 * E1_parl / (Rindex2 * cost1 + Rindex1 * cost2);
    E2_total = E2_perp * E2_perp + E2_parl * E2_parl;

    // Equations in Table 1 of https://ieeexplore.ieee.org/document/9875513
    transCoeff = 1. - GetReflectivityThroughThinLayer(
                        sintTL, E1_perp, E1_parl, wavelength, cost1, cost2);
    if (!G4BooleanRand(transCoeff))
    {
      if(verboseLevel > 2)
        G4cout << "Reflection from " << Material1->GetName() << " to "
               << Material2->GetName() << G4endl;

      //Swap = false;

      if (sintTL >= 1.0)
      {
        theStatus = TotalInternalReflection;
      }
      else
      {
        theStatus = CoatedDielectricReflection;
      }

      PdotN = OldMomentum * theFacetNormal;
      NewMomentum = OldMomentum - (2. * PdotN) * theFacetNormal;

      if (sint1 > 0.0) {   // incident ray oblique

        E2_parl = Rindex2 * E2_parl / Rindex1 - E1_parl;
        E2_perp = E2_perp - E1_perp;
        E2_total = E2_perp * E2_perp + E2_parl * E2_parl;
        A_paral = NewMomentum.cross(A_trans);
        A_paral = A_paral.unit();
        E2_abs = std::sqrt(E2_total);
        C_parl = E2_parl / E2_abs;
        C_perp = E2_perp / E2_abs;

        NewPolarization = C_parl * A_paral + C_perp * A_trans;

        // ad-hoc handle of total internal reflection
        if (sint2*sint2>1.)
          NewPolarization = -OldPolarization + (2.*OldPolarization * theFacetNormal)*theFacetNormal;
      }
      else
      {               // incident ray perpendicular
        if (Rindex2 > Rindex1)
        {
          NewPolarization = -OldPolarization;
        }
        else
        {
          NewPolarization = OldPolarization;
        }
      }

    } else { // photon gets transmitted
      if (verboseLevel > 2)
        G4cout << "Transmission from " << Material1->GetName() << " to "
               << Material2->GetName() << G4endl;

      //Inside = !Inside;
      through = true;

      // Need to define non-zero efficiency to trigger absorption process
      if (theEfficiency > 0. || true) // just force to do absorption for now
      {
        DoAbsorption();
        return;
      }
      else
      {
        if (sintTL >= 1.0)
        {
          theStatus = CoatedDielectricFrustratedTransmission;
        }
        else
        {
          theStatus = CoatedDielectricRefraction;
        }

        if (sint1 > 0.0) {      // incident ray oblique

          alpha = cost1 - cost2 * (Rindex2 / Rindex1);
          NewMomentum = OldMomentum + alpha * theFacetNormal;
          NewMomentum = NewMomentum.unit();
          A_paral = NewMomentum.cross(A_trans);
          A_paral = A_paral.unit();
          E2_abs = std::sqrt(E2_total);
          C_parl = E2_parl / E2_abs;
          C_perp = E2_perp / E2_abs;

          NewPolarization = C_parl * A_paral + C_perp * A_trans;

        }
        else
        {                  // incident ray perpendicular
          NewMomentum = OldMomentum;
          NewPolarization = OldPolarization;
        }
      }
    }

    OldMomentum = NewMomentum.unit();
    OldPolarization = NewPolarization.unit();
    if ((theStatus == CoatedDielectricFrustratedTransmission) ||
        (theStatus == CoatedDielectricRefraction))
    {
      done = (NewMomentum * theGlobalNormal <= 0.0);
    }
    else
    {
      done = (NewMomentum * theGlobalNormal >= -kCarTolerance);
    }

  } while (!done);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4double WCSimOpBoundaryProcess::GetReflectivityThroughThinLayer(G4double sinTL,
                   G4double E1_perp,
                   G4double E1_parl,
                   G4double wavelength, G4double costh1, G4double costh2) {
  G4complex Reflectivity, Reflectivity_TE, Reflectivity_TM;
  G4double gammaTL, costTL;

  G4complex i(0, 1);
  G4complex rTM, rTE;
  G4complex r1toTL, rTLto2;
  G4double k0 = 2 * pi / wavelength;

  // Angle > Angle limit
  if (sinTL >= 1.0) {
    if (fCoatedFrustratedTransmission) { //Frustrated transmission

      if (costh1 > 0.0)
      {
        gammaTL = std::sqrt(Rindex1 * Rindex1 * sint1 * sint1 -
                   fCoatedRindex * fCoatedRindex);
      }
      else
      {
        gammaTL = -std::sqrt(Rindex1 * Rindex1 * sint1 * sint1 -
                   fCoatedRindex * fCoatedRindex);
      }

      // TE
      r1toTL = (Rindex1 * costh1 - i * gammaTL) / (Rindex1 * costh1 + i * gammaTL);
      rTLto2 = (i * gammaTL - Rindex2 * costh2) / (i * gammaTL + Rindex2 * costh2);
      if (costh1 != 0.0)
      {
        rTE = (r1toTL + rTLto2 * std::exp(-2 * k0 * fCoatedThickness * gammaTL)) /
                 (1.0 + r1toTL * rTLto2 * std::exp(-2 * k0 * fCoatedThickness * gammaTL));
      }
      // TM
      r1toTL = (Rindex1 * i * gammaTL - fCoatedRindex * fCoatedRindex * costh1) /
                  (Rindex1 * i * gammaTL + fCoatedRindex * fCoatedRindex * costh1);
      rTLto2 = (fCoatedRindex * fCoatedRindex * costh2 - Rindex2 * i * gammaTL) /
                  (fCoatedRindex * fCoatedRindex * costh2 + Rindex2 * i * gammaTL);
      if (costh1 != 0.0)
      {
        rTM = (r1toTL + rTLto2 * std::exp(-2 * k0 * fCoatedThickness * gammaTL)) /
                 (1.0 + r1toTL * rTLto2 * std::exp(-2 * k0 * fCoatedThickness * gammaTL));
      }
    }
    else
    { //Total reflection
      return(1.);
    }
  }

  // Angle <= Angle limit
  else //if (sinTL < 1.0)
  {
    if (costh1 > 0.0)
    {
      costTL = std::sqrt(1. - sinTL * sinTL);
    }
    else
    {
      costTL = -std::sqrt(1. - sinTL * sinTL);
    }
    // TE
    r1toTL = (Rindex1 * costh1 - fCoatedRindex * costTL) / (Rindex1 * costh1 + fCoatedRindex * costTL);
    rTLto2 = (fCoatedRindex * costTL - Rindex2 * costh2) / (fCoatedRindex * costTL + Rindex2 * costh2);
    if (costh1 != 0.0)
    {
      rTE = (r1toTL + rTLto2 * std::exp(2.0 * i * k0 * fCoatedRindex * fCoatedThickness * costTL)) /
            (1.0 + r1toTL * rTLto2 * std::exp(2.0 * i * k0 * fCoatedRindex * fCoatedThickness * costTL));
    }
    // TM
    r1toTL = (Rindex1 * costTL - fCoatedRindex * costh1) / (Rindex1 * costTL + fCoatedRindex * costh1);
    rTLto2 = (fCoatedRindex * costh2 - Rindex2 * costTL) / (fCoatedRindex * costh2 + Rindex2 * costTL);
    if (costh1 != 0.0)
    {
      rTM = (r1toTL + rTLto2 * std::exp(2.0 * i * k0 * fCoatedRindex * fCoatedThickness * costTL)) /
            (1.0 + r1toTL * rTLto2 * std::exp(2.0 * i * k0 * fCoatedRindex * fCoatedThickness * costTL));
    }
  }

  Reflectivity_TE = (rTE * conj(rTE)) * (E1_perp * E1_perp) / (E1_perp * E1_perp + E1_parl * E1_parl);
  Reflectivity_TM = (rTM * conj(rTM)) * (E1_parl * E1_parl) / (E1_perp * E1_perp + E1_parl * E1_parl);
  Reflectivity = Reflectivity_TE + Reflectivity_TM;

  return real(Reflectivity);
}

void WCSimOpBoundaryProcess::CoatedDielectricDielectric_Model2()
{
  // Model taken from https://arxiv.org/abs/physics/0408075v1
  // Some bugs are found in the paper and corrected in this implementation

  G4MaterialPropertyVector* pp = nullptr;

  G4MaterialPropertiesTable* MPT = Material2->GetMaterialPropertiesTable();
  if((pp = MPT->GetProperty("RINDEX")))
  {
    Rindex2 = pp->Value(thePhotonMomentum);
  }

  MPT = OpticalSurface->GetMaterialPropertiesTable();
  // thin film properties to be defined in WCSimConstructMaterials.cc
  // need both real and imaginary refractive indices
  if((pp = MPT->GetProperty("COATEDRINDEX")))
  {
    fCoatedRindex = pp->Value(thePhotonMomentum);
  }
  if((pp = MPT->GetProperty("COATEDRINDEXIM")))
  {
    fCoatedRindexIm = pp->Value(thePhotonMomentum);
  }
  if(MPT->ConstPropertyExists("COATEDTHICKNESS"))
  {
    fCoatedThickness = MPT->GetConstProperty("COATEDTHICKNESS");
  }

  G4double sintTL;
  G4double wavelength = h_Planck * c_light / thePhotonMomentum;
  G4double PdotN;
  G4double E1_perp, E1_parl;
  G4double E2_perp, E2_parl, E2_total;
  G4double E2_abs, C_parl, C_perp;
  G4double alpha;
  G4ThreeVector A_trans, A_paral, E1pp, E1pl;
  //G4bool Inside  = false;
  //G4bool Swap    = false;
  G4bool through = false;
  G4bool done    = false;

  G4complex i(0, 1);
  G4complex r23p, r34p, t23p, t32p, t34p;
  G4complex r23s, r34s, t23s, t32s, t34s;
  G4complex rp, tp, rs, ts;
  G4complex delta, costh2, costh3, costh4, sinth3, sinth4, n2, n3, n4;
  n2 = Rindex1;
  n3 = fCoatedRindex-i*fCoatedRindexIm;
  n4 = Rindex2;
  G4double Rp, Tp, Rs, Ts, Rtot, Ttot;

  do {
    if (through)
    {
      //Swap = !Swap;
      through = false;
      theGlobalNormal = -theGlobalNormal;
      G4SwapPtr(Material1, Material2);
      G4SwapObj(&Rindex1, &Rindex2);
    }

    if(theFinish == polished)
    {
      theFacetNormal = theGlobalNormal;
    }
    else
    {
      theFacetNormal = GetFacetNormal(OldMomentum, theGlobalNormal);
    }

    PdotN = OldMomentum * theFacetNormal;
    cost1 = -PdotN;
    sint2 = 0; cost2 = 0.;

    if (std::abs(cost1) < 1.0 - kCarTolerance)
    {
      sint1 = std::sqrt(1. - cost1 * cost1);
      sint2 = sint1 * Rindex1 / Rindex2;
      sintTL = sint1 * Rindex1 / fCoatedRindex;
    } else
    {
      sint1 = 0.0;
      sint2 = 0.0;
      sintTL = 0.0;
    }

    if (sint1 > 0.0)
    {
      A_trans = OldMomentum.cross(theFacetNormal);
      A_trans = A_trans.unit();
      E1_perp = OldPolarization * A_trans;
      E1pp = E1_perp * A_trans;
      E1pl = OldPolarization - E1pp;
      E1_parl = E1pl.mag();
    }
    else
    {
      A_trans = OldPolarization;
      E1_perp = 0.0;
      E1_parl = 1.0;
    }

    // Calculate reflection and transmission probability from Eq. 2 of https://arxiv.org/abs/physics/0408075v1
    // Rename the variables to match those in paper
    costh2 = cost1;
    sinth3 = Rindex1*sint1/n3;
    sinth4 = Rindex1*sint1/Rindex2;
    costh3 = sqrt(1.-sinth3*sinth3);
    if (cost1<0) costh3 *= -1;
    costh4 = sqrt(1.-sinth4*sinth4);
    if (cost1<0) costh4 *= -1;

    // reflection and transmission coefficients
    // TM =  p wave
    r23p = (n2*costh3-n3*costh2)/(n2*costh3+n3*costh2);
    t23p = 2.*n2*costh2/(n2*costh3+n3*costh2);
    t32p = 2.*n3*costh3/(n2*costh3+n3*costh2);
    r34p = (n3*costh4-n4*costh3)/(n3*costh4+n4*costh3);
    t34p = 2.*n3*costh3/(n4*costh3+n3*costh4);

    // TE = s wave
    r23s = (n2*costh2-n3*costh3)/(n2*costh2+n3*costh3);
    t23s = 2.*n2*costh2/(n2*costh2+n3*costh3);
    t32s = 2.*n3*costh3/(n2*costh2+n3*costh3);
    r34s = (n3*costh3-n4*costh4)/(n3*costh3+n4*costh4);
    t34s = 2.*n3*costh3/(n3*costh3+n4*costh4);

    delta = -2.*pi*fCoatedThickness*n3/wavelength*costh3;

    rp = r23p+(t23p*t32p*r34p*exp(2.*i*delta))/(1.+r23p*r34p*exp(2.*i*delta));
    tp = t23p*t34p*exp(i*delta)/(1.+r23p*r34p*exp(2.*i*delta));
    Rp = real(rp*conj(rp));
    Tp = real(n4*costh4/n2/costh2*tp*conj(tp));

    rs = r23s+(t23s*t32s*r34s*exp(2.*i*delta))/(1.+r23s*r34s*exp(2.*i*delta));
    ts = t23s*t34s*exp(i*delta)/(1.+r23s*r34s*exp(2.*i*delta));
    Rs = real(rs*conj(rs));
    Ts = real(n4*costh4/n2/costh2*ts*conj(ts));

    Rtot = (Rs * E1_perp * E1_perp + Rp * E1_parl * E1_parl)  / (E1_perp * E1_perp + E1_parl * E1_parl);
    Ttot = (Ts * E1_perp * E1_perp + Tp * E1_parl * E1_parl)  / (E1_perp * E1_perp + E1_parl * E1_parl);

    G4double rdm = G4UniformRand();

    if ( rdm < Rtot )
    {
      if(verboseLevel > 2)
        G4cout << "Reflection from " << Material1->GetName() << " to "
               << Material2->GetName() << G4endl;

      //Swap = false;

      if (sintTL >= 1.0 || sint2 >= 1.0)
      {
        theStatus = TotalInternalReflection;
      }
      else
      {
        theStatus = CoatedDielectricReflection;
      }

      PdotN = OldMomentum * theFacetNormal;
      NewMomentum = OldMomentum - (2. * PdotN) * theFacetNormal;

      if (sint1 > 0.0) {   // incident ray oblique

        // not sure about +/- sign, also ignore the phase change
        E2_parl = -sqrt(Rp)*E1_parl;
        if (std::arg(rp)<-pi/2 || std::arg(rp)>pi/2) E2_parl *= -1;
        E2_perp = sqrt(Rs)*E1_perp;
        if (std::arg(rs)<-pi/2 || std::arg(rs)>pi/2) E2_perp *= -1;

        E2_total = E2_perp * E2_perp + E2_parl * E2_parl;
        A_paral = NewMomentum.cross(A_trans);
        A_paral = A_paral.unit();
        E2_abs = std::sqrt(E2_total);
        C_parl = E2_parl / E2_abs;
        C_perp = E2_perp / E2_abs;

        NewPolarization = C_parl * A_paral + C_perp * A_trans;

      }
      else
      {               // incident ray perpendicular
        if (Rindex2 > Rindex1)
        {
          NewPolarization = -OldPolarization;
        }
        else
        {
          NewPolarization = OldPolarization;
        }
      }

    } else if ( rdm < Rtot + Ttot ) { // photon gets transmitted
      if (verboseLevel > 2)
        G4cout << "Transmission from " << Material1->GetName() << " to "
               << Material2->GetName() << G4endl;

      //Inside = !Inside;
      through = true;

      if (sintTL >= 1.0)
      {
         theStatus = CoatedDielectricFrustratedTransmission;
      }
      else
      {
         theStatus = CoatedDielectricRefraction;
      }

      if (sint1 > 0.0) {      // incident ray oblique

         cost2 = std::sqrt(1. - sint2 * sint2);
         if ( cost1 < 0.0 ) cost2 *= -1;

         alpha = cost1 - cost2 * (Rindex2 / Rindex1);
         NewMomentum = OldMomentum + alpha * theFacetNormal;
         NewMomentum = NewMomentum.unit();
         A_paral = NewMomentum.cross(A_trans);
         A_paral = A_paral.unit();
         // again the sign may be wrong
         E2_parl = sqrt(Tp)*E1_parl;
         E2_perp = sqrt(Ts)*E1_perp;
         if (std::arg(tp)<-pi/2 || std::arg(tp)>pi/2) E2_parl *= -1;
         if (std::arg(ts)<-pi/2 || std::arg(ts)>pi/2) E2_perp *= -1;
         E2_total = E2_perp * E2_perp + E2_parl * E2_parl;
         E2_abs = std::sqrt(E2_total);
         C_parl = E2_parl / E2_abs;
         C_perp = E2_perp / E2_abs;

         NewPolarization = C_parl * A_paral + C_perp * A_trans;

      }
      else
      {                  // incident ray perpendicular
         NewMomentum = OldMomentum;
         NewPolarization = OldPolarization;
      }
      
    }
    else {
      DoAbsorption();
      return;
    }

    OldMomentum = NewMomentum.unit();
    OldPolarization = NewPolarization.unit();
    if ((theStatus == CoatedDielectricFrustratedTransmission) ||
        (theStatus == CoatedDielectricRefraction))
    {
      done = (NewMomentum * theGlobalNormal <= 0.0);
    }
    else
    {
      done = (NewMomentum * theGlobalNormal >= -kCarTolerance);
    }

  } while (!done);
}