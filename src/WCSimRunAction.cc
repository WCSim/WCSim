#include "WCSimRunAction.hh"
#include "WCSimRunActionMessenger.hh"

#include "G4Run.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"

#include "jhfNtuple.h"

#ifdef REFLEX_DICTIONARY
#include "Cintex/Cintex.h"
#endif
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TStreamerInfo.h"
#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"
#include "WCSimPmtInfo.hh"

#include <vector>

int pawc_[500000];                // Declare the PAWC common
struct ntupleStruct jhfNtuple;    // global, ToDo: why not use and set the class member?

WCSimRunAction::WCSimRunAction(const WCSimDetectorConstruction* test, WCSimRandomParameters* rand)
  : wcsimrandomparameters(rand), useTimer(false)
{
  ntuples = 1;

  // Messenger to allow IO options
  wcsimdetector = test;
  messenger = new WCSimRunActionMessenger(this);

  useDefaultROOTout = true;
  wcsimrootoptions = new WCSimRootOptions();

  // By default do not try and save Rootracker interaction information
  SetSaveRooTracker(0);

}

WCSimRunAction::~WCSimRunAction()
{

}

void WCSimRunAction::BeginOfRunAction(const G4Run* aRun)
{
  run = aRun->GetRunID();

  fSettingsOutputTree = NULL;
  fSettingsInputTree = NULL;
  for(int i = 0; i < 3; ++i){
      WCXRotation[i] = 0;
      WCYRotation[i] = 0;
      WCZRotation[i] = 0;
      WCDetCentre[i] = 0;
  }

  WCDetRadius = 0;
  WCDetHeight = 0;

  if(wcsimdetector->GetIsNuPrism()){
    if(GetSaveRooTracker()){
      //Setup settings tree
      // Assume the NEUT file is open.
      fSettingsInputTree = (TTree*) gDirectory->Get("Settings");
      fSettingsInputTree->SetBranchAddress("NuIdfdPos",fNuPlanePos);
      fSettingsInputTree->SetBranchAddress("DetRadius",&fNuPrismRadius);
    }
  }

  if(useTimer) {
    timer.Reset();
    timer.Start();
  }
  
#ifdef REFLEX_DICTIONARY
  ROOT::Cintex::Cintex::Enable();
#endif

  // Needed for Root 4.00/04
  WCSimRootEvent::Class()->GetStreamerInfo()->Optimize(kFALSE);
  // MF, aug 2006 ... you never know...
  WCSimRootTrigger::Class()->GetStreamerInfo()->Optimize(kFALSE);

  // Create the Root file

  // Now controlled by the messenger
  G4String rootname = GetRootFileName();
  
  if(useDefaultROOTout){

    wcsimrootsuperevent = new WCSimRootEvent(); //empty list
    wcsimrootsuperevent2 = new WCSimRootEvent(); //empty list
    wcsimrootsuperevent_OD = new WCSimRootEvent(); //empty list
    //  wcsimrootsuperevent->AddSubEvent(); // make at least one event
    wcsimrootsuperevent->Initialize(); // make at least one event
    wcsimrootsuperevent2->Initialize(); // make at least one event
    wcsimrootsuperevent_OD->Initialize(); // make at least one event

    if (run == 0) {
      TFile *hfile = new TFile(rootname.c_str(), "RECREATE", "WCSim ROOT file");
      hfile->SetCompressionLevel(2);

      if (wcsimdetector->GetIsNuPrism()) {
	if (fSettingsInputTree) {
	  fSettingsOutputTree = fSettingsInputTree->CloneTree(0);
	} else {
	  fSettingsOutputTree = new TTree("Settings", "Settings");
	}

	fSettingsOutputTree->Branch("WCXRotation", WCXRotation, "WCXRotation[3]/D");
	fSettingsOutputTree->Branch("WCYRotation", WCYRotation, "WCYRotation[3]/D");
	fSettingsOutputTree->Branch("WCZRotation", WCZRotation, "WCZRotation[3]/D");
	fSettingsOutputTree->Branch("WCDetCentre", WCDetCentre, "WCDetCentre[3]/D");
	fSettingsOutputTree->Branch("WCDetRadius", &WCDetRadius, "WCDetRadius/D");
	fSettingsOutputTree->Branch("WCDetHeight", &WCDetHeight, "WCDetHeight/D");

#ifdef GIT_HASH
	const char* gitHash = GIT_HASH;
	fSettingsOutputTree->Branch("GitHash", (void*)gitHash, "GitHash/C");
#endif
      }//nuPRISM settings tree

      // Event tree
      WCSimTree = new TTree("wcsimT", "WCSim Tree");

      Int_t branchStyle = 1; //new style by default
      TTree::SetBranchStyle(branchStyle);
      Int_t bufsize = 64000;

      //  TBranch *branch = tree->Branch("wcsimrootsuperevent", "Jhf2kmrootsuperevent", &wcsimrootsuperevent, bufsize,0);
      wcsimrooteventbranch = WCSimTree->Branch("wcsimrootevent", "WCSimRootEvent", &wcsimrootsuperevent, bufsize, 2);
      wcsimrooteventbranch2 = WCSimTree->Branch("wcsimrootevent2", "WCSimRootEvent", &wcsimrootsuperevent2, bufsize, 2);
      wcsimrooteventbranch_OD = WCSimTree->Branch("wcsimrootevent_OD", "WCSimRootEvent", &wcsimrootsuperevent_OD, bufsize, 2);

      // Geometry tree
      geoTree = new TTree("wcsimGeoT", "WCSim Geometry Tree");
      wcsimrootgeom = new WCSimRootGeom();
      geoTree->Branch("wcsimrootgeom", "WCSimRootGeom", &wcsimrootgeom, bufsize, 0);
      FillGeoTree();

      // Options tree
      optionsTree = new TTree("wcsimRootOptionsT", "WCSim Options Tree");
      optionsTree->Branch("wcsimrootoptions", "WCSimRootOptions", &wcsimrootoptions, bufsize, 0);

      //set detector & random options
      wcsimdetector->SaveOptionsToOutput(wcsimrootoptions);
      wcsimrandomparameters->SaveOptionsToOutput(wcsimrootoptions);

      //Setup rooTracker tree
      if (SaveRooTracker) {
	//Setup TClonesArray to store Rootracker truth info
	fVertices = new TClonesArray("NRooTrackerVtx", 10);
	fVertices->Clear();
	fNVtx = 0;
	fRooTrackerOutputTree = new TTree("fRooTrackerOutputTree", "Event Vertex Truth Array");
	fRooTrackerOutputTree->Branch("NVtx", &fNVtx, "NVtx/I");
	fRooTrackerOutputTree->Branch("NRooTrackerVtx", "TClonesArray", &fVertices);
      }
    }
    else{
      TFile *hfile = new TFile(rootname.c_str(), "UPDATE");
      WCSimTree = (TTree*) hfile->Get("wcsimT");
      wcsimrooteventbranch = WCSimTree->GetBranch("wcsimrootevent");
      wcsimrooteventbranch2 = WCSimTree->GetBranch("wcsimrootevent2");
      wcsimrooteventbranch_OD = WCSimTree->GetBranch("wcsimrootevent_OD");
      wcsimrooteventbranch->SetAddress(&wcsimrootsuperevent);
      wcsimrooteventbranch2->SetAddress(&wcsimrootsuperevent2);
      wcsimrooteventbranch_OD->SetAddress(&wcsimrootsuperevent_OD);
      optionsTree = (TTree*) hfile->Get("wcsimRootOptionsT");
      optionsTree->SetBranchAddress("wcsimrootoptions", &wcsimrootoptions);
    }
  }//useDefaultROOTout

}

