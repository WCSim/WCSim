#include "WCSimLIGen.hh"
#include "WCSimPrimaryGeneratorMessenger.hh"
#include "WCSimPrimaryGeneratorAction.hh"

#include "json.hpp"

#include "G4ParticleGun.hh"
#include "G4PhysicalConstants.hh"
#include "G4OpticalPhoton.hh"
#include "G4RandomDirection.hh"
#include "G4SystemOfUnits.hh"

using json = nlohmann::json;

WCSimLIGen::WCSimLIGen(){

    // Initialise
    //myDetector = myDC;
    //myMessenger = new WCSimLIGenMessenger(this);
    this->Initialise();

}

WCSimLIGen::~WCSimLIGen(){

    // things to delete
    delete myLIGun;
}


void WCSimLIGen::Initialise(){

    // myMessenger, myDetector
    photonMode = 0;
    wcsimdir = string(getenv("WCSIMDIR"))+"/data/";
    
    // Use particle gun to generate a point source
    G4ParticleGun *myLIGun = new G4ParticleGun();
} 

void WCSimLIGen::SetPhotonMode(G4bool photonmode){
   
    if (photonmode){
        photonMode=1;
    }
    else
      photonMode = 0;
}
    

void WCSimLIGen::ReadFromDatabase(G4String injectorType, G4String injectorIdx, G4String injectorFilename){

    // Define the database to read from
    string db = injectorFilename;
    if (db.empty()) {
        db = wcsimdir + "LightInjectors.json";
    }
    else {
        db = wcsimdir + injectorFilename;
    }
    
    ifstream fJson(db.c_str());
    
    if (!fJson) {
        G4err << "LIGen: [ERROR] light injector db " << db << " not found" << G4endl;
        exit(-1); 
    } else {
        G4cout << " LIGen: Light Injector db " << db << " opened" << G4endl;
    }
    stringstream buffer;
    buffer << fJson.rdbuf();

    // Read in position and direction of injector from database
    // and fill the histogram with the light injector profile
    json data = json::parse(buffer.str());
    for (auto injector : data["injectors"]){
        if ( injector["type"].get<string>()==injectorType && G4String(injector["idx"].get<string>())==injectorIdx ){

            G4cout << "Simulating events from " << injector["type"].get<string>() << " " << injector["idx"].get<string>() << G4endl;
            injectorWavelength = injector["wavelength"].get<double>();
            injectorPosition = injector["position"].get<vector<double>>();
            injectorDirection = injector["direction"].get<vector<double>>();
            injectorOffset = injector["offset"].get<double>();
            thetaVals = injector["theta"].get<vector<double>>();
            phiVals = injector["phi"].get<vector<double>>();
            intensity = injector["intensity"].get<vector<double>>(); 
            if ( photonMode ){
                photonsFilename = injector["photonsfile"].get<string>();
            }
        }
    
    }	
    
    if ( photonMode ){
        LoadPhotonList();
    }
    else {
        LoadProfilePDF();
    }
}

void WCSimLIGen::LoadPhotonList(){

    string photonsFile = wcsimdir + photonsFilename;
    G4cout << photonsFile << G4endl; 
    ifstream photondata(photonsFile.c_str());
    if ( ! photondata) {
        G4err << "WCSimLIGen [ERROR]: Failed to open " << photonsFilename << G4endl;
        exit(-1);
    }
    else {
        G4cout << "WCSimLIGen photon list file: " << photonsFilename << " opened" << G4endl;
    }

    // Load in a csv list of x,y,z,px,py,pz in mm
    string line;
    // skip the first (header) line
    getline(photondata,line);

    while(getline(photondata,line)){
        
        Photon photon;
        istringstream iss(line);
        string value;
        getline(iss,value, ',');
        photon.x = stod(value);
        getline(iss,value, ',');
        photon.y = stod(value);
        getline(iss,value, ',');
        photon.z = stod(value);
        getline(iss,value, ',');
        photon.px = stod(value);
        getline(iss,value, ',');
        photon.py = stod(value);
        getline(iss,value, ',');
        photon.pz = stod(value);

        myPhotons.push_back(photon);
    }

  photondata.close();

}

void WCSimLIGen::LoadProfilePDF(){

    G4cout << "Reading the injector profile from file" << G4endl;

    // Creates histogram of light injector profile
    float thetaMin = thetaVals[0];
    float phiMin = phiVals[0];
    int nbins = thetaVals.size();
    float thetaMax = thetaVals[nbins-1];
    float phiMax = phiVals[nbins-1];

    hProfile = new TH2D("hProfile","hProfile",nbins,thetaMin,thetaMax,nbins,phiMin,phiMax);
    for (auto i=0u;i<thetaVals.size();i++){
        hProfile->Fill(thetaVals[i],phiVals[i]);
        int bin = hProfile->FindBin(thetaVals[i],phiVals[i]);
        hProfile->SetBinContent(bin, intensity[i]);
    }
    G4cout << "Profile filled." << G4endl;
}



