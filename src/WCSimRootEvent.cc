// Based on Root test Event.cxx
////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TDirectory.h"
#include "TProcessID.h"
#include <string>
#include <vector>
#include <iostream>
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
ClassImp(WCSimRootTrack)
ClassImp(WCSimRootPi0)
ClassImp(WCSimRootEventHeader)
ClassImp(WCSimRootTrigger)
ClassImp(WCSimRootEvent)
#endif

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


  // WARNING : default constructor for ROOT : do not allocate memory
  // inside it or upon re-reading the object there will be a memory leak
  // TClonesArray of WCSimRootTracks
  fTracks = 0;
  fNtrack = 0;
  fNtrack_slots = 0;

  // TClonesArray of WCSimRootCherenkovHits
  fCherenkovHits = 0;
  fCherenkovHitTimes = 0;
  fNcherenkovhits = 0;
  fNcherenkovhittimes = 0;

  // TClonesArray of WCSimRootCherenkovDigiHits
  fCherenkovDigiHits = 0;
  fNcherenkovdigihits = 0;
  fNcherenkovdigihits_slots = 0;
  fSumQ = 0;

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
  fCherenkovHits->BypassStreamer(kFALSE); // use the member Streamer
  fCherenkovHitTimes->BypassStreamer(kFALSE); // use the member Streamer
  fNcherenkovhits = 0;
  fNcherenkovhittimes = 0;

  // TClonesArray of WCSimRootCherenkovDigiHits
  fCherenkovDigiHits = new TClonesArray("WCSimRootCherenkovDigiHit", 
				       10000);
  fCherenkovDigiHits->BypassStreamer(kFALSE); // use the member Streamer
  fNcherenkovdigihits = 0;
  fNcherenkovdigihits_slots = 0;
  fSumQ = 0;

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
    fCherenkovDigiHits->Delete();  
    
    delete   fTracks;            
    delete   fCherenkovHits;      
    delete   fCherenkovHitTimes;   
    delete   fCherenkovDigiHits; 
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
  //first clear everything
  this->Clear();

  //then fill
  fEvtHdr = in.fEvtHdr;
  fMode = in.fMode;
  fNvtxs = in.fNvtxs;
  for(int i = 0; i < MAX_N_PRIMARIES; i++) {
    fVtxsvol[i] = in.fVtxsvol[i];
    for(int j = 0; j < 3; j++) {
      fVtxs[i][j] = in.fVtxs[i][j];
    }//j
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
  fNumDigitizedTubes = in.fNumDigitizedTubes;
  fNcherenkovdigihits = in.fNcherenkovdigihits;
  fSumQ = in.fSumQ;
  fCherenkovDigiHits = (TClonesArray*)in.fCherenkovDigiHits->Clone();
  fTriggerType = in.fTriggerType;
  fTriggerInfo = in.fTriggerInfo;
  IsZombie = in.IsZombie;
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

  // TClonesArray of WCSimRootCherenkovDigiHits
  fNcherenkovdigihits = 0;
  fNcherenkovdigihits_slots = 0;
  fSumQ = 0;

  // remove whatever's in the arrays
  // but don't deallocate the arrays themselves

  fTracks->Delete();
  fCherenkovHits->Delete();
  fCherenkovHitTimes->Delete();
  fCherenkovDigiHits->Delete();

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
				  Int_t date,Int_t subevent)
{
  // Set the header values
  fEvtHdr.Set(i, run, date,subevent);
}

//_____________________________________________________________________________

void WCSimRootTrigger::SetTriggerInfo(TriggerType_t trigger_type,
				      std::vector<Float_t> trigger_info)
{
  fTriggerType = trigger_type;
  fTriggerInfo = trigger_info;
}

//_____________________________________________________________________________

void WCSimRootTrigger::SetPi0Info(Float_t pi0Vtx[3], 
				 Int_t   gammaID[2], 
				 Float_t gammaE[2],
				 Float_t gammaVtx[2][3])
{
  fPi0.Set(pi0Vtx, 
	   gammaID, 
	   gammaE,
	   gammaVtx);
}

//_____________________________________________________________________________

void WCSimRootPi0::Set(Float_t pi0Vtx[3], 
			Int_t   gammaID[2], 
			Float_t gammaE[2],
			Float_t gammaVtx[2][3])
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

WCSimRootTrack *WCSimRootTrigger::AddTrack(Int_t ipnu, 
					   Int_t flag, 
					   Float_t m, 
					   Float_t p, 
					   Float_t E, 
					   Int_t startvol, 
					   Int_t stopvol, 
					   Float_t dir[3], 
					   Float_t pdir[3], 
					   Float_t stop[3], 
					   Float_t start[3],
					   Int_t parenttype,
					   Float_t time,
					   Int_t id)
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
					  time,id);
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

  Float_t dir[3], pdir[3], stop[3], start[3];
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
					  track->GetTime(),
					  track->GetId());
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
				 Float_t m, 
				 Float_t p, 
				 Float_t E, 
				 Int_t startvol, 
				 Int_t stopvol, 
				 Float_t dir[3], 
				 Float_t pdir[3], 
				 Float_t stop[3], 
				 Float_t start[3],
				 Int_t parenttype,
			       Float_t time,Int_t id)
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
  fTime = time;
  fId = id;
}

