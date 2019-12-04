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
G4double WCSimGenerator_Radioactivity::fRnDiffusion_Coef = 0.;

G4double WCSimGenerator_Radioactivity::fRnSK_Center	 = 0.;
G4double WCSimGenerator_Radioactivity::fRnSK_Bottom	 = 0.;
G4double WCSimGenerator_Radioactivity::fRn_Border	 = 0.;

G4double WCSimGenerator_Radioactivity::fHK_Z_max	 = 0.;
G4double WCSimGenerator_Radioactivity::fHK_R_max	 = 0.;
G4double WCSimGenerator_Radioactivity::fHK_R2_max	 = 0.;
G4double WCSimGenerator_Radioactivity::fHK_Z_reco	 = 0.;
G4double WCSimGenerator_Radioactivity::fHK_R_reco	 = 0.;
G4double WCSimGenerator_Radioactivity::fHK_R2_reco	 = 0.;

G4double WCSimGenerator_Radioactivity::fSK_Z_max	 = 0.;
G4double WCSimGenerator_Radioactivity::fSK_R_max	 = 0.;
G4double WCSimGenerator_Radioactivity::fSK_R2_max	 = 0.;
G4double WCSimGenerator_Radioactivity::fSK_Z_reco	 = 0.;
G4double WCSimGenerator_Radioactivity::fSK_R_reco	 = 0.;
G4double WCSimGenerator_Radioactivity::fSK_R2_reco	 = 0.;

G4double WCSimGenerator_Radioactivity::fZ_max		 = 0.;
G4double WCSimGenerator_Radioactivity::fR_max		 = 0.;
G4double WCSimGenerator_Radioactivity::fR2_max		 = 0.;
G4double WCSimGenerator_Radioactivity::fZ_reco		 = 0.;
G4double WCSimGenerator_Radioactivity::fR_reco		 = 0.;
G4double WCSimGenerator_Radioactivity::fR2_reco		 = 0.;

G4double WCSimGenerator_Radioactivity::fChangeZPMax	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeZNMax	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeRMax	 = 0.;

G4double WCSimGenerator_Radioactivity::fR2_000		 = 0.;
G4double WCSimGenerator_Radioactivity::fR2_025		 = 0.;
G4double WCSimGenerator_Radioactivity::fR2_045		 = 0.;
G4double WCSimGenerator_Radioactivity::fR2_075		 = 0.;
G4double WCSimGenerator_Radioactivity::fR2_145		 = 0.;
G4double WCSimGenerator_Radioactivity::fR2_175		 = 0.;
G4double WCSimGenerator_Radioactivity::fR2_195		 = 0.;
G4double WCSimGenerator_Radioactivity::fR2_215		 = 0.;

G4double WCSimGenerator_Radioactivity::fR_000		 = 0.;
G4double WCSimGenerator_Radioactivity::fR_025		 = 0.;
G4double WCSimGenerator_Radioactivity::fR_045		 = 0.;
G4double WCSimGenerator_Radioactivity::fR_075		 = 0.;
G4double WCSimGenerator_Radioactivity::fR_145		 = 0.;
G4double WCSimGenerator_Radioactivity::fR_175		 = 0.;
G4double WCSimGenerator_Radioactivity::fR_195		 = 0.;
G4double WCSimGenerator_Radioactivity::fR_215		 = 0.;

G4double WCSimGenerator_Radioactivity::fZ_p16		 = 0.;
G4double WCSimGenerator_Radioactivity::fZ_p14		 = 0.;
G4double WCSimGenerator_Radioactivity::fZ_000		 = 0.;
G4double WCSimGenerator_Radioactivity::fZ_m10		 = 0.;
G4double WCSimGenerator_Radioactivity::fZ_m13		 = 0.;
G4double WCSimGenerator_Radioactivity::fZ_m16		 = 0.;

G4double WCSimGenerator_Radioactivity::fChangeR210N13	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeR210N10	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeR210N5	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeR210P1	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeR210P8	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeR210P13	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeR190N10	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeR190N8	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeR170N10	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeR170N8	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeR140N10	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeR140N8	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeR070N10	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeR070N8	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeR040N10	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeR020N10	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeR000N10	 = 0.;

G4double WCSimGenerator_Radioactivity::fChangeZmMxR025	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeZmMxR175	 = 0.;

G4double WCSimGenerator_Radioactivity::fChangeZm16R025	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeZm16R125	 = 0.;

G4double WCSimGenerator_Radioactivity::fChangeZm13R023	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeZm13R080	 = 0.;

G4double WCSimGenerator_Radioactivity::fChangeZm10R023	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeZp16R025	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeZp16R115	 = 0.;

G4double WCSimGenerator_Radioactivity::fChangeZm16Flow	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeZm13Flow	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeZm10Flow	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeZ000Flow	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeZp14Flow	 = 0.;
G4double WCSimGenerator_Radioactivity::fChangeZp16Flow	 = 0.;

G4double WCSimGenerator_Radioactivity::fGaussM0	 	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussM1	 	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussM2	 	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussM3	 	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR190M3	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussM4		 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussM5		 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussM6		 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussM7		 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussM8		 = 0.;

G4double WCSimGenerator_Radioactivity::fGaussR190S0	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR190S1	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR190S2	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR190S3	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR190S4	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR190S5	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR190S6	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR190S7	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR190S8	 = 0.;

G4double WCSimGenerator_Radioactivity::fGaussR170S0	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR170S1	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR170S2	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR170S3	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR170S4	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR170S5	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR170S6	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR170S7	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR170S8	 = 0.;

G4double WCSimGenerator_Radioactivity::fGaussR140S0	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR140S1	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR140S2	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR140S3	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR140S4	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR140S5	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR140S6	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR140S7	 = 0.;
G4double WCSimGenerator_Radioactivity::fGaussR140S8	 = 0.;

TGraph* WCSimGenerator_Radioactivity::tFittingR000 	 = 0;
TGraph* WCSimGenerator_Radioactivity::tFittingR025	 = 0;
TGraph* WCSimGenerator_Radioactivity::tFittingR045	 = 0;
TGraph* WCSimGenerator_Radioactivity::tFittingR075 	 = 0;
TGraph* WCSimGenerator_Radioactivity::tFittingR145 	 = 0;
TGraph* WCSimGenerator_Radioactivity::tFittingR175 	 = 0;
TGraph* WCSimGenerator_Radioactivity::tFittingR195 	 = 0;
TGraph* WCSimGenerator_Radioactivity::tFittingR215 	 = 0;

WCSimGenerator_Radioactivity::WCSimGenerator_Radioactivity(WCSimDetectorConstruction* myDC) {
	myDetector = myDC;
	this->Initialize();
}

WCSimGenerator_Radioactivity::~WCSimGenerator_Radioactivity() {
	if ( thRnFunction ) delete thRnFunction;
	if ( tFittingR000 ) delete tFittingR000;
	if ( tFittingR025 ) delete tFittingR025;
	if ( tFittingR045 ) delete tFittingR045;
	if ( tFittingR075 ) delete tFittingR075;
	if ( tFittingR145 ) delete tFittingR145;
	if ( tFittingR175 ) delete tFittingR175;
	if ( tFittingR195 ) delete tFittingR195;
	if ( tFittingR215 ) delete tFittingR215;
}


void WCSimGenerator_Radioactivity::Initialize() {

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
	
	fRnSK_Center = 0.1;  // mBq/m^{3} -> From Nakano-san et al.
	fRnSK_Bottom = 2.44; // mBq/m^{3} -> From Nakano-san et al. 
	//fRnSK_Bottom = 3.84; // mBq/m^{3} -> Estimation from SK-IV
	
	fRn_Border = 19.82;  // mBq/m^{3} -> SK case
	//fRn_Border = 25.10;  // mBq/m^{3} -> HK-40k case
	//fRn_Border = 12.55;  // mBq/m^{3} -> HK-20k case
	
	// Detector size:
	// HK (Should be extract from detector construction?)
	//fHK_Z_max   = 66.8/2.;
	//fHK_R_max   = 64.8/2.;
	fHK_Z_max   = myDetector->GetIDHeight() / CLHEP::m / 2.;
	fHK_R_max   = myDetector->GetIDRadius() / CLHEP::m;
	fHK_R2_max  = fHK_R_max * fHK_R_max;
	
	fHK_Z_reco  = fHK_Z_max - 2.;
	fHK_R_reco  = fHK_R_max - 2.;
	fHK_R2_reco = fHK_R_reco * fHK_R_reco;
	
	// Constant
	fSK_Z_max   = 36.200  / 2.;
	fSK_R_max   = 33.6815 / 2.;
	fSK_R2_max  = fSK_R_max * fSK_R_max;
	
	fSK_Z_reco  = fSK_Z_max - 2.;
	fSK_R_reco  = fSK_R_max - 2.;
	fSK_R2_reco = fSK_R_reco * fSK_R_reco;
	
	fZ_max   = fHK_Z_max;
	fR_max   = fHK_R_max;
	fR2_max  = fR_max * fR_max;
	
	fZ_reco  = fZ_max - 2.;
	fR_reco  = fR_max - 2.;
	fR2_reco = fR_reco * fR_reco;
	
	
	G4int nPMT = myDetector->Get_Pmts()->size();
	fRn_Border = 10. * nPMT / (2. * TMath::Pi() * fHK_R_max * fHK_R_max + 2. * TMath::Pi() * fHK_R_max * fHK_Z_max * 2. );
		
	// Binning:
	fMperBin     = 0.36; //m
	fBin_Z_reco  = (fHK_Z_reco * 2.) / fMperBin;
	fBin_Z_max   = (fHK_Z_max  * 2.) / fMperBin;
	fBin_D_reco  = (fHK_R_reco * 2.) / fMperBin;
	fBin_D_max   = (fHK_R_max  * 2.) / fMperBin;
	fBin_R2_reco = (fHK_R2_reco) / (fMperBin * fMperBin);
	fBin_R2_max  = (fHK_R2_max ) / (fMperBin * fMperBin);
	
	fScenario = 0;
	thRnFunction = 0;
	
	this->SetScenario(0);
}

