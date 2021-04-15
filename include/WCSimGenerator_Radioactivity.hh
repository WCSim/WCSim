#ifndef WCSimGenerator_Radioactivity_hh
#define WCSimGenerator_Radioactivity_hh 1

#include "G4ReactionProductVector.hh"
#include "G4ThreeVector.hh"
#include <vector>

#include "WCSimDetectorConstruction.hh"
#include "TF2.h"
#include "TGraph.h"
#include "TMath.h"

using namespace std;

#define	RNMODEL_BIN_R_MAX	10 // Highest R2 layer
#define	RNMODEL_BIN_Z_MIN	 2 // Lowest Z layer
#define	RNMODEL_BIN_Z_MAX	16 // Highest Z layer
#define	RNMODEL_NBIN_R		16 // Number of R2 layers
#define	RNMODEL_NBIN_Z		18 // Number of Z layers
// GetSign function is defined as pre-compiler function to fasten the computation
#define 	GetSign(a) 		a<0?-1.:1.

class WCSimDetectorConstruction;

class WCSimGenerator_Radioactivity
{
	public:
		WCSimGenerator_Radioactivity(WCSimDetectorConstruction* myDC);
		~WCSimGenerator_Radioactivity();
		
		// Initialize the Model
		void Initialize();

		// Set the Scenario, allow to modify the Rn lifetime (in sec)
		// DON'T MODIFY THE LIFETIME IF YOU DON'T KNOW WHAT YOU ARE DOING!
		void Configuration(G4int iScenario, G4double dLifeTime=0);
		
		// Get a Random vertex for a Rn decay in the detector 
		// if tSymNumber is not 1, the detector is divided in tSymNumber radial sector.
		G4ThreeVector GetRandomVertex(G4int tSymNumber=1);
		
		// Get the Mean Rn Activity in ID
		G4double GetMeanActivity()		{ return fConcentrationID; }
		
		// Get the Mean Rn Activity in FV
		G4double GetMeanActivityFV()		{ return fConcentrationFV; }
		
		// Get the ID Volume used by the model
		G4double GetIDVolume()			{ return (fR_max*fR_max*TMath::Pi()*fZ_max*2.); }
		
		
	private:
		
		// ROOT like 2D function used to setup tfRnFunction
		// val[0] is R^{2}
		// val[1] is Z
		// par[0] is the lambda of the radioisotope
		// This function calculate the R2 and Z layers for a given (R2,Z) position and interpolates the results from RadonFormulaR and RadonFormulaZ
		static double RadonFormula(double *val, double *par);
		
		// R2 Concentration function
		// val[0] is R^{2}
		// val[1] is Z
		// par is vParam_R2[x] 
		static double RadonFormulaR(double* val, double* par);
		
		// R2 Concentration function
		// val[0] is Z
		// val[1] is R^{2}
		// par is vParam_Z[x] 
		static double RadonFormulaZ(double* val, double* par);
		
		// Diffusion functions
		static double DiffusionZ(double x, double origin, double factor);
		static double DiffusionR(double x, double origin, double factor);
		
		// Function
		TF2* tfRnFunction;
		
		// Model integral (only used for debug)
		G4double fIntegral;   // in ID
		G4double fIntegralFV; // in FV
		
		// Average concentrations (only used for debug)
		G4double fConcentrationID; // in ID
		G4double fConcentrationFV; // in FV 
		
		// Configuration function to set the Scenario:
		//	iScenario = 0 -> Uniform concentration in ID
		//	iScenario = 1 -> Scenario A (Relative scaling, Pessimistic, should be default)
		//	iScenario = 2 -> Scenario B (Absolute scaling, Optimistic)
		// This function compute all the intermediary Concentration values and set up the Rn model
		void SetScenario(G4int iScenario);
		

		// Holder for Detector construction
		WCSimDetectorConstruction*      myDetector;
		
		// Constant	
		G4int    fScenario; 			// Scenario holder
		static G4double fRnDiffusion_Coef;	// Constant Rn diffusion coefficient value
		static G4double fRnLambda_Global;	// Constant Rn Lambda
		G4double fRnLambda;
		
		static G4double fRn_PerPMT;		// Average Rn emanation per PMT
		static G4double fRn_Border;		// Average Rn concentration at the PMT wall
		
		// Parameter for fit, computed from the fit parameters:	
		G4double fConc_Middle;
		G4double fConc_Int_R2_6;
		G4double fConc_Int_R2_7;
		G4double fConc_Int_R2_8;
		G4double fConc_Int_R2_9;
		
		// Parameter array declaration:
		static G4double vParam_Z [RNMODEL_BIN_R_MAX][7];	
		static G4double vParam_R2[RNMODEL_BIN_Z_MAX][7];
	
		// Min and Max R2 values for each R2 layer
		static G4double vLayer_MinR2_Z[RNMODEL_BIN_R_MAX];
		static G4double vLayer_MaxR2_Z[RNMODEL_BIN_R_MAX];

		// Min and Max Z values for each Z layer
		static G4double vLayer_MinZ_R2[RNMODEL_BIN_Z_MAX];
		static G4double vLayer_MaxZ_R2[RNMODEL_BIN_Z_MAX];
		
		// WCSim detector limits:		
		static G4double fCurrentDetector_Z_min;
		static G4double fCurrentDetector_Z_max;
		static G4double fCurrentDetector_R_max;
		static G4double fCurrentDetector_R2_max;
		
		// SK detector limits:	
		static G4double fSK_Z_min;
		static G4double fSK_Z_max;
		static G4double fSK_R_max;
		static G4double fSK_R2_max;
		
		// Detector limits used by the model (normally = WCSim detector limits)
		static G4double fZ_min;
		static G4double fZ_max;
		static G4double fR_max;
		static G4double fR2_max;
		
		// FV volume limits (used for debug only)
		static G4double fZ_min_FV;
		static G4double fZ_max_FV;
		static G4double fR_max_FV;
		static G4double fR2_max_FV;
		
};
#endif
