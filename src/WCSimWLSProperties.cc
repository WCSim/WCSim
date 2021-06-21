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
#define nEntries_WLS_rindex 60

EljenEJ286::EljenEJ286() { SetgAbs(); SetgEm(); SethEm();}
EljenEJ286::~EljenEJ286(){}

G4String EljenEJ286::GetWLSPlateName() { G4String WLSPlateName = "EljenEJ286"; return WLSPlateName;}

G4int EljenEJ286::GetNumEntries() { return nEntries_WLS_rindex;}

G4double* EljenEJ286::GetPhotonWL() {
  static G4double PhotonWL[nEntries_WLS_rindex] =
      { 200, 210, 220, 230, 240, 250, 260, 270, 280, 290,
        300, 310, 320, 330, 340, 350, 360, 370, 380, 390,
        400, 410, 420, 430, 440, 450, 460, 470, 480, 490,
        500, 510, 520, 530, 540, 550, 560, 570, 580, 590,
        600, 610, 620, 630, 640, 650, 660, 670, 680, 690,
        700, 710, 720, 730, 740, 750, 760, 770, 780, 790};
  return PhotonWL;
}

G4double* EljenEJ286::GetPhotonEnergy(){
  static G4double WLS_energy_for_rindex[nEntries_WLS_rindex] =
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
  return WLS_energy_for_rindex;
}

// ###################### //
////////// RINDEX //////////
// ###################### //

G4double* EljenEJ286::GetRIndex(){
  static G4double eljen_rindex_vs_energy[nEntries_WLS_rindex] =
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
       1.52039, 1.52021, 1.52004, 1.51988, 1.51973 };
  return eljen_rindex_vs_energy;
}

// ###################### //
//////// ABSORPTION ////////
// ###################### //

#define nEntries_abslength_eljen 574

G4int EljenEJ286::GetNumEntries_ABS() { return nEntries_abslength_eljen;}

