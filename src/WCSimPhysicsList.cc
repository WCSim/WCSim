// Physics List

#include "WCSimPhysicsList.hh"
#include "WCSimPhysicsMessenger.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleWithCuts.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4Material.hh"
#include "G4ios.hh"

#include "G4UImanager.hh"
#include "globals.hh"
#include <iomanip>

#include "G4HadronCaptureProcess.hh" //amb79


//WCSimPhysicsList::WCSimPhysicsList():  G4VUserPhysicsList(), PhysicsMessenger(0)
WCSimPhysicsList::WCSimPhysicsList():  G4VPhysicsConstructor(), PhysicsMessenger(0)
{
 //defaultCutValue = 1.0*mm; //moved to WCSimPhysicsListFactory.cc
 SetVerboseLevel(1);

 PhysicsMessenger = new WCSimPhysicsMessenger(this);

}

WCSimPhysicsList::~WCSimPhysicsList()
{
  delete PhysicsMessenger;
  PhysicsMessenger = 0;
}

//----particle construction----

void WCSimPhysicsList::ConstructParticle()
{
  G4LeptonConstructor leptonConstructor;
  G4MesonConstructor  mesonConstructor;
  G4BaryonConstructor baryonConstructor;
  G4BosonConstructor bosonConstructor;
  G4IonConstructor ionConstructor;
  G4ShortLivedConstructor ShortLivedConstructor;
  leptonConstructor.ConstructParticle();
  mesonConstructor.ConstructParticle();
  baryonConstructor.ConstructParticle();
  bosonConstructor.ConstructParticle();
  ionConstructor.ConstructParticle();
  ShortLivedConstructor.ConstructParticle();
  G4OpticalPhoton::OpticalPhotonDefinition();
}

//----construction of processes----

void WCSimPhysicsList::ConstructProcess()
{
  //AddTransportation();
  ConstructEM();
  ConstructlArStepLimiter();
  ConstructGeneral();
  ConstructOp();
  ConstructHad();
}


#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
//#include "G4MultipleScattering.hh"//K.Z.: obsolete class, has to be removed.
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4hIonisation.hh"
#include "G4MuonMinusCaptureAtRest.hh"

//K.Z.: New MultipleScattering classes 
#include "G4eMultipleScattering.hh"
#include "G4MuMultipleScattering.hh"
#include "G4hMultipleScattering.hh"

//---E&M construction

