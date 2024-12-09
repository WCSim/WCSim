#ifndef WCSim_RootEvent
#define WCSim_RootEvent

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//    WCSim_RootEvent                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <string>
#include <vector>
//#include <map>

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TVector3.h"

#include "jhfNtuple.h"
#include "WCSimEnumerations.hh"

class TDirectory;


/**
 * \class WCSimRootTrack
 * \brief Class holding true track information.
 *
 * By default,
 * - No Cherenkov photons are saved in the output file
 * (in fact no photons are stored at all. The exception
 * is what Geant terms as gammas - basically high energy photons)
 * - Everything that creates a photon that creates a true PMT hit is saved,
 * along with all its ancestors
 */
class WCSimRootTrack : public TObject {

private:

  // See jhfNtuple.h for the meaning of these data members:
  Int_t   fIpnu; //!< Track PDG code
  Int_t   fFlag; //!< -2: Neutrino target nucleus track. -1: Primary neutrino track. 0: Final state particle track
  Float_t fM; //!< Particle mass (units: MeV/c2)
  Float_t fP; //!< Particle initial momentum magnitude (units: MeV/c)
  Float_t fE; //!< Particle initial total energy = sqrt(p^2 + m^2) (units: MeV)
  Int_t   fStartvol; //!< Particle starting volume
  Int_t   fStopvol; //!< Particle stopping volume
  Float_t fDir[3]; //!< Particle initial direction. Unit vector in x,y,z
  Float_t fPdir[3]; //!< Particle initial momentum. Vector in x,y,z (units: MeV). The magnitude of this vector is equivalent to fP.
  Float_t fStop[3]; //!< Particle stopping position. Vector in x,y,z (units: cm)
  Float_t fStart[3]; //!< Particle starting position. Vector in x,y,z (units: cm)
  Int_t fParenttype; //!< PDG code of parent particle (0 for primary, 999 for parent PDG code not found in the specific logic used)
  ProcessType_t fCreatorProcess; //!< The physics process that created the track
  Double_t fTime; //!< Particle start time
  Int_t fId; //!< Unique track ID for this particle track. This is the ID to use for particle history tracking (not position in the WCSimRootTrack array)
  Int_t fParentId; //!< Unique track ID for the parent of this track. This is the ID to use for particle history tracking (not position in the WCSimRootTrack array)
  std::vector<std::vector<float>> boundaryPoints; //!< The position (x,y,z) where the particle crossed the blacksheet/tyvek/cave (units: cm)
  std::vector<float> boundaryKEs; //!< The particle kinetic energy as it crossed the blacksheet/tyvek/cave (units: MeV)
  std::vector<double> boundaryTimes; //!< The time as the particle crossed the blacksheet/tyvek/cave (units: ns)
  std::vector<int> boundaryTypes; //!< The surface the particle has crossed. 1 = blacksheet, 2 = tyvek, 3 = cave. Note that all boundary* variables are in synch

public:
  WCSimRootTrack() {}
  WCSimRootTrack(Int_t ipnu, 
		 Int_t flag,
		 Double_t m,
		 Double_t p,
		 Double_t E,
		 Int_t startvol,
		 Int_t stopvol,
		 Double_t dir[3],
		 Double_t pdir[3],
		 Double_t stop[3],
		 Double_t start[3],
		 Int_t parenttype,
     ProcessType_t creatorProcess,
		 Double_t time,
		 Int_t id,
		 Int_t idParent,
     std::vector<std::vector<float>> bPs,
     std::vector<float> bKEs,
     std::vector<double> bTimes,
     std::vector<int> bTypes);
  virtual ~WCSimRootTrack() { }
  bool CompareAllVariables(const WCSimRootTrack * c) const;

