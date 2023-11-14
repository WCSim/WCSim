#ifndef WCSim_RootEvent
#define WCSim_RootEvent

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//    WCSim_RootEvent                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include <string>
#include <vector>
#include "jhfNtuple.h"
//#include <map>
//#include "G4Transform3D.hh"

// #include "WCSimDetectorConstruction.hh"
#include "WCSimEnumerations.hh"

class TDirectory;


class WCSimRootTrack : public TObject {

private:

  // See jhfNtuple.h for the meaning of these data members:
  Int_t   fIpnu;        
  Int_t   fFlag;        
  Float_t fM;
  Float_t fP;
  Float_t fE;
  Int_t   fStartvol;
  Int_t   fStopvol;
  Float_t fDir[3];
  Float_t fPdir[3];
  Float_t fStop[3];
  Float_t fStart[3];
  Int_t fParenttype;
  Double_t fTime;
  Int_t fId;

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
		 Double_t time,Int_t id);
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

  ClassDef(WCSimRootTrack,1)  
};


//////////////////////////////////////////////////////////////////////////


class WCSimRootCherenkovHit : public TObject {

private:
  Int_t fTubeID;
  Int_t fTotalPe[2];

public:
  WCSimRootCherenkovHit() {}
  WCSimRootCherenkovHit(Int_t tubeID,
			Int_t totalPe[2]);

  virtual ~WCSimRootCherenkovHit() { }

  bool CompareAllVariables(const WCSimRootCherenkovHit * c) const;

  Int_t GetTubeID()       const { return fTubeID;}
  Int_t GetTotalPe(int i) const { return (i<2) ? fTotalPe[i]: 0;}

  ClassDef(WCSimRootCherenkovHit,1)  
};

class WCSimRootCherenkovHitTime : public TObject {

private:
  // See jhfNtuple.h for the meaning of these data members:
  Double_t fTruetime;
  Int_t   fPrimaryParentID;
  Double_t fWavelength;

public:
  WCSimRootCherenkovHitTime() {}
  WCSimRootCherenkovHitTime(Double_t truetime,
			    Int_t   primaryParentID,
			    Double_t wavelength);
  virtual ~WCSimRootCherenkovHitTime() { }
  bool CompareAllVariables(const WCSimRootCherenkovHitTime * c) const;

  Double_t   GetTruetime() const { return fTruetime;}
  Int_t     GetParentID() const { return fPrimaryParentID;}
  Double_t GetWavelength() const  { return fWavelength; };

  ClassDef(WCSimRootCherenkovHitTime,1)  
};


//////////////////////////////////////////////////////////////////////////


class WCSimRootCherenkovDigiHit : public TObject {

private:
  // See jhfNtuple.h for the meaning of these data members:
  Float_t fQ;
  Double_t fT;
  Int_t fTubeId;
  std::vector<int> fPhotonIds;

public:
  WCSimRootCherenkovDigiHit() {}
  WCSimRootCherenkovDigiHit(Double_t q, Double_t t, Int_t tubeid, std::vector<int> photon_ids);

  virtual ~WCSimRootCherenkovDigiHit() { }
  bool CompareAllVariables(const WCSimRootCherenkovDigiHit * c) const;

  void SetT(float t) { fT = t; }
  void SetPhotonIds(std::vector<int> photon_ids) { fPhotonIds = photon_ids; }
  Float_t     GetQ() const { return fQ;}
  Double_t     GetT() const { return fT;}
  Int_t       GetTubeId() const { return fTubeId;}
  std::vector<int> GetPhotonIds() const { return fPhotonIds; }

  ClassDef(WCSimRootCherenkovDigiHit,2)  
};


//////////////////////////////////////////////////////////////////////////

class WCSimRootEventHeader {

private:
  Int_t   fEvtNum;
  Int_t   fRun;
  int64_t fDate;
  Int_t   fSubEvtNumber;

public:
  WCSimRootEventHeader() : fEvtNum(0), fRun(0), fDate(0), fSubEvtNumber(1) { }
  virtual ~WCSimRootEventHeader() { }
  bool CompareAllVariables(const WCSimRootEventHeader * c) const;
  void   Set(Int_t i, Int_t r, int64_t d, Int_t s=1) { fEvtNum = i; fRun = r; fDate = d; fSubEvtNumber = s;}
  void SetDate(int64_t d) { fDate=d; }
  Int_t  GetEvtNum() const { return fEvtNum; }
  Int_t  GetRun() const { return fRun; }
  int64_t GetDate() const { return fDate; }
  Int_t GetSubEvtNumber() const { return fSubEvtNumber;}
  

   ClassDef(WCSimRootEventHeader,2)  //WCSimRootEvent Header
};

//////////////////////////////////////////////////////////////////////////

class WCSimRootPi0 : public TObject {
  // this is a class used specifically for Pi0 events

private:
  Float_t fPi0Vtx[3];
  Int_t   fGammaID[2];
  Float_t fGammaE[2];
  Float_t fGammaVtx[2][3];

public:
  WCSimRootPi0() {}

  virtual ~WCSimRootPi0() { }
  bool CompareAllVariables(const WCSimRootPi0 * c) const;

  void Set(Double_t pi0Vtx[3],
	   Int_t   gammaID[2],
	   Double_t gammaE[2],
	   Double_t gammaVtx[2][3]);