void WCSimGenerator_Radioactivity::Configuration(G4int iScenario, G4double dLifeTime) {
	
	
	G4cout << " ========================================================================== " << G4endl;
	G4cout << " Configuration for Radioactivity generator " << G4endl;
	
	fScenario = iScenario;
	if ( dLifeTime > 0 )
		fRnLambda = log(2) / (dLifeTime);
	this->SetScenario(iScenario);
	
	G4cout << " Livetime: " << log(2) / fRnLambda << " sec " <<  G4endl;
	G4cout << " Lambda:   " << fRnLambda << "     " <<  G4endl;
	G4cout << " Activity on ID border:   " << fRn_Border << " mBq/m^3  " <<  G4endl;
	G4cout << " PMT number: " << myDetector->Get_Pmts()->size() << G4endl;
	G4cout << " Surface: " << (2. * TMath::Pi() * fHK_R_max * fHK_R_max + 2. * TMath::Pi() * fHK_R_max * fHK_Z_max * 2. ) << G4endl;
	//G4cout << fHK_R_max << " " << fHK_Z_max * 2 << G4endl;
	G4cout << " Mean activity in the full ID:   " << fIntegral << " mBq  " <<  G4endl;
	G4cout << " ========================================================================== " << G4endl;
}

void WCSimGenerator_Radioactivity::SetScenario(G4int iScenario) {

	fScenario = iScenario;

	if ( fScenario == 1 ) {
		// Scenario A: Every thing is scaled to Max
		fScaleTypeInsideZ   =  fZ_max /  fSK_Z_max;
		fScaleTypeInsideR   =  fR_max /  fSK_R_max;
		fScaleTypeInsideR2  = fR2_max / fSK_R2_max;

		fScaleTypeInStrucZ1 = 0.;
		fScaleTypeInStrucZ2 = fZ_reco / fSK_Z_reco; // Scalling 
		fScaleTypeInStrucZ3 = 0.;

		fScaleTypeOutsideZ1 = 0.;
		fScaleTypeOutsideZ2 =  fZ_max / fSK_Z_max; // Scalling 
		fScaleTypeOutsideZ3 = 0.;

		fScaleTypeOutsideR1 = 0.;
		fScaleTypeOutsideR2 =  fR_max / fSK_R_max; // Scalling 
		fScaleTypeOutsideR3 = 0.;
	}
	else if ( fScenario == 2 ) {
		// Scenario B: Every thing inside the fiduicial volume is scaled to Reco, every else is scale to Max-Reco
		fScaleTypeInsideZ   =  fZ_reco /  fSK_Z_reco;
		fScaleTypeInsideR   =  fR_reco /  fSK_R_reco;
		fScaleTypeInsideR2  = fR2_reco / fSK_R2_reco;

		fScaleTypeInStrucZ1 = 0.;
		fScaleTypeInStrucZ2 = fZ_reco / fSK_Z_reco; // Scalling 
		fScaleTypeInStrucZ3 = 0.;

		fScaleTypeOutsideZ1 = fSK_Z_max;
		fScaleTypeOutsideZ2 = (fZ_max - fZ_reco) / (fSK_Z_max - fSK_Z_reco); // Scalling 
		fScaleTypeOutsideZ3 = fZ_max;

		fScaleTypeOutsideR1 = fSK_R_max;
		fScaleTypeOutsideR2 = (fR_max - fR_reco) / (fSK_R_max - fSK_R_reco); // Scalling 
		fScaleTypeOutsideR3 = fR_max;
	
	}
	else if ( fScenario == 3 ) {
		// Scenario C: Z: Structure at -10m and -8m are absolute from bottom, R2: Flow is absolute from border, other structure are scaled to Reco
		fScaleTypeInsideZ   =  fZ_reco /  fSK_Z_reco;
		fScaleTypeInsideR   =  fR_reco /  fSK_R_reco;
		fScaleTypeInsideR2  = fR2_reco / fSK_R2_reco;

		fScaleTypeInStrucZ1 = fSK_Z_max;
		fScaleTypeInStrucZ2 = 1.; // No scalling
		fScaleTypeInStrucZ3 = fZ_max;

		fScaleTypeOutsideZ1 = fSK_Z_max;
		fScaleTypeOutsideZ2 = 1.; // No scalling
		fScaleTypeOutsideZ3 = fZ_max;

		fScaleTypeOutsideR1 = fSK_R_max;
		fScaleTypeOutsideR2 = 1.; // No scalling 
		fScaleTypeOutsideR3 = fR_max;
	
	}
	else {
		// Default scenario (Scenario C)
		fScaleTypeInsideZ   =  fZ_reco /  fSK_Z_reco;
		fScaleTypeInsideR   =  fR_reco /  fSK_R_reco;
		fScaleTypeInsideR2  = fR2_reco / fSK_R2_reco;

		fScaleTypeInStrucZ1 = fSK_Z_max;
		fScaleTypeInStrucZ2 = 1.; // No scalling
		fScaleTypeInStrucZ3 = fZ_max;

		fScaleTypeOutsideZ1 = fSK_Z_max;
		fScaleTypeOutsideZ2 = 1.; // No scalling
		fScaleTypeOutsideZ3 = fZ_max;

		fScaleTypeOutsideR1 = fSK_R_max;
		fScaleTypeOutsideR2 = 1.; // No scalling 
		fScaleTypeOutsideR3 = fR_max;
	}
	
	// Compute positions following scenario:	
	fChangeZPMax   = 	 fZ_max;
	fChangeZNMax   = -1.0   * fZ_max;
	fChangeRMax    = fR2_max;
	
	fR2_000 =   0.; 
	fR2_025 =  25. * fScaleTypeInsideR2; 
	fR2_045 =  45. * fScaleTypeInsideR2; 
	fR2_075 =  75. * fScaleTypeInsideR2; 
	fR2_145 = 145. * fScaleTypeInsideR2;
	fR2_175 = 175. * fScaleTypeInsideR2;
	fR2_195 = 195. * fScaleTypeInsideR2;
	fR2_215 = 220. * fScaleTypeInsideR2;
		
	fR_000 = 0; 
	fR_025 = sqrt( 25.) * fScaleTypeInsideR; 
	fR_045 = sqrt( 45.) * fScaleTypeInsideR; 
	fR_075 = sqrt( 75.) * fScaleTypeInsideR; 
	fR_145 = sqrt(145.) * fScaleTypeInsideR;
	fR_175 = sqrt(175.) * fScaleTypeInsideR;
	fR_195 = sqrt(195.) * fScaleTypeInsideR;
	fR_215 = sqrt(220.) * fScaleTypeInsideR;	
		
	//fZ_p16 = ( 15.3 - fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 + fScaleTypeInStrucZ3; // before tunning
	//fZ_p14 = ( 15.3 - fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 + fScaleTypeInStrucZ3; // before tunning
	//fZ_m16 = (-15.3 + fScaleTypeOutsideZ1) * fScaleTypeOutsideZ2 - fScaleTypeOutsideZ3; // before tunning

	fZ_p16 = ( 15.7 - fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 + fScaleTypeInStrucZ3; // Inside reco-volume structure
	fZ_p14 = ( 14.0 - fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 + fScaleTypeInStrucZ3; // Inside reco-volume structure
	fZ_000 = 0.; 
	fZ_m10 = (-10.2 + fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 - fScaleTypeInStrucZ3; // Inside reco-volume structure
	fZ_m13 = (-13.0 + fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 - fScaleTypeInStrucZ3; // Inside reco-volume structure
	fZ_m16 = (-16.7 + fScaleTypeOutsideZ1) * fScaleTypeOutsideZ2 - fScaleTypeOutsideZ3; // Outside reco-volume structure
	
	// Change point for R210:
	fChangeR210N13 = (-13.032  + fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 - fScaleTypeInStrucZ3; // -13m  Inside reco-volume structure
	fChangeR210N10 = (- 9.955  + fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 - fScaleTypeInStrucZ3; // -10m  Inside reco-volume structure
	fChangeR210N5  =  - 4.9956 * fScaleTypeInsideZ;
	fChangeR210P1  =    0.9955 * fScaleTypeInsideZ;
	fChangeR210P8  =    7.783  * fScaleTypeInsideZ;
	fChangeR210P13 =   13.5026 * fScaleTypeInsideZ;
	
	// Change point for R190:
	fChangeR190N10 = (-10.136  + fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 - fScaleTypeInStrucZ3; // -10m  Inside reco-volume structure
	fChangeR190N8  = (- 7.964  + fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 - fScaleTypeInStrucZ3; // - 8m  Inside reco-volume structure

	// Change point for R170:
	fChangeR170N10 = (-10.136  + fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 - fScaleTypeInStrucZ3; // -10m  Inside reco-volume structure
	fChangeR170N8  = (- 7.964  + fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 - fScaleTypeInStrucZ3; // - 8m  Inside reco-volume structure

	// Change point for R140:
	fChangeR140N10 = (-10.136  + fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 - fScaleTypeInStrucZ3; // -10m  Inside reco-volume structure
	fChangeR140N8  = (- 8.4527 + fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 - fScaleTypeInStrucZ3; // -8.5m Inside reco-volume structure

	// Change point for R070:
	fChangeR070N10 = (-10.136  + fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 - fScaleTypeInStrucZ3; // -10m  Inside reco-volume structure
	fChangeR070N8  = (- 7.964  + fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 - fScaleTypeInStrucZ3; // - 8m  Inside reco-volume structure

	// Change point for R040:
	fChangeR040N10 = (-10.679  + fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 - fScaleTypeInStrucZ3; // -10m  Inside reco-volume structure

	// Change point for R020:
	fChangeR020N10 = (-10.498  + fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 - fScaleTypeInStrucZ3; // -10m  Inside reco-volume structure

	// Change point for R000:
	fChangeR000N10 = (-10.86   + fScaleTypeInStrucZ1) * fScaleTypeInStrucZ2 - fScaleTypeInStrucZ3; // -10m  Inside reco-volume structure

	// Change point for ZmMx:
	fChangeZmMxR025 =  25. 	   * fScaleTypeInsideR2; 
	fChangeZmMxR175 = 175. 	   * fScaleTypeInsideR2; 

	// Change point for Zm16:
	fChangeZm16R025 =  25. 	   * fScaleTypeInsideR2; 
	fChangeZm16R125 = 125. 	   * fScaleTypeInsideR2; 

	// Change point for Zm13:
	fChangeZm13R023 =  23. 	   * fScaleTypeInsideR2; 
	fChangeZm13R080 =  80. 	   * fScaleTypeInsideR2; 

	// Change point for Zm10:
	fChangeZm10R023 =  23. 	   * fScaleTypeInsideR2; 

	// Change point for Zp16:
	fChangeZp16R025 =  25. 	   * fScaleTypeInsideR2;
	fChangeZp16R115 = 115. 	   * fScaleTypeInsideR2;
	
	// Barrel flow structure
	fChangeZm16Flow = pow(( (sqrt(260.92199) - fScaleTypeOutsideR1) * fScaleTypeOutsideR2 + fScaleTypeOutsideR3), 2.);// Outside reco-volume structure
	fChangeZm13Flow = pow(( (sqrt(252.41367) - fScaleTypeOutsideR1) * fScaleTypeOutsideR2 + fScaleTypeOutsideR3), 2.);// Outside reco-volume structure
	fChangeZm10Flow = pow(( (sqrt(255.24977) - fScaleTypeOutsideR1) * fScaleTypeOutsideR2 + fScaleTypeOutsideR3), 2.);// Outside reco-volume structure
	fChangeZ000Flow = pow(( (sqrt(243.90534) - fScaleTypeOutsideR1) * fScaleTypeOutsideR2 + fScaleTypeOutsideR3), 2.);// Outside reco-volume structure
	fChangeZp14Flow = pow(( (sqrt(252.41367) - fScaleTypeOutsideR1) * fScaleTypeOutsideR2 + fScaleTypeOutsideR3), 2.);// Outside reco-volume structure
	fChangeZp16Flow = pow(( (sqrt(258.08588) - fScaleTypeOutsideR1) * fScaleTypeOutsideR2 + fScaleTypeOutsideR3), 2.);// Outside reco-volume structure

	// Gaussian shapes:	
	// R190
	fGaussM0     = - 3.0000001  * fScaleTypeInsideZ;
	fGaussM1     = - 1.0000000  * fScaleTypeInsideZ;
	fGaussM2     =   1.2859100  * fScaleTypeInsideZ;
	fGaussM3     =   3.1980399  * fScaleTypeInsideZ;
	fGaussR190M3 =   3.2580000  * fScaleTypeInsideZ;
	fGaussM4     =   5.3665300  * fScaleTypeInsideZ;
	fGaussM5     =   7.5549000  * fScaleTypeInsideZ;
	fGaussM6     =   9.6014800  * fScaleTypeInsideZ;
	fGaussM7     =  11.8807000  * fScaleTypeInsideZ;
	fGaussM8     =  13.9810000  * fScaleTypeInsideZ;
	
	fGaussR190S0 =  0.60000000  * fScaleTypeInsideZ;
	fGaussR190S1 =  0.72400000  * fScaleTypeInsideZ;
	fGaussR190S2 =  0.81450000  * fScaleTypeInsideZ;
	fGaussR190S3 =  0.72400000  * fScaleTypeInsideZ;
	fGaussR190S4 =  0.81450000  * fScaleTypeInsideZ;
	fGaussR190S5 =  0.72400000  * fScaleTypeInsideZ;
	fGaussR190S6 =  0.81450000  * fScaleTypeInsideZ;
	fGaussR190S7 =  0.90500000  * fScaleTypeInsideZ;
	fGaussR190S8 =  0.72400000  * fScaleTypeInsideZ;

	// R170
	fGaussR170S0 =  0.60000000  * fScaleTypeInsideZ;
	fGaussR170S1 =  0.72400000  * fScaleTypeInsideZ;
	fGaussR170S2 =  0.72400000  * fScaleTypeInsideZ;
	fGaussR170S3 =  0.72400000  * fScaleTypeInsideZ;
	fGaussR170S4 =  0.81450000  * fScaleTypeInsideZ;
	fGaussR170S5 =  0.72400000  * fScaleTypeInsideZ;
	fGaussR170S6 =  0.72400000  * fScaleTypeInsideZ;
	fGaussR170S7 =  0.81450000  * fScaleTypeInsideZ;
	fGaussR170S8 =  0.81450000  * fScaleTypeInsideZ;

	// R140:
	fGaussR140S0 =  0.60000000  * fScaleTypeInsideZ;
	fGaussR140S1 =  0.60000000  * fScaleTypeInsideZ;
	fGaussR140S2 =  0.60000000  * fScaleTypeInsideZ;
	fGaussR140S3 =  0.60000000  * fScaleTypeInsideZ;
	fGaussR140S4 =  0.70000000  * fScaleTypeInsideZ;
	fGaussR140S5 =  0.60000000  * fScaleTypeInsideZ;
	fGaussR140S6 =  0.60000000  * fScaleTypeInsideZ;
	fGaussR140S7 =  0.70000000  * fScaleTypeInsideZ;
	fGaussR140S8 =  0.70000000  * fScaleTypeInsideZ;
	
	if ( tFittingR000 ) delete tFittingR000;
	if ( tFittingR025 ) delete tFittingR025;
	if ( tFittingR045 ) delete tFittingR045;
	if ( tFittingR075 ) delete tFittingR075;
	if ( tFittingR145 ) delete tFittingR145;
	if ( tFittingR175 ) delete tFittingR175;
	if ( tFittingR195 ) delete tFittingR195;
	if ( tFittingR215 ) delete tFittingR215;
	
	tFittingR000 = new TGraph(8);
	tFittingR025 = new TGraph(8);
	tFittingR045 = new TGraph(8);
	tFittingR075 = new TGraph(8);
	tFittingR145 = new TGraph(8);
	tFittingR175 = new TGraph(8);
	tFittingR195 = new TGraph(8);
	tFittingR215 = new TGraph(8);
	
	double dZaxis = 0;
	int    i      = 0;
	
	dZaxis = -1. * fZ_max;
	tFittingR000->SetPoint(i,dZaxis, R2Fit_ZmM(fR2_000,fRnLambda,1.) / ZFit_R000(dZaxis,fRnLambda,1.) );
	tFittingR025->SetPoint(i,dZaxis, R2Fit_ZmM(fR2_025,fRnLambda,1.) / ZFit_R020(dZaxis,fRnLambda,1.) );
	tFittingR045->SetPoint(i,dZaxis, R2Fit_ZmM(fR2_045,fRnLambda,1.) / ZFit_R040(dZaxis,fRnLambda,1.) );
	tFittingR075->SetPoint(i,dZaxis, R2Fit_ZmM(fR2_075,fRnLambda,1.) / ZFit_R070(dZaxis,fRnLambda,1.) );
	tFittingR145->SetPoint(i,dZaxis, R2Fit_ZmM(fR2_145,fRnLambda,1.) / ZFit_R140(dZaxis,fRnLambda,1.) );
	tFittingR175->SetPoint(i,dZaxis, R2Fit_ZmM(fR2_175,fRnLambda,1.) / ZFit_R170(dZaxis,fRnLambda,1.) );
	tFittingR195->SetPoint(i,dZaxis, R2Fit_ZmM(fR2_195,fRnLambda,1.) / ZFit_R190(dZaxis,fRnLambda,1.) );
	tFittingR215->SetPoint(i,dZaxis, R2Fit_ZmM(fR2_215,fRnLambda,1.) / ZFit_R210(dZaxis,fRnLambda,1.) );
	i += 1;
	
	dZaxis = fZ_m16;
	tFittingR000->SetPoint(i,dZaxis, R2Fit_Z16(fR2_000,fRnLambda,1.) / ZFit_R000(dZaxis,fRnLambda,1.) );
	tFittingR025->SetPoint(i,dZaxis, R2Fit_Z16(fR2_025,fRnLambda,1.) / ZFit_R020(dZaxis,fRnLambda,1.) );
	tFittingR045->SetPoint(i,dZaxis, R2Fit_Z16(fR2_045,fRnLambda,1.) / ZFit_R040(dZaxis,fRnLambda,1.) );
	tFittingR075->SetPoint(i,dZaxis, R2Fit_Z16(fR2_075,fRnLambda,1.) / ZFit_R070(dZaxis,fRnLambda,1.) );
	tFittingR145->SetPoint(i,dZaxis, R2Fit_Z16(fR2_145,fRnLambda,1.) / ZFit_R140(dZaxis,fRnLambda,1.) );
	tFittingR175->SetPoint(i,dZaxis, R2Fit_Z16(fR2_175,fRnLambda,1.) / ZFit_R170(dZaxis,fRnLambda,1.) );
	tFittingR195->SetPoint(i,dZaxis, R2Fit_Z16(fR2_195,fRnLambda,1.) / ZFit_R190(dZaxis,fRnLambda,1.) );
	tFittingR215->SetPoint(i,dZaxis, R2Fit_Z16(fR2_215,fRnLambda,1.) / ZFit_R210(dZaxis,fRnLambda,1.) );
	i += 1;
	
	dZaxis = fZ_m13;
	tFittingR000->SetPoint(i,dZaxis, R2Fit_Z13(fR2_000,fRnLambda,1.) / ZFit_R000(dZaxis,fRnLambda,1.) );
	tFittingR025->SetPoint(i,dZaxis, R2Fit_Z13(fR2_025,fRnLambda,1.) / ZFit_R020(dZaxis,fRnLambda,1.) );
	tFittingR045->SetPoint(i,dZaxis, R2Fit_Z13(fR2_045,fRnLambda,1.) / ZFit_R040(dZaxis,fRnLambda,1.) );
	tFittingR075->SetPoint(i,dZaxis, R2Fit_Z13(fR2_075,fRnLambda,1.) / ZFit_R070(dZaxis,fRnLambda,1.) );
	tFittingR145->SetPoint(i,dZaxis, R2Fit_Z13(fR2_145,fRnLambda,1.) / ZFit_R140(dZaxis,fRnLambda,1.) );
	tFittingR175->SetPoint(i,dZaxis, R2Fit_Z13(fR2_175,fRnLambda,1.) / ZFit_R170(dZaxis,fRnLambda,1.) );
	tFittingR195->SetPoint(i,dZaxis, R2Fit_Z13(fR2_195,fRnLambda,1.) / ZFit_R190(dZaxis,fRnLambda,1.) );
	tFittingR215->SetPoint(i,dZaxis, R2Fit_Z13(fR2_215,fRnLambda,1.) / ZFit_R210(dZaxis,fRnLambda,1.) );
	i += 1;
	
	dZaxis = fZ_m10;
	tFittingR000->SetPoint(i,dZaxis, R2Fit_Z10(fR2_000,fRnLambda,1.) / ZFit_R000(dZaxis,fRnLambda,1.) );
	tFittingR025->SetPoint(i,dZaxis, R2Fit_Z10(fR2_025,fRnLambda,1.) / ZFit_R020(dZaxis,fRnLambda,1.) );
	tFittingR045->SetPoint(i,dZaxis, R2Fit_Z10(fR2_045,fRnLambda,1.) / ZFit_R040(dZaxis,fRnLambda,1.) );
	tFittingR075->SetPoint(i,dZaxis, R2Fit_Z10(fR2_075,fRnLambda,1.) / ZFit_R070(dZaxis,fRnLambda,1.) );
	tFittingR145->SetPoint(i,dZaxis, R2Fit_Z10(fR2_145,fRnLambda,1.) / ZFit_R140(dZaxis,fRnLambda,1.) );
	tFittingR175->SetPoint(i,dZaxis, R2Fit_Z10(fR2_175,fRnLambda,1.) / ZFit_R170(dZaxis,fRnLambda,1.) );
	tFittingR195->SetPoint(i,dZaxis, R2Fit_Z10(fR2_195,fRnLambda,1.) / ZFit_R190(dZaxis,fRnLambda,1.) );
	tFittingR215->SetPoint(i,dZaxis, R2Fit_Z10(fR2_215,fRnLambda,1.) / ZFit_R210(dZaxis,fRnLambda,1.) );
	i += 1;
	
	dZaxis = 0.;
	tFittingR000->SetPoint(i,dZaxis, R2Fit_Z00(fR2_000,fRnLambda,1.) / ZFit_R000(dZaxis,fRnLambda,1.) );
	tFittingR025->SetPoint(i,dZaxis, R2Fit_Z00(fR2_025,fRnLambda,1.) / ZFit_R020(dZaxis,fRnLambda,1.) );
	tFittingR045->SetPoint(i,dZaxis, R2Fit_Z00(fR2_045,fRnLambda,1.) / ZFit_R040(dZaxis,fRnLambda,1.) );
	tFittingR075->SetPoint(i,dZaxis, R2Fit_Z00(fR2_075,fRnLambda,1.) / ZFit_R070(dZaxis,fRnLambda,1.) );
	tFittingR145->SetPoint(i,dZaxis, R2Fit_Z00(fR2_145,fRnLambda,1.) / ZFit_R140(dZaxis,fRnLambda,1.) );
	tFittingR175->SetPoint(i,dZaxis, R2Fit_Z00(fR2_175,fRnLambda,1.) / ZFit_R170(dZaxis,fRnLambda,1.) );
	tFittingR195->SetPoint(i,dZaxis, R2Fit_Z00(fR2_195,fRnLambda,1.) / ZFit_R190(dZaxis,fRnLambda,1.) );
	tFittingR215->SetPoint(i,dZaxis, R2Fit_Z00(fR2_215,fRnLambda,1.) / ZFit_R210(dZaxis,fRnLambda,1.) );
	i += 1;
	
	dZaxis = fZ_p14;
	tFittingR000->SetPoint(i,dZaxis, R2Fit_Z14p(fR2_000,fRnLambda,1.) / ZFit_R000(dZaxis,fRnLambda,1.) );
	tFittingR025->SetPoint(i,dZaxis, R2Fit_Z14p(fR2_025,fRnLambda,1.) / ZFit_R020(dZaxis,fRnLambda,1.) );
	tFittingR045->SetPoint(i,dZaxis, R2Fit_Z14p(fR2_045,fRnLambda,1.) / ZFit_R040(dZaxis,fRnLambda,1.) );
	tFittingR075->SetPoint(i,dZaxis, R2Fit_Z14p(fR2_075,fRnLambda,1.) / ZFit_R070(dZaxis,fRnLambda,1.) );
	tFittingR145->SetPoint(i,dZaxis, R2Fit_Z14p(fR2_145,fRnLambda,1.) / ZFit_R140(dZaxis,fRnLambda,1.) );
	tFittingR175->SetPoint(i,dZaxis, R2Fit_Z14p(fR2_175,fRnLambda,1.) / ZFit_R170(dZaxis,fRnLambda,1.) );
	tFittingR195->SetPoint(i,dZaxis, R2Fit_Z14p(fR2_195,fRnLambda,1.) / ZFit_R190(dZaxis,fRnLambda,1.) );
	tFittingR215->SetPoint(i,dZaxis, R2Fit_Z14p(fR2_215,fRnLambda,1.) / ZFit_R210(dZaxis,fRnLambda,1.) );
	i += 1;
	
	dZaxis = fZ_p16;
	tFittingR000->SetPoint(i,dZaxis, R2Fit_Z16p(fR2_000,fRnLambda,1.) / ZFit_R000(dZaxis,fRnLambda,1.) );
	tFittingR025->SetPoint(i,dZaxis, R2Fit_Z16p(fR2_025,fRnLambda,1.) / ZFit_R020(dZaxis,fRnLambda,1.) );
	tFittingR045->SetPoint(i,dZaxis, R2Fit_Z16p(fR2_045,fRnLambda,1.) / ZFit_R040(dZaxis,fRnLambda,1.) );
	tFittingR075->SetPoint(i,dZaxis, R2Fit_Z16p(fR2_075,fRnLambda,1.) / ZFit_R070(dZaxis,fRnLambda,1.) );
	tFittingR145->SetPoint(i,dZaxis, R2Fit_Z16p(fR2_145,fRnLambda,1.) / ZFit_R140(dZaxis,fRnLambda,1.) );
	tFittingR175->SetPoint(i,dZaxis, R2Fit_Z16p(fR2_175,fRnLambda,1.) / ZFit_R170(dZaxis,fRnLambda,1.) );
	tFittingR195->SetPoint(i,dZaxis, R2Fit_Z16p(fR2_195,fRnLambda,1.) / ZFit_R190(dZaxis,fRnLambda,1.) );
	tFittingR215->SetPoint(i,dZaxis, R2Fit_Z16p(fR2_215,fRnLambda,1.) / ZFit_R210(dZaxis,fRnLambda,1.) );
	i += 1;
	
	dZaxis = fZ_max;
	tFittingR000->SetPoint(i,dZaxis, R2Fit_ZpM(fR2_000,fRnLambda,1.) / ZFit_R000(dZaxis,fRnLambda,1.) );
	tFittingR025->SetPoint(i,dZaxis, R2Fit_ZpM(fR2_025,fRnLambda,1.) / ZFit_R020(dZaxis,fRnLambda,1.) );
	tFittingR045->SetPoint(i,dZaxis, R2Fit_ZpM(fR2_045,fRnLambda,1.) / ZFit_R040(dZaxis,fRnLambda,1.) );
	tFittingR075->SetPoint(i,dZaxis, R2Fit_ZpM(fR2_075,fRnLambda,1.) / ZFit_R070(dZaxis,fRnLambda,1.) );
	tFittingR145->SetPoint(i,dZaxis, R2Fit_ZpM(fR2_145,fRnLambda,1.) / ZFit_R140(dZaxis,fRnLambda,1.) );
	tFittingR175->SetPoint(i,dZaxis, R2Fit_ZpM(fR2_175,fRnLambda,1.) / ZFit_R170(dZaxis,fRnLambda,1.) );
	tFittingR195->SetPoint(i,dZaxis, R2Fit_ZpM(fR2_195,fRnLambda,1.) / ZFit_R190(dZaxis,fRnLambda,1.) );
	tFittingR215->SetPoint(i,dZaxis, R2Fit_ZpM(fR2_215,fRnLambda,1.) / ZFit_R210(dZaxis,fRnLambda,1.) );
	/*
	tFittingR000->SetBit(TGraph::kIsSortedX);
	tFittingR000->SetBit(TGraph::kIsSortedX);
	tFittingR025->SetBit(TGraph::kIsSortedX);
	tFittingR045->SetBit(TGraph::kIsSortedX);
	tFittingR075->SetBit(TGraph::kIsSortedX);
	tFittingR145->SetBit(TGraph::kIsSortedX);
	tFittingR175->SetBit(TGraph::kIsSortedX);
	tFittingR195->SetBit(TGraph::kIsSortedX);
	tFittingR215->SetBit(TGraph::kIsSortedX);
	*/
	// Generate TF2:
	if ( thRnFunction ) delete thRnFunction;
	thRnFunction = new TF2("RndPos",RadonFormula,0.,fR2_max,-1.*fZ_max,fZ_max,2);
	thRnFunction->SetNpx(fBin_R2_max);
	thRnFunction->SetNpy(fBin_Z_max );
	
	thRnFunction->SetParameter(0,fRnLambda);
	thRnFunction->SetParameter(1,fMperBin);
	
	// Compute activity Integral
	fIntegral = thRnFunction->Integral(0,fR_max*fR_max,-1.*fZ_max,fZ_max);
}
	

G4ThreeVector WCSimGenerator_Radioactivity::GetRandomVertex() {
	
	G4double R2 = 0;
	G4double Z  = 0;
	
	G4double& R2_ref = R2;
	G4double& Z_ref = Z;
	
	thRnFunction->GetRandom2(R2_ref,Z_ref);
	G4double theta = G4UniformRand() * 2. * TMath::Pi();
	
	G4double X = sqrt(R2) * cos(theta);
	G4double Y = sqrt(R2) * sin(theta);
	
	G4ThreeVector vec(X * CLHEP::m, Y * CLHEP::m, Z * CLHEP::m);
	
	return vec;
}


G4double WCSimGenerator_Radioactivity::ZFit_R210(G4double x, G4double Lambda, G4double BinConversion) {

	G4double dConc0      = BinConversion * fRnSK_Center;      //   0m
	G4double dConc10     = BinConversion * fRnSK_Bottom;      // -10m (12m)
	G4double dConcPMT    = BinConversion * fRn_Border;	// Border (PMT)
				
	G4double dConc       = dConc0										// Center
			     + dConcPMT  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*8.)  ) * (fChangeZPMax - x) )	// PMT Top
			     + dConc0*3. / cosh( sqrt(Lambda/(fRnDiffusion_Coef*150.)) * (fChangeZPMax - x) )	// PMT Top
			     + dConcPMT  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*12.) ) * (x - fChangeZNMax) );	// PMT Bottom 
			   
	if ( x <= fChangeR210N13 ) {
		dConc      += dConc10 * 0.85 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*120.)) * (fChangeR210N13 - x) );
	}
	else if ( x < fChangeR210P8 ) {
		dConc 	   += dConc10 * 0.85;
	}
	else if ( x < fChangeR210P13 ) {
		dConc 	   += dConc10 * 0.65;
	}
	
	if ( x <= fChangeR210N10 ) {
		dConc      += dConc10 * 0.15 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*20.)) * (fChangeR210N10 - x) );
	}
	else if ( x < fChangeR210P8 ) {
		dConc 	   += dConc10 * 0.15;
	}
	if ( x <= fChangeR210N5 ) {
		dConc      += dConc10 * 0.28 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*10.)) * (fChangeR210N5 - x) );
	}
	else if ( x < fChangeR210P1 ) {
		dConc 	   += dConc10 * 0.28;
	}
	
	if ( x >= fChangeR210P1 ) {
		dConc      += dConc10 * 0.28 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*400.)) * (x - fChangeR210P1) );
	}
	if ( x >= fChangeR210P8 ) {
		dConc      += dConc10 * 0.35 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*20.)) * (x - fChangeR210P8) );
	}
	if ( x >= fChangeR210P13 ) {
		dConc      += dConc10 * 0.65 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*80.)) * (x - fChangeR210P13) );
	}
			   
	return dConc;
	
}

