#ifndef WCSimPrimaryGeneratorAction_h
#define WCSimPrimaryGeneratorAction_h

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "WCSimIBDGen.hh"
#include "globals.hh"

#include "WCSimRootOptions.hh"
#include "WCSimGenerator_Radioactivity.hh"
#include "WCSimLIGen.hh"
#include "WCSimEnumerations.hh"
#include "jhfNtuple.h"

#include <G4String.hh>
#include <fstream>

#include "TFile.h"
#include "TTree.h"
#include "TNRooTrackerVtx.hh"
#include "TClonesArray.h"
#include "TH2D.h"

class WCSimDetectorConstruction;
class WCSimPrimaryGeneratorMessenger;
class G4ParticleGun;
class G4GeneralParticleSource;
class G4Event;

struct radioactive_source {
  G4String IsotopeName;
  G4String IsotopeLocation;
  G4double IsotopeActivity;
};

class WCSimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  WCSimPrimaryGeneratorAction(WCSimDetectorConstruction*);
  ~WCSimPrimaryGeneratorAction();

  void GeneratePrimaries(G4Event* anEvent);

  // RooTracker related functions
  void SetupBranchAddresses(NRooTrackerVtx* nrootrackervtx);
  void OpenRootrackerFile(G4String fileName);
  void CopyRootrackerVertex(NRooTrackerVtx* nrootrackervtx);
  bool GetIsRooTrackerFileFinished(){return (fEvNum==fNEntries);}

  // Gun, laser & gps setting calls these functions to fill jhfNtuple and Root tree
  void SetVtx(G4ThreeVector i)     { vtxs[0] = i; nvtxs = 1; };
  void SetBeamEnergy(G4double i, G4int n = 0)   { beamenergies[n] = i;};
  void SetBeamDir(G4ThreeVector i, G4int n = 0) { beamdirs[n] = i;};
  void SetBeamPDG(G4int i, G4int n = 0)         { beampdgs[n] = i;};
  void SetNvtxs(G4int i)     { nvtxs = i; };
  void SetVtxs(G4int i, G4ThreeVector v)     { vtxs[i] = v; };

  // These go with jhfNtuple
  G4int GetVecRecNumber(){return vecRecNumber;}
  G4int GetMode(int vertex = 0){return mode[vertex];};
  //InteractionType_t GetMode(int vertex = 0) {return mode[vertex];};
  G4int GetNvtxs() {return nvtxs;};
  G4int GetVtxVol(G4int n = 0) {return vtxsvol[n];};
  G4ThreeVector GetVtx(G4int n = 0) {return vtxs[n];}
  G4double GetVertexTime(G4int n = 0){return vertexTimes[n];}
  G4int GetNpar() {return npar;};
  G4int GetBeamPDG(G4int n = 0) {return beampdgs[n];};
  G4double GetBeamEnergy(G4int n = 0) {return beamenergies[n];};
  G4ThreeVector GetBeamDir(G4int n = 0) {return beamdirs[n];};
  G4int GetTargetPDG(G4int n = 0) {return targetpdgs[n];};
  G4double GetTargetEnergy(G4int n = 0) {return targetenergies[n];};
  G4ThreeVector GetTargetDir(G4int n = 0) {return targetdirs[n];};

  // older ...
  G4double GetNuEnergy() {return nuEnergy;};
  G4double GetEnergy() {return energy;};
  G4double GetXPos() {return xPos;};
  G4double GetYPos() {return yPos;};
  G4double GetZPos() {return zPos;};
  G4double GetXDir() {return xDir;};
  G4double GetYDir() {return yDir;};
  G4double GetZDir() {return zDir;};

  G4String GetGeneratorTypeString();

  void SaveOptionsToOutput(WCSimRootOptions * wcopt);

