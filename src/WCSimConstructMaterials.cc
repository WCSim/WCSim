#include <WCSimWLSProperties.hh>
#include "WCSimDetectorConstruction.hh"
#include "WCSimTuningParameters.hh"
#include "WCSimPMTObject.hh"

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
  
  //---BGO
  BGO = nist_man->FindOrBuildMaterial("G4_BGO");

  //---Vacuum

  density     = universe_mean_density;              //from PhysicalConstants.h
  G4double pressure    = 1.e-19*pascal;
  G4double temperature = 0.1*kelvin;
  a = 1.01*g/mole;
  //G4Material* Vacuum =
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

  G4Material* Air1 // with realistic attenuation length, for WCTE mPMT construction
    = new G4Material("Air1",density,2);
  Air1->AddElement(elN, 70.*perCent);
  Air1->AddElement(elO, 30.*perCent);
  
  //---Plastic
  
  density = 0.95*g/cm3;
  G4Material* Plastic
    = new G4Material("Plastic",density,2);
  Plastic->AddElement(elC, 1);
  Plastic->AddElement(elH, 2);

  //---Poron between PMT and matrix in mPMT, for WCTE mPMT construction
  density = 0.2403*g/cm3;
  
  G4Material* poron
    = new G4Material("PolyurethaneFoam",density,4);

  poron->AddElement(elC, 27);
  poron->AddElement(elH, 36);
  poron->AddElement(elN, 2);
  poron->AddElement(elO, 10);

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

  G4Material *SilGel_WCTE // for WCTE mPMT construction
    = new G4Material("SilGel_WCTE",density,1);
  SilGel_WCTE->AddElement(elSi, 1);

  // TF: Add Acrylic from G4 database:
  G4Material* Acrylic 
    = nist_man->FindOrBuildMaterial("G4_PLEXIGLASS");

  //---Absorber: artificial material to absorb all photons, for WCTE mPMT construction
  density = 0.96*g/cm3;
  G4Material *absorberMaterial = new G4Material("customAbsorber", density, 2);
  absorberMaterial->AddElement(elC, 1);
  absorberMaterial->AddElement(elH, 4);


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

  //---PVT - WLS plates eljen EJ-286
  // linear formula [CH2CH(C6H4CH3)]n
  density = 1.023*g/cm3;  // at 20deg
  G4Material* WLS_PVT
      = new G4Material("WLS_PVT",density,2);
  WLS_PVT->AddElement(elC, 9); // PVT
  WLS_PVT->AddElement(elH, 10);


  //** OD WLS cladding properties
  const G4int wls_od_cladding_num = 2;
  G4double wls_od_cladding_ephoton[wls_od_cladding_num] = { 1.*eV, 10*eV };
  double WCODWLSCladdingReflectivity = WCSimTuningParams->GetWCODWLSCladdingReflectivity();
  G4double claddingReflectivity[wls_od_cladding_num] = {WCODWLSCladdingReflectivity, WCODWLSCladdingReflectivity};
  G4double claddingEfficiency[wls_od_cladding_num] = {0., 0.};
  G4MaterialPropertiesTable* claddingPT = new G4MaterialPropertiesTable();
  claddingPT->AddProperty("REFLECTIVITY", wls_od_cladding_ephoton, claddingReflectivity, wls_od_cladding_num);
  claddingPT->AddProperty("EFFICIENCY", wls_od_cladding_ephoton, claddingEfficiency, wls_od_cladding_num);
  WlsOdOpCladdingSurface =
      new G4OpticalSurface("WLSCladdingSurface",unified,polished,dielectric_metal);
  WlsOdOpCladdingSurface->SetMaterialPropertiesTable(claddingPT);

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

  G4Material* GlassWCTE = new G4Material("GlassWCTE",density,4); // for WCTE mPMT construction
  GlassWCTE->AddMaterial(SiO2, 80.6*perCent);
  GlassWCTE->AddMaterial(B2O3, 13.0*perCent);
  GlassWCTE->AddMaterial(Na2O, 4.0*perCent);
  GlassWCTE->AddMaterial(Al2O3, 2.4*perCent);

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

  /*
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
  */

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
    G4cout<<"ABWFF : "<<ABWFF<<G4endl;
    
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
   G4cout << "RAYFF: " << RAYFF << G4endl;

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
   G4cout << "MIEFF: " << MIEFF << G4endl;

   //Values extracted from Skdetsim
   /*
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
   */

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

   //G4double MIE_water_const[3]={0.4,0.,1};// gforward, gbackward, forward backward ratio


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

   // for WCTE mPMT construction
   G4double ENERGY_glasswcte[45]= 
     {4.95937*eV, 4.76862*eV, 4.59201*eV, 4.42801*eV, 4.27532*eV,
      4.13281*eV, 3.99949*eV, 3.87451*eV, 3.7571*eV, 3.64659*eV,
      3.54241*eV, 3.44401*eV, 3.35092*eV, 3.26274*eV, 3.17908*eV,
      3.0996*eV, 3.024*eV, 2.952*eV, 2.88335*eV, 2.81782*eV,
      2.7552*eV, 2.69531*eV, 2.63796*eV, 2.583*eV, 2.53029*eV,
      2.47968*eV, 2.43106*eV, 2.38431*eV, 2.33932*eV, 2.296*eV,
      2.25426*eV, 2.214*eV, 2.17516*eV, 2.13766*eV, 2.10143*eV,
      2.0664*eV, 2.03253*eV, 1.99975*eV, 1.968*eV, 1.93725*eV, 
      1.90745*eV, 1.87855*eV, 1.85051*eV, 1.8233*eV,  1.79687*eV };
   G4double RINDEX_glasswcte[45]=
     {1.578110, 1.571475, 1.565786, 1.560859, 1.556556,
      1.552770, 1.549417, 1.546429, 1.543752, 1.541344,
      1.539166, 1.537190, 1.535390, 1.533745, 1.532236, 
      1.530849, 1.529569, 1.528385, 1.527288, 1.526269,
      1.525320, 1.524433, 1.523605, 1.522829, 1.522100,
      1.521414, 1.520769, 1.520160, 1.519584, 1.519039,
      1.518522, 1.518032, 1.517566, 1.517122, 1.516699,
      1.516295, 1.515909, 1.515539, 1.515186, 1.514846,
      1.514520, 1.514207, 1.513905, 1.513615, 1.513335};
   G4double ABSORPTION_glasswcte[45]=
     {0.000574415*m, 0.000854808*m, 0.00134354*m, 0.00226156*m, 0.00414292*m,
      0.00834524*m, 0.0180342*m, 0.0382308*m, 0.0785805*m, 0.143885*m,
      0.299827*m, 0.629713*m, 1.07439*m, 1.45803*m, 2.26023*m, 
      3.11245*m, 3.75419*m, 3.55892*m, 3.2194*m, 3.11393*m, 
      3.30197*m, 3.55878*m, 3.6993*m, 3.77316*m, 3.88867*m, 
      4.15414*m, 4.63536*m, 5.22806*m, 5.78547*m, 6.16079*m, 
      6.21063*m, 5.92339*m, 5.45867*m, 4.98751*m, 4.64493*m, 
      4.42188*m, 4.27329*m, 4.15408*m, 4.03614*m, 3.95912*m,
      3.97959*m, 4.15417*m, 4.51931*m, 5.031*m, 5.31356*m};
   
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
   /*
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
   */
     
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

  // for WCTE mPMT construction
  const G4int NUMENTRIES_reflector = 501;
  
  G4double ENERGY_reflector[NUMENTRIES_reflector] = 
    { 1.7712*eV, 1.77374*eV, 1.77628*eV, 1.77883*eV, 1.78138*eV, 1.78395*eV,
      1.78652*eV, 1.78909*eV, 1.79168*eV, 1.79427*eV, 1.79687*eV, 1.79948*eV,
      1.8021*eV, 1.80472*eV, 1.80735*eV, 1.80999*eV, 1.81263*eV, 1.81529*eV,
      1.81795*eV, 1.82062*eV, 1.8233*eV, 1.82598*eV, 1.82868*eV, 1.83138*eV,
      1.83409*eV, 1.8368*eV, 1.83953*eV, 1.84226*eV, 1.845*eV, 1.84775*eV,
      1.85051*eV, 1.85328*eV, 1.85605*eV, 1.85883*eV, 1.86162*eV, 1.86442*eV,
      1.86723*eV, 1.87005*eV, 1.87287*eV, 1.87571*eV, 1.87855*eV, 1.8814*eV,
      1.88426*eV, 1.88713*eV, 1.89*eV, 1.89289*eV, 1.89578*eV, 1.89869*eV,
      1.9016*eV, 1.90452*eV, 1.90745*eV, 1.91039*eV, 1.91334*eV, 1.91629*eV,
      1.91926*eV, 1.92224*eV, 1.92522*eV, 1.92821*eV, 1.93122*eV, 1.93423*eV,
      1.93725*eV, 1.94028*eV, 1.94333*eV, 1.94638*eV, 1.94944*eV, 1.95251*eV,
      1.95559*eV, 1.95868*eV, 1.96178*eV, 1.96488*eV, 1.968*eV, 1.97113*eV,
      1.97427*eV, 1.97742*eV, 1.98058*eV, 1.98375*eV, 1.98693*eV, 1.99012*eV,
      1.99332*eV, 1.99652*eV, 1.99975*eV, 2.00298*eV, 2.00622*eV, 2.00947*eV,
      2.01273*eV, 2.016*eV, 2.01929*eV, 2.02258*eV, 2.02589*eV, 2.0292*eV,
      2.03253*eV, 2.03587*eV, 2.03921*eV, 2.04257*eV, 2.04594*eV, 2.04933*eV,
      2.05272*eV, 2.05612*eV, 2.05954*eV, 2.06297*eV, 2.0664*eV, 2.06985*eV,
      2.07331*eV, 2.07679*eV, 2.08027*eV, 2.08377*eV, 2.08728*eV, 2.0908*eV,
      2.09433*eV, 2.09787*eV, 2.10143*eV, 2.10499*eV, 2.10857*eV, 2.11217*eV,
      2.11577*eV, 2.11939*eV, 2.12302*eV, 2.12666*eV, 2.13031*eV, 2.13398*eV,
      2.13766*eV, 2.14135*eV, 2.14506*eV, 2.14877*eV, 2.1525*eV, 2.15625*eV,
      2.16*eV, 2.16377*eV, 2.16756*eV, 2.17135*eV, 2.17516*eV, 2.17898*eV,
      2.18282*eV, 2.18667*eV, 2.19053*eV, 2.19441*eV, 2.1983*eV, 2.20221*eV,
      2.20612*eV, 2.21006*eV, 2.214*eV, 2.21796*eV, 2.22194*eV, 2.22593*eV,
      2.22993*eV, 2.23395*eV, 2.23798*eV, 2.24203*eV, 2.24609*eV, 2.25017*eV,
      2.25426*eV, 2.25836*eV, 2.26249*eV, 2.26662*eV, 2.27077*eV, 2.27494*eV,
      2.27912*eV, 2.28332*eV, 2.28753*eV, 2.29176*eV, 2.296*eV, 2.30026*eV,
      2.30454*eV, 2.30883*eV, 2.31314*eV, 2.31746*eV, 2.3218*eV, 2.32616*eV,
      2.33053*eV, 2.33492*eV, 2.33932*eV, 2.34375*eV, 2.34819*eV,
      2.35264*eV, 2.35711*eV, 2.3616*eV, 2.36611*eV, 2.37063*eV, 2.37518*eV,
      2.37974*eV, 2.38431*eV, 2.38891*eV, 2.39352*eV, 2.39815*eV, 2.40279*eV,
      2.40746*eV, 2.41214*eV, 2.41685*eV, 2.42157*eV, 2.42631*eV, 2.43106*eV,
      2.43584*eV, 2.44063*eV, 2.44545*eV, 2.45028*eV, 2.45513*eV, 2.46*eV,
      2.46489*eV, 2.4698*eV, 2.47473*eV, 2.47968*eV, 2.48465*eV, 2.48964*eV,
      2.49465*eV, 2.49968*eV, 2.50473*eV, 2.5098*eV, 2.51489*eV, 2.52*eV,
      2.52514*eV, 2.53029*eV, 2.53546*eV, 2.54066*eV, 2.54588*eV, 2.55112*eV,
      2.55638*eV, 2.56166*eV, 2.56696*eV, 2.57229*eV, 2.57763*eV, 2.583*eV,
      2.5884*eV, 2.59381*eV, 2.59925*eV, 2.60471*eV, 2.61019*eV, 2.6157*eV,
      2.62123*eV, 2.62678*eV, 2.63236*eV, 2.63796*eV, 2.64359*eV, 2.64924*eV,
      2.65491*eV, 2.66061*eV, 2.66633*eV, 2.67207*eV, 2.67784*eV, 2.68364*eV,
      2.68946*eV, 2.69531*eV, 2.70118*eV, 2.70708*eV, 2.713*eV, 2.71895*eV,
      2.72493*eV, 2.73093*eV, 2.73696*eV, 2.74301*eV, 2.7491*eV, 2.7552*eV,
      2.76134*eV, 2.7675*eV, 2.7737*eV, 2.77991*eV, 2.78616*eV, 2.79244*eV,
      2.79874*eV, 2.80507*eV, 2.81143*eV, 2.81782*eV, 2.82424*eV, 2.83069*eV,
      2.83717*eV, 2.84367*eV, 2.85021*eV, 2.85678*eV, 2.86338*eV, 2.87*eV,
      2.87666*eV, 2.88335*eV, 2.89007*eV, 2.89683*eV, 2.90361*eV, 2.91043*eV,
      2.91728*eV, 2.92416*eV, 2.93107*eV, 2.93801*eV, 2.94499*eV, 2.952*eV,
      2.95905*eV, 2.96613*eV, 2.97324*eV, 2.98039*eV, 2.98757*eV, 2.99479*eV,
      3.00204*eV, 3.00933*eV, 3.01665*eV, 3.024*eV, 3.0314*eV, 3.03883*eV,
      3.04629*eV, 3.0538*eV, 3.06134*eV, 3.06892*eV, 3.07653*eV, 3.08418*eV,
      3.09188*eV, 3.0996*eV, 3.10737*eV, 3.11518*eV, 3.12303*eV, 3.13091*eV,
      3.13884*eV, 3.14681*eV, 3.15481*eV, 3.16286*eV, 3.17095*eV, 3.17908*eV,
      3.18725*eV, 3.19547*eV, 3.20373*eV, 3.21203*eV, 3.22037*eV, 3.22876*eV,
      3.23719*eV, 3.24566*eV, 3.25418*eV, 3.26274*eV, 3.27135*eV, 3.28001*eV,
      3.28871*eV, 3.29745*eV, 3.30625*eV, 3.31509*eV, 3.32397*eV, 3.33291*eV,
      3.34189*eV, 3.35092*eV, 3.36001*eV, 3.36914*eV, 3.37832*eV, 3.38755*eV,
      3.39683*eV, 3.40616*eV, 3.41554*eV, 3.42498*eV, 3.43447*eV, 3.44401*eV,
      3.4536*eV, 3.46325*eV, 3.47295*eV, 3.4827*eV, 3.49251*eV, 3.50238*eV,
      3.5123*eV, 3.52228*eV, 3.53231*eV, 3.54241*eV, 3.55256*eV, 3.56276*eV,
      3.57303*eV, 3.58336*eV, 3.59374*eV, 3.60419*eV, 3.6147*eV, 3.62527*eV,
      3.6359*eV, 3.64659*eV, 3.65735*eV, 3.66817*eV, 3.67906*eV, 3.69001*eV,
      3.70102*eV, 3.7121*eV, 3.72325*eV, 3.73446*eV, 3.74575*eV, 3.7571*eV,
      3.76852*eV, 3.78001*eV, 3.79157*eV, 3.8032*eV, 3.8149*eV, 3.82667*eV,
      3.83852*eV, 3.85044*eV, 3.86244*eV, 3.87451*eV, 3.88665*eV, 3.89887*eV,
      3.91117*eV, 3.92355*eV, 3.93601*eV, 3.94854*eV, 3.96116*eV, 3.97385*eV,
      3.98663*eV, 3.99949*eV, 4.01243*eV, 4.02546*eV, 4.03857*eV, 4.05177*eV,
      4.06506*eV, 4.07843*eV, 4.09189*eV, 4.10544*eV, 4.11908*eV, 4.13281*eV,
      4.14663*eV, 4.16054*eV, 4.17455*eV, 4.18866*eV, 4.20285*eV, 4.21715*eV,
      4.23154*eV, 4.24603*eV, 4.26063*eV, 4.27532*eV, 4.29011*eV, 4.30501*eV,
      4.32001*eV, 4.33511*eV, 4.35032*eV, 4.36564*eV, 4.38107*eV, 4.3966*eV,
      4.41225*eV, 4.42801*eV, 4.44388*eV, 4.45986*eV, 4.47596*eV, 4.49218*eV,
      4.50852*eV, 4.52497*eV, 4.54155*eV, 4.55824*eV, 4.57506*eV, 4.59201*eV,
      4.60908*eV, 4.62628*eV, 4.6436*eV, 4.66106*eV, 4.67865*eV, 4.69637*eV,
      4.71423*eV, 4.73222*eV, 4.75035*eV, 4.76862*eV, 4.78703*eV, 4.80559*eV,
      4.82429*eV, 4.84313*eV, 4.86213*eV, 4.88127*eV, 4.90056*eV, 4.92001*eV,
      4.93961*eV, 4.95937*eV, 4.97929*eV, 4.99936*eV, 5.0196*eV, 5.04001*eV,
      5.06058*eV, 5.08132*eV, 5.10223*eV, 5.12331*eV, 5.14457*eV, 5.16601*eV,
      5.18762*eV, 5.20942*eV, 5.2314*eV, 5.25357*eV, 5.27592*eV, 5.29847*eV,
      5.32121*eV, 5.34415*eV, 5.36728*eV, 5.39062*eV, 5.41416*eV, 5.4379*eV,
      5.46186*eV, 5.48603*eV, 5.51041*eV, 5.53501*eV, 5.55983*eV, 5.58487*eV,
      5.61014*eV, 5.63565*eV, 5.66138*eV, 5.68735*eV, 5.71356*eV, 5.74001*eV,
      5.76671*eV, 5.79365*eV, 5.82085*eV, 5.84831*eV, 5.87603*eV, 5.90401*eV,
      5.93226*eV, 5.96078*eV, 5.98957*eV, 6.01865*eV, 6.04801*eV, 6.07766*eV,
      6.1076*eV, 6.13783*eV, 6.16837*eV, 6.19921*eV };

  G4double REFLECTIVITY_reflector[NUMENTRIES_reflector] = 
  { .91867018, .91878821, .91909624, .91933169, .91975042, .91994669,
    .92023845, .9205233, .9204163, .92116707, .92116159, .92172474, .92190017,
    .9218805, .92234936, .92254988, .9228962, .92306398, .92334575, .92357688,
    .92381986, .92396137, .92430164, .92448784, .92465907, .92492628,
    .92514583, .92564694, .92586161, .92585568, .92609252, .92623107,
    .92653067, .92670681, .9269944, .9271874, .9274472, .92735985, .92774786,
    .92797425, .9277678, .9283881, .92882547, .92886619, .92880202, .92926185,
    .9291282, .92941198, .9297765, .92971139, .93010272, .93005497, .93035958,
    .93064111, .93085596, .93108483, .931252, .93113579, .93144708, .93159892,
    .93169539, .93167694, .93210855, .93221407, .93229929, .93217626,
    .93261842, .93267996, .9328614, .93305264, .93279379, .93313851, .93334877,
    .93357556, .93384519, .93398249, .93397436, .9341664, .93426486, .93431823,
    .93442491, .93463716, .93500783, .93516479, .93504207, .93518962,
    .93530918, .93544686, .93560372, .93582261, .93591123, .93591639,
    .93596435, .93605721, .93621836, .93639598, .93625838, .9365027, .93679588,
    .93701544, .93727586, .93736008, .93731122, .93730506, .93732266,
    .93737014, .93732351, .93745206, .93774816, .93792166, .9381091, .93814387,
    .93829906, .93835502, .93849515, .93874573, .9387148, .93837158, .93857773,
    .9388873, .93908168, .93917556, .93931774, .93956124, .93966123, .93963233,
    .93975599, .93985593, .93976342, .93978551, .94003286, .9402512, .94037606,
    .94037748, .94046234, .94061184, .94085374, .94105208, .94130384,
    .94116635, .94085321, .94089851, .9408085, .94080903, .94106384, .94120388,
    .94128558, .94128922, .94135115, .941421, .94151582, .94165153, .94178154,
    .94191268, .94201176, .94197218, .94199962, .94206345, .94217231,
    .94216899, .94229288, .94235705, .94232253, .94245712, .94255995,
    .94256122, .94258842, .9426191, .94280816, .9429058, .94279921, .9427562,
    .94297274, .94310444, .94316056, .94319721, .94326782, .94322525,
    .94329357, .94328025, .94330324, .94323669, .94327122, .94343237,
    .94336868, .94333659, .94338033, .94356735, .94368169, .94355407,
    .94343574, .94340561, .94336776, .94315306, .94281768, .94247779,
    .94216414, .94205492, .94147018, .94064085, .93994456, .93916984,
    .93853796, .93763506, .93648232, .93582235, .93517931, .93426367,
    .93344305, .93276234, .93235772, .93186943, .93167268, .93151248,
    .93126298, .93117566, .93152638, .93196265, .93244511, .93297256,
    .93351183, .93386929, .93437783, .9348046, .93489035, .93494792,
    .93507843, .93521117, .9350747, .93552294, .93569038, .93554846,
    .9354536, .93516454, .93517806, .9350129, .93488663, .9341786, .93334727,
    .9331322, .93240218, .93156117, .9303463, .92941609, .92819197, .92743487,
    .92660401, .92548155, .92433372, .92295113, .9223734, .92134051, .92041279,
    .91948048, .91850696, .91834416, .9177905, .91734713, .91697977, .9168431,
    .91645102, .91564356, .91502549, .91471914, .91375934, .91319727, .9125141,
    .91194892, .91117668, .91025524, .90978327, .9090553, .90788165, .90687478,
    .90599473, .90530206, .90407009, .90329717, .90225505, .90110879,
    .90013234, .89890837, .89790649, .89661338, .89520829, .89373676,
    .8926066, .89094177, .88966278, .88799792, .88655118, .8849559, .88305646,
    .88077914, .87912389, .8769281, .87467818, .87275388, .87031673, .86773113,
    .86518416, .86211943, .85960268, .85638584, .85344267, .85031389,
    .84651235, .84253471, .83843647, .83438015, .8299535, .8252885, .82069422,
    .81525201, .80920414, .80305407, .79638057, .78943532, .78211256,
    .77434004, .76559918, .75566542, .73961647, .72914792, .71985872,
    .71870066, .70170831, .67919187, .66317085, .64606509, .62820679,
    .60919403, .58910818, .56816265, .54628183, .52432657, .50238826,
    .48064867, .46011777, .44105821, .42452535, .41061547, .39954065,
    .3917797, .38732376, .38664667, .38903143, .39417775, .40172204,
    .41096687, .42153995, .43298425, .444908, .45674237, .46857062, .47960048,
    .49011047, .50008499, .50912678, .51714986, .52463473, .53124773,
    .53694391, .54214078, .54641626, .54989054, .55281175, .55503411,
    .55669049, .55809089, .55879853, .55877649, .55851489, .55789731,
    .55664089, .55494965, .55270052, .55014417, .5472109, .54343811,
    .5396432, .53571113, .53331219, .53202828, .52733801, .52234025, .5171219,
    .51148939, .50605254, .50016304, .49404896, .48785501, .48143283,
    .47467895, .46746983, .46018764, .45290908, .44542634, .43792217, .4304205,
    .42264221, .41504135, .4076822, .40020852, .39292485, .38603477, .37930307,
    .37291234, .36670246, .36064879, .35528691, .3501708, .34529849, .34082121,
    .33655095, .33245391, .32863984, .32495056, .32138775, .31821843,
    .31533121, .31223797, .309226, .30649842, .3038037, .30125075, .29874417,
    .29637182, .29410163, .29184399, .28967887, .28748432, .28537412,
    .28336286, .28142074, .27952112, .27763106, .27554904, .27355905,
    .27163331, .26961636, .26755006, .26539136, .26322431, .26095423,
    .25867645, .25637042, .25402191, .25155426, .24900057, .24639498,
    .24353394, .24069377, .2378243, .23489127, .23185162, .22876163, .22581776,
    .22300379, .22014197, .21738425, .21477526, .21218674, .20982489,
    .20741723, .20509995, .2028618, .20063595, .19845714, .19625256, .19407225,
    .19188555, .18965879, .18715538, .18485834, .18247306, .17989033,
    .17730006, .1747415, .17203123, .1693376, .166481, .1636511, .16081017,
    .1578858, .15510825, .15234954, .14960043, .14689829, .1443013, .14181921,
    .13937926, .13699244, .13469614, .13258739, .13064613, .12895495,
    .12757626, .12645281, .12563377, .12518963};

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

    G4double RINDEX_SilGel_WCTE[87] = // for WCTE mPMT construction
       {1.608013009, 1.591127095, 1.575602811, 1.561337877, 1.548237552,
				1.536214093, 1.52518625, 1.515078789, 1.505822046, 1.497351509,
				1.489607425, 1.482534437, 1.476081242, 1.470200274, 1.464847411,
				1.459981699, 1.455565103, 1.451562271, 1.447940318, 1.444668631,
				1.441718687, 1.439063886, 1.436679401, 1.434542037, 1.432630111,
				1.430923337, 1.429402722, 1.428050477, 1.426849937, 1.425785482,
				1.424842478, 1.424007219, 1.423266872, 1.422609439, 1.422023711,
				1.42149924, 1.421026309, 1.420595902, 1.420199685, 1.419829987,
				1.419479779, 1.419142664, 1.418812857, 1.41848518, 1.41815504,
				1.417818427, 1.417471897, 1.417112559, 1.416738066, 1.416346602,
				1.415936863, 1.415508046, 1.415059829, 1.414592355, 1.41410621,
				1.413602403, 1.413082337, 1.412547791, 1.412000885, 1.411444054,
				1.410880015, 1.410311734, 1.40974239, 1.409175337, 1.408614066,
				1.408062163, 1.40752327, 1.407001038, 1.406499087, 1.406020956,
				1.405570065, 1.405149661, 1.40476278, 1.404412198, 1.404100388,
				1.403829476, 1.403601203, 1.403416881, 1.40327736, 1.40318299,
				1.40318299, 1.403133596, 1.403128446, 1.403166233, 1.403245055,
				1.403362408, 1.403515176};

    // From Lew Classen (Erlangen) with PHOTON_NRG_MIN = .1*eV, and MAX = 7*eV
    G4double ENERGY_SilGel[18] = { .1*eV,
				   2.034*eV,2.13*eV,2.18*eV,2.22*eV,2.25*eV,2.3*eV,
				   2.34*eV, 2.43*eV, 2.53*eV, 2.67*eV, 2.79*eV,
				   3.1*eV, 3.28*eV, 3.56*eV, 3.77*eV,4.136*eV, 7.*eV};
    G4double ABSORPTION_SilGel[18] = { 0*m,
				       .975*m, .975*m, .966*m, .935*m, .890*m, .898*m,
				       .876*m, .844*m, .786*m, .727*m,  .674*m,
				       .566*m, .485*m, .360*m, .220*m, .220*m, 0*m };

    G4double ENERGY_SilGel_WCTE[87] = // for WCTE mPMT construction
       {4.678776674*eV, 4.592027283*eV, 4.508478709*eV, 4.427958491*eV, 4.35030587*eV,
				4.275369924*eV, 4.202452758*eV, 4.13256199*eV, 4.064996939*eV, 3.999137915*eV,
				3.935906953*eV, 3.87420652*eV, 3.814907862*eV, 3.756987416*eV, 3.700835849*eV,
				3.646372728*eV, 3.593524966*eV, 3.542221472*eV, 3.492396167*eV, 3.443986692*eV,
				3.396571472*eV, 3.350829881*eV, 3.305993162*eV, 3.262706518*eV, 3.220245191*eV,
				3.178906699*eV, 3.138646881*eV, 3.099424803*eV, 3.061201121*eV, 3.023938677*eV,
				2.987602577*eV, 2.95188653*eV, 2.917309072*eV, 2.883302061*eV, 2.850107745*eV,
				2.817696981*eV, 2.786043724*eV, 2.7551212*eV, 2.72490498*eV, 2.695146764*eV,
				2.666278206*eV, 2.637833975*eV, 2.610016557*eV, 2.583011752*eV, 2.556384369*eV,
				2.53013002*eV, 2.504627471*eV, 2.479660085*eV, 2.455025471*eV, 2.430901116*eV,
				2.407447973*eV, 2.384294297*eV, 2.361606176*eV, 2.339203259*eV, 2.317409616*eV,
				2.295881811*eV, 2.274931174*eV, 2.254228848*eV, 2.233923745*eV, 2.213856184*eV,
				2.194315186*eV, 2.175138494*eV, 2.156177117*eV, 2.137566187*eV, 2.119296781*eV,
				2.10135941*eV, 2.083745787*eV, 2.06631904*eV, 2.049203275*eV, 2.032514943*eV,
				2.01587474*eV, 1.999646316*eV, 1.983698791*eV, 1.967909817*eV, 1.952391965*eV,
				1.937138174*eV, 1.92214235*eV, 1.90739769*eV, 1.892792856*eV, 1.878535228*eV,
				1.864408987*eV, 1.850413879*eV, 1.836747218*eV, 1.823203874*eV, 1.809879596*eV,
				1.796768999*eV, 1.783867346*eV};


  G4double ABSORPTION_SilGel_WCTE[87] = 
   {0.038149438*m, 0.042804385*m, 0.052016858*m, 0.065315823*m, 0.082271384*m,
    0.102492373*m, 0.125624001*m, 0.151345579*m, 0.179368306*m, 0.209433124*m,
    0.241308636*m, 0.274789092*m, 0.309692442*m, 0.345858458*m, 0.383146918*m,
    0.421435858*m, 0.460619889*m, 0.500608586*m, 0.541324938*m, 0.582703865*m,
    0.624690804*m, 0.667240357*m, 0.710315014*m, 0.753883932*m, 0.797921786*m,
    0.842407687*m, 0.887324167*m, 0.932656224*m, 0.978390441*m, 1.024514168*m,
    1.071014774*m, 1.117878956*m, 1.165092126*m, 1.212637859*m, 1.260497406*m,
    1.308649275*m, 1.357068882*m, 1.405728262*m, 1.45459585*m, 1.503636328*m,
    1.552810543*m, 1.602075479*m, 1.65138431*m, 1.700686509*m, 1.74992803*m,
    1.799051552*m, 1.847996793*m, 1.896700886*m, 1.945098826*m, 1.993123983*m,
    2.040708675*m, 2.08778482*m, 2.134284638*m, 2.180141438*m, 2.225290453*m,
    2.269669756*m, 2.313221236*m, 2.355891637*m, 2.397633674*m, 2.438407204*m,
    2.478180472*m, 2.516931417*m, 2.55464905*m, 2.591334893*m, 2.627004493*m,
    2.661688988*m, 2.695436756*m, 2.72831512*m, 2.760412122*m, 2.791838362*m,
    2.822728908*m, 2.853245269*m, 2.883577433*m, 2.913945973*m, 2.944604226*m,
    2.975840522*m, 3.0079805*m, 3.041389473*m, 3.076474872*m, 3.113688745*m,
    3.153530334*m, 3.196548711*m, 3.243345483*m, 3.29457756*m, 3.350959998*m,
    3.413268897*m, 3.482344375*m};

  G4double REFLECTIVITY_poron[NUMENTRIES_water] =
    { .01, .01, .01, .01, .01, .01,
      .01, .01, .01, .01, .01, .01,
      .01, .01, .01, .01, .01, .01,
      .01, .01, .01, .01, .01, .01,
      .01, .01, .01, .01, .01, .01,
      .01, .01, .01, .01, .01, .01,
      .01, .01, .01, .01, .01, .01,
      .01, .01, .01, .01, .01, .01,
      .01, .01, .01, .01, .01, .01,
      .01, .01, .01, .01, .01, .01};

  G4double REFLECTIVITY_absorber[NUMENTRIES_water] =
  { .0, .0, .0, .0, .0, .0,
    .0, .0, .0, .0, .0, .0,
    .0, .0, .0, .0, .0, .0,
    .0, .0, .0, .0, .0, .0,
    .0, .0, .0, .0, .0, .0,
    .0, .0, .0, .0, .0, .0,
    .0, .0, .0, .0, .0, .0,
    .0, .0, .0, .0, .0, .0,
    .0, .0, .0, .0, .0, .0,
    .0, .0, .0, .0, .0, .0};

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

   PoronSkinSurface = new G4OpticalSurface("PoronSurface"); // for WCTE mPMT construction
   PoronSkinSurface->SetType(dielectric_dielectric);
   PoronSkinSurface->SetModel(unified); 
   PoronSkinSurface->SetFinish(groundfrontpainted);
   PoronSkinSurface->SetSigmaAlpha(0.1);
   G4MaterialPropertiesTable *poron_foam = new G4MaterialPropertiesTable();
   poron_foam->AddProperty("REFLECTIVITY", ENERGY_water, REFLECTIVITY_poron, NUMENTRIES_water);
   PoronSkinSurface->SetMaterialPropertiesTable(poron_foam);

   AbsorberSkinSurface = new G4OpticalSurface("AbsorberSurface"); // for WCTE mPMT construction
   AbsorberSkinSurface->SetType(dielectric_metal);
   AbsorberSkinSurface->SetModel(unified);
   AbsorberSkinSurface->SetFinish(polished);
   G4MaterialPropertiesTable *absMaterial = new G4MaterialPropertiesTable();
   absMaterial->AddProperty("REFLECTIVITY", ENERGY_water, REFLECTIVITY_absorber, NUMENTRIES_water);
   AbsorberSkinSurface->SetMaterialPropertiesTable(absMaterial);

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

   // Set to 1 to trigger WCSimOpBoundaryProcess::InvokeSD()
   G4double EFFICIENCY_glasscath[NUM] =
     { 1.0, 1.0 }; 

   // Coated surface properties for new photocathode physics (see WCSimOpBoundaryProcess.cc)
   // Allows frustrated transmission through photocathode film or not, only meaningful for Model 1
   // However the photocathode film generally has a larger refractive than glass 
   // so total internal reflection does not occur on the glass-film interface but on the glass-air interface 
   G4int COATEDFRUSTRATEDTRANSMISSION_glasscath = 1;

  ///////////////////////
  // ##### TYVEK ##### //
  ///////////////////////

  OpWaterTySurfaceInWallTop =
      new G4OpticalSurface("WaterTyCellSurfaceInWallTop");

  OpWaterTySurfaceInWallTop->SetType(dielectric_metal); // Only absorption and reflection
  OpWaterTySurfaceInWallTop->SetModel(unified);
  OpWaterTySurfaceInWallTop->SetFinish(ground); // ground surface with tyvek
  OpWaterTySurfaceInWallTop->SetSigmaAlpha(0.2);

  OpWaterTySurfaceInWallBarrel =
      new G4OpticalSurface("WaterTyCellSurfaceInWallBarrel");

  OpWaterTySurfaceInWallBarrel->SetType(dielectric_metal); // Only absorption and reflection
  OpWaterTySurfaceInWallBarrel->SetModel(unified);
  OpWaterTySurfaceInWallBarrel->SetFinish(ground); // ground surface with tyvek
  OpWaterTySurfaceInWallBarrel->SetSigmaAlpha(0.2);

  OpWaterTySurfaceInWallBottom =
      new G4OpticalSurface("WaterTyCellSurfaceInWallBottom");

  OpWaterTySurfaceInWallBottom->SetType(dielectric_metal); // Only absorption and reflection
  OpWaterTySurfaceInWallBottom->SetModel(unified);
  OpWaterTySurfaceInWallBottom->SetFinish(ground); // ground surface with tyvek
  OpWaterTySurfaceInWallBottom->SetSigmaAlpha(0.2);

  OpWaterTySurfaceOutWallTop =
    new G4OpticalSurface("WaterTyCellSurfaceOutWallTop");
  
  OpWaterTySurfaceOutWallTop->SetType(dielectric_metal); // Only absorption and reflection
  OpWaterTySurfaceOutWallTop->SetModel(unified);
  OpWaterTySurfaceOutWallTop->SetFinish(ground); // ground surface with tyvek
  OpWaterTySurfaceOutWallTop->SetSigmaAlpha(0.2);

  OpWaterTySurfaceOutWallBarrel =
    new G4OpticalSurface("WaterTyCellSurfaceOutWallBarrel");
  
  OpWaterTySurfaceOutWallBarrel->SetType(dielectric_metal); // Only absorption and reflection
  OpWaterTySurfaceOutWallBarrel->SetModel(unified);
  OpWaterTySurfaceOutWallBarrel->SetFinish(ground); // ground surface with tyvek
  OpWaterTySurfaceOutWallBarrel->SetSigmaAlpha(0.2);

  OpWaterTySurfaceOutWallBottom =
    new G4OpticalSurface("WaterTyCellSurfaceOutWallBottom");
  
  OpWaterTySurfaceOutWallBottom->SetType(dielectric_metal); // Only absorption and reflection
  OpWaterTySurfaceOutWallBottom->SetModel(unified);
  OpWaterTySurfaceOutWallBottom->SetFinish(ground); // ground surface with tyvek
  OpWaterTySurfaceOutWallBottom->SetSigmaAlpha(0.2);


  G4double RINDEX_tyvek[NUM] =
      { 1.5, 1.5 }; // polyethylene permittivity is ~2.25
  G4double TySPECULARLOBECONSTANT[NUM] =
      { 0.75, 0.75 }; // crudely estimated from A. Chavarria's thesis
  G4double TySPECULARSPIKECONSTANT[NUM] =
      { 0.0, 0.0 };
  G4double TyBACKSCATTERCONSTANT[NUM] =
      { 0.0, 0.0 };
  // Lambertian prob is therefore 0.25