G4double WCSimGenerator_Radioactivity::ZFit_R190(G4double x, G4double Lambda, G4double BinConversion) {
		
	G4double dConc0      = BinConversion * fRnSK_Center;		//   0m
	G4double dConc10     = BinConversion * fRnSK_Bottom * 0.825;	// -10m (12m)
	G4double dConcPMT    = BinConversion * fRn_Border;		// Border (PMT)
		
	G4double dConcG0 =  dConc10 * 0.031 * 1;
	G4double dConcG1 =  dConc10 * 0.031 * 4;
	G4double dConcG2 =  dConc10 * 0.031 * 7.5;
	G4double dConcG3 =  dConc10 * 0.031 * 9;
	G4double dConcG4 =  dConc10 * 0.031 * 10.;
	G4double dConcG5 =  dConc10 * 0.031 * 9.5;
	G4double dConcG6 =  dConc10 * 0.031 * 8;
	G4double dConcG7 =  dConc10 * 0.031 * 7;
	G4double dConcG8 =  dConc10 * 0.031 * 5;
				
	G4double dConc     = dConc0										// Center
			   + dConcPMT  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*10.) ) * (fChangeZPMax - x) )	// PMT Top
			   + dConc0*7. / cosh( sqrt(Lambda/(fRnDiffusion_Coef*150.)) * (fChangeZPMax - x) )	// PMT Top
			   + dConcPMT  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*10.) ) * (x - fChangeZNMax) )	// PMT Bottom 
			   + dConcG0 * TMath::Gaus(x,fGaussM0,fGaussR190S0)					// Gaussian 0
			   + dConcG1 * TMath::Gaus(x,fGaussM1,fGaussR190S1)					// Gaussian 1 
			   + dConcG2 * TMath::Gaus(x,fGaussM2,fGaussR190S2)					// Gaussian 2 
			   + dConcG3 * TMath::Gaus(x,fGaussR190M3,fGaussR190S3)					// Gaussian 3 
			   + dConcG4 * TMath::Gaus(x,fGaussM4,fGaussR190S4)					// Gaussian 4 
			   + dConcG5 * TMath::Gaus(x,fGaussM5,fGaussR190S5)					// Gaussian 5 
			   + dConcG6 * TMath::Gaus(x,fGaussM6,fGaussR190S6)					// Gaussian 6 
			   + dConcG7 * TMath::Gaus(x,fGaussM7,fGaussR190S7)					// Gaussian 7 
			   + dConcG8 * TMath::Gaus(x,fGaussM8,fGaussR190S8);					// Gaussian 8
			   
	G4double dNorm09  = max((dConc10*0.9-dConc0),0.);
	G4double dNorm06  = max((dConc10*0.6-dConc0),0.);
	G4double dNorm035 = max((dConc10*0.35-dConc0),0.);
		
	if ( x >= fChangeR190N10 ) { // transition to Wall
		dConc += dNorm035 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*20.)) * (x-fChangeR190N10) ); 	
	}
	else { // transition to Wall
		dConc  += dNorm09*0.3  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*10.)) * (fChangeR190N10-x) )
			+ dNorm09*0.7 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*1600.)) * (fChangeR190N10-x) );
	}
	
	
	if ( x >= fChangeR190N8 ) { // bump	
		dConc     += dNorm06 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*600.)) * (x-fChangeR190N8) ); 	
	}
	else if ( x >= fChangeR190N10) { // Bottom Wall
		dConc     += dNorm06;
	}
	
	return dConc;
}

