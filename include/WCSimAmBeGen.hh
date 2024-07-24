#ifndef WCSimAmBeGen_h
#define WCSimAmBeGen_h

#include "WCSimDetectorMessenger.hh"

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
    WCSimAmBeGen(WCSimDetectorConstruction* detector);
    ~WCSimAmBeGen();

    // Initialise the AmBe generator
    void Initialise();
    
    // Set Particle Gun Properties of the neutron and gamma
    G4double NeutronEnergy();
    G4double GammaEnergy();
    void GenerateNG(G4Event* anEvent);
    void SetPositionBGOGeometry(G4double X, G4double Y, G4double Z) { BGOX=X, BGOY=Y, BGOZ=Z; }
    G4ThreeVector GetPositionBGOGeometry() const { return G4ThreeVector(BGOX, BGOY, BGOZ); }

  private:
    G4ParticleGun*        myAmBeGun;
    G4SPSRandomGenerator* rGen;
    G4SPSEneDistribution* nEnergyDistGS;
    G4SPSEneDistribution* nEnergyDistFE;
    G4SPSEneDistribution* nEnergyDistSE;
    WCSimDetectorConstruction *myDetector;
    
    // Variables for the initialisation of AmBe generator parameters
    G4double gEnergy;
    G4ThreeVector pos;
    static G4int nGammaOutcomes;
    static G4double gammaProbabilities[3];
    static G4double gammaEnergies[3];
    static G4int pdgids[2];
    G4double time;
    G4ThreeVector dir;
    G4double epsilon;
    G4double BGOX, BGOY, BGOZ;

    // Variables for reading in the file
    /// Points to $WCSIM_BUILD_DIR/data/
    string wcsimdir;
    string gs_path;
    string fe_path;
    string se_path;
};

#endif