#define NUMENTRIES_TY 36 // Number of bins of wavelength to be used for the Tyvek reflectivity

  G4double PP_TyREFLECTIVITY[NUMENTRIES_TY] = //Tyvek reflectivity wavelength bins
      { 2.06642*eV,
        2.10144*eV, 2.13768*eV, 2.17518*eV, 2.21402*eV, 2.25428*eV,
        2.29602*eV, 2.33934*eV, 2.38433*eV, 2.43108*eV, 2.4797*eV,
        2.53031*eV, 2.58302*eV, 2.63798*eV, 2.69533*eV, 2.75523*eV,
        2.81784*eV, 2.88338*eV, 2.95203*eV, 3.02403*eV, 3.09963*eV,
        3.17911*eV, 3.26277*eV, 3.35095*eV, 3.44403*eV, 3.54243*eV,
        3.64662*eV, 3.75713*eV, 3.87454*eV, 3.99952*eV, 4.13284*eV,
        4.27535*eV, 4.42804*eV, 4.6*eV, 4.8*eV, 5.0*eV};

  double WCODTyvekReflectivityInWallTop = WCSimTuningParams->GetWCODTyvekReflectivityInWallTop();
  G4double OD_tyvek_reflectivity_scaling_factor_inwalltop = WCODTyvekReflectivityInWallTop/0.95;

  double WCODTyvekReflectivityInWallBarrel = WCSimTuningParams->GetWCODTyvekReflectivityInWallBarrel();
  G4double OD_tyvek_reflectivity_scaling_factor_inwallbarrel = WCODTyvekReflectivityInWallBarrel/0.95;

  double WCODTyvekReflectivityInWallBottom = WCSimTuningParams->GetWCODTyvekReflectivityInWallBottom();
  G4double OD_tyvek_reflectivity_scaling_factor_inwallbottom = WCODTyvekReflectivityInWallBottom/0.95;

  double WCODTyvekReflectivityOutWallTop = WCSimTuningParams->GetWCODTyvekReflectivityOutWallTop();
  G4double OD_tyvek_reflectivity_scaling_factor_outwalltop = WCODTyvekReflectivityOutWallTop/0.87;

  double WCODTyvekReflectivityOutWallBarrel = WCSimTuningParams->GetWCODTyvekReflectivityOutWallBarrel();
  G4double OD_tyvek_reflectivity_scaling_factor_outwallbarrel = WCODTyvekReflectivityOutWallBarrel/0.87;

  double WCODTyvekReflectivityOutWallBottom = WCSimTuningParams->GetWCODTyvekReflectivityOutWallBottom();
  G4double OD_tyvek_reflectivity_scaling_factor_outwallbottom = WCODTyvekReflectivityOutWallBottom/0.87;

  G4double TyREFLECTIVITY_INWALLTOP[NUMENTRIES_TY] = // Tyvek refelctivity
    { 0.94, // 600 nm
      0.941, 0.942, 0.943, 0.944, 0.945, // 590-550
      0.946, 0.947, 0.948, 0.949, 0.95, // 540-500
      0.95, 0.95, 0.95, 0.95, 0.95, // 490-450
      0.95, 0.95, 0.95, 0.95, 0.95, // 440-400
      0.948, 0.946, 0.944, 0.942, 0.94, // 390-350
      0.93, 0.92, 0.91, 0.89, 0.86, // 340-300
      0.80, 0.76, 0.70, 0.65, 0.55}; // 290-250

  G4double TyREFLECTIVITY_INWALLBARREL[NUMENTRIES_TY];
  for(int i=0; i<NUMENTRIES_TY; i++)
    TyREFLECTIVITY_INWALLBARREL[i] = TyREFLECTIVITY_INWALLTOP[i];

  G4double TyREFLECTIVITY_INWALLBOTTOM[NUMENTRIES_TY];
  for(int i=0; i<NUMENTRIES_TY; i++)
    TyREFLECTIVITY_INWALLBOTTOM[i] = TyREFLECTIVITY_INWALLTOP[i];

  for(int i=0; i<NUMENTRIES_TY; i++)
    TyREFLECTIVITY_INWALLTOP[i] *= OD_tyvek_reflectivity_scaling_factor_inwalltop;

  for(int i=0; i<NUMENTRIES_TY; i++)
    TyREFLECTIVITY_INWALLBARREL[i] *= OD_tyvek_reflectivity_scaling_factor_inwallbarrel;

  for(int i=0; i<NUMENTRIES_TY; i++)
    TyREFLECTIVITY_INWALLBOTTOM[i] *= OD_tyvek_reflectivity_scaling_factor_inwallbottom;

  G4double TyREFLECTIVITY_OUTWALLTOP[NUMENTRIES_TY] = // Tyvek refelctivity
  { 0.86, // 600 nm
      0.86, 0.86, 0.86, 0.86, 0.861, // 590-550
      0.862, 0.863, 0.864, 0.865, 0.866, // 540-500
      0.867, 0.868, 0.869, 0.87, 0.87, // 490-450
      0.868, 0.866, 0.864, 0.862, 0.86, // 440-400
      0.85, 0.84, 0.83, 0.81, 0.79, // 390-350
      0.75, 0.72, 0.70, 0.68, 0.67, // 340-300
      0.65, 0.62, 0.61, 0.59, 0.56}; // 290-250

  G4double TyREFLECTIVITY_OUTWALLBARREL[NUMENTRIES_TY];
  for(int i=0; i<NUMENTRIES_TY; i++)
    TyREFLECTIVITY_OUTWALLBARREL[i] = TyREFLECTIVITY_OUTWALLTOP[i];

  G4double TyREFLECTIVITY_OUTWALLBOTTOM[NUMENTRIES_TY];
  for(int i=0; i<NUMENTRIES_TY; i++)
    TyREFLECTIVITY_OUTWALLBOTTOM[i] = TyREFLECTIVITY_OUTWALLTOP[i];

  for(int i=0; i<NUMENTRIES_TY; i++)
    TyREFLECTIVITY_OUTWALLTOP[i] *= OD_tyvek_reflectivity_scaling_factor_outwalltop;

  for(int i=0; i<NUMENTRIES_TY; i++)
    TyREFLECTIVITY_OUTWALLBARREL[i] *= OD_tyvek_reflectivity_scaling_factor_outwallbarrel;

  for(int i=0; i<NUMENTRIES_TY; i++)
    TyREFLECTIVITY_OUTWALLBOTTOM[i] *= OD_tyvek_reflectivity_scaling_factor_outwallbottom;

  G4MaterialPropertiesTable *MPT_Tyvek = new G4MaterialPropertiesTable();
  // MPT_Tyvek->AddProperty("RINDEX", PP, RINDEX_tyvek, NUM);
  // MPT_Tyvek->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet, NUMENTRIES_water);
  Tyvek->SetMaterialPropertiesTable(MPT_Tyvek);

  G4MaterialPropertiesTable *MPTWater_TyInWallTop = new G4MaterialPropertiesTable();
  MPTWater_TyInWallTop->AddProperty("RINDEX", PP, RINDEX_tyvek, NUM);
  MPTWater_TyInWallTop->AddProperty("SPECULARLOBECONSTANT", PP, TySPECULARLOBECONSTANT, NUM);
  MPTWater_TyInWallTop->AddProperty("SPECULARSPIKECONSTANT", PP, TySPECULARSPIKECONSTANT, NUM);
  MPTWater_TyInWallTop->AddProperty("BACKSCATTERCONSTANT", PP, TyBACKSCATTERCONSTANT, NUM);
  MPTWater_TyInWallTop->AddProperty("REFLECTIVITY",  PP_TyREFLECTIVITY, TyREFLECTIVITY_INWALLTOP, NUMENTRIES_TY);
  OpWaterTySurfaceInWallTop->SetMaterialPropertiesTable(MPTWater_TyInWallTop);

  G4MaterialPropertiesTable *MPTWater_TyInWallBarrel = new G4MaterialPropertiesTable();
  MPTWater_TyInWallBarrel->AddProperty("RINDEX", PP, RINDEX_tyvek, NUM);
  MPTWater_TyInWallBarrel->AddProperty("SPECULARLOBECONSTANT", PP, TySPECULARLOBECONSTANT, NUM);
  MPTWater_TyInWallBarrel->AddProperty("SPECULARSPIKECONSTANT", PP, TySPECULARSPIKECONSTANT, NUM);
  MPTWater_TyInWallBarrel->AddProperty("BACKSCATTERCONSTANT", PP, TyBACKSCATTERCONSTANT, NUM);
  MPTWater_TyInWallBarrel->AddProperty("REFLECTIVITY",  PP_TyREFLECTIVITY, TyREFLECTIVITY_INWALLBARREL, NUMENTRIES_TY);
  OpWaterTySurfaceInWallBarrel->SetMaterialPropertiesTable(MPTWater_TyInWallBarrel);

  G4MaterialPropertiesTable *MPTWater_TyInWallBottom = new G4MaterialPropertiesTable();
  MPTWater_TyInWallBottom->AddProperty("RINDEX", PP, RINDEX_tyvek, NUM);
  MPTWater_TyInWallBottom->AddProperty("SPECULARLOBECONSTANT", PP, TySPECULARLOBECONSTANT, NUM);
  MPTWater_TyInWallBottom->AddProperty("SPECULARSPIKECONSTANT", PP, TySPECULARSPIKECONSTANT, NUM);
  MPTWater_TyInWallBottom->AddProperty("BACKSCATTERCONSTANT", PP, TyBACKSCATTERCONSTANT, NUM);
  MPTWater_TyInWallBottom->AddProperty("REFLECTIVITY",  PP_TyREFLECTIVITY, TyREFLECTIVITY_INWALLBOTTOM, NUMENTRIES_TY);
  OpWaterTySurfaceInWallBottom->SetMaterialPropertiesTable(MPTWater_TyInWallBottom);

  G4MaterialPropertiesTable *MPTWater_TyOutWallTop = new G4MaterialPropertiesTable();
  MPTWater_TyOutWallTop->AddProperty("RINDEX", PP, RINDEX_tyvek, NUM);
  MPTWater_TyOutWallTop->AddProperty("SPECULARLOBECONSTANT", PP, TySPECULARLOBECONSTANT, NUM);
  MPTWater_TyOutWallTop->AddProperty("SPECULARSPIKECONSTANT", PP, TySPECULARSPIKECONSTANT, NUM);
  MPTWater_TyOutWallTop->AddProperty("BACKSCATTERCONSTANT", PP, TyBACKSCATTERCONSTANT, NUM);
  MPTWater_TyOutWallTop->AddProperty("REFLECTIVITY",  PP_TyREFLECTIVITY, TyREFLECTIVITY_OUTWALLTOP, NUMENTRIES_TY);
  OpWaterTySurfaceOutWallTop->SetMaterialPropertiesTable(MPTWater_TyOutWallTop);

  G4MaterialPropertiesTable *MPTWater_TyOutWallBarrel = new G4MaterialPropertiesTable();
  MPTWater_TyOutWallBarrel->AddProperty("RINDEX", PP, RINDEX_tyvek, NUM);
  MPTWater_TyOutWallBarrel->AddProperty("SPECULARLOBECONSTANT", PP, TySPECULARLOBECONSTANT, NUM);
  MPTWater_TyOutWallBarrel->AddProperty("SPECULARSPIKECONSTANT", PP, TySPECULARSPIKECONSTANT, NUM);
  MPTWater_TyOutWallBarrel->AddProperty("BACKSCATTERCONSTANT", PP, TyBACKSCATTERCONSTANT, NUM);
  MPTWater_TyOutWallBarrel->AddProperty("REFLECTIVITY",  PP_TyREFLECTIVITY, TyREFLECTIVITY_OUTWALLBARREL, NUMENTRIES_TY);
  OpWaterTySurfaceOutWallBarrel->SetMaterialPropertiesTable(MPTWater_TyOutWallBarrel);

  G4MaterialPropertiesTable *MPTWater_TyOutWallBottom = new G4MaterialPropertiesTable();
  MPTWater_TyOutWallBottom->AddProperty("RINDEX", PP, RINDEX_tyvek, NUM);
  MPTWater_TyOutWallBottom->AddProperty("SPECULARLOBECONSTANT", PP, TySPECULARLOBECONSTANT, NUM);
  MPTWater_TyOutWallBottom->AddProperty("SPECULARSPIKECONSTANT", PP, TySPECULARSPIKECONSTANT, NUM);
  MPTWater_TyOutWallBottom->AddProperty("BACKSCATTERCONSTANT", PP, TyBACKSCATTERCONSTANT, NUM);
  MPTWater_TyOutWallBottom->AddProperty("REFLECTIVITY",  PP_TyREFLECTIVITY, TyREFLECTIVITY_OUTWALLBOTTOM, NUMENTRIES_TY);
  OpWaterTySurfaceOutWallBottom->SetMaterialPropertiesTable(MPTWater_TyOutWallBottom);

  //
  // ----

  ///////////////////////
  // ###### WLS ###### //
  ///////////////////////

  G4MaterialPropertiesTable *WlsPlasticMPT = new G4MaterialPropertiesTable();

  //double no_absorption = 1000.*m;
  //double immediate_absorption = 0.*m;
  double some_absorption = 1.*m;

  // active glass will be a thin layer absorbing everything in the right energy range
  // passive glass will be a thick layer responsible of absorption outside of the range

  const G4int nEntries_fake_abslength = 2;
  G4double fake_energy_for_abslength[nEntries_fake_abslength] =
    { 1.*eV, 7*eV };
  G4double fake_passive_abslength_vs_energy[nEntries_fake_abslength] =
    {some_absorption,some_absorption};
  WlsPlasticMPT->AddProperty("ABSLENGTH",fake_energy_for_abslength,fake_passive_abslength_vs_energy,nEntries_fake_abslength);

  // Water -> WLS surface properties
  // OpWaterWLSSurface =
  //     new G4OpticalSurface("WaterWLSSurface");

  // OpWaterWLSSurface->SetType(dielectric_dielectric);
  // OpWaterWLSSurface->SetModel(unified);
  // OpWaterWLSSurface->SetFinish(ground); // surface WLS/Water
  // OpWaterWLSSurface->SetSigmaAlpha(0.1); // TODO: What's this?

  // MATERIAL properties