G4double* EljenEJ286::GetPhotonEnergy_ABS(){
  static G4double PhotonEnergy_ABS[nEntries_abslength_eljen] = {
	1.60393*eV, 1.60601*eV, 1.60809*eV, 1.61018*eV, 1.61228*eV, 1.61437*eV, 1.61648*eV, 1.61859*eV, 1.62071*eV, 1.62283*eV, 
	1.62495*eV, 1.62709*eV, 1.62922*eV, 1.63137*eV, 1.63352*eV, 1.63567*eV, 1.63783*eV, 1.64*eV, 1.64217*eV, 1.64435*eV, 
	1.64653*eV, 1.64872*eV, 1.65092*eV, 1.65312*eV, 1.65533*eV, 1.65754*eV, 1.65976*eV, 1.66198*eV, 1.66421*eV, 1.66645*eV, 
	1.66869*eV, 1.67094*eV, 1.6732*eV, 1.67546*eV, 1.67773*eV, 1.68*eV, 1.68228*eV, 1.68456*eV, 1.68686*eV, 1.68916*eV, 
	1.69146*eV, 1.69377*eV, 1.69609*eV, 1.69841*eV, 1.70074*eV, 1.70308*eV, 1.70542*eV, 1.70777*eV, 1.71012*eV, 1.71249*eV, 
	1.71485*eV, 1.71723*eV, 1.71961*eV, 1.722*eV, 1.72439*eV, 1.7268*eV, 1.7292*eV, 1.73162*eV, 1.73404*eV, 1.73647*eV, 
	1.73891*eV, 1.74135*eV, 1.7438*eV, 1.74625*eV, 1.74872*eV, 1.75119*eV, 1.75366*eV, 1.75615*eV, 1.75864*eV, 1.76114*eV, 
	1.76364*eV, 1.76615*eV, 1.76867*eV, 1.7712*eV, 1.77373*eV, 1.77627*eV, 1.77882*eV, 1.78138*eV, 1.78394*eV, 1.78651*eV, 
	1.78909*eV, 1.79168*eV, 1.79427*eV, 1.79687*eV, 1.79948*eV, 1.80209*eV, 1.80472*eV, 1.80735*eV, 1.80999*eV, 1.81263*eV, 
	1.81529*eV, 1.81795*eV, 1.82062*eV, 1.82329*eV, 1.82598*eV, 1.82867*eV, 1.83137*eV, 1.83408*eV, 1.8368*eV, 1.83952*eV, 
	1.84226*eV, 1.845*eV, 1.84775*eV, 1.85051*eV, 1.85327*eV, 1.85605*eV, 1.85883*eV, 1.86162*eV, 1.86442*eV, 1.86723*eV, 
	1.87004*eV, 1.87287*eV, 1.8757*eV, 1.87855*eV, 1.8814*eV, 1.88425*eV, 1.88712*eV, 1.89*eV, 1.89289*eV, 1.89578*eV, 
	1.89868*eV, 1.90159*eV, 1.90452*eV, 1.90745*eV, 1.91038*eV, 1.91333*eV, 1.91629*eV, 1.91926*eV, 1.92223*eV, 1.92522*eV, 
	1.92821*eV, 1.93121*eV, 1.93423*eV, 1.93725*eV, 1.94028*eV, 1.94332*eV, 1.94637*eV, 1.94943*eV, 1.9525*eV, 1.95558*eV, 
	1.95867*eV, 1.96177*eV, 1.96488*eV, 1.968*eV, 1.97113*eV, 1.97427*eV, 1.97742*eV, 1.98057*eV, 1.98374*eV, 1.98692*eV, 
	1.99011*eV, 1.99331*eV, 1.99652*eV, 1.99974*eV, 2.00297*eV, 2.00621*eV, 2.00946*eV, 2.01273*eV, 2.016*eV, 2.01928*eV, 
	2.02258*eV, 2.02588*eV, 2.0292*eV, 2.03252*eV, 2.03586*eV, 2.03921*eV, 2.04257*eV, 2.04594*eV, 2.04932*eV, 2.05271*eV, 
	2.05612*eV, 2.05953*eV, 2.06296*eV, 2.0664*eV, 2.06985*eV, 2.07331*eV, 2.07678*eV, 2.08027*eV, 2.08376*eV, 2.08727*eV, 
	2.09079*eV, 2.09432*eV, 2.09787*eV, 2.10142*eV, 2.10499*eV, 2.10857*eV, 2.11216*eV, 2.11577*eV, 2.11938*eV, 2.12301*eV, 
	2.12665*eV, 2.13031*eV, 2.13398*eV, 2.13765*eV, 2.14135*eV, 2.14505*eV, 2.14877*eV, 2.1525*eV, 2.15624*eV, 2.16*eV, 
	2.16377*eV, 2.16755*eV, 2.17135*eV, 2.17516*eV, 2.17898*eV, 2.18282*eV, 2.18667*eV, 2.19053*eV, 2.19441*eV, 2.1983*eV, 
	2.2022*eV, 2.20612*eV, 2.21005*eV, 2.214*eV, 2.21796*eV, 2.22194*eV, 2.22592*eV, 2.22993*eV, 2.23395*eV, 2.23798*eV, 
	2.24202*eV, 2.24609*eV, 2.25016*eV, 2.25425*eV, 2.25836*eV, 2.26248*eV, 2.26662*eV, 2.27077*eV, 2.27494*eV, 2.27912*eV, 
	2.28331*eV, 2.28753*eV, 2.29176*eV, 2.296*eV, 2.30026*eV, 2.30453*eV, 2.30883*eV, 2.31313*eV, 2.31746*eV, 2.3218*eV, 
	2.32615*eV, 2.33053*eV, 2.33491*eV, 2.33932*eV, 2.34374*eV, 2.34818*eV, 2.35264*eV, 2.35711*eV, 2.3616*eV, 2.36611*eV, 
	2.37063*eV, 2.37517*eV, 2.37973*eV, 2.38431*eV, 2.3889*eV, 2.39351*eV, 2.39814*eV, 2.40279*eV, 2.40746*eV, 2.41214*eV, 
	2.41684*eV, 2.42156*eV, 2.4263*eV, 2.43106*eV, 2.43583*eV, 2.44063*eV, 2.44544*eV, 2.45028*eV, 2.45513*eV, 2.46*eV, 
	2.46489*eV, 2.4698*eV, 2.47473*eV, 2.47968*eV, 2.48465*eV, 2.48964*eV, 2.49465*eV, 2.49968*eV, 2.50473*eV, 2.5098*eV, 
	2.51489*eV, 2.52*eV, 2.52513*eV, 2.53029*eV, 2.53546*eV, 2.54066*eV, 2.54587*eV, 2.55111*eV, 2.55637*eV, 2.56165*eV, 
	2.56696*eV, 2.57228*eV, 2.57763*eV, 2.583*eV, 2.58839*eV, 2.59381*eV, 2.59924*eV, 2.60471*eV, 2.61019*eV, 2.6157*eV, 
	2.62123*eV, 2.62678*eV, 2.63236*eV, 2.63796*eV, 2.64358*eV, 2.64923*eV, 2.6549*eV, 2.6606*eV, 2.66632*eV, 2.67207*eV, 
	2.67784*eV, 2.68364*eV, 2.68946*eV, 2.6953*eV, 2.70118*eV, 2.70707*eV, 2.713*eV, 2.71895*eV, 2.72492*eV, 2.73092*eV, 
	2.73695*eV, 2.74301*eV, 2.74909*eV, 2.7552*eV, 2.76134*eV, 2.7675*eV, 2.77369*eV, 2.77991*eV, 2.78616*eV, 2.79243*eV, 
	2.79874*eV, 2.80507*eV, 2.81143*eV, 2.81782*eV, 2.82424*eV, 2.83068*eV, 2.83716*eV, 2.84367*eV, 2.85021*eV, 2.85677*eV, 
	2.86337*eV, 2.87*eV, 2.87666*eV, 2.88335*eV, 2.89007*eV, 2.89682*eV, 2.90361*eV, 2.91042*eV, 2.91727*eV, 2.92415*eV, 
	2.93106*eV, 2.93801*eV, 2.94499*eV, 2.952*eV, 2.95904*eV, 2.96612*eV, 2.97324*eV, 2.98038*eV, 2.98757*eV, 2.99478*eV, 
	3.00203*eV, 3.00932*eV, 3.01664*eV, 3.024*eV, 3.03139*eV, 3.03882*eV, 3.04629*eV, 3.05379*eV, 3.06133*eV, 3.06891*eV, 
	3.07653*eV, 3.08418*eV, 3.09187*eV, 3.0996*eV, 3.10737*eV, 3.11518*eV, 3.12302*eV, 3.13091*eV, 3.13883*eV, 3.1468*eV, 
	3.15481*eV, 3.16286*eV, 3.17095*eV, 3.17908*eV, 3.18725*eV, 3.19546*eV, 3.20372*eV, 3.21202*eV, 3.22036*eV, 3.22875*eV, 
	3.23718*eV, 3.24565*eV, 3.25417*eV, 3.26274*eV, 3.27135*eV, 3.28*eV, 3.2887*eV, 3.29745*eV, 3.30624*eV, 3.31508*eV, 
	3.32397*eV, 3.3329*eV, 3.34189*eV, 3.35092*eV, 3.36*eV, 3.36913*eV, 3.37831*eV, 3.38754*eV, 3.39682*eV, 3.40615*eV, 
	3.41554*eV, 3.42497*eV, 3.43446*eV, 3.444*eV, 3.45359*eV, 3.46324*eV, 3.47294*eV, 3.4827*eV, 3.49251*eV, 3.50237*eV, 
	3.51229*eV, 3.52227*eV, 3.53231*eV, 3.5424*eV, 3.55255*eV, 3.56276*eV, 3.57303*eV, 3.58335*eV, 3.59374*eV, 3.60419*eV, 
	3.61469*eV, 3.62526*eV, 3.63589*eV, 3.64659*eV, 3.65734*eV, 3.66817*eV, 3.67905*eV, 3.69*eV, 3.70101*eV, 3.7121*eV, 
	3.72324*eV, 3.73446*eV, 3.74574*eV, 3.75709*eV, 3.76851*eV, 3.78*eV, 3.79156*eV, 3.80319*eV, 3.81489*eV, 3.82667*eV, 
	3.83851*eV, 3.85043*eV, 3.86243*eV, 3.8745*eV, 3.88665*eV, 3.89887*eV, 3.91117*eV, 3.92354*eV, 3.936*eV, 3.94853*eV, 
	3.96115*eV, 3.97385*eV, 3.98662*eV, 3.99948*eV, 4.01243*eV, 4.02545*eV, 4.03857*eV, 4.05176*eV, 4.06505*eV, 4.07842*eV, 
	4.09188*eV, 4.10543*eV, 4.11907*eV, 4.1328*eV, 4.14662*eV, 4.16054*eV, 4.17454*eV, 4.18865*eV, 4.20285*eV, 4.21714*eV, 
	4.23154*eV, 4.24603*eV, 4.26062*eV, 4.27531*eV, 4.2901*eV, 4.305*eV, 4.32*eV, 4.3351*eV, 4.35032*eV, 4.36563*eV, 
	4.38106*eV, 4.39659*eV, 4.41224*eV, 4.428*eV, 4.44387*eV, 4.45986*eV, 4.47596*eV, 4.49217*eV, 4.50851*eV, 4.52496*eV, 
	4.54154*eV, 4.55823*eV, 4.57505*eV, 4.592*eV, 4.60907*eV, 4.62627*eV, 4.64359*eV, 4.66105*eV, 4.67864*eV, 4.69636*eV, 
	4.71422*eV, 4.73221*eV, 4.75034*eV, 4.76861*eV, 4.78703*eV, 4.80558*eV, 4.82428*eV, 4.84312*eV, 4.86212*eV, 4.88126*eV, 
	4.90055*eV, 4.92*eV, 4.9396*eV, 4.95936*eV, 4.97928*eV, 4.99935*eV, 5.01959*eV, 5.04*eV, 5.06057*eV, 5.08131*eV, 
	5.10222*eV, 5.1233*eV, 5.14456*eV, 5.166*eV, 5.18761*eV, 5.20941*eV, 5.23139*eV, 5.25356*eV, 5.27591*eV, 5.29846*eV, 
	5.3212*eV, 5.34414*eV, 5.36727*eV, 5.39061*eV, 5.41415*eV, 5.43789*eV, 5.46185*eV, 5.48602*eV, 5.5104*eV, 5.535*eV, 
	5.55982*eV, 5.58486*eV, 5.61013*eV, 5.63564*eV, 5.66137*eV, 5.68734*eV, 5.71355*eV, 5.74*eV, 5.7667*eV, 5.79364*eV, 
	5.82084*eV, 5.8483*eV, 5.87602*eV, 5.904*eV, 5.93225*eV, 5.96077*eV, 5.98956*eV, 6.01864*eV, 6.048*eV, 6.07765*eV, 
	6.10759*eV, 6.13782*eV, 6.16836*eV, 6.1992*eV
    };

  return PhotonEnergy_ABS;
}