G4double WCSimGenerator_Radioactivity::ZFit_R170(G4double x, G4double Lambda, G4double BinConversion) {
		
	G4double dConc0      = BinConversion * fRnSK_Center;         //   0m
	G4double dConc10     = BinConversion * fRnSK_Bottom * 0.825; // -10m (12m)
	G4double dConcPMT    = BinConversion * fRn_Border;	// Border (PMT)

	G4double dConcG0 =  dConc10 * 0.031 * 2.5;
	G4double dConcG1 =  dConc10 * 0.031 * 2.8;
	G4double dConcG2 =  dConc10 * 0.031 * 5;
	G4double dConcG3 =  dConc10 * 0.031 * 5;
	G4double dConcG4 =  dConc10 * 0.031 * 6.;
	G4double dConcG5 =  dConc10 * 0.031 * 6.;
	G4double dConcG6 =  dConc10 * 0.031 * 5;
	G4double dConcG7 =  dConc10 * 0.031 * 3.5;
	G4double dConcG8 =  dConc10 * 0.031 * 2;
			
	G4double dConc     = dConc0										// Center
			   + dConcPMT  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*10.) ) * (fChangeZPMax - x) )	// PMT Top
			   + dConc0*7. / cosh( sqrt(Lambda/(fRnDiffusion_Coef*150.)) * (fChangeZPMax - x) )	// PMT Top
			   + dConcPMT  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*12.) ) * (x - fChangeZNMax) )	// PMT Bottom 
			   + dConcG0 * TMath::Gaus(x,fGaussM0,fGaussR170S0)					// Gaussian 0 
			   + dConcG1 * TMath::Gaus(x,fGaussM1,fGaussR170S1)					// Gaussian 1 
			   + dConcG2 * TMath::Gaus(x,fGaussM2,fGaussR170S2)					// Gaussian 2 
			   + dConcG3 * TMath::Gaus(x,fGaussM3,fGaussR170S3)					// Gaussian 3 
			   + dConcG4 * TMath::Gaus(x,fGaussM4,fGaussR170S4)					// Gaussian 4 
			   + dConcG5 * TMath::Gaus(x,fGaussM5,fGaussR170S5)					// Gaussian 5 
			   + dConcG6 * TMath::Gaus(x,fGaussM6,fGaussR170S6)					// Gaussian 6 
			   + dConcG7 * TMath::Gaus(x,fGaussM7,fGaussR170S7)					// Gaussian 7 
			   + dConcG8 * TMath::Gaus(x,fGaussM8,fGaussR170S8);					// Gaussian 8 
		
	G4double dNorm055 = max((dConc10*0.55-dConc0),0.);
	G4double dNorm045 = max((dConc10*0.45-dConc0),0.);
	
	if ( x >= fChangeR170N10 ) { // transition to Wall
		dConc += dNorm045 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*10.)) * (x-fChangeR170N10) ); 	
	}
	else { // transition to Wall
		dConc  += dNorm045*0.4  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*10.)) * (fChangeR170N10-x) )
			+ dNorm045*0.6 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*80.)) * (fChangeR170N10-x) ); 	
	}
	
	
	if ( x >= fChangeR170N8 ) { // bump	
		dConc     += dNorm055 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*200.)) * (x-fChangeR170N8) ); 	
	}
	else { // Bottom Wall
		dConc     += dNorm055;
	}
	
	return dConc;
}

