#ifndef WCSimPhysicsListFactory_h
#define WCSimPhysicsListFactory_h 1

#include "globals.hh"
#include "G4VModularPhysicsList.hh"
#include "G4PhysListFactory.hh"
#include "G4UnitsTable.hh"
#include "G4OpticalPhysics.hh"

#include "WCSimPhysicsListFactoryMessenger.hh"
#include "WCSimRootOptions.hh"

//class WCSimPhysicsList;

class WCSimPhysicsListFactory : public G4VModularPhysicsList
{
  public:
    WCSimPhysicsListFactory();
    ~WCSimPhysicsListFactory();

    void SetList(G4String newvalue);  // called by messenger
    void SetnCaptModel(G4String newvalue);  // called by messenger
    void InitializeList();

    //G4String GetPhysicsListName() {return PhysicsListName;}

    void ConstructParticle();
    void ConstructProcess();
    void SetCuts();

    void SaveOptionsToOutput(WCSimRootOptions * wcopt);

  private:

    G4String PhysicsListName;
    G4String ValidListsString;
    
    G4String nCaptModelChoice;

    WCSimPhysicsListFactoryMessenger* PhysicsMessenger;
    G4PhysListFactory* factory;

};

#endif
