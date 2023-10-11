#ifndef WCSimIBDGen_h
#define WCSimIBDGen_h

#include "WCSimDetectorConstruction.hh"
#include <G4LorentzVector.hh>
#include <G4String.hh>
#include <G4ThreeVector.hh>

class WCSimIBDGen {

  public:
    WCSimIBDGen(WCSimDetectorConstruction *myDC);
    ~WCSimIBDGen();

    // Read the IBD spectrum from a file
    void ReadSpectrum(G4String spectrum_name);

    // Fit the spectrum
    double InterpolateSpectrum(std::vector<float> energy, std::vector<float> flux, float ene);

    // Generate a random position inside the detector geometry
    G4ThreeVector GenRandomPosition();

    // Generate event
    void GenEvent(G4ThreeVector &nu_dir, G4LorentzVector &neutrino, G4LorentzVector &positron,
                  G4LorentzVector &neutron);

    // Interaction generator
    void GenInteraction(float &E, float &CosTheta);

    // Cross section
    double CrossSection(double e_nu, double cos_theta);

    // Positron energy correction
    double PositronEnergy(double e_nu, double cos_theta);

  private:
    // Energy and flux vectors
    std::vector<float> energy;
    std::vector<float> flux;

    // Emin and Emax
    float e_min;
    float e_max;

    // Flux maximum
    float flux_max;

    // Detector
    WCSimDetectorConstruction *myDetector;
};

#endif
