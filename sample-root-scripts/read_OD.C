#include <stdio.h>     
#include <stdlib.h>    

#include <libgen.h>

  /* A simple script to plot aspects of phototube hits 
   * 
   * I like to run this macro as 
   * $ root -l -x 'read_OD.C("OD.root")'
   */

Double_t simpleExp(Double_t *x, Double_t *par) {
  Double_t X = x[0];

  Double_t tau = par[0];
  Double_t CExp = par[1];

  return CExp*TMath::Exp(-tau*X);;

}

void read_OD(char *filename=NULL) {

  gROOT->Reset();
  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIMDIR");
  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.rootmap");
    gSystem->Load("${WCSIMDIR}/src/WCSimRootDict_rdict.pcm");
  }else{
    std::cout << "Can't load WCSim ROOT dictionaries" << std::endl;
  }
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

  const int nbBins = 50;
  const int nbPEMax = 1000;
  const int nbBinsByPMT = 50;
  const int nbPEMaxByPMT = 50;

  const double cutPEByPMT = 1.;
  
  TH1D *hPEByEvtsByPMT = new TH1D("hPEByEvtsByPMT","RAW PE by Evts by PMT",
				  nbBinsByPMT,0,nbPEMaxByPMT);
  hPEByEvtsByPMT->GetXaxis()->SetTitle("raw PE");
  hPEByEvtsByPMT->SetLineColor(kBlue-4);
  hPEByEvtsByPMT->SetMarkerColor(kBlue-4);  
  TH1D *hPECollectedByEvtsByPMT = new TH1D("hPECollectedByEvtsByPMT","collected PE by Evts by PMT",
					   nbBinsByPMT,0,nbPEMaxByPMT);
  hPECollectedByEvtsByPMT->GetXaxis()->SetTitle("digi PE");
  hPECollectedByEvtsByPMT->SetLineColor(kRed-4);
  hPECollectedByEvtsByPMT->SetMarkerColor(kRed-4);
  
  TH1D *hPEByEvts = new TH1D("hPEByEvts","Total RAW PE by Evts",nbBins,0,nbPEMax);
  hPEByEvts->GetXaxis()->SetTitle("raw PE");
  hPEByEvts->SetLineColor(kBlue+1);
  hPEByEvts->SetMarkerColor(kBlue+1);
  hPEByEvts->SetFillColor(kBlue+1);  
  TH1D *hPECollectedByEvts = new TH1D("hPECollectedByEvts","Total collected PE by Evts",nbBins,0,nbPEMax);
  hPECollectedByEvts->GetXaxis()->SetTitle("digi PE");
  hPECollectedByEvts->SetLineColor(kRed+1);
  hPECollectedByEvts->SetMarkerColor(kRed+1);
  hPECollectedByEvts->SetFillColor(kRed+1);

  TH1D *hNbTubesHit = new TH1D("hNbTubesHit","Nb of Tubes Hit",50,0,1000);
  
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
      
      // RAW HITS
      int rawMax = wcsimrootevent->GetNcherenkovhits();
      int totRawPE = 0;
      for (int i = 0; i < rawMax; i++){
	WCSimRootCherenkovHit *chit = (WCSimRootCherenkovHit*)wcsimrootevent->GetCherenkovHits()->At(i);
	
	if(chit->GetTotalPe(1) > cutPEByPMT) hPEByEvtsByPMT->Fill(chit->GetTotalPe(1));
	totRawPE+=chit->GetTotalPe(1);
      } // END FOR RAW HITS

      hNbTubesHit->Fill(rawMax);
      hPEByEvts->Fill(totRawPE);

      // DIGI HITS
      int digiMax = wcsimrootevent->GetNcherenkovdigihits();
      int totDigiPE = 0;
      for (int i = 0; i < digiMax; i++){
	WCSimRootCherenkovDigiHit *cDigiHit =
	  (WCSimRootCherenkovDigiHit*)wcsimrootevent->GetCherenkovDigiHits()->At(i);
	//WCSimRootChernkovDigiHit has methods GetTubeId(), GetT(), GetQ()
	WCSimRootCherenkovHitTime *cHitTime =
	  (WCSimRootCherenkovHitTime*)wcsimrootevent->GetCherenkovHitTimes()->At(i);
	//WCSimRootCherenkovHitTime has methods GetTubeId(), GetTruetime()

	if(cDigiHit->GetQ() > cutPEByPMT) hPECollectedByEvtsByPMT->Fill(cDigiHit->GetQ());
	totDigiPE+=cDigiHit->GetQ();
      } // END FOR DIGI HITS

      hPECollectedByEvts->Fill(totDigiPE);
      
    } // END FOR iTRIG
    
  } // END FOR iENTRY


  //////////////////////////////////////////
  // FIT ///////////////////////////////
  //////////////////////////////////////////
  
  TF1 *fPEByEvt = new TF1("FitPEByEvt",simpleExp,cutPEByPMT,30,2);
  fPEByEvt->SetParameter(0,hPEByEvtsByPMT->GetMaximum());
  fPEByEvt->SetParameter(1,1/hPEByEvtsByPMT->GetMean());

  fPEByEvt->SetParNames ("C0","Tau");

  //////////////////////////////////////////
  // DRAWING ///////////////////////////////
  //////////////////////////////////////////

  TCanvas *c1;

  c1 = new TCanvas("cPE","cPE",800,600);
  c1->Divide(2,2);
  c1->cd(1);
  gPad->SetLogy();
  hPEByEvtsByPMT->Draw("HIST");
  // hPEByEvtsByPMT->Fit("FitPEByEvt","LEMR+");
  hPEByEvtsByPMT->Fit("expo","+","SAME");
  c1->cd(2);
  hPEByEvts->Draw(""); // hPEByEvts->Fit("gaus");
  c1->cd(3);
  gPad->SetLogy();
  hPECollectedByEvtsByPMT->Draw("HIST");
  hPECollectedByEvtsByPMT->Fit("expo","+","SAME");
  c1->cd(4);  
  hPECollectedByEvts->Draw("HIST");

  // TF1 *fit = hPEByEvts->GetFunction("gaus");
  
  c1 = new TCanvas("cNbTubesHit","cNbTubesHit",800,600);
  hNbTubesHit->Draw("HIST");

  cout << "Mean nb of tubes hit by events : " << hNbTubesHit->GetMean()
       << " +- " << hNbTubesHit->GetRMS() << endl;
  cout << "Mean raw PE by events by PMT : " << hPEByEvtsByPMT->GetMean()
       << " +- " << hPEByEvtsByPMT->GetRMS() << endl;  
  cout << "Mean PE collected by events by PMT : " << hPECollectedByEvtsByPMT->GetMean()
       << " +- " << hPECollectedByEvtsByPMT->GetRMS() << endl;
  cout << "Mean raw PE by events : " << hPEByEvts->GetMean()
       << " +- " << hPEByEvts->GetRMS() << endl;  
  cout << "Mean PE collected by events : " << hPECollectedByEvts->GetMean()
       << " +- " << hPECollectedByEvts->GetRMS() << endl;
  // cout << "FIT : " << fit->GetParameter(1)
  //      << " +- " << fit->GetParError(1) << endl;  

    TFile *output=NULL;

  char outputName[1000];
  sprintf(outputName,"PROCESSED/PROCESSED_%s",bname);
  output = new TFile(outputName,"recreate");
  hPEByEvtsByPMT->Write();
  hPEByEvts->Write();
  hPECollectedByEvtsByPMT->Write();
  hPECollectedByEvts->Write();
  hNbTubesHit->Write();

} // END MACRO
