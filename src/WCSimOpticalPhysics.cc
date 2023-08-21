// Modified from G4OpticalPhysics.cc of geant4.10 to implement custom optical physics
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
//---------------------------------------------------------------------------
//
// ClassName:   G4OpticalPhysics
//
// Author:      P.Gumplinger 30.09.2009
//
// Modified:    P.Gumplinger 29.09.2011
//              (based on code from I. Hrivnacova)
//
//----------------------------------------------------------------------------
//

#include "WCSimOpticalPhysics.hh"

//#include "G4OpticalPhoton.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpMieHG.hh"
#include "WCSimOpBoundaryProcess.hh"
#include "G4OpWLS.hh"
#include "G4Scintillation.hh"
#include "G4Cerenkov.hh"

#include "G4LossTableManager.hh"
#include "G4EmSaturation.hh"

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

#include "G4Version.hh"

// factory
#include "G4PhysicsConstructorFactory.hh"
G4_DECLARE_PHYSCONSTR_FACTORY(WCSimOpticalPhysics);

G4ThreadLocal G4Scintillation*     WCSimOpticalPhysics::fScintillationProcess = nullptr;
G4ThreadLocal G4Cerenkov*          WCSimOpticalPhysics::fCerenkovProcess = nullptr;
G4ThreadLocal G4OpWLS*             WCSimOpticalPhysics::fWLSProcess = nullptr;
G4ThreadLocal G4OpAbsorption*      WCSimOpticalPhysics::fAbsorptionProcess = nullptr;
G4ThreadLocal G4OpRayleigh*        WCSimOpticalPhysics::fRayleighProcess = nullptr;
G4ThreadLocal G4OpMieHG*           WCSimOpticalPhysics::fMieProcess = nullptr;
G4ThreadLocal WCSimOpBoundaryProcess* WCSimOpticalPhysics::fBoundaryProcess = nullptr; // custom boundary process

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WCSimOpticalPhysics::WCSimOpticalPhysics(G4int verbose, const G4String& name)
  : G4VPhysicsConstructor(name),

    fYieldFactor(1.),
    fExcitationRatio(0.0),
    fFiniteRiseTime(false),
    fScintillationByParticleType(false),
    fScintillationTrackInfo(false),
    fScintillationStackPhotons(true),
    fScintillationVerbosity(0),
    fMaxNumPhotons(100),
    fMaxBetaChange(10.0),
    fCerenkovStackPhotons(true),
    fCerenkovVerbosity(0),
    fWLSTimeProfileName("delta"),
    fWLSVerbosity(0),
    fAbsorptionVerbosity(0),
    fRayleighVerbosity(0),
    fMieVerbosity(0),
    fInvokeSD(true),
    fBoundaryVerbosity(0)

