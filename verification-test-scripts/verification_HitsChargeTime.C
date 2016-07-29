#include <iostream>
#include <TH1F.h>
#include <stdio.h>     
#include <stdlib.h>    
// Simple example of reading a generated Root file
void verification_HitsChargeTime(char *filename="wcsimtest.root", char *filename2="../../WCSim_clean/verification-test-scripts/wcsimtest.root", bool verbose=false)
{
  // Clear global scope
  //gROOT->Reset();
  
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetTitleColor(1);
  gStyle->SetStatColor(0);
  gStyle->SetFrameFillColor(0);
  gStyle->SetPadColor(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetTitleSize(0.04);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);
  gStyle->SetTitleAlign(23);
  gStyle->SetTitleX(.5);
  gStyle->SetTitleY(0.99);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetHatchesLineWidth(2);
  gStyle->SetLineWidth(1.5);
  gStyle->SetTitleFontSize(0.07);
  gStyle->SetLabelSize(0.05,"X");
  gStyle->SetLabelSize(0.05,"Y");
  gStyle->SetTitleSize(0.04,"X");
  gStyle->SetTitleSize(0.04,"Y");
  gStyle->SetTitleBorderSize(0);
  gStyle->SetCanvasBorderMode(0);
  
  
  // Load the library with class dictionary info
  // (create with "gmake shared")
  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIMDIR");
  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
  }else{
    gSystem->Load("../libWCSimRoot.so");
  }

  TFile *f = new TFile(filename,"read");
  if (!f->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    return -1;
  }
  
  TFile *f2 = new TFile(filename2,"read");
  if (!f2->IsOpen()){
    cout << "Error, could not open input file: " << filename2 << endl;
    return -1;
  }
  
 
  TTree  *wcsimT = f->Get("wcsimT");
  int nevent = wcsimT->GetEntries();
  TTree  *wcsimT2 = f2->Get("wcsimT");
  int nevent2 = wcsimT2->GetEntries();

  // Create a WCSimRootEvent to put stuff from the tree in and set the branch address for reading from the tree
  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
  wcsimT->SetBranchAddress("wcsimrootevent",&wcsimrootsuperevent);
  WCSimRootEvent *wcsimrootsuperevent2 = new WCSimRootEvent();
  wcsimT2->SetBranchAddress("wcsimrootevent",&wcsimrootsuperevent2);

  // Force deletion to prevent memory leak when issuing multiple calls to GetEvent()
  wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);
  wcsimT2->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);

  // Print the first event from the modified WCSim version
  wcsimT->GetEvent(0);
  WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  cout << "Stats for the first event in your version of WCSim using " << filename << endl;
  cout << "Number of tube hits " << wcsimrootevent->GetNumTubesHit() << endl;
  cout << "Number of digitized tube hits " << wcsimrootevent->GetNumDigiTubesHit() << endl;
  cout << "Number of photoelectron hit times " << wcsimrootevent->GetCherenkovHitTimes()->GetEntries() << endl;

  // Print the first event from the "clean" WCSim version
  wcsimT2->GetEvent(0);
  WCSimRootTrigger *wcsimrootevent2 = wcsimrootsuperevent2->GetTrigger(0);
  cout << "***********************************************************" << endl;
  cout << "Stats for the first event of WCSim version on GitHub using "<< filename2 << endl;
  cout << "Number of tube hits " << wcsimrootevent2->GetNumTubesHit() << endl;
  cout << "Number of digitized tube hits " << wcsimrootevent2->GetNumDigiTubesHit() << endl;
  cout << "Number of photoelectron hit times " << wcsimrootevent2->GetCherenkovHitTimes()->GetEntries() << endl;

  // Compare these two events
  cout <<  "***********************************************************" << endl;
  if (abs(wcsimrootevent->GetNumTubesHit() - wcsimrootevent2->GetNumTubesHit())>1.0e-6){cout << "FIRST EVENT TEST FAILED: Number of hit tubes do not match" << endl;}
  else {cout << "FIRST EVENT TEST PASSED: Number of hit tubes matches" << endl;}
  if (abs(wcsimrootevent->GetNumDigiTubesHit() - wcsimrootevent2->GetNumDigiTubesHit())>1.0e-6){cout << "FIRST EVENT TEST FAILED: Number of digitized tubes do not match" << endl; }
  else {cout << "FIRST EVENT TEST PASSED: Number of digitized tubes matches" << endl; }
  if (abs(wcsimrootevent->GetCherenkovHitTimes()->GetEntries() - wcsimrootevent2->GetCherenkovHitTimes()->GetEntries())> 1.0e-6){cout << "FIRST EVENT TEST FAILED: Number of hit times do not match" << endl;}
  else {cout << "FIRST EVENT TEST PASSED: Number of hit times matches" << endl;}

  if (nevent != nevent2) {
    cout <<  "***********************************************************" << endl;
    cout << "The input files don’t contain the same number of events. Only the first events were compared. To see histograms of the number of hits, deposited charge and hit time, please choose two input files which contain the same number of events." << endl;
    return -1;
  }


  // Histograms for the modified WCSim version
  TH1F *hits = new TH1F("PMT Hits", "# Digitized Hits", 500, 0, 3000);
  TH1F *time = new TH1F("Average Time", "Average Time", 600, 900, 2000);
  TH1F *charge = new TH1F("Q/# Digitized PMT", "Average Charge", 200, 0, 5);
  // ... and for the "clean" version
  TH1F *hits2 = new TH1F("PMT Hits 2", "Digitized Hits", 500, 0, 3000);
  TH1F *time2 = new TH1F("Average Time 2", "Average Time", 600, 900, 2000);
  TH1F *charge2 = new TH1F("Q/# Digitized PMT 2", "Average Charge", 200, 0, 5);


  // Now loop over events from the modified WCSim version and fill the histograms
  for (int ev=0; ev<nevent; ev++){
    // Read the event from the tree into the WCSimRootEvent instance
    wcsimT->GetEvent(ev);
    wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
    if(verbose){
      printf("********************************************************");
      printf("Evt, date %d %d\n", wcsimrootevent->GetHeader()->GetEvtNum(),
	     wcsimrootevent->GetHeader()->GetDate());
      printf("Mode %d\n", wcsimrootevent->GetMode());
      printf("Number of subevents %d\n",
	     wcsimrootsuperevent->GetNumberOfSubEvents());
      
      printf("Vtxvol %d\n", wcsimrootevent->GetVtxvol());
      printf("Vtx %f %f %f\n", wcsimrootevent->GetVtx(0),
	     wcsimrootevent->GetVtx(1),wcsimrootevent->GetVtx(2));
    }
    
    for (int index = 0 ; index < wcsimrootsuperevent->GetNumberOfEvents(); index++){ 
	wcsimrootevent = wcsimrootsuperevent->GetTrigger(index);
	int ncherenkovdigihits = wcsimrootevent->GetNcherenkovdigihits();
	hits->Fill(ncherenkovdigihits);
	
	
	float totalq = 0.;
	float totalt = 0.;
	// Loop through elements in the TClonesArray of WCSimRootCherenkovHits
	for (int i=0; i< ncherenkovdigihits; i++){
	    TObject *Digi = (wcsimrootevent->GetCherenkovDigiHits())->At(i);
	    WCSimRootCherenkovDigiHit *wcsimrootcherenkovdigihit = 
	      dynamic_cast<WCSimRootCherenkovDigiHit*>(Digi);
	    
	    float q = wcsimrootcherenkovdigihit->GetQ();
	    float t = wcsimrootcherenkovdigihit->GetT();
	    totalq+=q;
	    totalt+=t;
	}
	float av_time = (ncherenkovdigihits > 0) ? totalt/ncherenkovdigihits : 0;
	float av_q = (ncherenkovdigihits > 0) ? totalq/ncherenkovdigihits : 0;
	charge->Fill(av_q);  
	time->Fill(av_time);
    }

    // reinitialize super event between loops.
    wcsimrootsuperevent->ReInitialize();
  }// End of loop over events


 
  
  // Now loop over events
  for (int ev=0; ev<nevent2; ev++){
    // Read the event from the tree into the WCSimRootEvent instance
    wcsimT2->GetEvent(ev);
    wcsimrootevent2 = wcsimrootsuperevent2->GetTrigger(0);
  
    if(verbose){
      printf("********************************************************");
      printf("Evt, date %d %d\n", wcsimrootevent2->GetHeader()->GetEvtNum(),
	     wcsimrootevent2->GetHeader()->GetDate());
      printf("Mode %d\n", wcsimrootevent2->GetMode());
      printf("Number of subevents %d\n",
	     wcsimrootsuperevent2->GetNumberOfSubEvents());
      
      printf("Vtxvol %d\n", wcsimrootevent2->GetVtxvol());
      printf("Vtx %f %f %f\n", wcsimrootevent2->GetVtx(0),
	     wcsimrootevent2->GetVtx(1),wcsimrootevent2->GetVtx(2));
    }
    
    for (int index = 0 ; index < wcsimrootsuperevent2->GetNumberOfEvents(); index++){ 
	wcsimrootevent2 = wcsimrootsuperevent2->GetTrigger(index);
	int ncherenkovdigihits = wcsimrootevent2->GetNcherenkovdigihits();
	hits2->Fill(ncherenkovdigihits);

	float totalq = 0.;
	float totalt = 0.;
	// Loop through elements in the TClonesArray of WCSimRootCherenkovHits
	for (int i=0; i< ncherenkovdigihits; i++){
	    TObject *Digi = (wcsimrootevent2->GetCherenkovDigiHits())->At(i);
	    WCSimRootCherenkovDigiHit *wcsimrootcherenkovdigihit = 
	      dynamic_cast<WCSimRootCherenkovDigiHit*>(Digi);
	    
	    float q = wcsimrootcherenkovdigihit->GetQ();
	    float t = wcsimrootcherenkovdigihit->GetT();
	    totalq+=q;
	    totalt+=t;
	}
	float av_time = (ncherenkovdigihits > 0) ? totalt/ncherenkovdigihits : 0;
	float av_q = (ncherenkovdigihits > 0) ? totalq/ncherenkovdigihits : 0;
	charge2->Fill(av_q);  
	time2->Fill(av_time);
    }

    // reinitialize super event between loops.
    wcsimrootsuperevent2->ReInitialize();
  }// End of loop over events

	Double_t ks_hits = hits->KolmogorovTest(hits2);
	Double_t ks_charge = charge->KolmogorovTest(charge2);
	Double_t ks_time = time->KolmogorovTest(time2);
	cout << "***********************************************************" << endl;
	cout << "ks test for # of digitized hits: " << ks_hits << endl;
	cout << "ks test for average charge: " << ks_charge << endl;
	cout << "ks test for average time: " << ks_time << endl;

  //  TCanvas c1("c1"); 
  float win_scale = 0.75;
  int n_wide(2);
  int n_high(2);
  TCanvas* c1 = new TCanvas("c1", "Test Plots", 500*n_wide*win_scale, 500*n_high*win_scale);
  c1->Draw();
  c1->Divide(2,2);
  c1->cd(1); 
  hits2->SetLineColor(kRed);
  hits->Draw();
  c1->cd(1); hits2->Draw("SAME");

 TLegend *leg = new TLegend(0.2,0.7,0.55,0.85, "");
 leg->SetFillColor(0);
 leg->SetBorderSize(0);
 leg->AddEntry(hits,filename, "l");
 leg->AddEntry(hits2,filename2, "l");
 leg->Draw();
 
 c1->cd(2);
 charge->GetXaxis()->SetTitle("Total Charge / # digitized hits");
 charge->Draw();
 charge2->SetLineColor(kRed);
 c1->cd(2); charge2->Draw("SAME");
 c1->cd(3); 
 time->GetXaxis()->SetTitle("Total Time / # digitized hits (ns)");  
 time->Draw();
 time2->SetLineColor(kRed);
 c1->cd(3); time2->Draw("SAME");
  

}
