/*
	Rn model parameters version 2020/09/25
	Author: Guillaume Pronost

	Detector slicing:
	- The detector is divided in X layers over R2 from center to side, and Y layer over Z, from bottom to top.
	ex: R2 layer 0 is at the center of the detector, Z layer 0 is the bottom of the detector
	- R2 layers:
		- For each R2 layer, we have a Z distribution
		- There are RNMODEL_NBIN_R layers
	- For each Z layer we have a R2 distribution
	- Due to the noise, fit is impossible in the layers too close to the walls
		-> R2 layers after RNMODEL_BIN_R_MAX are not fitted
		-> Z layers below RNMODEL_BIN_Z_MIN and above RNMODEL_BIN_Z_MAX are not fitted

	Fit parameter explanations:
	- vParam_Z[x][..] are the model fit parameters of the Z distribution from R2 layer x
		[0]: bottom Z-limit of the convection front (unit: m)
		[1]: upper Z-limit of the convection front (unit: m)
		[2]: Radon concentration in the above the convection front (unit: mBq/m3)
		[3]: Radon concentration in the below the convection front (unit: mBq/m3)
		[4]: Radon concentration in the convection front (unit: mBq/m3)
		[5]: Correction to the diffusion factor to the top from the convection front (unit: abr.)
		[6]: Correction to the diffusion factor to the bottom from the convection front (unit: abr.)
	- vParam_R2[y][..] are the model fit parameters of the R2 distribution from Z layer y
		[0]: Constant Radon concentration on the detector side (unit: mBq/m3)
		[1]: R2 value where fast diffusion toward the center starts (unit: m2)
		[2]: Correction to the diffusion factor for the fast diffusion (unit: abr.)
		Note: fast diffusion -> short range
		[3]: Radon concentration in the center of the detector (unit: mBq/m3)
		[4]: (Optional) R2 value where slow diffusion toward the detector center starts (unit: m2)
		[5]: (Optional) Correction to the diffusion factor for the slow diffusion (unit: abr.)
		[6]: (Optional) Radon contration at [4]
		Note: slow diffusion -> long range
*/