  Int_t     GetIpnu() const { return fIpnu;}
  Int_t     GetFlag() const { return fFlag;}
  Float_t   GetM() const { return fM;}
  Float_t   GetP() const { return fP;}
  Float_t   GetE() const { return fE;}
  Int_t     GetStartvol() const { return fStartvol;}
  Int_t     GetStopvol() const { return fStopvol;}
  Float_t   GetDir(Int_t i=0) const {return (i<3) ? fDir[i] : 0;}
  Float_t   GetPdir(Int_t i=0) const {return (i<3) ? fPdir[i] : 0;}
  Float_t   GetStop(Int_t i=0) const {return (i<3) ? fStop[i] : 0;}
  Float_t   GetStart(Int_t i=0) const {return (i<3) ? fStart[i] : 0;}
  Int_t     GetParenttype(/*Int_t i=0*/) const {return fParenttype;}
  Double_t   GetTime() const { return fTime;}
  Int_t     GetId() const {return fId;}
  Int_t     GetParentId() const {return fParentId;}
  ProcessType_t GetCreatorProcess() const {return fCreatorProcess;}
  std::string   GetCreatorProcessName() const {return WCSimEnumerations::EnumAsString(fCreatorProcess);}
  std::vector<std::vector<float>> GetBoundaryPoints() const {return boundaryPoints;}
  std::vector<float> GetBoundaryKEs() const {return boundaryKEs;}
  std::vector<double> GetBoundaryTimes() const {return boundaryTimes;}
  std::vector<int> GetBoundaryTypes() const {return boundaryTypes;}

  ClassDef(WCSimRootTrack,4)
};


//////////////////////////////////////////////////////////////////////////

/**
 * \class WCSimRootCherenkovHit
 * \brief Class holding true (Cherenkov photon + dark noise) hit information.
 *
 * For each PMT that has at least 1 PMT hit,
 * this class specifies the position of the detailed
 * true hit information in the WCSimRootCherenkovHitTime array
 */
class WCSimRootCherenkovHit : public TObject {

private:
  Int_t fTubeID; //!< PMT ID for the given PMT type. WARNING: Runs from 1 to N (not 0 to N-1, which is how you access elements from WCSimRootGeom)
  Int_t fmPMTID; //!< The mPMT number this 3" PMT is in (Equivalent to fTubeID for 20" & OD PMTs)
  Int_t fmPMT_PMTID; //!< The 3" PMT position ID inside this mPMT (runs from 1-19 for 3" PMTs inside mPMTs. Is 0 for 20" & OD PMTs)
  Int_t fTotalPe[2]; //!< 0th element: position in the WCSimRootCherenkovHitTime array where this PMTs' true hits start in this event. 1st element: Number of true hits on this PMT in this event. 

public:
  WCSimRootCherenkovHit() {}
  WCSimRootCherenkovHit(Int_t tubeID,
			Int_t totalPe[2]);

  WCSimRootCherenkovHit(Int_t tubeID,
			Int_t mPMTID,
			Int_t mPMT_PMTID,
			Int_t totalPe[2]);

  virtual ~WCSimRootCherenkovHit() { }

  bool CompareAllVariables(const WCSimRootCherenkovHit * c) const;

  Int_t GetTubeID()       const { return fTubeID;}
  Int_t GetmPMTID()       const { return fmPMTID;}
  Int_t GetmPMT_PMTID()       const { return fmPMT_PMTID;}
  Int_t GetTotalPe(int i) const { return (i<2) ? fTotalPe[i]: 0;}

  ClassDef(WCSimRootCherenkovHit,2)  
};

/**
 * \class WCSimRootCherenkovHitTime
 * \brief Class holding true (Cherenkov photon + dark noise) hit information
 *
 * There is one entry per hit.
 */
class WCSimRootCherenkovHitTime : public TObject {

private:
  // See jhfNtuple.h for the meaning of these data members:
  Double_t fTruetime; //!< True hit time (unit: ns)
  Int_t   fParentSavedTrackID; //!< Truth matching. ID of the parent track that created the Cherenkov photon that created this hit. Note that this is not the position in the WCSimRootTrack array - you do need to loop and check the ID. Note that if you are running in non-default mode and you are saving photon tracks, this will be the ID of the photon track. Note: for dark noise, this number is -1
  Float_t fPhotonStartTime; //!< Start time of the photon that created this hit (unit: ns)
  Float_t fPhotonStartPos[3]; //!< Start position (x,y,z) of the photon that created this hit (unit: cm)
  Float_t fPhotonEndPos[3]; //!< End position (x,y,z) of the photon that created this hit (unit: cm)
  Float_t fPhotonStartDir[3]; //< Start direction unit vector (x,y,z) of the photon that created this hit
  Float_t fPhotonEndDir[3]; //< End direction unit vector (x,y,z) of the photon that created this hit
  ProcessType_t fPhotonCreatorProcess; //!< Process that created the photon that created this hit
  Double_t fWavelength; // photon wavelength

public:
  WCSimRootCherenkovHitTime() {}
  WCSimRootCherenkovHitTime(Double_t truetime,
			    Int_t   parentSavedTrackID,
			    Float_t photonStartTime,
			    Float_t photonStartPos[3],
			    Float_t photonEndPos[3],
			    Float_t photonStartDir[3],
			    Float_t photonEndDir[3],
			    ProcessType_t photonCreatorProcess,
			    Double_t wavelength);
  virtual ~WCSimRootCherenkovHitTime() { }
  bool CompareAllVariables(const WCSimRootCherenkovHitTime * c) const;

