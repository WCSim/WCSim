#ifndef WCSim_RootEvent
#define WCSim_RootEvent

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//    WCSim_RootEvent                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include <string>
#include <vector>
#include "TVector3.h"
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
  Double_t fM;
  Double_t fP;
  Double_t fE;
  Int_t   fStartvol;
  Int_t   fStopvol;
  Double_t fDir[3];
  Double_t fPdir[3];
  Double_t fStop[3];
  Double_t fStart[3];
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

  Int_t     GetIpnu() const { return fIpnu;}
  Int_t     GetFlag() const { return fFlag;}
  Double_t   GetM() const { return fM;}
  Double_t   GetP() const { return fP;}
  Double_t   GetE() const { return fE;}
  Int_t     GetStartvol() { return fStartvol;}
  Int_t     GetStopvol() { return fStopvol;}
  Double_t   GetDir(Int_t i=0) {return (i<3) ? fDir[i] : 0;} 
  Double_t   GetPdir(Int_t i=0) {return (i<3) ? fPdir[i] : 0;}
  Double_t   GetStop(Int_t i=0) {return (i<3) ? fStop[i] : 0;}
  Double_t   GetStart(Int_t i=0) {return (i<3) ? fStart[i] : 0;}
  Int_t     GetParenttype(/*Int_t i=0*/) {return fParenttype;}
  Double_t   GetTime() { return fTime;}
  Int_t     GetId(){return fId;}

  ClassDef(WCSimRootTrack,1)  
};


//////////////////////////////////////////////////////////////////////////


class WCSimRootCherenkovHit : public TObject {

private:
  Int_t fTubeID;
  Int_t fmPMTID;
  Int_t fmPMT_PMTID;
  Int_t fTotalPe[2];

public:
  WCSimRootCherenkovHit() {}
  WCSimRootCherenkovHit(Int_t tubeID,
			Int_t totalPe[2]);

  WCSimRootCherenkovHit(Int_t tubeID,
			Int_t mPMTID,
			Int_t mPMT_PMTID,
			Int_t totalPe[2]);

  virtual ~WCSimRootCherenkovHit() { }

  Int_t GetTubeID()       const { return fTubeID;}
  Int_t GetmPMTID()       const { return fmPMTID;}
  Int_t GetmPMT_PMTID()       const { return fmPMT_PMTID;}
  Int_t GetTotalPe(int i) const { return (i<2) ? fTotalPe[i]: 0;}

  ClassDef(WCSimRootCherenkovHit,2)  
};

class WCSimRootCherenkovHitTime : public TObject {

private:
  // See jhfNtuple.h for the meaning of these data members:
  Double_t fTruetime;
  Int_t   fPrimaryParentID;
  Float_t fPhotonStartTime;
  Float_t fPhotonStartPos[3];
  Float_t fPhotonEndPos[3];

public:
  WCSimRootCherenkovHitTime() {}
  WCSimRootCherenkovHitTime(Double_t truetime,
			    Int_t   primaryParentID,
			    Float_t photonStartTime,
			    Float_t photonStartPos[3],
			    Float_t photonEndPos[3]);
  virtual ~WCSimRootCherenkovHitTime() { }

  Double_t   GetTruetime() { return fTruetime;}
  Int_t     GetParentID() { return fPrimaryParentID;}
  Float_t   GetPhotonStartTime() { return fPhotonStartTime; }
  Float_t   GetPhotonStartPos(int i) { return (i<3) ? fPhotonStartPos[i] : 0; }
  Float_t   GetPhotonEndPos(int i) { return (i<3) ? fPhotonEndPos[i] : 0; }

  ClassDef(WCSimRootCherenkovHitTime,1)  
};


//////////////////////////////////////////////////////////////////////////


class WCSimRootCherenkovDigiHit : public TObject {

private:
  // See jhfNtuple.h for the meaning of these data members:
  Double_t fQ;
  Double_t fT;
  Int_t fTubeId;
  Int_t fmPMTId;
  Int_t fmPMT_PMTId;
  std::vector<int> fPhotonIds;

public:
  WCSimRootCherenkovDigiHit() {}
  WCSimRootCherenkovDigiHit(Double_t q, Double_t t, Int_t tubeid, std::vector<int> photon_ids);
  WCSimRootCherenkovDigiHit(Double_t q, Double_t t, Int_t tubeid, Int_t mpmtid, Int_t mpmt_pmtid, std::vector<int> photon_ids);

  virtual ~WCSimRootCherenkovDigiHit() { }

  Double_t     GetQ() const { return fQ;}
  Double_t     GetT() const { return fT;}
  Int_t       GetTubeId() const { return fTubeId;}
  Int_t       GetmPMTId() const { return fmPMTId;}
  Int_t       GetmPMT_PMTId() const { return fmPMT_PMTId;}
  std::vector<int> GetPhotonIds() const { return fPhotonIds; }

  ClassDef(WCSimRootCherenkovDigiHit,3)  
};


//////////////////////////////////////////////////////////////////////////