void WCSimLIGen::GeneratePhotons(G4Event* anEvent,G4int nphotons){

    // Calculate photon energy now we have the wavelength
    energy = PhotonEnergyFromWavelength(injectorWavelength);

    if (photonMode){
        // Get the position and direction from the photon list
        for (int iphoton=0;iphoton<nphotons;iphoton++){
            // Generate random time for this photon in 20 ns pulse window
            G4double time = G4RandFlat::shoot(20.0,40.0)*ns;

            G4ThreeVector vtx = {myPhotons[iphoton].x,myPhotons[iphoton].y,myPhotons[iphoton].z};
            G4ThreeVector dir = {myPhotons[iphoton].px,myPhotons[iphoton].py,myPhotons[iphoton].pz};

            // Set the gun with the photon parameters
            myLIGun->SetNumberOfParticles(1);
            myLIGun->SetParticleDefinition(G4OpticalPhoton::Definition());
            myLIGun->SetParticleTime(time);		
            myLIGun->SetParticlePosition(vtx);
            myLIGun->SetParticleMomentumDirection(dir);
            myLIGun->SetParticleEnergy(energy);
            myLIGun->SetParticlePolarization(G4RandomDirection());
            if (anEvent){
                // Fill the event
                myLIGun->GeneratePrimaryVertex(anEvent);
            }
        }
    }

    else {

        // Find the angle and axis of rotation of injector axis (v) wrt to +z (u)
        // so that we can rotate the direction from the profile if we need to
        G4ThreeVector u = G4ThreeVector(0,0,1);
        G4ThreeVector v = G4ThreeVector(injectorDirection[0],injectorDirection[1],injectorDirection[2]);
        // Find the angle of injector axis wrt +z
        G4double angle = acos(v.cosTheta(u));
        // Find the axis of rotation using the cross product
        G4ThreeVector axis = u.unit().cross(v.unit());
        // If direction is +z or -z, set x-axis as axis of rotation
        if (axis.mag()==0){
            axis = {1,0,0};
        }

        // Now generate the photon positions and directions
        for (int iphoton = 0; iphoton<nphotons; iphoton++){
 
            // Generate random time for this photon in 20 ns pulse window
            G4double time = G4RandFlat::shoot(20.0,40.0)*ns;
         
            // Get a random theta and phi from the LI profile
            double theta;
            double phi;
            hProfile->GetRandom2(theta,phi);
            
            // Calculate the direction of this photon wrt +z direction
            G4double costheta = cos(theta*deg);
            G4double sintheta = sqrt(1. - costheta*costheta);
            G4double sinphi = sin(phi*deg);
            G4double cosphi = cos(phi*deg);
            G4double px = sintheta*cosphi;
            G4double py = sintheta*sinphi;
            G4double pz = costheta;
            	   
            // Rotate the photon direction wrt the injector axis using the
            // angle and axis of rotation calculated earlier
            G4ThreeVector dir = {px,py,pz};
            dir.rotate(angle,axis);
            
            // Now move the photon vtx to front edge of the injector to avoid 
            // issues with collimator geometries
            G4ThreeVector vtx = G4ThreeVector(injectorPosition[0]*cm,injectorPosition[1]*cm,injectorPosition[2]*cm);
            // Get the vtx at dr in the direction of the photon
            vtx += dir*injectorOffset*cm;
                          
            // Set the gun with the photon parameters
            myLIGun->SetNumberOfParticles(1);
            myLIGun->SetParticleDefinition(G4OpticalPhoton::Definition());
            myLIGun->SetParticleTime(time);		
            myLIGun->SetParticlePosition(vtx);
            myLIGun->SetParticleMomentumDirection(dir);
            myLIGun->SetParticleEnergy(energy);
            myLIGun->SetParticlePolarization(G4RandomDirection());
            // Fill the event
            if (anEvent){
                myLIGun->GeneratePrimaryVertex(anEvent);
            }
        }
    }
}


G4double WCSimLIGen::PhotonEnergyFromWavelength(G4double wavelength){

    double planck = 4.1357e-15; // ev.s
    double lightspeed = 299792458e9; // nm/s
    energy = (planck*lightspeed/wavelength)*eV;// wavelength
    return energy;
}

G4ThreeVector WCSimLIGen::GetInjectorPosition(){

    return G4ThreeVector(injectorPosition[0]*cm,injectorPosition[1]*cm,injectorPosition[2]*cm);
}

G4ThreeVector WCSimLIGen::GetInjectorDirection(){

    return G4ThreeVector(injectorDirection[0],injectorDirection[1],injectorDirection[2]);
}

G4double WCSimLIGen::GetPhotonEnergy(){
   
    return PhotonEnergyFromWavelength(injectorWavelength);
}
