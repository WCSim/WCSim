#include "WCSimLIGen.hh"

#include <chrono>
#include "json.hpp"
#include "G4ParticleGun.hh"
#include "G4PhysicalConstants.hh"
#include "G4OpticalPhoton.hh"
#include "G4RandomDirection.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"

using json = nlohmann::json;

// Linear search for index i such that x_vals[i] <= x < x_vals[i+1]
int FindInterval(const std::vector<double>& vals, double x) {
    auto it = std::upper_bound(vals.begin(), vals.end(), x);
    int idx = std::max(0, int(it - vals.begin()) - 1);
    if (idx >= int(vals.size()) - 1) idx = vals.size() - 2;
    return idx;
}

// Compute finite difference slopes (delta)
std::vector<double> ComputeDeltas(const std::vector<double>& x, const std::vector<double>& y) {
    std::vector<double> deltas;
    for (size_t i = 0; i + 1 < x.size(); ++i)
        deltas.push_back((y[i+1] - y[i]) / (x[i+1] - x[i]));
    return deltas;
}

// Compute monotonic slopes (m) using Fritsch–Carlson method
std::vector<double> ComputeMonotonicSlopes(const std::vector<double>& x, const std::vector<double>& y) {
    size_t n = x.size();
    std::vector<double> ma(n);
    std::vector<double> d = ComputeDeltas(x, y);

    ma[0] = d[0];
    ma[n-1] = d[n-2];

    for (size_t i = 1; i < n - 1; ++i) {
        if (d[i-1] * d[i] <= 0) {
            ma[i] = 0;
        } else {
            double w1 = 2 * (x[i+1] - x[i]);
            double w2 = 2 * (x[i] - x[i-1]);
            ma[i] = (w1 + w2) / ((w1 / d[i-1]) + (w2 / d[i]));
        }
    }
    return ma;
}

// Monotonic 1D interpolation at x in [x0, x1]
double PCHIPInterpolate(double x, const std::vector<double>& xs, const std::vector<double>& ys, const std::vector<double>& mas) {
    int i = FindInterval(xs, x);
    double h = xs[i+1] - xs[i];
    double t = (x - xs[i]) / h;

    double t2 = t * t;
    double t3 = t2 * t;

    double h00 = 2*t3 - 3*t2 + 1;
    double h10 = t3 - 2*t2 + t;
    double h01 = -2*t3 + 3*t2;
    double h11 = t3 - t2;

    return h00 * ys[i] + h10 * h * mas[i] + h01 * ys[i+1] + h11 * h * mas[i+1];
}

double Monotonic2DInterpolate(const std::vector<double>& x_vals, const std::vector<double>& y_vals, const std::vector<std::vector<double>>& z_grid, const std::vector<std::vector<double>>& precomputed_slopes, double x, double y){
    // Interpolate in x for fixed y (rows)
    std::vector<double> z_interp_y;
    for (size_t j = 0; j < y_vals.size(); ++j) {
        const auto& z_row = z_grid[j];
        const auto& slopes = precomputed_slopes[j];
        double zx = PCHIPInterpolate(x, x_vals, z_row, slopes);
        z_interp_y.push_back(zx);
    }

    // Interpolate the x-results along y
    auto slopes_y = ComputeMonotonicSlopes(y_vals, z_interp_y);
    double result = PCHIPInterpolate(y, y_vals, z_interp_y, slopes_y);

    // Clamp to 0 to prevent negatives
    return std::max(0.0, result);
}

std::vector<std::vector<double>> PrecomputeMonotonicSlopesAndRows(
    const std::vector<double>& x_vals,
    const std::vector<double>& y_vals,
    const std::vector<std::vector<double>>& z_grid)
{
    std::vector<std::vector<double>> slopes_and_rows(y_vals.size());

    for (size_t j = 0; j < y_vals.size(); ++j) {
        const auto& z_row = z_grid[j];
        auto slopes = ComputeMonotonicSlopes(x_vals, z_row);
        slopes_and_rows[j] = slopes;  // Store precomputed slopes for each row
    }
    
    return slopes_and_rows;
}


