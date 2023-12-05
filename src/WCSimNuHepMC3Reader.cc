#include "WCSimNuHepMC3Reader.hh"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle_fwd.h"
#include "HepMC3/Print.h"
#include <G4LorentzVector.hh>
#include <G4ios.hh>
#include <cstdlib>

WCSimNuHepMC3Reader::WCSimNuHepMC3Reader(G4String filename) {
    G4cout << "NuHepMC3Reader: [INFO] input file called " << filename << G4endl;

    // Get reader object for the input file
    hepmc_reader = HepMC3::deduce_reader(filename);

    // Set events parsed to 0
    events_parsed = 0;
}

WCSimNuHepMC3Reader::~WCSimNuHepMC3Reader() {
    // Close the input file
    hepmc_reader->close();
}

// Read the next event from the input file
bool WCSimNuHepMC3Reader::ReadEvent() {
    // Read event
    hepmc_reader->read_event(event);

    // Check if the reading failed
    if (hepmc_reader->failed()) {
        G4cout << "NuHepMC3Reader: \033[31m[ERROR]\033[0m Failed to read event " << events_parsed << G4endl;
        return false;
    }

    // If this is the first event, print things
    if (events_parsed == 0) {
        G4cout << "NuHepMC3Reader: [INFO] First event: " << G4endl;

        // Print event listing and content
        HepMC3::Print::listing(event);
        HepMC3::Print::content(event);
    }

    if (!GetParticles()) {
        return false;
    };

    events_parsed++;
    return true;
}

// Get particles
bool WCSimNuHepMC3Reader::GetParticles() {
    // Clear the vector of particles
    particles.clear();

    particles = event.particles();

    if (particles.size() == 0) {
        G4cout << "NuHepMC3Reader: \033[31m[ERROR]\033[0m No particles in event " << events_parsed << G4endl;
        return false;
    }

    return true;
}

// Return Lorentz vector of a specific particle
G4LorentzVector WCSimNuHepMC3Reader::GetLorentzVector(int particle_index) {
    // Get particle
    HepMC3::GenParticlePtr particle = particles[particle_index];

    // Get momentum
    HepMC3::FourVector momentum = particle->momentum();

    // Return Lorentz vector
    return G4LorentzVector(momentum.px(), momentum.py(), momentum.pz(), momentum.e());
}

// Loop over particles in event and return a vector of momenta and PDGs.
// This is used to fill the truth tree.
std::vector<std::pair<G4LorentzVector, int>> WCSimNuHepMC3Reader::GetMomentaAndPDGs() {
    // Vector of momenta and PDGs
    std::vector<std::pair<G4LorentzVector, int>> momenta_and_pdgs;

    // Loop over particles
    for (size_t i = 0; i < particles.size(); i++) {
        // Get Lorentz vector
        G4LorentzVector lorentz_vec = GetLorentzVector(i);

        // Add to vector
        momenta_and_pdgs.push_back(std::make_pair(lorentz_vec, particles[i]->pid()));
    }

    return momenta_and_pdgs;
}
