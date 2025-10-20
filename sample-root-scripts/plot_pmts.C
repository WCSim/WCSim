#include <iostream>
#include <stdio.h>     
#include <stdlib.h>
#include <algorithm>

#include "TTree.h"
#include "TGraph.h"
#include "TEllipse.h"
#include "TH1F.h"
#include "THStack.h"
#include "TH1D.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TFile.h"

#include "WCSimRootOptions.hh"
#include "WCSimRootGeom.hh"
#include "WCSimRootEvent.hh"

/*
  Produces X-Y and R-Z displays for each PMT type
  - In the X-Y displays, the detector edge is shown as a circle
    - Note that both top & bottom cap PMTs are displayed on this plot
      By construction in WCSim, PMTs are placed in the same location on each end cap
  - In the R-Z displays, the detector edge is the edge of the image

Output
- display_geo.pdf has the 2D plots
  - There is one page per PMT type (20" / OD / mPMT)
- display_geo.root has
  - The TGraph's used to create the 2D plots
  - Additionally two THStack's (one for R, one for Z), that contain the 1D information

Running
From the command line, something like
root -b -q $WCSIM_SOURCE_DIR/sample-root-scripts/plot_pmts.C+g'(1,"wcsim.root")'
or however you prefer to run your root macros
*/

int display(int verbose, //how verbose should the printout be? 0 = lowest, higher number = more
	    const char *filename); //WCSim file. Should work for all geometries

TGraph * MakeGraph(vector<pair<double, double> > v, int col, int style, const float size=1.5)
{
  cout << "Making graph from vector of size " << v.size() << endl;
  TGraph * g = new TGraph(v.size());
  for(size_t i = 0; i < v.size(); i++) {
    auto pair = v.at(i);
    g->SetPoint(i, pair.first, pair.second);
    //cout << pair.first << "\t" << pair.second << endl;
  }
  g->SetMarkerColor(col);
  g->SetMarkerStyle(style);
  g->SetMarkerSize(size);
  return g;
}

double GetAzimuth(double x, double y)
{
  double t = TMath::ATan(y / x);
  if(x < 0) {
    if(y < 0)
      t -= TMath::Pi();
    else
      t += TMath::Pi();
  }
  t = TMath::ATan2(y, x);
  return t;
}

void PrintPMT(const WCSimRootPMT * tube, const int ipmt)
{
  double x, y, z, r, t;
  x = tube->GetPosition(0);
  y = tube->GetPosition(1);
  z = tube->GetPosition(2);
  r = TMath::Sqrt(x*x + y*y);
  t = GetAzimuth(x, y);
  cout << std::setw(6) << ipmt
       << " " << std::setw(8) << tube->GetTubeNo()
       << " " << std::setw(8) << tube->GetmPMTNo()
       << " " << std::setw(3) << tube->GetmPMT_PMTNo()
       << " " << std::setw(9) << x
       << " " << std::setw(9) << y
       << " " << std::setw(9) << z
       << " " << std::setw(9) << r
       << " " << std::setw(9) << t
       << " " << std::setw(2) << tube->GetCylLoc()
       << endl;
}

std::string CycLocs[9] = {"ID top cap",
			  "ID barrel",
			  "ID bottom cap",
			  "OD bottom cap",
			  "OD barrel",
			  "OD top cap",
			  "mPMT top cap",
			  "mPMT barrel",
			  "mPMT bottom cap"};