std::pair<double,double> SampleFromInterpolatedSurface(const std::vector<double>& x_vals,
                                     const std::vector<double>& y_vals,
                                     const std::vector<std::vector<double>>& z_grid,
				     std::vector<std::vector<double>> slopes_and_rows,
                                     double xMin, double xMax,
                                     double yMin, double yMax,
				     double zMax, TRandom3& rng) {
  while (true) {
    double x = rng.Uniform(xMin, xMax);
    double y = rng.Uniform(yMin, yMax);
    double z = Monotonic2DInterpolate(x_vals, y_vals, z_grid, slopes_and_rows, x, y);
    double u = rng.Uniform(0.0, zMax);
    //Accept x,y values if interpolated z is below a random value
    if (u < z){
      return {x, y};
    }
  }
}


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
    wcsimdir = string(getenv("WCSIM_BUILD_DIR"))+"/data/";
    // Use particle gun to generate a point source
    myLIGun = new G4ParticleGun();
} 

void WCSimLIGen::SetPhotonMode(G4bool photonmode){
   
    if (photonmode){
        photonMode=1;
    }
    else
      photonMode = 0;
}
    

void WCSimLIGen::ReadFromDatabase(G4String injectorType, G4String injectorIdx, G4String injectorFilename, G4String injectorDetails, G4String injectorDetector, G4double injectorWavelength){

  photonWavelength = injectorWavelength;
    // Define the database to read from
    string db = injectorFilename;
    string db_pos = injectorDetails;
    
    if (db.empty()) {
        db = wcsimdir + "LightInjectors.json";
    }
    else {
        db = wcsimdir + injectorFilename;
    }

    if (db_pos.empty()) {
        db_pos = wcsimdir + "LightInjectorsDetails.json";
    }
    else {
        db_pos = wcsimdir + injectorDetails;
    }
    
    ifstream fJson(db.c_str());
    ifstream f2Json(db_pos.c_str());
    
    if (!fJson) {
        G4cerr << "LIGen: [ERROR] light injector db " << db << " not found" << G4endl;
        exit(-1);
    } else {
        G4cout << " LIGen: Light Injector db " << db << " opened" << G4endl;
    }
    if (!f2Json) {
        G4cerr << "LIGen: [ERROR] light injector db for positioning " << db\
_pos << " not found" << G4endl;
        exit(-1);
    } else {
        G4cout << " LIGen: Light Injector db for positioning " << db_pos <<\
 " opened" << G4endl;
    }

    
    stringstream buffer;
    buffer << fJson.rdbuf();

    stringstream buffer2;
    buffer2 << f2Json.rdbuf();
    
    // Read in position and direction of injector from database
    // and fill the histogram with the light injector profile
    json data = json::parse(buffer.str());
    for (auto injector : data["profiles"]){
      if ( injector["type"].get<string>()==injectorType && injector["detector"].get<string>()==injectorDetector){
	thetaVals = injector["theta"].get<vector<double>>();
	phiVals = injector["phi"].get<vector<double>>();
	intensity = injector["intensity"].get<vector<double>>();
      }
    }
    if ( injectorDetector == "ID" ) thetabins = 180;
    else  thetabins = thetaVals.size();
    
    json data2 = json::parse(buffer2.str());
    for (auto injector2 : data2["injectors"]){
      if ( injector2["type"].get<string>()==injectorType && G4String(injector2["idx"].get<string>())==injectorIdx && injector2["detector"].get<string>()==injectorDetector){
	injectorPosition = injector2["position"].get<vector<double>>();
	injectorDirection = injector2["direction"].get<vector<double>>();
	injectorOffset = injector2["offset"].get<double>();
	if ( photonMode ){
	  photonsFilename = injector2["photonsfile"].get<string>();
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
    G4cout << "photonsFile" << photonsFile << G4endl; 
    ifstream photondata(photonsFile.c_str());
    if ( ! photondata) {
        G4cerr << "WCSimLIGen [ERROR]: Failed to open " << photonsFilename << G4endl;
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
    float phiMin = phiVals[0];
    unsigned int nbins = thetaVals.size();
    unsigned int bins = thetabins;
    float phiMax = phiVals[nbins-1];

    if (hProfile != NULL) {
      delete hProfile;
      hProfile = nullptr;
    }
    if (prof != NULL){
      delete prof;
      prof = nullptr;
    }
    
    double cosTheta;
    int pointIndex = 0;
    
    prof = new TGraph2D();
    for (auto i=0u;i<nbins;i++){
      cosTheta = cos((thetaVals[i]-90)*deg);
      prof->SetPoint(pointIndex,cosTheta,phiVals[i],intensity[i]);
      pointIndex++;
    }

    std::map<std::pair<double, double>, double> intensity_map;
    std::set<double> cosTheta_set;
    std::set<double> phi_set;

    //Setup set of possible cosTheta, phi and intensity values
    for (int i = 0; i < prof->GetN(); ++i) {
        double xi = prof->GetX()[i];
        double yi = prof->GetY()[i];
        double zi = prof->GetZ()[i];
        cosTheta_set.insert(xi);
        phi_set.insert(yi);
        intensity_map[{xi, yi}] = zi;
    }

    cosTheta_vals.assign(cosTheta_set.begin(), cosTheta_set.end());
    phi_vals.assign(phi_set.begin(), phi_set.end());
    
    intensity_grid.resize(phi_vals.size(), std::vector<double>(cosTheta_vals.size()));
    intensityMax = 0;
    double total_intensity = 0;
    
    //Create vector of 2D histogram
    for (size_t i = 0; i < phi_vals.size(); ++i) {
      for (size_t j = 0; j < cosTheta_vals.size(); ++j) {
        auto key = std::make_pair(cosTheta_vals[j], phi_vals[i]);
        auto it = intensity_map.find(key);
        if (it != intensity_map.end()) {
	  intensity_grid[i][j] = it->second;
	  if (it->second > intensityMax)
	    intensityMax = it->second;
        }
	else {
	  intensity_grid[i][j] = 0.0;
        }
	total_intensity += intensity_grid[i][j];
      }
    }
    double sum = 0;
    double minIntensity = 100;
    hProfile = new TH2D("hProfile","hProfile",bins,0,1,bins,phiMin,phiMax);
    //Precompute the interpolated PDF
    slopes_and_rows = PrecomputeMonotonicSlopesAndRows(cosTheta_vals, phi_vals, intensity_grid);
    //Setup profile for visualisation, and determine minimum CosTheta value which was filled. 
    for (int ix = 1; ix <= hProfile->GetNbinsX(); ++ix) {
      for (int iy = 1; iy <= hProfile->GetNbinsY(); ++iy) {
        double x = hProfile->GetXaxis()->GetBinCenter(ix);
        double y = hProfile->GetYaxis()->GetBinCenter(iy);
	double z = Monotonic2DInterpolate(cosTheta_vals, phi_vals, intensity_grid, slopes_and_rows,x, y);
	sum += z;
	hProfile->SetBinContent(ix, iy, sum/total_intensity);
	if (z > 0 && z < minIntensity){
	  minIntensity = z;
	  minCosTheta =  hProfile->GetXaxis()->GetBinCenter(ix);
	}
      }
    }
    G4cout << "Profile filled." << G4endl;
}

void WCSimLIGen::GeneratePhotons(G4Event* anEvent,G4int nphotons){

  // Calculate photon energy now we have the wavelength
  energy = PhotonEnergyFromWavelength(photonWavelength);

    if (photonMode){
        // Get the position and direction from the photon list
        for (int iphoton=0;iphoton<nphotons;iphoton++){
            // Generate random time for this photon in 1 ns pulse window
            G4double time = G4RandFlat::shoot(20.0,21.0)*ns;

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
 
            // Generate random time for this photon in 1 ns pulse window
            G4double time = G4RandFlat::shoot(20.0,21.0)*ns;

	    //Create unique random seed based on event ID and photon number
	    TRandom3 rng(anEvent->GetEventID()*iphoton + iphoton);
	    //Determine photon costheta and phi needed for the photon direction from interpolated PDF
	    auto [costheta, phi] = SampleFromInterpolatedSurface(cosTheta_vals, phi_vals, intensity_grid, slopes_and_rows,minCosTheta, cosTheta_vals.back(), phi_vals.front(),phi_vals.back(), intensityMax, rng);

	    // Calculate the direction of this photon wrt +z direction
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
   
    return PhotonEnergyFromWavelength(photonWavelength);
}
