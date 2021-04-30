#include <stdio.h>     
#include <stdlib.h>    
#include <iostream>

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TStyle.h"

#include "WCSimRootEvent.hh"

int read_OD(const char *filename="wcsim.root") {
  /* A simple script to plot aspects of phototube hits 
   * 
   * I like to run this macro as 
   * $ root -l -x 'read_OD.C("OD.root")'
   */

  gROOT->Reset();  
  gStyle->SetOptStat(1);
  
  TFile *f = new TFile(filename);
  if (!f->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    return -1;
  }
  cout << "File open: " << filename << endl;

  TTree  *wcsimT = (TTree*)f->Get("wcsimT");

  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
  int error = wcsimT->SetBranchAddress("wcsimrootevent_OD",&wcsimrootsuperevent);  

  const long unsigned int nbEntries = wcsimT->GetBranch("wcsimrootevent_OD")->GetEntries();
  cout << "Nb of entries in OD branch: " << nbEntries << endl;

  //////////////////////////////////////////
  // HISTOGRAMS DEFINITION /////////////////
  //////////////////////////////////////////

  const int nbBins = 100;
  const int nbPEMax = 1000;
  const int nbBinsByPMT = 100;
  const int nbPEMaxByPMT = nbBinsByPMT;
  
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

  TH1D *hNbTubesHit = new TH1D("hNbTubesHit","Nb of Tubes Hit",1000,0,1000);
  
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
	
	hPEByEvtsByPMT->Fill(chit->GetTotalPe(1));
	totRawPE+=chit->GetTotalPe(1);
      } // END FOR RAW HITS

      hNbTubesHit->Fill(rawMax);
      hPEByEvts->Fill(totRawPE);

      // DIGI HITS
      int digiMax = wcsimrootevent->GetNcherenkovdigihits_slots();
      int totDigiPE = 0;
      for (int i = 0; i < digiMax; i++){
	WCSimRootCherenkovDigiHit *cDigiHit =
	  (WCSimRootCherenkovDigiHit*)wcsimrootevent->GetCherenkovDigiHits()->At(i);
	if(!cDigiHit)
	  continue;
	//WCSimRootChernkovDigiHit has methods GetTubeId(), GetT(), GetQ()
	//WCSimRootCherenkovHitTime *cHitTime =
	//(WCSimRootCherenkovHitTime*)wcsimrootevent->GetCherenkovHitTimes()->At(i); //this should be a loop over number of cherenkov hit times
	//WCSimRootCherenkovHitTime has methods GetTubeId(), GetTruetime()

	hPECollectedByEvtsByPMT->Fill(cDigiHit->GetQ());
	totDigiPE+=cDigiHit->GetQ();
      } // END FOR DIGI HITS

      hPECollectedByEvts->Fill(totDigiPE);
      
    } // END FOR iTRIG
    
  } // END FOR iENTRY

  //////////////////////////////////////////
  // DRAWING ///////////////////////////////
  //////////////////////////////////////////

  TCanvas *c1;

  c1 = new TCanvas("cPE","cPE",800,600);
  c1->Divide(2,2);
  c1->cd(1);
  gPad->SetLogy();
  hPEByEvtsByPMT->Draw("HIST");
  c1->cd(2);
  hPEByEvts->Draw("HIST");
  c1->cd(3);
  gPad->SetLogy();
  hPECollectedByEvtsByPMT->Draw("HIST");
  c1->cd(4);  
  hPECollectedByEvts->Draw("sameBAR");

  c1 = new TCanvas("cNbTubesHit","cNbTubesHit",800,600);
  hNbTubesHit->Draw("HIST");

  cout << "Mean nb of tubes hit by events : " << hNbTubesHit->GetMean()
       << " +- " << hNbTubesHit->GetRMS() << endl;
  cout << "Mean raw PE by events by PMT : " << hPEByEvtsByPMT->GetMean()
       << " +- " << hPEByEvtsByPMT->GetRMS() << endl;  
  cout << "Mean PE collected by events by PMT : " << hPECollectedByEvtsByPMT->GetMean()
       << " +- " << hPECollectedByEvtsByPMT->GetRMS() << endl;

  TFile *output=NULL;

  char outputName[1000];
  sprintf(outputName,"PROCESSED_%s",filename);
  output = new TFile(outputName,"recreate");
  hPEByEvtsByPMT->Write();
  hPEByEvts->Write();
  hPECollectedByEvtsByPMT->Write();
  hPECollectedByEvts->Write();

  return 0;
} // END MACRO