void DumpGeoTree(WCSimRootGeom * geo,
		 TCanvas * c,
		 int verbose)
{
  const float detR = geo->GetWCCylRadius();
  const float detZ = geo->GetWCCylLength() / 2;

  TClonesArray * pmts  = geo->GetPMTs();
  cout << "N 20\" PMTs in TClonesArray: " << pmts->GetEntries() << endl
       << "N 20\" PMTs from rootgeom  : " << geo->GetWCNumPMT() << endl;
  TClonesArray * pmts2 = geo->GetPMTs(true);
  cout << "N mPMTs in TClonesArray: " << pmts2->GetEntries() << endl
       << "N mPMTs from rootgeom  : " << geo->GetWCNumPMT(true) << endl;
  TClonesArray * pmtsod = geo->GetODPMTs();
  cout << "N OD PMTs in TClonesArray: " << pmtsod->GetEntries() << endl;
  cout << "N OD PMTs from rootgeom  : " << geo->GetODWCNumPMT() << endl;
  
  const int narrays = 9;
  vector<pair<double, double> > pos[narrays];
  TH1D * Rs[narrays], * Zs[narrays];
  vector<double> Rsv[narrays], Zsv[narrays];
  THStack * stackR = new THStack("stackR", ";R (cm);Number in bin");
  THStack * stackZ = new THStack("stackZ", ";Z (cm);Number in bin");
  for(int i = 0; i < narrays; i++) {
    Rs[i] = new TH1D(TString::Format("R%d", i), ";R (cm);Number in bin", TMath::Ceil(detR*8), 0, detR);
    Zs[i] = new TH1D(TString::Format("Z%d", i), ";Z (cm);Number in bin", TMath::Ceil(detZ*8), -detZ, +detZ);
    stackR->Add(Rs[i]);
    stackZ->Add(Zs[i]);
  }
  double x, y, z, r, t;

  //Get the 20" PMT information
  if(verbose > 1)
    cout << endl << "20\" PMT list" << endl;
  for(int ipmt = 0; ipmt < pmts->GetEntries() - 1; ipmt++) {
    const WCSimRootPMT * tube = geo->GetPMTPtr(ipmt);
    //get the tube position. Depends on cycloc
    x = tube->GetPosition(0);
    y = tube->GetPosition(1);
    z = tube->GetPosition(2);
    r = TMath::Sqrt(x*x + y*y);
    t = GetAzimuth(x, y);
    const int loc = tube->GetCylLoc();
    switch(loc) {
      //endcap
    case 0:
    case 2:
      pos[loc].push_back(std::make_pair(x, y));
      break;
      //wall
    case 1:
      pos[loc].push_back(std::make_pair(t, z));
      break;
    default:
      cout << "Unknown case " << tube->GetCylLoc() << endl;
      break;
    }
    Rs[loc]->Fill(r);
    Zs[loc]->Fill(z);
    Rsv[loc].push_back(r);
    Zsv[loc].push_back(z);
    if(verbose > 1)
      PrintPMT(tube, ipmt);
  }//20" PMTs

  //and for OD pmts
  if(verbose > 1)
    cout << endl << "OD PMT list" << endl;
  for(int ipmt = 0; ipmt < pmtsod->GetEntries() - 1; ipmt++) {
    const WCSimRootPMT * tube = geo->GetODPMTPtr(ipmt);
    //get the tube position. Depends on cycloc
    x = tube->GetPosition(0);
    y = tube->GetPosition(1);
    z = tube->GetPosition(2);
    r = TMath::Sqrt(x*x + y*y);
    t = GetAzimuth(x, y);
    const int loc = tube->GetCylLoc();
    switch(loc) {
      //endcap
    case 3:
    case 5:
      pos[loc].push_back(std::make_pair(x, y));
      break;
      //wall
    case 4:
      pos[loc].push_back(std::make_pair(t, z));
      break;
    default:
      cout << "Unknown case " << tube->GetCylLoc() << endl;
      break;
    }
    Rs[loc]->Fill(r);
    Zs[loc]->Fill(z);
    Rsv[loc].push_back(r);
    Zsv[loc].push_back(z);
    if(verbose > 1)
      PrintPMT(tube, ipmt);
  }//OD PMTs

  //and for mPMTs
  if(verbose > 1)
    cout << endl << "mPMT list" << endl;
  for(int ipmt = 0; ipmt < pmts2->GetEntries() - 1; ipmt++) {
    const WCSimRootPMT * tube = geo->GetPMTPtr(ipmt, true);
    //get the tube position. Depends on cycloc
    x = tube->GetPosition(0);
    y = tube->GetPosition(1);
    z = tube->GetPosition(2);
    r = TMath::Sqrt(x*x + y*y);
    t = GetAzimuth(x, y);
    const int loc = tube->GetCylLoc();
    switch(loc) {
      //endcap
    case 6:
    case 8:
      pos[loc].push_back(std::make_pair(x, y));
      break;
      //wall
    case 7:
      pos[loc].push_back(std::make_pair(t, z));
      break;
    default:
      cout << "Unknown case " << tube->GetCylLoc() << endl;
      break;
    }
    Rs[loc]->Fill(r);
    Zs[loc]->Fill(z);
    Rsv[loc].push_back(r);
    Zsv[loc].push_back(z);
    if(verbose > 1)
      PrintPMT(tube, ipmt);
  }//mPMTs

  //now setup the plotting
  c->SaveAs("display_geo.pdf[");
  TGraph * g[narrays];
  Color_t cols[narrays] = {kBlack, kBlue, kRed,
			   kBlack, kBlue, kRed,
			   kBlack, kBlue, kRed};
  Style_t styles[narrays] = {29, 29, 29,
			     20, 20, 20,
			     21, 21, 21};
  for(int i = 0; i < narrays; i++) {
    cout << "Vector " << i << " (" << CycLocs[i]
	 << ") contains " << pos[i].size() << " entries" << endl;
    g[i] = MakeGraph(pos[i], cols[i], styles[i], 0.2);
  }
  
  TEllipse circle(0, 0, detR);
  for(int i = 0; i < narrays; i+=3) {
    c->cd(1)->DrawFrame(-detR,-detR,+detR,+detR,";X (cm);Y (cm)")->GetYaxis()->SetTitleOffset(1);
    circle.Draw();
    if(g[i]->GetN())
      g[i]->Draw("P");
    if(g[i+2]->GetN())
      g[i+2]->Draw("P");
    c->cd(2)->DrawFrame(-TMath::Pi(),-detZ,+TMath::Pi(),+detZ,";Azimuthal angle;Z (cm)")->GetYaxis()->SetTitleOffset(1);
    if(g[i+1]->GetN())
      g[i+1]->Draw("P");
    c->SaveAs("display_geo.pdf");
  }
  c->SaveAs("display_geo.pdf]");
  
  TFile fout("display_geo.root", "RECREATE");
  for(int i = 0; i < narrays; i++)
    g[i]->Write(TString::Format("graph%d", i));
  stackR->Write();
  stackZ->Write();
  fout.Close();

  for(int i = 0; i < narrays; i++) {
    cout << i << " " << CycLocs[i] << endl;
    if(!Rsv[i].size()) {
      cout << " No PMTs in this cycloc" << endl;
      continue;
    }
    cout << " MIN R: " << *std::min_element(Rsv[i].begin(), Rsv[i].end())
	 << "\t MAX R: " << *std::max_element(Rsv[i].begin(), Rsv[i].end()) << endl;
    cout << " MIN Z: " << *std::min_element(Zsv[i].begin(), Zsv[i].end())
	 << "\t MAX Z: " << *std::max_element(Zsv[i].begin(), Zsv[i].end()) << endl;
  }
  cout << "Detector half z: " << detZ << endl
       << "Detector radius: " << detR << endl;
}