  Double_t  GetTruetime() const { return fTruetime;}
  Int_t     GetParentID() const { return fParentSavedTrackID;} // deprecated
  Int_t     GetParentSavedTrackID() const { return fParentSavedTrackID;}
  Float_t   GetPhotonStartTime() const { return fPhotonStartTime; }
  Float_t   GetPhotonStartPos(int i) const { return (i<3) ? fPhotonStartPos[i] : 0; }
  Float_t   GetPhotonEndPos(int i) const { return (i<3) ? fPhotonEndPos[i] : 0; }
  Float_t   GetPhotonStartDir(int i) const { return (i<3) ? fPhotonStartDir[i] : 0; }
  Float_t   GetPhotonEndDir(int i) const { return (i<3) ? fPhotonEndDir[i] : 0; }
  ProcessType_t GetPhotonCreatorProcess() const {return  fPhotonCreatorProcess;}
  std::string   GetPhotonCreatorProcessName() const {return  WCSimEnumerations::EnumAsString(fPhotonCreatorProcess);}
  Double_t GetWavelength() const  { return fWavelength; };

  ClassDef(WCSimRootCherenkovHitTime,2)
};

/**
 * \class WCSimRootCherenkovHitHistory
 * \brief Class holding scattering and reflection history for each true hit (Cherenkov photon; dark noise are all 0)
 *
 * There is one entry per hit.
 * Note that this information is not filled, unless WCSim is compiled with -DWCSIM_SAVE_PHOTON_HISTORY_FLAG=ON (default OFF)
 */
class WCSimRootCherenkovHitHistory : public TObject {

private:
  
  Int_t   fNRayScat; //!< Number of Rayliegh scatters the photon that created this hit underwent
  Int_t   fNMieScat; //!< Number of Mie scatters the photon that created this hit underwent
  std::vector<ReflectionSurface_t> fReflec; //!< Surfaces of reflections that the photon that created this hit underwent

public:
  WCSimRootCherenkovHitHistory() {}
  WCSimRootCherenkovHitHistory(Int_t nRayScat, Int_t nMieScat, std::vector<ReflectionSurface_t> refle);
  virtual ~WCSimRootCherenkovHitHistory() { }
  bool CompareAllVariables(const WCSimRootCherenkovHitHistory * c) const;

  Int_t     GetNRayScatters() const { return fNRayScat; } // Get the number of Rayleigh scattering a photon experienced
  Int_t     GetNMieScatters() const { return fNMieScat; } // Get the number of Mie scattering a photon experienced
  std::vector<ReflectionSurface_t> GetReflectionSurfaces() const { return fReflec; } //  Get the vector of reflection surfaces a photon experienced

  ClassDef(WCSimRootCherenkovHitHistory,1)
};


//////////////////////////////////////////////////////////////////////////

/**
 * \class WCSimRootCherenkovDigiHit
 * \brief Class holding digitised hit (aka digit or digi) information (after PMT & electronics simulation + triggering)
 *
 * There is one entry per digit
 */
class WCSimRootCherenkovDigiHit : public TObject {

private:
  // See jhfNtuple.h for the meaning of these data members:
  Float_t fQ; //!< Digitised charge (unit: something like p.e.)
  Double_t fT; //!< Digitised time, relative to the digit time (date in the WCSimRootEventHeader) (unit: ns)
  Int_t fTubeId; //!< PMT ID. WARNING: Runs from 1 to N (not 0 to N-1, which is how you access elements from WCSimRootGeom)
  Int_t fmPMTId; //!< The mPMT number this 3" PMT is in (Equivalent to fTubeId for 20" & OD PMTs)
  Int_t fmPMT_PMTId; //!< The 3" PMT position ID inside this mPMT (runs from 1-19 for 3" PMTs inside mPMTs. Is 0 for 20" & OD PMTs)
  std::vector<int> fPhotonIds; //!< Truth matching. Position(s) in the CherenkovHitTime array that contains the true hit(s) that created this digit

public:
  WCSimRootCherenkovDigiHit() {}
  WCSimRootCherenkovDigiHit(Double_t q, Double_t t, Int_t tubeid, std::vector<int> photon_ids);
  WCSimRootCherenkovDigiHit(Double_t q, Double_t t, Int_t tubeid, Int_t mpmtid, Int_t mpmt_pmtid, std::vector<int> photon_ids);

