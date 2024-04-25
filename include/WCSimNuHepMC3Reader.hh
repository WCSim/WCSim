#ifndef WCSimNuHepMC3Reader_h
#define WCSimNuHepMC3Reader_h

#include <G4LorentzVector.hh>
#include <G4String.hh>
#include <memory>

// #include "HepMC3/GenParticle.h"
#include "HepMC3/FourVector.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/ReaderAscii.h"
#include "WCSimDetectorConstruction.hh"

namespace HepMC3 {
class Reader;
typedef std::shared_ptr<Reader> ReaderPtr;
} // namespace HepMC3

class WCSimNuHepMC3Reader {
  public:
    WCSimNuHepMC3Reader(std::string filename, WCSimDetectorConstruction *myDC);
    ~WCSimNuHepMC3Reader();

    /// Read the next event from the input file
    bool ReadEvent(bool positionFlag);

    /// Vector of particles in event
    std::vector<HepMC3::GenParticlePtr> particles;

    /// Populate particles using event.particles()
    bool GetParticles();

    /// Return Lorentz vector of a specific particle
    G4LorentzVector GetLorentzVector(int particle_index);

    /// Generate a random position in the detector construction
    HepMC3::FourVector GenRandomPosition();

    /// Generate a random direction
    HepMC3::FourVector GenRandomDirection();

    /// Rotate the event
    void RotateEvent();

    /// Return a vector of momenta and PDGs
    std::vector<std::pair<G4LorentzVector, int>> GetMomentaAndPDGs();

    /// void GenDirection(double energy);

    /// Reader object
    std::shared_ptr<HepMC3::Reader> my_hepmc_reader;

    /// Event object
    HepMC3::GenEvent event;

  private:
    /// Event reader
    int events_parsed = 0;

    /// Detector construction
    WCSimDetectorConstruction *myDetector;
};

#endif