#if 0
  EljenEJ286 *WLSProps = new EljenEJ286();
#else
  Kuraray *WLSProps = new Kuraray();
  //Inr *WLSProps = new Inr();
#endif
  // Define normal reflectivity from Fresnel equations

  WlsPlasticMPT->AddProperty("TRANSMITTANCE", WLSProps->GetPhotonEnergy(), WLSProps->GetTransmittance(), WLSProps->GetNumEntriesTransmittance());


  // Water -> WLS surface properties
  // OpWLSTySurface =
  //     new G4OpticalSurface("WLSTySurface");

  // OpWLSTySurface->SetType(dielectric_metal); // Only absorption and reflection
  // OpWLSTySurface->SetModel(unified);
  // OpWLSTySurface->SetFinish(ground); // ground surface with tyvek
  // OpWLSTySurface->SetSigmaAlpha(0.2);

  // Define normal reflectivity from Fresnel equations
  // G4double TransWLSTy[NUM] =
  //     {1.-0.00207792,1.-0.00207792};


  WlsPlasticMPT->AddProperty("RINDEX",WLSProps->GetPhotonEnergy(),WLSProps->GetRIndex(),WLSProps->GetNumEntries());
#if 1 // turn to 0 to make plate non-reemitting, purely passive
  WlsPlasticMPT->AddProperty("WLSABSLENGTH",WLSProps->GetPhotonEnergy_ABS(),WLSProps->GetAbs(),WLSProps->GetNumEntries_ABS());
  WlsPlasticMPT->AddProperty("WLSCOMPONENT",WLSProps->GetPhotonEnergy_EM(),WLSProps->GetEm(),WLSProps->GetNumEntries_EM());
