/*********************************************************************************/
/**	WCSimGenerator_Radioactivity						**/
/**	Author: Guillaume Pronost (pronost@km.icrr.u-tokyo.ac.jp)		**/
/**	Date: October 8th 2019							**/
/**	Desc: Generate random vertex position for radioactivity events 		**/
/** 	following SK-IV radon-like event distribution				**/
/*********************************************************************************/

#include "WCSimGenerator_Radioactivity.hh"
#include "Randomize.hh"

// TF2 function
// val[0] is R^{2}
// val[1] is Z
// par[0] is the lambda of the radioisotope


// Declare static variables:
G4double WCSimGenerator_Radioactivity::fRnDiffusion_Coef		= 0.;
G4double WCSimGenerator_Radioactivity::fRnLambda			= 0.;

G4double WCSimGenerator_Radioactivity::fRn_PerPMT			= 0.;
G4double WCSimGenerator_Radioactivity::fRn_Border			= 0.;

G4double WCSimGenerator_Radioactivity::fSK_Z_min			= 0.;
G4double WCSimGenerator_Radioactivity::fSK_Z_max			= 0.;
G4double WCSimGenerator_Radioactivity::fSK_R_max			= 0.;
G4double WCSimGenerator_Radioactivity::fSK_R2_max			= 0.;

G4double WCSimGenerator_Radioactivity::fZ_min				= 0.;
G4double WCSimGenerator_Radioactivity::fZ_max				= 0.;
G4double WCSimGenerator_Radioactivity::fR_max				= 0.;
G4double WCSimGenerator_Radioactivity::fR2_max				= 0.;

G4double WCSimGenerator_Radioactivity::fZ_min_FV			= 0.;
G4double WCSimGenerator_Radioactivity::fZ_max_FV			= 0.;
G4double WCSimGenerator_Radioactivity::fR_max_FV			= 0.;
G4double WCSimGenerator_Radioactivity::fR2_max_FV			= 0.;

WCSimGenerator_Radioactivity::WCSimGenerator_Radioactivity(const WCSimDetectorConstruction* myDC) {
	myDetector = myDC;
	this->Initialize();
}

WCSimGenerator_Radioactivity::~WCSimGenerator_Radioactivity() {
	if ( tfRnFunction ) delete tfRnFunction;
}


void WCSimGenerator_Radioactivity::Initialize() {

	// SK:
	// Using Z = 36.2 m and R = (33.6815/2.) m
	// We have a surface of the ID of about 5612.4302 m^{2}
	// Taking 11 129 PMTs we have 0.50430678 m^{2} per PMT
	// Assuming 10 mBq at equilibrium near the PMTs we have:
	// 19.82 mBq/m^{2} -> Assume that at border we have 19.82 mBq/m^{3} (infinitisemal volume)

	// HK:
	// ID surface: 15934.360 m^{2}
	// 40k PMT: 0.39835899 m^{2} per PMT -> 25.10 mBq/m^{2}
	// 20k PMT: 0.79671798 m^{2} per PMT -> 12.55 mBq/m^{2}
	
	 //Radon diffusion coefficient in water from  https://pdfs.semanticscholar.org/a6d1/7962c7ae2af1e9663349be4e85eb304541ca.pdf
	fRnDiffusion_Coef = 6.8e-4 * 1e-4; //m2 / sec

	// Radon radioactivity constant
	fRnLambda =  log(2) / (3.824*24.*3600.);

	//fRn_PerPMT   = 30.; // SK: mBq/PMT (equilibrium), assuming 20 mBq/Band + 10 mBq/PMT
	fRn_PerPMT   = 10.; // HK: mBq/PMT (equilibrium), assuming ~0 mBq/Band + 10 mBq/PMT

	// SK Detector size, use for reference
	fSK_Z_max   = 36.200  / 2.;
	fSK_Z_min   = -1. * fSK_Z_max;
	fSK_R_max   = 33.6815 / 2.;
	fSK_R2_max  = fSK_R_max * fSK_R_max;

	// Detector size:
	fZ_max   = myDetector->GetIDHeight() / CLHEP::m / 2.;
	fZ_min   = -1. * fZ_max;
	fR_max   = myDetector->GetIDRadius() / CLHEP::m;
	fR2_max  = fR_max * fR_max;
	
	// Calculate binning:
	fNBins = 100.0;
	fMperBinsZ = (fZ_max - fZ_min)/fNBins;
	fMperBinsR = sqrt(fR2_max/fNBins);

	// Detector FV (assume 2 m from wall), only used for debug
	fZ_max_FV   = fZ_max - 2.;
	fZ_min_FV   = fZ_min + 2.;
	fR_max_FV   = fR_max - 2.;
	fR2_max_FV  = fR_max_FV * fR_max_FV;

	// Estimate Rn concentration at ID border
	G4int nPMT = myDetector->Get_Pmts()->size();
	G4double dSurface = TMath::Pi() * fR2_max * 2. + (2.* TMath::Pi() * fR_max)  * (fZ_max - fZ_min);
	fRn_Border = fRn_PerPMT * nPMT / dSurface;
	
	// Initialize
	fConcWater = 2.63; // SK value
	
	fScenario = 0;
	tfRnFunction = 0;
	fConcentrationID = 0;
	fConcentrationFV = 0;

	this->GenerateRnFunction();
}

