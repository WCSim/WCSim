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
// par[1] is the M / bin


// Declare static variables:
G4double WCSimGenerator_Radioactivity::fRnDiffusion_Coef		= 0.;
G4double WCSimGenerator_Radioactivity::fRnLambda_Global		= 0.;

G4double WCSimGenerator_Radioactivity::fRn_PerPMT			= 0.;
G4double WCSimGenerator_Radioactivity::fRn_Border			= 0.;

G4double WCSimGenerator_Radioactivity::fCurrentDetector_Z_min	= 0.;
G4double WCSimGenerator_Radioactivity::fCurrentDetector_Z_max	= 0.;
G4double WCSimGenerator_Radioactivity::fCurrentDetector_R_max	= 0.;
G4double WCSimGenerator_Radioactivity::fCurrentDetector_R2_max	= 0.;

G4double WCSimGenerator_Radioactivity::fSK_Z_min			= 0.;
G4double WCSimGenerator_Radioactivity::fSK_Z_max			= 0.;
G4double WCSimGenerator_Radioactivity::fSK_R_max			= 0.;
G4double WCSimGenerator_Radioactivity::fSK_R2_max			= 0.;

G4double WCSimGenerator_Radioactivity::fZ_min				= 0.;
G4double WCSimGenerator_Radioactivity::fZ_max				= 0.;
G4double WCSimGenerator_Radioactivity::fR_max				= 0.;
G4double WCSimGenerator_Radioactivity::fR2_max			= 0.;

G4double WCSimGenerator_Radioactivity::fZ_min_FV			= 0.;
G4double WCSimGenerator_Radioactivity::fZ_max_FV			= 0.;
G4double WCSimGenerator_Radioactivity::fR_max_FV			= 0.;
G4double WCSimGenerator_Radioactivity::fR2_max_FV			= 0.;


// Parameter array initialization:
G4double WCSimGenerator_Radioactivity::vParam_Z [RNMODEL_BIN_R_MAX][7]	= {};	
G4double WCSimGenerator_Radioactivity::vParam_R2[RNMODEL_BIN_Z_MAX][7]	= {};
	
G4double WCSimGenerator_Radioactivity::vLayer_MinR2_Z[RNMODEL_BIN_R_MAX]	= {};
G4double WCSimGenerator_Radioactivity::vLayer_MaxR2_Z[RNMODEL_BIN_R_MAX]	= {};

G4double WCSimGenerator_Radioactivity::vLayer_MinZ_R2[RNMODEL_BIN_Z_MAX]	= {};
G4double WCSimGenerator_Radioactivity::vLayer_MaxZ_R2[RNMODEL_BIN_Z_MAX]	= {};

WCSimGenerator_Radioactivity::WCSimGenerator_Radioactivity(WCSimDetectorConstruction* myDC) {
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
	fRnLambda_Global = 0;
	
	// Auto filled parameters
	fConc_Middle 	= 0;
	fConc_Int_R2_6	= 0;
	fConc_Int_R2_7	= 0;
	fConc_Int_R2_8	= 0;
	fConc_Int_R2_9	= 0;
	
	fRn_PerPMT   = 30.; // mBq/PMT (equilibrium), assuming 20 mBq/Band + 10 mBq/PMT 
//	fRn_PerPMT   = 24.; // mBq/PMT (equilibrium), assuming 20 mBq/Band +  4 mBq/PMT 
		
	// Detector size:
	fCurrentDetector_Z_max   = myDetector->GetIDHeight() / CLHEP::m / 2.;
	fCurrentDetector_Z_min   = -1. * fCurrentDetector_Z_max;
	fCurrentDetector_R_max   = myDetector->GetIDRadius() / CLHEP::m;
	fCurrentDetector_R2_max  = fCurrentDetector_R_max * fCurrentDetector_R_max;
	
	// Constant
	fSK_Z_max   = 36.200  / 2.;
	fSK_Z_min   = -1. * fSK_Z_max;
	fSK_R_max   = 33.6815 / 2.;
	fSK_R2_max  = fSK_R_max * fSK_R_max;
	
	fZ_max   = fCurrentDetector_Z_max;
	fZ_min   = fCurrentDetector_Z_min;
	fR_max   = fCurrentDetector_R_max;
	fR2_max  = fR_max * fR_max;
	
	// Detector FV (assume 2 m from wall), only used for debug
	fZ_max_FV   = fZ_max - 2.;
	fZ_min_FV   = fZ_min - 2.;
	fR_max_FV   = fR_max - 2.;
	fR2_max_FV  = fR_max_FV * fR_max_FV;
	
	G4int nPMT = myDetector->Get_Pmts()->size();
	fRn_Border = fRn_PerPMT * nPMT / (2. * TMath::Pi() * fCurrentDetector_R_max * fCurrentDetector_R_max + 2. * TMath::Pi() * fCurrentDetector_R_max * fCurrentDetector_Z_max * 2. );
			
	fScenario = 0;
	tfRnFunction = 0;
	fConcentrationID = 0;
	fConcentrationFV = 0;
	
	this->SetScenario(0);
}

