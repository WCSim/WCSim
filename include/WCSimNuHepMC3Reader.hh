#include <G4String.hh>

#include "HepMC3/ReaderFactory.h"

class WCSimNuHepMC3Reader {
  public:
    WCSimNuHepMC3Reader(G4String filename);
    ~WCSimNuHepMC3Reader();

    int ReadEvent(HepMC3::GenEvent &evt);

  private:
    std::shared_ptr<HepMC3::Reader> hepmc_reader;
};
