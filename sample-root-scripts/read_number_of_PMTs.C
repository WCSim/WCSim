#include <iostream>
#include <stdio.h>     
#include <stdlib.h>    

#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"

#include "WCSimRootGeom.hh"

// Simple example of reading a generated Root file
int read_number_of_PMTs(const char *filename="../wcsim.root")
{

  // Open the file
  TFile *f = new TFile(filename,"read");
  if (!f->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    return -1;
  }
  
  TTree *tree = (TTree*)f->Get("wcsimGeoT");
  WCSimRootGeom* geom = new WCSimRootGeom();
  TBranch *gb = tree->GetBranch("wcsimrootgeom");
  gb->SetAddress(&geom);
  tree->GetEntry(0);
  Printf("Number of 20\" PMTs: %d", geom->GetWCNumPMT());
  Printf("Number of OD PMTs: %d", geom->GetODWCNumPMT());
  Printf("Number of mPMTs: %d", geom->GetWCNumPMT(true));

  return 0;
}
