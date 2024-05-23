// Based on Root test Event.cxx
////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include "TObject.h"
#include "TDirectory.h"
#include "TProcessID.h"
#include <string>
#include <vector>
#include <typeinfo>

#include <TStopwatch.h>
#include "WCSimRootEvent.hh"
#include "WCSimRootTools.hh"

using std::cout;
using std::cerr;
using std::endl;
using std::vector;

#ifndef REFLEX_DICTIONARY
ClassImp(WCSimRootCherenkovDigiHit)
ClassImp(WCSimRootCherenkovHit)
ClassImp(WCSimRootCherenkovHitTime)
ClassImp(WCSimRootCherenkovHitHistory)
ClassImp(WCSimRootTrack)
ClassImp(WCSimRootPi0)
ClassImp(WCSimRootEventHeader)
ClassImp(WCSimRootTrigger)
ClassImp(WCSimRootEvent)
#endif
//#define DEBUG

//TClonesArray* WCSimRootTrigger::fgTracks = 0;
//
//TClonesArray* WCSimRootTrigger::fgCherenkovHits = 0;
//TClonesArray* WCSimRootTrigger::fgCherenkovHitTimes = 0;
//TClonesArray* WCSimRootTrigger::fgCherenkovDigiHits = 0;
//
//TClonesArray* WCSimRootTrigger::fgFVCherenkovHits = 0;
//TClonesArray* WCSimRootTrigger::fgFVCherenkovHitTimes = 0;
//TClonesArray* WCSimRootTrigger::fgFVCherenkovDigiHits = 0;
//
// 
//_____________________________________________________________________________

WCSimRootTrigger::WCSimRootTrigger()
{
  // Create an WCSimRootTrigger object.

  fNvtxs=0;

  // WARNING : default constructor for ROOT : do not allocate memory
  // inside it or upon re-reading the object there will be a memory leak
  // TClonesArray of WCSimRootTracks
  fTracks = 0;
  fNtrack = 0;
  fNtrack_slots = 0;

  // TClonesArray of WCSimRootCherenkovHits
  fCherenkovHits = 0;
  fCherenkovHitTimes = 0;
  fCherenkovHitHistories = 0;
  fNcherenkovhits = 0;
  fNcherenkovhittimes = 0;
  fNcherenkovhithistories = 0;

  // TClonesArray of WCSimRootCherenkovDigiHits
  fCherenkovDigiHits = 0;
  fNcherenkovdigihits = 0;
  fNcherenkovdigihits_slots = 0;
  fSumQ = 0;

  fCaptures = 0;

  fTriggerType = kTriggerUndefined;
  fTriggerInfo.clear();
  
  IsZombie = true;
  
}

WCSimRootTrigger::WCSimRootTrigger(int Number,int Subevt)
{
  this->Initialize();
  fEvtHdr.Set(Number,0,0,Subevt);
}

//copy constructor --> only shallow copy of preallocated objects ??


void WCSimRootTrigger::Initialize() //actually allocate memory for things in here
{
  // Create an WCSimRootTrigger object.
  // When the constructor is invoked for the first time, the class static
  // variable fgTracks is 0 and the TClonesArray fgTracks is created.
  // Sim. for the other TClonesArray
  TStopwatch* mystopw = new TStopwatch();

  // TClonesArray of WCSimRootTracks
  fTracks = new TClonesArray("WCSimRootTrack", 10000);
  fTracks->BypassStreamer(kFALSE); // use the member Streamer
  fNtrack = 0;
  fNtrack_slots = 0;

  // TClonesArray of WCSimRootCherenkovHits
  fCherenkovHits = new TClonesArray("WCSimRootCherenkovHit", 
				    10000);
  fCherenkovHitTimes = new TClonesArray("WCSimRootCherenkovHitTime", 
					10000);
  fCherenkovHitHistories = new TClonesArray("WCSimRootCherenkovHitHistory", 
					10000);
  fCherenkovHits->BypassStreamer(kFALSE); // use the member Streamer
  fCherenkovHitTimes->BypassStreamer(kFALSE); // use the member Streamer
  fCherenkovHitHistories->BypassStreamer(kFALSE); // use the member Streamer
  fNcherenkovhits = 0;
  fNcherenkovhittimes = 0;
  fNcherenkovhithistories = 0;

  // TClonesArray of WCSimRootCherenkovDigiHits
  fCherenkovDigiHits = new TClonesArray("WCSimRootCherenkovDigiHit", 
				       10000);
  fCherenkovDigiHits->BypassStreamer(kFALSE); // use the member Streamer
  fNcherenkovdigihits = 0;
  fNcherenkovdigihits_slots = 0;
  fSumQ = 0;

  // TClonesArray of WCSimRootCaptures
  fCaptures = new TClonesArray("WCSimRootCapture", 100);
  fNcaptures = 0;

  fTriggerType = kTriggerUndefined;
  fTriggerInfo.clear();
  
  //  G4cout << " Time to allocate the TCAs :  Real = " << mystopw->RealTime() 
  //	    << " ; CPU = " << mystopw->CpuTime() << "\n";
  delete mystopw;
  

  IsZombie = false; // the memory has been allocated
}

//_____________________________________________________________________________

WCSimRootTrigger::~WCSimRootTrigger()
{
  // now we must do a bunch a deleting stuff...
  //Destroys all the TClonesArray.. Let's see if Ren'e Brun is right...

  TStopwatch* mystopw = new TStopwatch();

  mystopw->Start();

  if (!IsZombie) {

    fTracks->Delete();            
    fCherenkovHits->Delete();      
    fCherenkovHitTimes->Delete();   
    fCherenkovHitHistories->Delete();   
    fCherenkovDigiHits->Delete();
    fCaptures->Delete();
    
    delete   fTracks;            
    delete   fCherenkovHits;      
    delete   fCherenkovHitTimes;   
    delete   fCherenkovHitHistories;   
    delete   fCherenkovDigiHits;
    delete   fCaptures;
  }
  mystopw->Stop();

  //  G4cout << " Time to delete the TCAs :  Real = " << mystopw->RealTime() 
  //    << " ; CPU = " << mystopw->CpuTime() << "\n";

  delete mystopw;
  //Clear("C");
}

//_____________________________________________________________________________

