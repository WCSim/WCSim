#ifndef WCSimPrimaryGeneratorAction_h
#define WCSimPrimaryGeneratorAction_h

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include "WCSimEnumerations.hh"

#include <fstream>

#include "WCSimRootOptions.hh"
#include "TFile.h"
#include "TTree.h"
#include "TNRooTrackerVtx.hh"
#include "TClonesArray.h"

class WCSimDetectorConstruction;
class G4ParticleGun;
class G4GeneralParticleSource;
class G4Event;
class WCSimPrimaryGeneratorMessenger;

class WCSimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
    public:
        WCSimPrimaryGeneratorAction(WCSimDetectorConstruction*);
        ~WCSimPrimaryGeneratorAction();

    public:
        void GeneratePrimaries(G4Event* anEvent);
        void SetupBranchAddresses(NRooTrackerVtx* nrootrackervtx);
        void OpenRootrackerFile(G4String fileName);
        void CopyRootrackerVertex(NRooTrackerVtx* nrootrackervtx);
        bool GetIsRooTrackerFileFinished(){return (fEvNum==fNEntries);}

        // Gun, laser & gps setting calls these functions to fill jhfNtuple and Root tree
        void SetVtx(G4ThreeVector i)     { vtx = i; };
        void SetBeamEnergy(G4double i)   { beamenergy = i; };
        void SetBeamDir(G4ThreeVector i) { beamdir = i; };
        void SetBeamPDG(G4int i)         { beampdg = i; };

        // These go with jhfNtuple
        G4int GetVecRecNumber(){return vecRecNumber;}
        //G4int GetMode() {return mode;};
        InteractionType_t GetMode() {return mode;};
        G4int GetVtxVol() {return vtxvol;};
        G4ThreeVector GetVtx() {return vtx;}
        G4int GetNpar() {return npar;};
        G4int GetBeamPDG() {return beampdg;};
        G4double GetBeamEnergy() {return beamenergy;};
        G4ThreeVector GetBeamDir() {return beamdir;};
        G4int GetTargetPDG() {return targetpdg;};
        G4double GetTargetEnergy() {return targetenergy;};
        G4ThreeVector GetTargetDir() {return targetdir;};

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
        G4bool   useMulineEvt;
        G4bool   useRootrackerEvt;
        G4bool   useGunEvt;
        G4bool   useLaserEvt;  //T. Akiri: Laser flag
        G4bool   useGPSEvt;
        std::fstream inputFile;
        G4String vectorFileName;
        G4bool   GenerateVertexInRock;
        G4bool   usePoissonPMT;
        G4double poissonPMTMean;

        // These go with jhfNtuple
        //G4int mode;
        InteractionType_t mode;
        G4int vtxvol;
        G4ThreeVector vtx;
        G4int npar;
        G4int beampdg, targetpdg;
        G4ThreeVector beamdir, targetdir;
        G4double beamenergy, targetenergy;
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

    public:

        inline TFile* GetInputRootrackerFile(){ return fInputRootrackerFile;}

        inline void SetMulineEvtGenerator(G4bool choice) { useMulineEvt = choice; }
        inline G4bool IsUsingMulineEvtGenerator() { return useMulineEvt; }

        inline void SetRootrackerEvtGenerator(G4bool choice) { useRootrackerEvt = choice; }
        inline G4bool IsUsingRootrackerEvtGenerator() { return useRootrackerEvt; }

        inline void SetGunEvtGenerator(G4bool choice) { useGunEvt = choice; }
        inline G4bool IsUsingGunEvtGenerator()  { return useGunEvt; }

        //T. Akiri: Addition of function for the laser flag
        inline void SetLaserEvtGenerator(G4bool choice) { useLaserEvt = choice; }
        inline G4bool IsUsingLaserEvtGenerator()  { return useLaserEvt; }
  
        inline void SetGPSEvtGenerator(G4bool choice) { useGPSEvt = choice; }
        inline G4bool IsUsingGPSEvtGenerator()  { return useGPSEvt; }

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
        inline G4bool IsGeneratingVertexInRock() { return GenerateVertexInRock; }
        inline void SetGenerateVertexInRock(G4bool choice) { GenerateVertexInRock = choice; }

        inline void SetPoissonPMT(G4bool choice) { usePoissonPMT = choice; }
        inline G4bool IsUsingPoissonPMT(){ return usePoissonPMT; }
  
        inline void SetPoissonPMTMean(G4double val){ poissonPMTMean = val; }
        inline G4double GetPoissonPMTMean(){ return poissonPMTMean; }
};

#endif