void WCSimGenerator_Radioactivity::Configuration(G4int iScenario, G4double dLifeTime) {
	
	
	G4cout << " ========================================================================== " << G4endl;
	G4cout << " Configuration for Radioactivity generator " << G4endl;
	
	fScenario = iScenario;
	if ( dLifeTime > 0 )
		fRnLambda = log(2) / (dLifeTime);
	this->SetScenario(iScenario);
	G4cout << " ========================================================================== " << G4endl;
	
	G4cout << " Mean lifetime: " << log(2) / fRnLambda << " sec " <<  G4endl;
	G4cout << " Lambda:   " << fRnLambda << "     " <<  G4endl;
	G4cout << " Activity on ID border:   " << fRn_Border << " mBq/m^3  " <<  G4endl;
	if ( fScenario == 0 ) {
		G4cout << " Scenario 0: Uniform Rn concentration is assumed " << G4endl;
		fConcentrationID = 0;
		fConcentrationFV = 0;
	}
	else {
		// Compute concentration
		// Integral is of a 2D projection, it is already divided by PI
		fConcentrationID = fIntegral   / (fR2_max * (fZ_max - fZ_min) );
		fConcentrationFV = fIntegralFV / (fR2_max_FV * (fZ_max_FV - fZ_min_FV) );
		
		G4cout << " Scenario " << fScenario << ": Rn concentration is defined with the following parameters " << G4endl;
		G4cout << " PMT number: " << myDetector->Get_Pmts()->size() << G4endl;
		G4cout << " Detector radius: " << fCurrentDetector_R_max << " ; Detector height: " << fCurrentDetector_Z_max * 2. << G4endl;
		G4cout << " Surface: " << (2. * TMath::Pi() * fR2_max + 2. * TMath::Pi() * fR_max * (fZ_max - fZ_min) ) << G4endl;
		//G4cout << fCurrentDetector_R_max << " " << fCurrentDetector_Z_max * 2 << G4endl;
		G4cout << " Mean activity in the full ID:   "         << fIntegral   << " mBq  ( Concentration: " << fConcentrationID << " mBq / m^3 ) " <<  G4endl;
		G4cout << " Mean activity in the fiducial volume:   " << fIntegralFV << " mBq  ( Concentration: " << fConcentrationFV << " mBq / m^3 ) " <<  G4endl;
	}
	G4cout << " ========================================================================== " << G4endl;
}