  Float_t  GetPi0Vtx(int i)           const { return (i<3) ? fPi0Vtx[i]: 0;}
  Int_t    GetGammaID(int i)          const { return (i<2) ? fGammaID[i]: 0;}
  Float_t  GetGammaE(int i)           const { return (i<2) ? fGammaE[i]: 0;}
  Float_t  GetGammaVtx(int i, int j)  const { return fGammaVtx[i][j];}

  ClassDef(WCSimRootPi0,1)
};

//////////////////////////////////////////////////////////////////////////

class WCSimRootTrigger : public TObject {

private:
  WCSimRootEventHeader    fEvtHdr;  // The header
  // See jhfNtuple.h for the meaning of these data members:
  Int_t                fMode[MAX_N_VERTICES];
  Int_t                fNvtxs;
  Int_t                fVtxsvol[MAX_N_VERTICES];
  Float_t              fVtxs[MAX_N_VERTICES][4];
  Int_t                fVecRecNumber;       // "info event" number in inputvectorfile 
  Int_t                fJmu;
  Int_t                fJp;

  WCSimRootPi0        fPi0;                // Pi0 info (default = not used)

  Int_t                fNpar;               // Number of particles
  Int_t                fNtrack;             // Number of tracks in the array
  Int_t                fNtrack_slots;       // Number of slots in the tracks array. This is potentially more than fNtrack (i.e. if any tracks have been removed that aren't at the very start/end)
  TClonesArray         *fTracks;            //-> Array of WCSimRootTracks 

  Int_t                fNumTubesHit;        // Number of tubes hit
  Int_t                fNcherenkovhits;      // Number of hits in the array
  TClonesArray         *fCherenkovHits;      //-> Array of WCSimRootCherenkovHits

  Int_t                fCherenkovHitCounter;
  Int_t                fNcherenkovhittimes;      // Number of hits in the array
  TClonesArray         *fCherenkovHitTimes;      //-> Array of WCSimRootCherenkovHits

  Int_t                fNumDigitizedTubes;  // Number of digitized tubes
  Int_t                fNcherenkovdigihits;  // Number of digihits in the array
  Int_t                fNcherenkovdigihits_slots;  // Number of slots in the digihits array. This is potentially more than fNcherenkovdigihits (i.e. if any digihits have been removed that aren't at the very start/end)
  Float_t              fSumQ;
  TClonesArray         *fCherenkovDigiHits;  //-> Array of WCSimRootCherenkovDigiHit's

  TriggerType_t        fTriggerType;         // Trigger algorithm that created this trigger
  std::vector<Float_t> fTriggerInfo;         // Information about how it passed the trigger (e.g. how many hits in the NDigits window)

  bool IsZombie;

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
  void          SetTriggerInfo(TriggerType_t trigger_type, std::vector<Float_t> trigger_info);
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
  Int_t               GetNtrack_slots()       const {return fTracks->GetLast() + 1; } //don't use fNtrack_slots directly as it doesn't take into account automatic TClonesArray shortening when tracks at start/end are removed
  Int_t               GetNcherenkovhits()     const {return fNcherenkovhits; }
  Int_t               GetNcherenkovhittimes() const {return fNcherenkovhittimes;}
  Int_t               GetNcherenkovdigihits() const {return fNcherenkovdigihits;}
  Int_t               GetNcherenkovdigihits_slots() const {return fCherenkovDigiHits->GetLast() + 1; } //don't use fNcherenkovdigihits_slots directly as it doesn't take into account automatic TClonesArray shortening when digits at start/end are removed
  Float_t             GetSumQ()               const { return fSumQ;}
  TriggerType_t       GetTriggerType()        const { return fTriggerType;}
  std::vector<Float_t> GetTriggerInfo()        const { return fTriggerInfo;}

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
				   Double_t time,
				   Int_t id);

  WCSimRootTrack * AddTrack   (WCSimRootTrack * track);
  WCSimRootTrack * RemoveTrack(WCSimRootTrack * track);

  TClonesArray        *GetTracks() const {return fTracks;}

  WCSimRootCherenkovHit   *AddCherenkovHit(Int_t                tubeID,
					  std::vector<Double_t> truetime,
					   std::vector<Int_t>   primParID,
					   std::vector<Double_t> wavelength);
  TClonesArray        *GetCherenkovHits() const {return fCherenkovHits;}
  TClonesArray        *GetCherenkovHitTimes() const {return fCherenkovHitTimes;}

  WCSimRootCherenkovDigiHit   *AddCherenkovDigiHit(Double_t q,
						   Double_t t,
						   Int_t tubeid,
						   std::vector<int> photon_ids);
//  WCSimRootCherenkovDigiHit   *AddCherenkovDigiHit(Double_t q,
//						  Double_t t,
//						  Int_t tubeid,
 //                                                 Double_t sumq);
  WCSimRootCherenkovDigiHit * AddCherenkovDigiHit(WCSimRootCherenkovDigiHit * digit);
  WCSimRootCherenkovDigiHit * RemoveCherenkovDigiHit(WCSimRootCherenkovDigiHit * digit);
  TClonesArray            *GetCherenkovDigiHits() const {return fCherenkovDigiHits;}

  ClassDef(WCSimRootTrigger,3) //WCSimRootEvent structure
};


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
  TObjArray* fEventList;
  Int_t Current;                      //!               means transient, not writable to file
  ClassDef(WCSimRootEvent,1)

};

#endif
