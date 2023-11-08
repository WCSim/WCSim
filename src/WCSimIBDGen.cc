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
#include <cmath>
#include <fstream>

// Set json to nlohmann::json
using json = nlohmann::json;

WCSimIBDGen::WCSimIBDGen(G4String spectrum_database, G4String ibd_model, WCSimDetectorConstruction *myDC)
    : myDetector(myDC) {
    G4cout << "IBDGen: [INFO] spectrum database " << spectrum_database << " found." << G4endl;
    G4cout << "IBDGen: [INFO] using model " << ibd_model << G4endl;
    ReadSpectrumFromDB(spectrum_database, ibd_model);
    G4cout << "IBDGen: [INFO] Initialised IBDGen" << G4endl;
}

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

double WCSimIBDGen::MaxXSecFlux() {
    // Loops over the energies in the flux model and calculates the maximum value of dSigma/dCosTheta * flux.
    double xs_flux_max = 0.0;

    for (float ene : energy) {
        // Calculate the cross section for forward scattered positrons and backward scattered positrons. One of
        // these values will always represent the maximum cross section across the energy range (forward at high
        // energies and backward at low energies)
        double x_sec_forward = dSigmaBydCosTheta(ene, 1.0);
        double x_sec_backward = dSigmaBydCosTheta(ene, -1.0);

        // Get the flux at ene
        double flux_at_ene = InterpolateSpectrum(energy, flux, ene);

        // Calculate the max cross section * flux
        if (std::max(x_sec_forward, x_sec_backward) * flux_at_ene > xs_flux_max) {
            xs_flux_max = std::max(x_sec_forward, x_sec_backward) * flux_at_ene;
        }
    }

    // Check if xs_flux_max is still 0.0. If it is then something has gone wrong.
    if (xs_flux_max == 0.0) {
        G4cerr << "IBDGen: \033[31m[ERROR]\033[0m xs_flux_max is 0.0. Check the flux database." << G4endl;
        exit(-1);
    }

    return xs_flux_max;
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
        x_nu = myDetector->GetGeo_Dm(0) * (-0.5 + G4UniformRand());
        y_nu = myDetector->GetGeo_Dm(1) * (-0.5 + G4UniformRand());
    }

    // Generate random z position between -1/2 ID height and +1/2 ID height
    double z_nu = myDetector->GetGeo_Dm(2) * (-0.5 + G4UniformRand());

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
    double e1 = GetEe(e_nu, cos_theta);

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

    G4double xs_max = dSigmaBydCosTheta(e_max, -1.0);

    // Rejection sampling
    while (!passed) {
        // Pick energy and direction uniformly
        rand_ene = e_min + (e_max - e_min) * G4UniformRand();
        rand_cos_theta = -1.0 + 2.0 * G4UniformRand();

        // Passing threshold. Must be generated once per try or there is the possibility that the event will never
        // pass
        G4double xs_test = xs_max * flux_max * G4UniformRand();

        // Cross section
        G4double xs_weight = dSigmaBydCosTheta(rand_ene, rand_cos_theta);

        // Flux at rand_ene
        G4double flux_weight = InterpolateSpectrum(energy, flux, rand_ene);

        passed = (xs_test < xs_weight * flux_weight);

        n_tries++;
    }

    G4cout << "IBDGen: [INFO] number of iterations before energy and direction accepted: " << n_tries << G4endl;
}

// Cross section calculations below here were taken from astro-ph/0302055 available at
// https://arxiv.org/abs/astro-ph/0302055

