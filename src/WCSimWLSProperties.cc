//
// Created by zsoldos on 24/01/18.
//

#include "WCSimWLSProperties.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

const double planck = 4.1357e-15; // ev.s
const double lightspeed = 299792458e9; // nm/s

double hWL(double E){
  return planck * (lightspeed/E);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// ELJEN EJ-286
// http://www.eljentechnology.com/products/wavelength-shifting-plastics/ej-280-ej-282-ej-284-ej-286
#define NUMENTRIES_WLS 60

EljenEJ286::EljenEJ286() { SetgAbs(); SetgEm(); SethEm();}
EljenEJ286::~EljenEJ286(){}

G4String EljenEJ286::GetWLSPlateName() { G4String WLSPlateName = "EljenEJ286"; return WLSPlateName;}

G4int EljenEJ286::GetNumEntries() { return NUMENTRIES_WLS;}

G4double* EljenEJ286::GetPhotonWL() {
  static G4double PhotonWL[NUMENTRIES_WLS] =
      { 200, 210, 220, 230, 240, 250, 260, 270, 280, 290,
        300, 310, 320, 330, 340, 350, 360, 370, 380, 390,
        400, 410, 420, 430, 440, 450, 460, 470, 480, 490,
        500, 510, 520, 530, 540, 550, 560, 570, 580, 590,
        600, 610, 620, 630, 640, 650, 660, 670, 680, 690,
        700, 710, 720, 730, 740, 750, 760, 770, 780, 790};
  return PhotonWL;
}

G4double* EljenEJ286::GetPhotonEnergy(){
  static G4double PhotonEnergy[NUMENTRIES_WLS] =
      {1.56943*eV, 1.58955*eV, 1.6102*eV, 1.63138*eV, 1.65314*eV,
       1.67548*eV, 1.69843*eV, 1.72202*eV, 1.74627*eV, 1.77122*eV,
       1.79689*eV, 1.82331*eV, 1.85052*eV, 1.87856*eV, 1.90746*eV,
       1.93727*eV, 1.96802*eV, 1.99976*eV, 2.03254*eV, 2.06642*eV,
       2.10144*eV, 2.13768*eV, 2.17518*eV, 2.21402*eV, 2.25428*eV,
       2.29602*eV, 2.33934*eV, 2.38433*eV, 2.43108*eV, 2.4797*eV,
       2.53031*eV, 2.58302*eV, 2.63798*eV, 2.69533*eV, 2.75523*eV,
       2.81784*eV, 2.88338*eV, 2.95203*eV, 3.02403*eV, 3.09963*eV,
       3.17911*eV, 3.26277*eV, 3.35095*eV, 3.44403*eV, 3.54243*eV,
       3.64662*eV, 3.75713*eV, 3.87454*eV, 3.99952*eV, 4.13284*eV,
       4.27535*eV, 4.42804*eV, 4.59204*eV, 4.76866*eV, 4.95941*eV,
       5.16605*eV, 5.39066*eV, 5.63569*eV, 5.90406*eV, 6.19926*eV};
  return PhotonEnergy;
}

// ###################### //
////////// RINDEX //////////
// ###################### //

G4double* EljenEJ286::GetRIndex(){
  static G4double RIndex[NUMENTRIES_WLS] =
      {1.88417, 1.82311, 1.77462, 1.73568, 1.70409,
       1.67822, 1.65685, 1.63905, 1.62413, 1.61152,
       1.60081, 1.59164, 1.58376, 1.57694, 1.57102,
       1.56585, 1.56132, 1.55733, 1.5538, 1.55067,
       1.54789, 1.5454, 1.54317, 1.54116, 1.53935,
       1.53772, 1.53624, 1.53489, 1.53366, 1.53254,
       1.53152, 1.53058, 1.52971, 1.52892, 1.52818,
       1.5275, 1.52687, 1.52629, 1.52575, 1.52525,
       1.52478, 1.52434, 1.52393, 1.52355, 1.52319,
       1.52285, 1.52254, 1.52224, 1.52196, 1.5217,
       1.52145, 1.52121, 1.52099, 1.52078, 1.52058,
       1.52039, 1.52021, 1.52004, 1.51988, 1.51973, };
  return RIndex;
}

// ###################### //
//////// ABSORPTION ////////
// ###################### //

#define NUMENTRIES_WLS_ABS 50

G4int EljenEJ286::GetNumEntries_ABS() { return NUMENTRIES_WLS_ABS;}

G4double* EljenEJ286::GetPhotonEnergy_ABS(){
  static G4double PhotonEnergy_ABS[NUMENTRIES_WLS_ABS] =
      {1.79689*eV, 1.82331*eV, 1.85052*eV, 1.87856*eV, 1.90746*eV,
       1.93727*eV, 1.96802*eV, 1.99976*eV, 2.03254*eV, 2.06642*eV,
       2.10144*eV, 2.13768*eV, 2.17518*eV, 2.21402*eV, 2.25428*eV,
       2.29602*eV, 2.33934*eV, 2.38433*eV, 2.43108*eV, 2.4797*eV,
       2.53031*eV, 2.58302*eV, 2.63798*eV, 2.69533*eV, 2.75523*eV,
       2.81784*eV, 2.88338*eV, 2.95203*eV, 3.02403*eV, 3.09963*eV,
       3.17911*eV, 3.26277*eV, 3.35095*eV, 3.44403*eV, 3.54243*eV,
       3.64662*eV, 3.75713*eV, 3.87454*eV, 3.99952*eV, 4.13284*eV,
       4.27535*eV, 4.42804*eV, 4.59204*eV, 4.76866*eV, 4.95941*eV,
       5.16605*eV, 5.39066*eV, 5.63569*eV, 5.90406*eV, 6.19926*eV};

  return PhotonEnergy_ABS;
}

G4double* EljenEJ286::GetAbs() {
  static G4double AbsPlate[NUMENTRIES_WLS_ABS] =
      { 19.6429*cm, 19.5783*cm, 19.4179*cm, 19.2909*cm, 19.4348*cm, // 700 --- 650
        19.4447*cm, 19.4035*cm, 18.9912*cm, 19.1066*cm, 19.0923*cm, // 650 --- 600
        19.1714*cm, 18.9591*cm, 18.997*cm, 18.9408*cm, 18.7645*cm, // 600 --- 550
        18.5919*cm, 18.6909*cm, 18.5189*cm, 18.3881*cm, 18.1821*cm, // 550 --- 500
        18.0372*cm, 18.0747*cm, 17.8693*cm, 17.6159*cm, 17.5833*cm, // 500 --- 450
        17.3554*cm, 16.8257*cm, 15.4036*cm, 9.17667*cm, 1.21309*cm, // 450 --- 400
        0.32882*cm, 0.293643*cm, 0.284121*cm, 0.282219*cm, 0.284037*cm, // 400 --- 350
        0.278603*cm, 0.278088*cm, 0.279076*cm, 0.279771*cm, 0.280512*cm, // 350 --- 300
        0.281305*cm, 0.282154*cm, 0.278283*cm, 0.278459*cm, 0.272505*cm, // 300 --- 250
        100*cm, 100*cm, 100*cm, 100*cm, 100*cm}; // 250 --- 200
  return AbsPlate;
}

// ###################### //
///////// EMISSION /////////
// ###################### //

#define NUMENTRIES_WLS_EM 60

G4int EljenEJ286::GetNumEntries_EM() { return NUMENTRIES_WLS_EM;}

G4double* EljenEJ286::GetPhotonEnergy_EM() {
  static G4double PhotonEnergy_EM[NUMENTRIES_WLS_EM] =
      {1.56943*eV, 1.58955*eV, 1.6102*eV, 1.63138*eV, 1.65314*eV,
       1.67548*eV, 1.69843*eV, 1.72202*eV, 1.74627*eV, 1.77122*eV,
       1.79689*eV, 1.82331*eV, 1.85052*eV, 1.87856*eV, 1.90746*eV,
       1.93727*eV, 1.96802*eV, 1.99976*eV, 2.03254*eV, 2.06642*eV,
       2.10144*eV, 2.13768*eV, 2.17518*eV, 2.21402*eV, 2.25428*eV,
       2.29602*eV, 2.33934*eV, 2.38433*eV, 2.43108*eV, 2.4797*eV,
       2.53031*eV, 2.58302*eV, 2.63798*eV, 2.69533*eV, 2.75523*eV,
       2.81784*eV, 2.88338*eV, 2.95203*eV, 3.02403*eV, 3.09963*eV,
       3.17911*eV, 3.26277*eV, 3.35095*eV, 3.44403*eV, 3.54243*eV,
       3.64662*eV, 3.75713*eV, 3.87454*eV, 3.99952*eV, 4.13284*eV,
       4.27535*eV, 4.42804*eV, 4.59204*eV, 4.76866*eV, 4.95941*eV,
       5.16605*eV, 5.39066*eV, 5.63569*eV, 5.90406*eV, 6.19926*eV};
  return PhotonEnergy_EM;
}

G4double *EljenEJ286::GetEm() {
  static G4double EmissionPlate[NUMENTRIES_WLS_EM] =
      { 0., 0., 0., 0., 0.,  // 800 --- 750
        0., 0., 0., 0., 0., // 750 --- 700
        0., 0., 0., 0., 0.,  // 700 --- 650
        0., 0., 0., 0., 0., // 650 --- 600
        0., 0., 0., 0., 0., // 600 --- 550
        0., 0.01, 0.02, 0.05, 0.08,  // 550 --- 500
        0.1, 0.12, 0.17, 0.38, 0.55, // 500 --- 450
        0.70, 1., 0.9, 0.3, 0.05,    // 450 --- 400
        0., 0., 0., 0., 0., // 400 --- 350
        0., 0., 0., 0., 0., // 350 --- 300
        0., 0., 0., 0., 0., // 300 --- 250
        0., 0., 0., 0., 0.}; // 250 --- 200
  return EmissionPlate;
}

// ###################### //
///////// STACKING /////////
// ###################### //

void EljenEJ286::SetgAbs(){
  double AbsAmp[NUMENTRIES_WLS_ABS] =
      { 0, 0, 0, 0, 0, // 700 --- 650
        0, 0, 0, 0, 0, // 650 --- 600
        0, 0, 0, 0, 0, // 600 --- 550
        0, 0, 0, 0, 0, // 550 --- 500
        0, 0, 0, 0, 0, // 500 --- 450
        0, 0, 0.01, 0.05, 0.64, // 450 --- 400
        0.93, 0.9999, 0.9999, 0.9999, 0.9999, // 400 --- 350
        0.9999, 0.9999, 0.99, 0.97, 0.89, // 350 --- 300
        0.71, 0.54, 0, 0, // 300 --- 250
        0, 0, 0, 0, 0}; // 250 --- 200
  gAbs = new TGraph();
  const int step = 10;

  // std::cout << "Creating Stacking graph objects for WLS" << std::endl;
  // std::cout << "WLS ABS" << std::endl;
  // Then fill with point there is data
  for(int i=NUMENTRIES_WLS_ABS-1; i>0; i--){
    gAbs->SetPoint(i,round(hWL(GetPhotonEnergy_ABS()[i]))*nm,AbsAmp[i]);
    // std::cout << round(hWL(GetPhotonEnergy_ABS()[i]))*nm << "nm -> " << AbsAmp[i] << std::endl;
  }

}

void EljenEJ286::SetgEm(){
  gEm = new TGraph();
  const int step = 10;

  // std::cout << "Creating Stacking graph objects for WLS" << std::endl;
  // std::cout << "WLS EM" << std::endl;
  // Then fill with point there is data
  for(int i=NUMENTRIES_WLS_EM-1; i>0; i--){
    gEm->SetPoint(i,round(hWL(GetPhotonEnergy_EM()[i]))*nm,GetEm()[i]);
    // std::cout << round(hWL(GetPhotonEnergy_EM()[i]))*nm << "nm -> " << GetEm()[i] << std::endl;
  }

}

// ###################### //
///////// STEPPING /////////
// ###################### //

void EljenEJ286::SethEm() {

  static const int emitArraySize = 15;
  float emitWavelength[emitArraySize] = {390,400,410,420,430,440,450,460,470,480,490,500,510,520,530};
  float emit[emitArraySize] =
      {0.0, 0.1, 0.71, 0.98,
       0.91, 0.63, 0.49, 0.35,
       0.20, 0.13, 0.081, 0.052,
       0.03, 0.021, 0};

  emissionHist = new TH1F("hist","hist", 15, 390., 530.);
  emissionHist->Scale(1/emissionHist->Integral());

  for (int i = 0 ; i < emitArraySize; i++) {

    emissionHist->Fill(emitWavelength[i], emit[i]);
    emissionHist->SetBinError(i+1, 0);
  }
}