G4double WCSimGenerator_Radioactivity::ZFit_R140(G4double x, G4double Lambda, G4double BinConversion) {
	
		
	G4double dConc0      = BinConversion * fRnSK_Center;		//   0m
	G4double dConc10     = BinConversion * fRnSK_Bottom * 0.837;	// -10m (12m)
	G4double dConcPMT    = BinConversion * fRn_Border;		// Border (PMT)
	
	// Gaus 0:
	G4double dConcG0 =  dConc10 * 0.031 * 0.5;
	G4double dConcG1 =  dConc10 * 0.031 * 1.;
	G4double dConcG2 =  dConc10 * 0.031 * 2.5;
	G4double dConcG3 =  dConc10 * 0.031 * 2.5;
	G4double dConcG4 =  dConc10 * 0.031 * 3.;
	G4double dConcG5 =  dConc10 * 0.031 * 3.;
	G4double dConcG6 =  dConc10 * 0.031 * 2.8;
	G4double dConcG7 =  dConc10 * 0.031 * 1.5;
	G4double dConcG8 =  dConc10 * 0.031 * 0.5;
			
	G4double dConc     = dConc0										// Center
			   + dConcPMT  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*10.) ) * (fChangeZPMax - x) )	// PMT Top
			   + dConc0*7. / cosh( sqrt(Lambda/(fRnDiffusion_Coef*150.)) * (fChangeZPMax - x) )	// PMT Top
			   + dConcPMT  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*15.) ) * (x - fChangeZNMax) )	// PMT Bottom 
			   + dConcG0 * TMath::Gaus(x,fGaussM0,fGaussR140S0)					// Gaussian 0 
			   + dConcG1 * TMath::Gaus(x,fGaussM1,fGaussR140S1)					// Gaussian 1
			   + dConcG2 * TMath::Gaus(x,fGaussM2,fGaussR140S2)					// Gaussian 2 
			   + dConcG3 * TMath::Gaus(x,fGaussM3,fGaussR140S3)					// Gaussian 3 
			   + dConcG4 * TMath::Gaus(x,fGaussM4,fGaussR140S4)					// Gaussian 4 
			   + dConcG5 * TMath::Gaus(x,fGaussM5,fGaussR140S5)					// Gaussian 5 
			   + dConcG6 * TMath::Gaus(x,fGaussM6,fGaussR140S6)					// Gaussian 6 
			   + dConcG7 * TMath::Gaus(x,fGaussM7,fGaussR140S7)					// Gaussian 6 
			   + dConcG8 * TMath::Gaus(x,fGaussM8,fGaussR140S8);					// Gaussian 6 
			   
	G4double dNorm05 = max((dConc10*0.5-dConc0),0.);
		
	if ( x >= fChangeR140N10 ) { // transition to Wall
		dConc += dNorm05 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*10.)) * (x-fChangeR140N10) ); 	
	}
	else { // transition to Wall
		dConc  += dNorm05*0.4  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*10.)) * (fChangeR140N10-x) )
			+ dNorm05*0.6 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*200.)) * (fChangeR140N10-x) ); 	
	}
	
	
	if ( x >= fChangeR140N8 ) { // bump
		dConc     += dNorm05 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*175.)) * (x-fChangeR140N8) ); 	
	}
	else { // Bottom Wall
		dConc     += dNorm05;
	}
	
	return dConc;
}

