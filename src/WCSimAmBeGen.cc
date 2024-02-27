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

G4double WCSimAmBeGen::gammaProbabilities[3] = {0.26, 0.65, 0.08};
G4double WCSimAmBeGen::gammaEnergies[3] = {0.0, 4.4, 7.7};
G4int    WCSimAmBeGen::pdgids[2] = {2112, 22};

WCSimAmBeGen::WCSimAmBeGen(){
  // Initialise
  this->Initialise();
  wcsimdir = string(getenv("WCSIMDIR"))+"/data/";

  gs_path = wcsimdir + "ground_state_spectrum.txt";
  fe_path = wcsimdir + "first_excited_spectrum.txt";
  se_path = wcsimdir + "second_excited_spectrum.txt";
}

WCSimAmBeGen::~WCSimAmBeGen(){
  
  // This needed to be deleted
  delete myAmBeGun;
  delete rGen;
  delete nEnergyDistGS;
  delete nEnergyDistFE;
  delete nEnergyDistSE;
}

void WCSimAmBeGen::Initialise(){
    nEnergyDistGS = new G4SPSEneDistribution();
    nEnergyDistFE = new G4SPSEneDistribution();
    nEnergyDistSE = new G4SPSEneDistribution();

    // Configure the energy distributions for each state
    nEnergyDistGS->SetEnergyDisType("Arb");
    nEnergyDistGS->ArbEnergyHistoFile(gs_path);
    nEnergyDistGS->SetBiasRndm(rGen);
    nEnergyDistGS->ArbInterpolate("Lin");

    nEnergyDistFE->SetEnergyDisType("Arb");
    nEnergyDistFE->ArbEnergyHistoFile(fe_path);
    nEnergyDistFE->SetBiasRndm(rGen);
    nEnergyDistFE->ArbInterpolate("Lin");

    nEnergyDistSE->SetEnergyDisType("Arb");
    nEnergyDistSE->ArbEnergyHistoFile(se_path);
    nEnergyDistSE->SetBiasRndm(rGen);
    nEnergyDistSE->ArbInterpolate("Lin"); 

    vtx = G4ThreeVector(0., 0., 0.);
    time = 0.;
    epsilon = 1e-6;
}

G4double WCSimAmBeGen::GammaEnergy(){
    
    // Initialise Geant4 random number generator
    G4Random::setTheEngine(new CLHEP::RanecuEngine);

    // Generation of the random number
    G4double random_number = G4UniformRand();

    // Select the energy taking into account the probabilities
    G4double cumulative_probability = 0.0;
    G4int selected_energy_index = -1;

    for (G4int j = 0; j < sizeof(gammaProbabilities) / sizeof(gammaProbabilities[0]); ++j) {
        cumulative_probability += gammaProbabilities[j];
        if (random_number <= cumulative_probability) {
            selected_energy_index = j;
            break;
        }
    }

    gEnergy = gammaEnergies[selected_energy_index];

    return gEnergy; 
}

G4double WCSimAmBeGen::NeutronEnergy(G4double gEnergy){
    
    // Depending on the gEnergy, we load the correspondent neutron energy spectrum 
    if (std::abs(gEnergy - 0.0) < epsilon) { 
      nEnergy = nEnergyDistGS->GenerateOne(G4Neutron::Definition());
    }
    else if (std::abs(gEnergy - 4.4) < epsilon) {
      nEnergy = nEnergyDistFE->GenerateOne(G4Neutron::Definition());
    }
    else {
      nEnergy = nEnergyDistSE->GenerateOne(G4Neutron::Definition());
    }

    return nEnergy;
}

void WCSimAmBeGen::GenerateNG(G4Event* anEvent){
    for (G4int i=0; i<2; i++){
      // Create a vertex for the particle following the index of the loop
      G4int pdgid = pdgids[i];
      
      // Configure particle's properties in particleGun
      if (pdgid == 22) {
        myAmBeGun->SetParticleDefinition(G4Gamma::Definition());
        myAmBeGun->SetParticleEnergy(GammaEnergy());
      }
      else {
        myAmBeGun->SetParticleDefinition(G4Neutron::Definition());
        myAmBeGun->SetParticleEnergy(NeutronEnergy(GammaEnergy()));
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