void WCSimGenerator_Radioactivity::SetScenario(G4int iScenario) {

	fScenario = iScenario;
	

	// Initialize arrays
	for ( int iZ=0; iZ < RNMODEL_BIN_Z_MAX; iZ++ ) {
		for ( int i=0; i < 7; i++ ) {
			vParam_R2[iZ][i] = 0;
		}
		
		vLayer_MinZ_R2[iZ] = 0;
		vLayer_MaxZ_R2[iZ] = 0;
	}
	
	for ( int iR=0; iR < RNMODEL_BIN_R_MAX; iR++ ) {
		for ( int i=0; i < 7; i++ ) {
			vParam_Z [iR][i] = 0;
		}
		
		vLayer_MinR2_Z[iR] = 0;
		vLayer_MaxR2_Z[iR] = 0;
	}
	
	if ( fScenario == 0 ) {
		fIntegral = 0;
		fIntegralFV = 0;
		
		if ( tfRnFunction ) delete tfRnFunction;
		return;
	}
	
	fRnLambda_Global = fRnLambda;

	//---------------------------------------------//
	// Set parameters values here:
	#include "RnModel_Fit_Params.hh" 
	// 20200922 original parametrized Rn model
	// 20200925 slightly more consistent model (no impact on results)
	double val1[2];	
	val1[0] = -10; // m (Z)
	val1[1] = 0.; // (R2)	
	double val2[2];	
	val2[0] = -8; // m (Z)
	val2[1] = 0.; // (R2)
	fConc_Middle 	= ( RadonFormulaZ( val1, vParam_Z[0] ) - RadonFormulaZ( val2, vParam_Z[0] ) ) / 2.;
	
	double valR2[2];	
	valR2[0] = 115.; // m2 (R2)
	valR2[1] = 0.; // m (Z)
	fConc_Int_R2_6	= ( RadonFormulaR(valR2, vParam_R2[8] ) + RadonFormulaR(valR2, vParam_R2[9] ) ) / 2.;
	
	valR2[0] = 133.; // m2 (R2)
	valR2[1] = 0.; // m (Z)
	fConc_Int_R2_7	= ( RadonFormulaR(valR2, vParam_R2[8] ) + RadonFormulaR(valR2, vParam_R2[9] ) ) / 2.;
	
	valR2[0] = 151.; // m2 (R2)
	valR2[1] = 0.; // m (Z)
	fConc_Int_R2_8	= ( RadonFormulaR(valR2, vParam_R2[8] ) + RadonFormulaR(valR2, vParam_R2[9] ) ) / 2.;
	
	valR2[0] = 169.; // m2 (R2)
	valR2[1] = 0.; // m (Z)
	fConc_Int_R2_9	= ( RadonFormulaR(valR2, vParam_R2[8] ) + RadonFormulaR(valR2, vParam_R2[9] ) ) / 2.;
		
	//---------------------------------------------//
	// Reload parameter to take into account the new variables values	
	#include "RnModel_Fit_Params.hh"
		
	// Z layers
	for ( int iR = 0; iR < RNMODEL_BIN_R_MAX; iR++ ) {
		
		// Set Layer position:
		double dR2_step_SK 	= fSK_R2_max 	/ (double) RNMODEL_NBIN_R;
		double dR2_step 	= fR2_max 	/ (double) RNMODEL_NBIN_R;
		
		double dMinR2 = iR 	* dR2_step_SK;
		double dMaxR2 = (iR+1) * dR2_step_SK;
	
		// Move positions for Z
		
		double dPre_PeakLeft  = vParam_Z[iR][0];
		double dPre_PeakRight = vParam_Z[iR][1];
		
		double dNew_PeakLeft  = dPre_PeakLeft;
		double dNew_PeakRight = dPre_PeakRight;
		
		// Apply shift depending of the scenario:
		
		switch ( fScenario ) {
		
			case 1: {
				// Relative scaling
				
				dMinR2 	= iR 		* dR2_step;
				dMaxR2 	= (iR+1) 	* dR2_step;
				
				vLayer_MinR2_Z[iR] = dMinR2;
				vLayer_MaxR2_Z[iR] = dMaxR2;
				
				dNew_PeakLeft	= dPre_PeakLeft  * fZ_max / fSK_Z_max;
				dNew_PeakRight	= dPre_PeakRight * fZ_max / fSK_Z_max;
				
		
				break;
			}
				
			case 2: {
				// Absolute scaling
			
				// For layer limits we need to start from the end:
				int iRend = RNMODEL_BIN_R_MAX - iR - 1;
				
				dMinR2 			= iRend 	* dR2_step_SK;
				dMaxR2 			= (iRend+1) 	* dR2_step_SK;
				
				dMaxR2 			= fR2_max - (fSK_R2_max - dMaxR2);
				dMinR2				= fR2_max - (fSK_R2_max - dMinR2);
				
				if ( iRend == 0 ) {
					dMinR2			= 0;
				}
				
				vLayer_MinR2_Z[iRend]		= dMinR2;
				vLayer_MaxR2_Z[iRend]		= dMaxR2;
				
				double dSignLeft = GetSign(dPre_PeakLeft);
				double dSignRight = GetSign(dPre_PeakRight);
				
				//
				dNew_PeakLeft  = (fZ_max - (fSK_Z_max - TMath::Abs(dPre_PeakLeft))  ) * dSignLeft;
				dNew_PeakRight = (fZ_max - (fSK_Z_max - TMath::Abs(dPre_PeakRight)) ) * dSignRight;
				
				break;
			}
				
			default: {
				std::cout << "[ERROR] RnModel: Error this scenario doesn't exist! " << std::endl;
				exit(0);
				return;
			}
		}
		
		
		vParam_Z[iR][0] = dNew_PeakLeft;
		vParam_Z[iR][1] = dNew_PeakRight;
	}
	
	// R2 layers
	for ( int iZ = RNMODEL_BIN_Z_MIN; iZ < RNMODEL_BIN_Z_MAX; iZ++ ) {
	
		// Set Layer position:
		
		double dZ_step_SK 	= (fSK_Z_max 	- fSK_Z_min)	/ (double) RNMODEL_NBIN_Z;
		double dZ_step 	= (fZ_max 	- fZ_min)	/ (double) RNMODEL_NBIN_Z;
		
		double dMinZ = iZ 	* dZ_step_SK + fSK_Z_min;
		double dMaxZ = (iZ+1) 	* dZ_step_SK + fSK_Z_min;
		
		
	
		// Move positions for Z
		
		double dPre_Shift_Post 	= vParam_R2[iZ][1];
		double dPre_Diffusion_X	= vParam_R2[iZ][4];
		
		double dNew_Shift_Post 	= dPre_Shift_Post;
		double dNew_Diffusion_X	= dPre_Diffusion_X;
		
		// Apply shift depending of the scenario:
		
		switch ( fScenario ) {
		
			case 1: {
				// Relative scaling
			
				dMinZ 			= iZ 		* dZ_step + fZ_min;
				dMaxZ 			= (iZ+1) 	* dZ_step + fZ_min;
				
				vLayer_MinZ_R2[iZ] 	= dMinZ;
				vLayer_MaxZ_R2[iZ] 	= dMaxZ;
				
				dNew_Shift_Post	= dPre_Shift_Post  * fR2_max / fSK_R2_max;
				dNew_Diffusion_X	= dPre_Diffusion_X * fR2_max / fSK_R2_max;
				
				break;
			}
			case 2: {
				// Absolute scaling
				
				double dSignMin = GetSign(dMinZ);
				double dSignMax = GetSign(dMaxZ);
				
				if ( iZ == RNMODEL_BIN_Z_MIN ) {
					// Set Previous Max Z:
					vLayer_MaxZ_R2[RNMODEL_BIN_Z_MIN-1] 
							= (fZ_max - (fSK_Z_max - TMath::Abs(dMinZ)) ) * dSignMin;
							
				}
				
				dMinZ 			= vLayer_MaxZ_R2[iZ-1];
				
				if ( iZ < 5 ) {
					// Asbolute scaling 
					dMaxZ 		= (fZ_max - (fSK_Z_max - TMath::Abs(dMaxZ))) * dSignMax;
				}
				else {
					// Relative scaling
					dMaxZ 		= (iZ+1) 	* dZ_step + fZ_min;
				} 
			
				vLayer_MinZ_R2[iZ] 	= dMinZ;
				vLayer_MaxZ_R2[iZ] 	= dMaxZ;
		
				dNew_Shift_Post  = fR2_max - (fSK_R2_max - dPre_Shift_Post);
				dNew_Diffusion_X = fR2_max - (fSK_R2_max - dPre_Diffusion_X);
				
				if ( dPre_Diffusion_X == 0 ) {
					dNew_Diffusion_X = 0;
				}
				
				break;
			}
				
			default: {
				std::cout << "[ERROR] RnModel: Error this scenario doesn't exist! " << std::endl;
				exit(0);
				return;
			}
		}
		
		vParam_R2[iZ][1] = dNew_Shift_Post;
		vParam_R2[iZ][4] = dNew_Diffusion_X;
		
				
		if ( dMaxZ < dMinZ ) {
			std::cout << "[ERROR] RnModel: For " << iZ << " dMaxZ < dMinZ this should not happen ( " << dMaxZ << " , " << dMinZ << " ) " << std::endl;
		}
		
		
	}
	std::cout << "[NOTICE] RnModel: fConc_Middle is set to " << fConc_Middle << std::endl;
	for ( int iZ = RNMODEL_BIN_Z_MIN; iZ < RNMODEL_BIN_Z_MAX; iZ++ ) {
		std::cout << "[NOTICE] RnModel: Layer R2 " << iZ << " is between: " << vLayer_MinZ_R2[iZ] << " " << vLayer_MaxZ_R2[iZ] << std::endl;
	}
	for ( int iR = 0; iR < RNMODEL_BIN_R_MAX; iR++ ) {
		std::cout << "[NOTICE] RnModel: Layer Z " << iR << " is between: " << vLayer_MinR2_Z[iR] << " " << vLayer_MaxR2_Z[iR] << std::endl;
	}
	
	
	for ( int iZ = RNMODEL_BIN_Z_MIN; iZ < RNMODEL_BIN_Z_MAX; iZ++ ) {
		std::cout << "[NOTICE] RnModel: Parameter_R2[" << iZ << "][1] = " << vParam_R2[iZ][1] << std::endl;
		std::cout << "[NOTICE] RnModel: Parameter_R2[" << iZ << "][4] = " << vParam_R2[iZ][4] << std::endl;
	}
	
	std::cout << " ------------------------------------------- " << std::endl;
	for ( int iR = 0; iR < RNMODEL_BIN_R_MAX; iR++ ) {
		std::cout << "[NOTICE] RnModel: Parameter_Z[" << iR << "][0] = " << vParam_Z[iR][0] << std::endl;
		std::cout << "[NOTICE] RnModel: Parameter_Z[" << iR << "][1] = " << vParam_Z[iR][1] << std::endl;
	}
	
	// Generate TF2:
	if ( tfRnFunction ) delete tfRnFunction;
	tfRnFunction = new TF2("RndPos",RadonFormula,0.,fR2_max,fZ_min,fZ_max,1);
	
	tfRnFunction->SetParameter(0,fRnLambda);
	
	// Compute activity Integral
	fIntegral = tfRnFunction->Integral(0,fR2_max,fZ_min,fZ_max);
	fIntegralFV = tfRnFunction->Integral(0,fR2_max_FV,fZ_min_FV,fZ_max_FV);
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
	
	return vec;
}
//*****************************************************************************************************
double WCSimGenerator_Radioactivity::DiffusionZ(double x, double origin, double factor) {

	return 1. / cosh( sqrt(fRnLambda_Global/(fRnDiffusion_Coef*factor)) * (origin - x) );
}
double WCSimGenerator_Radioactivity::DiffusionR(double x, double origin, double factor) {

	return 1. / cosh( sqrt(fRnLambda_Global/(fRnDiffusion_Coef*factor)) * (sqrt(origin) -sqrt(x)) );
}

