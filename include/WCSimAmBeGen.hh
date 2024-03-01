#ifndef WCSimAmBeGen_h
#define WCSimAmBeGen_h

#include "G4SPSEneDistribution.hh"
#include "G4SPSRandomGenerator.hh"
#include "G4RandomDirection.hh"

#include "globals.hh"
#include "jhfNtuple.h"
#include <fstream>

using namespace std;

class G4ParticleGun;
class G4Event;

class WCSimAmBeGen
{
  public:
    WCSimAmBeGen();
    ~WCSimAmBeGen();

    // Initialise the AmBe generator
    void Initialise();
    //void ReadFromFile();
    
    // Set Particle Gun Properties of the neutron and gamma
    G4double NeutronEnergy();
    G4double GammaEnergy();
    void GenerateNG(G4Event* anEvent);

  private:
    G4ParticleGun*        myAmBeGun;
    G4SPSRandomGenerator* rGen;
    G4SPSEneDistribution* nEnergyDistGS;
    G4SPSEneDistribution* nEnergyDistFE;
    G4SPSEneDistribution* nEnergyDistSE;
    
    // Variables for the initialisation of AmBe generator parameters
    G4double nEnergy;
    G4double gEnergy;
    G4ThreeVector vtx;
    static G4int nGammaOutcomes;
    static G4double gammaProbabilities[3];
    static G4double gammaEnergies[3];
    static G4int pdgids[2];
    G4double time;
    G4ThreeVector dir;
    G4double epsilon;

    // Variables for reading in the file
    string wcsimdir;
    string gs_path;
    string fe_path;
    string se_path;
};

#endif