G4double* EljenEJ286::GetAbs() {
  static G4double AbsPlate[nEntries_abslength_eljen] = {
	13028.8*cm, 3065.61*cm, 1861.26*cm, 1579.25*cm, 1271.11*cm, 1021.87*cm, 914.304*cm, 744.505*cm, 627.896*cm, 531.789*cm, 
	457.152*cm, 407.151*cm, 367.009*cm, 338.411*cm, 319.726*cm, 292.783*cm, 277.209*cm, 263.209*cm, 251.765*cm, 241.275*cm, 
	232.658*cm, 228.576*cm, 226.588*cm, 225.608*cm, 227.578*cm, 229.583*cm, 230.599*cm, 231.624*cm, 232.658*cm, 239.061*cm, 
	245.827*cm, 252.987*cm, 268.636*cm, 306.561*cm, 321.7*cm, 340.623*cm, 354.526*cm, 369.612*cm, 377.647*cm, 380.404*cm, 
	386.04*cm, 386.04*cm, 386.04*cm, 369.612*cm, 340.623*cm, 312.068*cm, 286.348*cm, 252.987*cm, 220.828*cm, 191.601*cm, 
	169.206*cm, 152.831*cm, 140.473*cm, 131.604*cm, 126.801*cm, 125.277*cm, 126.187*cm, 128.68*cm, 132.609*cm, 138.237*cm, 
	144.765*cm, 151.94*cm, 158.888*cm, 165.972*cm, 173.141*cm, 179.708*cm, 184.806*cm, 188.142*cm, 190.899*cm, 193.02*cm, 
	195.922*cm, 198.914*cm, 202.783*cm, 208.461*cm, 213.587*cm, 213.587*cm, 212.716*cm, 207.631*cm, 201.997*cm, 195.189*cm, 
	187.465*cm, 180.956*cm, 176.065*cm, 170.87*cm, 168.114*cm, 165.972*cm, 165.446*cm, 166.503*cm, 167.573*cm, 169.757*cm, 
	173.141*cm, 174.884*cm, 177.263*cm, 177.263*cm, 177.263*cm, 177.263*cm, 176.662*cm, 173.141*cm, 170.87*cm, 168.114*cm, 
	165.446*cm, 162.353*cm, 159.863*cm, 157.448*cm, 154.645*cm, 153.28*cm, 150.188*cm, 148.055*cm, 145.981*cm, 143.568*cm, 
	140.852*cm, 138.237*cm, 136.071*cm, 134.318*cm, 131.938*cm, 129.318*cm, 127.111*cm, 124.977*cm, 122.624*cm, 120.359*cm, 
	118.444*cm, 116.589*cm, 114.038*cm, 111.835*cm, 109.948*cm, 108.348*cm, 106.794*cm, 105.071*cm, 104.231*cm, 103.199*cm, 
	101.987*cm, 100.415*cm, 99.0786*cm, 97.9612*cm, 96.6889*cm, 95.4493*cm, 94.2411*cm, 93.5643*cm, 92.7319*cm, 92.0766*cm, 
	90.7933*cm, 90.165*cm, 89.3917*cm, 88.3311*cm, 87.5888*cm, 86.2837*cm, 85.5753*cm, 84.7404*cm, 83.7867*cm, 82.9862*cm, 
	82.2009*cm, 81.4302*cm, 80.5492*cm, 79.5654*cm, 78.6053*cm, 77.2079*cm, 75.9699*cm, 74.557*cm, 73.1957*cm, 71.9825*cm, 
	70.8089*cm, 69.8597*cm, 69.3023*cm, 68.5728*cm, 68.1246*cm, 67.8585*cm, 67.5069*cm, 67.1589*cm, 66.729*cm, 66.2203*cm, 
	65.8022*cm, 65.3074*cm, 64.8201*cm, 64.4195*cm, 63.9452*cm, 63.5553*cm, 63.0936*cm, 62.5634*cm, 62.116*cm, 61.6021*cm, 
	61.025*cm, 60.3886*cm, 59.6968*cm, 59.222*cm, 58.6884*cm, 58.2294*cm, 57.7136*cm, 57.2067*cm, 56.7705*cm, 56.3409*cm, 
	55.7383*cm, 55.2069*cm, 54.6282*cm, 54.0616*cm, 53.6166*cm, 53.1247*cm, 52.7483*cm, 52.2722*cm, 51.8045*cm, 51.2946*cm, 
	50.8442*cm, 50.3044*cm, 49.8235*cm, 49.4453*cm, 48.9346*cm, 48.5697*cm, 48.1657*cm, 47.7247*cm, 47.3346*cm, 46.9931*cm, 
	46.5731*cm, 46.1606*cm, 45.7553*cm, 45.3571*cm, 45.0435*cm, 44.6575*cm, 44.2781*cm, 43.9792*cm, 43.6112*cm, 43.3211*cm, 
	43.035*cm, 42.7175*cm, 42.3702*cm, 41.9946*cm, 41.6923*cm, 41.3614*cm, 40.9712*cm, 40.6516*cm, 40.337*cm, 40.0887*cm, 
	39.7827*cm, 39.4813*cm, 39.1845*cm, 38.892*cm, 38.5754*cm, 38.1797*cm, 37.8745*cm, 37.5741*cm, 37.3052*cm, 37.0664*cm, 
	36.7786*cm, 36.5209*cm, 36.292*cm, 36.091*cm, 35.8427*cm, 35.5736*cm, 35.3085*cm, 35.0709*cm, 34.8132*cm, 34.5135*cm, 
	34.2639*cm, 34.0178*cm, 33.7753*cm, 33.4502*cm, 33.1523*cm, 32.9219*cm, 32.6333*cm, 32.3698*cm, 32.1303*cm, 31.8748*cm, 
	31.6618*cm, 31.5277*cm, 31.357*cm, 31.1695*cm, 30.9841*cm, 30.7828*cm, 30.6021*cm, 30.388*cm, 30.1419*cm, 29.917*cm, 
	29.6953*cm, 29.5104*cm, 29.3112*cm, 29.0985*cm, 28.9048*cm, 28.6821*cm, 28.4939*cm, 28.2928*cm, 28.0794*cm, 27.8543*cm, 
	27.6181*cm, 27.4436*cm, 27.2427*cm, 27.0588*cm, 26.8636*cm, 26.6575*cm, 26.4679*cm, 26.2678*cm, 26.0707*cm, 25.8509*cm, 
	25.6726*cm, 25.4967*cm, 25.3356*cm, 25.1401*cm, 24.9595*cm, 24.7814*cm, 24.5595*cm, 24.3757*cm, 24.1722*cm, 23.9831*cm, 
	23.797*cm, 23.6244*cm, 23.4332*cm, 23.245*cm, 23.0599*cm, 22.8777*cm, 22.6884*cm, 22.5023*cm, 22.3001*cm, 22.1203*cm, 
	21.9433*cm, 21.7691*cm, 21.5888*cm, 21.4202*cm, 21.2716*cm, 21.1164*cm, 20.9467*cm, 20.7962*cm, 20.6561*cm, 20.5098*cm, 
	20.3735*cm, 20.2468*cm, 20.1373*cm, 20.029*cm, 19.9294*cm, 19.8308*cm, 19.7407*cm, 19.6365*cm, 19.5262*cm, 19.4243*cm, 
	19.3306*cm, 19.2024*cm, 19.0829*cm, 18.951*cm, 18.8346*cm, 18.6927*cm, 18.5266*cm, 18.3569*cm, 18.1713*cm, 17.977*cm, 
	17.7565*cm, 17.506*cm, 17.2282*cm, 16.9261*cm, 16.5814*cm, 16.205*cm, 15.8069*cm, 15.3778*cm, 14.9157*cm, 14.4364*cm, 
	13.9197*cm, 13.3526*cm, 12.7173*cm, 12.0026*cm, 11.2076*cm, 10.3424*cm, 9.39523*cm, 8.38002*cm, 7.32266*cm, 6.25409*cm, 
	5.22407*cm, 4.27105*cm, 3.43112*cm, 2.71958*cm, 2.1385*cm, 1.67687*cm, 1.31731*cm, 1.03998*cm, 0.826847*cm, 0.662758*cm, 
	0.535687*cm, 0.436663*cm, 0.359206*cm, 0.298826*cm, 0.25258*cm, 0.218972*cm, 0.19729*cm, 0.185792*cm, 0.180695*cm, 0.178475*cm, 
	0.177296*cm, 0.176483*cm, 0.175897*cm, 0.175384*cm, 0.174964*cm, 0.174622*cm, 0.174309*cm, 0.174114*cm, 0.173985*cm, 0.173864*cm, 
	0.173744*cm, 0.173626*cm, 0.173552*cm, 0.173484*cm, 0.173347*cm, 0.173216*cm, 0.173158*cm, 0.173128*cm, 0.173105*cm, 0.173085*cm, 
	0.173067*cm, 0.173088*cm, 0.173114*cm, 0.167886*cm, 0.167681*cm, 0.167488*cm, 0.167286*cm, 0.167102*cm, 0.166958*cm, 0.166833*cm, 
	0.166732*cm, 0.166645*cm, 0.166551*cm, 0.166514*cm, 0.166437*cm, 0.166353*cm, 0.166264*cm, 0.166179*cm, 0.166144*cm, 0.166057*cm, 
	0.166014*cm, 0.165917*cm, 0.165914*cm, 0.165893*cm, 0.165827*cm, 0.165741*cm, 0.165671*cm, 0.165602*cm, 0.165514*cm, 0.165454*cm, 
	0.165397*cm, 0.165354*cm, 0.165291*cm, 0.166951*cm, 0.166837*cm, 0.166702*cm, 0.166558*cm, 0.166398*cm, 0.166266*cm, 0.16611*cm, 
	0.16592*cm, 0.165713*cm, 0.165575*cm, 0.165579*cm, 0.165549*cm, 0.16547*cm, 0.165441*cm, 0.165412*cm, 0.165239*cm, 0.164992*cm, 
	0.164724*cm, 0.164475*cm, 0.164269*cm, 0.164091*cm, 0.163918*cm, 0.163709*cm, 0.163496*cm, 0.163389*cm, 0.163198*cm, 0.162996*cm, 
	0.162857*cm, 0.162677*cm, 0.162479*cm, 0.162256*cm, 0.161976*cm, 0.161573*cm, 0.161171*cm, 0.160681*cm, 0.160126*cm, 0.159531*cm, 
	0.158925*cm, 0.158273*cm, 0.157576*cm, 0.156984*cm, 0.156486*cm, 0.156011*cm, 0.155605*cm, 0.155224*cm, 0.154931*cm, 0.154726*cm, 
	0.15462*cm, 0.154425*cm, 0.15429*cm, 0.154142*cm, 0.154088*cm, 0.153931*cm, 0.153791*cm, 0.153704*cm, 0.153522*cm, 0.153435*cm, 
	0.153347*cm, 0.153216*cm, 0.153127*cm, 0.15303*cm, 0.152929*cm, 0.152828*cm, 0.152792*cm, 0.152657*cm, 0.152654*cm, 0.152552*cm, 
	0.152516*cm, 0.152449*cm, 0.15238*cm, 0.152364*cm, 0.152352*cm, 0.152285*cm, 0.152282*cm, 0.15227*cm, 0.152187*cm, 0.152151*cm, 
	0.152105*cm, 0.152102*cm, 0.152111*cm, 0.15205*cm, 0.152068*cm, 0.152068*cm, 0.152111*cm, 0.152074*cm, 0.152025*cm, 0.15209*cm, 
	0.152099*cm, 0.152163*cm, 0.152182*cm, 0.152212*cm, 0.15223*cm, 0.152252*cm, 0.152178*cm, 0.152151*cm, 0.152169*cm, 0.152175*cm, 
	0.152182*cm, 0.152242*cm, 0.152295*cm, 0.152331*cm, 0.152291*cm, 0.152319*cm, 0.152334*cm, 0.152291*cm, 0.152258*cm, 0.152203*cm, 
	0.152123*cm, 0.152022*cm, 0.151954*cm, 0.151908*cm, 0.151917*cm, 0.151824*cm, 0.151796*cm, 0.151759*cm, 0.151619*cm, 0.151513*cm, 
	0.15138*cm, 0.151361*cm, 0.151342*cm, 0.151323*cm, 0.151349*cm, 0.15144*cm, 0.151497*cm, 0.1516*cm, 0.151562*cm, 0.151584*cm, 
	0.151572*cm, 0.1515*cm, 0.151509*cm, 0.151534*cm, 0.151537*cm, 0.151562*cm, 0.151631*cm, 0.151737*cm, 0.151966*cm, 0.152074*cm, 
	0.152215*cm, 0.152446*cm, 0.152684*cm, 0.152961*cm
  };
  return AbsPlate;
}

