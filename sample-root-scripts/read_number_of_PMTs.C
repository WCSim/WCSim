#include <iostream>
#include <TH1F.h>
#include <stdio.h>     
#include <stdlib.h>    
// Simple example of reading a generated Root file
void read_number_of_PMTs(char *filename=NULL)
{

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
    f = new TFile("../wcsim.root","read");
  }else{
    f = new TFile(filename,"read");
  }
  if (!f->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    return -1;
  }
  
  TTree *tree = (TTree*)f->Get("wcsimGeoT");
  WCSimRootGeom* geom = new WCSimRootGeom();
  TBranch *gb = tree->GetBranch("wcsimrootgeom");
  gb->SetAddress(&geom);
  tree->GetEntry(0);
  Printf("Number of PMTs: %d", geom->GetWCNumPMT());
}