void WCSimGenerator_Radioactivity::Configuration(G4int iScenario, G4double dConcWater) {


	G4cout << " ========================================================================== " << G4endl;
	G4cout << " Configuration for Radioactivity generator " << G4endl;

	fScenario = iScenario;
	
	G4cout << " ========================================================================== " << G4endl;

	
	if ( fScenario == 0 ) {
		G4cout << " Uniform Rn concentration is assumed " << G4endl;
		fConcentrationID = 0;
		fConcentrationFV = 0;
	}
	else {
		//if ( dLifeTime > 0 ) fLifeTime = dLifeTime;
		if ( dConcWater > 0 ) fConcWater = dConcWater;
		
		this->GenerateRnFunction();
		
		G4cout << " Activity on ID border:\t" << fRn_Border << " mBq/m^3  " <<  G4endl;
		G4cout << " Activity in water:\t" << fConcWater << " mBq/m^3  " <<  G4endl;
		
		// Compute concentration
		// Integral is of a 2D projection, it is already divided by PI
		fConcentrationID = fIntegral   / (fR2_max * (fZ_max - fZ_min) );
		fConcentrationFV = fIntegralFV / (fR2_max_FV * (fZ_max_FV - fZ_min_FV) );

		G4cout << " Rn concentration is defined with the following parameters " << G4endl;
		G4cout << " PMT number: " << myDetector->Get_Pmts()->size() << G4endl;
		G4cout << " Detector radius: " << fR_max << " ; Detector height: " << fZ_max * 2. << G4endl;
		G4cout << " Surface: " << (2. * TMath::Pi() * fR2_max + 2. * TMath::Pi() * fR_max * (fZ_max - fZ_min) ) << G4endl;
		G4cout << " Mean activity in the full ID:   "         << fIntegral   << " mBq  ( Concentration: " << fConcentrationID << " mBq / m^3 ) " <<  G4endl;
		G4cout << " Mean activity in the fiducial volume:   " << fIntegralFV << " mBq  ( Concentration: " << fConcentrationFV << " mBq / m^3 ) " <<  G4endl;
	}
	G4cout << " ========================================================================== " << G4endl;
}


void WCSimGenerator_Radioactivity::GenerateRnFunction() {

	int iBin_R2 = fR2_max / ( fMperBinsR * fMperBinsR );
	int iBin_Z  = (fZ_max - fZ_min) / ( fMperBinsZ );
	
	std::cout << "DEBUG: Radioactivity volume: R2=[0, " << fR2_max << "], Z=[" << fZ_min << ", " << fZ_max << "]" << std::endl;
	if ( tfRnFunction ) delete tfRnFunction;
	tfRnFunction = new TF2("RndPos",RadonFormula,0.,fR2_max,fZ_min,fZ_max,1);
	tfRnFunction->SetParameter(0,fConcWater);
	tfRnFunction->SetNpx(iBin_R2);
	tfRnFunction->SetNpy(iBin_Z );
	
	std::cout << "DEBUG: Radioactivity integral: R2=[0, " << fR2_max << "], Z=[" << fZ_min << ", " << fZ_max << "]" << std::endl;
	fIntegral   = tfRnFunction->Integral(0,fR2_max,		fZ_min,		fZ_max);
	std::cout << fIntegral << std::endl;
	std::cout << "DEBUG: Radioactivity integral in FV: R2=[0, " << fR2_max_FV << "], Z=[" << fZ_min_FV << ", " << fZ_max_FV << "]" << std::endl;
	fIntegralFV = tfRnFunction->Integral(0,fR2_max_FV,	fZ_min_FV,	fZ_max_FV);
	std::cout << fIntegralFV << std::endl;
}