WCSimRootTrigger & WCSimRootTrigger::operator=(const WCSimRootTrigger & in)
{
  if(this == &in) return *this;
  //first clear everything
  this->Clear();

  //then fill
  fEvtHdr = in.fEvtHdr;
  fNvtxs = in.fNvtxs;
  for(int i = 0; i < MAX_N_VERTICES; i++) {
    fVtxsvol[i] = in.fVtxsvol[i];
    for(int j = 0; j < 4; j++) {
      fVtxs[i][j] = in.fVtxs[i][j];
    }//j
    fMode[i] = in.fMode[i];

  }//i
  fVecRecNumber = in.fVecRecNumber;
  fJmu = in.fJmu;
  fJp = in.fJp;
  fPi0 = in.fPi0;
  fNpar = in.fNpar;
  fNtrack = in.fNtrack;
  fTracks = (TClonesArray*)in.fTracks->Clone();
  fNumTubesHit = in.fNumTubesHit;
  fNcherenkovhits = in.fNcherenkovhits;
  fCherenkovHits = (TClonesArray*)in.fCherenkovHits->Clone();
  fCherenkovHitCounter = in.fCherenkovHitCounter;
  fNcherenkovhittimes = in.fNcherenkovhittimes;
  fCherenkovHitTimes = (TClonesArray*)in.fCherenkovHitTimes->Clone();
  fNcherenkovhithistories = in.fNcherenkovhithistories;
  fCherenkovHitHistories = (TClonesArray*)in.fCherenkovHitHistories->Clone();
  fNumDigitizedTubes = in.fNumDigitizedTubes;
  fNcherenkovdigihits = in.fNcherenkovdigihits;
  fSumQ = in.fSumQ;
  fCherenkovDigiHits = (TClonesArray*)in.fCherenkovDigiHits->Clone();
  fTriggerType = in.fTriggerType;
  fTriggerInfo = in.fTriggerInfo;
  IsZombie = in.IsZombie;
  return *this;
}

//_____________________________________________________________________________

void WCSimRootTrigger::Clear(Option_t */*option*/)
{
  // To be filled in 
  // Filled in, by MF, 31/08/06  -> Keep all the alloc'ed memory but reset all
  // the indices to 0 in the TCAs.
  fNtrack = 0;
  fNtrack_slots = 0;

  // TClonesArray of WCSimRootCherenkovHits
  fNcherenkovhits = 0;
  fNcherenkovhittimes = 0;
  fNcherenkovhithistories = 0;

  // TClonesArray of WCSimRootCherenkovDigiHits
  fNcherenkovdigihits = 0;
  fNcherenkovdigihits_slots = 0;
  fSumQ = 0;

  // TClonesArray of WCSimRootCaptures
  fNcaptures = 0;

  // remove whatever's in the arrays
  // but don't deallocate the arrays themselves

  fTracks->Delete();
  fCherenkovHits->Delete();
  fCherenkovHitTimes->Delete();
  fCherenkovHitHistories->Delete();
  fCherenkovDigiHits->Delete();
  fCaptures->Delete();

  fTriggerType = kTriggerUndefined;
  fTriggerInfo.clear();

  IsZombie = false ; // we DO NOT deallocate the memory
}

//_____________________________________________________________________________

void WCSimRootTrigger::Reset(Option_t */*option*/)
{
// Static function to reset all static objects for this event
// To be filled in
}

//_____________________________________________________________________________

void WCSimRootTrigger::SetHeader(Int_t i, 
				  Int_t run, 
				  int64_t date,Int_t subevent)
{
  // Set the header values
  fEvtHdr.Set(i, run, date,subevent);
}

//_____________________________________________________________________________

void WCSimRootTrigger::SetTriggerInfo(TriggerType_t trigger_type,
				      std::vector<Double_t> trigger_info)
{
  fTriggerType = trigger_type;
  fTriggerInfo.resize(trigger_info.size());
  for(size_t i = 0; i < trigger_info.size(); i++)
    fTriggerInfo[i] = trigger_info[i];
}

//_____________________________________________________________________________

void WCSimRootTrigger::SetPi0Info(Double_t pi0Vtx[3],
                                  Int_t   gammaID[2],
                                  Double_t gammaE[2],
                                  Double_t gammaVtx[2][3])
{
    fPi0.Set(pi0Vtx,
             gammaID,
             gammaE,
             gammaVtx);
}

//_____________________________________________________________________________
void WCSimRootTrigger::SetCaptureParticle(Int_t parent,
                                          Int_t ipnu,
                                          Double_t time,
                                          Double_t vtx[3],
                                          Double_t dir[3],
                                          Double_t energy,
                                          Int_t id)
{
    WCSimRootCapture * capture = 0;
    for(int i = 0; i<fCaptures->GetEntriesFast(); i++){
        if(((WCSimRootCapture*)fCaptures->At(i))->GetCaptureParent() == parent) {
            capture = (WCSimRootCapture *) fCaptures->At(i);
            break;
        }
    }
    if(capture == 0) {
        TClonesArray &captures = *fCaptures;
        capture = new(captures[fNcaptures++]) WCSimRootCapture(parent);
    }
    if(ipnu==22) capture->AddGamma(id, energy, dir);
    else capture->SetInfo(vtx, time, ipnu);
}

//_____________________________________________________________________________

void WCSimRootPi0::Set(Double_t pi0Vtx[3],
                       Int_t   gammaID[2],
                       Double_t gammaE[2],
                       Double_t gammaVtx[2][3])
{
    for (int i=0;i<2;i++)
    {
        fGammaID[i] = gammaID[i];
        fGammaE[i]  = gammaE[i];
    }

    for (int j=0;j<3;j++)
    {
        fPi0Vtx[j]      = pi0Vtx[j];
        fGammaVtx[0][j] = gammaVtx[0][j];
        fGammaVtx[1][j] = gammaVtx[1][j];
    }
}

//_____________________________________________________________________________

WCSimRootCapture::WCSimRootCapture(Int_t captureParent)
{
    fCaptureParent = captureParent;
    fNGamma = 0;
    fTotalGammaE = 0;
    fGammas = new TClonesArray("WCSimRootCaptureGamma", 10);
    IsZombie=false;
}

//_____________________________________________________________________________

WCSimRootCapture::~WCSimRootCapture()
{
    if(!IsZombie) {
        fGammas->Delete();
        delete fGammas;
    }
}

//_____________________________________________________________________________

void WCSimRootCapture::SetInfo(Double_t captureVtx[3],
                               Double_t captureT,
                               Int_t   captureNucleus)
{
    for (int i=0;i<3;i++) fCaptureVtx[i] = captureVtx[i];
    fCaptureT = captureT;
    fCaptureNucleus = captureNucleus;
}

//_____________________________________________________________________________

void WCSimRootCapture::AddGamma(Int_t   gammaID,
                                Double_t gammaE,
                                Double_t gammaDir[3])
{
    TClonesArray &gammas = *fGammas;
    new(gammas[fNGamma]) WCSimRootCaptureGamma(gammaID, gammaE, gammaDir);
    fTotalGammaE += gammaE;
    fNGamma++;
}

//_____________________________________________________________________________

WCSimRootCaptureGamma::WCSimRootCaptureGamma(Int_t id,
                                             Double_t energy,
                                             Double_t *dir) {
    fID = id;
    fEnergy = energy;
    for(int i=0;i<3;i++) fDir[i] = dir[i];
}