void WCSimRunAction::EndOfRunAction(const G4Run*)
{
//G4cout << "Number of Events Generated: "<< numberOfEventsGenerated << G4endl;
//G4cout<<"Number of times waterTube hit: " << numberOfTimesWaterTubeHit<<G4endl;
//   G4cout << ((double(numberOfTimesMRDHit)+double(numberOfTimesFGDHit))/double(numberOfEventsGenerated))*100.
// 	 << "% hit FGD or MRD" << G4endl;
//G4cout << "Number of times OD hit: " << numberOfTimesMRDHit << G4endl;
//G4cout << "Number of times FGD hit: "    << numberOfTimesFGDHit << G4endl;
//G4cout << "Number of times lArD hit: "  << numberOfTimeslArDHit << G4endl;
//G4cout<<"Number of times waterTube hit: " << numberOfTimesWaterTubeHit<<G4endl;
//   G4cout << ((double(numberOfTimesMRDHit)+double(numberOfTimesFGDHit))/double(numberOfEventsGenerated))*100.
// 	 << "% hit FGD or MRD" << G4endl;
//   G4cout << "Number of times Catcher hit: " << numberOfTimesCatcherHit<<G4endl;
//   G4cout << "Number of times Rock hit: " << numberOfTimesRockHit<<G4endl;
//  G4cout << (double(numberOfTimesCatcherHit)/double(numberOfEventsGenerated))*100.
//        << "% through-going (hit Catcher)" << G4endl;


  //Write the options tree
  G4cout << "EndOfRunAction" << G4endl;
  
  // Close the Root file at the end of the run

  if(useDefaultROOTout){

    // Close the Root file at the end of the run
    TFile* hfile = WCSimTree->GetCurrentFile();
    hfile->cd();
    optionsTree->Fill();
    optionsTree->Write();
    hfile->Write();
    hfile->Close();
  
    // Clean up stuff on the heap; I think deletion of hfile and trees
    // is taken care of by the file close
    delete wcsimrootsuperevent; wcsimrootsuperevent=0;
    delete wcsimrootsuperevent2; wcsimrootsuperevent2=0;
    delete wcsimrootsuperevent_OD; wcsimrootsuperevent_OD=0;
    delete wcsimrootgeom; wcsimrootgeom=0;
  }//useDefaultROOTout

  if(useTimer) {
    timer.Stop();
    G4cout << "WCSimRunAction ran from BeginOfRunAction() to EndOfRunAction() in:"
	   << "\t" << timer.CpuTime()  << " seconds (CPU)"
	   << "\t" << timer.RealTime() << " seconds (real)" << G4endl;
  }
}