  virtual ~WCSimRootCherenkovDigiHit() { }
  bool CompareAllVariables(const WCSimRootCherenkovDigiHit * c) const;

  void SetT(float t) { fT = t; }
  void SetPhotonIds(std::vector<int> photon_ids) { fPhotonIds = photon_ids; }

  Float_t     GetQ() const { return fQ;}
  Double_t    GetT() const { return fT;}
  Int_t       GetTubeId() const { return fTubeId;}
  Int_t       GetmPMTId() const { return fmPMTId;}
  Int_t       GetmPMT_PMTId() const { return fmPMT_PMTId;}
  std::vector<int> GetPhotonIds() const { return fPhotonIds; }

  ClassDef(WCSimRootCherenkovDigiHit,4)
};


//////////////////////////////////////////////////////////////////////////

/**
 * \class WCSimRootEventHeader
 * \brief Class containing header information for this trigger
 */
class WCSimRootEventHeader {

private:
  Int_t   fEvtNum; //!< Event number
  Int_t   fRun; //!< Run number. Should be 0 for the first call of /run/beamOn, and increment for each subsequent call of /run/beamOn
  int64_t fDate; //!< Time (ns)
  Int_t   fSubEvtNumber; //!< Trigger number. 0 for the first trigger (or if no trigger has been found), and increments for each subsequent trigger

public:
  WCSimRootEventHeader() : fEvtNum(0), fRun(0), fDate(0), fSubEvtNumber(1) { }
  virtual ~WCSimRootEventHeader() { }
  bool CompareAllVariables(const WCSimRootEventHeader * c) const;
  void   Set(Int_t i, Int_t r, int64_t d, Int_t sub=1) { fEvtNum = i; fRun = r; fDate = d; fSubEvtNumber = sub;}
  void SetDate(int64_t d) { fDate=d; }
  Int_t  GetEvtNum() const { return fEvtNum; }
  Int_t  GetRun() const { return fRun; }
  int64_t GetDate() const { return fDate; }
  Int_t GetSubEvtNumber() const { return fSubEvtNumber;}
  

  ClassDef(WCSimRootEventHeader,3)  //WCSimRootEvent Header
};

//////////////////////////////////////////////////////////////////////////

/**
 * \class WCSimRootPi0
 * \brief Class storing information about pi0 decays
 */
class WCSimRootPi0 : public TObject {
    // this is a class used specifically for Pi0 events

private:
  Float_t fPi0Vtx[3]; //!< pi0 vertex (x,y,z)
  Int_t   fGammaID[2]; //!< Truth matching. Track ID of each gamma
  Float_t fGammaE[2]; //!< Starting energy of each gamma
  Float_t fGammaVtx[2][3]; //!< Stopping position of each gamma (x,y,z)

public:
  WCSimRootPi0() {}

  virtual ~WCSimRootPi0() {}

  bool CompareAllVariables(const WCSimRootPi0 * c) const;

  void Set(Double_t pi0Vtx[3],
	   Int_t   gammaID[2],
	   Double_t gammaE[2],
	   Double_t gammaVtx[2][3]);

  Float_t  GetPi0Vtx(int i)           const { return (i<3) ? fPi0Vtx[i]: 0;}
  Int_t    GetGammaID(int i)          const { return (i<2) ? fGammaID[i]: 0;}
  Float_t  GetGammaE(int i)           const { return (i<2) ? fGammaE[i]: 0;}
  Float_t  GetGammaVtx(int i, int j)  const { return fGammaVtx[i][j];}

  ClassDef(WCSimRootPi0,2)
};

//////////////////////////////////////////////////////////////////////////

/**
 * \class WCSimRootCaptureGamma
 * \brief Class storing information about gammas released from neutron capture
 */
class WCSimRootCaptureGamma : public TObject {

private:
  Int_t   fID; //!<  Truth matching. Track ID of the gamma
  Float_t fEnergy; //!< Energy of the gamma (unit: MeV)
  Float_t fDir[3]; //!< Direction unit vector (x,y,z) of the gamma

public:
  WCSimRootCaptureGamma() {}
  WCSimRootCaptureGamma(Int_t id,
			Double_t energy,
			Double_t dir[3]
			);

  virtual ~WCSimRootCaptureGamma() {}
  bool CompareAllVariables(const WCSimRootCaptureGamma * c) const;