// ###################### //
///////// EMISSION /////////
// ###################### //

#define nEntries_eljen_component 380

G4int EljenEJ286::GetNumEntries_EM() { return nEntries_eljen_component;}

G4double* EljenEJ286::GetPhotonEnergy_EM() {
  static G4double eljen_energy_for_component[nEntries_eljen_component] = {
	2.06812*eV, 2.07158*eV, 2.07505*eV, 2.07852*eV, 2.08201*eV, 2.08552*eV, 2.08903*eV, 2.09256*eV, 2.09609*eV, 2.09964*eV, 
	2.10321*eV, 2.10678*eV, 2.11037*eV, 2.11396*eV, 2.11757*eV, 2.1212*eV, 2.12483*eV, 2.12848*eV, 2.13214*eV, 2.13581*eV, 
	2.1395*eV, 2.1432*eV, 2.14691*eV, 2.15063*eV, 2.15437*eV, 2.15812*eV, 2.16188*eV, 2.16566*eV, 2.16945*eV, 2.17325*eV, 
	2.17707*eV, 2.1809*eV, 2.18474*eV, 2.1886*eV, 2.19247*eV, 2.19635*eV, 2.20025*eV, 2.20416*eV, 2.20809*eV, 2.21202*eV, 
	2.21598*eV, 2.21995*eV, 2.22393*eV, 2.22792*eV, 2.23193*eV, 2.23596*eV, 2.24*eV, 2.24405*eV, 2.24812*eV, 2.25221*eV, 
	2.25631*eV, 2.26042*eV, 2.26455*eV, 2.26869*eV, 2.27285*eV, 2.27702*eV, 2.28121*eV, 2.28542*eV, 2.28964*eV, 2.29388*eV, 
	2.29813*eV, 2.3024*eV, 2.30668*eV, 2.31098*eV, 2.31529*eV, 2.31963*eV, 2.32397*eV, 2.32834*eV, 2.33272*eV, 2.33712*eV, 
	2.34153*eV, 2.34596*eV, 2.35041*eV, 2.35487*eV, 2.35935*eV, 2.36385*eV, 2.36837*eV, 2.3729*eV, 2.37745*eV, 2.38202*eV, 
	2.3866*eV, 2.3912*eV, 2.39583*eV, 2.40046*eV, 2.40512*eV, 2.4098*eV, 2.41449*eV, 2.4192*eV, 2.42393*eV, 2.42868*eV, 
	2.43344*eV, 2.43823*eV, 2.44303*eV, 2.44786*eV, 2.4527*eV, 2.45756*eV, 2.46244*eV, 2.46734*eV, 2.47226*eV, 2.4772*eV, 
	2.48216*eV, 2.48714*eV, 2.49214*eV, 2.49716*eV, 2.5022*eV, 2.50726*eV, 2.51234*eV, 2.51744*eV, 2.52256*eV, 2.52771*eV, 
	2.53287*eV, 2.53805*eV, 2.54326*eV, 2.54849*eV, 2.55374*eV, 2.55901*eV, 2.5643*eV, 2.56962*eV, 2.57495*eV, 2.58031*eV, 
	2.58569*eV, 2.5911*eV, 2.59652*eV, 2.60197*eV, 2.60744*eV, 2.61294*eV, 2.61846*eV, 2.624*eV, 2.62956*eV, 2.63515*eV, 
	2.64077*eV, 2.6464*eV, 2.65206*eV, 2.65775*eV, 2.66346*eV, 2.66919*eV, 2.67495*eV, 2.68073*eV, 2.68654*eV, 2.69238*eV, 
	2.69824*eV, 2.70412*eV, 2.71003*eV, 2.71597*eV, 2.72193*eV, 2.72792*eV, 2.73394*eV, 2.73998*eV, 2.74605*eV, 2.75214*eV, 
	2.75826*eV, 2.76441*eV, 2.77059*eV, 2.7768*eV, 2.78303*eV, 2.78929*eV, 2.79558*eV, 2.8019*eV, 2.80824*eV, 2.81462*eV, 
	2.82102*eV, 2.82746*eV, 2.83392*eV, 2.84041*eV, 2.84693*eV, 2.85349*eV, 2.86007*eV, 2.86668*eV, 2.87333*eV, 2.88*eV, 
	2.8867*eV, 2.89344*eV, 2.90021*eV, 2.90701*eV, 2.91384*eV, 2.92071*eV, 2.9276*eV, 2.93453*eV, 2.94149*eV, 2.94849*eV, 
	2.95552*eV, 2.96258*eV, 2.96968*eV, 2.97681*eV, 2.98397*eV, 2.99117*eV, 2.9984*eV, 3.00567*eV, 3.01298*eV, 3.02032*eV, 
	3.02769*eV, 3.0351*eV, 3.04255*eV, 3.05004*eV, 3.05756*eV, 3.06512*eV, 3.07271*eV, 3.08035*eV, 3.08802*eV, 3.09573*eV, 
	3.10348*eV, 3.11127*eV, 3.11909*eV, 3.12696*eV, 3.13487*eV, 3.14281*eV, 3.1508*eV, 3.15883*eV, 3.1669*eV, 3.17501*eV, 
	3.18316*eV, 3.19135*eV, 3.19959*eV, 3.20786*eV, 3.21619*eV, 3.22455*eV, 3.23296*eV, 3.24141*eV, 3.24991*eV, 3.25845*eV, 
	3.26703*eV, 3.27567*eV, 3.28434*eV, 3.29307*eV, 3.30184*eV, 3.31065*eV, 3.31952*eV, 3.32843*eV, 3.33739*eV, 3.3464*eV, 
	3.35545*eV, 3.36456*eV, 3.37371*eV, 3.38292*eV, 3.39217*eV, 3.40148*eV, 3.41084*eV, 3.42025*eV, 3.42971*eV, 3.43922*eV, 
	3.44879*eV, 3.45841*eV, 3.46808*eV, 3.47781*eV, 3.48759*eV, 3.49743*eV, 3.50733*eV, 3.51728*eV, 3.52728*eV, 3.53735*eV, 
	3.54747*eV, 3.55765*eV, 3.56788*eV, 3.57818*eV, 3.58854*eV, 3.59895*eV, 3.60943*eV, 3.61997*eV, 3.63057*eV, 3.64123*eV, 
	3.65196*eV, 3.66275*eV, 3.6736*eV, 3.68452*eV, 3.6955*eV, 3.70655*eV, 3.71766*eV, 3.72884*eV, 3.74009*eV, 3.75141*eV, 
	3.76279*eV, 3.77425*eV, 3.78577*eV, 3.79737*eV, 3.80903*eV, 3.82077*eV, 3.83258*eV, 3.84446*eV, 3.85642*eV, 3.86845*eV, 
	3.88056*eV, 3.89275*eV, 3.90501*eV, 3.91735*eV, 3.92976*eV, 3.94226*eV, 3.95483*eV, 3.96749*eV, 3.98022*eV, 3.99304*eV, 
	4.00594*eV, 4.01893*eV, 4.032*eV, 4.04515*eV, 4.0584*eV, 4.07172*eV, 4.08514*eV, 4.09864*eV, 4.11224*eV, 4.12592*eV, 
	4.1397*eV, 4.15357*eV, 4.16753*eV, 4.18158*eV, 4.19574*eV, 4.20998*eV, 4.22433*eV, 4.23877*eV, 4.25331*eV, 4.26795*eV, 
	4.28269*eV, 4.29754*eV, 4.31249*eV, 4.32754*eV, 4.3427*eV, 4.35796*eV, 4.37333*eV, 4.38881*eV, 4.4044*eV, 4.42011*eV, 
	4.43592*eV, 4.45185*eV, 4.46789*eV, 4.48405*eV, 4.50033*eV, 4.51672*eV, 4.53324*eV, 4.54987*eV, 4.56663*eV, 4.58351*eV, 
	4.60052*eV, 4.61765*eV, 4.63492*eV, 4.65231*eV, 4.66983*eV, 4.68748*eV, 4.70527*eV, 4.7232*eV, 4.74126*eV, 4.75946*eV, 
	4.7778*eV, 4.79629*eV, 4.81491*eV, 4.83368*eV, 4.8526*eV, 4.87167*eV, 4.89089*eV, 4.91026*eV, 4.92978*eV, 4.94946*eV, 
	4.9693*eV, 4.98929*eV, 5.00945*eV, 5.02978*eV, 5.05026*eV, 5.07092*eV, 5.09174*eV, 5.11274*eV, 5.13391*eV, 5.15526*eV, 
	5.17678*eV, 5.19849*eV, 5.22038*eV, 5.24245*eV, 5.26471*eV, 5.28716*eV, 5.30981*eV, 5.33264*eV, 5.35568*eV, 5.37891*eV, 
	5.40235*eV, 5.42599*eV, 5.44985*eV, 5.47391*eV, 5.49818*eV, 5.52267*eV, 5.54738*eV, 5.57231*eV, 5.59747*eV, 5.62286*eV
      };

  return eljen_energy_for_component;
}

