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
  Printf("Number of ID PMTs of the first type (e.g. 20\" PMTs in HK FD, 3\" PMTs in mPMTs in IWCD): %d", geom->GetWCNumPMT());
  Printf("Number of OD PMTs: %d", geom->GetODWCNumPMT());
  Printf("Number of ID PMTs of the second type (e.g. 3\" PMTs in mPMTs in HK FD): %d", geom->GetWCNumPMT(true));

  cout << "Tank radius & height: "
       << geom->GetWCCylRadius() << "\t"
       << geom->GetWCCylLength() << endl;

  //printout of the first few PMTs of the first type
  // See WCSimRootGeom.hh for how to get more PMT information,
  // and for how to get information about other PMT types
  cout << "PMT radius " << geom->GetWCPMTRadius(false) << endl;
  for(int i = 0; i < 10; i++)
    cout << "PMT " << i << "\tX, Y, Z: "
	 << geom->GetPMT(i).GetPosition(0) << ",\t"
	 << geom->GetPMT(i).GetPosition(1) << ",\t"
	 << geom->GetPMT(i).GetPosition(2) << endl;

  return 0;
}
