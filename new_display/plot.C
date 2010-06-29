void plot(){
  TFile *file = new TFile("temp.root");
  TTree *tree  = (TTree*)file->Get("wcsimT");
  WCSimRootEvent* wcsimrootevent = new WCSimRootEvent();
  
  TBranch *branch = tree->GetBranch("wcsimrootevent");
  branch->SetAddress(&wcsimrootevent);

  TTree *gtree = (TTree*)file->Get("wcsimGeoT");
  WCSimRootGeom* wcsimrootgeom = new WCSimRootGeom();
  TBranch *gbranch = gtree->GetBranch("wcsimrootgeom");

  gbranch->SetAddress(&wcsimrootgeom);

  gtree->GetEntry(0);

  WCSimRootTrigger* wcsimroottrigger;

  tree->GetEntry(0);
  wcsimroottrigger = wcsimrootevent->GetTrigger(0);
  
  TClonesArray *list = wcsimroottrigger->GetCherenkovHits();
  cout << wcsimroottrigger->GetNcherenkovhits() << endl;
  ofstream outfile("temp.dat");
  for (Int_t i=0;i!=wcsimroottrigger->GetNcherenkovhits();i++){
    outfile  << ((WCSimRootCherenkovHit*)(list->At(i))) ->GetTubeID() << "\t" << wcsimrootgeom->GetPMT(((WCSimRootCherenkovHit*)(list->At(i))) ->GetTubeID()-1)->GetPosition(0) << "\t" << wcsimrootgeom->GetPMT(((WCSimRootCherenkovHit*)(list->At(i))) ->GetTubeID()-1)->GetPosition(1) << "\t" <<  wcsimrootgeom->GetPMT(((WCSimRootCherenkovHit*)(list->At(i))) ->GetTubeID()-1)->GetPosition(2) << endl;
  }
  outfile.close();
}