void WCSimPhysicsList::ConstructEM()
{
  //G4MultipleScattering class becomes obsolete and has to be removed and
  //replaced by new G4MultipleScattering classes for e+-, mu+-, hadron and ions.
  //K. Zbiri, 12/30/2009

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
     
    if (particleName == "gamma") {
    // gamma
      pmanager->AddDiscreteProcess(new G4GammaConversion());
      pmanager->AddDiscreteProcess(new G4ComptonScattering());      
      pmanager->AddDiscreteProcess(new G4PhotoElectricEffect());

    } else if (particleName == "e-") {
    //electron
      // G4VProcess* theeminusMultipleScattering = new G4MultipleScattering();
      G4VProcess* theeminusMultipleScattering = new G4eMultipleScattering();
      G4VProcess* theeminusIonisation         = new G4eIonisation();
      G4VProcess* theeminusBremsstrahlung     = new G4eBremsstrahlung();
      //
      // add processes
      pmanager->AddProcess(theeminusMultipleScattering);
      pmanager->AddProcess(theeminusIonisation);
      pmanager->AddProcess(theeminusBremsstrahlung);
      //      
      // set ordering for AlongStepDoIt
      pmanager->SetProcessOrdering(theeminusMultipleScattering,idxAlongStep,1);
      pmanager->SetProcessOrdering(theeminusIonisation,        idxAlongStep,2);
      //
      // set ordering for PostStepDoIt
      pmanager->SetProcessOrdering(theeminusMultipleScattering, idxPostStep,1);
      pmanager->SetProcessOrdering(theeminusIonisation,         idxPostStep,2);
      pmanager->SetProcessOrdering(theeminusBremsstrahlung,     idxPostStep,3);

    } else if (particleName == "e+") {
    //positron
      // G4VProcess* theeplusMultipleScattering = new G4MultipleScattering();
      G4VProcess* theeplusMultipleScattering = new G4eMultipleScattering();
      G4VProcess* theeplusIonisation         = new G4eIonisation();
      G4VProcess* theeplusBremsstrahlung     = new G4eBremsstrahlung();
      G4VProcess* theeplusAnnihilation       = new G4eplusAnnihilation();
      //
      // add processes
      pmanager->AddProcess(theeplusMultipleScattering);
      pmanager->AddProcess(theeplusIonisation);
      pmanager->AddProcess(theeplusBremsstrahlung);
      pmanager->AddProcess(theeplusAnnihilation);
      //
      // set ordering for AtRestDoIt
      pmanager->SetProcessOrderingToFirst(theeplusAnnihilation, idxAtRest);
      //
      // set ordering for AlongStepDoIt
      pmanager->SetProcessOrdering(theeplusMultipleScattering, idxAlongStep,1);
      pmanager->SetProcessOrdering(theeplusIonisation,         idxAlongStep,2);
      //
      // set ordering for PostStepDoIt
      pmanager->SetProcessOrdering(theeplusMultipleScattering, idxPostStep,1);
      pmanager->SetProcessOrdering(theeplusIonisation,         idxPostStep,2);
      pmanager->SetProcessOrdering(theeplusBremsstrahlung,     idxPostStep,3);
      pmanager->SetProcessOrdering(theeplusAnnihilation,       idxPostStep,4);

    } else if( particleName == "mu+" ||
               particleName == "mu-"    ) {
    //muon
      //G4VProcess* aMultipleScattering = new G4MultipleScattering();
       G4VProcess* aMultipleScattering = new G4MuMultipleScattering();
       G4VProcess* aBremsstrahlung     = new G4MuBremsstrahlung();
      G4VProcess* aPairProduction     = new G4MuPairProduction();
      G4VProcess* anIonisation        = new G4MuIonisation();
      //
      // add processes
      pmanager->AddProcess(anIonisation);
      pmanager->AddProcess(aMultipleScattering);
      pmanager->AddProcess(aBremsstrahlung);
      pmanager->AddProcess(aPairProduction);
      //
      // set ordering for AlongStepDoIt
      pmanager->SetProcessOrdering(aMultipleScattering, idxAlongStep,1);
      pmanager->SetProcessOrdering(anIonisation,        idxAlongStep,2);
      //
      // set ordering for PostStepDoIt
      pmanager->SetProcessOrdering(aMultipleScattering, idxPostStep,1);
      pmanager->SetProcessOrdering(anIonisation,        idxPostStep,2);
      pmanager->SetProcessOrdering(aBremsstrahlung,     idxPostStep,3);
      pmanager->SetProcessOrdering(aPairProduction,     idxPostStep,4);

      // MF , stolen from CWW, april 2005
      if (particleName == "mu-")
        {
          G4VProcess* aG4MuonMinusCaptureAtRest =
            new G4MuonMinusCaptureAtRest();
          pmanager->AddProcess(aG4MuonMinusCaptureAtRest);
          pmanager->SetProcessOrdering(aG4MuonMinusCaptureAtRest,idxAtRest);
        }


    } else if ((!particle->IsShortLived()) &&
	       (particle->GetPDGCharge() != 0.0)&&
               (particle->GetParticleName() != "chargedgeantino")) {
      // G4VProcess* aMultipleScattering = new G4MultipleScattering();
     G4VProcess* aMultipleScattering = new G4hMultipleScattering();
     G4VProcess* anIonisation        = new G4hIonisation();
     //
     // add processes
     pmanager->AddProcess(anIonisation);
     pmanager->AddProcess(aMultipleScattering);
     //
     // set ordering for AlongStepDoIt
     pmanager->SetProcessOrdering(aMultipleScattering, idxAlongStep,1);
     pmanager->SetProcessOrdering(anIonisation,        idxAlongStep,2);
     //
     // set ordering for PostStepDoIt
     pmanager->SetProcessOrdering(aMultipleScattering, idxPostStep,1);
     pmanager->SetProcessOrdering(anIonisation,        idxPostStep,2);
    }
  }
}

#ifdef GEANT4_7_0
#include "G4StepLimiter.hh"
#endif

void WCSimPhysicsList::ConstructlArStepLimiter(){

#ifdef GEANT4_7_0
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){

    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();

     if ((!particle->IsShortLived()) &&
	 (particle->GetPDGCharge() != 0.0)&&
	 (particle->GetParticleName() != "chargedgeantino")) {
       G4VProcess* stepLimiter = new G4StepLimiter();   
       pmanager->AddProcess(stepLimiter);
       pmanager->SetProcessOrdering(stepLimiter,
				    idxPostStep,
				    pmanager->GetProcessListLength());
     }
  }