//_____________________________________________________________________________

WCSimRootTrack *WCSimRootTrigger::AddTrack(Int_t ipnu,
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
             std::vector<int> bTypes)
{
  // Add a new WCSimRootTrack to the list of tracks for this event.
  // To avoid calling the very time consuming operator new for each track,
  // the standard but not well know C++ operator "new with placement"
  // is called. If tracks[i] is 0, a new Track object will be created
  // otherwise the previous Track[i] will be overwritten.

  TClonesArray &tracks = *fTracks;
  WCSimRootTrack *track = 
    new(tracks[fNtrack_slots++]) WCSimRootTrack(ipnu,
						flag,
						m,
						p,
						E,
						startvol,
						stopvol,
						dir,
						pdir,
						stop,
						start,
						parenttype,
						creatorProcess,
						time,
						id,
						idParent,
            bPs,
            bKEs,
            bTimes,
            bTypes);
  fNtrack++;
  return track;
}

//_____________________________________________________________________________

WCSimRootTrack *WCSimRootTrigger::AddTrack(WCSimRootTrack * track)
{
  // Add a new WCSimRootTrack to the list of tracks for this event.
  // To avoid calling the very time consuming operator new for each track,
  // the standard but not well know C++ operator "new with placement"
  // is called. If tracks[i] is 0, a new Track object will be created
  // otherwise the previous Track[i] will be overwritten.

  Double_t dir[3], pdir[3], stop[3], start[3];
  for(int i = 0; i < 3; i++) {
    dir  [i] = track->GetDir(i);
    pdir [i] = track->GetPdir(i);
    stop [i] = track->GetStop(i);
    start[i] = track->GetStart(i);
  }//i
  TClonesArray &tracks = *fTracks;
  WCSimRootTrack *track_out =
    new(tracks[fNtrack_slots++]) WCSimRootTrack(track->GetIpnu(),
					  track->GetFlag(),
					  track->GetM(),
					  track->GetP(),
					  track->GetE(),
					  track->GetStartvol(),
					  track->GetStopvol(),
					  dir,
					  pdir,
					  stop,
					  start,
					  track->GetParenttype(),
            track->GetCreatorProcess(),
					  track->GetTime(),
					  track->GetId(),
					  track->GetParentId(),
            track->GetBoundaryPoints(),
            track->GetBoundaryKEs(),
            track->GetBoundaryTimes(),
            track->GetBoundaryTypes());
  fNtrack++;
  return track_out;
}

//_____________________________________________________________________________

WCSimRootTrack * WCSimRootTrigger::RemoveTrack(WCSimRootTrack * track)
{
  WCSimRootTrack * tmp = (WCSimRootTrack *)fTracks->Remove(track);
  if(tmp)
    fNtrack--;
  return tmp;
}

//_____________________________________________________________________________

WCSimRootTrack::WCSimRootTrack(Int_t ipnu, 
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
             std::vector<int> bTypes)
{

  // Create a WCSimRootTrack object and fill it with stuff

  fIpnu = ipnu;
  fFlag = flag;
  fM = m;
  fP = p;
  fE = E;
  fStartvol = startvol;
  fStopvol = stopvol;
  int i;
  for (i=0;i<3;i++)
  {
    fDir[i] = dir[i]; 
    fPdir[i] = pdir[i]; 
    fStop[i] = stop[i]; 
    fStart[i] = start[i];
  }
  fParenttype = parenttype;
  fCreatorProcess = creatorProcess;
  fTime = time;
  fId = id;
  fParentId = idParent;
  boundaryPoints = bPs;
  boundaryKEs = bKEs;
  boundaryTimes = bTimes;
  boundaryTypes = bTypes;
}

//_____________________________________________________________________________

WCSimRootCherenkovHit *WCSimRootTrigger::AddCherenkovHit(Int_t tubeID,
							 Int_t mPMTID,
							 Int_t mPMT_PMTID,
							 std::vector<Double_t> truetime,
							 std::vector<Int_t> parentSavedTrackID,
							 std::vector<Float_t> photonStartTime,
							 std::vector<TVector3> photonStartPos,
							 std::vector<TVector3> photonEndPos,
							 std::vector<TVector3> photonStartDir,
							 std::vector<TVector3> photonEndDir,
               std::vector<ProcessType_t> photonCreatorProcess)
{
  // Add a new Cherenkov hit to the list of Cherenkov hits
  TClonesArray &cherenkovhittimes = *fCherenkovHitTimes;

  for (unsigned int i =0;i<truetime.size();i++)
  {
    fCherenkovHitCounter++;
    Float_t startPos[3];
    Float_t endPos[3];
    Float_t startDir[3];
    Float_t endDir[3];
    for(int j=0; j<3; j++){
      startPos[j] = photonStartPos[i][j];
      endPos[j] = photonEndPos[i][j];
      startDir[j] = photonStartDir[i][j];
      endDir[j] = photonEndDir[i][j];
    }
    
    ProcessType_t creatorProcess = photonCreatorProcess[i]; // Get the creator process for this p.e. 

    //WCSimRootCherenkovHitTime *cherenkovhittime =
    new(cherenkovhittimes[fNcherenkovhittimes++]) WCSimRootCherenkovHitTime(truetime[i],parentSavedTrackID[i],
									    photonStartTime[i], startPos, endPos,
									    startDir, endDir, creatorProcess);
  }
  
#ifdef DEBUG
  std::cout << "Debug B.Q: A new Cherenkov hit was added for tube " << tubeID << ", corresponding to PMT = " << mPMTID << ", and to small PMT in mPMT = " << mPMT_PMTID << std::endl;
#endif

  Int_t WC_Index[2];
  WC_Index[0] = fNcherenkovhittimes-truetime.size(); //fCherenkovHitCounter-truetime.size();
  WC_Index[1] = truetime.size();

  TClonesArray &cherenkovhits = *fCherenkovHits;
 
  WCSimRootCherenkovHit *cherenkovhit
    = new(cherenkovhits[fNcherenkovhits++]) WCSimRootCherenkovHit(tubeID,
								  mPMTID,
								  mPMT_PMTID,
								  WC_Index);

  return cherenkovhit;
}

//_____________________________________________________________________________

WCSimRootCherenkovHitHistory *WCSimRootTrigger::AddCherenkovHitHistory(Int_t nRayScat,
             Int_t nMieScat,
					   std::vector<ReflectionSurface_t> reflec)
{
  // Add a new Cherenkov hit history to the list of Cherenkov hit histories
  TClonesArray &cherenkovhithistories = *fCherenkovHitHistories;
  WCSimRootCherenkovHitHistory* cherenkovhithistory = new(cherenkovhithistories[fNcherenkovhithistories++]) WCSimRootCherenkovHitHistory(nRayScat,nMieScat,reflec);
  return cherenkovhithistory;
}