  Int_t    GetID()           const { return fID;}
  Float_t  GetE()            const { return fEnergy;}
  Float_t  GetDir(int i)     const { return (i<3) ? fDir[i]: 0;}

  ClassDef(WCSimRootCaptureGamma,2)
};

//////////////////////////////////////////////////////////////////////////

/**
 * \class WCSimRootCapture
 * \brief Class used specifically for neutron capture events
 */
class WCSimRootCapture : public TObject {


private:
  Int_t   	   fCaptureParent; //!< Track ID of the parent neutron
  Float_t 	   fCaptureVtx[3]; //!< Position of the neutron capture (x,y,z) (unit: cm)
  Int_t   	   fNGamma; //!< Number of gammas produced by the neutron capture
  Float_t 	   fTotalGammaE; //!< Total gamma energy produced by the neutron capture (unit: MeV)
  Float_t 	   fCaptureT; //!< Time of the neutron capture (unit: ns)
  Int_t          fCaptureNucleus; //!< Nucleus the neutron captured on
  TClonesArray * fGammas; //!< Array of WCSimRootCaptureGamma storing detailed information about individual gammas released from the neutron capture
  bool 	   IsZombie; //!< Will be true if unfilled, false if filled

public:
  WCSimRootCapture() {
    fGammas = 0;
    IsZombie = true;
  }
  WCSimRootCapture(Int_t captureParent);

  virtual ~WCSimRootCapture();
  bool CompareAllVariables(const WCSimRootCapture * c) const;

  void SetInfo(Double_t captureVtx[3],
	       Double_t captureT,
	       Int_t   captureNucleus
	       );

  void AddGamma(Int_t   gammaID,
		Double_t gammaE,
		Double_t gammaDir[3]
		);

  Int_t         GetCaptureParent()   const { return fCaptureParent;}
  Float_t       GetCaptureVtx(int i) const { return (i<3) ? fCaptureVtx[i]: 0;}
  Int_t         GetNGamma()          const { return fNGamma;}
  Float_t       GetTotalGammaE()     const { return fTotalGammaE;}
  Float_t       GetCaptureT()        const { return fCaptureT;}
  Int_t         GetCaptureNucleus()  const { return fCaptureNucleus;}
  TClonesArray* GetGammas()          const { return fGammas;}

  ClassDef(WCSimRootCapture,2)
};

//////////////////////////////////////////////////////////////////////////

/**
 * \class WCSimRootTrigger
 * \brief Class storing trigger information
 *
 * Digitised hit information will be put in the WCSimRootTrigger it is associated with
 * (if there are multiple overlapping trigger readout windows, digits will only be saved in the first
 * trigger window they are associated with, to avoid double counting)
 *
 * True information (vertices, tracks, hits) are mostly put in the 0th WCSimRootTrigger
 */
class WCSimRootTrigger : public TObject {

private:
  WCSimRootEventHeader    fEvtHdr;  //!< The header
  // See jhfNtuple.h for the meaning of these data members:
  Int_t                fMode[MAX_N_VERTICES]; //!< Interaction mode for each vertex. Only set for the muline generator
  Int_t                fNvtxs; //!< Number of true vertices in the event
  Int_t                fVtxsvol[MAX_N_VERTICES]; //!< The detector volume this vertex occured in
  Float_t              fVtxs[MAX_N_VERTICES][4]; //!< True 4-position (x,y,z,t) of the vertex
  Int_t                fVecRecNumber;       //!< "info event" number in inputvectorfile. Only set for the muline generator
  Int_t                fJmu; //!< LEGACY VARIABLE MAY BE INACCURATE IN SOME RUNNING SCENARIOS. Index of the muon
  Int_t                fJp; //!< LEGACY VARIABLE MAY BE INACCURATE IN SOME RUNNING SCENARIOS. Index of the proton

  WCSimRootPi0         fPi0;                //!< Pi0 info (default = not used)

  TClonesArray         *fCaptures;           //!< Neutron capture info
  Int_t                fNcaptures;           //!< Number of tracks in the neutron capture array

  Int_t                fNpar;               //!< LEGACY VARIABLE MAY BE INACCURATE IN SOME RUNNING SCENARIOS. Index Number of tracks in the event (not all will be saved in the WCSimRootTrack array)
  Int_t                fNtrack;             //!< Number of tracks in the WCSimRootTrack array
  Int_t                fNtrack_slots;       //!< Number of slots in the WCSimRootTrack array. This is potentially more than fNtrack (i.e. if any tracks have been removed that aren't at the very start/end)
  TClonesArray         *fTracks;            //!< Array of WCSimRootTracks 