#endif
}

#include "G4Cerenkov.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpWLS.hh"
#include "G4OpMieHG.hh"
#include "G4OpBoundaryProcess.hh"

void WCSimPhysicsList::ConstructOp(){

  G4Cerenkov*           theCherenkovProcess        = new G4Cerenkov("Cerenkov");
  G4OpAbsorption*      theAbsorptionProcess         = new G4OpAbsorption();
  G4OpRayleigh*        theRayleighScatteringProcess = new G4OpRayleigh();
  G4OpWLS*              theWLSProcess                = new G4OpWLS();
  G4OpMieHG*  theMieHGScatteringProcess = new G4OpMieHG();
  G4OpBoundaryProcess* theBoundaryProcess          = new G4OpBoundaryProcess();

//   theCherenkovProcess->DumpPhysicsTable();
//   theAbsorptionProcess->DumpPhysicsTable();
//   theRayleighScatteringProcess->DumpPhysicsTable();

  theCherenkovProcess->SetVerboseLevel(0);
  theAbsorptionProcess->SetVerboseLevel(0);
  theRayleighScatteringProcess->SetVerboseLevel(0);
  theMieHGScatteringProcess->SetVerboseLevel(0);
  theBoundaryProcess->SetVerboseLevel(0);
  theWLSProcess->SetVerboseLevel(0);
  
  theWLSProcess->UseTimeProfile("exponential");

  G4int MaxNumPhotons = 300;
  theCherenkovProcess->SetTrackSecondariesFirst(true);
  theCherenkovProcess->SetMaxNumPhotonsPerStep(MaxNumPhotons);

  // FROM N06 example:
  // theCherenkovProcess->SetMaxNumPhotonsPerStep(20);
  // theCherenkovProcess->SetMaxBetaChangePerStep(10.0);
  // theCherenkovProcess->SetTrackSecondariesFirst(true);

  theParticleIterator->reset();
  while( (*theParticleIterator)() )
  {
    G4ParticleDefinition* particle     = theParticleIterator->value();
    G4ProcessManager*     pmanager     = particle->GetProcessManager();
    G4String              particleName = particle->GetParticleName();
    
    /*  if (theCherenkovProcess->IsApplicable(*particle))
	pmanager->AddContinuousProcess(theCherenkovProcess);*/

     if (theCherenkovProcess->IsApplicable(*particle)) {
      pmanager->AddProcess(theCherenkovProcess);
      pmanager->SetProcessOrdering(theCherenkovProcess,idxPostStep);
      }
    
    
    if (particleName == "opticalphoton") 
      {
	pmanager->AddDiscreteProcess(theAbsorptionProcess);
	//     G4cout << "warning direct light only\n";
	pmanager->AddDiscreteProcess(theRayleighScatteringProcess);
	pmanager->AddDiscreteProcess(theWLSProcess);
	pmanager->AddDiscreteProcess(theMieHGScatteringProcess);
	pmanager->AddDiscreteProcess(theBoundaryProcess);
	
      }
  }
}
//---General construction

#include "G4Decay.hh"

void WCSimPhysicsList::ConstructGeneral()
{
  // Add Decay Process
  G4Decay* theDecayProcess = new G4Decay();
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (theDecayProcess->IsApplicable(*particle)) { 
      pmanager ->AddProcess(theDecayProcess);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
    }
  }
}

// Elastic processes:
#include "G4HadronElasticProcess.hh"

// Inelastic processes:
#include "G4PionPlusInelasticProcess.hh"
#include "G4PionMinusInelasticProcess.hh"
#include "G4KaonPlusInelasticProcess.hh"
#include "G4KaonZeroSInelasticProcess.hh"
#include "G4KaonZeroLInelasticProcess.hh"
#include "G4KaonMinusInelasticProcess.hh"
#include "G4ProtonInelasticProcess.hh"
#include "G4AntiProtonInelasticProcess.hh"
#include "G4NeutronInelasticProcess.hh"
#include "G4AntiNeutronInelasticProcess.hh"
#include "G4DeuteronInelasticProcess.hh"
#include "G4TritonInelasticProcess.hh"
#include "G4AlphaInelasticProcess.hh"

