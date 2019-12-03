/*
  Macro that compares the events in two WCSim output files
  Utilizes the WCSimRoot comparison methods: CompareAllVariables(),
   to ensure that everything is equivalent
 */

#include <iostream>
#include <string>
#include <stdio.h>     
#include <stdlib.h>    

#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>
#include <TMath.h>

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"
#include "WCSimEnumerations.hh"
#endif

using namespace std;

class WCSimRootEvent;

void PrintHeaderInfo(WCSimRootEvent * wcsimrootsuperevent, const char * filename)
{
  cout << "********************************************************" << endl;
  WCSimRootTrigger * wcsimrootevent;
  for(int itrigger = 0; itrigger < wcsimrootsuperevent->GetNumberOfEvents(); itrigger++) {
    wcsimrootevent = wcsimrootsuperevent->GetTrigger(itrigger);
    if(itrigger == 0)
      cout << "Header+ in WCSim file: " << filename << endl;
    else
      cout << "-----------" << endl;
    cout << "Evt, subevent: " << wcsimrootevent->GetHeader()->GetEvtNum() << ", "
	 << itrigger << endl;
      cout << "Date: " << wcsimrootevent->GetHeader()->GetDate() << endl;
    cout << "Mode: " << wcsimrootevent->GetMode() << endl;
    cout << "Number of subevents: " << wcsimrootsuperevent->GetNumberOfSubEvents() << endl;
    if(itrigger == 0) {
      cout << "Vtxvol: " << wcsimrootevent->GetVtxvol() << endl;
      for(int i = 0; i < 3; i++)
	cout << "Vtx[" << i << "]: " << wcsimrootevent->GetVtx(i) << endl;
      cout << "Number of photoelectron hit times " << wcsimrootevent->GetCherenkovHitTimes()->GetEntries() << endl;
      cout << "Number of tube hits " << wcsimrootevent->GetNumTubesHit() << endl;
    }
    cout << "Number of tracks " << wcsimrootevent->GetNtrack() << endl;
    cout << "Number of digitized tube hits " << wcsimrootevent->GetNumDigiTubesHit() << endl;
  }//itrigger
}

void PrintGeomSummary(WCSimRootGeom * geo, const char * filename)
{
  cout << "********************************************************" << endl;
  cout << "Geom summary+ in WCSim file: " << filename << endl;
  cout << "WC radius " << geo->GetWCCylRadius() << endl;
  cout << "WC length " << geo->GetWCCylLength() << endl;
  cout << "Number of PMTs " << geo->GetWCNumPMT() << endl;
  cout << "PMT radius " << geo->GetWCPMTRadius() << endl;
}

TTree * GetTree(const char * filename, const char * treename)
{
  TFile *f = new TFile(filename,"read");
  if (!f->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    exit(-1);
  }
  
  TTree  * wcsimT = 0;
  f->GetObject(treename, wcsimT);
  if(!wcsimT) {
    cerr << treename << " tree could not be opened in input file: " << filename << endl;
    exit(-1);
  }
  return wcsimT;
}

WCSimRootEvent * GetRootEvent(TTree * wcsimT, int & nevent)
{
  nevent = wcsimT->GetEntries();
  WCSimRootEvent * wcsimrootsuperevent = 0;
  wcsimT->SetBranchAddress("wcsimrootevent",&wcsimrootsuperevent);

  // Force deletion to prevent memory leak when issuing multiple
  // calls to GetEvent()
  wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);

  wcsimT->GetEvent(0); 
  return wcsimrootsuperevent;
}

WCSimRootGeom * GetRootGeom(TTree * wcsimT, int & nevent)
{
  nevent = wcsimT->GetEntries();
  WCSimRootGeom * wcsimrootgeom = 0;
  wcsimT->SetBranchAddress("wcsimrootgeom", &wcsimrootgeom);
  wcsimT->GetEvent(0);
  return wcsimrootgeom;
}

// Simple example of reading a generated Root file
void complete_comparison(char *filename1, char *filename2, string treename = "wcsimT", bool verbose=false, const Long64_t oneevent = -1)
{
#if !defined(__MAKECINT__)
  // Load the library with class dictionary info
  // (create with "gmake shared")
  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIMDIR");
  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
  }else{
    gSystem->Load("../libWCSimRoot.so");
  }
#endif

  int nevents1;
  TTree * wcsimT1 = GetTree(filename1, treename.c_str());

  int nevents2;
  TTree * wcsimT2 = GetTree(filename2, treename.c_str());

  WCSimRootEvent * wcsimrootsuperevent1 = 0;
  WCSimRootGeom * wcsimrootgeom1 = 0;
  WCSimRootEvent * wcsimrootsuperevent2 = 0;
  WCSimRootGeom * wcsimrootgeom2 = 0;
  if(treename == "wcsimT") {
    wcsimrootsuperevent1 = GetRootEvent(wcsimT1, nevents1);
    wcsimrootsuperevent2 = GetRootEvent(wcsimT2, nevents2);
  }
  else if(treename == "wcsimGeoT") {
    wcsimrootgeom1 = GetRootGeom(wcsimT1, nevents1);
    wcsimrootgeom2 = GetRootGeom(wcsimT2, nevents2);
  }
  else {
    cerr << treename << " is not a valid treename for complete_comparison(). Exiting" << endl;
    return;
  }

  cout << "Tree " << treename << " in file 1 (2) has " << nevents1 << " (" << nevents2 << ") events" << endl;

  // Now loop over events
  int nevents_failed = 0, nevents_analysed = 0;
  int nevent = TMath::Min(nevents1, nevents2);
  int ev = 0;
  if(oneevent >= 0) {
    ev = oneevent;
    nevent = oneevent + 1;
  }
  for ( ; ev < nevent; ev++)
  {
    cout << "Comparing event " << ev << endl;
    nevents_analysed++;
    // Read the event from the tree into the WCSimRootEvent instance
    wcsimT1->GetEntry(ev);
    wcsimT2->GetEntry(ev);

    //print
    if(verbose || ev == 0){
	if(wcsimrootsuperevent1) {
	  PrintHeaderInfo(wcsimrootsuperevent1, filename1);
	  PrintHeaderInfo(wcsimrootsuperevent2, filename2);
	}
	else if(wcsimrootgeom1) {
	  PrintGeomSummary(wcsimrootgeom1, filename1);
	  PrintGeomSummary(wcsimrootgeom2, filename2);
	}
    }
      
    //compare
    bool failed = false;
    if(wcsimrootsuperevent1) {
      failed = !(wcsimrootsuperevent1->CompareAllVariables(wcsimrootsuperevent2, true));
    }
    else if(wcsimrootgeom1) {
      failed = !(wcsimrootgeom1->CompareAllVariables(wcsimrootgeom2));
    }

    if(verbose || ev == 0)
      cout << "Event " << ev << " comparison " << (failed ? "FAILED" : "PASSED") << endl;
    if(failed)
      nevents_failed++;

    if(wcsimrootsuperevent1) {
      // reinitialize super event between loops.
      wcsimrootsuperevent1->ReInitialize();
      wcsimrootsuperevent2->ReInitialize();
    }
  }// End of loop over events
  cout << nevents_failed << " events out of " << nevents_analysed << " have failures " << endl;
}