double WCSimIBDGen::MatrixElement(double e_nu, double e_e) {
    // Calculates and return the value of the absolute matrix element squared

    // Calculate the Mandelstam variables. See above equation 11
    double s_minus_u = 2 * CLHEP::proton_mass_c2 * (e_nu + e_e) - pow(CLHEP::electron_mass_c2, 2.);
    double t =
        pow(CLHEP::neutron_mass_c2, 2) - pow(CLHEP::proton_mass_c2, 2) - 2 * CLHEP::proton_mass_c2 * (e_nu - e_e);

    // Calculate f1, f2, g1 and g2. See equation 7
    double x = 0 + t / (4 * pow(M_av, 2));
    double y = 1 - t / pow(M_V_squared, 2);
    double z = 1 - t / pow(M_A_squared, 2);
    double f1 = (1 - (1 + xi) * x) / ((1 - x) * pow(y, 2));
    double f2 = xi / ((1 - x) * pow(y, 2));
    double g1 = g_1_0 / pow(z, 2);
    double g2 = 2 * pow(M_av, 2) * g1 / (pow(139.56995, 2) - t);

    // Calculate A, B and C. See equation 10
    double A = 1. / 16. *
               ((t - pow(CLHEP::electron_mass_c2, 2)) *
                    (4 * pow(f1, 2) * (4 * pow(M_av, 2) + t + pow(CLHEP::electron_mass_c2, 2)) +
                     4 * pow(g1, 2) * (-4 * pow(M_av, 2) + t + pow(CLHEP::electron_mass_c2, 2)) +
                     pow(f2, 2) * (pow(t, 2) / pow(M_av, 2) + 4 * t + 4 * pow(CLHEP::electron_mass_c2, 2)) +
                     4 * pow(CLHEP::electron_mass_c2, 2) * t * pow(g2, 2) / pow(M_av, 2) +
                     8 * f1 * f2 * (2 * t + pow(CLHEP::electron_mass_c2, 2)) +
                     16 * pow(CLHEP::electron_mass_c2, 2) * g1 * g2) -
                pow(delta, 2) * ((4 * pow(f1, 2) + t * pow(f2, 2) / pow(M_av, 2)) *
                                     (4 * pow(M_av, 2) + t - pow(CLHEP::electron_mass_c2, 2)) +
                                 4 * pow(g1, 2) * (4 * pow(M_av, 2) - t + pow(CLHEP::electron_mass_c2, 2)) +
                                 4 * pow(CLHEP::electron_mass_c2, 2) * pow(g2, 2) *
                                     (t - pow(CLHEP::electron_mass_c2, 2)) / pow(M_av, 2) +
                                 8 * f1 * f2 * (2 * t - pow(CLHEP::electron_mass_c2, 2)) +
                                 16 * pow(CLHEP::electron_mass_c2, 2) * g1 * g2) -
                32 * pow(CLHEP::electron_mass_c2, 2) * M_av * delta * g1 * (f1 + f2));

    double B = 1. / 16. *
               (16 * t * g1 * (f1 + f2) +
                4 * pow(CLHEP::electron_mass_c2, 2) * delta * (pow(f2, 2) + f1 * f2 + 2 * g1 * g2) / M_av);

    double C = 1. / 16. * (4 * (pow(f1, 2) + pow(g1, 2)) - t * pow(f2, 2) / pow(M_av, 2));

    // Calculate the absolute value of the matrix element squared. See equation 5.
    double abs_M_squared = A - (s_minus_u)*B + pow((s_minus_u), 2) * C;

    return abs_M_squared;
}

double WCSimIBDGen::dSigmaBydt(double e_nu, double e_e) {
    // Mandelstam variable s. See equation 7.
    double s = 2 * CLHEP::proton_mass_c2 * e_nu + pow(CLHEP::proton_mass_c2, 2);

    // Calculate dSigma/dt. See equation 3.
    double dsigma_dt = pow(G_f, 2) * pow(cos_cabibbo, 2) /
                       (2 * CLHEP::pi * pow((s - pow(CLHEP::proton_mass_c2, 2)), 2)) * MatrixElement(e_nu, e_e);

    return dsigma_dt;
}

double WCSimIBDGen::dSigmaBydEe(double e_nu, double e_e) {
    // Calculates dSigma/dE_e and applies the one-loop radiative correction

    // Calculate dSigma/dEe. See equation 11
    double dSigma_by_dEe = 2 * CLHEP::proton_mass_c2 * dSigmaBydt(e_nu, e_e);

    // Apply the one-loop radiative correction. See equation 14.
    double dSigma_by_dEe_corrected = RadiativeCorrection(dSigma_by_dEe, e_e);

    return dSigma_by_dEe_corrected;
}

double WCSimIBDGen::RadiativeCorrection(double dSigma_by_dEe, double e_e) {
    // Calculate and apply the radiative correction. See equation 14
    // This correction is valued for E_nu << m_p
    double rad_corrected = dSigma_by_dEe * (1 + alpha / CLHEP::pi *
                                                    (6.00 + 3. / 2. * log10(CLHEP::proton_mass_c2 / (2 * e_e)) +
                                                     1.2 * pow(CLHEP::electron_mass_c2 / e_e, 1.5)));

    return rad_corrected;
}

double WCSimIBDGen::dSigmaBydCosTheta(double e_nu, double cos_theta) {
    // Simplifies the cross section exapression when expanding in powers of epsilon. See equation 8.
    double epsilon = e_nu / CLHEP::proton_mass_c2;

    // Get the positron energy corresponding to the neutrino energy and scattering angle.
    double e_e = GetEe(e_nu, cos_theta);

    // Energy momentum relation. See equation 21
    double p_e = sqrt(pow(e_e, 2) - pow(CLHEP::electron_mass_c2, 2));

    // Calculate dSigma/dcos(theta). See equation 20
    double dSigma_by_dCosTheta =
        p_e * epsilon / (1 + epsilon * (1 - e_e / p_e * cos_theta)) * dSigmaBydEe(e_nu, e_e);

    return dSigma_by_dCosTheta * CLHEP::hbarc * CLHEP::hbarc;
}

double WCSimIBDGen::GetEe(double e_nu, double cos_theta) {
    // See above comment and equation 8.
    double epsilon = e_nu / CLHEP::proton_mass_c2;

    // See below equation 21 for kappa definition
    double kappa = pow(1 + epsilon, 2) - pow(epsilon * cos_theta, 2);

    // Positron energy calculation. See equation 21
    double e_e = ((e_nu - delta_cm) * (1 + epsilon) +
                  epsilon * cos_theta * sqrt(pow(e_nu - delta_cm, 2) - pow(CLHEP::electron_mass_c2, 2) * kappa)) /
                 kappa;

    return e_e;
}
