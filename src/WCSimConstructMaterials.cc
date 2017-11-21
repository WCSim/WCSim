#include "WCSimDetectorConstruction.hh"
#include "WCSimTuningParameters.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "globals.hh"
#include "G4UnitsTable.hh"
#include "G4NistManager.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpBoundaryProcess.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

void WCSimDetectorConstruction::ConstructMaterials()
{
  //****Materials Definitions****

  G4double density;
  G4double a;


  G4NistManager *nist_man = G4NistManager::Instance();


  //---Vacuum

  density     = universe_mean_density;              //from PhysicalConstants.h
  G4double pressure    = 1.e-19*pascal;
  G4double temperature = 0.1*kelvin;
  a = 1.01*g/mole;
  G4Material* Vacuum = 
    new G4Material("Vacuum", 1., a, density,
                   kStateGas,temperature,pressure);

  //---Water
  
  a = 1.01*g/mole;
  G4Element* elH 
    = new G4Element("Hydrogen","H", 1,a);
  
  a = 16.00*g/mole;
  G4Element* elO 
    = new G4Element("Oxygen","O", 8,a);
  
  density = 1.00*g/cm3;
  G4Material* Water 
    = new G4Material("Water",density,2);
  Water->AddElement(elH, 2);
  Water->AddElement(elO, 1);

 //---Gd doped Water

 a = 157.25*g/mole;
 G4Element* Gd
    = new G4Element("Gadolinium","Gd", 64,a);

 density = 1.00*g/cm3;
 G4Material* DopedWater
    = new G4Material("Doped Water",density,2);
 DopedWater->AddMaterial(Water,99.9*perCent);
 DopedWater->AddElement(Gd,0.1*perCent);

 //---Ice 
 
 density = 0.92*g/cm3;//Ice
 G4Material* Ice = new G4Material("Ice",density,2);
 Ice->AddElement(elH, 2);
 Ice->AddElement(elO, 1);

 //---Steel
  
  a= 12.01*g/mole;
  G4Element* elC 
    = new G4Element("Carbon","C", 6,a);
  
  a = 55.85*g/mole;
  G4Element* elFe
    = new G4Element("Iron","Fe", 26,a);
  
  density = 7.8*g/cm3;
  G4Material* Steel
    = new G4Material("Steel",density,2);
  Steel->AddElement(elC, 1.*perCent);
  Steel->AddElement(elFe, 99.*perCent);
  
  //---Stainless steel 304L (simple example, particular alloy can be different)
  
  a = 51.9961*g/mole;
  G4Element* elCr = new G4Element("Chromium", "Cr", 24., a);
  
  a = 58.6934*g/mole;
  G4Element* elNi = new G4Element("Nickel", "Ni", 28., a);

  a = 54.938*g/mole;
  G4Element* elMn = new G4Element("Manganese", "Mn", 25., a); 

  a = 30.974*g/mole;
  G4Element* elP = new G4Element("Phosphore", "P", 15., a);

  a = 28.09*g/mole;
  G4Element* elSi = new G4Element("Silicon", "Si", 14., a); 

  a = 32.065*g/mole;
  G4Element* elS = new G4Element("Sulphur", "S", 16., a);

  density = 7.81*g/cm3;
  G4Material* StainlessSteel = new G4Material("StainlessSteel", density, 8);
 
  StainlessSteel->AddElement(elFe, 70.44*perCent);
  StainlessSteel->AddElement(elCr, 18*perCent);
  StainlessSteel->AddElement(elC,  0.08*perCent);
  StainlessSteel->AddElement(elNi, 8*perCent); 
  StainlessSteel->AddElement(elP, 0.45*perCent);
  StainlessSteel->AddElement(elSi,  1*perCent);
  StainlessSteel->AddElement(elMn, 2*perCent);
  StainlessSteel->AddElement(elS, 0.03*perCent);
  
  G4MaterialPropertiesTable *mpt = new G4MaterialPropertiesTable();
   
  const G4int nEntries = 2;
  G4double photonEnergy[nEntries] = {1.*eV , 7.*eV};
  
  //G4double rindex_Steel[nEntries] = {1.462 , 1.462}; // No I haven't gone mad
  G4double abslength_Steel[nEntries] = {.001*mm , .001*mm};
  //mpt->AddProperty("RINDEX", photonEnergy, rindex_Steel, nEntries);
  mpt->AddProperty("ABSLENGTH", photonEnergy, abslength_Steel, nEntries);
  
  StainlessSteel->SetMaterialPropertiesTable(mpt);

  //---Solid Dry Ice

  density = 1.563*g/cm3;
  G4Material* DryIce = new G4Material("SolidDryIce", density, 2);
  DryIce->AddElement(elC, 1);
  DryIce->AddElement(elO, 2);



  //---Air
  
  a = 14.01*g/mole;
  G4Element* elN 
    = new G4Element("Nitrogen","N", 7,a);
  
  density = 1.290*mg/cm3;
  G4Material* Air 
    = new G4Material("Air",density,2);
  Air->AddElement(elN, 70.*perCent);
  Air->AddElement(elO, 30.*perCent);
  
  //---Plastic
  
  density = 0.95*g/cm3;
  G4Material* Plastic
    = new G4Material("Plastic",density,2);
  Plastic->AddElement(elC, 1);
  Plastic->AddElement(elH, 2);

  //---Aluminum (element and material)

  a = 26.98*g/mole;
  G4Element* elAl = new G4Element("Aluminum", "Al", 13, a);  

  density = 2.7*g/cm3;
  G4Material* Aluminum
    = new G4Material("Aluminum",density,1);
  Aluminum->AddElement(elAl, 1);


  // TF: --Silver (coating for reflector): higher reflectivity, but cutoff at 400nm + bad for water
  //       Also exposure to air not good for Silver
  // cfr. Wikipedia "Optical Coating" 

  a = 107.8682*g/mole;
  G4Element* elAg = new G4Element("Silver","Ag", 47, a);
  
  density = 10.5*g/cm3;
  G4Material *Silver
    = new G4Material("Silver",density,1);
  Silver->AddElement(elAg, 1);

  // ToDo: Add combinations of Al with silver coatings
  // ...


  // TF: Add Wacker SilGel for optical coupling (from wacker.com)
  // ToDo: update once final type of gel is decided and/or play with the gel in MC!!!
  density = 0.98*g/cm3;
  G4Material *SilGel
    = new G4Material("SilGel",density,1);
  SilGel->AddElement(elSi, 1);

  // TF: Add Acrylic from G4 database:
  G4Material* Acrylic 
    = nist_man->FindOrBuildMaterial("G4_PLEXIGLASS");




  //---Black sheet

  density = 0.95*g/cm3;
  G4Material* Blacksheet
    = new G4Material("Blacksheet",density,2);
  Blacksheet->AddElement(elC, 1);
  Blacksheet->AddElement(elH, 2);

  //---Tyvek - jl145

  density = 0.38*g/cm3;  //cf. DuPont product handbook
  G4Material* Tyvek
    = new G4Material("Tyvek",density,2);
  Tyvek->AddElement(elC, 1);  //polyethylene
  Tyvek->AddElement(elH, 2);

  //---Glass
 
  density = 2.20*g/cm3;
  G4Material* SiO2 = new G4Material("SiO2",density,2);
  SiO2->AddElement(elSi, 1);
  SiO2->AddElement(elO , 2);

  a = 10.81*g/mole;
  G4Element* elB = new G4Element("Boron", "B", 5, a);  

  density = 2.46*g/cm3;
  G4Material* B2O3 = new G4Material("B2O3",density,2);
  B2O3->AddElement(elB, 2);
  B2O3->AddElement(elO, 3);

  a = 22.99*g/mole;
  G4Element* elNa = new G4Element("Sodium", "Na", 11, a);  

  density = 2.27*g/cm3;
  G4Material* Na2O = new G4Material("Na2O",density,2);
  Na2O->AddElement(elNa, 2);
  Na2O->AddElement(elO, 1);

  density = 4.00*g/cm3;
  G4Material* Al2O3 = new G4Material("Al2O3",density,2);
  Al2O3->AddElement(elAl, 2);
  Al2O3->AddElement(elO, 3);

//   G4Material* blackAcryl
//     = new G4Material("blackAcryl", density, 3);
//   blackAcryl -> AddElement(elH, 6);
//   blackAcryl -> AddElement(elC, 4);
//   blackAcryl -> AddElement(elO, 2);

  density = 2.23*g/cm3;
  G4Material* Glass
    = new G4Material("Glass",density,4);
  //G4Material* Glass
  //= new G4Material("Glass",density,8);  //Put in 8 materials later
  
  Glass->AddMaterial(SiO2, 80.6*perCent);
  Glass->AddMaterial(B2O3, 13.0*perCent);
  Glass->AddMaterial(Na2O, 4.0*perCent);
  Glass->AddMaterial(Al2O3, 2.4*perCent);
  //Glass->AddMaterial(Al2O3, 2.3*perCent);  
  //Put in 2.3 percent if the other 4 materials = 0.1 percent

  //---Rock
 
  //  a = 16.00*g/mole;  G4Element* elO  = new G4Element("Oxygen","O", 8,a);
  //  a = 28.09*g/mole;  G4Element* elSi = new G4Element("Silicon", "Si", 14., a); 
  //  a = 26.98*g/mole;  G4Element* elAl = new G4Element("Aluminum", "Al", 13, a);  
  //  a = 55.85*g/mole;  G4Element* elFe = new G4Element("Iron","Fe", 26,a);
  a = 40.08*g/mole;  G4Element* elCa = new G4Element("Calcium","Ca", 20,a);
  //  a = 22.99*g/mole;  G4Element* elNa = new G4Element("Sodium", "Na", 11, a);  
  a = 39.10*g/mole;  G4Element* elK = new G4Element("Potassium","K", 19,a);
  a = 24.30*g/mole;  G4Element* elMg = new G4Element("Magnesium","Mg",12,a);
 
  density = 2.7*g/cm3; 
  G4Material* Rock = new G4Material("Rock", density, 8);
  
  //From Daya-Bay 
  Rock->AddElement(elO,  48.50*perCent);
  Rock->AddElement(elSi, 34.30*perCent);
  Rock->AddElement(elAl,  8.00*perCent);
  Rock->AddElement(elFe,  2.00*perCent);
  Rock->AddElement(elCa,  0.20*perCent);
  Rock->AddElement(elNa,  2.40*perCent);
  Rock->AddElement(elK,   4.50*perCent);
  Rock->AddElement(elMg,  0.10*perCent);



  // Potential alternative material definitions for HK
  // Get nist material manager
  //G4NistManager* nist = G4NistManager::Instance();
  //nist->FindOrBuildMaterial("G4_AIR");
  //nist->FindOrBuildMaterial("G4_WATER");
  //nist->FindOrBuildMaterial("G4_Galactic");
  //nist->FindOrBuildMaterial("G4_Pyrex_Glass");
  //nist->FindOrBuildMaterial("G4_POLYETHYLENE");







// -------------------------------------------------------------
// Generate & Add Material Properties Table
// -------------------------------------------------------------


  const G4int NUMENTRIES = 32;
 
  G4double PPCKOV[NUMENTRIES] =
    { 2.034E-9*GeV, 2.068E-9*GeV, 2.103E-9*GeV, 2.139E-9*GeV,
      2.177E-9*GeV, 2.216E-9*GeV, 2.256E-9*GeV, 2.298E-9*GeV,
      2.341E-9*GeV, 2.386E-9*GeV, 2.433E-9*GeV, 2.481E-9*GeV,
      2.532E-9*GeV, 2.585E-9*GeV, 2.640E-9*GeV, 2.697E-9*GeV,
      2.757E-9*GeV, 2.820E-9*GeV, 2.885E-9*GeV, 2.954E-9*GeV,
      3.026E-9*GeV, 3.102E-9*GeV, 3.181E-9*GeV, 3.265E-9*GeV,
      3.353E-9*GeV, 3.446E-9*GeV, 3.545E-9*GeV, 3.649E-9*GeV,
      3.760E-9*GeV, 3.877E-9*GeV, 4.002E-9*GeV, 4.136E-9*GeV };


  // default values
  /*
  G4double RINDEX1[NUMENTRIES] =
    { 1.3435, 1.344,  1.3445, 1.345,  1.3455,
      1.346,  1.3465, 1.347,  1.3475, 1.348,
      1.3485, 1.3492, 1.35,   1.3505, 1.351,
      1.3518, 1.3522, 1.3530, 1.3535, 1.354,
      1.3545, 1.355,  1.3555, 1.356,  1.3568,
      1.3572, 1.358,  1.3585, 1.359,  1.3595,
      1.36,   1.3608};
  */
  
  // M Fechner,  values from refsg.F in apdetsim
  /*  G4double RINDEX1[NUMENTRIES] = 
    { 1.33332, 1.333364, 1.333396, 1.3343, 1.33465,
      1.33502, 1.3354, 1.33579, 1.3362, 1.33663, 1.33709,
      1.33756, 1.33806, 1.3386, 1.33915, 1.33974,
      1.34037, 1.34105, 1.34176, 1.34253, 1.34336,
      1.34425, 1.34521, 1.34626, 1.3474, 1.34864,
      1.35002, 1.35153, 1.35321, 1.35507, 1.35717, 1.35955 };
  */

   //From SKDETSIM water absorption
   const G4int NUMENTRIES_water=60;

   G4double ENERGY_water[NUMENTRIES_water] =
     { 1.56962e-09*GeV, 1.58974e-09*GeV, 1.61039e-09*GeV, 1.63157e-09*GeV, 
       1.65333e-09*GeV, 1.67567e-09*GeV, 1.69863e-09*GeV, 1.72222e-09*GeV, 
       1.74647e-09*GeV, 1.77142e-09*GeV, 1.7971e-09*GeV, 1.82352e-09*GeV, 
       1.85074e-09*GeV, 1.87878e-09*GeV, 1.90769e-09*GeV, 1.93749e-09*GeV, 
       1.96825e-09*GeV, 1.99999e-09*GeV, 2.03278e-09*GeV, 2.06666e-09*GeV,
       2.10169e-09*GeV, 2.13793e-09*GeV, 2.17543e-09*GeV, 2.21428e-09*GeV, 
       2.25454e-09*GeV, 2.29629e-09*GeV, 2.33962e-09*GeV, 2.38461e-09*GeV, 
       2.43137e-09*GeV, 2.47999e-09*GeV, 2.53061e-09*GeV, 2.58333e-09*GeV, 
       2.63829e-09*GeV, 2.69565e-09*GeV, 2.75555e-09*GeV, 2.81817e-09*GeV, 
       2.88371e-09*GeV, 2.95237e-09*GeV, 3.02438e-09*GeV, 3.09999e-09*GeV,
       3.17948e-09*GeV, 3.26315e-09*GeV, 3.35134e-09*GeV, 3.44444e-09*GeV, 
       3.54285e-09*GeV, 3.64705e-09*GeV, 3.75757e-09*GeV, 3.87499e-09*GeV, 
       3.99999e-09*GeV, 4.13332e-09*GeV, 4.27585e-09*GeV, 4.42856e-09*GeV, 
       4.59258e-09*GeV, 4.76922e-09*GeV, 4.95999e-09*GeV, 5.16665e-09*GeV, 
       5.39129e-09*GeV, 5.63635e-09*GeV, 5.90475e-09*GeV, 6.19998e-09*GeV };



      // Air
   G4double RINDEX_air[NUMENTRIES_water] = 
     { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
       

   // M Fechner : new ; define the water refraction index using refsg.F 
   //from skdetsim using the whole range.   
    G4double RINDEX1[NUMENTRIES_water] = 
     {1.32885, 1.32906, 1.32927, 1.32948, 1.3297, 1.32992, 1.33014, 
      1.33037, 1.3306, 1.33084, 1.33109, 1.33134, 1.3316, 1.33186, 1.33213,
      1.33241, 1.3327, 1.33299, 1.33329, 1.33361, 1.33393, 1.33427, 1.33462,
      1.33498, 1.33536, 1.33576, 1.33617, 1.3366, 1.33705, 1.33753, 1.33803,
      1.33855, 1.33911, 1.3397, 1.34033, 1.341, 1.34172, 1.34248, 1.34331,
      1.34419, 1.34515, 1.3462, 1.34733, 1.34858, 1.34994, 1.35145, 1.35312,
      1.35498, 1.35707, 1.35943, 1.36211, 1.36518, 1.36872, 1.37287, 1.37776,
      1.38362, 1.39074, 1.39956, 1.41075, 1.42535};
   

    G4double ABWFF = 1.0;

    // Get from the tuning parameters
    ABWFF = WCSimTuningParams->GetAbwff();

    //T. Akiri: Values from Skdetsim 
    G4double ABSORPTION_water[NUMENTRIES_water] =
      {
        16.1419*cm*ABWFF,  18.278*cm*ABWFF, 21.0657*cm*ABWFF, 24.8568*cm*ABWFF, 30.3117*cm*ABWFF, 
	38.8341*cm*ABWFF, 54.0231*cm*ABWFF, 81.2306*cm*ABWFF, 120.909*cm*ABWFF, 160.238*cm*ABWFF, 
	193.771*cm*ABWFF, 215.017*cm*ABWFF, 227.747*cm*ABWFF,  243.85*cm*ABWFF, 294.036*cm*ABWFF, 
	321.647*cm*ABWFF,  342.81*cm*ABWFF, 362.827*cm*ABWFF, 378.041*cm*ABWFF, 449.378*cm*ABWFF,
        739.434*cm*ABWFF, 1114.23*cm*ABWFF, 1435.56*cm*ABWFF, 1611.06*cm*ABWFF, 1764.18*cm*ABWFF, 
	2100.95*cm*ABWFF,  2292.9*cm*ABWFF, 2431.33*cm*ABWFF,  3053.6*cm*ABWFF, 4838.23*cm*ABWFF, 
	6539.65*cm*ABWFF, 7682.63*cm*ABWFF, 9137.28*cm*ABWFF, 12220.9*cm*ABWFF, 15270.7*cm*ABWFF, 
	19051.5*cm*ABWFF, 23671.3*cm*ABWFF, 29191.1*cm*ABWFF, 35567.9*cm*ABWFF,   42583*cm*ABWFF,
        49779.6*cm*ABWFF, 56465.3*cm*ABWFF,   61830*cm*ABWFF, 65174.6*cm*ABWFF, 66143.7*cm*ABWFF,   
	  64820*cm*ABWFF,   61635*cm*ABWFF, 57176.2*cm*ABWFF, 52012.1*cm*ABWFF, 46595.7*cm*ABWFF, 
	41242.1*cm*ABWFF, 36146.3*cm*ABWFF, 31415.4*cm*ABWFF, 27097.8*cm*ABWFF, 23205.7*cm*ABWFF, 
	19730.3*cm*ABWFF, 16651.6*cm*ABWFF, 13943.6*cm*ABWFF, 11578.1*cm*ABWFF, 9526.13*cm*ABWFF
      };

    //Xin Qian: proposed value for absorption length
    // G4double ABSORPTION_water[NUMENTRIES_water] =
//       {22.8154*cm*ABWFF, 28.6144*cm*ABWFF, 35.9923*cm*ABWFF, 45.4086*cm*ABWFF, 57.4650*cm*ABWFF,
//        72.9526*cm*ABWFF, 75*cm*ABWFF,      81.2317*cm*ABWFF, 120.901*cm*ABWFF, 160.243*cm*ABWFF,
//        193.797*cm*ABWFF, 215.045*cm*ABWFF, 227.786*cm*ABWFF, 243.893*cm*ABWFF, 294.113*cm*ABWFF,
//        321.735*cm*ABWFF, 342.931*cm*ABWFF, 362.967*cm*ABWFF, 378.212*cm*ABWFF, 449.602*cm*ABWFF,
//        740.143*cm*ABWFF, 1116.06*cm*ABWFF, 1438.78*cm*ABWFF, 1615.48*cm*ABWFF, 1769.86*cm*ABWFF,
//        2109.67*cm*ABWFF, 2304.13*cm*ABWFF, 2444.97*cm*ABWFF, 3076.83*cm*ABWFF, 4901.5*cm*ABWFF,
//        6666.57*cm*ABWFF, 7873.95*cm*ABWFF, 9433.81*cm*ABWFF, 10214.5*cm*ABWFF, 10845.8*cm*ABWFF,
//        15746.9*cm*ABWFF, 20201.8*cm*ABWFF, 22025.8*cm*ABWFF, 21142.2*cm*ABWFF, 15083.9*cm*ABWFF,
//        11751*cm*ABWFF,   8795.34*cm*ABWFF, 8741.23*cm*ABWFF, 7102.37*cm*ABWFF, 6060.68*cm*ABWFF,
//        4498.56*cm*ABWFF, 3039.56*cm*ABWFF, 2232.2*cm*ABWFF,  1938*cm*ABWFF,    1811.58*cm*ABWFF,
//        1610.32*cm*ABWFF, 1338.7*cm*ABWFF,  1095.3*cm*ABWFF,  977.525*cm*ABWFF, 965.258*cm*ABWFF,
//        1082.86*cm*ABWFF, 876.434*cm*ABWFF, 633.723*cm*ABWFF, 389.87*cm*ABWFF,  142.011*cm*ABWFF
//       };

    /*G4double ABSORPTION_water[NUMENTRIES_water] = //old
     { 22.8154*cm*ABWFF, 28.6144*cm*ABWFF, 35.9923*cm*ABWFF, 45.4086*cm*ABWFF, 57.4650*cm*ABWFF,
       72.9526*cm*ABWFF, 92.9156*cm*ABWFF, 118.737*cm*ABWFF, 152.255*cm*ABWFF, 195.925*cm*ABWFF,
       202.429*cm*ABWFF, 224.719*cm*ABWFF, 236.407*cm*ABWFF, 245.700*cm*ABWFF, 289.017*cm*ABWFF,
       305.810*cm*ABWFF, 316.456*cm*ABWFF, 326.797*cm*ABWFF, 347.222*cm*ABWFF, 414.938*cm*ABWFF,
       636.943*cm*ABWFF, 934.579*cm*ABWFF, 1245.33*cm*ABWFF, 1402.52*cm*ABWFF, 1550.39*cm*ABWFF,
       1745.20*cm*ABWFF, 1883.24*cm*ABWFF, 2016.13*cm*ABWFF, 2442.18*cm*ABWFF, 3831.28*cm*ABWFF,
       4652.89*cm*ABWFF, 5577.04*cm*ABWFF, 6567.08*cm*ABWFF, 7559.88*cm*ABWFF, 8470.06*cm*ABWFF,
       9205.54*cm*ABWFF, 9690.95*cm*ABWFF, 9888.36*cm*ABWFF, 9804.50*cm*ABWFF, 9482.17*cm*ABWFF,
       8982.77*cm*ABWFF, 8369.39*cm*ABWFF, 7680.31*cm*ABWFF, 6902.11*cm*ABWFF, 6183.84*cm*ABWFF,
       5522.27*cm*ABWFF, 4914.33*cm*ABWFF, 4357.09*cm*ABWFF, 3847.72*cm*ABWFF, 3383.51*cm*ABWFF,
       2961.81*cm*ABWFF, 2580.08*cm*ABWFF, 2235.83*cm*ABWFF, 1926.66*cm*ABWFF, 1650.21*cm*ABWFF,
       1404.21*cm*ABWFF, 1186.44*cm*ABWFF, 994.742*cm*ABWFF, 827.027*cm*ABWFF, 681.278*cm*ABWFF};
    */
    
    /*
   G4double ABSORPTION_water[NUMENTRIES_water] = //new
     {25.3504*cm, 31.7938*cm, 39.9915*cm, 50.454*cm, 63.85*cm, 
      81.0584*cm, 103.24*cm, 131.93*cm, 169.172*cm, 217.694*cm, 
      224.921*cm, 249.688*cm, 262.674*cm, 273*cm, 321.13*cm, 339.789*cm,
      351.617*cm, 363.108*cm, 385.802*cm, 461.042*cm, 707.714*cm, 
      1038.42*cm, 1383.7*cm, 1558.36*cm, 1722.65*cm, 1939.11*cm, 
      2092.49*cm, 2240.14*cm, 2962.96*cm, 4967.03*cm, 6368.58*cm, 
      8207.56*cm, 10634.2*cm, 13855.3*cm, 18157.3*cm, 23940.2*cm, 
      31766*cm, 42431.6*cm, 57074.9*cm, 77335.9*cm, 105598*cm, 
      145361*cm, 192434*cm, 183898*cm, 176087*cm, 168913*cm, 162301*cm, 
      156187*cm, 150516*cm, 145243*cm, 140327*cm, 135733*cm, 131430*cm, 
      127392*cm, 123594*cm, 120016*cm, 116640*cm, 113448*cm, 110426*cm, 
      107562*cm};
  */
   // M Fechner: Rayleigh scattering -- as of version 4.6.2 of GEANT,
   // one may use one's own Rayleigh scattering lengths (the buffer is no
   // longer overwritten for "water", see 4.6.2 release notes)

   // RAYFF = 1/ARAS, for those of you who know SKdetsim...
   // actually that's not quite right because the scattering models
   // are different; in G4 there is no scattering depolarization
   // std value at SK = 0.6. But Mie scattering is implemented
   // in SKdetsim and not in G4

   
  // april 2005 : reduced reflections, let's increase scattering...
  // sep 09: for the large detector like superK the old values are muc better
   //G4double RAYFF = 1.0/1.65;  //old
   //    G4double RAYFF = 1.0/1.5;  

   G4double RAYFF = 0.625;

   // Get from the tuning parameters
   RAYFF = WCSimTuningParams->GetRayff();
   //    printf("RAYFF: %f\n",RAYFF);

   //T. Akiri: Values from Skdetsim 
   G4double RAYLEIGH_water[NUMENTRIES_water] = {
      386929*cm*RAYFF,  366249*cm*RAYFF,  346398*cm*RAYFF,  327355*cm*RAYFF,  309097*cm*RAYFF,  
      291603*cm*RAYFF,  274853*cm*RAYFF,  258825*cm*RAYFF,  243500*cm*RAYFF,  228856*cm*RAYFF,  
      214873*cm*RAYFF,  201533*cm*RAYFF,  188816*cm*RAYFF,  176702*cm*RAYFF,  165173*cm*RAYFF,
      154210*cm*RAYFF,  143795*cm*RAYFF,  133910*cm*RAYFF,  124537*cm*RAYFF,  115659*cm*RAYFF,  
      107258*cm*RAYFF, 99318.2*cm*RAYFF, 91822.2*cm*RAYFF,   84754*cm*RAYFF, 78097.3*cm*RAYFF, 
     71836.5*cm*RAYFF,   65956*cm*RAYFF, 60440.6*cm*RAYFF, 55275.4*cm*RAYFF, 50445.6*cm*RAYFF,
       45937*cm*RAYFF, 41735.2*cm*RAYFF, 37826.6*cm*RAYFF, 34197.6*cm*RAYFF, 30834.9*cm*RAYFF, 
     27725.4*cm*RAYFF, 24856.6*cm*RAYFF, 22215.9*cm*RAYFF, 19791.3*cm*RAYFF, 17570.9*cm*RAYFF,   
       15543*cm*RAYFF, 13696.6*cm*RAYFF, 12020.5*cm*RAYFF, 10504.1*cm*RAYFF, 9137.15*cm*RAYFF,
     7909.45*cm*RAYFF,  6811.3*cm*RAYFF, 5833.25*cm*RAYFF,  4966.2*cm*RAYFF, 4201.36*cm*RAYFF, 
     3530.28*cm*RAYFF, 2944.84*cm*RAYFF, 2437.28*cm*RAYFF, 2000.18*cm*RAYFF,  1626.5*cm*RAYFF, 
     1309.55*cm*RAYFF, 1043.03*cm*RAYFF, 821.016*cm*RAYFF,  637.97*cm*RAYFF, 488.754*cm*RAYFF
   };

   /*G4double RAYLEIGH_water[NUMENTRIES_water] = {
     167024.4*cm*RAYFF, 158726.7*cm*RAYFF, 150742*cm*RAYFF,
     143062.5*cm*RAYFF, 135680.2*cm*RAYFF, 128587.4*cm*RAYFF,
     121776.3*cm*RAYFF, 115239.5*cm*RAYFF, 108969.5*cm*RAYFF,
     102958.8*cm*RAYFF, 97200.35*cm*RAYFF, 91686.86*cm*RAYFF,
     86411.33*cm*RAYFF, 81366.79*cm*RAYFF, 76546.42*cm*RAYFF,
     71943.46*cm*RAYFF, 67551.29*cm*RAYFF, 63363.36*cm*RAYFF,
     59373.25*cm*RAYFF, 55574.61*cm*RAYFF, 51961.24*cm*RAYFF,
     48527.00*cm*RAYFF, 45265.87*cm*RAYFF, 42171.94*cm*RAYFF,
     39239.39*cm*RAYFF, 36462.50*cm*RAYFF, 33835.68*cm*RAYFF,
     31353.41*cm*RAYFF, 29010.30*cm*RAYFF, 26801.03*cm*RAYFF,
     24720.42*cm*RAYFF, 22763.36*cm*RAYFF, 20924.88*cm*RAYFF,
     19200.07*cm*RAYFF, 17584.16*cm*RAYFF, 16072.45*cm*RAYFF,
     14660.38*cm*RAYFF, 13343.46*cm*RAYFF, 12117.33*cm*RAYFF,
     10977.70*cm*RAYFF, 9920.416*cm*RAYFF, 8941.407*cm*RAYFF,
     8036.711*cm*RAYFF, 7202.470*cm*RAYFF, 6434.927*cm*RAYFF,
     5730.429*cm*RAYFF, 5085.425*cm*RAYFF, 4496.467*cm*RAYFF,
     3960.210*cm*RAYFF, 3473.413*cm*RAYFF, 3032.937*cm*RAYFF,
     2635.746*cm*RAYFF, 2278.907*cm*RAYFF, 1959.588*cm*RAYFF,
     1675.064*cm*RAYFF, 1422.710*cm*RAYFF, 1200.004*cm*RAYFF,
     1004.528*cm*RAYFF, 833.9666*cm*RAYFF, 686.1063*cm*RAYFF
     };*/


   // Get from the tuning parameters
   G4double MIEFF = WCSimTuningParams->GetMieff();
   //G4double MIEFF = 0.0;
   //    printf("MIEFF: %f\n",MIEFF);

   //Values extracted from Skdetsim
   G4double MIE_water[NUMENTRIES_water] = {
     7790020*cm*MIEFF, 7403010*cm*MIEFF, 7030610*cm*MIEFF, 6672440*cm*MIEFF, 6328120*cm*MIEFF, 
     5997320*cm*MIEFF, 5679650*cm*MIEFF, 5374770*cm*MIEFF, 5082340*cm*MIEFF, 4802000*cm*MIEFF, 
     4533420*cm*MIEFF, 4276280*cm*MIEFF, 4030220*cm*MIEFF, 3794950*cm*MIEFF, 3570120*cm*MIEFF,
     3355440*cm*MIEFF, 3150590*cm*MIEFF, 2955270*cm*MIEFF, 2769170*cm*MIEFF, 2592000*cm*MIEFF, 
     2423470*cm*MIEFF, 2263300*cm*MIEFF, 2111200*cm*MIEFF, 1966900*cm*MIEFF, 1830120*cm*MIEFF, 
     1700610*cm*MIEFF, 1578100*cm*MIEFF, 1462320*cm*MIEFF, 1353040*cm*MIEFF, 1250000*cm*MIEFF,
     1152960*cm*MIEFF, 1061680*cm*MIEFF,  975936*cm*MIEFF,  895491*cm*MIEFF,  820125*cm*MIEFF, 
      749619*cm*MIEFF,  683760*cm*MIEFF,  622339*cm*MIEFF,  565152*cm*MIEFF,  512000*cm*MIEFF, 
      462688*cm*MIEFF,  417027*cm*MIEFF,  374832*cm*MIEFF,  335923*cm*MIEFF,  300125*cm*MIEFF,
      267267*cm*MIEFF,  237184*cm*MIEFF,  209715*cm*MIEFF,  184704*cm*MIEFF,  162000*cm*MIEFF, 
      141456*cm*MIEFF,  122931*cm*MIEFF,  106288*cm*MIEFF, 91395.2*cm*MIEFF,   78125*cm*MIEFF, 
     66355.2*cm*MIEFF, 55968.2*cm*MIEFF, 46851.2*cm*MIEFF, 38896.2*cm*MIEFF,   32000*cm*MIEFF
   };

   //Mie scattering length values when assuming 10 times larger than Rayleigh scattering. 
   /*G4double MIE_water[NUMENTRIES_water] = {
      3869290*cm*MIEFF, 3662490*cm*MIEFF,  3463980*cm*MIEFF,  3273550*cm*MIEFF,  3090970*cm*MIEFF,  
      2916030*cm*MIEFF, 2748530*cm*MIEFF,  2588250*cm*MIEFF,  2435000*cm*MIEFF,  2288560*cm*MIEFF,  
      2148730*cm*MIEFF, 2015330*cm*MIEFF,  1888160*cm*MIEFF,  1767020*cm*MIEFF,  1651730*cm*MIEFF,
      1542100*cm*MIEFF, 1437950*cm*MIEFF,  1339100*cm*MIEFF,  1245370*cm*MIEFF,  1156590*cm*MIEFF,  
      1072580*cm*MIEFF,  993182*cm*MIEFF,   918222*cm*MIEFF,   847540*cm*MIEFF,   780973*cm*MIEFF, 
       718365*cm*MIEFF,   65956*cm*MIEFF,   604406*cm*MIEFF,   552754*cm*MIEFF,   504456*cm*MIEFF,
       459370*cm*MIEFF,  417352*cm*MIEFF,   378266*cm*MIEFF,   341976*cm*MIEFF,   308349*cm*MIEFF, 
       277254*cm*MIEFF,  248566*cm*MIEFF,   222159*cm*MIEFF,   197913*cm*MIEFF,   175709*cm*MIEFF,   
       155430*cm*MIEFF,  136966*cm*MIEFF,   120205*cm*MIEFF,   105041*cm*MIEFF,  91371.5*cm*MIEFF,
      79094.5*cm*MIEFF,   68113*cm*MIEFF,  58332.5*cm*MIEFF,    49662*cm*MIEFF,  42013.6*cm*MIEFF, 
      35302.8*cm*MIEFF, 29448.4*cm*MIEFF,  24372.8*cm*MIEFF,  20001.8*cm*MIEFF,    16265*cm*MIEFF, 
      13095.5*cm*MIEFF, 10430.3*cm*MIEFF,  8210.16*cm*MIEFF,   6379.7*cm*MIEFF,  4887.54*cm*MIEFF
   };
   */

   G4double MIE_water_const[3]={0.4,0.,1};// gforward, gbackward, forward backward ratio


   //From SKDETSIM
   /*
   G4double RINDEX_glass[NUMENTRIES] =
     { 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600 };
   */
   // M Fechner : unphysical, I want to reduce reflections
   // back to the old value 1.55

   
   /* TF: UNPHYSICAL 
      G4double RINDEX_glass[NUMENTRIES_water] =
     { 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600 }; 
   */

   // Based on analytical formula from http://refractiveindex.info/?shelf=glass&book=HIKARI-BK&page=J-BK7A for Borosilicate glass 
   G4double RINDEX_glass[NUMENTRIES_water] =
     { 1.5110, 1.5112, 1.5114, 1.5116, 1.5118, 
       1.5121, 1.5123, 1.5126, 1.5128, 1.5131, 
       1.5133, 1.5136, 1.5139, 1.5142, 1.5145, 
       1.5148, 1.5152, 1.5155, 1.5159, 1.5163, 
       1.5167, 1.5171, 1.5176, 1.5180, 1.5185, 
       1.5190, 1.5196, 1.5202, 1.5208, 1.5214, 
       1.5221, 1.5228, 1.5236, 1.5244, 1.5253, 
       1.5263, 1.5273, 1.5284, 1.5296, 1.5309, 
       1.5323, 1.5338, 1.5354, 1.5372, 1.5392, 
       1.5414, 1.5438, 1.5465, 1.5495, 1.5528, 
       1.5566, 1.5608, 1.5657, 1.5713, 1.5777, 
       1.5853, 1.5941, 1.6047, 1.6173, 1.6328 }; 
   

   //G4double RINDEX_blacksheet[NUMENTRIES] =
   //{ 2.500, 2.500, 2.500, 2.500, 2.500, 2.500, 2.500,
   //  2.500, 2.500, 2.500, 2.500, 2.500, 2.500, 2.500,
   //  2.500, 2.500, 2.500, 2.500, 2.500, 2.500, 2.500,
   //  2.500, 2.500, 2.500, 2.500, 2.500, 2.500, 2.500,
   //  2.500, 2.500, 2.500, 2.500 };
   
   
   //G4double ABSORPTION1[NUMENTRIES] =
   //{344.8*cm,  408.2*cm,  632.9*cm,  917.4*cm, 1234.6*cm, 1388.9*cm,
   // 1515.2*cm, 1724.1*cm, 1886.8*cm, 2000.0*cm, 2631.6*cm, 3571.4*cm,
   // 4545.5*cm, 4761.9*cm, 5263.2*cm, 5263.2*cm, 5555.6*cm, 5263.2*cm,
   // 5263.2*cm, 4761.9*cm, 4545.5*cm, 4166.7*cm, 3703.7*cm, 3333.3*cm,
   // 3000.0*cm, 2850.0*cm, 2700.0*cm, 2450.0*cm, 2200.0*cm, 1950.0*cm,
   // 1750.0*cm, 1450.0*cm };
   
   /*   
   G4double ABSORPTION_glass[NUMENTRIES] = 
     { 100.0*cm, 110.0*cm, 120.0*cm, 130.0*cm, 140.0*cm, 150.0*cm, 160.0*cm,
       165.0*cm, 170.0*cm, 175.0*cm, 180.0*cm, 185.0*cm, 190.0*cm, 195.0*cm,
       200.0*cm, 200.0*cm, 200.0*cm, 200.0*cm, 200.0*cm, 195.0*cm, 190.0*cm,
       185.0*cm, 180.0*cm, 175.0*cm, 170.0*cm, 160.0*cm, 150.0*cm, 140.0*cm,
       130.0*cm, 120.0*cm, 110.0*cm, 100.0*cm };
   */
   // M Fechner : the quantum efficiency already takes glass abs into account

   G4double ABSORPTION_glass[NUMENTRIES_water]= 
     { 1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,
       1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,
       1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,
       1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,
       1.0e9*cm, 1.0e9*cm,
       1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,
       1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,
       1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,
       1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,1.0e9*cm,
       1.0e9*cm, 1.0e9*cm };
   
   G4double BLACKABS_blacksheet[NUMENTRIES_water] =
     { 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 
       1.0e-9*cm,
       1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm,
       1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm,
       1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm,
       1.0e-9*cm, 1.0e-9*cm,
       1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 
       1.0e-9*cm,
       1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm,
       1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm,
       1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm, 1.0e-9*cm,
       1.0e-9*cm, 1.0e-9*cm};
   
   
   //The following reflectivity for blacksheet is obtained from skdetsim
   //There is a fudge factor of 2.7 for the reflectivity of blacksheet
   //depending on whether SK1 or SK2 simulation is used.  
   //The BlackSheetFudgeFactor is set to true if you want to use the 
   //SK2 values, false if not.
   //G4double SK1SK2FF = 1.0;
   //G4bool BlackSheetFudgeFactor=false;
   //G4bool BlackSheetFudgeFactor=true;
   //   if (BlackSheetFudgeFactor) SK1SK2FF=SK1SK2FF*2.7;
   //if (BlackSheetFudgeFactor) SK1SK2FF=SK1SK2FF*1.55;

   //July 1, 2010, F. Beroz: changed SK1SK2FF to BSRFF to avoid confusion,
   // since this parameter is not from SK.

   G4double BSRFF = 1.0;
   
   // Get from the tuning parameters
   BSRFF = WCSimTuningParams->GetBsrff();

   G4double REFLECTIVITY_blacksheet[NUMENTRIES_water] =
     { 0.055*BSRFF, 0.055*BSRFF, 
       0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 
       0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 
       0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 
       0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 
       0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 
       0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 
       0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 
       0.055*BSRFF, 0.057*BSRFF, 0.059*BSRFF, 0.060*BSRFF, 
       0.059*BSRFF, 0.058*BSRFF, 0.057*BSRFF, 0.055*BSRFF, 
       0.050*BSRFF, 0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF, 
       0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF,
       0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF,
       0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF,
       0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF,
       0.045*BSRFF, 0.045*BSRFF };

   /*
   G4double REFLECTIVITY_blacksheet[NUMENTRIES] =
     { 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.057*SK1SK2FF, 0.059*SK1SK2FF, 0.060*SK1SK2FF, 
       0.059*SK1SK2FF, 0.058*SK1SK2FF, 0.057*SK1SK2FF, 0.055*SK1SK2FF, 
       0.050*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 
       0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF,
       0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF };
   */
   
   /*
   G4double REFLECTIVITY_blacksheet[NUMENTRIES_water] =
     { 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.057*SK1SK2FF, 0.059*SK1SK2FF, 0.060*SK1SK2FF, 
       0.059*SK1SK2FF, 0.058*SK1SK2FF, 0.057*SK1SK2FF, 0.055*SK1SK2FF, 
       0.050*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 
       0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF,
       0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF,
       0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF,
       0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF ,
       0.045*SK1SK2FF, 0.045*SK1SK2FF };
   */

   //utter fiction at this stage
   G4double EFFICIENCY[NUMENTRIES_water] =
     { 0.001*m };
      
   //utter fiction at this stage, does not matter
   G4double RAYLEIGH_air[NUMENTRIES_water] =
     { 0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,
       0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,
       0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,
       0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,
       0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,
       0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,
       0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,
       0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,
       0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,
       0.001*m,0.001*m,0.001*m,0.001*m,0.001*m,0.001*m};
      
       //utter fiction at this stage, does not matter
   G4double MIE_air[NUMENTRIES_water] =
     { 0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,
       0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,
       0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,
       0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,
       0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,
       0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,
       0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,
       0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,
       0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,
       0.1*m,0.1*m,0.1*m,0.1*m,0.1*m,0.1*m};

   G4double MIE_air_const[3]={0.99,0.99,0.8};// gforward, gbackward, forward backward ratio

     
   //Not used yet, fictional values
   //G4double SPECULARLOBECONSTANT1[NUMENTRIES] =
   //{ 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001 };
   
   //Not used yet, fictional values
   //G4double SPECULARSPIKECONSTANT1[NUMENTRIES] =
   //{ 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001 };
   
   //Not used yet, fictional values
   //G4double BACKSCATTERCONSTANT1[NUMENTRIES] =
   //{ 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001 };
   
   G4double EFFICIENCY_blacksheet[NUMENTRIES_water] =
     { 0.0 };

   //TF: Al, Ag and coatings
   G4double REFLECTIVITY_aluminium[NUMENTRIES_water] =  //start simple with flat 90%
     { 0.9,0.9,0.9,0.9,0.9,0.9,
       0.9,0.9,0.9,0.9,0.9,0.9,
       0.9,0.9,0.9,0.9,0.9,0.9,
       0.9,0.9,0.9,0.9,0.9,0.9,
       0.9,0.9,0.9,0.9,0.9,0.9,
       0.9,0.9,0.9,0.9,0.9,0.9,
       0.9,0.9,0.9,0.9,0.9,0.9,
       0.9,0.9,0.9,0.9,0.9,0.9,
       0.9,0.9,0.9,0.9,0.9,0.9,
       0.9,0.9,0.9,0.9,0.9,0.9};

   // TF: Properties acrylic from skdetsim: refractive index and absorption is enough

   // photon energies between 295nm and 600nm, using hc/(e*lambda) up to 3 digits 
   // ToDo: remove comment: python magic: L = ['%.3f*eV' %(h*c/(i*1e-9*eC)) for i in range(295,601)]
   //                                     L[::-1] to revert list
   G4double ENERGY_skAcrylic[306] =
     { 2.066*eV, 2.070*eV, 2.073*eV, 2.077*eV, 2.080*eV, 2.084*eV, 2.087*eV,
       2.091*eV, 2.094*eV, 2.098*eV, 2.101*eV, 2.105*eV, 2.109*eV, 2.112*eV,
       2.116*eV, 2.119*eV, 2.123*eV, 2.127*eV, 2.130*eV, 2.134*eV, 2.138*eV,
       2.141*eV, 2.145*eV, 2.149*eV, 2.153*eV, 2.156*eV, 2.160*eV, 2.164*eV,
       2.168*eV, 2.171*eV, 2.175*eV, 2.179*eV, 2.183*eV, 2.187*eV, 2.191*eV,
       2.194*eV, 2.198*eV, 2.202*eV, 2.206*eV, 2.210*eV, 2.214*eV, 2.218*eV,
       2.222*eV, 2.226*eV, 2.230*eV, 2.234*eV, 2.238*eV, 2.242*eV, 2.246*eV,
       2.250*eV, 2.254*eV, 2.258*eV, 2.262*eV, 2.267*eV, 2.271*eV, 2.275*eV,
       2.279*eV, 2.283*eV, 2.288*eV, 2.292*eV, 2.296*eV, 2.300*eV, 2.305*eV,
       2.309*eV, 2.313*eV, 2.317*eV, 2.322*eV, 2.326*eV, 2.331*eV, 2.335*eV,
       2.339*eV, 2.344*eV, 2.348*eV, 2.353*eV, 2.357*eV, 2.362*eV, 2.366*eV,
       2.371*eV, 2.375*eV, 2.380*eV, 2.384*eV, 2.389*eV, 2.394*eV, 2.398*eV,
       2.403*eV, 2.407*eV, 2.412*eV, 2.417*eV, 2.422*eV, 2.426*eV, 2.431*eV,
       2.436*eV, 2.441*eV, 2.445*eV, 2.450*eV, 2.455*eV, 2.460*eV, 2.465*eV,
       2.470*eV, 2.475*eV, 2.480*eV, 2.485*eV, 2.490*eV, 2.495*eV, 2.500*eV,
       2.505*eV, 2.510*eV, 2.515*eV, 2.520*eV, 2.525*eV, 2.530*eV, 2.535*eV,
       2.541*eV, 2.546*eV, 2.551*eV, 2.556*eV, 2.562*eV, 2.567*eV, 2.572*eV,
       2.578*eV, 2.583*eV, 2.588*eV, 2.594*eV, 2.599*eV, 2.605*eV, 2.610*eV,
       2.616*eV, 2.621*eV, 2.627*eV, 2.632*eV, 2.638*eV, 2.644*eV, 2.649*eV,
       2.655*eV, 2.661*eV, 2.666*eV, 2.672*eV, 2.678*eV, 2.684*eV, 2.689*eV,
       2.695*eV, 2.701*eV, 2.707*eV, 2.713*eV, 2.719*eV, 2.725*eV, 2.731*eV,
       2.737*eV, 2.743*eV, 2.749*eV, 2.755*eV, 2.761*eV, 2.768*eV, 2.774*eV,
       2.780*eV, 2.786*eV, 2.792*eV, 2.799*eV, 2.805*eV, 2.811*eV, 2.818*eV,
       2.824*eV, 2.831*eV, 2.837*eV, 2.844*eV, 2.850*eV, 2.857*eV, 2.863*eV,
       2.870*eV, 2.877*eV, 2.883*eV, 2.890*eV, 2.897*eV, 2.904*eV, 2.910*eV,
       2.917*eV, 2.924*eV, 2.931*eV, 2.938*eV, 2.945*eV, 2.952*eV, 2.959*eV,
       2.966*eV, 2.973*eV, 2.980*eV, 2.988*eV, 2.995*eV, 3.002*eV, 3.009*eV,
       3.017*eV, 3.024*eV, 3.031*eV, 3.039*eV, 3.046*eV, 3.054*eV, 3.061*eV,
       3.069*eV, 3.077*eV, 3.084*eV, 3.092*eV, 3.100*eV, 3.107*eV, 3.115*eV,
       3.123*eV, 3.131*eV, 3.139*eV, 3.147*eV, 3.155*eV, 3.163*eV, 3.171*eV,
       3.179*eV, 3.187*eV, 3.195*eV, 3.204*eV, 3.212*eV, 3.220*eV, 3.229*eV,
       3.237*eV, 3.246*eV, 3.254*eV, 3.263*eV, 3.271*eV, 3.280*eV, 3.289*eV,
       3.297*eV, 3.306*eV, 3.315*eV, 3.324*eV, 3.333*eV, 3.342*eV, 3.351*eV, 
       3.360*eV, 3.369*eV, 3.378*eV, 3.388*eV, 3.397*eV, 3.406*eV, 3.416*eV, 
       3.425*eV, 3.434*eV, 3.444*eV, 3.454*eV, 3.463*eV, 3.473*eV, 3.483*eV, 
       3.493*eV, 3.502*eV, 3.512*eV, 3.522*eV, 3.532*eV, 3.542*eV, 3.553*eV, 
       3.563*eV, 3.573*eV, 3.583*eV, 3.594*eV, 3.604*eV, 3.615*eV, 3.625*eV, 
       3.636*eV, 3.647*eV, 3.657*eV, 3.668*eV, 3.679*eV, 3.690*eV, 3.701*eV, 
       3.712*eV, 3.723*eV, 3.734*eV, 3.746*eV, 3.757*eV, 3.769*eV, 3.780*eV, 
       3.792*eV, 3.803*eV, 3.815*eV, 3.827*eV, 3.839*eV, 3.850*eV, 3.862*eV, 
       3.875*eV, 3.887*eV, 3.899*eV, 3.911*eV, 3.924*eV, 3.936*eV, 3.949*eV, 
       3.961*eV, 3.974*eV, 3.987*eV, 3.999*eV, 4.012*eV, 4.025*eV, 4.039*eV, 
       4.052*eV, 4.065*eV, 4.078*eV, 4.092*eV, 4.105*eV, 4.119*eV, 4.133*eV, 
       4.147*eV, 4.161*eV, 4.175*eV, 4.189*eV, 4.203*eV };

   // reverted array from skdetsim acrnsg.F because that one is function of wavelength
   // instead of photon energy here.
   // Real part of refractive index (2002: R. Nambu)
   /* G4double RINDEX_skAcrylic[306] =
     { 1.50443324, 1.504443389, 1.504455332, 1.504469061, 1.50448457, 1.504501851, 
       1.504520896, 1.5045417, 1.504564254, 1.504588551, 1.504614584, 1.504642346, 
       1.50467183, 1.504703029, 1.504735934, 1.50477054, 1.504806839, 1.504844823, 
       1.504884485, 1.504925819, 1.504968817, 1.505013472, 1.505059776, 1.505107723, 
       1.505157305, 1.505208515, 1.505261346, 1.505315791, 1.505371841, 1.505429491, 
       1.505488733, 1.50554956, 1.505611965, 1.505675939, 1.505741477, 1.505808571, 
       1.505877213, 1.505947398, 1.506019116, 1.506092362, 1.506167127, 1.506243405, 
       1.506321189, 1.506400471, 1.506481244, 1.506563501, 1.506647235, 1.506732438, 
       1.506819104, 1.506907224, 1.506996793, 1.507087802, 1.507180244, 1.507274113, 
       1.507369401, 1.5074661, 1.507564205, 1.507663706, 1.507764598, 1.507866873, 
       1.507970524, 1.508075544, 1.508181924, 1.508289659, 1.508398741, 1.508509163, 
       1.508620917, 1.508733997, 1.508848395, 1.508964104, 1.509081116, 1.509199426, 
       1.509319024, 1.509439905, 1.50956206, 1.509685483, 1.509810167, 1.509936104, 
       1.510063288, 1.51019171, 1.510321364, 1.510452242, 1.510584337, 1.510717643, 
       1.510852152, 1.510987856, 1.511124749, 1.511262823, 1.511402071, 1.511542485, 
       1.51168406, 1.511826787, 1.511970659, 1.512115669, 1.51226181, 1.512409075, 
       1.512557456, 1.512706946, 1.512857538, 1.513009225, 1.513162, 1.513315855, 
       1.513470783, 1.513626777, 1.51378383, 1.513941935, 1.514101083, 1.514261269, 
       1.514422485, 1.514584724, 1.514747978, 1.514912241, 1.515077504, 1.515243762, 
       1.515411006, 1.51557923, 1.515748426, 1.515918587, 1.516089706, 1.516261775, 
       1.516434788, 1.516608738, 1.516783616, 1.516959417, 1.517136132, 1.517313754, 
       1.517492277, 1.517671692, 1.517851994, 1.518033174, 1.518215226, 1.518398141, 
       1.518581914, 1.518766537, 1.518952002, 1.519138302, 1.519325431, 1.519513381, 
       1.519702144, 1.519891714, 1.520082084, 1.520273245, 1.520465192, 1.520657916, 
       1.520851411, 1.521045669, 1.521240683, 1.521436447, 1.521632951, 1.521830191, 
       1.522028158, 1.522226844, 1.522426244, 1.522626349, 1.522827153, 1.523028648, 
       1.523230828, 1.523433684, 1.523637209, 1.523841398, 1.524046241, 1.524251732, 
       1.524457865, 1.524664631, 1.524872023, 1.525080035, 1.525288658, 1.525497887, 
       1.525707713, 1.525918129, 1.526129129, 1.526340704, 1.526552849, 1.526765554, 
       1.526978815, 1.527192622, 1.52740697, 1.52762185, 1.527837255, 1.52805318, 
       1.528269615, 1.528486554, 1.52870399, 1.528921915, 1.529140323, 1.529359205, 
       1.529578556, 1.529798368, 1.530018632, 1.530239344, 1.530460494, 1.530682076, 
       1.530904083, 1.531126507, 1.531349341, 1.531572579, 1.531796212, 1.532020234, 
       1.532244638, 1.532469415, 1.53269456, 1.532920065, 1.533145922, 1.533372124, 
       1.533598665, 1.533825537, 1.534052732, 1.534280244, 1.534508066, 1.534736189, 
       1.534964608, 1.535193314, 1.535422301, 1.535651561, 1.535881088, 1.536110873, 
       1.53634091, 1.536571192, 1.536801711, 1.53703246, 1.537263432, 1.537494619, 
       1.537726015, 1.537957613, 1.538189404, 1.538421383, 1.538653541, 1.538885871, 
       1.539118367, 1.539351021, 1.539583826, 1.539816774, 1.540049859, 1.540283073, 
       1.540516409, 1.54074986, 1.540983419, 1.541217077, 1.54145083, 1.541684668, 
       1.541918585, 1.542152573, 1.542386626, 1.542620736, 1.542854896, 1.543089099, 
       1.543323337, 1.543557604, 1.543791892, 1.544026194, 1.544260503, 1.544494811, 
       1.544729111, 1.544963397, 1.54519766, 1.545431894, 1.545666092, 1.545900246, 
       1.546134349, 1.546368394, 1.546602374, 1.546836281, 1.547070108, 1.547303849, 
       1.547537495, 1.54777104, 1.548004477, 1.548237797, 1.548470995, 1.548704063, 
       1.548936993, 1.549169778, 1.549402412, 1.549634887, 1.549867196, 1.550099331, 
       1.550331286, 1.550563052, 1.550794624, 1.551025994, 1.551257154, 1.551488097, 
       1.551718817, 1.551949306, 1.552179556, 1.552409561, 1.552639313, 1.552868806, 
       1.553098031, 1.553326982, 1.553555652, 1.553784032, 1.554012117, 1.554239899, 
       1.554467371, 1.554694525, 1.554921354, 1.555147851, 1.55537401, 1.555599822, 
       1.55582528, 1.556050378, 1.556275107, 1.556499462, 1.556723434, 1.556947017 };
   */

   // TF: from http://refractiveindex.info/?shelf=organic&book=poly%28methyl_methacrylate%29&page=Szczurowski
   G4double RINDEX_skAcrylic[306] =
     {1.4901, 1.4901, 1.4902, 1.4902, 1.4902, 1.4903, 1.4903, 1.4904, 1.4904, 1.4904, 
      1.4905, 1.4905, 1.4906, 1.4906, 1.4907, 1.4907, 1.4908, 1.4908, 1.4908, 1.4909, 
      1.4909, 1.4910, 1.4910, 1.4911, 1.4911, 1.4912, 1.4912, 1.4913, 1.4913, 1.4913, 
      1.4914, 1.4914, 1.4915, 1.4915, 1.4916, 1.4916, 1.4917, 1.4917, 1.4918, 1.4918, 
      1.4919, 1.4919, 1.4920, 1.4920, 1.4921, 1.4921, 1.4922, 1.4922, 1.4923, 1.4923, 
      1.4924, 1.4924, 1.4925, 1.4926, 1.4926, 1.4927, 1.4927, 1.4928, 1.4928, 1.4929, 
      1.4929, 1.4930, 1.4931, 1.4931, 1.4932, 1.4932, 1.4933, 1.4933, 1.4934, 1.4935, 
      1.4935, 1.4936, 1.4936, 1.4937, 1.4938, 1.4938, 1.4939, 1.4940, 1.4940, 1.4941, 
      1.4941, 1.4942, 1.4943, 1.4943, 1.4944, 1.4945, 1.4945, 1.4946, 1.4947, 1.4947, 
      1.4948, 1.4949, 1.4949, 1.4950, 1.4951, 1.4951, 1.4952, 1.4953, 1.4954, 1.4954, 
      1.4955, 1.4956, 1.4956, 1.4957, 1.4958, 1.4959, 1.4959, 1.4960, 1.4961, 1.4962, 
      1.4962, 1.4963, 1.4964, 1.4965, 1.4965, 1.4966, 1.4967, 1.4968, 1.4969, 1.4970, 
      1.4970, 1.4971, 1.4972, 1.4973, 1.4974, 1.4974, 1.4975, 1.4976, 1.4977, 1.4978, 
      1.4979, 1.4980, 1.4980, 1.4981, 1.4982, 1.4983, 1.4984, 1.4985, 1.4986, 1.4987, 
      1.4988, 1.4989, 1.4990, 1.4991, 1.4992, 1.4993, 1.4994, 1.4995, 1.4996, 1.4997, 
      1.4997, 1.4998, 1.5000, 1.5001, 1.5002, 1.5003, 1.5004, 1.5005, 1.5006, 1.5007, 
      1.5008, 1.5009, 1.5010, 1.5011, 1.5012, 1.5013, 1.5015, 1.5016, 1.5017, 1.5018, 
      1.5019, 1.5020, 1.5022, 1.5023, 1.5024, 1.5025, 1.5026, 1.5028, 1.5029, 1.5030, 
      1.5031, 1.5033, 1.5034, 1.5035, 1.5036, 1.5038, 1.5039, 1.5040, 1.5042, 1.5043, 
      1.5044, 1.5046, 1.5047, 1.5049, 1.5050, 1.5051, 1.5053, 1.5054, 1.5056, 1.5057, 
      1.5059, 1.5060, 1.5062, 1.5063, 1.5065, 1.5066, 1.5068, 1.5069, 1.5071, 1.5073, 
      1.5074, 1.5076, 1.5077, 1.5079, 1.5081, 1.5082, 1.5084, 1.5086, 1.5088, 1.5089, 
      1.5091, 1.5093, 1.5095, 1.5096, 1.5098, 1.5100, 1.5102, 1.5104, 1.5106, 1.5108, 
      1.5109, 1.5111, 1.5113, 1.5115, 1.5117, 1.5119, 1.5121, 1.5123, 1.5125, 1.5127, 
      1.5130, 1.5132, 1.5134, 1.5136, 1.5138, 1.5141, 1.5143, 1.5145, 1.5147, 1.5149, 
      1.5152, 1.5154, 1.5157, 1.5159, 1.5161, 1.5164, 1.5166, 1.5169, 1.5171, 1.5174, 
      1.5176, 1.5179, 1.5181, 1.5184, 1.5187, 1.5190, 1.5192, 1.5195, 1.5198, 1.5201, 
      1.5203, 1.5206, 1.5209, 1.5212, 1.5215, 1.5218, 1.5221, 1.5224, 1.5227, 1.5230, 
      1.5234, 1.5237, 1.5240, 1.5243, 1.5247, 1.5250, 1.5254, 1.5257, 1.5260, 1.5264, 
      1.5267, 1.5271, 1.5275, 1.5279, 1.5282, 1.5286, 1.5290, 1.5294, 1.5297, 1.5302, 
      1.5306, 1.5310, 1.5314, 1.5318, 1.5322, 1.5326};


   // Table from acrsg.F is abs coeff, so converted to abs length in m and inverted as
   // it was original a function of wavelength and now photon energy in increased energies.
   G4double ABSORPTION_skAcrylic[306] =
     {3472.22*m, 3355.70*m, 3300.33*m, 3412.97*m, 3496.50*m, 3649.64*m, 3861.00*m, 3921.57*m, 3846.15*m, 3703.70*m, 
      3448.28*m, 3048.78*m, 2932.55*m, 2770.08*m, 2808.99*m, 2906.98*m, 2898.55*m, 2906.98*m, 3003.00*m, 3164.56*m, 
      3184.71*m, 3246.75*m, 3086.42*m, 2994.01*m, 2994.01*m, 2967.36*m, 3030.30*m, 3039.51*m, 3105.59*m, 2958.58*m, 
      3012.05*m, 3039.51*m, 2967.36*m, 2932.55*m, 2923.98*m, 2890.17*m, 2857.14*m, 2865.33*m, 2915.45*m, 2898.55*m, 
      2881.84*m, 2754.82*m, 2702.70*m, 2754.82*m, 2890.17*m, 2840.91*m, 2659.57*m, 2604.17*m, 2624.67*m, 2857.14*m, 
      2849.00*m, 2849.00*m, 2610.97*m, 2597.40*m, 2688.17*m, 2590.67*m, 2439.02*m, 2380.95*m, 2398.08*m, 2320.19*m, 
      2369.67*m, 2293.58*m, 2267.57*m, 2262.44*m, 2320.19*m, 2309.47*m, 2237.14*m, 2247.19*m, 2257.34*m, 2336.45*m, 
      2347.42*m, 2421.31*m, 2439.02*m, 2392.34*m, 2392.34*m, 2500.00*m, 2500.00*m, 2512.56*m, 2481.39*m, 2444.99*m, 
      2155.17*m, 2053.39*m, 2008.03*m, 2040.82*m, 1988.07*m, 2004.01*m, 1976.28*m, 1834.86*m, 1798.56*m, 1869.16*m, 
      1808.32*m, 1754.39*m, 1805.05*m, 1841.62*m, 1872.66*m, 1964.64*m, 1976.28*m, 1893.94*m, 1890.36*m, 1908.40*m, 
      1897.53*m, 1855.29*m, 1886.79*m, 1904.76*m, 1841.62*m, 1855.29*m, 1834.86*m, 1805.05*m, 1733.10*m, 1779.36*m, 
      1754.39*m, 1788.91*m, 1831.50*m, 1834.86*m, 1869.16*m, 1912.05*m, 2004.01*m, 1968.50*m, 1915.71*m, 1818.18*m, 
      1795.33*m, 1766.78*m, 1808.32*m, 1766.78*m, 1754.39*m, 1751.31*m, 1776.20*m, 1838.24*m, 1872.66*m, 1919.39*m, 
      1901.14*m, 1988.07*m, 1926.78*m, 1893.94*m, 1901.14*m, 1851.85*m, 1818.18*m, 1769.91*m, 1727.12*m, 1724.14*m, 
      1709.40*m, 1689.19*m, 1692.05*m, 1680.67*m, 1697.79*m, 1727.12*m, 1763.67*m, 1697.79*m, 1692.05*m, 1666.67*m, 
      1600.00*m, 1636.66*m, 1615.51*m, 1610.31*m, 1538.46*m, 1501.50*m, 1519.76*m, 1492.54*m, 1515.15*m, 1457.73*m, 
      1479.29*m, 1461.99*m, 1470.59*m, 1470.59*m, 1453.49*m, 1512.86*m, 1522.07*m, 1589.83*m, 1577.29*m, 1597.44*m, 
      1600.00*m, 1672.24*m, 1663.89*m, 1647.45*m, 1661.13*m, 1607.72*m, 1543.21*m, 1488.10*m, 1459.85*m, 1373.63*m, 
      1326.26*m, 1373.63*m, 1410.44*m, 1406.47*m, 1396.65*m, 1308.90*m, 1328.02*m, 1386.96*m, 1347.71*m, 1297.02*m, 
      1262.63*m, 1234.57*m, 1283.70*m, 1319.26*m, 1310.62*m, 1319.26*m, 1402.52*m, 1459.85*m, 1464.13*m, 1472.75*m, 
      1420.45*m, 1432.66*m, 1400.56*m, 1362.40*m, 1358.70*m, 1358.70*m, 1379.31*m, 1360.54*m, 1362.40*m, 1355.01*m, 
      1342.28*m, 1367.99*m, 1293.66*m, 1221.00*m, 1197.60*m, 1165.50*m, 1119.82*m, 1107.42*m, 1108.65*m, 1040.58*m, 
      1033.06*m, 1000.00*m, 952.38*m, 925.93*m, 909.09*m, 869.57*m, 800.00*m, 781.25*m, 769.23*m, 729.93*m, 735.29*m, 
      729.93*m, 709.22*m, 684.93*m, 657.89*m, 645.16*m, 625.00*m, 581.40*m, 568.18*m, 558.66*m, 543.48*m, 540.54*m, 523.56*m, 
      505.05*m, 490.20*m, 485.44*m, 462.96*m, 450.45*m, 442.48*m, 429.18*m, 420.17*m, 411.52*m, 411.52*m, 393.70*m, 393.70*m, 
      386.10*m, 381.68*m, 355.87*m, 343.64*m, 331.13*m, 343.64*m, 338.98*m, 335.57*m, 326.80*m, 313.48*m, 303.95*m, 294.12*m, 
      292.40*m, 280.90*m, 273.97*m, 270.27*m, 265.25*m, 261.78*m, 256.41*m, 250.00*m, 242.13*m, 237.53*m, 233.10*m, 228.83*m, 
      226.76*m, 223.21*m, 219.30*m, 215.05*m, 211.86*m, 208.77*m, 206.61*m, 201.21*m, 196.85*m, 193.42*m, 189.75*m, 186.22*m, 
      182.15*m, 175.75*m, 168.92*m, 162.60*m, 154.80*m, 144.51*m, 133.51*m, 120.19*m, 105.93*m, 90.91*m, 75.76*m, 61.35*m, 
      48.54*m, 37.88*m, 29.41*m};


   // Can not use AddConstProperty for RINDEX (see http://hypernews.slac.stanford.edu/HyperNews/geant4/get/opticalphotons/379/1.html)
   // ToDo: update with actual wavelength dependence, once known.
    G4double RINDEX_SilGel[NUMENTRIES_water] = 
     {1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 
      1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 
      1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 
      1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 
      1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 
      1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404, 1.404};

    // From Lew Classen (Erlangen) with PHOTON_NRG_MIN = .1*eV, and MAX = 7*eV
    G4double ENERGY_SilGel[18] = { .1*eV,
				   2.034*eV,2.13*eV,2.18*eV,2.22*eV,2.25*eV,2.3*eV,
				   2.34*eV, 2.43*eV, 2.53*eV, 2.67*eV, 2.79*eV,
				   3.1*eV, 3.28*eV, 3.56*eV, 3.77*eV,4.136*eV, 7.*eV};
    G4double ABSORPTION_SilGel[18] = { 0*m,
				       .975*m, .975*m, .966*m, .935*m, .890*m, .898*m,
				       .876*m, .844*m, .786*m, .727*m,  .674*m,
				       .566*m, .485*m, .360*m, .220*m, .220*m, 0*m };

    // If necessary:
    //Reference: A. D. Rakić, A. B. Djurišic, J. M. Elazar, and M. L. Majewski. Optical properties of metallic films for vertical-cavity optoelectronic devices, Appl. Opt. 37, 5271-5283 (1998)
    // from http://refractiveindex.info/?shelf=3d&book=metals&page=aluminium
    /*
    G4double ENERGY_Al[64] =
      {1.644*eV, 1.678*eV, 1.713*eV, 1.748*eV, 1.785*eV, 1.822*eV, 
       1.860*eV, 1.898*eV, 1.938*eV, 1.978*eV, 2.019*eV, 2.061*eV, 
       2.104*eV, 2.148*eV, 2.193*eV, 2.238*eV, 2.285*eV, 2.332*eV, 
       2.381*eV, 2.430*eV, 2.481*eV, 2.532*eV, 2.585*eV, 2.639*eV, 
       2.693*eV, 2.749*eV, 2.807*eV, 2.865*eV, 2.925*eV, 2.985*eV, 
       3.047*eV, 3.111*eV, 3.175*eV, 3.241*eV, 3.309*eV, 3.378*eV, 
       3.448*eV, 3.519*eV, 3.593*eV, 3.667*eV, 3.744*eV, 3.821*eV, 
       3.901*eV, 3.982*eV, 4.065*eV, 4.149*eV, 4.235*eV, 4.324*eV, 
       4.413*eV, 4.505*eV, 4.599*eV, 4.694*eV, 4.792*eV, 4.892*eV, 
       4.993*eV, 5.097*eV, 5.203*eV, 5.311*eV, 5.422*eV, 5.534*eV, 
       5.649*eV, 5.767*eV, 5.887*eV, 6.009*eV};

    G4double RINDEX_Al[64] =
      {2.299, 2.096, 1.911, 1.752, 1.623, 1.518, 1.432, 
       1.361, 1.299, 1.245, 1.196, 1.150, 1.106, 1.063, 
       1.022, 0.982, 0.942, 0.903, 0.865, 0.827, 0.790, 
       0.754, 0.719, 0.686, 0.653, 0.622, 0.592, 0.564, 
       0.536, 0.511, 0.486, 0.463, 0.441, 0.420, 0.401, 
       0.382, 0.365, 0.349, 0.333, 0.318, 0.305, 0.292, 
       0.279, 0.268, 0.257, 0.246, 0.236, 0.227, 0.218, 
       0.210, 0.202, 0.194, 0.187, 0.180, 0.173, 0.166, 
       0.160, 0.154, 0.149, 0.143, 0.138, 0.133, 0.128, 0.123};
    */
   //	------------- Surfaces --------------

   OpWaterBSSurface =
     new G4OpticalSurface("WaterBSCellSurface");

   OpWaterBSSurface->SetType(dielectric_dielectric);
   OpWaterBSSurface->SetModel(unified); 
   OpWaterBSSurface->SetFinish(groundfrontpainted);
   OpWaterBSSurface->SetSigmaAlpha(0.1);

   BSSkinSurface =
     new G4OpticalSurface("BSSkinSurface");

   BSSkinSurface->SetType(dielectric_dielectric);
   BSSkinSurface->SetModel(unified); 
   BSSkinSurface->SetFinish(groundfrontpainted);
   BSSkinSurface->SetSigmaAlpha(0.1);

   const G4int NUM = 2;
   //   G4double PP[NUM] =
   //{ 2.038E-9*GeV, 4.144E-9*GeV };

   G4double PP[NUM] = { 1.4E-9*GeV,6.2E-9*GeV};
   G4double RINDEX_blacksheet[NUM] =
     { 1.6, 1.6 };

   G4double SPECULARLOBECONSTANT[NUM] =
     { 0.3, 0.3 };
   G4double SPECULARSPIKECONSTANT[NUM] =
     { 0.2, 0.2 };
   G4double BACKSCATTERCONSTANT[NUM] =
     { 0.2, 0.2 };

   OpGlassCathodeSurface =
     new G4OpticalSurface("GlassCathodeSurface");
   OpGlassCathodeSurface->SetType(dielectric_dielectric);
   OpGlassCathodeSurface->SetModel(unified);  
   //   OpGlassCathodeSurface->SetFinish(groundbackpainted);
   OpGlassCathodeSurface->SetFinish(polished);
   //OpGlassCathodeSurface->SetSigmaAlpha(0.002);
   // was 1.0
   // totally unphysical anyway 
   G4double RINDEX_cathode[NUM] =
     { 1.0, 1.0 };
   /*   
   G4double SPECULARLOBECONSTANT_glasscath[NUM] =
     { 1.0, 1.0 };
     // { 0.3, 0.3 };
   G4double SPECULARSPIKECONSTANT_glasscath[NUM] =
     { 0.0, 0.0 };
     //     { 0.2, 0.2 };
   G4double BACKSCATTERCONSTANT_glasscath[NUM] =
     {0.0, 0.0};
   //     { 0.2, 0.2 };
   */


   G4double RGCFF = 0.0;
   RGCFF = WCSimTuningParams->GetRgcff();   //defaults in mac: 0.32 and flat
 
   G4double REFLECTIVITY_glasscath[NUM] =
     //{ 0.0+RGCFF, 0.0+RGCFF };
     //{ RGCFF, RGCFF };
     //{ 0.0, 0.0 };
     { 1.0*RGCFF, 1.0*RGCFF };


   G4double EFFICIENCY_glasscath[NUM] =
     { 0.0, 0.0 };


   // jl145 ----
   //
   OpWaterTySurface =
     new G4OpticalSurface("WaterTyCellSurface");

   OpWaterTySurface->SetType(dielectric_dielectric);
   OpWaterTySurface->SetModel(unified); 
   OpWaterTySurface->SetFinish(groundbackpainted); //a guess, but seems to work
   OpWaterTySurface->SetSigmaAlpha(0.5); //cf. A. Chavarria's ~30deg

   G4double RINDEX_tyvek[NUM] =
     { 1.5, 1.5 }; // polyethylene permittivity is ~2.25
   G4double TySPECULARLOBECONSTANT[NUM] =
     { 0.75, 0.75 }; // crudely estimated from A. Chavarria's thesis
   G4double TySPECULARSPIKECONSTANT[NUM] =
     { 0.0, 0.0 };
   G4double TyBACKSCATTERCONSTANT[NUM] =
     { 0.0, 0.0 };
   // Lambertian prob is therefore 0.25

   G4double TyREFLECTIVITY[NUM] =
     { 0.94, 0.94 }; //cf. DuPont
   //
   // ----


   G4MaterialPropertiesTable *myMPT1 = new G4MaterialPropertiesTable();
   // M Fechner : new   ; wider range for lambda
   myMPT1->AddProperty("RINDEX", ENERGY_water, RINDEX1, NUMENTRIES_water);
   myMPT1->AddProperty("ABSLENGTH",ENERGY_water, ABSORPTION_water, NUMENTRIES_water);
   // M Fechner: new, don't let G4 compute it.
   myMPT1->AddProperty("RAYLEIGH",ENERGY_water,RAYLEIGH_water,NUMENTRIES_water);

  //  myMPT1->AddProperty("MIEHG",ENERGY_water,MIE_water,NUMENTRIES_water);
//    myMPT1->AddConstProperty("MIEHG_FORWARD",MIE_water_const[0]);
//    myMPT1->AddConstProperty("MIEHG_BACKWARD",MIE_water_const[1]);
//    myMPT1->AddConstProperty("MIEHG_FORWARD_RATIO",MIE_water_const[2]);


   Water->SetMaterialPropertiesTable(myMPT1);
   //Gd doped water has the same optical properties as pure water
   DopedWater->SetMaterialPropertiesTable(myMPT1);
   // myMPT1->DumpTable();
   
   G4MaterialPropertiesTable *myMPT2 = new G4MaterialPropertiesTable();
   myMPT2->AddProperty("RINDEX", ENERGY_water, RINDEX_air, NUMENTRIES_water);
   // M Fechner : what is that ?????
   myMPT2->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet, NUMENTRIES_water);
   myMPT2->AddProperty("RAYLEIGH",ENERGY_water, RAYLEIGH_air, NUMENTRIES_water);

   // myMPT2->AddProperty("MIEHG",ENERGY_water,MIE_air,NUMENTRIES_water);
//       myMPT2->AddConstProperty("MIEHG_FORWARD",MIE_air_const[0]);
//       myMPT2->AddConstProperty("MIEHG_BACKWARD",MIE_air_const[1]);
//       myMPT2->AddConstProperty("MIEHG_FORWARD_RATIO",MIE_air_const[2]);
   
   Air->SetMaterialPropertiesTable(myMPT2);
   
   G4MaterialPropertiesTable *myMPT3 = new G4MaterialPropertiesTable();
   myMPT3->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet, NUMENTRIES_water);
   myMPT3->AddProperty("REFLECTIVITY", ENERGY_water, REFLECTIVITY_blacksheet, NUMENTRIES_water);
   myMPT3->AddProperty("EFFICIENCY",   ENERGY_water, EFFICIENCY, NUMENTRIES_water);
   Plastic->SetMaterialPropertiesTable(myMPT3);
   
   G4MaterialPropertiesTable *myMPT4 = new G4MaterialPropertiesTable();
   myMPT4->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet, NUMENTRIES_water);
   //myMPT4->AddProperty("RINDEX", ENERGY_water, RINDEX_blacksheet, NUMENTRIES_water); //TF - never required if BS is properly defined as surface.
   Blacksheet->SetMaterialPropertiesTable(myMPT4);
   
   G4MaterialPropertiesTable *myMPT5 = new G4MaterialPropertiesTable();
   myMPT5->AddProperty("RINDEX", ENERGY_water, RINDEX_glass, NUMENTRIES_water);
   myMPT5->AddProperty("ABSLENGTH",ENERGY_water, ABSORPTION_glass, NUMENTRIES_water);
   Glass->SetMaterialPropertiesTable(myMPT5);
    
   // jl145 ----
   // Abs legnth is same as blacksheet, very small.
   G4MaterialPropertiesTable *myMPT6 = new G4MaterialPropertiesTable();
   myMPT6->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet, NUMENTRIES_water);
   Tyvek->SetMaterialPropertiesTable(myMPT6);

   /// SilGel : Currently based on WackerSilGel 612, BUT should be adjusted to best one (R&D)
   G4MaterialPropertiesTable *SilGelPropTable = new G4MaterialPropertiesTable();
   SilGelPropTable->AddProperty("RINDEX", ENERGY_water, RINDEX_SilGel, NUMENTRIES_water);
   SilGelPropTable->AddProperty("ABSLENGTH",ENERGY_SilGel, ABSORPTION_SilGel, 18); //ToDo: get measurement of optical properties of the optical gel. From slides: better than 40cm above 350nm.
   SilGelPropTable->AddProperty("RAYLEIGH",ENERGY_water,RAYLEIGH_water,NUMENTRIES_water); //ToDo: get actual Rayleigh scattering in gel
   SilGel->SetMaterialPropertiesTable(SilGelPropTable);

   //Acrylic
   G4MaterialPropertiesTable *AcrPropTable = new G4MaterialPropertiesTable();
   AcrPropTable->AddProperty("RINDEX", ENERGY_skAcrylic, RINDEX_skAcrylic, 306);
   AcrPropTable->AddProperty("ABSLENGTH", ENERGY_skAcrylic, ABSORPTION_skAcrylic, 306);
   Acrylic->SetMaterialPropertiesTable(AcrPropTable);



   //	------------- Surfaces --------------

   // Blacksheet
   G4MaterialPropertiesTable *myST1 = new G4MaterialPropertiesTable();
   myST1->AddProperty("RINDEX", ENERGY_water, RINDEX_blacksheet, NUMENTRIES_water);
   myST1->AddProperty("SPECULARLOBECONSTANT", PP, SPECULARLOBECONSTANT, NUM);
   myST1->AddProperty("SPECULARSPIKECONSTANT", PP, SPECULARSPIKECONSTANT, NUM);
   myST1->AddProperty("BACKSCATTERCONSTANT", PP, BACKSCATTERCONSTANT, NUM);
   myST1->AddProperty("REFLECTIVITY", ENERGY_water, REFLECTIVITY_blacksheet, NUMENTRIES_water);
   myST1->AddProperty("EFFICIENCY", ENERGY_water, EFFICIENCY_blacksheet, NUMENTRIES_water);
   OpWaterBSSurface->SetMaterialPropertiesTable(myST1);

   BSSkinSurface->SetMaterialPropertiesTable(myST1);

   //Glass to Cathode surface inside PMTs
   G4MaterialPropertiesTable *myST2 = new G4MaterialPropertiesTable();
   myST2->AddProperty("RINDEX", PP, RINDEX_cathode, NUM);
   //   myST2->AddProperty("SPECULARLOBECONSTANT", PP, SPECULARLOBECONSTANT_glasscath, NUM);
   //   myST2->AddProperty("SPECULARSPIKECONSTANT", PP, SPECULARSPIKECONSTANT_glasscath, NUM);
   //   myST2->AddProperty("BACKSCATTERCONSTANT", PP, BACKSCATTERCONSTANT_glasscath, NUM);
   myST2->AddProperty("REFLECTIVITY", PP, REFLECTIVITY_glasscath, NUM);
   myST2->AddProperty("EFFICIENCY", PP, EFFICIENCY_glasscath, NUM);
   //myST2->AddProperty("ABSLENGTH", PP, abslength_paint , NUM);
   OpGlassCathodeSurface->SetMaterialPropertiesTable(myST2);

   //Tyvek - jl145
   G4MaterialPropertiesTable *myST3 = new G4MaterialPropertiesTable();
   myST3->AddProperty("RINDEX", PP, RINDEX_tyvek, NUM);
   myST3->AddProperty("SPECULARLOBECONSTANT", PP, TySPECULARLOBECONSTANT, NUM);
   myST3->AddProperty("SPECULARSPIKECONSTANT", PP, TySPECULARSPIKECONSTANT, NUM);
   myST3->AddProperty("BACKSCATTERCONSTANT", PP, TyBACKSCATTERCONSTANT, NUM);
   myST3->AddProperty("REFLECTIVITY", PP, TyREFLECTIVITY, NUM);
   myST3->AddProperty("EFFICIENCY", PP, EFFICIENCY_blacksheet, NUM);
   //use same efficiency as blacksheet, which is 0
   OpWaterTySurface->SetMaterialPropertiesTable(myST3);


   // Surfaces for Al, Ag and future combinations:
   ReflectorSkinSurface =
     new G4OpticalSurface("ReflectorSurface");
   ReflectorSkinSurface->SetType(dielectric_metal);
   ReflectorSkinSurface->SetModel(unified);  
   ReflectorSkinSurface->SetFinish(polished);

   G4MaterialPropertiesTable *AlPropTable = new G4MaterialPropertiesTable();
   //AlPropTable->AddProperty("RINDEX", ENERGY_Al, RINDEX_Al, 64);  //not necessary for boundaries if kept within their MotherVolumes.
   AlPropTable->AddProperty("REFLECTIVITY", ENERGY_water, REFLECTIVITY_aluminium, NUMENTRIES_water);
   ReflectorSkinSurface->SetMaterialPropertiesTable(AlPropTable);

   OpGelFoamSurface =
     new G4OpticalSurface("GelFoamSurface");

   OpGelFoamSurface->SetType(dielectric_dielectric);
   OpGelFoamSurface->SetModel(unified); 
   OpGelFoamSurface->SetFinish(groundfrontpainted);
   OpGelFoamSurface->SetSigmaAlpha(0.1);

   OpGelFoamSurface->SetMaterialPropertiesTable(myST1); //TF: same as water-blacksheet for now



   //ToDo:
   G4MaterialPropertiesTable *AgPropTable = new G4MaterialPropertiesTable();
   G4MaterialPropertiesTable *AlAg1PropTable = new G4MaterialPropertiesTable();
}