G4double *EljenEJ286::GetEm() {
  static G4double eljen_component_vs_energy[nEntries_eljen_component] = {
	30.5089, 24.3652, 18.9108, 14.057, 13.4173, 9.05551, 17.6713, 23.7538, 18.6373, 30.4587, 
	37.8401, 34.3346, 37.6347, 38.2739, 36.4491, 37.9368, 40.9634, 41.2123, 43.0908, 40.6553, 
	43.8286, 46.7081, 53.0591, 56.5856, 56.2894, 57.357, 64.2468, 68.9353, 72.5026, 74.2633, 
	80.6629, 90.7411, 94.5129, 97.4258, 100.869, 106.449, 115.255, 116.952, 119.68, 121.841, 
	129.267, 139.069, 143.429, 146.951, 155.396, 164.781, 170.97, 178.244, 179.491, 190.298, 
	200.432, 215.389, 223.186, 235.204, 250.464, 265.415, 279.805, 290.126, 299.501, 318.619, 
	339.1, 353.926, 373.58, 390.925, 410.372, 432.984, 452.398, 469.572, 489.184, 510.653, 
	535.765, 558.209, 583.945, 612.977, 643.836, 676.665, 700.784, 735.561, 765.652, 804.578, 
	839.262, 876.956, 912.343, 960.147, 1012.7, 1066.13, 1114.83, 1171.76, 1263.56, 1408.68, 
	1684.41, 2085.07, 2529.24, 2973.63, 3398.19, 3843.29, 4288.43, 4739.98, 5180.92, 5530.44, 
	5957.41, 6375.95, 6805.27, 7237.84, 7555.73, 7957.29, 8296.3, 8397.32, 8444.65, 8615.44, 
	8669.69, 8722.09, 8785.95, 8846.55, 8913.53, 8990.08, 9085.24, 9174.55, 9297.28, 9326.37, 
	9413.16, 9515.14, 9619.03, 9753.76, 9848.8, 9973.47, 10108.5, 10268.2, 10493, 10647.6, 
	10838.1, 11049.9, 11295.1, 11559.4, 11826.7, 12179.6, 12548, 12936.2, 13294.5, 13679.4, 
	14093.8, 14502.4, 14812.2, 15301.7, 15688.4, 16110.5, 16520.9, 16975.4, 17388.6, 17768.7, 
	18193.4, 18630.1, 19065, 19492.7, 19937.3, 20462, 21023.1, 21614.3, 22235.6, 22886.6, 
	23593.2, 24421.8, 25060.7, 25770.5, 26467, 27128.2, 27835.2, 28339.5, 28767.4, 29127.4, 
	29372.6, 29536.6, 29528.7, 29441.6, 29224.5, 28889.1, 28478.4, 27901.8, 27277.9, 26502.2, 
	25632.3, 24739.1, 23641, 22506.7, 21287.5, 19947.4, 18651.1, 17253.6, 15867, 14465.7, 
	13034.6, 11660.9, 10337.6, 9038.89, 7804.24, 6599.31, 5518.99, 4579.77, 3666.72, 2888.6, 
	2216.06, 1673.89, 1277.31, 920.492, 651.354, 451.711, 313.114, 221.159, 156.305, 112.801, 
	84.9561, 63.2998, 49.6208, 39.5509, 33.5793, 29.6825, 29.0258, 22.8064, 23.0578, 26.614, 
	28.7748, 26.0248, 25.8214, 27.3878, 27.049, 26.1555, 25.1733, 24.8057, 28.0867, 28.449, 
	28.9368, 29.8021, 27.8966, 28.1537, 27.7476, 27.9881, 20.4094, 20.4175, 24.0545, 23.1038, 
	22.9155, 22.0755, 21.7028, 23.7999, 21.7777, 16.2794, 13.9256, 17.4295, 15.5754, 16.5594, 
	14.0908, 14.7582, 18.5307, 19.4294, 18.4887, 18.5516, 17.0382, 20.9751, 23.2415, 20.0518, 
	23.8798, 24.7696, 26.9215, 29.9342, 27.0665, 31.2296, 31.5986, 33.4633, 35.2167, 32.4512, 
	29.0157, 30.2677, 31.2907, 30.7118, 25.3401, 25.8986, 23.5157, 22.4214, 22.5397, 23.051, 
	19.1204, 18.9691, 16.9159, 17.782, 13.2649, 13.6162, 19.0826, 19.6119, 20.0295, 18.2931, 
	17.4625, 20.5512, 23.6618, 22.853, 19.2703, 18.6003, 19.4221, 19.7589, 21.723, 20.3116, 
	19.8483, 20.4982, 19.8304, 20.1763, 18.5058, 19.8518, 20.2992, 21.4355, 16.6702, 14.5394, 
	18.8193, 20.2057, 21.0858, 17.9655, 19.6285, 19.854, 22.0102, 22.379, 21.9284, 21.9469, 
	20.9204, 21.8176, 21.4981, 17.3374, 20.12, 22.3727, 22.5763, 18.1656, 16.5643, 18.0199, 
	19.8482, 19.7989, 18.6625, 18.8546, 20.3136, 21.7623, 20.3002, 19.292, 18.5301, 21.734, 
	24.9841, 23.0722, 21.2413, 22.2118, 25.0339, 23.5018, 21.9702, 19.3274, 18.9602, 19.2639, 
	21.6156, 14.8791, 17.5215, 16.4008, 18.6484, 18.0436, 13.564, 15.1429, 15.2968, 20.8932, 
	20.8696, 21.6895, 21.518, 20.474, 27.0346, 22.812, 31.0448, 25.841, 29.9976, 25.1321, 
	26.5766, 22.2891, 22.988, 25.2949, 19.6909, 27.5086, 22.1865, 28.2814, 29.5177, 32.6297
      };

  return eljen_component_vs_energy;
}