G4ThreeVector WCSimGenerator_Radioactivity::GetRandomVertex(G4int tSymNumber) {

	G4double Dim  = (G4double) tSymNumber;
	G4double R2 = 0;
	G4double Z  = 0;

	G4double& R2_ref = R2;
	G4double& Z_ref = Z;

	if ( fScenario == 0 ) {
		R2 = G4UniformRand() * (fR_max * fR_max);
		Z  = G4UniformRand() * (fZ_max * 2.) - fZ_max;
	}
	else {
		tfRnFunction->GetRandom2(R2_ref,Z_ref);
	}

	G4double theta = G4UniformRand() * 2. * TMath::Pi() / Dim;

	G4double X = sqrt(R2) * cos(theta);
	G4double Y = sqrt(R2) * sin(theta);

	G4ThreeVector vec(X * CLHEP::m, Y * CLHEP::m, Z * CLHEP::m);
	
	std::cout << "DEBUG: Random vertex: (" << X << " " << Y << " " << " " << Z << ")" << std::endl;

	return vec;
}
//*****************************************************************************************************

double WCSimGenerator_Radioactivity::DiffusionZ(double x, double origin, double factor) {

	return 1. / cosh( sqrt(fRnLambda/(fRnDiffusion_Coef*factor)) * (origin - x) );
}
double WCSimGenerator_Radioactivity::DiffusionR(double x, double origin, double factor) {

	return 1. / cosh( sqrt(fRnLambda/(fRnDiffusion_Coef*factor)) * (sqrt(origin) - sqrt(x)) );
}

//*****************************************************************************************************

double WCSimGenerator_Radioactivity::RadonFormula(double *val, double *par) {

	double lR2 	= val[0];
	double lZ 	= val[1];
	
	double dConcWater = par[0];
	
	// Set model positions:
	double dLimit_Border_Bot_Z  	= -11.	* fZ_max  / fSK_Z_max;
	double dLimit_Border_Bot_Max_Z  = -9.5	* fZ_max  / fSK_Z_max;
	double dScale_Border_R2 	= 45.	* fR2_max / fSK_R2_max;
	double dScale_Diffusion_R2 	= 50.	* fR2_max / fSK_R2_max;
	double dScale_Diffusion_Z 	= 20.	* fZ_max  / fSK_Z_max;
	
	double dBoost_Z			= -7.0	* fZ_max  / fSK_Z_max;
	double dLimit_Lat_R2 		= 205	* fR2_max / fSK_R2_max;
	
	//----------------------------------//
	
	double dConcentration_PMT_Top	= fRn_Border * DiffusionZ(lZ , fZ_max,1.);
	double dConcentration_PMT_Bot	= fRn_Border * DiffusionZ(lZ , fZ_min,1.);
	double dConcentration_PMT_Lat	= fRn_Border * DiffusionR(lR2,fR2_max,1.);
	
	double dConcentration_PMT	= dConcentration_PMT_Top + dConcentration_PMT_Bot + dConcentration_PMT_Lat;
	
	double dLimit_Bot_Z  		= std::min(dLimit_Border_Bot_Z + lR2 / dScale_Border_R2, dLimit_Border_Bot_Max_Z);
	
	double dDiffusionFactor  = 1;
	if ( lZ > dLimit_Bot_Z )  {
		dDiffusionFactor += std::min((dLimit_Bot_Z - lZ) / (dLimit_Bot_Z - dBoost_Z), 1.0) * dScale_Diffusion_Z * lR2 / dScale_Diffusion_R2;
	}
	
	double dConcentration_water = 0.;
	
	if ( lR2 > dLimit_Lat_R2 ) {
		dConcentration_water = dConcWater;
	}
	else if ( lZ < dLimit_Bot_Z ) {
		dConcentration_water = dConcWater;
	}
	else {
		double dDiffusion = std::max(DiffusionZ(lZ , dLimit_Bot_Z, dDiffusionFactor), DiffusionR(lR2,dLimit_Lat_R2,1.) );
		dConcentration_water = dConcWater * dDiffusion;
	}
	
	double dConcentration_increase = 0.;
	
	if ( lZ < dLimit_Bot_Z && lZ > dLimit_Border_Bot_Z ) {
		dConcentration_increase = (dConcWater * 1.3 - dConcentration_water);
	}
	else {
		double dDiffusion = std::max(DiffusionZ(lZ , dLimit_Bot_Z,1.), DiffusionZ(lZ , dLimit_Border_Bot_Z, dDiffusionFactor) );
		dConcentration_increase = (dConcWater * 1.3 - dConcentration_water) * dDiffusion;
	}
	
	return std::max(0.1, dConcentration_PMT + dConcentration_water + dConcentration_increase);
}

//*****************************************************************************************************