  Int_t                fNumTubesHit;         //!< Number of tubes hit
  Int_t                fNcherenkovhits;      //!< Number of hits in the array
  TClonesArray         *fCherenkovHits;      //!< Array of WCSimRootCherenkovHits

  Int_t                fCherenkovHitCounter; //!< Incremented with every call of WCSimRootTrigger::AddCherenkovHit(). Not initialised to 0. Not used. Inaccessible publicly. Avoid
  Int_t                fNcherenkovhittimes;      //!< Number of hits in the WCSimRootCherenkovHits array
  TClonesArray         *fCherenkovHitTimes;      //!< Array of WCSimRootCherenkovHits
  Int_t                fNcherenkovhithistories;      //!< Number of hits in the WCSimRootCherenkovHitHistories array. Should be identical to fNcherenkovhittimes
  TClonesArray         *fCherenkovHitHistories;  //!< Array of WCSimRootCherenkovHitHistories

  Int_t                fNumDigitizedTubes;  //!< Number of digitized tubes
  Int_t                fNcherenkovdigihits;  //!< Number of digihits in the WCSimRootCherenkovDigiHit array
  Int_t                fNcherenkovdigihits_slots;  //!< Number of slots in the WCSimRootCherenkovDigiHit array. This is potentially more than fNcherenkovdigihits (i.e. if any digihits have been removed that aren't at the very start/end)
  Float_t              fSumQ; //!< Sum of digitised hit charge in this trigger
  TClonesArray         *fCherenkovDigiHits;  //!< Array of WCSimRootCherenkovDigiHit's

  TriggerType_t        fTriggerType;         //!< Trigger algorithm that created this trigger
  std::vector<Double_t> fTriggerInfo;        //!< Information about how it passed the trigger (e.g. how many hits in the NDigits window)

  bool IsZombie; //!< Will be true if unfilled, false if filled 

public:
  WCSimRootTrigger();
  WCSimRootTrigger(int, int);
  virtual ~WCSimRootTrigger();
  WCSimRootTrigger & operator=(const WCSimRootTrigger & in);
  bool CompareAllVariables(const WCSimRootTrigger * c, bool deep_comparison = false) const;
  
  void Initialize();

  void          Clear(Option_t *option ="");
  static void   Reset(Option_t *option ="");

  void          SetHeader(Int_t i, Int_t run, int64_t date,Int_t subevtn=1);
  void          SetTriggerInfo(TriggerType_t trigger_type, std::vector<Double_t> trigger_info);
  bool          IsASubEvent() {  return (fEvtHdr.GetSubEvtNumber()>=1); }
  void          SetMode(Int_t i) {fMode[0] = i;}
  void          SetMode(Int_t index, Int_t value){fMode[index]=value;}
  void          SetNvtxs(Int_t i) {fNvtxs = i;}
  void          SetVtxvol(Int_t i) {fVtxsvol[0] = i;}
  void          SetVtxsvol(Int_t i, Int_t v) {fVtxsvol[i] = v;}
  void          SetVtx(Int_t i, Double_t f) {SetVtxs(0,i,f);}
  void          SetVtxs(Int_t n, Int_t i, Double_t f) {fVtxs[n][i]= ( (i<4) ? f : 0);}
  void          SetVecRecNumber(Int_t i) {fVecRecNumber = i;}
  void          SetJmu(Int_t i) {fJmu = i;}
  void          SetJp(Int_t i) {fJp = i;}
  void          SetNpar(Int_t i) {fNpar = i;}
  void          SetNumTubesHit(Int_t i) {fNumTubesHit = i;}
  void          SetSumQ(Double_t x){fSumQ = x;}
  void          SetNumDigitizedTubes(Int_t i) {fNumDigitizedTubes = i;}
  void          SetPi0Info(Double_t pi0Vtx[3],
			   Int_t   gammaID[2],
			   Double_t gammaE[2],
			   Double_t gammaVtx[2][3]);
  void          SetCaptureParticle(Int_t parent,
                                   Int_t ipnu,
                                   Double_t time,
                                   Double_t vtx[3],
                                   Double_t dir[3],
                                   Double_t energy,
                                   Int_t id);


