#include "WCSimNuHepMC3Reader.hh"
#include <G4ios.hh>
#include <cstdlib>

WCSimNuHepMC3Reader::WCSimNuHepMC3Reader(G4String filename) {
    G4cout << "WCSimNuHepMC3Reader::WCSimNuHepMC3Reader(G4String filename) called " << filename << G4endl;

    hepmc_reader = HepMC3::deduce_reader(filename);
    if (!hepmc_reader) {
        G4cout << "NuHepMC3Reader: \033[31m[ERROR]\033[0m Failed to determine input type for HepMC3 file: "
               << filename << G4endl;
        exit(-1);
    }
}

int WCSimNuHepMC3Reader::ReadEvent(HepMC3::GenEvent &event) {
    hepmc_reader->read_event(event);
    return 1;
}
