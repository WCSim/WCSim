#ifndef WCSimRunAction_h
#define WCSimRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4String.hh"

#include "TFile.h"
#include "TTree.h"
#include "TStopwatch.h"
#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"
#include "WCSimRootOptions.hh"
#include "WCSimDetectorConstruction.hh"
#include "WCSimRandomParameters.hh"

class G4Run;
class WCSimRunActionMessenger;

class WCSimRunAction : public G4UserRunAction
{
public:
  WCSimRunAction(WCSimDetectorConstruction*, WCSimRandomParameters*);
  ~WCSimRunAction();

public:
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);
  void SetRootFileName(G4String fname) { RootFileName = fname; }
  G4String GetRootFileName() { return RootFileName; }
  void FillGeoTree();
  TTree* GetTree(){return WCSimTree;}
  TTree* GetGeoTree(){return geoTree;}
  TTree* GetOptionsTree(){return optionsTree;}
  WCSimRootGeom* GetRootGeom(){return wcsimrootgeom;}
  WCSimRootEvent* GetRootEvent(){return wcsimrootsuperevent;}
  WCSimRootOptions* GetRootOptions(){return wcsimrootoptions;}

  int  GetNumberOfEventsGenerated() { return numberOfEventsGenerated;}
  int  GetNtuples(){return ntuples;}

  void incrementEventsGenerated() { numberOfEventsGenerated++;}
  void incrementWaterTubeHits()   { numberOfTimesWaterTubeHit++;} 
  void incrementFVWaterTubeHits() { numberOfTimesFVWaterTubeHit++;} 
  void incrementCatcherHits()     { numberOfTimesCatcherHit++;}
  void SetNtuples(int ntup) {ntuples=ntup;}

  void SetUseTimer(bool use) { useTimer = use; }
  
private:
  // MFechner : set by the messenger
  std::string RootFileName;
  //
  TTree* WCSimTree;
  TTree* geoTree;
  TTree* optionsTree;
  WCSimRootEvent* wcsimrootsuperevent;
  WCSimRootGeom* wcsimrootgeom;
  WCSimRootOptions* wcsimrootoptions;
  WCSimDetectorConstruction* wcsimdetector;
  WCSimRandomParameters* wcsimrandomparameters;

  int numberOfEventsGenerated;
  int numberOfTimesWaterTubeHit;
  int numberOfTimesFVWaterTubeHit;
  int numberOfTimesCatcherHit;

  WCSimRunActionMessenger* messenger;
  int ntuples;  // 1 for ntuples to be written

  bool useTimer; ///< Use the timer? Set by Messenger.
  TStopwatch timer; ///< A timer for runtime analysis
};

#endif
