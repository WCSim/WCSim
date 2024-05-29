#include <stdio.h>     
#include <stdlib.h>
#include <libgen.h>

void trigger_OD(char *filename=NULL) {
  /* A simple script to plot aspects of phototube hits 
   * 
   * I like to run this macro as 
   * $ rootwc -l -x 'trigger_OD.C("OD.root")'
   */

  gROOT->Reset();
  
  gStyle->SetOptStat(1);

  TFile *f;
  char fTest[128];
  if (filename==NULL){
    std::cout << "Please provide filename in option" << std::endl;
    std::cout << "Will load auto wcsim.root in WCSIMDIR ..." << std::endl;
    char* name = "wcsim.root";
    strncpy(fTest, wcsimdirenv, sizeof(fTest));
    strncat(fTest, "/", (sizeof(fTest) - strlen(fTest)) );
    strncat(fTest, name, (sizeof(fTest) - strlen(fTest)) );
    f = new TFile(fTest);
  }else{
    f = new TFile(filename);
  }
  if (!f->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    return -1;
  }else{
    if (filename==NULL) cout << "File open bro: " << fTest << endl;
    else cout << "File open bro: " << filename << endl;
  }

  char *dirc, *basec, *bname, *dname;

  dirc = strdup(filename);
  basec = strdup(filename);
  dname = dirname(dirc);
  bname = basename(basec);
  printf("dirname=%s, basename=%s\n", dname, bname);
  // std::cout << "Filename : " << filename << std::endl;
  // std::cout << "dirname(filename) : " << dirname(filename) << std::endl;
  // std::cout << "basename(filename) : " << basename(filename) << std::endl;
  
  TTree  *wcsimT = (TTree*)f->Get("wcsimT");

  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
  wcsimT->SetBranchAddress("wcsimrootevent_OD",&wcsimrootsuperevent);

  // Force deletion to prevent memory leak when issuing multiple
  // calls to GetEvent()
  //wcsimT->GetBranch("wcsimrootevent_OD")->SetAutoDelete(kTRUE);

  // const long unsigned int nbEntries = wcsimT->GetEntries();
  const long unsigned int nbEntries = wcsimT->GetEntries();
  cout << "Nb of entries " << wcsimT->GetEntries() << endl;

  //////////////////////////////////////////
  // HISTOGRAMS DEFINITION /////////////////
  //////////////////////////////////////////

  const int nbThresh = 100;
  int trigThresh[nbThresh];

  for(int iThresh=0;iThresh<nbThresh;iThresh++){
    trigThresh[iThresh]=iThresh; // in PE
  }

  TGraph *grEffVSThres = new TGraph();
  grEffVSThres->SetName("grEffVSThres");
  grEffVSThres->SetTitle("Trigger veto efficiency VS Trigger threshold; Threshold (PE) ; Eff (%)");
  grEffVSThres->SetLineColor(kGreen+1);
  grEffVSThres->SetLineWidth(1.);
  grEffVSThres->SetMarkerColor(kGreen+1);
  grEffVSThres->SetMarkerSize(1.);
  grEffVSThres->SetMarkerStyle(kOpenThreeTriangles);
  
  TGraph *grMultVSThres = new TGraph();
  grMultVSThres->SetName("grMultVSThres");
  grMultVSThres->SetTitle("Multiplicity VS Trigger threshold; Threshold (PE) ; Multiplicity");
  grMultVSThres->SetLineColor(kBlue-4);
  grMultVSThres->SetLineWidth(1.);
  grMultVSThres->SetMarkerColor(kBlue-4);
  grMultVSThres->SetMarkerSize(1.);
  grMultVSThres->SetMarkerStyle(kDiamond);

  TH1D *hMultByThresh[nbThresh];
  const int nbBins = 10000;
  const int nbODPMTs = 10000;

  int nbEffByThresh[nbThresh];
  int nbMultByThresh[nbEntries][nbThresh];
  for(int iThresh=0;iThresh<nbThresh;iThresh++){
    nbEffByThresh[iThresh] = 0;
    hMultByThresh[iThresh] = new TH1D(Form("hMultByThresh%d",iThresh),
				      Form("Multiplicity for thresh %d PE",iThresh),
				      nbBins,
				      0,
				      nbODPMTs);
    for(long unsigned int iEntry = 0; iEntry < nbEntries; iEntry++){
      nbMultByThresh[iEntry][iThresh] = 0;
    }
  }

  
  // END HISTOGRAMS DEFINITION /////////////
  //////////////////////////////////////////

  for(long unsigned int iEntry = 0; iEntry < nbEntries; iEntry++){
    // Point to event iEntry inside WCSimTree
    wcsimT->GetEvent(iEntry); 
    
    // Nb of Trigger inside the event
    const unsigned int nbTriggers = wcsimrootsuperevent->GetNumberOfEvents();
    const unsigned int nbSubTriggers = wcsimrootsuperevent->GetNumberOfSubEvents();

    // cout << "iEntry : " << iEntry << endl;
    // cout << "nbTrig : " << nbTriggers << endl;
    // cout << "nbSubTrig : " << nbSubTriggers << endl;

    for(long unsigned int iTrig = 0; iTrig < nbTriggers; iTrig++){
      WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(iTrig);
      
      // DIGI HITS
      int digiMax = wcsimrootevent->GetNcherenkovdigihits();
      int totDigiPE = 0;

      for (int i = 0; i < digiMax; i++){
	//WCSimRootChernkovDigiHit has methods GetTubeId(), GetT(), GetQ()
	WCSimRootCherenkovDigiHit *cDigiHit =
	  (WCSimRootCherenkovDigiHit*)wcsimrootevent->GetCherenkovDigiHits()->At(i);

	for(int iThresh=0;iThresh<nbThresh;iThresh++){
	  if(cDigiHit->GetQ() > trigThresh[iThresh]) nbMultByThresh[iEntry][iThresh]++;
	}

      } // END FOR DIGI HITS
     
    } // END FOR iTRIG

    for(int iThresh=0;iThresh<nbThresh;iThresh++){
      hMultByThresh[iThresh]->Fill(nbMultByThresh[iEntry][iThresh]);
    }
    
  } // END FOR iENTRY

  //////////////////////////////////////////
  // EFFICIENCY AND MULTIPILICITY //////////
  //////////////////////////////////////////

  for(int iThresh=0;iThresh<nbThresh;iThresh++){

    for(long unsigned int iEntry = 0; iEntry < nbEntries; iEntry++){
      if(nbMultByThresh[iEntry][iThresh] > 0){
	nbEffByThresh[iThresh]++;
	continue;
      }
    }

    grEffVSThres->SetPoint(iThresh,
			   trigThresh[iThresh],
			   (double)(nbEffByThresh[iThresh])/(double)(nbEntries));
    grMultVSThres->SetPoint(iThresh,
			    trigThresh[iThresh],
			    hMultByThresh[iThresh]->GetMean());    
  }
  
  //////////////////////////////////////////
  // DRAWING ///////////////////////////////
  //////////////////////////////////////////

  TCanvas *c1;

  c1 = new TCanvas("cEff","cEff",800,600);
  gPad->SetGrid();
  grEffVSThres->Draw("APL");

  c1 = new TCanvas("cMult","cMult",800,600);
  gPad->SetGrid();
  grMultVSThres->Draw("APL");

  TFile *output=NULL;

  char outputName[1000];
  sprintf(outputName,"TRIGGER_%s",bname);
  output = new TFile(outputName,"recreate");
  grEffVSThres->Write();
  grMultVSThres->Write();

} // END MACRO
