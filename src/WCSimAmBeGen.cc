#include "WCSimAmBeGen.hh"
#include "WCSimPrimaryGeneratorMessenger.hh"
#include "WCSimPrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4PhysicalConstants.hh"
#include "G4RandomDirection.hh"
#include "G4SystemOfUnits.hh"
#include "G4SPSEneDistribution.hh"
#include "G4SPSRandomGenerator.hh"
#include "G4ParticleTable.hh"
#include "G4Neutron.hh"
#include "G4Gamma.hh"

#include <random>

using namespace std;

WCSimAmBeGen::WCSimAmBeGen(){
  // Initialise
  this->Initialise();

}

WCSimAmBeGen::~WCSimAmBeGen(){
  
  // This needed to be deleted
  delete myAmBeGun;
  delete nEnergyDist;
  delete rGen;
}

void WCSimAmBeGen::Initialise(){
    nEnergyDist = new G4SPSEneDistribution();
    rGen        = new G4SPSRandomGenerator();
    myAmBeGun   = new G4ParticleGun();
}

G4double WCSimAmBeGen::NeutronEnergy(){
    // Set the neutron energy distribution from a file
    nEnergyDist->SetEnergyDisType("Arb");
    nEnergyDist->ArbEnergyHistoFile("data/resampled_nSpectrum.txt");
    nEnergyDist->SetBiasRndm(rGen);
    nEnergyDist->ArbInterpolate("Lin");

    // Obtain the definition of the neutron and sample an energy from the distribution
    nEnergy = nEnergyDist->GenerateOne(G4Neutron::Definition());

    return nEnergy;
}

G4double WCSimAmBeGen::GammaEnergy(){
    // Define the correspondent probabilities for every possible gamma scenario
    probabilities = {0.26, 0.65, 0.08};
    energies      = {0.0, 4.4, 7.7};

    // Generate a weighted random number so we can select the gamma energy
    random_device rd;
    mt19937 gen(rd());
    discrete_distribution<int> dist(probabilities.begin(), probabilities.end());
    int index = dist(gen);
    gEnergy = energies[index];
    
    return gEnergy;
}

void WCSimAmBeGen::GenerateNG(G4Event* anEvent){
    // Define PDG IDs, energies and desired particle names
    G4int pdgids[] = {2112, 22};
      
    for (G4int i=0; i<2; i++){
      // Create a vertex for the particle following the index of the loop
      G4int pdgid = pdgids[i];
      vtx = G4ThreeVector(0., 0., 0.);
      time = 0.;

      // Configure particle's properties in particleGun
      if (pdgid == 22) {
        myAmBeGun->SetParticleDefinition(G4Gamma::Definition());
        myAmBeGun->SetParticleEnergy(GammaEnergy());
      }
      else {
        myAmBeGun->SetParticleDefinition(G4Neutron::Definition());
        myAmBeGun->SetParticleEnergy(NeutronEnergy());
      }

      dir = G4RandomDirection();

      // Configure the final properties of the particle
      myAmBeGun->SetParticlePosition(vtx);
      myAmBeGun->SetParticleTime(time);
      myAmBeGun->SetParticleMomentumDirection(dir);

      // Generate the primary vertex for the particle
      myAmBeGun->GeneratePrimaryVertex(anEvent);
      }
}