// Global parameters:
#define 	RNMODEL_CONC_BOTTOM 		2.63 	// mBq/m^{3} -> From Nakano-san et al.
#define 	RNMODEL_CONC_CENTER 		0.1 	// mBq/m^{3} -> From Nakano-san et al.
#define 	RNMODEL_CONC_INTERMEDIATE 	0.3 	// mBq/m^{3} -> Arbitrary


	// Fit parameter for R2 layer = 0
	vParam_Z [0][0] = -12.6566;
	vParam_Z [0][1] = -10.2926;
	vParam_Z [0][2] = RNMODEL_CONC_CENTER;
	vParam_Z [0][3] = RNMODEL_CONC_BOTTOM;
	vParam_Z [0][4] = RNMODEL_CONC_BOTTOM + 0.0419033;
	vParam_Z [0][5] = 1.46931;
	vParam_Z [0][6] = 0.591534;

	// Fit parameter for R2 layer = 1
	vParam_Z [1][0] = -11.4759;
	vParam_Z [1][1] = -10.2941;
	vParam_Z [1][2] = RNMODEL_CONC_CENTER;
	vParam_Z [1][3] = RNMODEL_CONC_BOTTOM;
	vParam_Z [1][4] = RNMODEL_CONC_BOTTOM + 0.110054;
	vParam_Z [1][5] = 5.72643;
	vParam_Z [1][6] = 20.4479;

	// Fit parameter for R2 layer = 2
	vParam_Z [2][0] = -10.6932;
	vParam_Z [2][1] = -10.3933;
	vParam_Z [2][2] = RNMODEL_CONC_CENTER;
	vParam_Z [2][3] = RNMODEL_CONC_BOTTOM;
	vParam_Z [2][4] = RNMODEL_CONC_BOTTOM + 0.365747;
	vParam_Z [2][5] = 11.4212;
	vParam_Z [2][6] = 5.95607;

	// Fit parameter for R2 layer = 3
	vParam_Z [3][0] = -10.8868;
	vParam_Z [3][1] = -10.5867;
	vParam_Z [3][2] = RNMODEL_CONC_CENTER;
	vParam_Z [3][3] = RNMODEL_CONC_BOTTOM;
	vParam_Z [3][4] = RNMODEL_CONC_BOTTOM + 0.686557;
	vParam_Z [3][5] = 19.38;
	vParam_Z [3][6] = 8.74995;

	// Fit parameter for R2 layer = 4
	vParam_Z [4][0] = -10.5333;
	vParam_Z [4][1] = -10.5171;
	vParam_Z [4][2] = RNMODEL_CONC_CENTER;
	vParam_Z [4][3] = RNMODEL_CONC_BOTTOM;
	vParam_Z [4][4] = RNMODEL_CONC_BOTTOM + 0.87035;
	vParam_Z [4][5] = 29.0229;
	vParam_Z [4][6] = 4.60759;

	// Fit parameter for R2 layer = 5
	vParam_Z [5][0] = -11.5924;
	vParam_Z [5][1] = -10.3517;
	vParam_Z [5][2] = RNMODEL_CONC_CENTER;
	vParam_Z [5][3] = RNMODEL_CONC_BOTTOM;
	vParam_Z [5][4] = RNMODEL_CONC_BOTTOM + 0.896323;
	vParam_Z [5][5] = 33;
	vParam_Z [5][6] = 0.0498523;

	// Fit parameter for R2 layer = 6
	vParam_Z [6][0] = -11.713;
	vParam_Z [6][1] = -10.5491;
	vParam_Z [6][2] = fConc_Int_R2_6;
	vParam_Z [6][3] = RNMODEL_CONC_BOTTOM;
	vParam_Z [6][4] = RNMODEL_CONC_BOTTOM + 1.08428;
	vParam_Z [6][5] = 43.6658;
	vParam_Z [6][6] = 0.00406903;

	// Fit parameter for R2 layer = 7
	vParam_Z [7][0] = -12.1015;
	vParam_Z [7][1] = -10.9959;
	vParam_Z [7][2] = fConc_Int_R2_7;
	vParam_Z [7][3] = RNMODEL_CONC_BOTTOM;
	vParam_Z [7][4] = RNMODEL_CONC_BOTTOM + 0.917145;
	vParam_Z [7][5] = 81.9926;
	vParam_Z [7][6] = 0.0304033;

	// Fit parameter for R2 layer = 8
	vParam_Z [8][0] = -12.0848;
	vParam_Z [8][1] = -10.6199;
	vParam_Z [8][2] = fConc_Int_R2_8;
	vParam_Z [8][3] = RNMODEL_CONC_BOTTOM;
	vParam_Z [8][4] = RNMODEL_CONC_BOTTOM + 0.763414;
	vParam_Z [8][5] = 89.599;
	vParam_Z [8][6] = 2.4599;

	// Fit parameter for R2 layer = 9
	vParam_Z [9][0] = -12.0212;
	vParam_Z [9][1] = -10.1307;
	vParam_Z [9][2] = fConc_Int_R2_9;
	vParam_Z [9][3] = RNMODEL_CONC_BOTTOM;
	vParam_Z [9][4] = RNMODEL_CONC_BOTTOM + 0.824749;
	vParam_Z [9][5] = 67.1916;
	vParam_Z [9][6] = 0.0125308;

	// Fit parameter for Z layer = 2
	vParam_R2[2][0] = RNMODEL_CONC_BOTTOM + 0.20968;
	vParam_R2[2][1] = 258.819;
	vParam_R2[2][2] = 2507.93;
	vParam_R2[2][3] = RNMODEL_CONC_BOTTOM;
	vParam_R2[2][4] = 0;
	vParam_R2[2][5] = 0.01;
	vParam_R2[2][6] = 0;

	// Fit parameter for Z layer = 3
	vParam_R2[3][0] = RNMODEL_CONC_BOTTOM + 0.493603;
	vParam_R2[3][1] = 118.73;
	vParam_R2[3][2] = 251.745;
	vParam_R2[3][3] = RNMODEL_CONC_BOTTOM;
	vParam_R2[3][4] = 0;
	vParam_R2[3][5] = 0.01;
	vParam_R2[3][6] = 0;

	// Fit parameter for Z layer = 4
	vParam_R2[4][0] = RNMODEL_CONC_BOTTOM + 0.0746789;
	vParam_R2[4][1] = 244.721;
	vParam_R2[4][2] = 847.75;
	vParam_R2[4][3] = fConc_Middle;
	vParam_R2[4][4] = 0;
	vParam_R2[4][5] = 0.01;
	vParam_R2[4][6] = 0;

	// Fit parameter for Z layer = 5
	vParam_R2[5][0] = RNMODEL_CONC_BOTTOM + -0.0110869;
	vParam_R2[5][1] = 291.507;
	vParam_R2[5][2] = 359.053;
	vParam_R2[5][3] = RNMODEL_CONC_CENTER;
	vParam_R2[5][4] = 169.178;
	vParam_R2[5][5] = 0.00285011;
	vParam_R2[5][6] = 0;

	// Fit parameter for Z layer = 6
	vParam_R2[6][0] = RNMODEL_CONC_BOTTOM + -1.07021;
	vParam_R2[6][1] = 183.46;
	vParam_R2[6][2] = 0.00196017;
	vParam_R2[6][3] = RNMODEL_CONC_CENTER;
	vParam_R2[6][4] = 170;
	vParam_R2[6][5] = 103.895;
	vParam_R2[6][6] = RNMODEL_CONC_INTERMEDIATE;

	// Fit parameter for Z layer = 7
	vParam_R2[7][0] = RNMODEL_CONC_BOTTOM + -1.39163;
	vParam_R2[7][1] = 202.99;
	vParam_R2[7][2] = 5.96393;
	vParam_R2[7][3] = RNMODEL_CONC_CENTER;
	vParam_R2[7][4] = 170;
	vParam_R2[7][5] = 183.113;
	vParam_R2[7][6] = RNMODEL_CONC_INTERMEDIATE;

	// Fit parameter for Z layer = 8
	vParam_R2[8][0] = RNMODEL_CONC_BOTTOM + -1.42447;
	vParam_R2[8][1] = 204.478;
	vParam_R2[8][2] = 6.54267;
	vParam_R2[8][3] = RNMODEL_CONC_CENTER;
	vParam_R2[8][4] = 169.998;
	vParam_R2[8][5] = 107.482;
	vParam_R2[8][6] = RNMODEL_CONC_INTERMEDIATE;

	// Fit parameter for Z layer = 9
	vParam_R2[9][0] = RNMODEL_CONC_BOTTOM + -1.25849;
	vParam_R2[9][1] = 204.312;
	vParam_R2[9][2] = 6.19722;
	vParam_R2[9][3] = RNMODEL_CONC_CENTER;
	vParam_R2[9][4] = 170;
	vParam_R2[9][5] = 83.7055;
	vParam_R2[9][6] = RNMODEL_CONC_INTERMEDIATE;

	// Fit parameter for Z layer = 10
	vParam_R2[10][0] = RNMODEL_CONC_BOTTOM + -1.2568;
	vParam_R2[10][1] = 206.289;
	vParam_R2[10][2] = 8.76213;
	vParam_R2[10][3] = RNMODEL_CONC_CENTER;
	vParam_R2[10][4] = 170;
	vParam_R2[10][5] = 91.7556;
	vParam_R2[10][6] = RNMODEL_CONC_INTERMEDIATE;

	// Fit parameter for Z layer = 11
	vParam_R2[11][0] = RNMODEL_CONC_BOTTOM + -1.34235;
	vParam_R2[11][1] = 204.644;
	vParam_R2[11][2] = 7.45582;
	vParam_R2[11][3] = RNMODEL_CONC_CENTER;
	vParam_R2[11][4] = 169.999;
	vParam_R2[11][5] = 83.9211;
	vParam_R2[11][6] = RNMODEL_CONC_INTERMEDIATE;

	// Fit parameter for Z layer = 12
	vParam_R2[12][0] = RNMODEL_CONC_BOTTOM + -1.392;
	vParam_R2[12][1] = 201.912;
	vParam_R2[12][2] = 5.79213;
	vParam_R2[12][3] = RNMODEL_CONC_CENTER;
	vParam_R2[12][4] = 169.999;
	vParam_R2[12][5] = 78.8235;
	vParam_R2[12][6] = RNMODEL_CONC_INTERMEDIATE;

	// Fit parameter for Z layer = 13
	vParam_R2[13][0] = RNMODEL_CONC_BOTTOM + -1.45474;
	vParam_R2[13][1] = 203.484;
	vParam_R2[13][2] = 8.33337;
	vParam_R2[13][3] = RNMODEL_CONC_CENTER;
	vParam_R2[13][4] = 169.995;
	vParam_R2[13][5] = 90.3481;
	vParam_R2[13][6] = RNMODEL_CONC_INTERMEDIATE;

	// Fit parameter for Z layer = 14
	vParam_R2[14][0] = RNMODEL_CONC_BOTTOM + -1.31712;
	vParam_R2[14][1] = 203.275;
	vParam_R2[14][2] = 4.61698;
	vParam_R2[14][3] = RNMODEL_CONC_CENTER;
	vParam_R2[14][4] = 169.992;
	vParam_R2[14][5] = 52.6069;
	vParam_R2[14][6] = RNMODEL_CONC_INTERMEDIATE;

	// Fit parameter for Z layer = 15
	vParam_R2[15][0] = RNMODEL_CONC_BOTTOM + -1.1777;
	vParam_R2[15][1] = 209.059;
	vParam_R2[15][2] = 14.5265;
	vParam_R2[15][3] = RNMODEL_CONC_CENTER;
	vParam_R2[15][4] = 140.001;
	vParam_R2[15][5] = 38.2392;
	vParam_R2[15][6] = RNMODEL_CONC_INTERMEDIATE;