// Low-energy Models: < 20GeV
#include "G4LElastic.hh"
#include "G4LEPionPlusInelastic.hh"
#include "G4LEPionMinusInelastic.hh"
#include "G4LEKaonPlusInelastic.hh"
#include "G4LEKaonZeroSInelastic.hh"
#include "G4LEKaonZeroLInelastic.hh"
#include "G4LEKaonMinusInelastic.hh"
#include "G4LEProtonInelastic.hh"
#include "G4LEAntiProtonInelastic.hh"
#include "G4LENeutronInelastic.hh"
#include "G4LEAntiNeutronInelastic.hh"
#include "G4LEDeuteronInelastic.hh"
#include "G4LETritonInelastic.hh"
#include "G4LEAlphaInelastic.hh"

// High-energy Models: >20 GeV
#include "G4HEPionPlusInelastic.hh"
#include "G4HEPionMinusInelastic.hh"
#include "G4HEKaonPlusInelastic.hh"
#include "G4HEKaonZeroInelastic.hh"
#include "G4HEKaonZeroInelastic.hh"
#include "G4HEKaonMinusInelastic.hh"
#include "G4HEProtonInelastic.hh"
#include "G4HEAntiProtonInelastic.hh"
#include "G4HENeutronInelastic.hh"
#include "G4HEAntiNeutronInelastic.hh"

// Neutron high-precision models: <20 MeV
#include "G4NeutronHPElastic.hh"
#include "G4NeutronHPElasticData.hh"
#include "G4NeutronHPCapture.hh"
#include "G4NeutronHPCaptureData.hh"
#include "G4NeutronHPInelastic.hh"
#include "G4NeutronHPInelasticData.hh"
#include "G4LCapture.hh"

//=================================
// Added by JLR 2005-07-05
//=================================
// Secondary hadronic interaction models
#include "G4CascadeInterface.hh"
#include "G4BinaryCascade.hh"

// Stopping processes
#include "G4PiMinusAbsorptionAtRest.hh"
#include "G4KaonMinusAbsorptionAtRest.hh"
#include "G4AntiProtonAnnihilationAtRest.hh"
#include "G4AntiNeutronAnnihilationAtRest.hh"

