#include "WCSimNuHepMC3Reader.hh"
#include <G4ios.hh>

#include "HepMC3/HepMC3.h"
#include "HepMC3/Reader.h"

WCSimNuHepMC3Reader::WCSimNuHepMC3Reader(G4String filename) {
    G4cout << "WCSimNuHepMC3Reader::WCSimNuHepMC3Reader(G4String filename) called " << filename << G4endl;
}
