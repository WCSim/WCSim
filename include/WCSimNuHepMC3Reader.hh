#ifndef WCSimNuHepMC3Reader_h
#define WCSimNuHepMC3Reader_h

#include <G4LorentzVector.hh>
#include <G4String.hh>

// #include "HepMC3/GenParticle.h"
#include "HepMC3/GenEvent.h"

namespace HepMC3 {
class Reader;
typedef std::shared_ptr<Reader> ReaderPtr;
} // namespace HepMC3

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
    std::shared_ptr<HepMC3::Reader> my_hepmc_reader;
    HepMC3::GenEvent event;
    int events_parsed;
};

#endif