void WCSimRunAction::FillGeoTree(){

  // Fill the geometry tree
  G4int geo_type;
  G4double cylinfo[3];
  G4double pmtradius;
  G4double pmtradiusOD;
  G4int numpmt;
  G4int numpmtOD;
  G4int orientation;
  G4double pmtradius2;//Hybrid configuration
  G4int numpmt2;//Hybrid configuration
  //G4int orientation2;//Hybrid configuration
  Double_t offset[3];
 
  //Double_t rotation[3];

  Int_t tubeNo;
  Int_t mPMTNo = -1;
  Int_t mPMT_pmtNo = -1;
  Double_t pos[3];
  Double_t rot[3];
  Int_t cylLoc;

  if (wcsimdetector->GetIsEggShapedHyperK()) {
      geo_type = 2;
  }
  else {
      geo_type = 0;
  }
  // geo_type 1 is for defunct mailbox design

  wcsimrootgeom-> SetGeo_Type(geo_type);

  if (geo_type == 0) {
      //cylinder
      cylinfo[1] = wcsimdetector->GetGeo_Dm(3);
      cylinfo[2] = wcsimdetector->GetGeo_Dm(2);
      wcsimrootgeom-> SetWCCylRadius(cylinfo[1]);
      wcsimrootgeom-> SetWCCylLength(cylinfo[2]);
  }


  pmtradius = wcsimdetector->GetPMTSize1();
  pmtradius2 = 4.0;//B.Q debug, Temp wcsimdetector->GetPMTSize1();
  pmtradiusOD = wcsimdetector->GetODPMTSize();
  numpmt = wcsimdetector->GetTotalNumPmts();
  numpmt2 = wcsimdetector->GetTotalNumPmts2();//Hybrid configuration
  numpmtOD = wcsimdetector->GetTotalNumODPmts();
  orientation = 0;
  
  wcsimrootgeom-> SetWCPMTRadius(pmtradius);
  wcsimrootgeom-> SetWCPMTRadius(pmtradius2,true);
  wcsimrootgeom-> SetODWCPMTRadius(pmtradiusOD);
  wcsimrootgeom-> SetOrientation(orientation);
  
  G4ThreeVector offset1= wcsimdetector->GetWCOffset();
  offset[0] = offset1[0];
  offset[1] = offset1[1];
  offset[2] = offset1[2];
  wcsimrootgeom-> SetWCOffset(offset[0],offset[1],offset[2]);

  if(wcsimdetector->GetIsNuPrism()){ 
      G4ThreeVector rotation1= wcsimdetector->GetWCXRotation();
      WCXRotation[0] = rotation1[0];
      WCXRotation[1] = rotation1[1];
      WCXRotation[2] = rotation1[2];
    
      G4ThreeVector rotation2= wcsimdetector->GetWCYRotation();
      WCYRotation[0] = rotation2[0];
      WCYRotation[1] = rotation2[1];
      WCYRotation[2] = rotation2[2];
    
      G4ThreeVector rotation3= wcsimdetector->GetWCZRotation();
      WCZRotation[0] = rotation3[0];
      WCZRotation[1] = rotation3[1];
      WCZRotation[2] = rotation3[2];

      WCDetCentre[0] = offset1[0]/100.0;
      WCDetCentre[1] = offset1[1]/100.0;
      WCDetCentre[2] = offset1[2]/100.0;

      WCDetRadius = wcsimdetector->GetWCIDDiameter()/2.0;
      WCDetHeight = wcsimdetector->GetWCIDHeight();

      if(fSettingsInputTree){
          fSettingsInputTree->GetEntry(0);
          float z_offset = fNuPlanePos[2]/100.0;
          WCDetCentre[2] += z_offset;
          G4cout << "WCDetCentre[2] = " << WCDetCentre[2] << G4endl;
      }

      fSettingsOutputTree->Fill();
  }

  const std::vector<WCSimPmtInfo*> *fpmts = wcsimdetector->Get_Pmts();
  for (unsigned int i=0;i!=fpmts->size();i++){
    const WCSimPmtInfo * pmt = ((WCSimPmtInfo*)fpmts->at(i));
    pos[0] = pmt->Get_transx();
    pos[1] = pmt->Get_transy();
    pos[2] = pmt->Get_transz();
    rot[0] = pmt->Get_orienx();
    rot[1] = pmt->Get_orieny();
    rot[2] = pmt->Get_orienz();
    tubeNo = pmt->Get_tubeid();
    mPMTNo = pmt->Get_mPMTid();
    mPMT_pmtNo = pmt->Get_mPMT_pmtid();
    cylLoc = pmt->Get_cylocation();
    wcsimrootgeom-> SetPMT(i,tubeNo,mPMTNo, mPMT_pmtNo, cylLoc,rot,pos,true,false);
  }
  if (fpmts->size() != (unsigned int)numpmt) {
    G4cout << "Mismatch between number of ID pmts and pmt list in geofile.txt!!"<<G4endl;
    G4cout << fpmts->size() <<" vs. "<< numpmt <<G4endl;
  }
  //Hybrid version
  const std::vector<WCSimPmtInfo*> *fpmts2 = wcsimdetector->Get_Pmts2();
  for (unsigned int i=0;i!=fpmts2->size();i++){
    const WCSimPmtInfo * pmt = ((WCSimPmtInfo*)fpmts2->at(i));
    pos[0] = pmt->Get_transx();
    pos[1] = pmt->Get_transy();
    pos[2] = pmt->Get_transz();
    rot[0] = pmt->Get_orienx();
    rot[1] = pmt->Get_orieny();
    rot[2] = pmt->Get_orienz();
    tubeNo = pmt->Get_tubeid();
    mPMTNo = pmt->Get_mPMTid();
    mPMT_pmtNo = pmt->Get_mPMT_pmtid();
    cylLoc = pmt->Get_cylocation();
    wcsimrootgeom-> SetPMT(i,tubeNo,mPMTNo, mPMT_pmtNo, cylLoc,rot,pos,true,true);
  }
  if (fpmts2->size() != (unsigned int)numpmt2) {
    G4cout << "Mismatch between number of pmts and pmt list in geofile.txt!!"<<G4endl;
    G4cout << fpmts2->size() <<" vs. "<< numpmt2 <<G4endl;
  }
  //OD
  const std::vector<WCSimPmtInfo*> *fODpmts = wcsimdetector->Get_ODPmts();
  for (unsigned int i=0;i!=fODpmts->size();i++){
    const WCSimPmtInfo * pmt = ((WCSimPmtInfo*)fODpmts->at(i));
    pos[0] = pmt->Get_transx();
    pos[1] = pmt->Get_transy();
    pos[2] = pmt->Get_transz();
    rot[0] = pmt->Get_orienx();
    rot[1] = pmt->Get_orieny();
    rot[2] = pmt->Get_orienz();
    tubeNo = pmt->Get_tubeid();
    cylLoc = pmt->Get_cylocation();
    wcsimrootgeom-> SetODPMT(i,tubeNo,cylLoc,rot,pos,true);
  }
  if (fODpmts->size() != (unsigned int)numpmtOD) {
    G4cout << "Mismatch between number of OD pmts and pmt list in geofile.txt!!"<<G4endl;
    G4cout << fODpmts->size() <<" vs. "<< numpmtOD <<G4endl;
  }

  wcsimrootgeom-> SetWCNumPMT(numpmt,false);
  wcsimrootgeom-> SetWCNumPMT(numpmt2,true);
  wcsimrootgeom-> SetODWCNumPMT(numpmtOD);

  geoTree->Fill();
  //TFile* hfile = geoTree->GetCurrentFile();
  //hfile->Write(); 
  if(wcsimdetector->GetIsNuPrism()) fSettingsOutputTree->Write();
//  if(fSettingsInputTree) fSettingsInputTree->Close();

  geoTree->Write(); 
}

NRooTrackerVtx* WCSimRunAction::GetRootrackerVertex(){

  NRooTrackerVtx* currRootrackerVtx = new((*fVertices)[fNVtx])NRooTrackerVtx();
  fNVtx += 1;
  return currRootrackerVtx;
}