void WCSimPhysicsList::ConstructHad() 
{

// Makes discrete physics processes for the hadrons, at present limited
// to those particles with GHEISHA interactions (INTRC > 0).
// The processes are: Elastic scattering and Inelastic scattering.
// F.W.Jones  09-JUL-1998
// 
// This code stolen from:
// examples/advanced/underground_physics/src/DMXPhysicsList.cc
// CWW 2/23/05
//
  
  G4HadronElasticProcess* theElasticProcess = new G4HadronElasticProcess;
  G4LElastic* theElasticModel = new G4LElastic;
  theElasticProcess->RegisterMe(theElasticModel);
  
  theParticleIterator->reset();
  while ((*theParticleIterator)()) 
    {
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      G4String particleName = particle->GetParticleName();

      if (particleName == "pi+") 
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4PionPlusInelasticProcess* theInelasticProcess = 
	    new G4PionPlusInelasticProcess();
	  G4LEPionPlusInelastic* theLEInelasticModel = 
	    new G4LEPionPlusInelastic;
	  theInelasticProcess->RegisterMe(theLEInelasticModel);
	  G4HEPionPlusInelastic* theHEInelasticModel = 
	    new G4HEPionPlusInelastic;
	  theInelasticProcess->RegisterMe(theHEInelasticModel);
	  pmanager->AddDiscreteProcess(theInelasticProcess);
	} 

      else if (particleName == "pi-") 
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4PionMinusInelasticProcess* theInelasticProcess = 
	    new G4PionMinusInelasticProcess();
	  G4LEPionMinusInelastic* theLEInelasticModel = 
	    new G4LEPionMinusInelastic;
	  theInelasticProcess->RegisterMe(theLEInelasticModel);
	  G4HEPionMinusInelastic* theHEInelasticModel = 
	    new G4HEPionMinusInelastic;
	  theInelasticProcess->RegisterMe(theHEInelasticModel);
	  pmanager->AddDiscreteProcess(theInelasticProcess);
	  G4String prcNam;
	  pmanager->AddRestProcess(new G4PiMinusAbsorptionAtRest, ordDefault);
	}
      
      else if (particleName == "kaon+") 
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4KaonPlusInelasticProcess* theInelasticProcess = 
	    new G4KaonPlusInelasticProcess();
	  G4LEKaonPlusInelastic* theLEInelasticModel = 
	    new G4LEKaonPlusInelastic;
	  theInelasticProcess->RegisterMe(theLEInelasticModel);
	  G4HEKaonPlusInelastic* theHEInelasticModel = 
	    new G4HEKaonPlusInelastic;
	  theInelasticProcess->RegisterMe(theHEInelasticModel);
	  pmanager->AddDiscreteProcess(theInelasticProcess);
	}
      
      else if (particleName == "kaon0S") 
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4KaonZeroSInelasticProcess* theInelasticProcess = 
	    new G4KaonZeroSInelasticProcess();
	  G4LEKaonZeroSInelastic* theLEInelasticModel = 
	    new G4LEKaonZeroSInelastic;
	  theInelasticProcess->RegisterMe(theLEInelasticModel);
	  G4HEKaonZeroInelastic* theHEInelasticModel = 
	    new G4HEKaonZeroInelastic;
	  theInelasticProcess->RegisterMe(theHEInelasticModel);
	  pmanager->AddDiscreteProcess(theInelasticProcess);
	}

      else if (particleName == "kaon0L") 
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4KaonZeroLInelasticProcess* theInelasticProcess = 
	    new G4KaonZeroLInelasticProcess();
	  G4LEKaonZeroLInelastic* theLEInelasticModel = 
	    new G4LEKaonZeroLInelastic;
	  theInelasticProcess->RegisterMe(theLEInelasticModel);
	  G4HEKaonZeroInelastic* theHEInelasticModel = 
	    new G4HEKaonZeroInelastic;
	  theInelasticProcess->RegisterMe(theHEInelasticModel);
	  pmanager->AddDiscreteProcess(theInelasticProcess);
	}

      else if (particleName == "kaon-") 
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4KaonMinusInelasticProcess* theInelasticProcess = 
	    new G4KaonMinusInelasticProcess();
	  G4LEKaonMinusInelastic* theLEInelasticModel = 
	    new G4LEKaonMinusInelastic;
	  theInelasticProcess->RegisterMe(theLEInelasticModel);
	  G4HEKaonMinusInelastic* theHEInelasticModel = 
	    new G4HEKaonMinusInelastic;
	  theInelasticProcess->RegisterMe(theHEInelasticModel);
	  pmanager->AddDiscreteProcess(theInelasticProcess);
	  pmanager->AddRestProcess(new G4KaonMinusAbsorptionAtRest, ordDefault);
	}

      else if (particleName == "proton") 
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4ProtonInelasticProcess* theInelasticProcess = 
	    new G4ProtonInelasticProcess();

	  
	  //=================================
	  // Added by JLR 2005-07-05
	  //=================================
	  // Options for secondary interaction models
	  // Choice defined in jobOptions.mac, which is
	  // read in before initialization of the run manager.
	  // In the absence of this file, BINARY will be used.
	  // Gheisha = Original Geant4 default 
	  // Bertini = Bertini intra-nuclear cascade model
	  // Binary  = Binary intra-nuclear cascade model
	  if (gheishahad) {
	    G4LEProtonInelastic* theLEInelasticModel = new G4LEProtonInelastic;
	    theInelasticProcess->RegisterMe(theLEInelasticModel);
	  }
	  else if (bertinihad) {
	    G4CascadeInterface* theBertiniModel = new G4CascadeInterface;
	    theInelasticProcess->RegisterMe(theBertiniModel);
	  }
	  else if (binaryhad) {
	    G4BinaryCascade* theBinaryModel = new G4BinaryCascade();
	    theInelasticProcess->RegisterMe(theBinaryModel);

	    G4LEProtonInelastic* theLEInelasticModel = new G4LEProtonInelastic;
	    theLEInelasticModel->SetMinEnergy(10.1*GeV);
	    theLEInelasticModel->SetMaxEnergy( 45.*GeV );
	    theInelasticProcess->RegisterMe(theLEInelasticModel);

	  }
	  else {
	    G4cout << "No secondary interaction model chosen! Using G4 BINARY." << G4endl;
	    G4BinaryCascade* theBinaryModel = new G4BinaryCascade();
	    theInelasticProcess->RegisterMe(theBinaryModel);
	  }

	  G4HEProtonInelastic* theHEInelasticModel = new G4HEProtonInelastic;
	  theInelasticProcess->RegisterMe(theHEInelasticModel);
	  pmanager->AddDiscreteProcess(theInelasticProcess);
	}

      else if (particleName == "anti_proton") 
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4AntiProtonInelasticProcess* theInelasticProcess = 
	    new G4AntiProtonInelasticProcess();
	  G4LEAntiProtonInelastic* theLEInelasticModel = 
	    new G4LEAntiProtonInelastic;
	  theInelasticProcess->RegisterMe(theLEInelasticModel);
	  G4HEAntiProtonInelastic* theHEInelasticModel = 
	    new G4HEAntiProtonInelastic;
	  theInelasticProcess->RegisterMe(theHEInelasticModel);
	  pmanager->AddDiscreteProcess(theInelasticProcess);
	}

      else if (particleName == "neutron") 
	{
	  // elastic scattering
	  G4HadronElasticProcess* theNeutronElasticProcess = 
	    new G4HadronElasticProcess;
	  G4LElastic* theElasticModel1 = new G4LElastic;
	  G4NeutronHPElastic * theElasticNeutron = new G4NeutronHPElastic;
	  theNeutronElasticProcess->RegisterMe(theElasticModel1);
	  theElasticModel1->SetMinEnergy(19*MeV);
	  theNeutronElasticProcess->RegisterMe(theElasticNeutron);
	  G4NeutronHPElasticData * theNeutronData = new G4NeutronHPElasticData;
	  theNeutronElasticProcess->AddDataSet(theNeutronData);
	  pmanager->AddDiscreteProcess(theNeutronElasticProcess);
	  
	  // inelastic scattering
	  G4NeutronInelasticProcess* theInelasticProcess =
	    new G4NeutronInelasticProcess();
	  
	  //=================================
	  // Added by JLR 2005-07-05
	  //=================================
	  // Options for secondary interaction models
	  // Choice defined in jobOptions.mac, which is
	  // read in before initialization of the run manager.
	  // In the absence of this file, BINARY will be used.
	  // GHEISHA = Original Geant4 default model 
	  // BERTINI = Bertini intra-nuclear cascade model
	  // BINARY  = Binary intra-nuclear cascade model
	  if (gheishahad) {
	    G4LENeutronInelastic* theInelasticModel = new G4LENeutronInelastic;
	    theInelasticModel->SetMinEnergy(19*MeV);
	    theInelasticProcess->RegisterMe(theInelasticModel);
	  }
	  else if (bertinihad) {
	    G4CascadeInterface* theBertiniModel = new G4CascadeInterface;
	    theBertiniModel->SetMinEnergy(19*MeV);
	    theInelasticProcess->RegisterMe(theBertiniModel);
	  }
	  else if (binaryhad) {
	    G4BinaryCascade* theBinaryModel = new G4BinaryCascade();
	    theBinaryModel->SetMinEnergy(19*MeV);
	    theInelasticProcess->RegisterMe(theBinaryModel);

      G4LENeutronInelastic* theInelasticModel = new G4LENeutronInelastic;
      theInelasticModel->SetMinEnergy(10.1*GeV);
      theInelasticModel->SetMaxEnergy( 45.*GeV );
      theInelasticProcess->RegisterMe(theInelasticModel);
	  }
	  else {
	    G4cout << "No secondary interaction model chosen! Using G4 BINARY." << G4endl;
	    G4BinaryCascade* theBinaryModel = new G4BinaryCascade();
	    theBinaryModel->SetMinEnergy(19*MeV);
	    theInelasticProcess->RegisterMe(theBinaryModel);
	  }
	  
	  G4HENeutronInelastic* theHEInelasticModel = new G4HENeutronInelastic;
	  theInelasticProcess->RegisterMe(theHEInelasticModel);
	  
	  G4NeutronHPInelastic * theLENeutronInelasticModel =
	    new G4NeutronHPInelastic;
	  theInelasticProcess->RegisterMe(theLENeutronInelasticModel);
	  
	  G4NeutronHPInelasticData * theNeutronData1 = 
	    new G4NeutronHPInelasticData;
	  theInelasticProcess->AddDataSet(theNeutronData1);
	  pmanager->AddDiscreteProcess(theInelasticProcess);

	  // capture
	  G4HadronCaptureProcess* theCaptureProcess =
	    new G4HadronCaptureProcess;
	  G4LCapture* theCaptureModel = new G4LCapture;
	  theCaptureModel->SetMinEnergy(19*MeV);
	  theCaptureProcess->RegisterMe(theCaptureModel);
	  G4NeutronHPCapture * theLENeutronCaptureModel = new G4NeutronHPCapture;
	  theCaptureProcess->RegisterMe(theLENeutronCaptureModel);
	  G4NeutronHPCaptureData * theNeutronData3 = new G4NeutronHPCaptureData;
	  theCaptureProcess->AddDataSet(theNeutronData3);
	  pmanager->AddDiscreteProcess(theCaptureProcess);
	  //  G4ProcessManager* pmanager = G4Neutron::Neutron->GetProcessManager();
	  //  pmanager->AddProcess(new G4UserSpecialCuts(),-1,-1,1);
	}

      else if (particleName == "anti_neutron") 
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4AntiNeutronInelasticProcess* theInelasticProcess = 
	    new G4AntiNeutronInelasticProcess();
	  G4LEAntiNeutronInelastic* theLEInelasticModel = 
	    new G4LEAntiNeutronInelastic;
	  theInelasticProcess->RegisterMe(theLEInelasticModel);
	  G4HEAntiNeutronInelastic* theHEInelasticModel = 
	    new G4HEAntiNeutronInelastic;
	  theInelasticProcess->RegisterMe(theHEInelasticModel);
	  pmanager->AddDiscreteProcess(theInelasticProcess);
	}

      else if (particleName == "deuteron") 
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4DeuteronInelasticProcess* theInelasticProcess = 
	    new G4DeuteronInelasticProcess();
	  G4LEDeuteronInelastic* theLEInelasticModel = 
	    new G4LEDeuteronInelastic;
	  theInelasticProcess->RegisterMe(theLEInelasticModel);
	  pmanager->AddDiscreteProcess(theInelasticProcess);
	}
      
      else if (particleName == "triton") 
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4TritonInelasticProcess* theInelasticProcess = 
	    new G4TritonInelasticProcess();
	  G4LETritonInelastic* theLEInelasticModel = 
	    new G4LETritonInelastic;
	  theInelasticProcess->RegisterMe(theLEInelasticModel);
	  pmanager->AddDiscreteProcess(theInelasticProcess);
	}

      else if (particleName == "alpha") 
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4AlphaInelasticProcess* theInelasticProcess = 
	    new G4AlphaInelasticProcess();
	  G4LEAlphaInelastic* theLEInelasticModel = 
	    new G4LEAlphaInelastic;
	  theInelasticProcess->RegisterMe(theLEInelasticModel);
	  pmanager->AddDiscreteProcess(theInelasticProcess);
	}

    }
}

