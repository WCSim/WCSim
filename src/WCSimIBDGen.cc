#include "WCSimIBDGen.hh"
#include "WCSimDetectorConstruction.hh"
#include "WCSimPrimaryGeneratorAction.hh"
#include "WCSimPrimaryGeneratorMessenger.hh"

#include "json.hpp"

#include <CLHEP/Units/PhysicalConstants.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include <G4ThreeVector.hh>
#include <G4Types.hh>
#include <G4ios.hh>
#include <algorithm>
#include <fstream>

// Set json to nlohmann::json
using json = nlohmann::json;

WCSimIBDGen::WCSimIBDGen(WCSimDetectorConstruction *myDC) : myDetector(myDC) {}

WCSimIBDGen::~WCSimIBDGen() {
    // Delete things here
}

void WCSimIBDGen::ReadSpectrumFromDB(G4String spectrum_database, std::string model_name) {
    // Open the database
    std::ifstream spectrum_json(spectrum_database);

    // Check that the database has been found
    if (!spectrum_json.is_open()) {
        G4cerr << "IBDGen: [ERROR] spectrum database " << spectrum_database << " not found." << G4endl;
        exit(-1);
    }

    G4cout << "IBDGen: [INFO] spectrum database " << spectrum_database << " found." << G4endl;

    std::stringstream buffer;
    buffer << spectrum_json.rdbuf();

    json data = json::parse(buffer.str());

    // Loop over the json
    for (const auto &model : data["models"]) {
        if (model["name"].get<string>() == model_name) {
            G4cout << "IBDGen: [INFO] using model " << model_name << G4endl;

            energy = model["energy"].get<std::vector<float>>();
            flux = model["flux"].get<std::vector<float>>();

            // If the energy vector is not monotonically increasing wrt elements then exit
            if (!std::is_sorted(energy.begin(), energy.end())) {
                G4cerr << "IBDGen: \033[31m[ERROR]\033[0m energy vector is not monotonically increasing. Check "
                       << spectrum_database << G4endl;
                exit(-1);
            }

            G4cout << "IBDGen: [INFO] spectrum read from database: " << spectrum_database << G4endl;

            // Check if the flux vector and energy vector have the same size
            if (energy.size() != flux.size()) {
                G4cerr << "IBDGen: \033[31m[ERROR]\033[0m energy and flux vectors have different sizes. Energy: "
                       << energy.size() << ", flux: " << flux.size() << ". Check " << spectrum_database << G4endl;
                exit(-1);
            }
            // Set the minimum and maximum energy
            e_min = energy.front();
            e_max = energy.back();

            // Set the maximum flux
            flux_max = *std::max_element(flux.begin(), flux.end());

            return;
        }
    }

    // If we get here, the model was not found
    G4cerr << "IBDGen: \033[31m[ERROR]\033[0m model \033[31m" << model_name << "\033[0m not found in database "
           << spectrum_database << G4endl;
    exit(-1);
    return;
}

double WCSimIBDGen::InterpolateSpectrum(std::vector<float> ener_vec, std::vector<float> flux_vec, float ene) {

    // Interpolate the spectrum at the energies in ener_vec
    // The spectrum is given by the vectors ener_vec and flux_vec

    // Loop over the energies in energy
    for (size_t i = 1; i < ener_vec.size(); i++) {
        if (ene <= ener_vec[i]) {
            // Perform linear interpolation
            double e1 = ener_vec[i - 1];
            double e2 = ener_vec[i];
            double f1 = flux_vec[i - 1];
            double f2 = flux_vec[i];

            double interpolated_value = f1 + (f2 - f1) * (ene - e1) / (e2 - e1);
            return interpolated_value;
        }
    }

    // If energy is larger than the maximum energy in the spectrum, return the final flux value
    return flux_vec.back();
}

