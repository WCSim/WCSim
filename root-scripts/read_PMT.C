#include <stdio.h>     
#include <stdlib.h>    

void read_PMT(char *filename=NULL) {
  /* A simple script to plot aspects of phototube hits 
   * This code is rather cavalier; I should be checking return values, etc.
   * First revision 6-24-10 David Webber
   * 
   * I like to run this macro as 
   * $ root -l -x 'read_PMT.C("../wcsim_momentum.root")
   * $ root -l -x 'read_PMT.C("../wcsim_momentum_electron_10.root")
   */

  gROOT->Reset();
  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIMDIR");
  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
  }else{
    gSystem->Load("../libWCSimRoot.so");
  }
  gStyle->SetOptStat(1);

  TFile *f;
  if (filename==NULL){
    f = new TFile("../wcsim.root");
  }else{
    f = new TFile(filename);
  }
  if (!f->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    return -1;
  }

  TTree  *wcsimT = f->Get("wcsimT");

  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
  wcsimT->SetBranchAddress("wcsimrootevent",&wcsimrootsuperevent);

  // Force deletion to prevent memory leak when issuing multiple
  // calls to GetEvent()
  wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);

  int nevents = wcsimT->GetEntries();
  std::cout << "number events " << nevents << std::endl;
  wcsimT->GetEvent(0); 
  WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  cout << "Number of digitized tube hits " << wcsimrootevent->GetCherenkovDigiHits()->GetEntries() << endl;
  
  wcsimT->GetEvent(1); 
  WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  cout << "Number of digitized tube hits " << wcsimrootevent->GetCherenkovDigiHits()->GetEntries() << endl;

  wcsimT->GetEvent(2); 
  WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  cout << "Number of digitized tube hits " << wcsimrootevent->GetCherenkovDigiHits()->GetEntries() << endl;
  


  WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  cout << "Number of digitized tube hits " << wcsimrootevent->GetCherenkovDigiHits()->GetEntries() << endl;
  
 //-----------------------

  TH1D *Pmomen = new TH1D("momentum","momentum distribution", 100,-0.5,10000);
  Pmomen->SetXTitle("momentum");

  TH1D *PMT_momentum = new TH1D("PMT_momentum","Momentum vs PMT detector number", 120000,-0.5,120000-0.5);

  for (int ivt = 0; ivt < nevents; ivt++){
    wcsimT->GetEvent(ivt);   

    WCSimRootTrack *thistrack = wcsimrootevent->GetTracks()->At(0); 
    Pmomen->Fill(thistrack->GetP());
  }

//----------------------------

  TH2D *PvH = new TH2D("PvH","momentum vs. hits", 40, -0.5, 10000, 40, -0.5, 40000);
  PvH->SetXTitle("momentum");
  PvH->SetYTitle("hits");

   for (int ivt = 0; ivt < nevents; ivt++){
    wcsimT->GetEvent(ivt);   
    
    int nhits = wcsimrootevent->GetCherenkovDigiHits()->GetEntries());
    //WCSimRootTrack *thistrack = wcsimrootevent->GetTracks()->At(0); 
    PvH->Fill( thistrack->GetP(), nhits);
  }
 
 //---------------------------
  TH1 *temp;
  float win_scale=0.75;
  int n_wide=1;
  int n_high=2;
  TCanvas *c1 = new TCanvas("c1","c1",700*n_wide*win_scale,500*n_high*win_scale);
  c1->Divide(n_wide,n_high);

  c1->cd(1);
  temp=Pmomen;
  temp->Draw();
  c1->GetPad(1);

  c1->cd(2);
  temp=PvH;
  temp->Draw("COLZ");
  c1->GetPad(2);

}
