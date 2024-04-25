#include "WCSimNuHepMC3Reader.hh"

#include "HepMC3/FourVector.h"
#include "HepMC3/Print.h"
#include "HepMC3/ReaderFactory.h"

#include <CLHEP/Random/Random.h>
#include <G4Types.hh>
#include <G4ios.hh>
#include <Randomize.hh>
#include <cmath>

WCSimNuHepMC3Reader::WCSimNuHepMC3Reader(std::string filename, WCSimDetectorConstruction *myDC)
    : myDetector(myDC) {
    G4cout << "NuHepMC3Reader: [INFO] input file called " << filename << G4endl;

    // Get reader object for the input file
    my_hepmc_reader = HepMC3::deduce_reader(filename);

    G4cout << "NuHepMC3Reader: [INFO] reader object created" << G4endl;

    // Check if the reader is valid
    if (!my_hepmc_reader) {
        G4cerr << "NuHepMC3Reader: \033[31m[ERROR]\033[0m Failed to determine input type for HepMC3 file: "
               << filename << G4endl;
        exit(1);
    }

    // Set events parsed to 0
    events_parsed = 0;
}

WCSimNuHepMC3Reader::~WCSimNuHepMC3Reader() {}

/// Read the next event from the input file
bool WCSimNuHepMC3Reader::ReadEvent(bool positionFlag) {
    // Read event
    my_hepmc_reader->read_event(event);

    // Check if the reading failed
    if (my_hepmc_reader->failed()) {
        G4cout << "NuHepMC3Reader: \033[31m[ERROR]\033[0m Failed to read event " << events_parsed << G4endl;
        // return false;
    }

    // Set the positon of the vertex to a random position in the detector
    if (positionFlag) {
        HepMC3::FourVector position = GenRandomPosition();
        for (HepMC3::GenParticlePtr part : event.particles()) {
            if (part->status() == 1) {
                part->production_vertex()->set_position(position);
            }
        }
    }

    if (!GetParticles()) {
        return false;
    };

    events_parsed++;
    return true;
}

/// Get particles
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

/// Return Lorentz vector of a specific particle
G4LorentzVector WCSimNuHepMC3Reader::GetLorentzVector(int particle_index) {
    // Get particle
    HepMC3::GenParticlePtr particle = particles[particle_index];

    // Get momentum
    HepMC3::FourVector momentum = particle->momentum();

    // Return Lorentz vector
    return G4LorentzVector(momentum.px(), momentum.py(), momentum.pz(), momentum.e());
}

/// Loop over particles in event and return a vector of momenta and PDGs.
/// This is used to fill the truth tree.
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

HepMC3::FourVector WCSimNuHepMC3Reader::GenRandomPosition() {
    // Generate a random neutrino position

    // Initialise x and y values to be outside of the detector volume
    double x_nu = 10.0 * myDetector->GetGeo_Dm(3);
    double y_nu = 10.0 * myDetector->GetGeo_Dm(3);

    // Generate random x and y values until they are within the detector volume
    while (sqrt(x_nu * x_nu + y_nu * y_nu) > myDetector->GetGeo_Dm(3)) {
        x_nu = myDetector->GetGeo_Dm(0) * (-0.5 + G4UniformRand());
        y_nu = myDetector->GetGeo_Dm(1) * (-0.5 + G4UniformRand());
    }

    // Generate a random z value within the detector volume
    double z_nu = myDetector->GetGeo_Dm(2) * (-0.5 + G4UniformRand());

    HepMC3::FourVector position(x_nu, y_nu, z_nu, 0.0);
    return position;
}