class WCSimRootEventHeader {

private:
  Int_t   fEvtNum;
  Int_t   fRun;
  Int_t   fDate;
  Int_t   fSubEvtNumber;

public:
  WCSimRootEventHeader() : fEvtNum(0), fRun(0), fDate(0), fSubEvtNumber(1) { }
   virtual ~WCSimRootEventHeader() { }
  void   Set(Int_t i, Int_t r, Int_t d, Int_t s=1) { fEvtNum = i; fRun = r; fDate = d; fSubEvtNumber = s;}
  void SetDate(Int_t d) { fDate=d; }
   Int_t  GetEvtNum() const { return fEvtNum; }
   Int_t  GetRun() const { return fRun; }
   Int_t  GetDate() const { return fDate; }
  Int_t GetSubEvtNumber() const { return fSubEvtNumber;}
  

   ClassDef(WCSimRootEventHeader,2)  //WCSimRootEvent Header
};

//////////////////////////////////////////////////////////////////////////

class WCSimRootPi0 : public TObject {
    // this is a class used specifically for Pi0 events

private:
    Double_t fPi0Vtx[3];
    Int_t   fGammaID[2];
    Double_t fGammaE[2];
    Double_t fGammaVtx[2][3];

public:
    WCSimRootPi0() {}

    virtual ~WCSimRootPi0() {}

    void Set(Double_t pi0Vtx[3],
	     Int_t   gammaID[2],
	     Double_t gammaE[2],
	     Double_t gammaVtx[2][3]);

    Double_t  GetPi0Vtx(int i)           const { return (i<3) ? fPi0Vtx[i]: 0;}
    Int_t    GetGammaID(int i)          const { return (i<2) ? fGammaID[i]: 0;}
    Double_t  GetGammaE(int i)           const { return (i<2) ? fGammaE[i]: 0;}
    Double_t  GetGammaVtx(int i, int j)  const { return fGammaVtx[i][j];}

ClassDef(WCSimRootPi0,1)
};

//////////////////////////////////////////////////////////////////////////

class WCSimRootCaptureGamma : public TObject {

private:
    Int_t   fID;
    Double_t fEnergy;
    Double_t fDir[3];

public:
    WCSimRootCaptureGamma() {}
    WCSimRootCaptureGamma(Int_t id,
                          Double_t energy,
                          Double_t dir[3]
    );

    virtual ~WCSimRootCaptureGamma() {}

    Int_t    GetID()           const { return fID;}
    Double_t  GetE()            const { return fEnergy;}
    Double_t  GetDir(int i)     const { return (i<3) ? fDir[i]: 0;}

ClassDef(WCSimRootCaptureGamma,1)
};

//////////////////////////////////////////////////////////////////////////

class WCSimRootCapture : public TObject {
    // this is a class used specifically for neutron capture events

private:
    Int_t   	   fCaptureParent;
    Double_t 	   fCaptureVtx[3];
    Int_t   	   fNGamma;
    Double_t 	   fTotalGammaE;
    Double_t 	   fCaptureT;
    Int_t          fCaptureNucleus;
    TClonesArray * fGammas;
    bool 	   IsZombie;

public:
    WCSimRootCapture() {
      fGammas = 0;
      IsZombie = true;
    }
    WCSimRootCapture(Int_t captureParent);

    virtual ~WCSimRootCapture();

    void SetInfo(Double_t captureVtx[3],
	         Double_t captureT,
		 Int_t   captureNucleus
    );

    void AddGamma(Int_t   gammaID,
		  Double_t gammaE,
		  Double_t gammaDir[3]
    );

    Int_t                   GetCaptureParent()   const { return fCaptureParent;}
    Double_t                 GetCaptureVtx(int i) const { return (i<3) ? fCaptureVtx[i]: 0;}
    Int_t                   GetNGamma()          const { return fNGamma;}
    Double_t                 GetTotalGammaE()     const { return fTotalGammaE;}
    Double_t                 GetCaptureT()        const { return fCaptureT;}
    Int_t                   GetCaptureNucleus()  const { return fCaptureNucleus;}
    TClonesArray	   *GetGammas()          const { return fGammas;}

ClassDef(WCSimRootCapture,1)
};

//////////////////////////////////////////////////////////////////////////

class WCSimRootTrigger : public TObject {

private:
  WCSimRootEventHeader    fEvtHdr;  // The header
  // See jhfNtuple.h for the meaning of these data members:
  Int_t                fMode;
  Int_t                fVtxvol;
  Double_t              fVtx[3];
  Int_t                fVecRecNumber;       // "info event" number in inputvectorfile 
  Int_t                fJmu;
  Int_t                fJp;

  WCSimRootPi0        fPi0;                // Pi0 info (default = not used)

  TClonesArray        *fCaptures;            // Neutron capture info (default = not used)
  Int_t                fNcaptures;             // Number of tracks in the array

  Int_t                fNpar;               // Number of particles
  Int_t                fNtrack;             // Number of tracks in the array
  TClonesArray         *fTracks;            //-> Array of WCSimRootTracks 

