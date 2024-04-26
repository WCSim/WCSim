#ifndef WCSimRunAction_h
#define WCSimRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4String.hh"

#include "TFile.h"
#include "TTree.h"

#include "TClonesArray.h"

#include "TStopwatch.h"

#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"
#include "WCSimRootOptions.hh"
#include "WCSimDetectorConstruction.hh"
#include "WCSimEnumerations.hh"                //ToDo: move more Enums over there!
#include "evNtuple.h"
#include "WCSimRandomParameters.hh"


#include "TNRooTrackerVtx.hh"

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
  void SetSaveRooTracker(G4bool fsave) { SaveRooTracker = fsave; }
  G4String GetRootFileName() const { return RootFileName; }
  void SetOptionalRootFile(G4bool choice) { useDefaultROOTout = choice; }
  G4bool GetRootFileOption() const { return useDefaultROOTout; }
  bool GetSaveRooTracker() const { return SaveRooTracker; }
  void FillGeoTree();
  TTree* GetTree() const {return WCSimTree;}
  TBranch* GetBranch(G4String detectorElement = "tank"){
    if(detectorElement=="tank") return wcsimrooteventbranch;
    else if(detectorElement=="tankPMT2")  return wcsimrooteventbranch2;
    else if(detectorElement=="OD")  return wcsimrooteventbranch_OD;
    else G4cout << "Unkown detector element: " << detectorElement << G4endl;
    return nullptr;
  }
  TTree* GetGeoTree() const {return geoTree;}
  TTree* GetOptionsTree() const {return optionsTree;}
  WCSimRootGeom* GetRootGeom() const {return wcsimrootgeom;}
  WCSimRootEvent* GetRootEvent(G4String detectorElement = "tank") const{
    if(detectorElement=="tank") return wcsimrootsuperevent;
    else if(detectorElement=="tankPMT2") return wcsimrootsuperevent2;
    else if(detectorElement=="OD") return wcsimrootsuperevent_OD;
    else G4cout << "Unkown detector element: " << detectorElement << G4endl;
    return nullptr;
  }
  WCSimRootOptions* GetRootOptions() const {return wcsimrootoptions;}

  void SetTree(TTree* tree){WCSimTree=tree;}
  void SetBranch(TBranch* branchin, G4String detectorElement = "tank"){
    if(detectorElement=="tank") wcsimrooteventbranch=branchin;
    else if(detectorElement=="tankPMT2") wcsimrooteventbranch2=branchin;
    else if(detectorElement=="OD") wcsimrooteventbranch_OD=branchin;
  }
  void SetGeoTree(TTree* tree){geoTree=tree;}
  void SetRootEvent(WCSimRootEvent* revent, G4String detectorElement = "tank"){
    if(detectorElement=="tank") wcsimrootsuperevent=revent;
    else if(detectorElement=="tankPMT2") wcsimrootsuperevent2=revent;
    else if(detectorElement=="OD") wcsimrootsuperevent_OD=revent;
  }
  void SetRootGeom(WCSimRootGeom* rgeom){wcsimrootgeom=rgeom;}

  int  GetNtuples() const {return ntuples;}

  void SetNtuples(int ntup) {ntuples=ntup;}
  
  //New:
  NRooTrackerVtx* GetRootrackerVertex();
  void FillRootrackerVertexTree() { 
    fRooTrackerOutputTree->Fill();
  }
  void ClearRootrackerVertexArray() { 
      fVertices->Clear(); 
      fNVtx = 0;
  }

  void SetUseTimer(bool use) { useTimer = use; }

  int GetRunID() const { return run; }
  void SetRunID(int runID) { run = runID; }

 private:
  // MFechner : set by the messenger
  std::string RootFileName;
  // Only required for verification scripts and current fiTQun tuning
  // But making initialization very slow due to large TCloneArray init.
  G4bool useDefaultROOTout;

	int run;

  //
  TTree* WCSimTree;
  TBranch* wcsimrooteventbranch;
  TBranch* wcsimrooteventbranch2;
  TBranch* wcsimrooteventbranch_OD;
  TTree* geoTree;
  TTree* optionsTree;
  WCSimRootEvent* wcsimrootsuperevent;
  WCSimRootEvent* wcsimrootsuperevent2;
  WCSimRootEvent* wcsimrootsuperevent_OD;
  WCSimRootGeom* wcsimrootgeom;
  WCSimRootOptions* wcsimrootoptions;
  WCSimDetectorConstruction* wcsimdetector;
  WCSimRandomParameters* wcsimrandomparameters;

  TClonesArray* fVertices;
  TTree* fRooTrackerOutputTree;
  int fNVtx;
  bool SaveRooTracker;
  TTree* fSettingsOutputTree;
  TTree* fSettingsInputTree;

  double WCXRotation[3];
  double WCYRotation[3];
  double WCZRotation[3];
  double WCDetCentre[3];
  double WCDetRadius;
  double WCDetHeight;
  float  fNuPlanePos[3];
  float  fNuPrismRadius;

  WCSimRunActionMessenger* messenger;
  int ntuples;  // 1 for ntuples to be written

  // GeoTree
  Char_t geo_type_string[20];   
  double cyl_radius;     
  double cyl_length;     
  Char_t pmt_id_string[50];
  double pmt_radius_id;
  Char_t pmt_od_string[50];
  double pmt_radius_od;
  int numPMT_id;
  int numPMT_od;
  int orient; 
  double offset_x;
  double offset_y;
  double offset_z;
  int num_mPMT;
  int tube_id[5000000];
  int mPMT_id[5000000];
  int mPMT_pmt_id[5000000];
  double tube_x[5000000];
  double tube_y[5000000];
  double tube_z[5000000];
  double dir_x[5000000];
  double dir_y[5000000];
  double dir_z[5000000];
  double theta[5000000];
  double phi[5000000];
  
  enum CylinderLocation{
    UNDEFINED = -1,
    WALL = 0,
    TOPCAP = 10,
    BOTTOMCAP = 20
  } cylLocation[5000000];
  
  const G4Run* fG4Run;

  bool useTimer; ///< Use the timer? Set by Messenger.
  TStopwatch timer; ///< A timer for runtime analysis

};

#endif
