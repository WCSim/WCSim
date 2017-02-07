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

// FDL replaced in 4.9 by G4MuonMinusCapture
#include "G4MuonMinusCapture.hh"
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

  aParticleIterator->reset();
  while( (*aParticleIterator)() ){
    G4ParticleDefinition* particle = aParticleIterator->value();
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
	  // Change to G4MuonMinusCapture - FDL
	  G4VProcess* aG4MuonMinusCapture = new G4MuonMinusCapture();
          pmanager->AddProcess(aG4MuonMinusCapture);
          pmanager->SetProcessOrdering(aG4MuonMinusCapture, idxAtRest);
        }


    } else if ((!particle->IsShortLived()) &&
	       (particle->GetPDGCharge() != 0.0)&&
               (particle->GetParticleName() != "chargedgeantino")) {

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
  aParticleIterator->reset();
  while( (*aParticleIterator)() ){

    G4ParticleDefinition* particle = aParticleIterator->value();
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

  aParticleIterator->reset();
  while( (*aParticleIterator)() )
  {
    G4ParticleDefinition* particle     = aParticleIterator->value();
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
  aParticleIterator->reset();
  while( (*aParticleIterator)() ){
    G4ParticleDefinition* particle = aParticleIterator->value();
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
#include "G4INCLXXInterface.hh"
#include "G4TritonInelasticProcess.hh"
#include "G4BinaryLightIonReaction.hh"
#include "G4AlphaInelasticProcess.hh"

// Low-energy Models: < 20GeV
#include "G4HadronElastic.hh"

// Neutron high-precision models: <20 CLHEP::MeV
#include "G4NeutronHPElastic.hh"
#include "G4NeutronHPElasticData.hh"
#include "G4NeutronHPCapture.hh"
#include "G4NeutronHPCaptureData.hh"
#include "G4NeutronHPInelastic.hh"
#include "G4NeutronHPInelasticData.hh"

//=================================
// Added by JLR 2005-07-05
//=================================
// Secondary hadronic interaction models
#include "G4CascadeInterface.hh"
#include "G4BinaryCascade.hh"

// Stopping processes
#include "G4PiMinusAbsorptionBertini.hh"
#include "G4KaonMinusAbsorptionBertini.hh"
#include "G4AntiProtonAbsorptionFritiof.hh"
 #include "G4AntiNeutronAnnihilationAtRest.hh"

// Include the FTF model - FDL
#include "G4FTFModel.hh"
#include "G4LundStringFragmentation.hh"
#include "G4ExcitedStringDecay.hh"
#include "G4PreCompoundModel.hh"
#include "G4GeneratorPrecompoundInterface.hh"
#include "G4TheoFSGenerator.hh"

void WCSimPhysicsList::ConstructHad() 
{

// This code stolen from:
// examples/advanced/underground_physics/src/DMXPhysicsList.cc
// CWW 2/23/05
//

  G4HadronElasticProcess* theElasticProcess = new G4HadronElasticProcess;
  G4HadronElastic* theElasticModel = new G4HadronElastic;
  theElasticProcess->RegisterMe(theElasticModel);

  aParticleIterator->reset();
  while ((*aParticleIterator)()) 
    {

     // Add the FRITIOF model - FDL
      G4TheoFSGenerator* FTFP_model = new G4TheoFSGenerator();
      G4GeneratorPrecompoundInterface* theCascade = new G4GeneratorPrecompoundInterface();
      G4ExcitationHandler* theHandler = new G4ExcitationHandler();
      G4PreCompoundModel* thePreEquilib = new G4PreCompoundModel(theHandler);
      theCascade->SetDeExcitation(thePreEquilib);
      FTFP_model->SetTransport(theCascade);
      G4LundStringFragmentation* theFragmentation = new G4LundStringFragmentation();
      G4ExcitedStringDecay* theStringDecay = new G4ExcitedStringDecay(theFragmentation);    
      G4FTFModel* theStringModel = new G4FTFModel;
      theStringModel->SetFragmentationModel(theStringDecay);
      FTFP_model->SetHighEnergyGenerator(theStringModel);
 
      G4ParticleDefinition* particle = aParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      G4String particleName = particle->GetParticleName();

      if (particleName == "pi+")
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4PionPlusInelasticProcess* theInelasticProcess = 
	    new G4PionPlusInelasticProcess();

	  // Added Bertini Model - FDL
	  G4CascadeInterface* theBertiniModel = new G4CascadeInterface;
	  theBertiniModel->SetMaxEnergy(5.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(theBertiniModel);

	  // Added FTFP Model - SS
	  FTFP_model->SetMinEnergy(4.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(FTFP_model);
	  pmanager->AddDiscreteProcess(theInelasticProcess);
	} 

      else if (particleName == "pi-")
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4PionMinusInelasticProcess* theInelasticProcess =
 	    new G4PionMinusInelasticProcess();
	  
	  // Added Bertini Model - FDL
	  G4CascadeInterface* theBertiniModel = new G4CascadeInterface;
	  theBertiniModel->SetMaxEnergy(5.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(theBertiniModel);

	  // Added FTFP Model - SS
	  FTFP_model->SetMinEnergy(4.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(FTFP_model);

 	  pmanager->AddDiscreteProcess(theInelasticProcess);
 	  G4String prcNam;
	  pmanager->AddRestProcess(new G4PiMinusAbsorptionBertini, ordDefault);
 	}
      else if (particleName == "kaon+")
 	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4KaonPlusInelasticProcess* theInelasticProcess =
 	    new G4KaonPlusInelasticProcess();
      
	  // Added Bertini Model - FDL
          G4CascadeInterface* theBertiniModel = new G4CascadeInterface;
          theBertiniModel->SetMaxEnergy(5.0*CLHEP::GeV);
          theInelasticProcess->RegisterMe(theBertiniModel);

          // Added FTFP Model - SS
          FTFP_model->SetMinEnergy(4.0*CLHEP::GeV);
          theInelasticProcess->RegisterMe(FTFP_model);

 	  pmanager->AddDiscreteProcess(theInelasticProcess);
 	}

      else if (particleName == "kaon0S")
 	{
 	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4KaonZeroSInelasticProcess* theInelasticProcess =
	    new G4KaonZeroSInelasticProcess();

	  // Added Bertini Model - FDL
	  G4CascadeInterface* theBertiniModel = new G4CascadeInterface;
	  theBertiniModel->SetMaxEnergy(10.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(theBertiniModel);

	  // Added FTFP Model - SS
	  FTFP_model->SetMinEnergy(3.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(FTFP_model);
	  
 	  pmanager->AddDiscreteProcess(theInelasticProcess);
 	}
 
      else if (particleName == "kaon0L")
 	{
 	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4KaonZeroLInelasticProcess* theInelasticProcess =
 	    new G4KaonZeroLInelasticProcess();
	  
	  // Added Bertini Model - FDL
	  G4CascadeInterface* theBertiniModel = new G4CascadeInterface;
	  theBertiniModel->SetMaxEnergy(10.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(theBertiniModel);

	  // Added FTFP Model - SS
	  FTFP_model->SetMinEnergy(3.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(FTFP_model);

 	  pmanager->AddDiscreteProcess(theInelasticProcess);
 	}
 
      else if (particleName == "kaon-")
 	{
 	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4KaonMinusInelasticProcess* theInelasticProcess =
 	    new G4KaonMinusInelasticProcess();

	  // Added Bertini Model - FDL
	  G4CascadeInterface* theBertiniModel = new G4CascadeInterface;
	  theBertiniModel->SetMaxEnergy(5.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(theBertiniModel);

	  // Added FTFP Model - SS
	  FTFP_model->SetMinEnergy(4.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(FTFP_model);

 	  pmanager->AddDiscreteProcess(theInelasticProcess);
	  pmanager->AddRestProcess(new G4KaonMinusAbsorptionBertini, ordDefault);
 	}
 
      else if (particleName == "proton")
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4ProtonInelasticProcess* theInelasticProcess =
	    new G4ProtonInelasticProcess();

	  if (bertinihad) {
	    G4CascadeInterface* theBertiniModel = new G4CascadeInterface;
	    theBertiniModel->SetMaxEnergy(5.0*CLHEP::GeV);
	    theInelasticProcess->RegisterMe(theBertiniModel);
	  }
	  else if (binaryhad) {
	    G4BinaryCascade* theBinaryModel = new G4BinaryCascade();
	    theBinaryModel->SetMaxEnergy(5.0*CLHEP::GeV);
	    theInelasticProcess->RegisterMe(theBinaryModel);
	  }
	  else {
	    G4cout << "No secondary interaction model chosen! Using G4 BERTINI." << G4endl;
	    G4CascadeInterface* theBertiniModel = new G4CascadeInterface;
	    theBertiniModel->SetMaxEnergy(5.0*CLHEP::GeV);
	    theInelasticProcess->RegisterMe(theBertiniModel);
	  }

	  // Use FTFP model - FDL
	  FTFP_model->SetMinEnergy(4.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(FTFP_model);
	  pmanager->AddDiscreteProcess(theInelasticProcess);
	}

      else if (particleName == "anti_proton")
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4AntiProtonInelasticProcess* theInelasticProcess =
	    new G4AntiProtonInelasticProcess();
	  theInelasticProcess->RegisterMe(FTFP_model);
	  pmanager->AddDiscreteProcess(theInelasticProcess);
	}

      else if (particleName == "neutron")
	{
	  // elastic scattering
	  G4HadronElasticProcess* theNeutronElasticProcess =
	    new G4HadronElasticProcess;
	  G4HadronElastic* theElasticModel1 = new G4HadronElastic;
	  G4NeutronHPElastic * theElasticNeutron = new G4NeutronHPElastic;
	  theNeutronElasticProcess->RegisterMe(theElasticModel1);
	  theElasticModel1->SetMinEnergy(19*CLHEP::MeV);
	  theNeutronElasticProcess->RegisterMe(theElasticNeutron);
	  G4NeutronHPElasticData * theNeutronData = new G4NeutronHPElasticData;
	  theNeutronElasticProcess->AddDataSet(theNeutronData);
	  pmanager->AddDiscreteProcess(theNeutronElasticProcess);

	  // inelastic scattering
	  G4NeutronInelasticProcess* theInelasticProcess =
	    new G4NeutronInelasticProcess();
	  if (bertinihad) {
	    G4CascadeInterface* theBertiniModel = new G4CascadeInterface;
	    theBertiniModel->SetMinEnergy(19*CLHEP::MeV);
	    theBertiniModel->SetMaxEnergy(5.0*CLHEP::GeV);
	    theInelasticProcess->RegisterMe(theBertiniModel);
	  }
	  else if (binaryhad) {
	    G4BinaryCascade* theBinaryModel = new G4BinaryCascade();
	    theBinaryModel->SetMinEnergy(19*CLHEP::MeV);
	    theBinaryModel->SetMaxEnergy(5.0*CLHEP::GeV);
	    theInelasticProcess->RegisterMe(theBinaryModel);
	  }
	  else {
	    G4cout << "No secondary interaction model chosen! Using G4 BERTINI." << G4endl;
	    G4CascadeInterface* theBertiniModel = new G4CascadeInterface();
	    theBertiniModel->SetMinEnergy(19*CLHEP::MeV);
	    theBertiniModel->SetMaxEnergy(5.0*CLHEP::GeV);
	    theInelasticProcess->RegisterMe(theBertiniModel);
	  }
	  
	  // Use FTFP - FDL
	  FTFP_model->SetMinEnergy(4.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(FTFP_model);

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
	  G4NeutronHPCapture* theCaptureModelHP = new G4NeutronHPCapture();
	  theCaptureProcess->RegisterMe(theCaptureModelHP);
	  G4NeutronHPCaptureData* theNeutronCaptureData = new G4NeutronHPCaptureData;
	  theCaptureProcess->AddDataSet(theNeutronCaptureData);

	  pmanager->AddDiscreteProcess(theCaptureProcess);
	}

      else if (particleName == "anti_neutron")
	{
	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4AntiNeutronInelasticProcess* theInelasticProcess =
	    new G4AntiNeutronInelasticProcess();
	  theInelasticProcess->RegisterMe(FTFP_model);
	  pmanager->AddDiscreteProcess(theInelasticProcess);
	}

else if (particleName == "deuteron")
 	{
 	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4DeuteronInelasticProcess* theInelasticProcess =
 	    new G4DeuteronInelasticProcess();

	  // Use INCL++ for low energy and FTFP for high energy - SS
	  G4INCLXXInterface* theINCLXXModel = new G4INCLXXInterface();
	  theINCLXXModel->SetMaxEnergy(3.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(theINCLXXModel);
	  FTFP_model->SetMinEnergy(3.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(FTFP_model);
	  
 	  pmanager->AddDiscreteProcess(theInelasticProcess);
 	}
       
      else if (particleName == "triton")
 	{
 	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4TritonInelasticProcess* theInelasticProcess =
 	    new G4TritonInelasticProcess();

	  // Use Binary Light Ion Reaction for low energy
	  // and FTFP for high energy - SS
	  G4BinaryLightIonReaction* ionModel = new G4BinaryLightIonReaction();
	  ionModel->SetMaxEnergy(10.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(ionModel);
	  FTFP_model->SetMinEnergy(3.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(FTFP_model);

 	  pmanager->AddDiscreteProcess(theInelasticProcess);
 	}
      
      else if (particleName == "alpha")
 	{
 	  pmanager->AddDiscreteProcess(theElasticProcess);
	  G4AlphaInelasticProcess* theInelasticProcess =
 	    new G4AlphaInelasticProcess();

	  // Use Binary for low energy and FTFP for high energy - SS
	  G4BinaryCascade* theBinaryModel = new G4BinaryCascade();
	  theBinaryModel->SetMaxEnergy(4.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(theBinaryModel);
	  FTFP_model->SetMinEnergy(2.0*CLHEP::GeV);
	  theInelasticProcess->RegisterMe(FTFP_model);

 	  pmanager->AddDiscreteProcess(theInelasticProcess);
 	}
    }
}

void WCSimPhysicsList::SetSecondaryHad(G4String hadval)
{

  SecondaryHadModel = hadval;

  if (SecondaryHadModel == "BERTINI") {
    G4cout << "Secondary interaction model set to BERTINI" << G4endl;

    bertinihad = true;
    binaryhad  = false;
  }
  else if (SecondaryHadModel == "BINARY") {
    G4cout << "Secondary interaction model set to BINARY" << G4endl;

    bertinihad = false;
    binaryhad  = true;
  }
  else {
    G4cout << "Secondary interaction model " << SecondaryHadModel
	   << " is not a valid choice. BINARY model will be used." << G4endl;

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