//_____________________________________________________________________________

WCSimRootCherenkovHit *WCSimRootTrigger::AddCherenkovHit(Int_t tubeID,std::vector<Float_t> truetime,std::vector<Int_t> primParID)
{
  // Add a new Cherenkov hit to the list of Cherenkov hits
  TClonesArray &cherenkovhittimes = *fCherenkovHitTimes;

  for (unsigned int i =0;i<truetime.size();i++)
  {
    fCherenkovHitCounter++;

    WCSimRootCherenkovHitTime *cherenkovhittime = 
      new(cherenkovhittimes[fNcherenkovhittimes++]) WCSimRootCherenkovHitTime(truetime[i],primParID[i]);
  }

  Int_t WC_Index[2];
  WC_Index[0] = fNcherenkovhittimes-truetime.size(); //fCherenkovHitCounter-truetime.size();
  WC_Index[1] = truetime.size();

  TClonesArray &cherenkovhits = *fCherenkovHits;
 
  WCSimRootCherenkovHit *cherenkovhit
    = new(cherenkovhits[fNcherenkovhits++]) WCSimRootCherenkovHit(tubeID,
								  WC_Index);

  return cherenkovhit;
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

WCSimRootCherenkovHitTime::WCSimRootCherenkovHitTime(Float_t truetime,
						     Int_t primParID)
{
  // Create a WCSimRootCherenkovHit object and fill it with stuff
    fTruetime        = truetime; 
    fPrimaryParentID = primParID;
}

//_____________________________________________________________________________

WCSimRootCherenkovDigiHit *WCSimRootTrigger::AddCherenkovDigiHit(Float_t q, 
								 Float_t t, 
								 Int_t tubeid,
								 std::vector<int> photon_ids)
{
  // Add a new digitized hit to the list of digitized hits
  TClonesArray &cherenkovdigihits = *fCherenkovDigiHits;
  WCSimRootCherenkovDigiHit *cherenkovdigihit = 
    new(cherenkovdigihits[fNcherenkovdigihits_slots++]) WCSimRootCherenkovDigiHit(q,
									    t,
									    tubeid,
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
										  digit->GetPhotonIds());

  fNcherenkovdigihits++;

  return cherenkovdigihit;
}

//_____________________________________________________________________________

WCSimRootCherenkovDigiHit::WCSimRootCherenkovDigiHit(Float_t q,
						     Float_t t, 
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

WCSimRootEvent & WCSimRootEvent::operator=(const WCSimRootEvent & in)
{
  this->Clear();
  Current = in.Current;
  fEventList = (TClonesArray*)in.fEventList->Clone();
}

void WCSimRootEvent::Initialize()
{
  fEventList = new TObjArray(10,0); // very rarely more than 10 subevents...
  fEventList->AddAt(new WCSimRootTrigger(0,0),0);
  Current = 0;
}


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

void WCSimRootEvent::Clear(Option_t* /*o*/)
{
  //fEventList->Delete();
}

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

  return !failed;
}

//_____________________________________________________________________________
bool WCSimRootCherenkovHit::CompareAllVariables(const WCSimRootCherenkovHit * c) const
{
  bool failed = false;

  failed = (!ComparisonPassed(fTubeID, c->GetTubeID(), typeid(*this).name(), __func__, "TubeID")) || failed;
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
  failed = (!ComparisonPassed(fPrimaryParentID, c->GetParentID(), typeid(*this).name(), __func__, "PrimaryParentID")) || failed;

  return !failed;
}

//_____________________________________________________________________________
bool WCSimRootCherenkovDigiHit::CompareAllVariables(const WCSimRootCherenkovDigiHit * c) const
{
  bool failed = false;

  failed = (!ComparisonPassed(fQ, c->GetQ(), typeid(*this).name(), __func__, "Q")) || failed;
  failed = (!ComparisonPassed(fT, c->GetT(), typeid(*this).name(), __func__, "T")) || failed;
  failed = (!ComparisonPassed(fTubeId, c->GetTubeId(), typeid(*this).name(), __func__, "TubeId")) || failed;
  failed = (!ComparisonPassedVec(fPhotonIds, c->GetPhotonIds(), typeid(*this).name(), __func__, "PhotonIds")) || failed;

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
bool WCSimRootTrigger::CompareAllVariables(const WCSimRootTrigger * c) const
{
  bool failed = false;

  failed = !(fEvtHdr.CompareAllVariables(c->GetHeader() )) || failed;
  failed = !(fPi0   .CompareAllVariables(c->GetPi0Info())) || failed;

  //
  //Check the totals of the arrays of tracks/hits/hittimes/digits
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

    if(!thisfailed) {
      //"hit" compared, now check the "hit times" for the same PMT
      int timepos = ((WCSimRootCherenkovHit *)this->GetCherenkovHits()->At(i))->GetTotalPe(0);
      int totalpe = ((WCSimRootCherenkovHit *)this->GetCherenkovHits()->At(i))->GetTotalPe(1);
      for(int j = timepos; j < timepos + totalpe; j++) {
#ifdef VERBOSE_COMPARISON
	cout << "Hit Time " << j << endl;
#endif
	failed = !((WCSimRootCherenkovHitTime *)this->GetCherenkovHitTimes()->At(i))->CompareAllVariables((WCSimRootCherenkovHitTime *)c->GetCherenkovHitTimes()->At(i)) || failed;
      }//j (WCSimRootCherenkovHitTime)
    }
  }//i (WCSimRootCherenkovHit)

  //check digitised hits
  // this is more complicated because there can be some empty slots for at least one of the TClonesArray
  ithis = -1, ithat = -1;
  WCSimRootCherenkovDigiHit * tmp_digit_1, * tmp_digit_2;
  int ncomp_digi = 0;
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
    failed = !(tmp_digit_1->CompareAllVariables(tmp_digit_2)) || failed;
    ncomp_digi++;
  }//ithis ithat
  if(ncomp_digi != fNcherenkovdigihits && ncomp_digi != c->GetNcherenkovdigihits()) {
    cerr << "Only compared " << ncomp_digi << " digits. There should be " << TMath::Min(fNcherenkovdigihits, c->GetNcherenkovdigihits()) << " comparisons" << endl;
  }

  failed = (!ComparisonPassed(fMode, c->GetMode(), typeid(*this).name(), __func__, "Mode")) || failed;
  failed = (!ComparisonPassed(fNvtxs, c->GetNvtxs(), typeid(*this).name(), __func__, "Nvtxs")) || failed;
  for(int ivtx = 0; ivtx < fNvtxs; ivtx++) {
    failed = (!ComparisonPassed(fVtxsvol[ivtx], c->GetVtxsvol(ivtx), typeid(*this).name(), __func__, TString::Format("Vtxvols[%d]", ivtx))) || failed;
    for(int i = 0; i < 3; i++) {
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
  ComparisonPassed(fNtrack_slots, c->GetNtrack_slots(), typeid(*this).name(), __func__, "Ntrack_slots (shouldn't necessarily be equal)");
  failed = (!ComparisonPassed(fNcherenkovhits, c->GetNcherenkovhits(), typeid(*this).name(), __func__, "Ncherenkovhits")) || failed;
  failed = (!ComparisonPassed(fNcherenkovhittimes, c->GetNcherenkovhittimes(), typeid(*this).name(), __func__, "Ncherenkovhittimes")) || failed;
  failed = (!ComparisonPassed(fNcherenkovdigihits, c->GetNcherenkovdigihits(), typeid(*this).name(), __func__, "Ncherenkovdigihits")) || failed;
  //don't expect this to pass in general, so don't affect failed
  ComparisonPassed(fNcherenkovdigihits_slots, c->GetNcherenkovdigihits_slots(), typeid(*this).name(), __func__, "Ncherenkovdigihits_slots (shouldn't necessarily be equal)");
  failed = (!ComparisonPassed(fSumQ, c->GetSumQ(), typeid(*this).name(), __func__, "SumQ")) || failed;
  failed = (!ComparisonPassed(fTriggerType, c->GetTriggerType(), typeid(*this).name(), __func__, "TriggerType")) || failed;
  failed = (!ComparisonPassedVec(fTriggerInfo, c->GetTriggerInfo(), typeid(*this).name(), __func__, "TriggerInfo")) || failed;

  return !failed;
}

//_____________________________________________________________________________
bool WCSimRootEvent::CompareAllVariables(const WCSimRootEvent * c) const
{
  bool failed = false;

  if(this->GetNumberOfEvents() != c->GetNumberOfEvents()) {
    cerr << "WCSimRootEvent::CompareAllVariables() Different number of events: " << this->GetNumberOfEvents() << ", " << c->GetNumberOfEvents() << endl;
    failed = true;
  }

  for(int i = 0; i < TMath::Min(this->GetNumberOfEvents(), c->GetNumberOfEvents()); i++) {
    failed = !(this->GetTrigger(i)->CompareAllVariables(c->GetTrigger(i))) || failed;
  }

  return !failed;
}


//_____________________________________________________________________________

//  LocalWords:  GetCherenkovDigiHits