{
  verboseLevel = verbose;
  fMessenger = new WCSimOpticalPhysicsMessenger(this);

  for ( G4int i=0; i<kNoProcess; i++ ) {
    fProcessUse.push_back(true);
    fProcessTrackSecondariesFirst.push_back(true);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WCSimOpticalPhysics::~WCSimOpticalPhysics()
{
  delete fMessenger;
  fMessenger = nullptr;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCSimOpticalPhysics::PrintStatistics() const
{
// Print all processes activation and their parameters

  for ( G4int i=0; i<kNoProcess; i++ ) {
    G4cout << "  " << G4OpticalProcessName(i) << " process:  ";
    if ( ! fProcessUse[i] ) {
      G4cout << "not used" << G4endl;
    }
    else {
      G4cout << "used" << G4endl;
      if ( i == kCerenkov ) {
        G4cout << "    Max number of photons per step: " << fMaxNumPhotons << G4endl;
        G4cout << "    Max beta change per step:       " << fMaxBetaChange << G4endl;
        if ( fProcessTrackSecondariesFirst[kCerenkov] ) {
          G4cout << "    Track secondaries first:  activated" << G4endl;
        }
        else {
          G4cout << "    Track secondaries first:  inactivated" << G4endl;
        }
      }
      if ( i == kScintillation ) {
        if (fScintillationByParticleType)
        G4cout << "    Scintillation by Particle Type:  activated " << G4endl;
        G4cout << "    Yield factor: "  << fYieldFactor << G4endl;
        G4cout << "    ExcitationRatio: " << fExcitationRatio << G4endl;
        if ( fProcessTrackSecondariesFirst[kScintillation] ) {
          G4cout << "    Track secondaries first:  activated" << G4endl;
        }
        else {
          G4cout << "    Track secondaries first:  inactivated" << G4endl;
        }
      }
      if ( i == kWLS ) {
        G4cout << "     WLS process time profile: " << fWLSTimeProfileName << G4endl;
      }
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCSimOpticalPhysics::ConstructParticle()
{
  G4OpticalPhoton::OpticalPhotonDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCSimOpticalPhysics::ConstructProcess()
{
  if(verboseLevel>0)
         G4cout <<"WCSimOpticalPhysics:: Add Optical Physics Processes"<< G4endl;

  // A vector of optical processes
  std::vector<G4VProcess*> OpProcesses;

  for ( G4int i=0; i<kNoProcess; i++ ) OpProcesses.push_back(nullptr);

  // Add Optical Processes

  fAbsorptionProcess  = new G4OpAbsorption();
  OpProcesses[kAbsorption] = fAbsorptionProcess;

  fRayleighProcess = new G4OpRayleigh();
  OpProcesses[kRayleigh] = fRayleighProcess;

  fMieProcess = new G4OpMieHG();
  OpProcesses[kMieHG] = fMieProcess;

  // custom boundary process
  fBoundaryProcess = new WCSimOpBoundaryProcess();
  fBoundaryProcess->SetInvokeSD(fInvokeSD);
  OpProcesses[kBoundary] = fBoundaryProcess;

  fWLSProcess = new G4OpWLS();
  fWLSProcess->UseTimeProfile(fWLSTimeProfileName);
  OpProcesses[kWLS] = fWLSProcess;

  G4ProcessManager* pManager = nullptr;
  pManager = G4OpticalPhoton::OpticalPhoton()->GetProcessManager();

  if (!pManager) {
     G4ExceptionDescription ed;
     ed << "Optical Photon without a Process Manager";
     G4Exception("WCSimOpticalPhysics::ConstructProcess()","",
                  FatalException,ed);
     return;
  }

  for ( G4int i=kAbsorption; i<=kWLS; i++ ) {
      if ( fProcessUse[i] ) {
         pManager->AddDiscreteProcess(OpProcesses[i]);
      }
  }

  fScintillationProcess = new G4Scintillation();
  fScintillationProcess->SetScintillationYieldFactor(fYieldFactor);
  fScintillationProcess->SetScintillationExcitationRatio(fExcitationRatio);
  fScintillationProcess->SetFiniteRiseTime(fFiniteRiseTime);
  fScintillationProcess->SetScintillationByParticleType(fScintillationByParticleType);
#if G4VERSION_NUMBER > 1023
  fScintillationProcess->SetScintillationTrackInfo(fScintillationTrackInfo);
#endif
  fScintillationProcess->SetTrackSecondariesFirst(fProcessTrackSecondariesFirst[kScintillation]);
#if G4VERSION_NUMBER > 1030
  fScintillationProcess->SetStackPhotons(fScintillationStackPhotons);
#endif
  G4EmSaturation* emSaturation = G4LossTableManager::Instance()->EmSaturation();
  fScintillationProcess->AddSaturation(emSaturation);
  OpProcesses[kScintillation] = fScintillationProcess;

  fCerenkovProcess = new G4Cerenkov();
  fCerenkovProcess->SetMaxNumPhotonsPerStep(fMaxNumPhotons);
  fCerenkovProcess->SetMaxBetaChangePerStep(fMaxBetaChange);
  fCerenkovProcess->SetTrackSecondariesFirst(fProcessTrackSecondariesFirst[kCerenkov]);
#if G4VERSION_NUMBER > 1030
  fCerenkovProcess->SetStackPhotons(fCerenkovStackPhotons);
#endif
  OpProcesses[kCerenkov] = fCerenkovProcess;

#if G4VERSION_NUMBER > 1022
  auto myParticleIterator=GetParticleIterator();
#else
  auto myParticleIterator=aParticleIterator;
#endif
  myParticleIterator->reset();

  while( (*myParticleIterator)() ){

    G4ParticleDefinition* particle = myParticleIterator->value();
    G4String particleName = particle->GetParticleName();

    pManager = particle->GetProcessManager();
    if (!pManager) {
       G4ExceptionDescription ed;
       ed << "Particle " << particleName << "without a Process Manager";
       G4Exception("WCSimOpticalPhysics::ConstructProcess()","",
                    FatalException, ed);
       return;                 // else coverity complains for pManager use below
    }

    if( fCerenkovProcess->IsApplicable(*particle) &&
        fProcessUse[kCerenkov] ) {
          pManager->AddProcess(fCerenkovProcess);
          pManager->SetProcessOrdering(fCerenkovProcess,idxPostStep);
    }
    if( fScintillationProcess->IsApplicable(*particle) &&
        fProcessUse[kScintillation] ) {
          pManager->AddProcess(fScintillationProcess);
          pManager->SetProcessOrderingToLast(fScintillationProcess,idxAtRest);
          pManager->SetProcessOrderingToLast(fScintillationProcess,idxPostStep);
    }
  }

  // Add verbose
  for ( G4int i=0; i<kNoProcess; i++ ) {
    if ( fProcessUse[i] ) OpProcesses[i]->SetVerboseLevel(verboseLevel);
  }

    if (verboseLevel > 1) PrintStatistics();
  if (verboseLevel > 0)
    G4cout << "### " << namePhysics << " physics constructed." << G4endl;
}

void WCSimOpticalPhysics::SetScintillationYieldFactor(G4double val)
{
   fYieldFactor = val;
   if (fScintillationProcess) {
    fScintillationProcess->SetScintillationYieldFactor(fYieldFactor);
  }
}

void WCSimOpticalPhysics::SetScintillationExcitationRatio(G4double val)
{
  fExcitationRatio = val;
  if (fScintillationProcess) {
    fScintillationProcess->SetScintillationExcitationRatio(fExcitationRatio);
  }
}

void WCSimOpticalPhysics::SetMaxNumPhotonsPerStep(G4int val)
{
/// Limit step to the specified maximum number of Cerenkov photons
  fMaxNumPhotons = val;
  if (fCerenkovProcess) {
    fCerenkovProcess->SetMaxNumPhotonsPerStep(fMaxNumPhotons);
  }
}

void WCSimOpticalPhysics::SetMaxBetaChangePerStep(G4double val)
{
/// Limit step to the specified maximum change of beta of the parent particle
  fMaxBetaChange = val;
  if (fCerenkovProcess) {
    fCerenkovProcess->SetMaxBetaChangePerStep(fMaxBetaChange);
  }
}

void WCSimOpticalPhysics::SetCerenkovStackPhotons(G4bool val)
{
  fCerenkovStackPhotons = val;
  if (fCerenkovProcess) {
#if G4VERSION_NUMBER > 1030
    fCerenkovProcess->SetStackPhotons(fCerenkovStackPhotons);
#endif
  }
}

void WCSimOpticalPhysics::SetCerenkovTrackSecondariesFirst(G4bool val)
{
  fProcessTrackSecondariesFirst[kCerenkov] = val;
  if (fCerenkovProcess) {
    fCerenkovProcess->
      SetTrackSecondariesFirst(fProcessTrackSecondariesFirst[kCerenkov]);
  }
}

void WCSimOpticalPhysics::SetCerenkovVerbosity(G4int ver)
{
  fCerenkovVerbosity = ver;
  if (fCerenkovProcess) {
    fCerenkovProcess->SetVerboseLevel(fCerenkovVerbosity);
  }
}

void WCSimOpticalPhysics::SetWLSTimeProfile(G4String name)
{
/// Set the WLS time profile (delta or exponential)
  fWLSTimeProfileName = name;
  if (fWLSProcess) {
    fWLSProcess->UseTimeProfile(fWLSTimeProfileName);
  }
}

void WCSimOpticalPhysics::SetWLSVerbosity(G4int ver)
{
  fWLSVerbosity = ver;
  if (fWLSProcess) {
    fWLSProcess->SetVerboseLevel(fWLSVerbosity);
  }
}

void WCSimOpticalPhysics::SetScintillationByParticleType(G4bool val)
{
  fScintillationByParticleType = val;
  if (fScintillationProcess) {
    fScintillationProcess->
      SetScintillationByParticleType(fScintillationByParticleType);
  }
}

void WCSimOpticalPhysics::SetScintillationTrackSecondariesFirst(G4bool val)
{
  fProcessTrackSecondariesFirst[kScintillation] = val;
  if (fScintillationProcess) {
    fScintillationProcess->
      SetTrackSecondariesFirst(fProcessTrackSecondariesFirst[kScintillation]);
  }
}

void WCSimOpticalPhysics::SetScintillationTrackInfo(G4bool val)
{
  fScintillationTrackInfo = val;
  if (fScintillationProcess) {
#if G4VERSION_NUMBER > 1023
    fScintillationProcess->SetScintillationTrackInfo(fScintillationTrackInfo);
#endif
  }
}

void WCSimOpticalPhysics::SetScintillationVerbosity(G4int ver)
{
  fScintillationVerbosity = ver;
  if (fScintillationProcess) {
    fScintillationProcess->SetVerboseLevel(fScintillationVerbosity);
  }
}

void WCSimOpticalPhysics::SetAbsorptionVerbosity(G4int ver)
{
  fAbsorptionVerbosity = ver;
  if (fAbsorptionProcess) {
    fAbsorptionProcess->SetVerboseLevel(fAbsorptionVerbosity);
  }
}

void WCSimOpticalPhysics::SetRayleighVerbosity(G4int ver)
{
  fRayleighVerbosity = ver;
  if (fRayleighProcess) {
    fRayleighProcess->SetVerboseLevel(fRayleighVerbosity);
  }
}

void WCSimOpticalPhysics::SetMieVerbosity(G4int ver)
{
  fMieVerbosity = ver;
  if (fMieProcess) {
    fMieProcess->SetVerboseLevel(fMieVerbosity);
  }
}

void WCSimOpticalPhysics::SetBoundaryVerbosity(G4int ver)
{
  fBoundaryVerbosity = ver;
  if (fBoundaryProcess) {
    fBoundaryProcess->SetVerboseLevel(fBoundaryVerbosity);
  }
}

void WCSimOpticalPhysics::SetTrackSecondariesFirst(G4OpticalProcessIndex index,
                                                G4bool trackSecondariesFirst)
{
  if ( index >= kNoProcess ) return;
  if ( fProcessTrackSecondariesFirst[index] == trackSecondariesFirst ) return;
  fProcessTrackSecondariesFirst[index] = trackSecondariesFirst;
}

void WCSimOpticalPhysics::SetFiniteRiseTime(G4bool b)
{
  fFiniteRiseTime = b;
  if (fScintillationProcess) {
    fScintillationProcess->SetFiniteRiseTime(fFiniteRiseTime);
  }
}

void WCSimOpticalPhysics::SetInvokeSD(G4bool b)
{
  fInvokeSD = b;
  if (fBoundaryProcess) {
    fBoundaryProcess->SetInvokeSD(fInvokeSD);
  }
}

void WCSimOpticalPhysics::SetScintillationStackPhotons(G4bool stackingFlag)
{
  fScintillationStackPhotons = stackingFlag;
  if (fScintillationProcess) {
#if G4VERSION_NUMBER > 1030
    fScintillationProcess->SetStackPhotons(fScintillationStackPhotons);
#endif
  }
}

void WCSimOpticalPhysics::Configure(G4OpticalProcessIndex index, G4bool isUse)
{
  // Configure the physics constructor to use/not use a selected process.
  // This method can only be called in PreInit> phase (before execution of
  // ConstructProcess). The process is not added to particle's process manager
  // and so it cannot be re-activated later in Idle> phase with the command
  // /process/activate.

  if ( index >= kNoProcess ) return;
  if ( fProcessUse[index] == isUse ) return;
  fProcessUse[index] = isUse;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
