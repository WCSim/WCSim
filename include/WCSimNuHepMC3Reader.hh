#include <G4LorentzVector.hh>
#include <G4String.hh>

#include "HepMC3/GenParticle_fwd.h"
#include "HepMC3/ReaderFactory.h"

class WCSimNuHepMC3Reader {
  public:
    WCSimNuHepMC3Reader(G4String filename);
    ~WCSimNuHepMC3Reader();

    // Read the next event from the input file
    bool ReadEvent();

    // Vector of particles in event
    std::vector<HepMC3::GenParticlePtr> particles;

    // Populate particles using event.particles()
    bool GetParticles();

    // Return Lorentz vector of a specific particle
    G4LorentzVector GetLorentzVector(int particle_index);

    // Return a vector of momenta and PDGs
    std::vector<std::pair<G4LorentzVector, int>> GetMomentaAndPDGs();

  private:
    // Event reader
    std::shared_ptr<HepMC3::Reader> hepmc_reader;
    HepMC3::GenEvent event;
    int events_parsed;
};