  WCSimRootEventHeader *GetHeader()               {return &fEvtHdr; }
  const WCSimRootEventHeader * GetHeader()     const {return &fEvtHdr; }
  WCSimRootPi0       *GetPi0Info()                 {return &fPi0; }
  const WCSimRootPi0         * GetPi0Info()    const {return &fPi0; }
  Int_t               GetMode()               const {return fMode[0];}
  Int_t               GetMode(Int_t index)    const {return fMode[index];}
  Int_t               GetVtxvol()             const {return fVtxsvol[0];}
  Float_t             GetVtx(Int_t i=0)             {return (i<3) ? fVtxs[0][i]: 0;}
  TVector3            GetVertex(Int_t i)       {return TVector3(fVtxs[i][0],fVtxs[i][1],fVtxs[i][2]);}
  Int_t               GetNvtxs()             const {return fNvtxs;}
  Int_t               GetVtxsvol(Int_t i)             const {return fVtxsvol[i];}
  Float_t             GetVtxs(Int_t n, Int_t i=0)     const {return (i<3) ? fVtxs[n][i]: 0;}
  TLorentzVector      Get4Vertex(Int_t i)   {return TLorentzVector(fVtxs[i][0],fVtxs[i][1],fVtxs[i][2],fVtxs[i][3]);}
  Int_t               GetVecRecNumber()       const {return fVecRecNumber;}
  Int_t               GetJmu()                const {return fJmu;}
  Int_t               GetJp()                 const {return fJp;}
  Int_t               GetNpar()               const {return fNpar;}
  Int_t               GetNumTubesHit()        const {return fNumTubesHit;}
  Int_t               GetNumDigiTubesHit()    const {return fNumDigitizedTubes;}
  Int_t               GetNtrack()             const {return fNtrack; }
  Int_t               GetNcaptures()          const {return fNcaptures; }
  Int_t               GetNtrack_slots()       const {return fTracks->GetLast() + 1; } //don't use fNtrack_slots directly as it doesn't take into account automatic TClonesArray shortening when tracks at start/end are removed
  Int_t               GetNcherenkovhits()     const {return fNcherenkovhits; }
  Int_t               GetNcherenkovhittimes() const {return fNcherenkovhittimes;}
  Int_t               GetNcherenkovhithistories() const {return fNcherenkovhithistories;}
  Int_t               GetNcherenkovdigihits() const {return fNcherenkovdigihits;}
  Int_t               GetNcherenkovdigihits_slots() const {return fCherenkovDigiHits->GetLast() + 1; } //don't use fNcherenkovdigihits_slots directly as it doesn't take into account automatic TClonesArray shortening when digits at start/end are removed
  Float_t             GetSumQ()               const { return fSumQ;}
  TriggerType_t       GetTriggerType()        const { return fTriggerType;}
  std::vector<Double_t> GetTriggerInfo()       const { return fTriggerInfo;}

  WCSimRootTrack         *AddTrack(Int_t ipnu, 
				   Int_t flag, 
				   Double_t m,
				   Double_t p,
				   Double_t E,
				   Int_t startvol, 
				   Int_t stopvol, 
				   Double_t dir[3],
				   Double_t pdir[3],
				   Double_t stop[3],
				   Double_t start[3],
				   Int_t parenttype,
           ProcessType_t creatorProcess,
				   Double_t time,
				   Int_t id,
				   Int_t idParent,
           std::vector<std::vector<float>> bPs,
           std::vector<float> bKEs,
           std::vector<double> bTimes,
           std::vector<int> bTypes);

  WCSimRootTrack * AddTrack   (WCSimRootTrack * track);
  WCSimRootTrack * RemoveTrack(WCSimRootTrack * track);

  TClonesArray        *GetTracks() const {return fTracks;}

  WCSimRootCherenkovHit   *AddCherenkovHit(Int_t                tubeID,
					   Int_t                mPMTID,
					   Int_t                mPMT_PMTID,
					   std::vector<Double_t>  truetime,
					   std::vector<Int_t>     primParID,
					   std::vector<Float_t>   photonStartTime,
					   std::vector<TVector3>  photonStartPos,
					   std::vector<TVector3>  photonEndPos,
					   std::vector<TVector3>  photonStartDir,
					   std::vector<TVector3>  photonEndDir,
					   std::vector<ProcessType_t> photonCreatorProcess,
					   std::vector<Double_t> wavelength);

