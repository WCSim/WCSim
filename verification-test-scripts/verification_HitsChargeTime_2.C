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


  TH1F *h1 = new TH1F("PMT Hits", "# Digitized Hits", 500, 0, 25000);
  TH1F *time = new TH1F("Average time", "Average time", 600, 500, 5000);
  TH1F *pe = new TH1F("Q/# Digitzed PMT", "Average Charge", 200, 0, 5);
  TH1F *hit_pmts = new TH1F("Hit PMTs","# Hit PMTs",500,0,35000); 

  TH1F *tot_charge = new TH1F("tot","Total Charge in event",500,0,20000);

  TH1F *charge = new TH1F("charge","",200,0,1000);
  TH1F *ttime = new TH1F("ttime","",200,900,2000);
  TH2F *t_q = new TH2F("tq","",200,900,2000,200,0,20);
  //TH2F *tubeID_q = new TH2F("tubeIDq","",150,185100,185250,200,0,100);
  TH2F *tubeID_q = new TH2F("tubeIDq","",300,1400,1700,100,0,300);
  TH2F *mPMTid_q = new TH2F("mPMTidq","",34,1,35,100,0,300);
  TH1F *occupancy = new TH1F("occupancy","",20000,0,20000);
  TH1F *occupancy_mPMT = new TH1F("occupancy_mPMT","",35,0,35);
  TH1F *occupancy_mPMT2 = new TH1F("occupancy_mPMT2","",200,0,20000);
 TH1F *nhit_pmt = new TH1F("nhit_pmt","",30,0,30);
  std::cout << "nevent: " << nevent << std::endl;
  // Now loop over events
  for (int ev=0; ev<nevent; ev++)
  {
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


    //Calculate distance vertex to center, in z plane
    // dWall
    // muon range
    // end point to center
    double vtx_x = wcsimrootevent->GetVtx(0);
    double vtx_y = wcsimrootevent->GetVtx(1);
    double vtx_z = wcsimrootevent->GetVtx(2);
    double dist = sqrt(vtx_x*vtx_x + vtx_y * vtx_y);
    
    //GetTracks, pick muon, use GetStart and GetStop
    for (int i = 0; i < wcsimrootevent->GetNtrack() ; i++){
      WCSimRootTrack *track = dynamic_cast<WCSimRootTrack*>((wcsimrootevent->GetTracks())->At(i));
      if( track->GetIpnu() == 13 ) { // mu-
	//GetStart(0)
	//GetStop(0)
	break;
      }
    }

    //std::cout << "test: " << wcsimrootsuperevent->GetNumberOfEvents() << std::endl;
    //if(wcsimrootevent->GetNumTubesHit() > wcsimrootevent->GetNcherenkovdigihits() + 2000)
    if(wcsimrootsuperevent->GetNumberOfEvents() > 1)
      //std::cout << "test2: " << ev << " " << wcsimrootsuperevent->GetNumberOfEvents() << " " << wcsimrootevent->GetNumTubesHit() << " " << wcsimrootevent->GetNcherenkovdigihits() << std::endl;
      
      std::cout << wcsimrootsuperevent->GetNumberOfEvents() << std::endl;

    for (int index = 0 ; index < wcsimrootsuperevent->GetNumberOfEvents(); index++) 
      { 
	int ncherenkovdigihits = wcsimrootevent->GetNcherenkovdigihits();
	h1->Fill(ncherenkovdigihits);
	
	//	std::cout << "DEBUG " <<  ev << " " << index << " " << wcsimrootevent->GetNumTubesHit() << std::endl;
	hit_pmts->Fill(wcsimrootevent->GetNumTubesHit());	
	float totalq = 0.;
	float totalt = 0.;
	
	std::cout << ncherenkovdigihits << std::endl;
	std::cout << wcsimrootevent->GetNumTubesHit() << std::endl;
	//TH1F *occup_per_event = new TH1F("occup_per_event","",20000,0,20000);
	// Loop through elements in the TClonesArray of WCSimRootCherenkovHits
	for (int i=0; i< ncherenkovdigihits; i++){
	    TObject *Digi = (wcsimrootevent->GetCherenkovDigiHits())->At(i);
	    WCSimRootCherenkovDigiHit *wcsimrootcherenkovdigihit = 
	      dynamic_cast<WCSimRootCherenkovDigiHit*>(Digi);
	    
	    float q = wcsimrootcherenkovdigihit->GetQ();
	    float t = wcsimrootcherenkovdigihit->GetT();
	    charge->Fill(q);
	    ttime->Fill(t);
	    t_q->Fill(t,q);
	    occupancy->Fill(tubeNumber);
	    //occup_per_event->Fill(tubeNumber);
	    occupancy_mPMT->Fill(tubeNumber%33 == 0 ? 33 : tubeNumber%33);
	    occupancy_mPMT2->Fill(tubeNumber/33);
	    totalq+=q;
	    totalt+=t;

	    tubeID_q->Fill(tubeNumber,q);
	    mPMTid_q->Fill(tubeNumber%33 == 0 ? 33 : tubeNumber%33,q);
	  }
	
	tot_charge->Fill(totalq);
	float av_time = totalt/ncherenkovdigihits;
	float av_q = totalq/ncherenkovdigihits;

	/*
	for(int j = 1; j < 15000; j++ ){//occupancy->GetNbinsX() ; j++){
	  if(occup_per_event->GetBinContent(j) > 0 )
	    nhit_pmt->Fill(occup_per_event->GetBinContent(j));
	    }*/

      }
    pe->Fill(av_q);  
    time->Fill(av_time);

    // reinitialize super event between loops.
    wcsimrootsuperevent->ReInitialize();
  }// End of loop over events


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

  TH1F *h2 = new TH1F("PMT Hits 2", "# Digitized Hits", 500, 0, 25000);
  TH1F *time2 = new TH1F("Average time 2", "Average time", 600, 500, 5000);
  TH1F *pe2 = new TH1F("Q/# Digitzed PMT 2", "Q/# Digitzed PMT", 200, 0, 5);
  TH1F *hit_pmts2 = new TH1F("Hit PMTs 2","# Hit PMTs",500,0,35000);  

  TH1F *tot_charge2 = new TH1F("tot2","Total Charge in event",500,0,20000);  

  TH1F *charge2 = new TH1F("charge2","",200,0,1000);
  TH1F *ttime2 = new TH1F("ttime2","",200,900,2000);
  TH1F *occupancy2 = new TH1F("occupancy2","",200,0,500000);
  TH1F *occupancy2_mPMT = new TH1F("occupancy2_mPMT","",34,0,34);
  TH1F *occupancy2_mPMT2 = new TH1F("occupancy2_mPMT2","",200,0,20000);

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
    
    for (int index = 0 ; index < wcsimrootsuperevent->GetNumberOfEvents(); index++) 
      { 
	int ncherenkovdigihits = wcsimrootevent->GetNcherenkovdigihits();
	h2->Fill(ncherenkovdigihits);
	hit_pmts2->Fill(wcsimrootevent->GetNumTubesHit());	
	
	float totalq = 0.;
	float totalt = 0.;
	// Loop through elements in the TClonesArray of WCSimRootCherenkovHits
	for (int i=0; i< ncherenkovdigihits; i++){
	    TObject *Digi = (wcsimrootevent2->GetCherenkovDigiHits())->At(i);
	    WCSimRootCherenkovDigiHit *wcsimrootcherenkovdigihit = 
	      dynamic_cast<WCSimRootCherenkovDigiHit*>(Digi);
	    
	    float q = wcsimrootcherenkovdigihit->GetQ();
	    float t = wcsimrootcherenkovdigihit->GetT();
	    charge2->Fill(q);
	    ttime2->Fill(t);
	    totalq+=q;
	    totalt+=t;
	  }

	tot_charge2->Fill(totalq);
	if(ncherenkovdigihits > 0){
	  float av_time = totalt/ncherenkovdigihits;
	  float av_q = totalq/ncherenkovdigihits;
	}
      }
    pe2->Fill(av_q);  
    time2->Fill(av_time);
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
 // leg->AddEntry(h1,filename, "l");
 //leg->AddEntry(h2,filename2, "l");
 leg->AddEntry(h1,"mPMT", "l");
 leg->AddEntry(h2,"20in", "l");
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
  

 c1->cd(4);
 hit_pmts->Draw();
 hit_pmts2->SetLineColor(kRed);
 hit_pmts2->Draw("same");

 TCanvas* c2 = new TCanvas("c2", "Test Plots", 500*n_wide*win_scale, 500*n_high*win_scale);
 gStyle->SetOptStat(1);
 gStyle->SetOptFit(111);
 /* TF1 *func = new TF1("func","gaus(0)",0,20000);
 func->SetParameters(10,5000,10);
 TF1 *func2 = new TF1("func2","gaus(0)",0,20000);
 func2->SetParameters(10,5000,10); */
 tot_charge->Draw();
 //tot_charge->Fit("gaus","","",tot_charge->GetMean()-1.5*tot_charge->GetRMS(),tot_charge->GetMean()+1.5*tot_charge->GetRMS());
 tot_charge2->SetLineColor(kRed);
 tot_charge2->Draw("same");
 //tot_charge2->Fit("gaus","","",tot_charge->GetMean()-1.5*tot_charge->GetRMS(),tot_charge->GetMean()+1.5*tot_charge->GetRMS());
 //std::cout << "Tot charge ratio : " << func->GetParameter(0)/func2->GetParameter(0) << std::endl;

 TCanvas *c3 = new TCanvas();
 c3->Divide(2,2);
 c3->cd(1);
 occupancy_mPMT2->Draw();
 c3->cd(2);
 occupancy_mPMT->Draw();
 c3->cd(3);
 ttime->Draw();
 ttime2->SetLineColor(kRed);
 ttime2->Draw("same");
 c3->cd(4);
 charge->Draw();
 charge2->SetLineColor(kRed);
 charge2->Draw("same");


 TCanvas *c4 = new TCanvas();

 nhit_pmt->Draw();

 TCanvas *c5 = new TCanvas();
 t_q->Draw("colz");
}