G4double WCSimGenerator_Radioactivity::ZFit_R070(G4double x, G4double Lambda, G4double BinConversion) {
		
	G4double dConc0      = BinConversion * fRnSK_Center;		//   0m
	G4double dConc10     = BinConversion * fRnSK_Bottom * 0.825;	// -10m (12m)
	G4double dConcPMT    = BinConversion * fRn_Border;		// Border (PMT)
			
	G4double dConc       = dConc0										// Center
			     + dConcPMT  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*10.) ) * (fChangeZPMax - x) )	// PMT Top
			     + dConc0*3. / cosh( sqrt(Lambda/(fRnDiffusion_Coef*150.)) * (fChangeZPMax - x) )	// PMT Top
			     + dConcPMT  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*15.) ) * (x - fChangeZNMax) );	// PMT Bottom 
	
	G4double dNorm075 = max((dConc10*0.75-dConc0),0.);
	G4double dNorm025 = max((dConc10*0.25-dConc0),0.);
	
	if ( x >= fChangeR070N10 ) { // transition to Wall
		dConc 	  += dNorm075 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*10.)) * (x-fChangeR070N10) ); 	
	}
	else { // transition to Wall
		dConc 	  += dNorm075 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*800.)) * (fChangeR070N10-x) ); 	
	}
	
	if ( x >= fChangeR070N8 ) { // bump	
		dConc     += dNorm025 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*70.)) * (x-fChangeR070N8) ); 	
	}
	else { // Bottom Wall
		dConc     += dNorm025;
	}
	
	return dConc;
}

G4double WCSimGenerator_Radioactivity::ZFit_R040(G4double x, G4double Lambda, G4double BinConversion) {
		
	G4double dConc0    = BinConversion * fRnSK_Center;		//   0m
	G4double dConc10   = BinConversion * fRnSK_Bottom * 0.88;	// -10m (12m)
	G4double dConcPMT  = BinConversion * fRn_Border;		// Border (PMT)
			
	G4double dConc     = dConc0										// Center
			   + dConcPMT  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*10.) ) * (fChangeZPMax - x) )	// PMT Top
			   + dConc0*3. / cosh( sqrt(Lambda/(fRnDiffusion_Coef*150.)) * (fChangeZPMax - x) )	// PMT Top
			   + dConcPMT  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*13.) ) * (x - fChangeZNMax) );	// PMT Bottom 
	
	G4double dNorm10 = max((dConc10-dConc0),0.);
	
	if ( x >= fChangeR040N10 ) { // transition to Wall
		dConc += dNorm10 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*35.)) * (x-fChangeR040N10) ); 	
	}
	else { // transition to Wall
		dConc += dNorm10 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*2000.)) * (fChangeR040N10-x) ); 	
	}
	
	return dConc;
}

G4double WCSimGenerator_Radioactivity::ZFit_R020(G4double x, G4double Lambda, G4double BinConversion) {

	G4double dConc0      = BinConversion * fRnSK_Center;		//   0m
	G4double dConc10     = BinConversion * fRnSK_Bottom * 0.97;	// -10m (12m)
	G4double dConcPMT    = BinConversion * fRn_Border;		// Border (PMT)
			
	G4double dConc       = dConc0										// Center
			     + dConcPMT  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*10.) ) * (fChangeZPMax - x) )	// PMT Top
			     + dConc0*3. / cosh( sqrt(Lambda/(fRnDiffusion_Coef*150.)) * (fChangeZPMax - x) )	// PMT Top
			     + dConcPMT  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*13.) ) * (x - fChangeZNMax) );	// PMT Bottom 
	
	G4double dNorm10 = max((dConc10-dConc0),0.);
	
	if ( x >= fChangeR020N10 ) { // transition to Wall
		dConc	  += dNorm10 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*20.)) * (x-fChangeR020N10) ); 	
	}
	else { // transition to Wall
		dConc     += dNorm10;	
	}
	
	return dConc;
}

G4double WCSimGenerator_Radioactivity::ZFit_R000(G4double x, G4double Lambda, G4double BinConversion) {
		
	G4double dConc0      = BinConversion * fRnSK_Center;	//   0m
	G4double dConc10     = BinConversion * fRnSK_Bottom;	// -10m (12m)
	G4double dConcPMT    = BinConversion * fRn_Border;	// Border (PMT)
	
	G4double dConc       = dConc0										// Center
			     + dConcPMT / cosh( sqrt(Lambda/(fRnDiffusion_Coef*12.)) * (fChangeZPMax - x) )	// PMT Top
			     + dConcPMT / cosh( sqrt(Lambda/(fRnDiffusion_Coef*15.)) * (x - fChangeZNMax) );	// PMT Bottom 
	
	
	G4double dNorm10 = max((dConc10-dConc0),0.);
	
	if ( x >= fChangeR000N10 ) { // Transition to bottom Wall
		dConc     += dNorm10 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*17.)) * (x-fChangeR000N10) ); 
	}
	else { // Bottom Wall
		dConc     += dNorm10;
	}
	
	return dConc;

}

G4double WCSimGenerator_Radioactivity::R2Fit_ZpM(G4double x, G4double Lambda, G4double BinConversion) {
	
	G4double dConcPMT    = pow(BinConversion,2.) * fRn_Border;	// Border (PMT)
	
	G4double dConc       = dConcPMT;
	
	return dConc;
}

G4double WCSimGenerator_Radioactivity::R2Fit_ZmM(G4double x, G4double Lambda, G4double BinConversion) {

	G4double dConc0      = pow(BinConversion,2.) * fRnSK_Center;	//   0m
	G4double dConc10     = pow(BinConversion,2.) * fRnSK_Bottom;	// -10m (12m)
	G4double dConcPMT    = pow(BinConversion,2.) * fRn_Border;	// Border (PMT)
	
	G4double dNorm10 = max((dConc10-dConc0),0.);
	
	G4double dConc       = dConcPMT + dConc0
		     	   + dNorm10*0.06 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*400.)) * (sqrt(x)) );
	
	
	if ( x >= fChangeZmMxR025 ) {
		dConc     += dNorm10*0.50 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*25.)) * (sqrt(x) - sqrt(fChangeZmMxR025)) );
	}
	else {
		dConc     += dNorm10*0.50;
	}
	
	if ( x >= fChangeZmMxR175 ) {
		dConc     += dNorm10*0.44 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*20.)) * (sqrt(x) - sqrt(fChangeZmMxR175)) );
	} 
	else {
		dConc     += dNorm10*0.44;
	}
	
	return dConc;
}

G4double WCSimGenerator_Radioactivity::R2Fit_Z16(G4double x, G4double Lambda, G4double BinConversion) {
	
	G4double dConc0      = pow(BinConversion,2.) * fRnSK_Center;		//   0m
	G4double dConc10     = pow(BinConversion,2.) * fRn_Border * 0.39;	// -10m (12m)
	G4double dConcPMT    = pow(BinConversion,2.) * fRn_Border;		// Border (PMT)
		
	G4double dConc       = dConc0; 								  // Center
			   //+ dConcPMT        / cosh( sqrt(Lambda/(fRnDiffusion_Coef*4.)) * (sqrt(dR2Mx) - sqrt(x)) )   // PMT
			   //+ dConcPMT * 0.05 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*150.)) * (sqrt(dR2Mx) - sqrt(x)) );   // Diffusion
	
	G4double dNorm10 = max((dConc10-dConc0),0.);
	
	
	
	if ( x >= fChangeZm16R025 ) {
		dConc     += dNorm10 * 0.36/ cosh( sqrt(Lambda/(fRnDiffusion_Coef*50.)) * (sqrt(x) - sqrt(fChangeZm16R025)) );
	}
	else {
		dConc     += dNorm10 * 0.36;
	}
	if ( x >= fChangeZm16R125 ) {
		dConc     += dNorm10 * 0.64 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*225.)) * (sqrt(x) - sqrt(fChangeZm16R125)) );
	}
	else {
		dConc     += dNorm10 * 0.64;
	}
	
	// Water flow assumed		
	/*   
	double dConcWall   = dConc10 * 20;
	if ( x >= fChangeZm16Flow ) {
		dConc      += dConcWall / cosh( sqrt(Lambda/(fRnDiffusion_Coef*2.5)) * (sqrt(x) - sqrt(fChangeZm16Flow)) ); 
	}
	else {
		dConc      += dConcWall / cosh( sqrt(Lambda/(fRnDiffusion_Coef*2.5)) * (sqrt(fChangeZm16Flow) - sqrt(x)) ); 
	}
	*/
	
	// Water flow with convection is assumed
	if ( x >= fChangeZm16Flow ) {
		dConc      += dConcPMT;
	}
	else {
		dConc	   += dConcPMT        / cosh( sqrt(Lambda/(fRnDiffusion_Coef*7.)) * (sqrt(fChangeZm16Flow) - sqrt(x)) );
	}
	
	
	return dConc;
}

G4double WCSimGenerator_Radioactivity::R2Fit_Z13(G4double x, G4double Lambda, G4double BinConversion) {
	
	G4double dConc0      = pow(BinConversion,2.) * fRnSK_Center;		//   0m
	G4double dConc10     = pow(BinConversion,2.) * fRnSK_Bottom * 1.13;	// -10m (12m)
	G4double dConcPMT    = pow(BinConversion,2.) * fRn_Border;		// Border (PMT)
	
	G4double dConc       = dConc0; 								  // Center
			   //+ dConcPMT        / cosh( sqrt(Lambda/(fRnDiffusion_Coef*4.)) * (sqrt(dR2Mx) - sqrt(x)) )   // PMT
			   //+ dConcPMT * 0.05 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*150.)) * (sqrt(dR2Mx) - sqrt(x)) );   // Diffusion
	
	G4double dNorm10 = max((dConc10-dConc0),0.);
	
	
	
	if ( x >= fChangeZm13R023 ) {
		dConc     += dNorm10 * 0.30/ cosh( sqrt(Lambda/(fRnDiffusion_Coef*50.)) * (sqrt(x) - sqrt(fChangeZm13R023)) );
	}
	else {
		dConc     += dNorm10 * 0.30;
	}
	if ( x >= fChangeZm13R080 ) {
		dConc     += dNorm10 * 0.70 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*400.)) * (sqrt(x) - sqrt(fChangeZm13R080)) );
	}
	else {
		dConc     += dNorm10 * 0.70;
	}
	
	// Water flow assumed		
	/*   
	double dConcWall   = dConc10 * 20;
	if ( x >= fChangeZm13Flow ) {
		dConc      += dConcWall / cosh( sqrt(Lambda/(fRnDiffusion_Coef*4.)) * (sqrt(x) - sqrt(fChangeZm13Flow)) ); 
	}
	else {
		dConc      += dConcWall / cosh( sqrt(Lambda/(fRnDiffusion_Coef*4.)) * (sqrt(fChangeZm13Flow) - sqrt(x)) ); 
	}*/
	
	// Water flow with convection is assumed
	if ( x >= fChangeZm13Flow ) {
		dConc      += dConcPMT * 1.25;
	}
	else {
		dConc	   += dConcPMT * 1.2       / cosh( sqrt(Lambda/(fRnDiffusion_Coef*4.6)) * (sqrt(fChangeZm13Flow) - sqrt(x)) )
			   + dConcPMT * 0.01 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*150.)) * (sqrt(fChangeZm13Flow) - sqrt(x)) );   // Diffusion
	}
	
	return dConc;
}