//*****************************************************************************************************

double WCSimGenerator_Radioactivity::RadonFormulaR(double *val, double *par) {

	double dUpperConc 	= par[0];
	double dShift_Post 	= par[1];
	
	double dSlope 		= par[2];
	double dConstant 	= par[3];
	
	double dDiffusion_X	= par[4];
	double dDiffusion_S 	= par[5];
	double dDiffusion_C 	= std::max(par[6],dConstant);
	
	double lR2 = val[0];
	double lZ  = val[1];
	
	
	double dConcentration_PMT_Top	= fRn_Border * DiffusionZ(lZ , fZ_max,1.);
	double dConcentration_PMT_Bot  = fRn_Border * DiffusionZ(lZ , fZ_min,1.);
	
	
	double dDiffusion = 0;
	if ( dDiffusion_X > dShift_Post ) {
		return -999.;
	}
	
	if ( lR2 < dDiffusion_X ) {
		dDiffusion = (dDiffusion_C-dConstant) * DiffusionR(lR2, dDiffusion_X, dDiffusion_S);
	}
	else {
		dDiffusion = (dDiffusion_C-dConstant);
	}
	
	double dConcentration = 0;
	if ( lR2 > dShift_Post ) {
		dConcentration = dUpperConc;
	}
	else {
	
		dConcentration = dConstant + (dUpperConc - dDiffusion_C) * DiffusionR(lR2, dShift_Post, dSlope) + dDiffusion;
	}
	
	double dConcentration_PMT_Lat  	= (fRn_Border - dConcentration ) * DiffusionR(lR2,fR2_max,1.);
	double dConcentration_PMT 		= dConcentration_PMT_Top + dConcentration_PMT_Bot + dConcentration_PMT_Lat;
	
	return dConcentration + dConcentration_PMT;
}