#endif
  WlsPlasticMPT->AddConstProperty("WLSTIMECONSTANT", 1.2*ns); // TODO: Need measurement
  WLS_PVT->SetMaterialPropertiesTable(WlsPlasticMPT);

  // G4MaterialPropertiesTable *MPTWLS_Water = new G4MaterialPropertiesTable();
  // MPTWLS_Water->AddProperty("RINDEX",WLSProps->GetPhotonEnergy(),WLSProps->GetRIndex(),WLSProps->GetNumEntries());
  // MPTWLS_Water->AddProperty("TRANSMITTANCE", WLSProps->GetPhotonEnergy(), WLS_transmittance_vs_energy, nEntries_transmittance);
  // OpWaterWLSSurface->SetMaterialPropertiesTable(MPTWLS_Water);

  // G4MaterialPropertiesTable *MPTWLS_Tyvek = new G4MaterialPropertiesTable();
  // MPTWLS_Tyvek->AddProperty("RINDEX",WLSProps->GetPhotonEnergy(),WLSProps->GetRIndex(),WLSProps->GetNumEntries());
  // MPTWLS_Tyvek->AddProperty("TRANSMITTANCE", PP, TransWLSTy, NUM);
  // OpWLSTySurface->SetMaterialPropertiesTable(MPTWLS_Tyvek);

  ///////////////////////
  // ###### END ###### //
  ///////////////////////
  
  G4MaterialPropertiesTable *BGO_mpt = new G4MaterialPropertiesTable();
  G4double BGO_energy[3] = {1.9*eV, 2.6*eV, 3.3*eV};
  G4double BGO_SCINT[3] = {0.1, 1., 0.1};
  G4double BGO_RINDEX[3] = {2.15, 2.15, 2.15};
  G4double BGO_ABSL[3] = {1.118*cm, 1.118*cm, 1.118*cm};

  BGO_mpt->AddProperty("FASTCOMPONENT", BGO_energy, BGO_SCINT, 3);
  BGO_mpt->AddProperty("SLOWCOMPONENT", BGO_energy, BGO_SCINT, 3);
  BGO_mpt->AddProperty("RINDEX", BGO_energy, BGO_RINDEX, 3);
  BGO_mpt->AddProperty("ABSLENGTH", BGO_energy, BGO_ABSL, 3);

  BGO_mpt->AddConstProperty("SCINTILLATIONYIELD", 8000./MeV);
  BGO_mpt->AddConstProperty("RESOLUTIONSCALE", 1.0);
  BGO_mpt->AddConstProperty("FASTTIMECONSTANT", 300.*ns);
  BGO_mpt->AddConstProperty("SLOWTIMECONSTANT", 300.*ns);
  BGO_mpt->AddConstProperty("YIELDRATIO", 1.0);

  BGO->SetMaterialPropertiesTable(BGO_mpt);

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
   poron->SetMaterialPropertiesTable(myMPT2); // just kill all photons inside
   absorberMaterial->SetMaterialPropertiesTable(myMPT2);

   // No ABSLENGTH here in order to have actual photon propagation
   G4MaterialPropertiesTable *myMPT2_Air1 = new G4MaterialPropertiesTable();
   myMPT2_Air1->AddProperty("RINDEX", ENERGY_water, RINDEX_air, NUMENTRIES_water);
   //myMPT2_Air1->AddProperty("RAYLEIGH",ENERGY_water, RAYLEIGH_air, NUMENTRIES_water);
   Air1->SetMaterialPropertiesTable(myMPT2_Air1);
   
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

   G4MaterialPropertiesTable *myMPT5wcte = new G4MaterialPropertiesTable();
   myMPT5wcte->AddProperty("RINDEX", ENERGY_glasswcte, RINDEX_glasswcte, 45);
   myMPT5wcte->AddProperty("ABSLENGTH",ENERGY_glasswcte, ABSORPTION_glasswcte, 45);
   GlassWCTE->SetMaterialPropertiesTable(myMPT5wcte);
    
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

   G4MaterialPropertiesTable *SilGelPropTableWCTE = new G4MaterialPropertiesTable();
   SilGelPropTableWCTE->AddProperty("RINDEX", ENERGY_SilGel_WCTE, RINDEX_SilGel_WCTE, 87);
   SilGelPropTableWCTE->AddProperty("ABSLENGTH",ENERGY_SilGel_WCTE, ABSORPTION_SilGel_WCTE, 87);
   SilGel_WCTE->SetMaterialPropertiesTable(SilGelPropTableWCTE);

   //Acrylic
   G4MaterialPropertiesTable *AcrPropTable = new G4MaterialPropertiesTable();
   AcrPropTable->AddProperty("RINDEX", ENERGY_skAcrylic, RINDEX_skAcrylic, 306);
   AcrPropTable->AddProperty("ABSLENGTH", ENERGY_skAcrylic, ABSORPTION_skAcrylic, 306);
   Acrylic->SetMaterialPropertiesTable(AcrPropTable);

   //Aluminum
   //The ABSLENGTH property is added because there are rare occasions that photon is generated inside Al (reflector) material
   //Using the RINDEX_Al defined above would cause high energy photon to move faster than c and abort the event
   //This is a horrible hack to use the ABSLENGTH to kill the photons inside Al, and it should be fixed by adding a realistic refractive index
   G4MaterialPropertiesTable *AlPropTable1 = new G4MaterialPropertiesTable();
   AlPropTable1->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet, NUMENTRIES_water);
   Aluminum->SetMaterialPropertiesTable(AlPropTable1);

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

   // In order to use new photocathode physics
   G4int pmtsurftype = WCSimTuningParams->GetPMTSurfType(); // Choose one of the two models, see WCSimOpBoundaryProcess for model details
   if (pmtsurftype==1)
   {
      OpGlassCathodeSurface->SetType(G4SurfaceType(101));
   }
   else if (pmtsurftype==2)
   {
      OpGlassCathodeSurface->SetType(G4SurfaceType(102));
   }
   else if (pmtsurftype!=0)
   {
      G4cerr<<"Invalid PMT photocathode surface optical model choice: "<< pmtsurftype << G4endl;
      G4cerr<<" --> Exiting..."<<G4endl;
      exit(-1);
   }
   G4cout<<" Use photocathode model : "<<pmtsurftype<<G4endl;
   if (pmtsurftype!=0)
   {
      const G4int NCATHODEPARAMAX = 100; // a large enough number
      G4double COATEDRINDEX[NCATHODEPARAMAX], COATEDRINDEXIM[NCATHODEPARAMAX], ENERGY_COATED[NCATHODEPARAMAX];
      std::vector<std::vector<G4double>> cathodeparaTable = WCSimTuningParams->GetCathodeParaTable();
      G4int nCathodePara = WCSimTuningParams->GetNCathodePara();
      for (int i=0;i<nCathodePara;i++)
      {
        ENERGY_COATED[i] = cathodeparaTable[i][0]*eV;
        COATEDRINDEX[i] = cathodeparaTable[i][1];
        COATEDRINDEXIM[i] = cathodeparaTable[i][2];
      }
      myST2->AddProperty("COATEDRINDEX", ENERGY_COATED, COATEDRINDEX, nCathodePara);
      myST2->AddProperty("COATEDRINDEXIM", ENERGY_COATED, COATEDRINDEXIM, nCathodePara);
      myST2->AddConstProperty("COATEDTHICKNESS", WCSimTuningParams->GetCathodeThickness()*nm);
      myST2->AddConstProperty("COATEDFRUSTRATEDTRANSMISSION", COATEDFRUSTRATEDTRANSMISSION_glasscath);
   }

   OpGlassCathodeSurface->SetMaterialPropertiesTable(myST2);

   //Tyvek - jl145
   G4MaterialPropertiesTable *myST3 = new G4MaterialPropertiesTable();
   myST3->AddProperty("RINDEX", PP, RINDEX_tyvek, NUM);
   myST3->AddProperty("SPECULARLOBECONSTANT", PP, TySPECULARLOBECONSTANT, NUM);
   myST3->AddProperty("SPECULARSPIKECONSTANT", PP, TySPECULARSPIKECONSTANT, NUM);
   myST3->AddProperty("BACKSCATTERCONSTANT", PP, TyBACKSCATTERCONSTANT, NUM);
   myST3->AddProperty("REFLECTIVITY", PP_TyREFLECTIVITY, TyREFLECTIVITY_INWALLBARREL, NUMENTRIES_TY);
   myST3->AddProperty("EFFICIENCY", PP, EFFICIENCY_blacksheet, NUM);
   //use same efficiency as blacksheet, which is 0
   OpWaterTySurfaceInWallBarrel->SetMaterialPropertiesTable(myST3);


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

   ReflectorSkinSurfaceWCTE = new G4OpticalSurface("ReflectorSurfaceWCTE");
   ReflectorSkinSurfaceWCTE->SetType(dielectric_metal);
   ReflectorSkinSurfaceWCTE->SetModel(unified);  
   ReflectorSkinSurfaceWCTE->SetFinish(polished);
   G4MaterialPropertiesTable *ReflectorSkinSurfaceWCTETable = new G4MaterialPropertiesTable();
   ReflectorSkinSurfaceWCTETable->AddProperty("REFLECTIVITY", ENERGY_reflector, REFLECTIVITY_reflector, NUMENTRIES_reflector);
   ReflectorSkinSurfaceWCTE->SetMaterialPropertiesTable(ReflectorSkinSurfaceWCTETable);

   OpGelFoamSurface =
     new G4OpticalSurface("GelFoamSurface");

   OpGelFoamSurface->SetType(dielectric_dielectric);
   OpGelFoamSurface->SetModel(unified); 
   OpGelFoamSurface->SetFinish(groundfrontpainted);
   OpGelFoamSurface->SetSigmaAlpha(0.1);

   OpGelFoamSurface->SetMaterialPropertiesTable(myST1); //TF: same as water-blacksheet for now



   //ToDo:
   //G4MaterialPropertiesTable *AgPropTable = new G4MaterialPropertiesTable();
   //G4MaterialPropertiesTable *AlAg1PropTable = new G4MaterialPropertiesTable();
}

void WCSimDetectorConstruction::AddDopedWater(G4double percentGd){
  G4Material * Water = G4Material::GetMaterial("Water");
  G4Material * DopedWater = G4Material::GetMaterial("Doped Water", false);
  //Delete old doped water if it exists
  if(DopedWater) delete DopedWater;
  //Create new doped water material
  DopedWater = new G4Material("Doped Water",1.00*g/cm3,2);
  //Gd doped water has the same optical properties as pure water
  DopedWater->SetMaterialPropertiesTable(Water->GetMaterialPropertiesTable());
  //Set concentration
  G4double a = 157.25*g/mole;
  G4Element* Gd = new G4Element("Gadolinium","Gd", 64,a);
  DopedWater->AddMaterial(Water,(100.-percentGd)*perCent);
  DopedWater->AddElement(Gd,percentGd*perCent);
}
