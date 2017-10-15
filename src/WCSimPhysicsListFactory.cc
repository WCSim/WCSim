#include <G4ProcessTable.hh>
#include <G4Neutron.hh>
#include <G4NeutronHPCapture.hh>
#include <G4NeutronHPCaptureData.hh>
#include <G4HadronCaptureProcess.hh>
#include <G4NeutronRadCapture.hh>
#include <G4NeutronCaptureXS.hh>
#include <G4CrossSectionDataSetRegistry.hh>
#include <G4HadronicInteractionRegistry.hh>
#include "WCSimPhysicsListFactory.hh"

#include "GdNeutronHPCapture.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

/* This code draws upon examples/extended/fields/field04 for inspiration */



WCSimPhysicsListFactory::WCSimPhysicsListFactory() :  G4VModularPhysicsList()
{
 defaultCutValue = 1.0*mm;
 SetVerboseLevel(1);
 
 PhysicsListName="NULL_LIST"; // default list is set in WCSimPhysicsListFactoryMessenger to FTFP_BERT
 factory = new G4PhysListFactory();
 // ToDo create opticalPhyscics object?
 
 std::vector<G4String> ValidListsVector = factory->AvailablePhysLists();
 G4int nlists = ValidListsVector.size();
 G4cout << "There are " << nlists << " available physics lists, and they are: " << G4endl;
 for (G4int i=0; i<nlists; i++){
   G4cout << "  " << ValidListsVector[i] << G4endl;
   ValidListsString += ValidListsVector[i];
   ValidListsString += " ";
 }
 //G4cout << "ValidListsString=" << ValidListsString << G4endl;

 PhysicsMessenger = new WCSimPhysicsListFactoryMessenger(this, ValidListsString);

}

WCSimPhysicsListFactory::~WCSimPhysicsListFactory()
{
  delete PhysicsMessenger;
  PhysicsMessenger = NULL;

}

void WCSimPhysicsListFactory::ConstructParticle()
{
  G4VModularPhysicsList::ConstructParticle();
}

void WCSimPhysicsListFactory::ConstructProcess() {
    G4VModularPhysicsList::ConstructProcess();
    if (nCaptModelChoice.compareTo("Default", G4String::ignoreCase) == 0) return;
    G4ProcessTable *table = G4ProcessTable::GetProcessTable();
    G4ProcessManager *manager = G4Neutron::Neutron()->GetProcessManager();
    if (manager) {
        G4VProcess *captureProcess = table->FindProcess("nCapture", G4Neutron::Neutron());

        if (captureProcess) {
            G4cout << "Removing default nCapture process" << G4endl;
            manager->RemoveProcess(captureProcess);
            // Due to bug in Geant (fixed in v4.10.2) must remove old NeutronHPCapture from registry, if it exists, to prevent segfault on cleanup
            G4HadronicInteractionRegistry *registry = G4HadronicInteractionRegistry::Instance();
            registry->RemoveMe(registry->FindModel("NeutronHPCapture"));
        }

        G4HadronCaptureProcess *theCaptureProcess = new G4HadronCaptureProcess;
        manager->AddDiscreteProcess(theCaptureProcess);

        G4NeutronCaptureXS *xsNeutronCaptureXS = (G4NeutronCaptureXS *) G4CrossSectionDataSetRegistry::Instance()->GetCrossSectionDataSet(G4NeutronCaptureXS::Default_Name());
        theCaptureProcess->AddDataSet(xsNeutronCaptureXS);

        G4HadronicInteraction *theNeutronRadCapture = new G4NeutronRadCapture();

        if (nCaptModelChoice.compareTo("Rad", G4String::ignoreCase) != 0) {
            theCaptureProcess->AddDataSet(new G4NeutronHPCaptureData);
            //G4NeutronHPCapture *theNeutronHPCapture = new G4NeutronHPCapture();
            G4HadronicInteraction *theNeutronHPCapture;
            if (nCaptModelChoice.compareTo("GLG4Sim", G4String::ignoreCase) == 0) {
                G4cout << "Enabling GLG4Sim HP nCapture process" << G4endl;
                theNeutronHPCapture = new GdNeutronHPCapture();
            } else if (nCaptModelChoice.compareTo("HP", G4String::ignoreCase) == 0) {
                G4cout << "Enabling HP nCapture process" << G4endl;
                theNeutronHPCapture = new G4NeutronHPCapture();
            }
            else{
                G4cout << "Unknown model choice. Using HP." << G4endl;
                theNeutronHPCapture = new G4NeutronHPCapture();
            }
            theNeutronHPCapture->SetMinEnergy(0);
            theNeutronHPCapture->SetMaxEnergy(20 * MeV);
            theCaptureProcess->RegisterMe(theNeutronHPCapture);
            theCaptureProcess->AddDataSet(new G4NeutronHPCaptureData);
            theNeutronRadCapture->SetMinEnergy(19.9 * MeV);
        }
        G4cout << "Enabling RadCapture nCapture process" << G4endl;
        theCaptureProcess->RegisterMe(theNeutronRadCapture);
    }
}

void WCSimPhysicsListFactory::SetCuts()
{
  if (verboseLevel >0){
      G4cout << "WCSimPhysicsListFactory::SetCuts:";
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

void WCSimPhysicsListFactory::SetList(G4String newvalue){
    G4cout << "Setting Physics list to " << newvalue << " and delaying initialization" << G4endl;
    PhysicsListName = newvalue;
}

void WCSimPhysicsListFactory::SetnCaptModel(G4String newvalue){
    G4cout << "Setting neutron capture model to " << newvalue << G4endl;
    nCaptModelChoice = newvalue;
}

void WCSimPhysicsListFactory::InitializeList(){
  G4cout << "Initializing physics list " << PhysicsListName << G4endl;

  G4VModularPhysicsList* phys = 0;

  if (factory->IsReferencePhysList(PhysicsListName)) {
    phys=factory->GetReferencePhysList(PhysicsListName);
    for (G4int i = 0; ; ++i) {
      G4VPhysicsConstructor* elem =
        const_cast<G4VPhysicsConstructor*> (phys->GetPhysics(i));
      if (elem == NULL) break;
      G4cout << "RegisterPhysics: " << elem->GetPhysicsName() << G4endl;
      RegisterPhysics(elem);
    }
    G4cout << "RegisterPhysics: OpticalPhysics" << G4endl; 
    RegisterPhysics(new G4OpticalPhysics());
  } else {
    G4cout << "Physics list " << PhysicsListName << " is not understood" << G4endl;
  }
} 

void WCSimPhysicsListFactory::SaveOptionsToOutput(WCSimRootOptions * wcopt)
{
  wcopt->SetPhysicsListName(PhysicsListName);
}