//_____________________________________________________________________________

WCSimRootCherenkovHit::WCSimRootCherenkovHit(Int_t tubeID,
					     Int_t totalPe[2])
{
  // Create a WCSimRootCherenkovHitIndex object and fill it with stuff

  fTubeID     = tubeID;
  fTotalPe[0] = totalPe[0];
  fTotalPe[1] = totalPe[1];
}

//_____________________________________________________________________________

WCSimRootCherenkovHit::WCSimRootCherenkovHit(Int_t tubeID,
					     Int_t mPMTID,
					     Int_t mPMT_PMTID,
					     Int_t totalPe[2])
{
  // Create a WCSimRootCherenkovHitIndex object and fill it with stuff

  fTubeID     = tubeID;
  fmPMTID     = mPMTID;
  fmPMT_PMTID = mPMT_PMTID;
  fTotalPe[0] = totalPe[0];
  fTotalPe[1] = totalPe[1];
}

WCSimRootCherenkovHitTime::WCSimRootCherenkovHitTime(Double_t truetime,
						     Int_t parentSavedTrackID,
						     Float_t photonStartTime,
						     Float_t photonStartPos[3],
						     Float_t photonEndPos[3],
						     Float_t photonStartDir[3],
						     Float_t photonEndDir[3],
                 ProcessType_t photonCreatorProcess)
{
  // Create a WCSimRootCherenkovHit object and fill it with stuff
  fTruetime        = truetime;
  fParentSavedTrackID = parentSavedTrackID;
  fPhotonStartTime = photonStartTime;
  fPhotonCreatorProcess = photonCreatorProcess;
  for (int i=0;i<3;i++) {
    fPhotonStartPos[i] = photonStartPos[i];
    fPhotonEndPos[i] = photonEndPos[i];
    fPhotonStartDir[i] = photonStartDir[i];
    fPhotonEndDir[i] = photonEndDir[i];
  }
}

WCSimRootCherenkovHitHistory::WCSimRootCherenkovHitHistory(Int_t nRayScat, Int_t nMieScat, std::vector<ReflectionSurface_t> refle)
{
  // Create a WCSimRootCherenkovHitHistory object and fill it with stuff
  fNRayScat = nRayScat;
  fNMieScat = nMieScat;
  fReflec = refle;
}

//_____________________________________________________________________________

WCSimRootCherenkovDigiHit *WCSimRootTrigger::AddCherenkovDigiHit(Double_t q,
								 Double_t t,
								 Int_t tubeid,
								 Int_t mpmtid,
								 Int_t mpmt_pmtid,
								 std::vector<int> photon_ids)
{
  // Add a new digitized hit to the list of digitized hits
  TClonesArray &cherenkovdigihits = *fCherenkovDigiHits;
  WCSimRootCherenkovDigiHit *cherenkovdigihit = 
    new(cherenkovdigihits[fNcherenkovdigihits_slots++]) WCSimRootCherenkovDigiHit(q,
									    t,
									    tubeid,
									    mpmtid,
									    mpmt_pmtid,
									    photon_ids);
  fNcherenkovdigihits++;
 
  return cherenkovdigihit;
}

//_____________________________________________________________________________

WCSimRootCherenkovDigiHit *WCSimRootTrigger::AddCherenkovDigiHit(WCSimRootCherenkovDigiHit * digit)
{
  // Add a new digitized hit to the list of digitized hits
  TClonesArray &cherenkovdigihits = *fCherenkovDigiHits;
  WCSimRootCherenkovDigiHit *cherenkovdigihit =
    new(cherenkovdigihits[fNcherenkovdigihits_slots++]) WCSimRootCherenkovDigiHit(
										  digit->GetQ(),
										  digit->GetT(),
										  digit->GetTubeId(),
										  digit->GetmPMTId(),
										  digit->GetmPMT_PMTId(),
										  digit->GetPhotonIds());

  fNcherenkovdigihits++;

  return cherenkovdigihit;
}

//_____________________________________________________________________________

WCSimRootCherenkovDigiHit::WCSimRootCherenkovDigiHit(Double_t q,
						     Double_t t,
						     Int_t tubeid,
						     std::vector<int> photon_ids)
{
  // Create a WCSimRootCherenkovDigiHit object and fill it with stuff

  fQ = q;
  fT = t;
  fTubeId = tubeid;
  fPhotonIds = photon_ids;
}

//_____________________________________________________________________________
WCSimRootCherenkovDigiHit::WCSimRootCherenkovDigiHit(Double_t q,
						     Double_t t,
						     Int_t tubeid,
						     Int_t mpmtid,
						     Int_t mpmt_pmtid,
						     std::vector<int> photon_ids)
{
  // Create a WCSimRootCherenkovDigiHit object and fill it with stuff

  fQ = q;
  fT = t;
  fTubeId = tubeid;
  fmPMTId = mpmtid;
  fmPMT_PMTId = mpmt_pmtid;
  fPhotonIds = photon_ids;
}

//_____________________________________________________________________________

WCSimRootCherenkovDigiHit * WCSimRootTrigger::RemoveCherenkovDigiHit(WCSimRootCherenkovDigiHit * digit)
{
  WCSimRootCherenkovDigiHit * tmp = (WCSimRootCherenkovDigiHit *)fCherenkovDigiHits->Remove(digit);
  if(tmp)
    fNcherenkovdigihits--;
  return tmp;
}


//_____________________________________________________________________________

// M Fechner, august 2006

WCSimRootEvent::WCSimRootEvent()
{
  // default constructor : nothing happens
  //fEventList.push_back(new WCSimRootTrigger() ); //at least one event 
  // this is standard root practise for streaming ROOT objtecs : if memory is alloc'ed here,
  // it will be lost
  fEventList = 0;
  Current = 0;
}

//_____________________________________________________________________________
WCSimRootEvent & WCSimRootEvent::operator=(const WCSimRootEvent & in)
{
  if (this == &in) return *this;
  this->Clear();
  Current = in.Current;
  fEventList = (TClonesArray*)in.fEventList->Clone();
  return *this;
}

//_____________________________________________________________________________
void WCSimRootEvent::Initialize()
{
  fEventList = new TObjArray(10,0); // very rarely more than 10 subevents...
  fEventList->AddAt(new WCSimRootTrigger(0,0),0);
  Current = 0;
}


//_____________________________________________________________________________
WCSimRootEvent::~WCSimRootEvent()
{
  if (fEventList != 0) {
    for (int i = 0 ; i < fEventList->GetEntriesFast() ; i++) {
      delete (*fEventList)[i];
    }
    delete fEventList;
  }
  //  std::vector<WCSimRootTrigger*>::iterator  iter = fEventList.begin();
  //for ( ; iter != fEventList.end() ; ++iter) delete (*iter);
  //Clear("");
}

