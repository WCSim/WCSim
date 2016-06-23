/*
  Macro that compares the events in two WCSim output files
  Utilizes the WCSimRoot comparison methods: CompareAllVariables(),
   to ensure that everything is equivalent
 */

#include <iostream>
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

void PrintHeaderInfo(WCSimRootEvent * wcsimrootsuperevent, const char * filename)
{
  WCSimRootTrigger * wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  printf("********************************************************");
  cout << "Header+ in WCSim file: " << filename << endl;
  printf("Evt, date %d %d\n", wcsimrootevent->GetHeader()->GetEvtNum(),
	 wcsimrootevent->GetHeader()->GetDate());
  printf("Mode %d\n", wcsimrootevent->GetMode());
  printf("Number of subevents %d\n",
	 wcsimrootsuperevent->GetNumberOfSubEvents());
  
  printf("Vtxvol %d\n", wcsimrootevent->GetVtxvol());
  printf("Vtx %f %f %f\n", wcsimrootevent->GetVtx(0),
	 wcsimrootevent->GetVtx(1),wcsimrootevent->GetVtx(2));

  cout << "Number of tube hits " << wcsimrootevent->GetNumTubesHit() << endl;
  cout << "Number of digitized tube hits " << wcsimrootevent->GetNumDigiTubesHit() << endl;
  cout << "Number of photoelectron hit times " << wcsimrootevent->GetCherenkovHitTimes()->GetEntries() << endl;
}

TTree * GetTree(const char * filename)
{
  TFile *f = new TFile(filename,"read");
  if (!f->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    exit(-1);
  }
  
  TTree  * wcsimT = 0;
  f->GetObject("wcsimT", wcsimT);
  if(!wcsimT) {
    cerr << "wcsimT tree could not be opened in input file: " << filename << endl;
    exit(-1);
  }
  return wcsimT;
}

WCSimRootEvent * GetRootEvent(TTree * wcsimT, int & nevent)
{
  cout << wcsimT << endl;
  nevent = wcsimT->GetEntries();
  WCSimRootEvent * wcsimrootsuperevent = 0;
  wcsimT->SetBranchAddress("wcsimrootevent",&wcsimrootsuperevent);

  // Force deletion to prevent memory leak when issuing multiple
  // calls to GetEvent()
  wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);

  wcsimT->GetEvent(0); 
  return wcsimrootsuperevent;
}

// Simple example of reading a generated Root file
void complete_comparison(char *filename1, char *filename2, bool verbose=false, const Long64_t oneevent = -1)
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
  TTree * wcsimT1 = GetTree(filename1);
  WCSimRootEvent * wcsimrootsuperevent1 = GetRootEvent(wcsimT1, nevents1);
  WCSimRootTrigger* wcsimrootevent1;

  int nevents2;
  TTree * wcsimT2 = GetTree(filename2);
  WCSimRootEvent * wcsimrootsuperevent2 = GetRootEvent(wcsimT2, nevents2);
  WCSimRootTrigger* wcsimrootevent2;

  cout << "File 1 (2) has " << nevents1 << " (" << nevents2 << ") events" << endl;

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
    nevents_analysed++;
    // Read the event from the tree into the WCSimRootEvent instance
    wcsimT1->GetEntry(ev);
    wcsimT2->GetEntry(ev);
    if(verbose || ev == 0){
      PrintHeaderInfo(wcsimrootsuperevent1, filename1);
      PrintHeaderInfo(wcsimrootsuperevent2, filename2);
    }

    //compare
    bool failed = !(wcsimrootsuperevent1->CompareAllVariables(wcsimrootsuperevent2));
    const int ntriggers = TMath::Min(wcsimrootsuperevent1->GetNumberOfEvents(), wcsimrootsuperevent2->GetNumberOfEvents());
    for(int itrig = 0; itrig < ntriggers; itrig++) {
      wcsimrootevent1 = wcsimrootsuperevent1->GetTrigger(itrig);
      wcsimrootevent2 = wcsimrootsuperevent2->GetTrigger(itrig);
      //compare triggers
      failed = !(wcsimrootevent1->CompareAllVariables(wcsimrootevent2)) || failed;
      failed = !(wcsimrootevent1->GetPi0Info()->CompareAllVariables(wcsimrootevent2->GetPi0Info())) || failed;
      failed = !(wcsimrootevent1->GetHeader ()->CompareAllVariables(wcsimrootevent2->GetHeader()))  || failed;
    }
    if(verbose || ev == 0)
      cout << "Event " << ev << " comparison " << (failed ? "FAILED" : "PASSED") << endl;
    if(failed)
      nevents_failed++;

    // reinitialize super event between loops.
    wcsimrootsuperevent1->ReInitialize();
    wcsimrootsuperevent2->ReInitialize();
  }// End of loop over events
  cout << nevents_failed << " events out of " << nevents_analysed << " have failures " << endl;
}
