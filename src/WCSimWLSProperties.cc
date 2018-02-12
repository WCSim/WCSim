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
#define NUMENTRIES_WLS 33

EljenEJ286::EljenEJ286() { SetgAbs(); SetgEm();}
EljenEJ286::~EljenEJ286(){}

G4String EljenEJ286::GetWLSPlateName() { G4String WLSPlateName = "EljenEJ286"; return WLSPlateName;}

G4int EljenEJ286::GetNumEntries() { return NUMENTRIES_WLS;}

G4float* EljenEJ286::GetPhotonWL() {
  static G4float PhotonWL[NUMENTRIES_WLS] =
      { 280, 290,
        300, 310, 320, 330, 340, 350, 360, 370, 380, 390,
        400, 410, 420, 430, 440, 450, 460, 470, 480, 490,
        500, 510, 520, 530, 540, 550, 560, 570, 580, 590,
        600 };
  return PhotonWL;
}

G4double* EljenEJ286::GetPhotonEnergy(){
  static G4double PhotonEnergy[NUMENTRIES_WLS] =
      { 2.06642*eV,
        2.10144*eV, 2.13768*eV, 2.17518*eV, 2.21402*eV, 2.25428*eV,
        2.29602*eV, 2.33934*eV, 2.38433*eV, 2.43108*eV, 2.4797*eV,
        2.53031*eV, 2.58302*eV, 2.63798*eV, 2.69533*eV, 2.75523*eV,
        2.81784*eV, 2.88338*eV, 2.95203*eV, 3.02403*eV, 3.09963*eV,
        3.17911*eV, 3.26277*eV, 3.35095*eV, 3.44403*eV, 3.54243*eV,
        3.64662*eV, 3.75713*eV, 3.87454*eV, 3.99952*eV, 4.13284*eV,
        4.27535*eV, 4.42804*eV};
  return PhotonEnergy;
}

// ###################### //
////////// RINDEX //////////
// ###################### //

G4double* EljenEJ286::GetRIndex(){
  static G4double RIndex[NUMENTRIES_WLS] =
      { 1.58,
        1.58, 1.58, 1.58, 1.58, 1.58,
        1.58, 1.58, 1.58, 1.58, 1.58,
        1.58, 1.58, 1.58, 1.58, 1.58,
        1.58, 1.58, 1.58, 1.58, 1.58,
        1.58, 1.58, 1.58, 1.58, 1.58,
        1.58, 1.58, 1.58, 1.58, 1.58,
        1.58, 1.58};
  return RIndex;
}

// ###################### //
//////// ABSORPTION ////////
// ###################### //

#define NUMENTRIES_WLS_ABS 15

G4int EljenEJ286::GetNumEntries_ABS() { return NUMENTRIES_WLS_ABS;}

G4double* EljenEJ286::GetPhotonEnergy_ABS(){
  static G4double PhotonEnergy_ABS[NUMENTRIES_WLS_ABS] =
      { 2.95203*eV, 3.02403*eV, 3.09963*eV, 3.17911*eV, 3.26277*eV,
        3.35095*eV, 3.44403*eV, 3.54243*eV, 3.64662*eV, 3.75713*eV,
        3.87454*eV, 3.99952*eV, 4.13284*eV, 4.27535*eV, 4.42804*eV};
  return PhotonEnergy_ABS;
}

G4double* EljenEJ286::GetAbs() {
  G4double attL = 160.*cm; // From Datasheet EJ-286 and inferred with SG-BC404 // Basically BS
  static G4double AbsPlate[NUMENTRIES_WLS_ABS] =
      { (1-0.01)*attL, (1-0.05)*attL, (1-0.64)*attL, (1-0.93)*attL, (1-0.999)*attL,
        (1-0.999)*attL, (1-0.999)*attL, (1-0.999)*attL, (1-0.999)*attL, (1-0.999)*attL,
        (1-0.99)*attL, (1-0.97)*attL, (1-0.89)*attL, (1-0.71)*attL, (1-0.54)*attL};
  return AbsPlate;
}

// ###################### //
///////// EMISSION /////////
// ###################### //

#define NUMENTRIES_WLS_EM 16

G4int EljenEJ286::GetNumEntries_EM() { return NUMENTRIES_WLS_EM;}

G4double* EljenEJ286::GetPhotonEnergy_EM() {
  static G4double PhotonEnergy_EM[NUMENTRIES_WLS_EM] =
      { 2.29602*eV, 2.33934*eV, 2.38433*eV, 2.43108*eV, 2.4797*eV,
        2.53031*eV, 2.58302*eV, 2.63798*eV, 2.69533*eV, 2.75523*eV,
        2.81784*eV, 2.88338*eV, 2.95203*eV, 3.02403*eV, 3.09963*eV,
        3.17911*eV };
  return PhotonEnergy_EM;
}

G4double *EljenEJ286::GetEm() {
  static G4double EmissionPlate[NUMENTRIES_WLS_EM] =
      { 0., 0.01, 0.02, 0.05, 0.08,
        0.1, 0.12, 0.17, 0.38, 0.55,
        0.70, 1., 0.9, 0.3, 0.05,
        0. };
  return EmissionPlate;
}

// ###################### //
///////// STACKING /////////
// ###################### //

void EljenEJ286::SetgAbs(){
  double AbsAmp[NUMENTRIES_WLS_ABS] =
      { (0.01), (0.05), (0.64), (0.93), (0.999),
        (0.999), (0.999), (0.999), (0.999), (0.999),
        (0.99), (0.97), (0.89), (0.71), (0.54)};
  gAbs = new TGraph();
  int iPt=0;
  const int step = 10;

  // Start by defining to 0 where no data on the WLS is available
  for(int i=700;i>(int)(round(hWL(GetPhotonEnergy_ABS()[0]))*nm)+step;i=i-step){
    gAbs->SetPoint(iPt,i,0.);
    iPt++;
  }

  // Then fill with point there is data
  for(int i=0;i<NUMENTRIES_WLS_ABS;i++){
    gAbs->SetPoint(iPt,round(hWL(GetPhotonEnergy_ABS()[i]))*nm,AbsAmp[i]);
    iPt++;
  }

  // Then again set everywhere else to 0
  for(int i=(int)(round(hWL(GetPhotonEnergy_ABS()[NUMENTRIES_WLS_ABS-1]))*nm)-step;i>200;i=i-step){
    gAbs->SetPoint(iPt,i,0.);
    iPt++;
  }
}

void EljenEJ286::SetgEm(){
  gEm = new TGraph();
  int iPt=0;
  const int step = 10;

  // Start by defining to 0 where no data on the WLS is available
  for(int i=700;i>(int)(round(hWL(GetPhotonEnergy_EM()[0]))*nm)+step;i=i-step){
    gEm->SetPoint(iPt,i,0.);
    iPt++;
  }

  // Then fill with point there is data
  for(int i=0;i<NUMENTRIES_WLS_EM;i++){
    gEm->SetPoint(iPt,round(hWL(GetPhotonEnergy_EM()[i]))*nm,GetEm()[i]);
    G4cout << iPt << " " << round(hWL(GetPhotonEnergy_EM()[i]))*nm << " " << GetEm()[i] << G4endl;
    iPt++;
  }

  // Then again set everywhere else to 0
  for(int i=(int)(round(hWL(GetPhotonEnergy_EM()[NUMENTRIES_WLS_EM-1]))*nm)-step;i>200;i=i-step){
    gEm->SetPoint(iPt,i,0.);
    iPt++;
  }
}