G4double WCSimGenerator_Radioactivity::R2Fit_Z10(G4double x, G4double Lambda, G4double BinConversion) {

	
	G4double dConc0      = pow(BinConversion,2.) * fRnSK_Center;	//   0m
	G4double dConc10     = pow(BinConversion,2.) * fRnSK_Bottom;	// -10m (12m)
	G4double dConcPMT    = pow(BinConversion,2.) * fRn_Border;	// Border (PMT)
			
	G4double dNorm07     = max((dConc10*0.80-dConc0),0.);
	G4double dNorm01     = max((dConc10*0.10-dConc0),0.);
	
	G4double dConc       = dConc0 								  // Center
			   //+ dConcPMT        / cosh( sqrt(Lambda/(fRnDiffusion_Coef*4.)) * (sqrt(dR2Mx) - sqrt(x)) )   // PMT
			   //+ dConcPMT * 0.05 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*10.)) * (sqrt(dR2Mx) - sqrt(x)) )   // Diffusion
			     + dNorm07;
	
	
	if ( x <= fChangeZm10R023 ) {
		dConc     += dNorm01/ cosh( sqrt(Lambda/(fRnDiffusion_Coef*40.)) * (sqrt(fChangeZm10R023)-sqrt(x)) );
	}
	else {
		dConc     += dNorm01 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*450.)) * (sqrt(x) - sqrt(fChangeZm10R023)) );
	}
	
	// Water flow assumed	
	/*	   
	double dConcWall   = dConc10 * 40;
	if ( x >= fChangeZm10Flow ) {
		dConc      += dConcWall / cosh( sqrt(Lambda/(fRnDiffusion_Coef*2.5)) * (sqrt(x) - sqrt(fChangeZm10Flow)) ); 
	}
	else {
		dConc      += dConcWall / cosh( sqrt(Lambda/(fRnDiffusion_Coef*2.5)) * (sqrt(fChangeZm10Flow) - sqrt(x)) ); 
	}
	*/
	
	// Water flow with convection is assumed
	if ( x >= fChangeZm10Flow ) {
		dConc      += dConcPMT * 1.205;
	}
	else {
		dConc	   += dConcPMT * 1.2     / cosh( sqrt(Lambda/(fRnDiffusion_Coef*2.75)) * (sqrt(fChangeZm10Flow) - sqrt(x)) )
			   + dConcPMT * 0.005 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*250.)) * (sqrt(fChangeZm10Flow) - sqrt(x)) );   // Diffusion
	}
	return dConc;
}

G4double WCSimGenerator_Radioactivity::R2Fit_Z00(G4double x, G4double Lambda, G4double BinConversion) {
	
	G4double dConc0      = pow(BinConversion,2.) * fRnSK_Center;	//   0m
//	G4double dConc10     = pow(BinConversion,2.) * fRnSK_Bottom;	// Border (PMT)
	G4double dConcPMT    = pow(BinConversion,2.) * fRn_Border;	// Border (PMT)
	
	G4double dConc       = dConc0; 	 			  // Center
			   //+ dConcPMT        / cosh( sqrt(Lambda/(fRnDiffusion_Coef*4.))   * (sqrt(fChangeRMax) - sqrt(x)) )   // PMT
			   //+ dConcPMT * 0.05 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*250.))  * (sqrt(fChangeRMax) - sqrt(x)) );   // PMT
	

	// Water flow assumed		   
	/*
	double dConcWall   = dConc10 * 40;
	if ( x >= fChangeZ000Flow ) {
		dConc      += dConcWall / cosh( sqrt(Lambda/(fRnDiffusion_Coef*5.)) * (sqrt(x) - sqrt(fChangeZ000Flow)) ); 
	}
	else {
		dConc      += dConcWall / cosh( sqrt(Lambda/(fRnDiffusion_Coef*5.)) * (sqrt(fChangeZ000Flow) - sqrt(x)) ); 
	}
	*/
	// Water flow with convection is assumed
	if ( x >= fChangeZ000Flow ) {
		dConc      += dConcPMT * 1.44;
	}
	else {
		dConc	   += dConcPMT * 1.4       / cosh( sqrt(Lambda/(fRnDiffusion_Coef*4.5)) * (sqrt(fChangeZ000Flow) - sqrt(x)) )
			   + dConcPMT * 0.02 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*200.))  * (sqrt(fChangeZ000Flow) - sqrt(x)) );   // PMT
	}
	
	return  dConc;
}

G4double WCSimGenerator_Radioactivity::R2Fit_Z14p(G4double x, G4double Lambda, G4double BinConversion) {
	
	G4double dConc0      = pow(BinConversion,2.) * fRnSK_Center;	//   0m
//	G4double dConc10     = pow(BinConversion,2.) * fRnSK_Bottom;	// Border (PMT)
	G4double dConcPMT    = pow(BinConversion,2.) * fRn_Border;	// Border (PMT)
		
	G4double dConc       = dConc0*2; 								  // Center
			   //+ dConcPMT        / cosh( sqrt(Lambda/(fRnDiffusion_Coef*4.))   * (sqrt(fChangeRMax) - sqrt(x)) )   // PMT
			   //+ dConcPMT * 0.05 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*250.))  * (sqrt(fChangeRMax) - sqrt(x)) );   // PMT
	
	// Water flow assumed		   
	/*
	double dConcWall   = dConc10 * 40;
	if ( x >= fChangeZp14Flow ) {
		dConc      += dConcWall / cosh( sqrt(Lambda/(fRnDiffusion_Coef*5.)) * (sqrt(x) - sqrt(fChangeZp14Flow)) ); 
	}
	else {
		dConc      += dConcWall / cosh( sqrt(Lambda/(fRnDiffusion_Coef*5.)) * (sqrt(fChangeZp14Flow) - sqrt(x)) ); 
	}
	*/
	// Water flow with convection is assumed
	if ( x >= fChangeZp14Flow ) {
		dConc      += dConcPMT * 1.22;
	}
	else {
		dConc	   += dConcPMT * 1.2       / cosh( sqrt(Lambda/(fRnDiffusion_Coef*5)) * (sqrt(fChangeZp14Flow) - sqrt(x)) )
			   + dConcPMT * 0.02 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*200.))  * (sqrt(fChangeZp14Flow) - sqrt(x)) );   // PMT
	}
	
	return  dConc;
}


G4double WCSimGenerator_Radioactivity::R2Fit_Z16p(G4double x, G4double Lambda, G4double BinConversion) {

	
	G4double dConc0      = pow(BinConversion,2.) * fRnSK_Center;		//   0m
	G4double dConc10     = pow(BinConversion,2.) * fRn_Border * 0.06;	// -10m (12m)
	G4double dConcPMT    = pow(BinConversion,2.) * fRn_Border;		// Border (PMT)
		
	G4double dConc       = dConc0; 								  // Center
				//+ dConcPMT        / cosh( sqrt(Lambda/(fRnDiffusion_Coef*4.)) * (sqrt(fChangeRMax) - sqrt(x)) )   // PMT
				//+ dConcPMT * 0.05 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*150.)) * (sqrt(fChangeRMax) - sqrt(x)) );   // Diffusion
	
	double dNorm10 = max((dConc10-dConc0),0.);
	
	
	if ( x >= fChangeZp16R025 ) {
		dConc     += dNorm10 * 0.36/ cosh( sqrt(Lambda/(fRnDiffusion_Coef*50.)) * (sqrt(x) - sqrt(fChangeZp16R025)) );
	}
	else {
		dConc     += dNorm10 * 0.36;
	}
	if ( x >= fChangeZp16R115 ) {
		dConc     += dNorm10 * 0.94 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*300.)) * (sqrt(x) - sqrt(fChangeZp16R115)) );
	}
	else {
		dConc     += dNorm10 * 0.30 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*30.)) * (sqrt(fChangeZp16R115)) - sqrt(x)  );
		dConc     += dNorm10 * 0.64;
	}
	
	// Water flow assumed	
	/*
	double dConcWall   = dConc10 * 40;
	double dR2Floz      = R2_max * 0.94;
	if ( x >= dR2Floz ) {
		dConc      += dConcWall / cosh( sqrt(Lambda/(fRnDiffusion_Coef*5)) * (sqrt(x) - sqrt(dR2Floz)) ); 
	}
	else {
		dConc      += dConcWall / cosh( sqrt(Lambda/(fRnDiffusion_Coef*5)) * (sqrt(dR2Floz) - sqrt(x)) ); 
	}
	*/
	// Water flow with convection is assumed

	if ( x >= fChangeZp16Flow ) {
		dConc      += dConcPMT * 1.13;
	}
	else {
		dConc	   += dConcPMT * 1.1  / cosh( sqrt(Lambda/(fRnDiffusion_Coef*5)   ) * (sqrt(fChangeZp16Flow) - sqrt(x)) )
			   +  dConcPMT * 0.03 / cosh( sqrt(Lambda/(fRnDiffusion_Coef*250.)) * (sqrt(fChangeZp16Flow) - sqrt(x)) );   // PMT
	}
	
	return dConc;
}