/////////////////////
int plot_pmts(int verbose=0,
	      const char *filename="wcsim.root")
{
  // Clear global scope
  //gROOT->Reset();

  // Open the file
  TFile * file = new TFile(filename,"read");
  if (!file->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    return -1;
  }
    
  // Geometry tree - only need 1 "event"
  TTree *geotree = (TTree*)file->Get("wcsimGeoT");
  WCSimRootGeom *geo = 0; 
  geotree->SetBranchAddress("wcsimrootgeom", &geo);
  if(verbose) std::cout << "Geotree has: " << geotree->GetEntries() << " entries (1 expected)" << std::endl;
  if (geotree->GetEntries() == 0) {
      exit(9);
  }
  geotree->GetEntry(0);

  // Options tree - only need 1 "event"
  TTree *opttree = (TTree*)file->Get("wcsimRootOptionsT");
  WCSimRootOptions *opt = 0; 
  opttree->SetBranchAddress("wcsimrootoptions", &opt);
  if(verbose) std::cout << "Options tree has: " << opttree->GetEntries() << " entries (1 expected)" << std::endl;
  if (opttree->GetEntries() == 0) {
    exit(9);
  }
  opttree->GetEntry(0);
  opt->Print();

  float win_scale = 0.75;
  int n_wide(2);
  int n_high(2);
  TCanvas* c1 = new TCanvas("c1", "First canvas", 1000*n_wide*win_scale, 500*n_high*win_scale);
  c1->Divide(2,1);
  
  DumpGeoTree(geo, c1, verbose);

  return 0;
}