G4ThreeVector WCSimIBDGen::GenRandomPosition() {
    // Generate random neutrino position
    // Pick random position in the inner detector

    // Initialise x and y values to be outside of the detector (10 * the radius of the detector for both should do)
    double x_nu = 10.0 * myDetector->GetGeo_Dm(3);
    double y_nu = 10.0 * myDetector->GetGeo_Dm(3);

    // While the x and y positions correspond to a point outside of the inner detector (i.e. have a radial position
    // further from the z-axis than the inner detector's radius)
    while (sqrt(x_nu * x_nu + y_nu * y_nu) > myDetector->GetGeo_Dm(3)) {
        // Generate a random x and y value
        x_nu = myDetector->GetGeo_Dm(0) * (-1.0 + 2.0 * G4UniformRand());
        y_nu = myDetector->GetGeo_Dm(1) * (-1.0 + 2.0 * G4UniformRand());
    }

    // Generate random z position between -1/2 ID height and +1/2 ID height
    double z_nu = myDetector->GetGeo_Dm(2) * (-1.0 + 2.0 * G4UniformRand());

    G4ThreeVector nu_pos;

    // Set the nu_pos vector to the values generated above
    nu_pos.setX(x_nu);
    nu_pos.setY(y_nu);
    nu_pos.setZ(z_nu);

    return nu_pos;
}

void WCSimIBDGen::GenEvent(G4ThreeVector &nu_dir, G4LorentzVector &neutrino, G4LorentzVector &positron,
                           G4LorentzVector &neutron) {

    // Generate random neutrino direction
    // Pick isotropic direction
    double theta_nu = acos(2.0 * G4UniformRand() - 1.0);
    double phi_nu = 2.0 * G4UniformRand() * CLHEP::pi;
    nu_dir.setRThetaPhi(1.0, theta_nu, phi_nu);

    // Pick energy of neutrino and relative direction of positron
    float e_nu, cos_theta;
    GenInteraction(e_nu, cos_theta);

    // First order correction to positron quantities
    // for finite nucleon mass
    double e1 = PositronEnergy(e_nu, cos_theta);

    double p1 = sqrt(e1 * e1 - CLHEP::electron_mass_c2 * CLHEP::electron_mass_c2);

    // Compute neutrino 4-momentum
    neutrino.setVect(nu_dir * e_nu);
    neutrino.setE(e_nu);

    // Compute positron 4-momentum
    G4ThreeVector pos_momentum(p1 * nu_dir);

    // Rotation from nu direction to pos direction.
    double theta = acos(cos_theta);
    double phi = 2 * CLHEP::pi * G4UniformRand(); // Random phi
    G4ThreeVector rotation_axis = nu_dir.orthogonal();
    rotation_axis.rotate(phi, nu_dir);
    pos_momentum.rotate(theta, rotation_axis);

    positron.setVect(pos_momentum);
    positron.setE(e1);

    // Compute neutron 4-momentum
    neutron.setVect(neutrino.vect() - positron.vect());
    neutron.setE(sqrt(neutron.vect().mag2() + CLHEP::neutron_mass_c2 * CLHEP::neutron_mass_c2));
}

void WCSimIBDGen::GenInteraction(float &rand_ene, float &rand_cos_theta) {
    G4bool passed = false;

    G4double xs_max = CrossSection(e_max, -1.0);

    while (!passed) {
        // Pick energy and directory uniformly
        rand_ene = e_min + (e_max - e_min) * G4UniformRand();
        rand_cos_theta = -1.0 + 2.0 * G4UniformRand();

        // Weight events by spectrum
        G4double xs_test = xs_max * flux_max * G4UniformRand();

        // Cross section
        G4double xs_weight = CrossSection(rand_ene, rand_cos_theta);

        // Flux at rand_ene
        G4double flux_weight = InterpolateSpectrum(energy, flux, rand_ene);

        passed = (xs_test < xs_weight * flux_weight);
    }
}

