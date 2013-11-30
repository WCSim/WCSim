#include <iostream>
#include <TH1F.h>
#include <stdio.h>     
#include <stdlib.h>    
// Simple example of reading a generated Root file
void testgeo(char *filename=NULL)
{

  // Clear global scope
  gROOT->Reset();

  // Load the library with class dictionary info
  // (create with "gmake shared")
  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIMDIR");
  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
  }else{
    gSystem->Load("../libWCSimRoot.so");
  }

  // Open the file
  TFile *file; 
  if (filename==NULL){
    file = new TFile("../wcsim.root","read");
  }else{
    file = new TFile(filename,"read");
  }
  if (!file->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    return -1;
  }
  
  // Get the a pointer to the tree from the file
  TTree *gtree = (TTree*)file->Get("wcsimGeoT");
  
  // Get the number of events
  int nevent = gtree->GetEntries();
  printf("geo nevent %d\n",nevent);
  
  // Create a WCSimRootGeom to put stuff from the tree in

  WCSimRootGeom* wcsimrootgeom = new WCSimRootGeom();

  // Set the branch address for reading from the tree
  TBranch *branch = gtree->GetBranch("wcsimrootgeom");
  branch->SetAddress(&wcsimrootgeom);

  // Now loop over "events"  (should be only one for geo tree)
  int ev;
  for (ev=0;ev<nevent; ev++)  {
      // Read the event from the tree into the WCSimRootGeom instance
      gtree->GetEntry(ev);
      printf("Cyl radius %f\n", wcsimrootgeom->GetWCCylRadius());
      printf("Cyl length %f\n", wcsimrootgeom->GetWCCylLength());
      printf("PMT radius %f\n", wcsimrootgeom->GetWCPMTRadius());
      printf("Offset x y z %f %f %f\n", wcsimrootgeom->GetWCOffset(0),
	     wcsimrootgeom->GetWCOffset(1),wcsimrootgeom->GetWCOffset(2));
      int numpmt = wcsimrootgeom->GetWCNumPMT();
      printf("Num PMTs %d\n", numpmt);

      int i;
      for (i=0;i<((numpmt<20)?numpmt:20);i++){
	WCSimRootPMT pmt;
	pmt = wcsimrootgeom->GetPMT(i);
	printf ("pmt %d %d %d\n",i,pmt.GetTubeNo(), pmt.GetCylLoc());
	printf ("position: %f %f %f\n", pmt.GetPosition(0),
		pmt.GetPosition(1),pmt.GetPosition(2));
	printf ("orientation: %f %f %f\n", pmt.GetOrientation(0),
		pmt.GetOrientation(1),pmt.GetOrientation(2));
      }
            
    } // End of loop over events

}
