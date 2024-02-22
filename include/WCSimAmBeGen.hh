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
    G4double NeutronEnergy(G4double gEnergy);
    G4double GammaEnergy();
    void GenerateNG(G4Event* anEvent);

  private:
    G4ParticleGun*        myAmBeGun;
    G4SPSEneDistribution* nEnergyDist;
    G4SPSRandomGenerator* rGen;
    
    // Variables for the initialisation of AmBe generator parameters
    G4double nEnergy;
    G4double gEnergy;
    G4ThreeVector vtx;
    G4double time;
    G4ThreeVector dir;

    vector<double> probabilities;
    vector<double> energies;
    
    //vector<double> energies_1;
    //vector<double> energies_2;

    // Variables for reading in the file
    string wcsimdir;
};

#endif