double WCSimGenerator_Radioactivity::RadonFormulaZ(double *val, double *par) {

	double dLimit_PeakLeft  	= par[0];
	double dLimit_PeakRight 	= par[1];
	double dConc0 	   		= par[2];
	double dConc10	   		= par[3];
	double dConc10b		= par[4];
	double dFactorR 		= par[5];
	double dFactorL 		= par[6];
	
	double lZ  = val[0];
	double lR2 = val[1];
	
	
	if ( dConc10 > dConc10b ) dConc10b = dConc10;
	
	if ( dConc10 == dConc10b ) {
		dLimit_PeakRight = std::min(dLimit_PeakLeft,dLimit_PeakRight);
		dLimit_PeakLeft  = dLimit_PeakRight;
	}
	else if ( dLimit_PeakLeft > dLimit_PeakRight ) {
		double dTmp      = dLimit_PeakRight;
		dLimit_PeakRight = dLimit_PeakLeft;
		dLimit_PeakLeft  = dTmp;
	}
		
	double dConcentration_PMT_Top	= (fRn_Border - dConc0 ) * DiffusionZ(lZ , fZ_max,1.);
	double dConcentration_PMT_Bot  = (fRn_Border - dConc10) * DiffusionZ(lZ , fZ_min,1.);
	double dConcentration_PMT_Lat  = (fRn_Border          ) * DiffusionR(lR2,fR2_max,1.);
	
	double dConcentration_PMT 	= dConcentration_PMT_Top + dConcentration_PMT_Bot + dConcentration_PMT_Lat;
	
	if ( fRn_Border <= 0. ) dConcentration_PMT = 0;
	
	double dConcentration_Below = dConc10 + (dConc10b - dConc10) * DiffusionZ(lZ,dLimit_PeakLeft,dFactorL) + dConcentration_PMT;
	double dConcentration_Peak  = dConc10b + dConcentration_PMT;
	double dConcentration_Above = dConc0 + (dConc10b - dConc0) * DiffusionZ(lZ,dLimit_PeakRight,dFactorR) + dConcentration_PMT;
	
	//std::cout << "[DEBUG] RadonFormulaZ z= " << lZ << " " << dConcentration_Peak << " " << dConcentration_Below << " " << dConcentration_Above << std::endl;
	
	if ( lZ < dLimit_PeakLeft ) {
		return dConcentration_Below;  
	}
	else if ( lZ < dLimit_PeakRight ) {
		return dConcentration_Peak;
	}
	
	return dConcentration_Above;
}


