#include <stdio.h>     
#include <stdlib.h>    

void read_PMT(char *filename=NULL) {
  /* A simple script to plot aspects of phototube hits 
   * This code is rather cavalier; I should be checking return values, etc.
   * First revision 6-24-10 David Webber
   * 
   * I like to run this macro as 
   * $ root -l -x 'read_PMT.C("../wcsim.root")'
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

  wcsimT->GetEvent(0); 
  WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  cout << "Number of digitized tube hits " << wcsimrootevent->GetCherenkovDigiHits()->GetEntries() << endl;
  
  wcsimT->GetEvent(1); 
  WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  cout << "Number of digitized tube hits " << wcsimrootevent->GetCherenkovDigiHits()->GetEntries() << endl;

  wcsimT->GetEvent(2); 
  WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  cout << "Number of digitized tube hits " << wcsimrootevent->GetCherenkovDigiHits()->GetEntries() << endl;
  
  wcsimT->GetEvent(3); 
  WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  cout << "Number of digitized tube hits " << wcsimrootevent->GetCherenkovDigiHits()->GetEntries() << endl;
  
  //-----------------------

  TH1D *PE = new TH1D("PEmult","Photoelectron multiplicty", 16,-0.5,15.5);
  PE->SetXTitle("Photoelectrons");

  TH1D *PMT_hits = new TH1D("PMT_hits","Hits vs PMT detector number", 120000,-0.5,120000-0.5);
  int max=wcsimrootevent->GetNcherenkovhits();
  for (int i = 0; i<max; i++){
    WCSimRootCherenkovHit *chit = wcsimrootevent->GetCherenkovHits()->At(i);
    PMT_hits->Fill(chit->GetTubeID());
    //WCSimRootCherenkovHit has methods GetTubeId(), GetTotalPe(int)
    PE->Fill(chit->GetTotalPe(1));
  }
 //-----------------------

  TH1D *Pmomen = new TH1D("momentum","momentum", 16,-0.5,15.5);
  Pmomen->SetXTitle("momentum");

  TH1D *PMT_momentum = new TH1D("PMT_momentum","Momentum vs PMT detector number", 120000,-0.5,120000-0.5);
  int max=wcsimrootevent->GetNcherenkovhits();
  int max=3;
  for (int i = 0; i<max; i++){
    WCSimRootTrack *momentum = wcsimrootevent->GetTracks()->At(i); 
    PMT_hits->Fill(chit->GetTubeID());
    Pmomen->Fill(momentum->GetP());
  }

 
 //---------------------------
  TH1 *temp;
  float win_scale=0.75;
  int n_wide=2;
  int n_high=1;
  TCanvas *c1 = new TCanvas("c1","c1",700*n_wide*win_scale,500*n_high*win_scale);
  c1->Divide(n_wide,n_high);

  c1->cd(1);
  temp=PE;
  temp->Draw();
  c1->GetPad(1)->SetLogy();

 c1->cd(2);
  temp=PE;
  temp->Draw();
  c1->GetPad(2)->SetLogy();
}
