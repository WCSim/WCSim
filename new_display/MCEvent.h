#ifndef _MCEVENT_H
#define _MCEVENT_H

#include "TObject.h"
#include <iostream>
#include <vector>
#include <cmath>
#include "include/WCSimRootEvent.hh"
#include "include/WCSimRootGeom.hh"
#include <map>

/* ************************************************************* */
/* Event Display for WCSIM */
/* Xin Qian mailto: xqian@caltech.edu */

/* Modified from run_wcmap.cc from C. Walter of Duke */
/* ************************************************************* */
/* ************************************************************* */

/* Main features:  */
/* 1. Add GUI */
/* 2. Give the status of current event */
/* 3. Can Select to only display one or multiple particles out */
/*    of all the particles. */
/* 4. Change the color scale (log vs linear) */
/* 5. Three mode display, timing, energy, and special display  */
/*    for pi0 */
/* 6. With vertex/ring information or not */
/* 7. Can place a cut of NPE per pmt to clear plot */
/* 8. Can select set number for saving the plot */
/* 9  Can change index of refraction if needed (can used to  */
/*    project the ring) */
/* 10. Can select event no (other than next and prev) */
/* 11. Support both Mailbox and Cylinder shape.  */
/* 12. Can do both batch mode or interactive mode */
/*     see example in event_display.C */
/* ************************************************************** */



class MCEvent : public TObject {
protected:
  WCSimRootTrigger* wcsimrootevent;
  WCSimRootGeom* wcsimrootgeom;
  WCSimRootCherenkovHit* wcsimrootcherenkovhit;
  
  WCSimRootCherenkovDigiHit* wcsimrootcherenkovdigihit;
  WCSimRootPMT pmt;
  std::map<Int_t,Int_t> PMTmap;     // relates PMT list # to PMT ID

  Float_t energy;    // max charge

  Float_t time[2];      // average, standard deviation
  Float_t digitime[2];  // average, standard deviation
  
  Int_t   hitid;
  Int_t   digihitid;

  Int_t tracks[100];
  
  Int_t ntracks;

public:
  MCEvent() {};
  MCEvent(WCSimRootTrigger* evt, WCSimRootGeom* geom,Int_t* a, Int_t b);
  ~MCEvent() {};

    
  void FindEnergyandTimeScales(); // done as in SuperScan (sort of)

  void    SetCherenkovHit(Int_t i);
  Float_t GetEnergyRatio(Int_t i);       // ith PMT normalized to maxEnergy
  Float_t GetEnergyRatio_s(Int_t i);       // ith PMT normalized to maxEnergy

  Float_t GetEnergy(Int_t i);
  Float_t GetTimeRatio(Int_t i);         // ith PMT normalized to maxTime
  Float_t GetGammaRatio(Int_t i);        // ith PMT
 
  Float_t GetPosition(Int_t i, Int_t j); // ith PMT
  Int_t GetTubeID(Int_t i);

  Float_t GetMaxEnergy() { return energy; }
  Float_t GetMinEnergy() { return 1; }
  Float_t GetMaxTime() { Float_t tmp=time[0]+time[1];return tmp; }
  Float_t GetMinTime() { Float_t tmp=time[0]-time[1];return tmp; }
  Float_t GetMaxDigiTime() { Float_t tmp=digitime[0]+digitime[1];return tmp; }
  Float_t GetMinDigiTime() { Float_t tmp=digitime[0]-digitime[1];return tmp; }
  Float_t GetGammaEnergy(Int_t i) 
  { return wcsimrootevent->GetPi0Info()->GetGammaE(i); }
  Float_t GetGammaVertex(Int_t i, Int_t j)
  { return wcsimrootevent->GetPi0Info()->GetGammaVtx(i,j); }
  Float_t GetRealVertex(Int_t i)
  { return wcsimrootevent->GetVtx(i); }

  void    SetPMTMap();

  void    SetCherenkovDigiHit(Int_t i);
  Float_t GetDigiEnergyRatio(Int_t i); // ith DIGIPMT normalized to maxEnergy
  Float_t GetDigiTimeRatio(Int_t i);   // ith DIGIPMT normalized to maxDigiTime
  Float_t GetDigiGammaRatio(Int_t i);  // ith DIGIPMT
  Float_t GetDigiPosition(Int_t i, Int_t j); // ith DIGIPMT

  void    SetPMT(WCSimRootPMT pmt_);

  ClassDef(MCEvent,1);
};

#endif