//*****************************************************************************************************
double WCSimGenerator_Radioactivity::RadonFormula(double *val, double *par) {

	double lR2 	= val[0];
	double lZ 	= val[1];
	//double lR	= sqrt(lR2);
	
	//----------------------------------//
	// Make value arrays:
	double vZ [2]	= { lZ , lR2	};
	double vR2[2]	= { lR2, lZ 	};
	//----------------------------------//
	fRnLambda_Global = par[0];
	//----------------------------------//
	
	double dConcentration_Z 	= -1;
	double dConcentration_R2 	= -1;
	
	//----------------------------------//
	// Get Radon concentration from Z 1D-layers:

	// Z 1D-layers are from 0 to 10 (Center to Border direction)
	int iR=0;
		
	for ( iR = 0; iR < RNMODEL_BIN_R_MAX; iR++ ) {

		double dMinR2 = vLayer_MinR2_Z[iR];
		double dMaxR2 = vLayer_MaxR2_Z[iR];
		
		if ( lR2 >= dMinR2 && lR2 < dMaxR2 ) {
		
			dConcentration_Z  = RadonFormulaZ( vZ , vParam_Z [iR] );
			break;
		} 
	}
	
	//std::cout << "[DEBUG] iR= " << iR << " " << dConcentration_Z << std::endl;
	
	//----------------------------------//
	// Get Radon concentration from R2 1D-layers:

	// R2 1D-layers are from 2 to 16 (Bottom to Top direction)
	int iZ = 0;
	
	for ( iZ = RNMODEL_BIN_Z_MIN; iZ < RNMODEL_BIN_Z_MAX; iZ++ ) {

		double dMinZ = vLayer_MinZ_R2[iZ];
		double dMaxZ = vLayer_MaxZ_R2[iZ];
		
		if ( lZ >= dMinZ && lZ < dMaxZ ) {
		
			dConcentration_R2 = RadonFormulaR( vR2, vParam_R2[iZ] );
			break;
		} 
	}
	
	//std::cout << "[DEBUG] iZ= " << iZ << " " << dConcentration_R2 << std::endl;
	
	//----------------------------------//
	// Extremum cases:	
	if ( dConcentration_Z  < 0 && dConcentration_R2 >= 0 ) return dConcentration_R2;
	if ( dConcentration_R2 < 0 && dConcentration_Z  >= 0 ) return dConcentration_Z;
	
	if ( dConcentration_R2 < 0 && dConcentration_Z  <  0 ) {
		dConcentration_Z  = RadonFormulaZ( vZ , vParam_Z [9] );
		
		if ( lZ <= 0 ) {
			dConcentration_R2 = RadonFormulaR( vR2, vParam_R2[ 2] );
		}
		else {
			dConcentration_R2 = RadonFormulaR( vR2, vParam_R2[15] );
		}
		
		if ( fRn_Border > 0 ) {
			dConcentration_Z  = std::min(dConcentration_Z ,fRn_Border);
			dConcentration_R2 = std::min(dConcentration_R2,fRn_Border);
		}
		
		return std::max(dConcentration_Z,dConcentration_R2);
	}
	
	//----------------------------------//
	// Normal cases:
	//return std::max(dConcentration_Z,dConcentration_R2);
	//return (dConcentration_Z + dConcentration_R2 ) / 2.;
	return std::min(dConcentration_Z,dConcentration_R2); // Better agreement
}
