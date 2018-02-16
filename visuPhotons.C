#include <stdio.h>     
#include <stdlib.h>    

// Structure of a photon Evt from WCSimRootEvent.hh
struct photonEvt{
  Int_t trackID;
  Int_t parentID;
  Double_t pos[3];
  Double_t wl;
  Int_t proc;
};

  /* A simple script to visualize photons
   * 
   * I like to run this macro as 
   * $ root -l -x 'visuPhotons.C("OD.root")'
   */

void visuPhotons(char const *filename=NULL) {

  gROOT->Reset();
  gStyle->SetOptStat(1);

  //////////////////////////////////////////////////////
  // Load required libraries for processing the macro //
  //////////////////////////////////////////////////////
  
  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIMDIR"); // Recover $WCSIMDIR if define
  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so"); // Load shared WCSim lib
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.rootmap"); // Load rootmap (>ROOT6)
    gSystem->Load("${WCSIMDIR}/src/WCSimRootDict_rdict.pcm"); // Load root dict (>ROOT6)
  }else{
    std::cout << "Can't load WCSim ROOT dictionaries" << std::endl;
  }

  ////////////////////////////
  // Open output WCSim file // 
  ////////////////////////////
  
  TFile *f;
  char fTest[128];
  
  if (filename==NULL){ // If no filename provided try to open a default wcsim.root
    std::cout << "Please provide filename in option" << std::endl; // Please mate
    std::cout << "Will load auto wcsim.root in WCSIMDIR ..." << std::endl;
    char const *name = "wcsim.root";
    strncpy(fTest, wcsimdirenv, sizeof(fTest));
    strncat(fTest, "/", (sizeof(fTest) - strlen(fTest) -1) );
    strncat(fTest, name, (sizeof(fTest) - strlen(fTest) -1) );
    f = new TFile(fTest);
  }else{
    f = new TFile(filename);
  }
  if (!f->IsOpen()){
    std::cout << "ERROR, could not open input file: " << filename << std::endl;
    return -1;
  }else{
    if (filename==NULL) std::cout << "File open : " << fTest << std::endl;
    else std::cout << "File open : " << filename << std::endl;
  }

  /////////////////////////////////////////////////////////
  // Access photons information inside WCSim output file //
  /////////////////////////////////////////////////////////
  
  TTree  *WCSimPhotons = (TTree*)f->Get("photons");  // Open photon tree inside file
  if(WCSimPhotons){ // Test if tree is really open
    cout << "Photons tree open" << endl;
  }else{
    std::cout << "Tree NOT open try again mate" << std::endl;
    return -1;
  }

  photonEvt *pEvt = new photonEvt(); // Create new photon structure to access data inside tree
  WCSimPhotons->SetBranchAddress("phoEvt",pEvt); // "phoEvt" is the name of the structure inside output file

  //////////////////////////////////////////////////////////
  // Loop over tree entry and create histograms and stuff //
  //////////////////////////////////////////////////////////

  // Photons trajectory
  const double maxX = 40000.; // in mm
  const double maxY = 40000.; // in mm
  const double maxZ = 30000.; // in mm
  TH3D *hTraj = new TH3D("hTraj","Photons trajectory",
			 100,-maxX,maxX, // x in mm
			 100,-maxY,maxY, // y in mm
			 100,-maxZ,maxZ); // z in mm

  // Nb of Cer photons produced
  int nbPhCer = 0;
  int previousTrackID = -1;

  // Nb of WL shifted photons produced
  int nbPhWLS = 0;

  // WLS photons wavelength
  TH1D *hWL = new TH1D("hWL","Wavelength shifted photons",30,300,600.);
  
  const long unsigned int NbEntries = WCSimPhotons->GetEntries(); // Recover nb of tree entries
  std::cout << "Nb of tree entries : " << NbEntries << std::endl;
  
  for(int iEntry=0;iEntry<NbEntries;iEntry++){ // Loop over tree entry
    WCSimPhotons->GetEntry(iEntry);

    hTraj->Fill(pEvt->pos[0],pEvt->pos[1],pEvt->pos[2]);

    if(pEvt->proc == 0){ // ID for photons created via Cerenkov process
      if(previousTrackID != pEvt->trackID) nbPhCer++; // Each time trackID changes, we start tracking a new photon
      previousTrackID = pEvt->trackID;
    }

    if(pEvt->proc == 1){ // ID for photons created via OpWLS process
      if(previousTrackID != pEvt->trackID){
	nbPhWLS++; // Each time trackID changes, we start tracking a new photon
	hWL->Fill(pEvt->wl);
      }
      previousTrackID = pEvt->trackID;
    }

  }

  //////////////////////////////////////////////////////////
  // ##################### OUTPUTS ###################### //
  //////////////////////////////////////////////////////////

  // PRINTOUTS
  std::cout << "Nb of Cer photons produced : " << nbPhCer << std::endl;
  std::cout << "Nb of WLS photons produced : " << nbPhWLS << std::endl;
  std::cout << " RATIO : " << 100.*(double)(nbPhWLS)/(double)(nbPhCer) << "%" <<std::endl;
  
  // DRAWINGS
  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  hTraj->Draw();
  c1 = new TCanvas("c2","c2",800,600);
  hWL->Draw();
  
} // END MACRO