  Int_t                fNumTubesHit;        // Number of tubes hit
  Int_t                fNcherenkovhits;      // Number of hits in the array
  TClonesArray         *fCherenkovHits;      //-> Array of WCSimRootCherenkovHits

  Int_t                fCherenkovHitCounter;
  Int_t                fNcherenkovhittimes;      // Number of hits in the array
  TClonesArray         *fCherenkovHitTimes;      //-> Array of WCSimRootCherenkovHits

  Int_t                fNumDigitizedTubes;  // Number of digitized tubes
  Int_t                fNcherenkovdigihits;  // Number of digihits in the array
  Double_t              fSumQ;
  TClonesArray         *fCherenkovDigiHits;  //-> Array of WCSimRootCherenkovDigiHit's

  TriggerType_t        fTriggerType;         // Trigger algorithm that created this trigger
  std::vector<Double_t> fTriggerInfo;         // Information about how it passed the trigger (e.g. how many hits in the NDigits window)

  bool IsZombie;

public:
  WCSimRootTrigger();
  WCSimRootTrigger(int, int);
  virtual ~WCSimRootTrigger();
  
  void Initialize();

  void          Clear(Option_t *option ="");
  static void   Reset(Option_t *option ="");

  void          SetHeader(Int_t i, Int_t run, Int_t date,Int_t subevtn=1);
  void          SetTriggerInfo(TriggerType_t trigger_type, std::vector<Double_t> trigger_info);
  bool          IsASubEvent() {  return (fEvtHdr.GetSubEvtNumber()>=1); }
  void          SetMode(Int_t i) {fMode = i;}
  void          SetVtxvol(Int_t i) {fVtxvol = i;}
  void          SetVtx(Int_t i, Double_t f) {fVtx[i]= ( (i<3) ? f : 0);}
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


  WCSimRootEventHeader *GetHeader()                 {return &fEvtHdr; }
  WCSimRootPi0       *GetPi0Info()                  {return &fPi0; }
  Int_t               GetMode()               const {return fMode;}
  Int_t               GetVtxvol()             const {return fVtxvol;}
  Double_t             GetVtx(Int_t i=0)             {return (i<3) ? fVtx[i]: 0;}
  Int_t               GetVecRecNumber()       const {return fVecRecNumber;}
  Int_t               GetJmu()                const {return fJmu;}
  Int_t               GetJp()                 const {return fJp;}
  Int_t               GetNpar()               const {return fNpar;}
  Int_t               GetNumTubesHit()        const {return fNumTubesHit;}
  Int_t               GetNumDigiTubesHit()    const {return fNumDigitizedTubes;}
  Int_t               GetNtrack()             const {return fNtrack; }
  Int_t               GetNcaptures()          const {return fNcaptures; }
  Int_t               GetNcherenkovhits()     const {return fNcherenkovhits; }
  Int_t               GetNcherenkovhittimes() const {return fNcherenkovhittimes;}
  Int_t               GetNcherenkovdigihits() const {return fNcherenkovdigihits;}
  Double_t             GetSumQ()               const { return fSumQ;}
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
				   Double_t time,
				   Int_t id);

  TClonesArray        *GetTracks() const {return fTracks;}

  WCSimRootCherenkovHit   *AddCherenkovHit(Int_t                tubeID,
					   Int_t                mPMTID,
					   Int_t                mPMT_PMTID,
					  std::vector<Double_t> truetime,
					  std::vector<Int_t>   primParID,
					  std::vector<Float_t>   photonStartTime,
					  std::vector<TVector3>   photonStartPos,
					  std::vector<TVector3>   photonEndPos);
  TClonesArray        *GetCherenkovHits() const {return fCherenkovHits;}
  TClonesArray        *GetCherenkovHitTimes() const {return fCherenkovHitTimes;}

  WCSimRootCherenkovDigiHit   *AddCherenkovDigiHit(Double_t q, 
						   Double_t t, 
						   Int_t tubeid,
						   Int_t mpmtid,
						   Int_t mpmt_pmtid,
						   std::vector<int> photon_ids);
//  WCSimRootCherenkovDigiHit   *AddCherenkovDigiHit(Double_t q, 
//						  Double_t t, 
//						  Int_t tubeid,
 //                                                 Double_t sumq);

  TClonesArray            *GetCherenkovDigiHits() const {return fCherenkovDigiHits;}

  TClonesArray	      *GetCaptures() const {return fCaptures;}


ClassDef(WCSimRootTrigger,2) //WCSimRootEvent structure
};


class WCSimRootEvent : public TObject {
public:
  WCSimRootEvent();
  virtual ~WCSimRootEvent();

  void          Clear(Option_t *option ="");
  static void   Reset(Option_t *option ="");
  Int_t GetCurrentIndex() { return Current;}

  //  WCSimRootTrigger* GetTrigger(int number) { return fEventList[number];}
  WCSimRootTrigger* GetTrigger(int number) { return (WCSimRootTrigger*) (*fEventList)[number];}

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
    if ( Current > 9 ) fEventList->Expand(150);
    fEventList->AddAt(new WCSimRootTrigger(num,Current),Current);
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
  ClassDef(WCSimRootEvent,2)

};


#endif
