#include "WCSimRunAction.hh"
#include "WCSimRunActionMessenger.hh"

#include "G4Run.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"

#include "jhfNtuple.h"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TStreamerInfo.h"
#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"

int pawc_[500000];                // Declare the PAWC common
struct ntupleStruct jhfNtuple;

WCSimRunAction::WCSimRunAction()
{
  ntuples = 1;

  // Messenger to allow IO options

  messenger = new WCSimRunActionMessenger(this);

}

WCSimRunAction::~WCSimRunAction()
{

}

void WCSimRunAction::BeginOfRunAction(const G4Run* aRun)
{
//   G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
  numberOfEventsGenerated = 0;
  numberOfTimesWaterTubeHit = 0;
  numberOfTimesCatcherHit = 0;

  // Needed for Root 4.00/04
  WCSimRootEvent::Class()->GetStreamerInfo()->Optimize(kFALSE);
  // MF, aug 2006 ... you never know...
  WCSimRootTrigger::Class()->GetStreamerInfo()->Optimize(kFALSE);

  // Create the Root file

  // Now controlled by the messenger
  
  G4String rootname = GetRootFileName();
  TFile* hfile = new TFile(rootname.c_str(),"RECREATE","WCSim ROOT file");
  hfile->SetCompressionLevel(2);

  // Event tree
  TTree* tree = new TTree("wcsimT","WCSim Tree");

  SetTree(tree);
  wcsimrootsuperevent = new WCSimRootEvent(); //empty list
  //  wcsimrootsuperevent->AddSubEvent(); // make at least one event
  wcsimrootsuperevent->Initialize(); // make at least one event
  Int_t branchStyle = 1; //new style by default
  TTree::SetBranchStyle(branchStyle);
  Int_t bufsize = 64000;

  //  TBranch *branch = tree->Branch("wcsimrootsuperevent", "Jhf2kmrootsuperevent", &wcsimrootsuperevent, bufsize,0);
  TBranch *branch = tree->Branch("wcsimrootevent", "WCSimRootEvent", &wcsimrootsuperevent, bufsize,2);

  // Geometry tree

  geoTree = new TTree("wcsimGeoT","WCSim Geometry Tree");
  SetGeoTree(geoTree);
  wcsimrootgeom = new WCSimRootGeom();
  TBranch *geoBranch = geoTree->Branch("wcsimrootgeom", "WCSimRootGeom", &wcsimrootgeom, bufsize,0);

  FillGeoTree();
}

void WCSimRunAction::EndOfRunAction(const G4Run*)
{
//G4cout << "Number of Events Generated: "<< numberOfEventsGenerated << G4endl;
//G4cout << "Number of times MRD hit: " << numberOfTimesMRDHit << G4endl;
//G4cout << "Number of times FGD hit: "    << numberOfTimesFGDHit << G4endl;
//G4cout << "Number of times lArD hit: "  << numberOfTimeslArDHit << G4endl;
//G4cout<<"Number of times waterTube hit: " << numberOfTimesWaterTubeHit<<G4endl;
//   G4cout << ((float(numberOfTimesMRDHit)+float(numberOfTimesFGDHit))/float(numberOfEventsGenerated))*100.
// 	 << "% hit FGD or MRD" << G4endl;
//   G4cout << "Number of times Catcher hit: " << numberOfTimesCatcherHit<<G4endl;
//   G4cout << "Number of times Rock hit: " << numberOfTimesRockHit<<G4endl;
//  G4cout << (float(numberOfTimesCatcherHit)/float(numberOfEventsGenerated))*100.
//        << "% through-going (hit Catcher)" << G4endl;

  // Close the Root file at the end of the run

  TFile* hfile = WCSimTree->GetCurrentFile();
  hfile->Close();

  // Clean up stuff on the heap; I think deletion of hfile and trees
  // is taken care of by the file close

  delete wcsimrootsuperevent; wcsimrootsuperevent=0;
  delete wcsimrootgeom; wcsimrootgeom=0;

}

void WCSimRunAction::FillGeoTree(){

 // Fill the geometry tree

  // Open and read the geometry file
  
  std::ifstream geoFile;
  geoFile.open("geofile.txt", std::ios::in);
  int itube=0;
  if (geoFile){
    G4double cylinfo[2];
    G4double pmtradius;
    G4int numpmt;
    G4int orientation;

    geoFile>> cylinfo[0]>>cylinfo[1]>>numpmt>>pmtradius >> orientation;
    wcsimrootgeom-> SetWCCylRadius(cylinfo[0]);
    wcsimrootgeom-> SetWCCylLength(cylinfo[1]);
    wcsimrootgeom-> SetWCPMTRadius(pmtradius);
    wcsimrootgeom-> SetOrientation(orientation);

    Float_t offset[3];
    geoFile>>offset[0]>>offset[1]>>offset[2];
    wcsimrootgeom-> SetWCOffset(offset[0],offset[1],offset[2]);


    // Now the tube list
    Int_t tubeNo;
    while (geoFile>>tubeNo){
      Float_t pos[3];
      Float_t rot[3];
      Int_t cylLoc;
      geoFile>>pos[0]>>pos[1]>>pos[2]>>rot[0]>>rot[1]>>rot[2]>>cylLoc;
      //      G4cout<< itube<<" "<<tubeNo<<" "<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<" "<<rot[0]<<" "<<rot[1]<<" "<<rot[2]<<G4endl;
      wcsimrootgeom-> SetPMT(itube,tubeNo,cylLoc,rot,pos);
      itube++;

    }
    if (itube != numpmt) {
	G4cout << "Mismatch between number of pmts and pmt list in geofile.txt!!"<<G4endl;
	G4cout << itube <<" vs. "<< numpmt <<G4endl;
    }
    geoFile.close();
  }

  wcsimrootgeom-> SetWCNumPMT(itube);
  
  geoTree->Fill();
  TFile* hfile = geoTree->GetCurrentFile();
  hfile->Write(); 

}
