#ifndef WCSimPrimaryGeneratorAction_h
#define WCSimPrimaryGeneratorAction_h

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include <fstream>

class WCSimDetectorConstruction;
class G4ParticleGun;
class G4GeneralParticleSource;
class G4Event;
class WCSimPrimaryGeneratorMessenger;
class G4Generator;

class WCSimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  WCSimPrimaryGeneratorAction(WCSimDetectorConstruction*);
  ~WCSimPrimaryGeneratorAction();
  
public:
  void GeneratePrimaries(G4Event* anEvent);

  // Normal gun setting calls these functions to fill jhfNtuple and Root tree
  void SetVtx(G4ThreeVector i)     {  vtx = i; vtxs[0] = i; nvtxs = 1; };
  void SetBeamEnergy(G4double i)   { beamenergy = i; beamenergies[0] = i;};
  void SetBeamDir(G4ThreeVector i) { beamdir = i; beamdirs[0] = i;};
  void SetBeamPDG(G4int i)         { beampdg = i; beampdgs[0] = i;};
  void SetNvtxs(G4int i)     { nvtxs = i; };
  void SetVtxs(G4int i, G4ThreeVector v)     { vtxs[i] = v; };
  void SetBeamEnergies(G4int n, G4double i)   { beamenergies[n] = i; };
  void SetBeamDirs(G4int n, G4ThreeVector i) { beamdirs[n] = i; };
  void SetBeamPDGs(G4int n, G4int i)         { beampdgs[n] = i; };

  // These go with jhfNtuple
  G4int GetVecRecNumber(){return vecRecNumber;}
  G4int GetMode() {return mode;};
  G4int GetNvtxs() {return nvtxs;};
  G4int GetVtxVol() {return vtxvol;};
  G4int GetVtxsVol(G4int i) {return vtxsvol[i];};
  G4ThreeVector GetVtx() {return vtx;}
  G4ThreeVector GetVtxs(G4int i) {return vtxs[i];}
  G4int GetNpar() {return npar;};
  G4int GetBeamPDG() {return beampdg;};
  G4int GetBeamPDGs(G4int n) {return beampdgs[n];};
  G4double GetBeamEnergy() {return beamenergy;};
  G4double GetBeamEnergies(G4int n) {return beamenergies[n];};
  G4ThreeVector GetBeamDir() {return beamdir;};
  G4ThreeVector GetBeamDirs(G4int n) {return beamdirs[n];};
  G4int GetTargetPDG() {return targetpdg;};
  G4int GetTargetPDGs(G4int n) {return targetpdgs[n];};
  G4double GetTargetEnergy() {return targetenergy;};
  G4double GetTargetEnergies(G4int n) {return targetenergies[n];};
  G4ThreeVector GetTargetDir() {return targetdir;};
  G4ThreeVector GetTargetDirs(G4int n) {return targetdirs[n];};

  // older ...
  G4double GetNuEnergy() {return nuEnergy;};
  G4double GetEnergy() {return energy;};
  G4double GetXPos() {return xPos;};
  G4double GetYPos() {return yPos;};
  G4double GetZPos() {return zPos;};
  G4double GetXDir() {return xDir;};
  G4double GetYDir() {return yDir;};
  G4double GetZDir() {return zDir;};

private:
  WCSimDetectorConstruction*      myDetector;
  G4ParticleGun*                  particleGun;
  G4GeneralParticleSource*        MyGPS;  //T. Akiri: GPS to run Laser
  WCSimPrimaryGeneratorMessenger* messenger;

  // Variables set by the messenger
  G4bool   useMulineEvt;
  G4bool   useNormalEvt;
  G4bool   useLaserEvt;  //T. Akiri: Laser flag
  G4bool   useRadioactiveEvt;
  std::fstream inputFile;
  G4String vectorFileName;
  G4bool   GenerateVertexInRock;

  // These go with jhfNtuple
  G4int mode;
  G4int vtxvol;
  G4ThreeVector vtx;
  G4int nvtxs;
  G4int vtxsvol[50];
  G4ThreeVector vtxs[50];
  G4int npar;
  G4int beampdg, targetpdg;
  G4int beampdgs[50], targetpdgs[50];
  G4ThreeVector beamdir, targetdir;
  G4ThreeVector beamdirs[50], targetdirs[50];
  G4double beamenergy, targetenergy;
  G4double beamenergies[50], targetenergies[50];
  G4int vecRecNumber;

  G4double nuEnergy;
  G4double energy;
  G4double xPos, yPos, zPos;
  G4double xDir, yDir, zDir;

  G4int    _counterRock; 
  G4int    _counterCublic; 
public:

  inline void SetMulineEvtGenerator(G4bool choice) { useMulineEvt = choice; }
  inline G4bool IsUsingMulineEvtGenerator() { return useMulineEvt; }

  inline void SetNormalEvtGenerator(G4bool choice) { useNormalEvt = choice; }
  inline G4bool IsUsingNormalEvtGenerator()  { return useNormalEvt; }

  //T. Akiri: Addition of function for the laser flag
  inline void SetLaserEvtGenerator(G4bool choice) { useLaserEvt = choice; }
  inline G4bool IsUsingLaserEvtGenerator()  { return useLaserEvt; }

  inline void SetRadioactiveEvtGenerator(G4bool choice) { useRadioactiveEvt = choice; }
  inline G4bool IsUsingRadioactiveEvtGenerator()  { return useRadioactiveEvt; }

  inline void OpenVectorFile(G4String fileName) 
  {
    if ( inputFile.is_open() ) 
      inputFile.close();

    vectorFileName = fileName;
    inputFile.open(vectorFileName, std::fstream::in);
  }
  inline G4bool IsGeneratingVertexInRock() { return GenerateVertexInRock; }
  inline void SetGenerateVertexInRock(G4bool choice) { GenerateVertexInRock = choice; }

};

#endif


