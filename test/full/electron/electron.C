void make_plots(TFile *event_file, TTree *wcsimT, TFile *out_file)
{
  TH1D *pe = new TH1D("ncher",";hit PMTs per event", 20, 3800, 4150);
  
  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
  wcsimT->SetBranchAddress("wcsimrootevent",&wcsimrootsuperevent);

  // Force deletion to prevent memory leak when issuing multiple
  // calls to GetEvent()
  wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);
  for (int i=0; i < wcsimT->GetEntries(); i++)
  {
    wcsimT->GetEvent(i);
    WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
    pe->Fill(wcsimrootevent->GetNcherenkovhits());
  }
  
  out_file->cd();
  pe->Write();
}