double WCSimIBDGen::CrossSection(double e_nu, double cos_theta) {

    const double cos_theta_c = (0.9741 + 0.9756) / 2.0;

    // Radiative correction constant
    const double rad_cor = 0.024;

    const double DELTA = CLHEP::neutron_mass_c2 - CLHEP::proton_mass_c2;

    // Neutrino energy threshold for inverse beta decay
    const double e_nu_min = ((CLHEP::proton_mass_c2 + DELTA + CLHEP::electron_mass_c2) *
                                 (CLHEP::proton_mass_c2 + CLHEP::electron_mass_c2 + DELTA) -
                             CLHEP::proton_mass_c2 * CLHEP::proton_mass_c2) /
                            2 / CLHEP::proton_mass_c2;

    if (e_nu < e_nu_min) {
        return 0.0;
    }

    const double GFERMI = 1.16639e-11;

    const double sigma_0 = GFERMI * GFERMI * cos_theta_c * cos_theta_c / CLHEP::pi * (1 + rad_cor);

    // Couplings
    const double f = 1.00;
    const double f2 = 3.706;
    const double g = 1.26;

    // Order 0 terms
    double e0 = e_nu - DELTA;
    if (e0 < CLHEP::electron_mass_c2) {
        e0 = CLHEP::electron_mass_c2;
    }
    double p0 = sqrt(e0 * e0 - CLHEP::electron_mass_c2 * CLHEP::electron_mass_c2);
    double v0 = p0 / e0;

    // Order 1 terms
    const double y_squared = (DELTA * DELTA - CLHEP::electron_mass_c2 * CLHEP::electron_mass_c2) / 2;
    double e1 = e0 * (1 - e_nu / CLHEP::proton_mass_c2 * (1 - v0 * cos_theta)) - y_squared / CLHEP::proton_mass_c2;
    if (e1 < CLHEP::electron_mass_c2) {
        e1 = CLHEP::electron_mass_c2;
    }
    double p1 = sqrt(e1 * e1 - CLHEP::electron_mass_c2 * CLHEP::electron_mass_c2);
    double v1 = p1 / e1;

    double gamma =
        2 * (f + f2) * g *
            ((2 * e0 + DELTA) * (1 - v0 * cos_theta) - CLHEP::electron_mass_c2 * CLHEP::electron_mass_c2 / e0) +
        (f * f + g * g) * (DELTA * (1 + v0 * cos_theta) + CLHEP::electron_mass_c2 * CLHEP::electron_mass_c2 / e0) +
        (f * f + 3 * g * g) * ((e0 + DELTA) * (1 - cos_theta / v0) - DELTA) +
        (f * f - g * g) * ((e0 + DELTA) * (1 - cos_theta / v0) - DELTA) * v0 * cos_theta;

    double cross_section = ((f * f + 3 * g * g) + (f * f - g * g) * v1 * cos_theta) * e1 * p1 -
                           gamma / CLHEP::proton_mass_c2 * e0 * p0;

    cross_section *= sigma_0 / 2;

    // Convert from MeV^{-2} to mm^2 (native units for GEANT4)
    cross_section *= CLHEP::hbarc * CLHEP::hbarc;

    return cross_section;
}

double WCSimIBDGen::PositronEnergy(double e_nu, double cos_theta) {
    // Returns positron energy with first order corrections
    // Zero'th order approximation of positron quantities - infinite nucleon mass
    double e0 = e_nu - (CLHEP::neutron_mass_c2 - CLHEP::proton_mass_c2);
    double p0 = sqrt(e0 * e0 - CLHEP::electron_mass_c2 * CLHEP::electron_mass_c2);
    double v0 = p0 / e0;

    // First order correction to positron quantities -- see page 3 astro-ph/0302055
    const double y_squared =
        (CLHEP::neutron_mass_c2 - CLHEP::proton_mass_c2) * (CLHEP::neutron_mass_c2 - CLHEP::proton_mass_c2) / 2;
    double e1 = e0 * (1 - e_nu / CLHEP::proton_mass_c2 * (1 - v0 * cos_theta)) - y_squared / CLHEP::proton_mass_c2;
    if (e1 < CLHEP::electron_mass_c2) {
        e1 = CLHEP::electron_mass_c2;
    }
    return e1;
}
