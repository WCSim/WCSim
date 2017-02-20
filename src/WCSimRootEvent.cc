// Based on Root test Event.cxx
////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TDirectory.h"
#include "TProcessID.h"
#include <string>
#include <vector>

#include <TStopwatch.h>
#include "WCSimRootEvent.hh"

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

  // TClonesArray of WCSimRootCherenkovHits
  fCherenkovHits = 0;
  fCherenkovHitTimes = 0;
  fNcherenkovhits = 0;
  fNcherenkovhittimes = 0;

  // TClonesArray of WCSimRootCherenkovDigiHits
  fCherenkovDigiHits = 0;
  fNcherenkovdigihits = 0;
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
  fNtrack = 0;

  // TClonesArray of WCSimRootCherenkovHits
  fCherenkovHits = new TClonesArray("WCSimRootCherenkovHit", 
				    10000);
  fCherenkovHitTimes = new TClonesArray("WCSimRootCherenkovHitTime", 
					10000);
  fNcherenkovhits = 0;
  fNcherenkovhittimes = 0;

  // TClonesArray of WCSimRootCherenkovDigiHits
  fCherenkovDigiHits = new TClonesArray("WCSimRootCherenkovDigiHit", 
				       10000);
  fNcherenkovdigihits = 0;
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

void WCSimRootTrigger::Clear(Option_t */*option*/)
{
  // To be filled in 
  // Filled in, by MF, 31/08/06  -> Keep all the alloc'ed memory but reset all
  // the indices to 0 in the TCAs.
  fNtrack = 0;

  // TClonesArray of WCSimRootCherenkovHits
  fNcherenkovhits = 0;
  fNcherenkovhittimes = 0;

  // TClonesArray of WCSimRootCherenkovDigiHits
  fNcherenkovdigihits = 0;
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
    new(tracks[fNtrack++]) WCSimRootTrack(ipnu,
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

  return track;
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
    new(cherenkovdigihits[fNcherenkovdigihits++]) WCSimRootCherenkovDigiHit(q, 
									    t, 
									    tubeid,
									    photon_ids);
 
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
  //nothing for now
}

void WCSimRootEvent::Reset(Option_t* /*o*/)
{
  //nothing for now
}



