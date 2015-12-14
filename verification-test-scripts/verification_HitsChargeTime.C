#include <iostream>
#include <TH1F.h>
#include <stdio.h>     
#include <stdlib.h>    
// Simple example of reading a generated Root file
void verification_HitsChargeTime(char *filename=NULL, char *filename2, bool verbose=false)
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

  TFile *f;
  // Open the file
  if (filename==NULL){
    f = new TFile("wcsimtest.root","read");
    filename = "wcsimtest.root";
  }else{
    f = new TFile(filename,"read");
  }
  if (!f->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    return -1;
  }
  
  TFile *f2;
  // Open the file
  if (filename2==NULL){
    f2 = new TFile("../../WCSim_clean/verification-test-scripts/wcsimtest.root","read");
    filename2 = "../../WCSim_clean/verification-test-scripts/wcsimtest.root";
  }else{
    f2 = new TFile(filename2,"read");
  }
  if (!f2->IsOpen()){
    cout << "Error, could not open input file: " << filename2 << endl;
    return -1;
  }
  
 
  TTree  *wcsimT = f->Get("wcsimT");
  int nevent = wcsimT->GetEntries();
  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
  wcsimT->SetBranchAddress("wcsimrootevent",&wcsimrootsuperevent);

  // Force deletion to prevent memory leak when issuing multiple
  // calls to GetEvent()
  wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);

  wcsimT->GetEvent(0); 

  // In the default vis.mac, only one event is run.  I suspect you could loop over more events, if they existed.
  WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  cout << "Stats for the first event in your version of WCSim using " << filename << endl;
  cout << "Number of tube hits " << wcsimrootevent->GetNumTubesHit() << endl;

  cout << "Number of digitized tube hits " << wcsimrootevent->GetNumDigiTubesHit() << endl;
  cout << "Number of photoelectron hit times " << wcsimrootevent->GetCherenkovHitTimes()->GetEntries() << endl;

  //Save these to compare with the clean version of the code. 

  int num_tubes =  wcsimrootevent->GetNumTubesHit();
  int num_digi_tubes = wcsimrootevent->GetNumDigiTubesHit();
  int hit_times = wcsimrootevent->GetCherenkovHitTimes()->GetEntries();
 

  // Create a WCSimRootEvent to put stuff from the tree in

  WCSimRootEvent* wcsimrootsuperevent = new WCSimRootEvent();

  // Set the branch address for reading from the tree
  TBranch *branch = wcsimT->GetBranch("wcsimrootevent");
  branch->SetAddress(&wcsimrootsuperevent);

  // Force deletion to prevent memory leak 
  wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);


   // start with the main "subevent", as it contains most of the info
  // and always exists.
  WCSimRootTrigger* wcsimrootevent;

  TH1F *h1 = new TH1F("PMT Hits", "# Digitized Hits", 500, 0, 10000);
  TH1F *time = new TH1F("Average time", "Average time", 600, 900, 2000);
  TH1F *pe = new TH1F("Q/# Digitzed PMT", "Average Charge", 200, 0, 5);
  TH1F *hit_pmts = new TH1F("Hit PMTs","# Hit PMTs",500,0,10000); 

  TH1F *tot_charge = new TH1F("tot","Total Charge in event",200,0,10000);
  std::cout << "nevent: " << nevent << std::endl;
  // Now loop over events
  for (int ev=0; ev<nevent; ev++)
  {
    // Read the event from the tree into the WCSimRootEvent instance
    wcsimT->GetEntry(ev);      
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
    //std::cout << "test: " << wcsimrootsuperevent->GetNumberOfEvents() << std::endl;
    for (int index = 0 ; index < wcsimrootsuperevent->GetNumberOfEvents(); index++) 
      { 
	int ncherenkovdigihits = wcsimrootevent->GetNcherenkovdigihits();
	h1->Fill(ncherenkovdigihits);
	
	hit_pmts->Fill(wcsimrootevent->GetNumTubesHit());	
	//std::cout << "test2: " << index << " " << wcsimrootevent->GetNumTubesHit() << std::endl;
	float totalq = 0.;
	float totalt = 0.;
	// Loop through elements in the TClonesArray of WCSimRootCherenkovHits
	for (int i=0; i< ncherenkovdigihits; i++)
	  {
	    TObject *Digi = (wcsimrootevent->GetCherenkovDigiHits())->At(i);
	    WCSimRootCherenkovDigiHit *wcsimrootcherenkovdigihit = 
	      dynamic_cast<WCSimRootCherenkovDigiHit*>(Digi);
	    
	    int tubeNumber     =  (wcsimrootcherenkovdigihit->GetT(),wcsimrootcherenkovdigihit->GetTubeId());
	    float q = wcsimrootcherenkovdigihit->GetQ();
	    float t = wcsimrootcherenkovdigihit->GetT();
	    totalq+=q;
	    totalt+=t;
	  }

	tot_charge->Fill(totalq);
	float av_time = totalt/ncherenkovdigihits;
	float av_q = totalq/ncherenkovdigihits;
      }
    pe->Fill(av_q);  
    time->Fill(av_time);
    // reinitialize super event between loops.
    wcsimrootsuperevent->ReInitialize();
  }// End of loop over events