//_____________________________________________________________________________
void WCSimRootEvent::Clear(Option_t* /*o*/)
{
  //nothing for now
}

//_____________________________________________________________________________
void WCSimRootEvent::Reset(Option_t* /*o*/)
{
  //nothing for now
}

//
//COMPARISON OPERATORS
//
//_____________________________________________________________________________
bool WCSimRootTrack::CompareAllVariables(const WCSimRootTrack * c) const
{
  bool failed = false;
  failed = (!ComparisonPassed(fIpnu, c->GetIpnu(), typeid(*this).name(), __func__, "Ipnu")) || failed;
  failed = (!ComparisonPassed(fFlag, c->GetFlag(), typeid(*this).name(), __func__, "Flag")) || failed;
  failed = (!ComparisonPassed(fM, c->GetM(), typeid(*this).name(), __func__, "M")) || failed;
  failed = (!ComparisonPassed(fP, c->GetP(), typeid(*this).name(), __func__, "P")) || failed;
  failed = (!ComparisonPassed(fE, c->GetE(), typeid(*this).name(), __func__, "E")) || failed;
  failed = (!ComparisonPassed(fStartvol, c->GetStartvol(), typeid(*this).name(), __func__, "Startvol")) || failed;
  failed = (!ComparisonPassed(fStopvol, c->GetStopvol(), typeid(*this).name(), __func__, "Stopvol")) || failed;
  for(int i = 0; i < 3; i++) {
    failed = (!ComparisonPassed(fDir[i], c->GetDir(i), typeid(*this).name(), __func__, TString::Format("%s[%d]", "Dir", i))) || failed;
  }//i
  for(int i = 0; i < 3; i++) {
    failed = (!ComparisonPassed(fPdir[i], c->GetPdir(i), typeid(*this).name(), __func__, TString::Format("%s[%d]", "Pdir", i))) || failed;
  }//i
  for(int i = 0; i < 3; i++) {
    failed = (!ComparisonPassed(fStop[i], c->GetStop(i), typeid(*this).name(), __func__, TString::Format("%s[%d]", "Stop", i))) || failed;
  }//i
  for(int i = 0; i < 3; i++) {
    failed = (!ComparisonPassed(fStart[i], c->GetStart(i), typeid(*this).name(), __func__, TString::Format("%s[%d]", "Start", i))) || failed;
  }//i
  failed = (!ComparisonPassed(fParenttype, c->GetParenttype(), typeid(*this).name(), __func__, "Parenttype")) || failed;
  failed = (!ComparisonPassed(fTime, c->GetTime(), typeid(*this).name(), __func__, "Time")) || failed;
  failed = (!ComparisonPassed(fId, c->GetId(), typeid(*this).name(), __func__, "Id")) || failed;
  failed = (!ComparisonPassed(fParentId, c->GetParentId(), typeid(*this).name(), __func__, "ParentId")) || failed;
  failed = (!ComparisonPassedVecVec(boundaryPoints, c->GetBoundaryPoints(), typeid(*this).name(), __func__, "boundaryPoints")) || failed;
  failed = (!ComparisonPassedVec(boundaryKEs, c->GetBoundaryKEs(), typeid(*this).name(), __func__, "boundaryKEs")) || failed;
  failed = (!ComparisonPassedVec(boundaryTimes, c->GetBoundaryTimes(), typeid(*this).name(), __func__, "boundaryTimes")) || failed;
  failed = (!ComparisonPassedVec(boundaryTypes, c->GetBoundaryTypes(), typeid(*this).name(), __func__, "boundaryTypes")) || failed;
  failed = (!ComparisonPassed(fCreatorProcess, c->GetCreatorProcess(), typeid(*this).name(), __func__, "CreatorProcess")) || failed;

  return !failed;
}

//_____________________________________________________________________________
bool WCSimRootCherenkovHit::CompareAllVariables(const WCSimRootCherenkovHit * c) const
{
  bool failed = false;

  failed = (!ComparisonPassed(fTubeID, c->GetTubeID(), typeid(*this).name(), __func__, "TubeID")) || failed;
  failed = (!ComparisonPassed(fmPMTID, c->GetmPMTID(), typeid(*this).name(), __func__, "mPMTID")) || failed;
  failed = (!ComparisonPassed(fmPMT_PMTID, c->GetmPMT_PMTID(), typeid(*this).name(), __func__, "mPMT_PMTID")) || failed;
  for(int i = 0; i < 2; i++) {
    failed = (!ComparisonPassed(fTotalPe[i], c->GetTotalPe(i), typeid(*this).name(), __func__, TString::Format("%s[%d]", "TotalPe", i))) || failed;
  }//i

  return !failed;
}

//_____________________________________________________________________________
bool WCSimRootCherenkovHitTime::CompareAllVariables(const WCSimRootCherenkovHitTime * c) const
{
  bool failed = false;

  failed = (!ComparisonPassed(fTruetime, c->GetTruetime(), typeid(*this).name(), __func__, "Truetime")) || failed;
  failed = (!ComparisonPassed(fParentSavedTrackID, c->GetParentID(), typeid(*this).name(), __func__, "ParentSavedTrackID")) || failed;
  failed = (!ComparisonPassed(fPhotonStartTime, c->GetPhotonStartTime(), typeid(*this).name(), __func__, "PhotonStartTime")) || failed;
  failed = (!ComparisonPassed(fPhotonCreatorProcess, c->GetPhotonCreatorProcess(), typeid(*this).name(), __func__, "PhotonCreatorProcess")) || failed;
  for(int i = 0; i < 3; i++) {
    failed = (!ComparisonPassed(fPhotonStartPos[i], c->GetPhotonStartPos(i), typeid(*this).name(), __func__, TString::Format("%s[%d]", "PhotonStartPos", i))) || failed;
    failed = (!ComparisonPassed(fPhotonEndPos[i], c->GetPhotonEndPos(i), typeid(*this).name(), __func__, TString::Format("%s[%d]", "PhotonEndPos", i))) || failed;
    failed = (!ComparisonPassed(fPhotonStartDir[i], c->GetPhotonStartDir(i), typeid(*this).name(), __func__, TString::Format("%s[%d]", "PhotonStartDir", i))) || failed;
    failed = (!ComparisonPassed(fPhotonEndDir[i], c->GetPhotonEndDir(i), typeid(*this).name(), __func__, TString::Format("%s[%d]", "PhotonEndDir", i))) || failed;
  }//i

  return !failed;
}

