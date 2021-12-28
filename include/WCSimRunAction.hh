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
  TBranch* GetBranch(G4String detectorElement = "tank"){
    if(detectorElement=="tank") return wcsimrooteventbranch;
    else if(detectorElement=="OD")  return wcsimrooteventbranch_OD;
    else {
      G4cout << "Unkown detector element" << G4endl;
      return nullptr;
    }
  }
  TTree* GetGeoTree(){return geoTree;}
  TTree* GetOptionsTree(){return optionsTree;}
  WCSimRootGeom* GetRootGeom(){return wcsimrootgeom;}
  // WCSimRootEvent* GetRootEvent(){return wcsimrootsuperevent;}
  WCSimRootEvent* GetRootEvent(G4String detectorElement = "tank"){
    if(detectorElement=="tank") return wcsimrootsuperevent;
    else if(detectorElement=="OD") return wcsimrootsuperevent_OD;
    else {
      G4cout << "Unkown detector element" << G4endl;
      return nullptr;
    }
  }
  WCSimRootOptions* GetRootOptions(){return wcsimrootoptions;}


  void SetTree(TTree* tree){WCSimTree=tree;}
  void SetBranch(TBranch* branchin, G4String detectorElement = "tank"){
    if(detectorElement=="tank") wcsimrooteventbranch=branchin;
    if(detectorElement=="OD") wcsimrooteventbranch_OD=branchin;
  }
  void SetGeoTree(TTree* tree){geoTree=tree;}
  void SetRootEvent(WCSimRootEvent* revent, G4String detectorElement = "tank"){
    if(detectorElement=="tank") wcsimrootsuperevent=revent;
    if(detectorElement=="OD") wcsimrootsuperevent_OD=revent;
  }
  void SetRootGeom(WCSimRootGeom* rgeom){wcsimrootgeom=rgeom;}

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
  TBranch* wcsimrooteventbranch;
  TBranch* wcsimrooteventbranch_OD;
  TTree* geoTree;
  TTree* optionsTree;
  WCSimRootEvent* wcsimrootsuperevent;
  WCSimRootEvent* wcsimrootsuperevent_OD;
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