TTree  *wcsimT2 = f2->Get("wcsimT");
  int nevent2 = wcsimT2->GetEntries();
  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
  wcsimT2->SetBranchAddress("wcsimrootevent",&wcsimrootsuperevent);

  // Force deletion to prevent memory leak when issuing multiple
  // calls to GetEvent()
  wcsimT2->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);

  wcsimT2->GetEvent(0); 

  // In the default vis.mac, only one event is run.  I suspect you could loop over more events, if they existed.
  WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

  cout << "***********************************************************" << endl;
  cout << "Stats for the first event of WCSim version on GitHub using "<< filename2 << endl;
  cout << "Number of tube hits " << wcsimrootevent->GetNumTubesHit() << endl;

  cout << "Number of digitized tube hits " << wcsimrootevent->GetNumDigiTubesHit() << endl;
  cout << "Number of photoelectron hit times " << wcsimrootevent->GetCherenkovHitTimes()->GetEntries() << endl;

  cout <<  "***********************************************************" << endl;
  if (abs(num_tubes- wcsimrootevent->GetNumTubesHit())>1.0e-6){cout << "FIRST EVENT TEST FAILED: Number of hit tubes do not match" << endl;}
  else {cout << "FIRST EVENT TEST PASSED: Number of hit tubes matches" << endl;}
  if (abs(num_digi_tubes-wcsimrootevent->GetNumDigiTubesHit())>1.0e-6){cout << "FIRST EVENT TEST FAILED: Number of digitized tubes do not match" << endl; }
  else {cout << "FIRST EVENT TEST PASSED: Number of digitized tubes matches" << endl; }
  if (abs(hit_times-(wcsimrootevent->GetCherenkovHitTimes()->GetEntries()))> 1.0e-6){cout << "FIRST EVENT TEST FAILED: Number of hit times do not match" << endl;}
  else {cout << "FIRST EVENT TEST PASSED: Number of hit times matches" << endl;}


  // Create a WCSimRootEvent to put stuff from the tree in

  WCSimRootEvent* wcsimrootsuperevent = new WCSimRootEvent();

  // Set the branch address for reading from the tree
  TBranch *branch = wcsimT2->GetBranch("wcsimrootevent");
  branch->SetAddress(&wcsimrootsuperevent);

  // Force deletion to prevent memory leak 
  wcsimT2->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);


   // start with the main "subevent", as it contains most of the info
  // and always exists.
  WCSimRootTrigger* wcsimrootevent;

  TH1F *h2 = new TH1F("PMT Hits 2", "# Digitized Hits", 500, 0, 10000);
  TH1F *time2 = new TH1F("Average time 2", "Average time", 600, 900, 2000);
  TH1F *pe2 = new TH1F("Q/# Digitzed PMT 2", "Q/# Digitzed PMT", 200, 0, 5);
  TH1F *hit_pmts2 = new TH1F("Hit PMTs 2","# Hit PMTs",500,0,10000);  

  TH1F *tot_charge2 = new TH1F("tot2","Total Charge in event",200,0,10000);  
  // Now loop over events
  for (int ev=0; ev<nevent; ev++)
  {
    // Read the event from the tree into the WCSimRootEvent instance
    wcsimT2->GetEntry(ev);      
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
    
    for (int index = 0 ; index < wcsimrootsuperevent->GetNumberOfEvents(); index++) 
      { 
	int ncherenkovdigihits = wcsimrootevent->GetNcherenkovdigihits();
	h2->Fill(ncherenkovdigihits);
	hit_pmts2->Fill(wcsimrootevent->GetNumTubesHit());	
	
	float totalq = 0.;
	float totalt = 0.;
	// Loop through elements in the TClonesArray of WCSimRootCherenkovHits
	for (int i=0; i< ncherenkovdigihits; i++)
	  {
	    TObject *Digi = (wcsimrootevent->GetCherenkovDigiHits())->At(i);
	    WCSimRootCherenkovDigiHit *wcsimrootcherenkovdigihit = 
	      dynamic_cast<WCSimRootCherenkovDigiHit*>(Digi);
	    
	    int tubeNumber     =  (wcsimrootcherenkovdigihit->GetT(),wcsimrootcherenkovdigihit->GetTubeId());
	    float q = wcsimrootcherenkovdigihit->GetQ();
	    float t = wcsimrootcherenkovdigihit->GetT();
	    totalq+=q;
	    totalt+=t;
	  }

	tot_charge2->Fill(totalq);
	float av_time = totalt/ncherenkovdigihits;
	float av_q = totalq/ncherenkovdigihits;
      }
    pe2->Fill(av_q);  
    time2->Fill(av_time);
    // reinitialize super event between loops.
    wcsimrootsuperevent->ReInitialize();
  }// End of loop over events

	Double_t ks_hits = h1->KolmogorovTest(h2);
	Double_t ks_charge = pe->KolmogorovTest(pe2);
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
  h2->SetLineColor(kRed);
  h1->Draw();
  c1->cd(1); h2->Draw("SAME");

 TLegend *leg = new TLegend(0.2,0.7,0.55,0.85, "");
 leg->SetFillColor(0);
 leg->SetBorderSize(0);
 leg->AddEntry(h1,filename, "l");
 leg->AddEntry(h2,filename2, "l");
 leg->Draw();
 
 c1->cd(2);
 pe->GetXaxis()->SetTitle("Total Charge / # digitized hits");
 pe->Draw();
 pe2->SetLineColor(kRed);
 c1->cd(2); pe2->Draw("SAME");
 c1->cd(3); 
 time->GetXaxis()->SetTitle("Total Time / # digitized hits (ns)");  
 time->Draw();
 time2->SetLineColor(kRed);
 c1->cd(3); time2->Draw("SAME");
  

 c1->cd(4);
 hit_pmts->Draw();
 hit_pmts2->SetLineColor(kRed);
 hit_pmts2->Draw("same");

 TCanvas* c2 = new TCanvas("c2", "Test Plots", 500*n_wide*win_scale, 500*n_high*win_scale);
 tot_charge->Draw();
 tot_charge2->SetLineColor(kRed);
 tot_charge2->Draw("same");
}
