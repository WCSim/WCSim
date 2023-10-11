#ifndef WCSimLIGen_h
#define WCSimLIGen_h

#include "G4ThreeVector.hh"
#include "globals.hh"
#include "jhfNtuple.h"
#include <fstream>
#include <vector>
#include <TH2D.h>
#include <TH3D.h>

using namespace std;

class G4ParticleGun;
class G4Event;

class WCSimLIGen
{

    public:
        WCSimLIGen();
        ~WCSimLIGen();
  
        // Initialise the light injector
        void Initialise();
        void ReadFromDatabase(G4String injectorType, G4String injectorIdx, G4String injectorFilename);

        // Set the vertices, etc of all photons in the pulse
        void GeneratePhotons(G4Event* anEvent, G4int nphotons);
        // Get injector properties
        G4ThreeVector GetInjectorPosition();  
        G4ThreeVector GetInjectorDirection();
        G4double GetPhotonEnergy();
        // Set whether to read in profile or photon list
        void SetPhotonMode(G4bool photonMode);

    private:

        G4ParticleGun*                  myLIGun;

        // Variables for initialising light injector parameters
        vector<double> injectorPosition;
        vector<double> injectorDirection;
        vector<double> thetaVals;
        vector<double> phiVals;
        vector<double> zVals;
        vector<double> intensity;
        
        G4double injectorWavelength;
        G4double injectorOffset;
        G4double energy;

	// Variables for reading in injector profile or photon list
	string wcsimdir;
	G4String photonsFilename;
        G4bool photonMode;

        // Histogram for profile
        TH2D *hProfile;
        void FillProfilePDF();

        // Struct for reading in photons list from file
        struct Photon
        {
            double x;
            double y;
            double z;
            double px;
            double py;
            double pz;
        };
        vector<Photon> myPhotons;

	// Functions
        G4double PhotonEnergyFromWavelength(G4double wavelength);
        void LoadPhotonList();
        void LoadProfilePDF();

  };

#endif