//=================================
// Added by JLR 2005-07-05
//=================================
// Sets secondary hadronic interaction model 
// Note: this is currently only implemented for
// protons and neutrons -- not pions.
// Gheisha = Original Geant4 default 
// Bertini = Bertini intra-nuclear cascade model
// Binary  = Binary intra-nuclear cascade model
void WCSimPhysicsList::SetSecondaryHad(G4String hadval)
{
  SecondaryHadModel = hadval;

  if (SecondaryHadModel == "GHEISHA") {
    G4cout << "Secondary interaction model set to GHEISHA" << G4endl;
    gheishahad = true;
    bertinihad = false;
    binaryhad  = false;
  }
  else if (SecondaryHadModel == "BERTINI") {
    G4cout << "Secondary interaction model set to BERTINI" << G4endl;
    gheishahad = false;
    bertinihad = true;
    binaryhad  = false;
  }
  else if (SecondaryHadModel == "BINARY") {
    G4cout << "Secondary interaction model set to BINARY" << G4endl;
    gheishahad = false;
    bertinihad = false;
    binaryhad  = true;
  }
  else {
    G4cout << "Secondary interaction model " << SecondaryHadModel
	   << " is not a valid choice. BINARY model will be used." << G4endl;
    gheishahad = false;
    bertinihad = false;
    binaryhad  = true;
  }


}

//----set cut values----
/* Setting cuts occurs in WCSimPhysicsListFactory.cc
void WCSimPhysicsList::SetCuts()
{
  if (verboseLevel >0){
    G4cout << "WCSimPhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
  }

  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  //
  SetCutValue(defaultCutValue, "gamma");
  SetCutValue(defaultCutValue, "e-");
  SetCutValue(defaultCutValue, "e+");

  if (verboseLevel>0) DumpCutValuesTable();
}
*/
