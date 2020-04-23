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

class WCSimDetectorConstruction;

class WCSimGenerator_Radioactivity
{
	public:
		WCSimGenerator_Radioactivity(WCSimDetectorConstruction* myDC);
		~WCSimGenerator_Radioactivity();
		void Initialize();

		void Configuration(G4int iScenario, G4double dLifeTime=0);
		
		G4ThreeVector GetRandomVertex(G4int tSymNumber);
		G4double GetMeanActivity() { return fIntegral; }
		G4double GetIDVolume() { return (fR_max*fR_max*TMath::Pi()*fZ_max*2.); }
		
		
	private:
	
		static G4double ZFit_R210(G4double x, G4double Lambda, G4double BinConversion);
		static G4double ZFit_R190(G4double x, G4double Lambda, G4double BinConversion);
		static G4double ZFit_R170(G4double x, G4double Lambda, G4double BinConversion);
		static G4double ZFit_R140(G4double x, G4double Lambda, G4double BinConversion);
		static G4double ZFit_R070(G4double x, G4double Lambda, G4double BinConversion);
		static G4double ZFit_R040(G4double x, G4double Lambda, G4double BinConversion);
		static G4double ZFit_R020(G4double x, G4double Lambda, G4double BinConversion);
		static G4double ZFit_R000(G4double x, G4double Lambda, G4double BinConversion);

		static G4double R2Fit_ZpM(G4double x, G4double Lambda, G4double BinConversion);
		static G4double R2Fit_ZmM(G4double x, G4double Lambda, G4double BinConversion);
		static G4double R2Fit_Z16(G4double x, G4double Lambda, G4double BinConversion);
		static G4double R2Fit_Z13(G4double x, G4double Lambda, G4double BinConversion);
		static G4double R2Fit_Z10(G4double x, G4double Lambda, G4double BinConversion);
		static G4double R2Fit_Z00(G4double x, G4double Lambda, G4double BinConversion);
		static G4double R2Fit_Z14p(G4double x, G4double Lambda, G4double BinConversion);
		static G4double R2Fit_Z16p(G4double x, G4double Lambda, G4double BinConversion);
		
		// Function
		TF2* thRnFunction;
		G4double fIntegral;
		
		void SetScenario(G4int iScenario);
		static G4double RadonFormula(G4double *val, G4double *par);

		WCSimDetectorConstruction*      myDetector;
		
		// Constant	
		G4int    fScenario;
		static G4double fRnDiffusion_Coef;
		G4double fRnLambda;
		
		static G4double fRn_PerPMT;
		
		static G4double fRnSK_Center;
		static G4double fRnSK_Bottom;
		
		static G4double fRn_Border;
		
		static G4double fHK_Z_max;
		static G4double fHK_R_max;
		static G4double fHK_R2_max;
		static G4double fHK_Z_reco;
		static G4double fHK_R_reco;
		static G4double fHK_R2_reco;
		
		static G4double fSK_Z_max;
		static G4double fSK_R_max;
		static G4double fSK_R2_max;
		static G4double fSK_Z_reco;
		static G4double fSK_R_reco;
		static G4double fSK_R2_reco;
		
		static G4double fZ_max , fR_max , fR2_max;
		static G4double fZ_reco, fR_reco, fR2_reco;
	
		G4double fMperBin;
		G4double fBin_Z_reco;
		G4double fBin_Z_max;
		G4double fBin_D_reco;
		G4double fBin_D_max;
		G4double fBin_R2_reco;
		G4double fBin_R2_max;
		
		G4double fScaleTypeInsideZ;
		G4double fScaleTypeInsideR;
		G4double fScaleTypeInsideR2;
		
		G4double fScaleTypeInStrucZ1;
		G4double fScaleTypeInStrucZ2;
		G4double fScaleTypeInStrucZ3;
		
		G4double fScaleTypeOutsideZ1;
		G4double fScaleTypeOutsideZ2;
		G4double fScaleTypeOutsideZ3;
		
		G4double fScaleTypeOutsideR1;
		G4double fScaleTypeOutsideR2;
		G4double fScaleTypeOutsideR3;
		
		static G4double fChangeZPMax;
		static G4double fChangeZNMax;
		static G4double fChangeRMax;
		
		static G4double fR2_000, fR2_025, fR2_045, fR2_075,
				fR2_145, fR2_175, fR2_195, fR2_215;
		static G4double fR_000 , fR_025 , fR_045 , fR_075 ,
			 	fR_145 , fR_175 , fR_195 , fR_215 ;
			 
		static G4double fZ_p16, fZ_p14, fZ_000, fZ_m10, fZ_m13, fZ_m16;

		static G4double fChangeR210N13, fChangeR210N10, fChangeR210N5, fChangeR210P1, fChangeR210P8, fChangeR210P13;
		static G4double fChangeR190N10, fChangeR190N8;
		static G4double fChangeR170N10, fChangeR170N8;
		static G4double fChangeR140N10, fChangeR140N8;
		static G4double fChangeR070N10, fChangeR070N8;
		static G4double fChangeR040N10;
		static G4double fChangeR020N10;
		static G4double fChangeR000N10;
		
		static G4double fChangeZmMxR025, fChangeZmMxR175;
		static G4double fChangeZm16R025, fChangeZm16R125;
		static G4double fChangeZm13R023, fChangeZm13R080;
		static G4double fChangeZm10R023;
		static G4double fChangeZp16R025, fChangeZp16R115;

		static G4double fChangeZm16Flow, fChangeZm13Flow, fChangeZm10Flow, fChangeZ000Flow, fChangeZp14Flow, fChangeZp16Flow;
		
		static G4double fGaussM0, fGaussM1, fGaussM2, fGaussM3, fGaussR190M3, fGaussM4, fGaussM5, fGaussM6, fGaussM7, fGaussM8;
		static G4double fGaussR190S0, fGaussR190S1, fGaussR190S2, fGaussR190S3, fGaussR190S4, fGaussR190S5, fGaussR190S6, fGaussR190S7, fGaussR190S8;
		static G4double fGaussR170S0, fGaussR170S1, fGaussR170S2, fGaussR170S3, fGaussR170S4, fGaussR170S5, fGaussR170S6, fGaussR170S7, fGaussR170S8;
		static G4double fGaussR140S0, fGaussR140S1, fGaussR140S2, fGaussR140S3, fGaussR140S4, fGaussR140S5, fGaussR140S6, fGaussR140S7, fGaussR140S8;
		
		static TGraph* tFittingR000;
		static TGraph* tFittingR025;
		static TGraph* tFittingR045;
		static TGraph* tFittingR075;
		static TGraph* tFittingR145;
		static TGraph* tFittingR175;
		static TGraph* tFittingR195;
		static TGraph* tFittingR215;

};
#endif