private:
  WCSimDetectorConstruction*      myDetector;
  G4ParticleGun*                  particleGun;
  G4GeneralParticleSource*        MyGPS;  //T. Akiri: GPS to run Laser
  WCSimPrimaryGeneratorMessenger* messenger;

  // Variables set by the messenger
  G4bool   useAmBeEvt;
  G4bool   useMulineEvt;
  G4bool   useRootrackerEvt;
  G4bool   useGunEvt;
  G4bool   useLaserEvt;  //T. Akiri: Laser flag
  G4bool   useInjectorEvt; // K.M.Tsui: injector flag
  G4bool   useGPSEvt;
  G4bool   useIBDEvt; // IBD generator flag
  G4bool   useDataTableEvt; // DataTable flag
  G4bool   useCosmics;
  G4bool   useRadioactiveEvt; // F. Nova: Radioactive flag
  G4bool   useRadonEvt; // G. Pronost: Radon flag
  G4bool   useLightInjectorEvt; // L. Kneale injector with profile from db

  std::fstream inputFile;
  std::fstream inputCosmicsFile;
  G4String vectorFileName;
  G4String cosmicsFileName = "data/MuonFlux-HyperK-ThetaPhi.dat";
  G4bool   GenerateVertexInRock;

  // IBD generator
    // Database for spectra
  G4String ibd_database;
    // Specific model to use
  G4String ibd_model;
    // IBD generator object
  WCSimIBDGen* IBDGen;

  // Variables for Radioactive and Radon generators
  std::vector<struct radioactive_source> radioactive_sources;
  G4double radioactive_time_window;

  // For Rn event
  WCSimGenerator_Radioactivity* myRn222Generator;
  G4int fRnScenario;
  G4int fRnSymmetry;

  G4bool   usePoissonPMT;
  G4double poissonPMTMean;

  // For injector events
  G4int nPhotons;
  G4int injectorOnIdx;
  G4double twindow;
  G4double openangle;
  G4double wavelength;

  // For light injector with profile from db
  WCSimLIGen* LIGen;
  G4int nphotons;
  G4String injectorType;
  G4String injectorIdx;
  G4String injectorFilename;
  G4bool photonMode;

  //
  G4double fTimeUnit;

  // These go with jhfNtuple
  G4int mode[MAX_N_VERTICES];
  //InteractionType_t mode[MAX_N_VERTICES];
  G4int nvtxs;
  G4int vtxsvol[MAX_N_VERTICES];
  G4ThreeVector vtxs[MAX_N_VERTICES];
  G4double vertexTimes[MAX_N_VERTICES];
  G4int npar;
  G4int beampdgs[MAX_N_PRIMARIES], targetpdgs[MAX_N_PRIMARIES];
  G4ThreeVector beamdirs[MAX_N_PRIMARIES], targetdirs[MAX_N_PRIMARIES];
  G4double beamenergies[MAX_N_PRIMARIES], targetenergies[MAX_N_PRIMARIES];
  G4int vecRecNumber;

  G4double nuEnergy;
  G4double energy;
  G4double xPos, yPos, zPos;
  G4double xDir, yDir, zDir;

  G4int    _counterRock;
  G4int    _counterCublic;

  // Counters to read Rootracker event file
  int fEvNum;
  int fNEntries;
  TFile* fInputRootrackerFile;

  // Pointers to Rootracker vertex objects
  // Temporary vertex that is saved if desired, according to WCSimIO macro option
  TTree* fRooTrackerTree;
  TTree* fSettingsTree;
  NRooTrackerVtx* fTmpRootrackerVtx;
  float fNuPrismRadius;
  float fNuBeamAng;
  float fNuPlanePos[3];


  // Use Histograms to generate cosmics
  void Create_cosmics_histogram();
  TH2D *hFluxCosmics  = nullptr;
  TH2D *hEmeanCosmics = nullptr;

  // Set cosmics altitude
  G4double altCosmics;

  bool needConversion;
  bool foundConversion;
  const G4ParticleDefinition * conversionProductParticle[2];
  G4ThreeVector conversionProductMomentum[2];
	
