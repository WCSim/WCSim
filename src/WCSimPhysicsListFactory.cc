WCSimPhysicsListFactory::WCSimPhysicsListFactory():  G4VUserPhysicsList(), PhysicsMessenger(0)
{
 defaultCutValue = 1.0*mm;
 SetVerboseLevel(1);

 PhysicsMessenger = new WCSimPhysicsMessenger(this);

}

WCSimPhysicsListFactory::~WCSimPhysicsListFactory()
{
  delete PhysicsMessenger;
  PhysicsMessenger = 0;
}


void WCSimPhysicsListFactory::ConstructParticle()
{
}


void WCSimPhysicsListFactory::ConstructProcess()
{
}

void WCSimPhysicsListFactory::SetCuts()
{
}


void WCSimPhysicsListFactory::SetFactory(){
  G4PhysListFactory factory;
  G4VModularPhysicsList* phys = 0;
  G4String physName = "";
  if (argc==3) physName = argv[2];

  if (factory.IsReferencePhysList(physName)) {
    phys = factory.GetReferencePhysList(physName);
    runManager->SetUserInitialization(phys);
  } else {
    runManager->SetUserInitialization(new WCSimPhysicsList); // WCSim default
  }
}
