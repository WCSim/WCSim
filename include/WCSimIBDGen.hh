#ifndef WCSimIBDGen_h
#define WCSimIBDGen_h

#include "WCSimDetectorConstruction.hh"
#include <CLHEP/Units/PhysicalConstants.h>
#include <G4LorentzVector.hh>
#include <G4String.hh>
#include <G4ThreeVector.hh>

class WCSimIBDGen {

  public:
    WCSimIBDGen(G4String spectrum_database, G4String ibd_model, WCSimDetectorConstruction *myDC);
    ~WCSimIBDGen();

    // Read the spectrum from database
    void ReadSpectrumFromDB(G4String spectrum_name, std::string model_name);

    // Fit the spectrum
    double InterpolateSpectrum(std::vector<float> energy, std::vector<float> flux, float ene);

    // Get the maximum value of the cross section multiplied by the flux for the given model
    double MaxXSecFlux();

    // Generate a random position inside the detector geometry
    G4ThreeVector GenRandomPosition();

    // Generate event
    void GenEvent(G4ThreeVector &nu_dir, G4LorentzVector &neutrino, G4LorentzVector &positron,
                  G4LorentzVector &neutron);

    // Interaction generator
    void GenInteraction(float &E, float &CosTheta);

    // Current current structure calculation
    double MatrixElement(double e_nu, double e_e);

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

    // Max of cross section * flux
    double xsec_flux_max;

    // TODO: REMOVE THIS BEFORE PUSHING
    int n_tries = 0;

    // Flux maximum
    float flux_max;

    // Detector
    WCSimDetectorConstruction *myDetector;

    // Cross section constants
    // See equation 2. Note, the paper refers to M_av as "M". Labelled here as M_av (average mass of proton and
    // neutron) to avoid confusion with other Ms. delta here refers to uppercase delta in the paper.
    double M_av = 0.5 * (CLHEP::proton_mass_c2 + CLHEP::neutron_mass_c2);
    double delta = CLHEP::neutron_mass_c2 - CLHEP::proton_mass_c2;
    // See below equation 3. Slight mismatch in value of G_f here to match the one used by SNTools.
    double G_f = 1.16639e-11;
    double cos_cabibbo = 0.9746;
    // See below equation 7
    double g_1_0 = -1.270;
    double M_V_squared = 710.0;
    double M_A_squared = 1030.0;
    double xi = 3.706;
    // Fine structure constant
    double alpha = 1 / 137.035989;
    // Named delta_cm to avoid confusion with delta above. This is referred to as lowercase delta in the paper. See
    // equation 12.
    double delta_cm =
        (pow(CLHEP::neutron_mass_c2, 2) - pow(CLHEP::proton_mass_c2, 2) - pow(CLHEP::electron_mass_c2, 2)) /
        (2 * CLHEP::proton_mass_c2);
};

#endif