public:

  inline void SetMulineEvtGenerator(G4bool choice) { useMulineEvt = choice; }
  inline G4bool IsUsingMulineEvtGenerator() { return useMulineEvt; }

  inline void SetAmBeEvtGenerator(G4bool choice) { useAmBeEvt = choice; }
  inline G4bool IsUsingAmBeEvtGenerator()  { return useAmBeEvt; }

  inline TFile* GetInputRootrackerFile(){ return fInputRootrackerFile;}
  inline void SetRootrackerEvtGenerator(G4bool choice) { useRootrackerEvt = choice; }
  inline G4bool IsUsingRootrackerEvtGenerator() { return useRootrackerEvt; }

  inline void SetGunEvtGenerator(G4bool choice) { useGunEvt = choice; }
  inline G4bool IsUsingGunEvtGenerator()  { return useGunEvt; }

  //T. Akiri: Addition of function for the laser flag
  inline void SetLaserEvtGenerator(G4bool choice) { useLaserEvt = choice; }
  inline G4bool IsUsingLaserEvtGenerator()  { return useLaserEvt; }

  inline void SetGPSEvtGenerator(G4bool choice) { useGPSEvt = choice; }
  inline G4bool IsUsingGPSEvtGenerator()  { return useGPSEvt; }

  // K.M.Tsui: addition of injector events
  inline void SetInjectorEvtGenerator(G4bool choice) { useInjectorEvt = choice; }
  inline G4bool IsUsingInjectorEvtGenerator()  { return useInjectorEvt; }
  inline void SetInjectorBeamPhotons(G4int np) { nPhotons = np;}
  inline void SetInjectorOnIdx(G4int idx) { injectorOnIdx = idx;}
  inline void SetInjectorTimeWindow(G4double tw) { twindow = tw;}
  inline void SetInjectorOpeningAngle(G4double angle) { openangle = angle;}
  inline void SetInjectorWavelength(G4double wl) { wavelength = wl;}

  // Addition of IBD event generator
  inline void SetIBDEvtGenerator(G4bool choice) { useIBDEvt = choice; }
  inline G4bool IsUsingIBDEvtGenerator()  { return useIBDEvt; }
  inline void SetIBDDatabase(G4String choice) { ibd_database = choice; }
  inline G4String GetIBDDatabase()  { return ibd_database; }
  inline void SetIBDModel(G4String choice) { ibd_model = choice; }
  inline G4String GetIBDModel()  { return ibd_model; }

  // L. Kneale: light injector with profile from db
  inline void SetLightInjectorEvtGenerator(G4bool choice) {useLightInjectorEvt = choice; }
  inline G4bool IsUsingLightInjectorEvtGenerator()        {return useLightInjectorEvt; }
  inline void SetLightInjectorType(G4String choice)       { injectorType = choice; }
  inline void SetLightInjectorIdx(G4String choice)        { injectorIdx = choice; }
  inline void SetLightInjectorNPhotons(G4int choice)      { nphotons=choice; }
  inline void SetLightInjectorFilename(G4String choice)   { injectorFilename = choice; }
  inline void SetLightInjectorMode(G4bool choice)         { photonMode = choice; }

  inline void SetDataTableEvtGenerator(G4bool choice) {
    useDataTableEvt = choice;
  }
  inline G4bool IsUsingDataTableEvtGenerator() { return useDataTableEvt; }


  inline void SetCosmicsGenerator(G4bool choice) { useCosmics = choice; }
  inline G4bool IsUsingCosmicsGenerator()  { return useCosmics; }

  inline void OpenVectorFile(G4String fileName) 
  {
      if ( inputFile.is_open() ) 
          inputFile.close();

      vectorFileName = fileName;
      inputFile.open(vectorFileName, std::fstream::in);
      
      if ( !inputFile.is_open() ) {
        G4cout << "Vector file " << vectorFileName << " not found" << G4endl;
        exit(-1);
      }

  }

  inline void OpenCosmicsFile(G4String fileName)
  {
    if ( inputCosmicsFile.is_open() )
      inputCosmicsFile.close();

    cosmicsFileName = fileName;
    inputCosmicsFile.open(cosmicsFileName, std::fstream::in);

    if ( !inputCosmicsFile.is_open() ) {
      G4cout << "Cosmics data file " << cosmicsFileName << " not found" << G4endl;
      exit(-1);
    }
  }

  inline G4bool IsGeneratingVertexInRock() { return GenerateVertexInRock; }
  inline void SetGenerateVertexInRock(G4bool choice) { GenerateVertexInRock = choice; }

  inline void AddRadioactiveSource(G4String IsotopeName, G4String IsotopeLocation, G4double IsotopeActivity){
    struct radioactive_source r;
    r.IsotopeName = IsotopeName;
    r.IsotopeLocation = IsotopeLocation;
    r.IsotopeActivity = IsotopeActivity;
    radioactive_sources.push_back(r);
  }
  inline std::vector<struct radioactive_source> Radioactive_Sources()  { return radioactive_sources; }

  inline void SetRadioactiveEvtGenerator(G4bool choice) { useRadioactiveEvt = choice; }
  inline G4bool IsUsingRadioactiveEvtGenerator() 	{ return useRadioactiveEvt; }

  inline void SetRadioactiveTimeWindow(G4double choice) { radioactive_time_window = choice; }
  inline G4double GetRadioactiveTimeWindow()  		{ return radioactive_time_window; }

  inline void SetRadonEvtGenerator(G4bool choice) 	{ useRadonEvt = choice; }
  inline G4bool IsUsingRadonEvtGenerator()  		{ return useRadonEvt; }

  inline void SetRadonScenario(G4int choice) 		{ fRnScenario = choice; }
  inline G4int GetRadonScenario() 			{ return fRnScenario; }

  inline void SetRadonSymmetry(G4int choice) 		{ fRnSymmetry = choice; }
  inline G4int GetRadonSymmetry() 			{ return fRnSymmetry; }

  inline void SetPoissonPMT(G4bool choice) { usePoissonPMT = choice; }
  inline G4bool IsUsingPoissonPMT(){ return usePoissonPMT; }

  inline void SetPoissonPMTMean(G4double val){ poissonPMTMean = val; }
  inline G4double GetPoissonPMTMean(){ return poissonPMTMean; }

  inline bool IsConversionFound(){ return foundConversion; }
  inline void FoundConversion(){ foundConversion = true; }
  inline void SetConversionProductParticle(int i, const G4ParticleDefinition *p) { conversionProductParticle[i] = p; }
  inline void SetConversionProductMomentum(int i, const G4ThreeVector& p) { conversionProductMomentum[i] = p; }
  inline void SetNeedConversion(bool choice) { needConversion = choice; foundConversion = !choice; }
  inline bool NeedsConversion() { return needConversion; }

  //static const HepDouble nanosecond  = 1.;
  //static const HepDouble second      = 1.e+9 *nanosecond;
  //static const HepDouble millisecond = 1.e-3 *second;
  //static const HepDouble microsecond = 1.e-6 *second;
  //static const HepDouble  picosecond = 1.e-12*second;
  inline void SetTimeUnit(G4String choice)
  {
    if(choice == "ns" || choice=="nanosecond")
      fTimeUnit=CLHEP::nanosecond;//*second;
    else if(choice == "s" || choice=="second")
      fTimeUnit=CLHEP::second;
    else if (choice == "ms" || choice=="millisecond")
      fTimeUnit=CLHEP::millisecond;
    else if (choice=="microsecond")
      fTimeUnit=CLHEP::microsecond;
    else if(choice=="ps" || choice=="picosecond")
      fTimeUnit=CLHEP::picosecond;
    else
      fTimeUnit=CLHEP::nanosecond;
  }
  inline G4double GetTimeUnit()       { return fTimeUnit; }

};

#endif