// ###################### //
///////// STACKING /////////
// ###################### //

void EljenEJ286::SetgAbs(){
  // double AbsAmp[nEntries_abslength_eljen] =
  //     { 0, 0, 0, 0, 0, // 700 --- 650
  //       0, 0, 0, 0, 0, // 650 --- 600
  //       0, 0, 0, 0, 0, // 600 --- 550
  //       0, 0, 0, 0, 0, // 550 --- 500
  //       0, 0, 0, 0, 0, // 500 --- 450
  //       0, 0, 0.01, 0.05, 0.64, // 450 --- 400
  //       0.93, 0.9999, 0.9999, 0.9999, 0.9999, // 400 --- 350
  //       0.9999, 0.9999, 0.99, 0.97, 0.89, // 350 --- 300
  //       0.71, 0.54, 0, 0, // 300 --- 250
  //       0, 0, 0, 0, 0}; // 250 --- 200
  gAbs = new TGraph();
  const int step = 10;

  // std::cout << "Creating Stacking graph objects for WLS" << std::endl;
  // std::cout << "WLS ABS" << std::endl;
  // Then fill with point there is data
  for(int i=nEntries_abslength_eljen-1; i>0; i--){
    //    gAbs->SetPoint(i,round(hWL(GetPhotonEnergy_ABS()[i]))*nm,AbsAmp[i]);
    gAbs->SetPoint(i,round(hWL(GetPhotonEnergy_ABS()[i]))*nm,1. - exp(-1.2*cm/GetAbs()[i]));
    // std::cout << round(hWL(GetPhotonEnergy_ABS()[i]))*nm << "nm -> " << AbsAmp[i] << std::endl;
  }

}

void EljenEJ286::SetgEm(){
  gEm = new TGraph();
  const int step = 10;

  // std::cout << "Creating Stacking graph objects for WLS" << std::endl;
  // std::cout << "WLS EM" << std::endl;
  // Then fill with point there is data
  for(int i=nEntries_eljen_component-1; i>0; i--){
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