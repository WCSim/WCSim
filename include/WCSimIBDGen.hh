#ifndef WCSimIBDGen_h
#define WCSimIBDGen_h

#include "WCSimDetectorConstruction.hh"
#include <CLHEP/Units/PhysicalConstants.h>
#include <G4LorentzVector.hh>
#include <G4String.hh>
#include <G4ThreeVector.hh>

class WCSimIBDGen {

  public:
    WCSimIBDGen(WCSimDetectorConstruction *myDC);
    ~WCSimIBDGen();

    // Read the spectrum from database
    void ReadSpectrumFromDB(G4String spectrum_name, std::string model_name);

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

    // Current current structure calculation
    double CurrentCurrentStructure(double e_nu, double e_e);

    // dSigma by dt calculation
    double dSigmaBydt(double e_nu, double e_e);

    // dSigma by dE_e calculations
    double dSigmaBydEe(double e_nu, double e_e);

    // Radiative correction
    double RadiativeCorrection(double dSigma_by_dEe, double e_e);

    // dSigma by dCosTheta calculation
    double dSigmaBydCosTheta(double e_nu, double cos_theta);

    // Calculate positron energy
    double GetEe(double e_nu, double cos_theta);

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

    // Cross section constants
    double g_1_0 = -1.270;
    double M_V_squared = 710.0;
    double M_A_squared = 1030.0;
    double xi = 3.706;
    double M_av = 0.5 * (CLHEP::proton_mass_c2 + CLHEP::neutron_mass_c2);
    double G_f = 1.16639e-11;
    double cos_cabibbo = 0.9746;
    double alpha = 1 / 137.035989;
    double delta = CLHEP::neutron_mass_c2 - CLHEP::proton_mass_c2;
    double delta_cm =
        (pow(CLHEP::neutron_mass_c2, 2) - pow(CLHEP::proton_mass_c2, 2) - pow(CLHEP::electron_mass_c2, 2)) /
        (2 * CLHEP::proton_mass_c2);
};

#endif