//_____________________________________________________________________________
bool WCSimRootCherenkovHitHistory::CompareAllVariables(const WCSimRootCherenkovHitHistory * c) const
{
  bool failed = false;

  failed = (!ComparisonPassed(fNRayScat, c->GetNRayScatters(), typeid(*this).name(), __func__, "RayleighScattering")) || failed;
  failed = (!ComparisonPassed(fNMieScat, c->GetNMieScatters(), typeid(*this).name(), __func__, "MieScattering")) || failed;
  failed = (!ComparisonPassedVec(std::vector<int>(fReflec.begin(),fReflec.end()), std::vector<int>(c->GetReflectionSurfaces().begin(),c->GetReflectionSurfaces().end()), typeid(*this).name(), __func__, "Reflection")) || failed;

  return !failed;
}

//_____________________________________________________________________________
bool WCSimRootCherenkovDigiHit::CompareAllVariables(const WCSimRootCherenkovDigiHit * c) const
{
  bool failed = false;

  failed = (!ComparisonPassed(fQ, c->GetQ(), typeid(*this).name(), __func__, "Q")) || failed;
  failed = (!ComparisonPassed(fT, c->GetT(), typeid(*this).name(), __func__, "T")) || failed;
  failed = (!ComparisonPassed(fTubeId, c->GetTubeId(), typeid(*this).name(), __func__, "TubeId")) || failed;
  failed = (!ComparisonPassed(fmPMTId, c->GetmPMTId(), typeid(*this).name(), __func__, "mPMTID")) || failed;
  failed = (!ComparisonPassed(fmPMT_PMTId, c->GetmPMT_PMTId(), typeid(*this).name(), __func__, "mPMT_PMTID")) || failed;
  failed = (!ComparisonPassedVec(fPhotonIds, c->GetPhotonIds(), typeid(*this).name(), __func__, "PhotonIds")) || failed;

  return !failed;
}

//_____________________________________________________________________________
bool WCSimRootCaptureGamma::CompareAllVariables(const WCSimRootCaptureGamma * c) const
{
  bool failed = false;

  failed = (!ComparisonPassed(fID, c->GetID(), typeid(*this).name(), __func__, "ID")) || failed;
  failed = (!ComparisonPassed(fEnergy, c->GetE(), typeid(*this).name(), __func__, "Energy")) || failed;
  for(int i = 0; i < 3; i++) {
    failed = (!ComparisonPassed(fDir[i], c->GetDir(i), typeid(*this).name(), __func__, TString::Format("Dir[%d]", i))) || failed;
  }//i

  return !failed;
}
//_____________________________________________________________________________
bool WCSimRootCapture::CompareAllVariables(const WCSimRootCapture * c) const
{
  bool failed = false;

  failed = (!ComparisonPassed(fCaptureParent, c->GetCaptureParent(), typeid(*this).name(), __func__, "CaptureParent")) || failed;
  for(int i = 0; i < 3; i++) {
    failed = (!ComparisonPassed(fCaptureVtx[i], c->GetCaptureVtx(i), typeid(*this).name(), __func__, TString::Format("CaptureVtx[%d]", i))) || failed;
  }//i
  failed = (!ComparisonPassed(fNGamma, c->GetNGamma(), typeid(*this).name(), __func__, "NGamma")) || failed;
  failed = (!ComparisonPassed(fTotalGammaE, c->GetTotalGammaE(), typeid(*this).name(), __func__, "TotalGammaE")) || failed;
  failed = (!ComparisonPassed(fCaptureT, c->GetCaptureT(), typeid(*this).name(), __func__, "CaptureT")) || failed;
  failed = (!ComparisonPassed(fCaptureNucleus, c->GetCaptureNucleus(), typeid(*this).name(), __func__, "CaptureNucleus")) || failed;

  //Check the totals of the gamma arrays
  if(this->GetGammas()->GetEntries() != c->GetGammas()->GetEntries()) {
    cerr << "WCSimRootTrigger::CompareAllVariables() Different number of gammas: " << this->GetGammas()->GetEntries() << ", " << c->GetGammas()->GetEntries() << endl;
    failed = true;
  }
  //and check the individual gammas
  for(int i = 0; i < TMath::Min(this->GetGammas()->GetEntries(), c->GetGammas()->GetEntries()); i++) {
#ifdef VERBOSE_COMPARISON
    cout << "Capture gamma " << i << endl;
#endif
    bool thisfailed = !((WCSimRootCaptureGamma *)this->GetGammas()->At(i))->CompareAllVariables((WCSimRootCaptureGamma *)c->GetGammas()->At(i));
    failed = thisfailed || failed;
  }//i (WCSimRootCaptureGammas)

  return !failed;
}
//_____________________________________________________________________________
bool WCSimRootEventHeader::CompareAllVariables(const WCSimRootEventHeader * c) const
{
  bool failed = false;

  failed = (!ComparisonPassed(fEvtNum, c->GetEvtNum(), typeid(*this).name(), __func__, "EvtNum")) || failed;
  failed = (!ComparisonPassed(fRun, c->GetRun(), typeid(*this).name(), __func__, "Run")) || failed;
  failed = (!ComparisonPassed(fDate, c->GetDate(), typeid(*this).name(), __func__, "Date")) || failed;
  failed = (!ComparisonPassed(fSubEvtNumber, c->GetSubEvtNumber(), typeid(*this).name(), __func__, "SubEvtNumber")) || failed;

  return !failed;
}

//_____________________________________________________________________________
bool WCSimRootPi0::CompareAllVariables(const WCSimRootPi0 * c) const
{
  bool failed = false;

  for(int i = 0; i < 3; i++) {
    failed = (!ComparisonPassed(fPi0Vtx[i], c->GetPi0Vtx(i), typeid(*this).name(), __func__, TString::Format("%s[%d]", "Pi0Vtx", i))) || failed;
  }//i
  for(int i = 0; i < 2; i++) {
    failed = (!ComparisonPassed(fGammaID[i], c->GetGammaID(i), typeid(*this).name(), __func__, TString::Format("%s[%d]", "GammaID", i))) || failed;
  }//i
  for(int i = 0; i < 2; i++) {
    failed = (!ComparisonPassed(fGammaE[i], c->GetGammaE(i), typeid(*this).name(), __func__, TString::Format("%s[%d]", "GammaE", i))) || failed;
  }//i
  for(int i = 0; i < 2; i++) {
    for(int j = 0; j < 3; j++) {
      failed = (!ComparisonPassed(fGammaVtx[i][j], c->GetGammaVtx(i, j), typeid(*this).name(), __func__, TString::Format("%s[%d][%d]", "GammaVtx", i, j))) || failed;
    }//j
  }//i

  return !failed;
}