  WCSimRootCherenkovHitHistory   *AddCherenkovHitHistory(Int_t nRayScat,
             Int_t nMieScat,
					   std::vector<ReflectionSurface_t> reflec);
  TClonesArray        *GetCherenkovHits() const {return fCherenkovHits;}
  TClonesArray        *GetCherenkovHitTimes() const {return fCherenkovHitTimes;}
  TClonesArray        *GetCherenkovHitHistories() const {return fCherenkovHitHistories;}

  WCSimRootCherenkovDigiHit   *AddCherenkovDigiHit(Double_t q,
						   Double_t t,
						   Int_t tubeid,
						   Int_t mpmtid,
						   Int_t mpmt_pmtid,
						   std::vector<int> photon_ids);

  WCSimRootCherenkovDigiHit * AddCherenkovDigiHit(WCSimRootCherenkovDigiHit * digit);
  WCSimRootCherenkovDigiHit * RemoveCherenkovDigiHit(WCSimRootCherenkovDigiHit * digit);
  TClonesArray            *GetCherenkovDigiHits() const {return fCherenkovDigiHits;}

  TClonesArray	      *GetCaptures() const {return fCaptures;}

  ClassDef(WCSimRootTrigger,6) //WCSimRootEvent structure
};

/**
 * \class WCSimRootEvent
 * \brief Class containing event information
 *
 * Only events from a single PMT type (20" PMT OR 3" PMT in mPMT OR OD PMT) are available
 * in a single WCSimRootEvent
 *
 * See the macro sample-root-scripts/sample_readfile.C for an example of using this class
 */
class WCSimRootEvent : public TObject {
public:
  WCSimRootEvent();
  virtual ~WCSimRootEvent();
  WCSimRootEvent & operator=(const WCSimRootEvent & in);
  bool CompareAllVariables(const WCSimRootEvent * c, bool deep_comparison = false) const;

  void          Clear(Option_t *option ="");
  static void   Reset(Option_t *option ="");
  Int_t GetCurrentIndex() { return Current;}

  //  WCSimRootTrigger* GetTrigger(int number) { return fEventList[number];}
        WCSimRootTrigger * GetTrigger(int number)       { return (WCSimRootTrigger*) (*fEventList)[number];}
  const WCSimRootTrigger * GetTrigger(int number) const { return (WCSimRootTrigger*) (*fEventList)[number];}

  Int_t GetNumberOfEvents() const { return fEventList->GetEntriesFast();}
  Int_t GetNumberOfSubEvents() const { return (fEventList->GetEntriesFast()-1);}
  bool HasSubEvents() { return  (fEventList->GetEntriesFast() > 1); } 

  //Int_t GetNumberOfEvents() const { return fEventList.size();}
  //Int_t GetNumberOfSubEvents() const { return (fEventList.size()-1);}

  //void AddSubEvent() { fEventList.push_back(new WCSimRootTrigger()); }
  void AddSubEvent() { 
    // be sure not to call the default constructor BUT the actual one
    WCSimRootTrigger* tmp = dynamic_cast<WCSimRootTrigger*>( (*fEventList)[0] );
    int num = tmp->GetHeader()->GetEvtNum();
    ++Current; 

    //Initialize() creates array with 10 entries.
    // Using AddAtAndExpand() doubles the array size whenever
    // we hit the current array size.
    // This ensures we can keep adding entries,
    // even in the case of huge events
    // e.g. if we have a close SN
    fEventList->AddAtAndExpand(new WCSimRootTrigger(num,Current),Current);
  }
  
  /*  void ReInitialize() { // need to remove all subevents at the end, or they just get added anyway...
    std::vector<WCSimRootTrigger*>::iterator  iter = fEventList.begin();
    ++iter; // do not delete the first event --> regular beaviour for this program ?
  */
  void Initialize();

  void ReInitialize() { // need to remove all subevents at the end, or they just get added anyway...
    for ( int i = fEventList->GetLast() ; i>=1 ; i--) {
      //      G4cout << "removing element # " << i << "...";
      WCSimRootTrigger* tmp = 
	dynamic_cast<WCSimRootTrigger*>(fEventList->RemoveAt(i));
      delete tmp;
      //G4cout <<"done !\n";
    }
    Current = 0;
    WCSimRootTrigger* tmp = dynamic_cast<WCSimRootTrigger*>( (*fEventList)[0]);
    tmp->Clear();
  }

private:
  //std::vector<WCSimRootTrigger*> fEventList;
  TObjArray* fEventList; //!< Array of WCSimRootTrigger
  Int_t Current;                      //!               means transient, not writable to file
  ClassDef(WCSimRootEvent,3)

};

#endif
