{
gSystem->Load("../libWCSimRoot.so");
TFile *f2 = new TFile("../wcsim.root");  
TTree *tree = (TTree*)f2->Get("wcsimGeoT");
WCSimRootGeom* geom = new WCSimRootGeom();
TBranch *gb = tree->GetBranch("wcsimrootgeom");
gb->SetAddress(&geom);
tree->GetEntry(0);
Printf("Number of PMTs: %d", geom->GetWCNumPMT());
}