G4double WCSimGenerator_Radioactivity::RadonFormula(G4double *val, G4double *par) {

	G4double R2 = val[0];
	G4double Z  = val[1];
	G4double R  = sqrt(R2);
	
	G4double Lambda        = par[0];
	G4double BinConversion = par[1];
	
	//G4double dCZ  = 0;
	
	G4double dCZ1 = 0;
	G4double dCZ2 = 0;
	
	G4double dDZ1 = 0;
	G4double dDZ2 = 0;
	
	G4double dZZ1 = 0;
	G4double dZZ2 = 0;
		
	if ( R2 < fR2_025 ) {
		dCZ1 = WCSimGenerator_Radioactivity::ZFit_R000(Z,Lambda,BinConversion) * tFittingR000->Eval(Z)/BinConversion;
		dCZ2 = WCSimGenerator_Radioactivity::ZFit_R020(Z,Lambda,BinConversion) * tFittingR025->Eval(Z)/BinConversion;
				
		dDZ1 = 1. - (R - fR_000)/ (fR_025 - fR_000);
		dDZ2 = 1. - (fR_025 - R)/ (fR_025 - fR_000);
		
		dZZ1 = fR2_000;
		dZZ2 = fR2_025;
	}
	else if ( R2 < fR2_045 ) {
		dCZ1 = WCSimGenerator_Radioactivity::ZFit_R020(Z,Lambda,BinConversion) * tFittingR025->Eval(Z)/BinConversion;
		dCZ2 = WCSimGenerator_Radioactivity::ZFit_R040(Z,Lambda,BinConversion) * tFittingR045->Eval(Z)/BinConversion;
		
		dDZ1 = 1. - (R - fR_025)/ (fR_045 - fR_025);
		dDZ2 = 1. - (fR_045 - R)/ (fR_045 - fR_025);
		
		dZZ1 = fR2_025;
		dZZ2 = fR2_045;
	}
	else if ( R2 < fR2_075 ) {
		dCZ1 = WCSimGenerator_Radioactivity::ZFit_R040(Z,Lambda,BinConversion) * tFittingR045->Eval(Z)/BinConversion;
		dCZ2 = WCSimGenerator_Radioactivity::ZFit_R070(Z,Lambda,BinConversion) * tFittingR075->Eval(Z)/BinConversion;
		
		dDZ1 = 1. - (R - fR_045)/ (fR_075 - fR_045);
		dDZ2 = 1. - (fR_075 - R)/ (fR_075 - fR_045);
		
		dZZ1 = fR2_045;
		dZZ2 = fR2_075;
	}
	else if ( R2 < fR2_145 ) {
		dCZ1 = WCSimGenerator_Radioactivity::ZFit_R070(Z,Lambda,BinConversion) * tFittingR075->Eval(Z)/BinConversion;
		dCZ2 = WCSimGenerator_Radioactivity::ZFit_R140(Z,Lambda,BinConversion) * tFittingR145->Eval(Z)/BinConversion;
		
		dDZ1 = 1. - (R - fR_075)/ (fR_145 - fR_075);
		dDZ2 = 1. - (fR_145 - R)/ (fR_145 - fR_075);
		
		dZZ1 = fR2_075;
		dZZ2 = fR2_145;
	}
	else if ( R2 < fR2_175 ) {
		dCZ1 = WCSimGenerator_Radioactivity::ZFit_R140(Z,Lambda,BinConversion) * tFittingR145->Eval(Z)/BinConversion;
		dCZ2 = WCSimGenerator_Radioactivity::ZFit_R170(Z,Lambda,BinConversion) * tFittingR175->Eval(Z)/BinConversion;
		
		dDZ1 = 1. - (R - fR_145)/ (fR_175 - fR_145);
		dDZ2 = 1. - (fR_175 - R)/ (fR_175 - fR_145);
		
		dZZ1 = fR2_145;
		dZZ2 = fR2_175;
	}
	else if ( R2 < fR2_195 ) {
		dCZ1 = WCSimGenerator_Radioactivity::ZFit_R170(Z,Lambda,BinConversion) * tFittingR175->Eval(Z)/BinConversion;
		dCZ2 = WCSimGenerator_Radioactivity::ZFit_R190(Z,Lambda,BinConversion) * tFittingR195->Eval(Z)/BinConversion;
		
		dDZ1 = 1. - (R - fR_175)/ (fR_195 - fR_175);
		dDZ2 = 1. - (fR_195 - R)/ (fR_195 - fR_175);
		
		dZZ1 = fR2_175;
		dZZ2 = fR2_195;
	}
	else if ( R2 < fR2_215 ) {
		dCZ1 = WCSimGenerator_Radioactivity::ZFit_R190(Z,Lambda,BinConversion) * tFittingR195->Eval(Z)/BinConversion;
		dCZ2 = WCSimGenerator_Radioactivity::ZFit_R210(Z,Lambda,BinConversion) * tFittingR215->Eval(Z)/BinConversion;
		
		dDZ1 = 1. - (R - fR_195)/ (fR_215 - fR_195);
		dDZ2 = 1. - (fR_215 - R)/ (fR_215 - fR_195);
		
		dZZ1 = fR2_195;
		dZZ2 = fR2_215;
	}
	else {
		dCZ1 = WCSimGenerator_Radioactivity::ZFit_R210(Z,Lambda,BinConversion) * tFittingR215->Eval(Z)/BinConversion;
		dCZ2 = 0;
		
		dDZ1 = 1. - (R - fR_215)/ (fR_max - fR_215);
		dDZ2 = 1. - (fR_max - R)/ (fR_max - fR_215);
		
		dZZ1 = fR2_215;
		dZZ2 = fR2_max;
	}
		
	//return dCZ;
	G4double dCR  = 0;
	G4double dCR1 = 0;
	G4double dCR2 = 0;
	G4double dDR1 = 0;
	G4double dDR2 = 0;
	
	if ( Z > fZ_p16 ) {
		dDR1 = 1. - (fZ_max - Z) / ( fZ_max - fZ_p16);
		dDR2 = 1. - (Z - fZ_p16) / ( fZ_max - fZ_p16);
		
		dCR  = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_ZpM(R2  ,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z16p(R2  ,Lambda,BinConversion)) / pow(BinConversion,2.);
		dCR1 = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_ZpM(dZZ1,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z16p(dZZ1,Lambda,BinConversion)) / pow(BinConversion,2.);
		dCR2 = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_ZpM(dZZ2,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z16p(dZZ2,Lambda,BinConversion)) / pow(BinConversion,2.);
	}
	else if ( Z > fZ_p14 ) {
		dDR1 = 1. - (fZ_p16 - Z) / (fZ_p16 - fZ_p14);
		dDR2 = 1. - (Z - fZ_p14) / (fZ_p16 - fZ_p14);
		
		dCR  = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z16p(R2  ,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z14p(R2  ,Lambda,BinConversion)) / pow(BinConversion,2.);
		dCR1 = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z16p(dZZ1,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z14p(dZZ1,Lambda,BinConversion)) / pow(BinConversion,2.);
		dCR2 = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z16p(dZZ2,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z14p(dZZ2,Lambda,BinConversion)) / pow(BinConversion,2.);
	}
	else if ( Z > fZ_000 ) {
		dDR1 = 1. - (fZ_p14 - Z) / (fZ_p14 - fZ_000);
		dDR2 = 1. - (Z - fZ_000) / (fZ_p14 - fZ_000);
		
		dCR  = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z14p(R2  ,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z00(R2  ,Lambda,BinConversion)) / pow(BinConversion,2.);
		dCR1 = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z14p(dZZ1,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z00(dZZ1,Lambda,BinConversion)) / pow(BinConversion,2.);
		dCR2 = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z14p(dZZ2,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z00(dZZ2,Lambda,BinConversion)) / pow(BinConversion,2.);
	}
	else if ( Z > fZ_m10 ) {
		dDR1 = 1. - (fZ_000 - Z) / (fZ_000 - fZ_m10);
		dDR2 = 1. - (Z - fZ_m10) / (fZ_000 - fZ_m10);
		
		dCR  = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z00(R2  ,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z10(R2  ,Lambda,BinConversion)) / pow(BinConversion,2.);
		dCR1 = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z00(dZZ1,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z10(dZZ1,Lambda,BinConversion)) / pow(BinConversion,2.);
		dCR2 = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z00(dZZ2,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z10(dZZ2,Lambda,BinConversion)) / pow(BinConversion,2.);
	}
	else if ( Z > fZ_m13 ) {
		dDR1 = 1. - (fZ_m10 - Z) / (fZ_m10 - fZ_m13);
		dDR2 = 1. - (Z - fZ_m13) / (fZ_m10 - fZ_m13);
		
		dCR  = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z10(R2  ,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z13(R2  ,Lambda,BinConversion)) / pow(BinConversion,2.);
		dCR1 = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z10(dZZ1,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z13(dZZ1,Lambda,BinConversion)) / pow(BinConversion,2.);
		dCR2 = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z10(dZZ2,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z13(dZZ2,Lambda,BinConversion)) / pow(BinConversion,2.);
	}
	else if ( Z > fZ_m16 ) {
		dDR1 = 1. - (fZ_m13 - Z) / (fZ_m13 - fZ_m16);
		dDR2 = 1. - (Z - fZ_m16) / (fZ_m13 - fZ_m16);
		
		dCR  = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z13(R2  ,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z16(R2  ,Lambda,BinConversion)) / pow(BinConversion,2.);
		dCR1 = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z13(dZZ1,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z16(dZZ1,Lambda,BinConversion)) / pow(BinConversion,2.);
		dCR2 = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z13(dZZ2,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_Z16(dZZ2,Lambda,BinConversion)) / pow(BinConversion,2.);
	}
	else {
	
		//std::cout << "With Z= " << Z << " " << fZ_m16 << " " << -1. * Z_max << " " << (fZ_m16 +  Z_max) << std::endl;
		dDR1 = 1. - (fZ_m16 - Z) / (fZ_m16 +  fZ_max);
		dDR2 = 1. - (Z + fZ_max) / (fZ_m16 +  fZ_max);
		
		dCR  = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z16(R2  ,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_ZmM(R2  ,Lambda,BinConversion)) / pow(BinConversion,2.);
		dCR1 = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z16(dZZ1,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_ZmM(dZZ1,Lambda,BinConversion)) / pow(BinConversion,2.);
		dCR2 = (dDR1 * WCSimGenerator_Radioactivity::R2Fit_Z16(dZZ2,Lambda,BinConversion) + dDR2 * WCSimGenerator_Radioactivity::R2Fit_ZmM(dZZ2,Lambda,BinConversion)) / pow(BinConversion,2.);
	}
	
	G4double dC = 0;
	
	if ( dCZ2 == 0 ) {
		dC  = dCR * ( dCZ1/dCR1 * dDZ1 + dDZ2);
	//	dCZ = dCZ1 * dDZ1 + dCR2 * dDZ2;
	}
	else {
		dC = dCR * ( dCZ1/dCR1 * dDZ1 + dCZ2/dCR2 * dDZ2);
	//	dCZ = dCZ1 * dDZ1 + dCZ2 * dDZ2;
	}
	
	return dC;
}
