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
		void Initialize();

		void Configuration(G4int iScenario, G4double dLifeTime=0);
		
		G4ThreeVector GetRandomVertex(G4int tSymNumber);
		G4double GetMeanActivity()		{ return fConcentrationID; }
		G4double GetMeanActivityFV()		{ return fConcentrationFV; }
		G4double GetIDVolume()			{ return (fR_max*fR_max*TMath::Pi()*fZ_max*2.); }
		
		
	private:
	
		// Diffusion functions
		static double DiffusionZ(double x, double origin, double factor);
		static double DiffusionR(double x, double origin, double factor);
		
		// Concentration functions
		static double RadonFormulaR(double* val, double* par);
		static double RadonFormulaZ(double* val, double* par);
		
		// Function
		TF2* thRnFunction;
		G4double fIntegral;
		G4double fIntegralFV;
		
		G4double fConcentrationID;
		G4double fConcentrationFV;
		
		void SetScenario(G4int iScenario);
		static double RadonFormula(double *val, double *par);

		WCSimDetectorConstruction*      myDetector;
		
		// Constant	
		G4int    fScenario;
		static G4double fRnDiffusion_Coef;
		static G4double fRnLambda_Global;
		G4double fRnLambda;
		
		static G4double fRn_PerPMT;
		
		// Parameter:	
		G4double fConc_Middle;
		G4double fConc_Int_R2_6;
		G4double fConc_Int_R2_7;
		G4double fConc_Int_R2_8;
		G4double fConc_Int_R2_9;
		
		static G4double fRn_Border;
		
		static G4double fHK_Z_min;
		static G4double fHK_Z_max;
		static G4double fHK_R_max;
		static G4double fHK_R2_max;
		
		static G4double fSK_Z_min;
		static G4double fSK_Z_max;
		static G4double fSK_R_max;
		static G4double fSK_R2_max;
		
		static G4double fZ_min_FV;
		static G4double fZ_max_FV;
		static G4double fR_max_FV;
		static G4double fR2_max_FV;
		
		static G4double fZ_min, fZ_max, fR_max , fR2_max;
		
		// Parameter array declaration:
		static G4double vParam_Z [RNMODEL_NBIN_R][7];	
		static G4double vParam_R2[RNMODEL_NBIN_Z][7];
	
		static G4double vLayer_MinR2_Z[RNMODEL_NBIN_R];
		static G4double vLayer_MaxR2_Z[RNMODEL_NBIN_R];

		static G4double vLayer_MinZ_R2[RNMODEL_NBIN_Z];
		static G4double vLayer_MaxZ_R2[RNMODEL_NBIN_Z];

};
#endif