//_____________________________________________________________________________
bool WCSimRootTrigger::CompareAllVariables(const WCSimRootTrigger * c, bool deep_comparison) const
{
  bool failed = false;

  failed = !(fEvtHdr.CompareAllVariables(c->GetHeader() )) || failed;
  failed = !(fPi0   .CompareAllVariables(c->GetPi0Info())) || failed;

  //
  //Check the totals of the arrays of tracks/hits/hittimes/digits/etc.
  //
  if(this->GetTracks()->GetEntries() != c->GetTracks()->GetEntries()) {
    cerr << "WCSimRootTrigger::CompareAllVariables() Different number of tracks: " << this->GetTracks()->GetEntries() << ", " << c->GetTracks()->GetEntries() << endl;
    failed = true;
  }
  if(this->GetCherenkovHits()->GetEntries() != c->GetCherenkovHits()->GetEntries()) {
    cerr << "WCSimRootTrigger::CompareAllVariables() Different number of Cherenkov hits: " << this->GetCherenkovHits()->GetEntries() << ", " << c->GetCherenkovHits()->GetEntries() << endl;
    failed = true;
  }
  if(this->GetCherenkovHitTimes()->GetEntries() != c->GetCherenkovHitTimes()->GetEntries()) {
    cerr << "WCSimRootTrigger::CompareAllVariables() Different number of Cherenkov hit times: " << this->GetCherenkovHitTimes()->GetEntries() << ", " << c->GetCherenkovHitTimes()->GetEntries() << endl;
    failed = true;
  }
  if(this->GetCherenkovDigiHits()->GetEntries() != c->GetCherenkovDigiHits()->GetEntries()) {
    cerr << "WCSimRootTrigger::CompareAllVariables() Different number of Cherenkov digi hits: " << this->GetCherenkovDigiHits()->GetEntries() << ", " << c->GetCherenkovDigiHits()->GetEntries() << endl;
    failed = true;
  }
  if(this->GetCaptures()->GetEntries() != c->GetCaptures()->GetEntries()) {
    cerr << "WCSimRootTrigger::CompareAllVariables() Different number of captures: " << this->GetCaptures()->GetEntries() << ", " << c->GetCaptures()->GetEntries() << endl;
    failed = true;
  }

  //check tracks
  // this is more complicated because there can be some empty slots for at least one of the TClonesArray
  int ithis = -1, ithat = -1;
  WCSimRootTrack * tmp_track_1, * tmp_track_2;
  int ncomp_track = 0;
  while(true) {
    tmp_track_1 = 0;
    while(!tmp_track_1 && ithis < this->GetNtrack_slots() - 1) {
      ithis++;
      tmp_track_1 = (WCSimRootTrack *)this->GetTracks()->At(ithis);
    }
    tmp_track_2 = 0;
    while(!tmp_track_2 && ithat < c->GetNtrack_slots() - 1) {
      ithat++;
      tmp_track_2 = (WCSimRootTrack *)c->GetTracks()->At(ithat);
    }
    if(!tmp_track_1 || !tmp_track_2)
      break;
#ifdef VERBOSE_COMPARISON
    cout << "Comparing track " << ithis " in this with track"
	 << ithat " in that" << endl;
#endif
    failed = !(tmp_track_1->CompareAllVariables(tmp_track_2)) || failed;
    ncomp_track++;
  }//ithis ithat
  if(ncomp_track != fNtrack && ncomp_track != c->GetNtrack()) {
    cerr << "Only compared " << ncomp_track << " tracks. There should be " << TMath::Min(fNtrack, c->GetNtrack()) << " comparisons" << endl;
  }

  //check hits & hit times
  for(int i = 0; i < TMath::Min(this->GetCherenkovHits()->GetEntries(), c->GetCherenkovHits()->GetEntries()); i++) {
#ifdef VERBOSE_COMPARISON
    cout << "Hit " << i << endl;
#endif
    bool thisfailed = !((WCSimRootCherenkovHit *)this->GetCherenkovHits()->At(i))->CompareAllVariables((WCSimRootCherenkovHit *)c->GetCherenkovHits()->At(i));
    failed = thisfailed || failed;

		// The hit history isn't always filled (depends on build options)
		// So need to have additional check
		int nhist_this = this->GetCherenkovHitHistories()->GetEntries();
		int nhist_c = c->GetCherenkovHitHistories()->GetEntries();
		bool nhist_failed = nhist_this != nhist_c;
		int nhist_max = TMath::Max(nhist_this, nhist_c);
		failed = nhist_failed || failed;

    if(!thisfailed) {
      //"hit" compared, now check the "hit times" for the same PMT
      int timepos = ((WCSimRootCherenkovHit *)this->GetCherenkovHits()->At(i))->GetTotalPe(0);
      int totalpe = ((WCSimRootCherenkovHit *)this->GetCherenkovHits()->At(i))->GetTotalPe(1);
      for(int j = timepos; j < timepos + totalpe; j++) {
#ifdef VERBOSE_COMPARISON
				cout << "Hit Time " << j << endl;
#endif
				failed = !((WCSimRootCherenkovHitTime *)this->GetCherenkovHitTimes()->At(j))->CompareAllVariables((WCSimRootCherenkovHitTime *)c->GetCherenkovHitTimes()->At(j)) || failed;
				if(j < nhist_max) {
					failed = !((WCSimRootCherenkovHitHistory *)this->GetCherenkovHitHistories()->At(j))->CompareAllVariables((WCSimRootCherenkovHitHistory *)c->GetCherenkovHitHistories()->At(j)) || failed;
				}//history present in both
      }//j (WCSimRootCherenkovHitTime)
    }
  }//i (WCSimRootCherenkovHit)

  //check digitised hits
  // this is more complicated because there can be some empty slots for at least one of the TClonesArray
  // also, digits can potentially be in different orders
  ithis = -1, ithat = -1;
  WCSimRootCherenkovDigiHit * tmp_digit_1, * tmp_digit_2;
  int ncomp_digi = 0;
  bool failed_digits = false;
  while(true) {
    tmp_digit_1 = 0;
    while(!tmp_digit_1 && ithis < this->GetNcherenkovdigihits_slots() - 1) {
      ithis++;
      tmp_digit_1 = (WCSimRootCherenkovDigiHit *)this->GetCherenkovDigiHits()->At(ithis);
    }
    tmp_digit_2 = 0;
    while(!tmp_digit_2 && ithat < c->GetNcherenkovdigihits_slots() - 1) {
      ithat++;
      tmp_digit_2 = (WCSimRootCherenkovDigiHit *)c->GetCherenkovDigiHits()->At(ithat);
    }
    if(!tmp_digit_1 || !tmp_digit_2)
      break;
#ifdef VERBOSE_COMPARISON
    cout << "Comparing digit " << ithis " in this with digit"
	 << ithat " in that" << endl;
#endif
    failed_digits = !(tmp_digit_1->CompareAllVariables(tmp_digit_2)) || failed_digits;
    ncomp_digi++;
  }//while(true)
  if(ncomp_digi != fNcherenkovdigihits && ncomp_digi != c->GetNcherenkovdigihits()) {
    cerr << "Only compared " << ncomp_digi << " digits. There should be " << TMath::Min(fNcherenkovdigihits, c->GetNcherenkovdigihits()) << " comparisons" << endl;
  }
  if(!deep_comparison)
    failed = failed || failed_digits;
  else if(failed_digits) {
    cout << "Peforming deep comparison" << endl;
    // We're running a deep comparison and we have some failed digits.
    // We're therefore going to do the check under the assumption that the order of digits can be different between this and that
    vector<WCSimRootCherenkovDigiHit *> tmpdigis;
    for(ithis = 0; ithis < this->GetNcherenkovdigihits_slots() - 1; ithis++) {
      tmp_digit_1 = (WCSimRootCherenkovDigiHit *)this->GetCherenkovDigiHits()->At(ithis);
      if(!tmp_digit_1)
	continue;
      int this_pmtid = tmp_digit_1->GetTubeId();
      for(ithat = 0; ithat < c->GetNcherenkovdigihits_slots() - 1; ithat++) {
	tmp_digit_2 = (WCSimRootCherenkovDigiHit *)c->GetCherenkovDigiHits()->At(ithat);
	if(!tmp_digit_2)
	  continue;
	if(tmp_digit_2->GetTubeId() != this_pmtid) {
	  continue;
	}
	tmpdigis.push_back(tmp_digit_2);
      }//ithat
      //we've now got a single digit from this, and all the digits on the same PMT from that
      if(!tmpdigis.size()) {
	cerr << "No digits on this PMT with ID " << this_pmtid << " found on that" << endl;
	failed = true;
      }
      bool found = false;
      for(unsigned int i = 0; i < tmpdigis.size(); i++) {
	found = tmp_digit_1->CompareAllVariables(tmpdigis.at(i));
	if(found)
	  break;
      }
      if(!found)
	failed = true;
      tmpdigis.clear();
    }//ithis
  }//failed_digits && deep_comparison

  //check captures
  for(int i = 0; i < TMath::Min(this->GetCaptures()->GetEntries(), c->GetCaptures()->GetEntries()); i++) {
#ifdef VERBOSE_COMPARISON
    cout << "Capture " << i << endl;
#endif
    bool thisfailed = !((WCSimRootCapture *)this->GetCaptures()->At(i))->CompareAllVariables((WCSimRootCapture *)c->GetCaptures()->At(i));
    failed = thisfailed || failed;
  }//i (WCSimRootCapture)

  failed = (!ComparisonPassed(fNvtxs, c->GetNvtxs(), typeid(*this).name(), __func__, "Nvtxs")) || failed;
  for(int ivtx = 0; ivtx < fNvtxs; ivtx++) {
    failed = (!ComparisonPassed(fMode[ivtx], c->GetMode(), typeid(*this).name(), __func__, TString::Format("Mode[%d]", ivtx))) || failed;
    failed = (!ComparisonPassed(fVtxsvol[ivtx], c->GetVtxsvol(ivtx), typeid(*this).name(), __func__, TString::Format("Vtxvols[%d]", ivtx))) || failed;
    for(int i = 0; i < 4; i++) {
      failed = (!ComparisonPassed(fVtxs[ivtx][i], c->GetVtxs(ivtx, i), typeid(*this).name(), __func__, TString::Format("%s[%d][%d]", "Vtxs", ivtx, i))) || failed;
    }//i
  }//ivtx
  failed = (!ComparisonPassed(fVecRecNumber, c->GetVecRecNumber(), typeid(*this).name(), __func__, "VecRecNumber")) || failed;
  failed = (!ComparisonPassed(fJmu, c->GetJmu(), typeid(*this).name(), __func__, "Jmu")) || failed;
  failed = (!ComparisonPassed(fJp, c->GetJp(), typeid(*this).name(), __func__, "Jp")) || failed;
  failed = (!ComparisonPassed(fNpar, c->GetNpar(), typeid(*this).name(), __func__, "Npar")) || failed;
  failed = (!ComparisonPassed(fNumTubesHit, c->GetNumTubesHit(), typeid(*this).name(), __func__, "NumTubesHit")) || failed;
  failed = (!ComparisonPassed(fNumDigitizedTubes, c->GetNumDigiTubesHit(), typeid(*this).name(), __func__, "NumDigitizedTubes")) || failed;
  failed = (!ComparisonPassed(fNtrack, c->GetNtrack(), typeid(*this).name(), __func__, "Ntrack")) || failed;
  //don't expect this to pass in general, so don't affect failed
  ComparisonPassed(fNtrack_slots, c->GetNtrack_slots(), typeid(*this).name(), __func__, "Ntrack_slots (shouldn't necessarily be equal)");
  failed = (!ComparisonPassed(fNcherenkovhits, c->GetNcherenkovhits(), typeid(*this).name(), __func__, "Ncherenkovhits")) || failed;
  failed = (!ComparisonPassed(fNcherenkovhittimes, c->GetNcherenkovhittimes(), typeid(*this).name(), __func__, "Ncherenkovhittimes")) || failed;
  failed = (!ComparisonPassed(fNcherenkovhithistories, c->GetNcherenkovhithistories(), typeid(*this).name(), __func__, "Ncherenkovhithistories")) || failed;
  failed = (!ComparisonPassed(fNcherenkovdigihits, c->GetNcherenkovdigihits(), typeid(*this).name(), __func__, "Ncherenkovdigihits")) || failed;
  //don't expect this to pass in general, so don't affect failed
  ComparisonPassed(fNcherenkovdigihits_slots, c->GetNcherenkovdigihits_slots(), typeid(*this).name(), __func__, "Ncherenkovdigihits_slots (shouldn't necessarily be equal)");
  failed = (!ComparisonPassed(fSumQ, c->GetSumQ(), typeid(*this).name(), __func__, "SumQ")) || failed;
  failed = (!ComparisonPassed(fTriggerType, c->GetTriggerType(), typeid(*this).name(), __func__, "TriggerType")) || failed;
  failed = (!ComparisonPassedVec(fTriggerInfo, c->GetTriggerInfo(), typeid(*this).name(), __func__, "TriggerInfo")) || failed;

  return !failed;
}


//_____________________________________________________________________________
bool WCSimRootEvent::CompareAllVariables(const WCSimRootEvent * c, bool deep_comparison) const
{
  bool failed = false;

  if(this->GetNumberOfEvents() != c->GetNumberOfEvents()) {
    cerr << "WCSimRootEvent::CompareAllVariables() Different number of events: " << this->GetNumberOfEvents() << ", " << c->GetNumberOfEvents() << endl;
    failed = true;
  }

  for(int i = 0; i < TMath::Min(this->GetNumberOfEvents(), c->GetNumberOfEvents()); i++) {
    failed = !(this->GetTrigger(i)->CompareAllVariables(c->GetTrigger(i), deep_comparison)) || failed;
  }

  return !failed;
}


//_____________________________________________________________________________

//  LocalWords:  GetCherenkovDigiHits
