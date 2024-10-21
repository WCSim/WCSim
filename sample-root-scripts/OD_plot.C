#include <string.h>
#include <iostream>
#include <stdio.h>     
#include <stdlib.h>

#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TFile.h"

#include "WCSimRootOptions.hh"
#include "WCSimRootGeom.hh"
#include "WCSimRootEvent.hh"

int OD_plot(const char *filename="../wcsim.root")
{
  TFile * fout = new TFile("output.root","RECREATE");

  
  TFile * file = new TFile(filename,"read");
  if (!file->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    return -1;
  }
  
  TTree *tree = (TTree*)file->Get("wcsimT");
  const long nevent = tree->GetEntries();

  WCSimRootEvent* wcsimrootsuperevent_OD = new WCSimRootEvent();
  TBranch *branch_OD = 0;
  bool has_OD = false;
  for(int i = 0; i < tree->GetListOfBranches()->LastIndex(); i++){
    std::cout << "branch: "<< tree->GetListOfBranches()->At(i)->GetName() << std::endl;
    if( strcmp(tree->GetListOfBranches()->At(i)->GetName(),"wcsimrootevent_OD") == 0 ){
      branch_OD = (TBranch*)tree->GetListOfBranches()->At(i);
      has_OD = true;
      break;
    }
  }
  if( has_OD )
    branch_OD->SetAddress(&wcsimrootsuperevent_OD);

  tree->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);

  TTree *geotree = (TTree*)file->Get("wcsimGeoT");
  WCSimRootGeom *geo = 0; 
  geotree->SetBranchAddress("wcsimrootgeom", &geo);
  if (geotree->GetEntries() == 0) {
      exit(9);
  }
  geotree->GetEntry(0);

  WCSimRootTrigger* wcsimrootevent_OD;

  const float detR = geo->GetWCCylRadius();
  const float detZ = geo->GetWCCylLength();
  int n_pmts_ID = geo->GetWCNumPMT();
  int n_pmts_OD = geo->GetODWCNumPMT();
  double maxR = detR*1.1;
  double maxH = detZ/2.*1.1;
  maxR=4000.;
  maxH=4000.;
  std::cout << " detR " << detR << " detZ " << detZ << std::endl;
  std::cout << " n_pmts_ID " << n_pmts_ID << " n_pmts_OD " << n_pmts_OD << " nevents " << nevent << std::endl;
  int count_OD_barrel=0;
  int count_OD_top=0;
  int count_OD_bottom=0;

  TH1F *h_nhits_OD = new TH1F("h_nhits_OD", "nhits; number of hits", n_pmts_OD, 0,n_pmts_OD);
  TH1F *h_hits_time_OD = new TH1F("h_hits_time_OD", "hits_time; hits time [ns]", 100, 1, -1);
  TH1F *h_hits_charge_OD = new TH1F("h_hits_charge_OD", "hits_charge; OD hits charge [p.e.]", 100, 1, -1);
  TH1F *h_hits_PMT_tube_id_OD = new TH1F("h_hits_PMT_tube_id_OD", "hits_PMT_tube_id; OD tube id", 100, 0,n_pmts_OD);
  TH2F *h_hits_map_OD = new TH2F("h_hits_map_OD", "hits_map; #phi [rad]; z [cm]", 50, -3.3, 3.3, 50, -maxH, maxH);
  TH2F *h_hits_map_top_OD = new TH2F("h_hits_map_top_OD", "hits_map_top; x [cm]; y [cm]", 50, -maxR, maxR, 50, -maxR, maxR);
  TH2F *h_hits_map_bottom_OD = new TH2F("h_hits_map_bottom_OD", "hits_map_bottom; x [cm]; y [cm]", 50, -maxR, maxR, 50, -maxR, maxR);
  TH3F *OD_PMTS = new TH3F("OD_PMTS", "OD PMTs; x [cm]; y [cm]; z[cm]", 100, -maxR, maxR,100,-maxR, maxR,100,-maxH, maxH);
  TH2F *h2_z_orientation = new TH2F("h2_z_orientation", "h2_z_orientation; orientation; z [cm]", 50, -1.1,1.1, 50, -maxH, maxH);
  
  for(int i=0; i<geo->GetODWCNumPMT(); i++){
    WCSimRootPMT pmt = geo->GetPMT(geo->GetWCNumPMT()+i);
    double pmt_x = pmt.GetPosition(0);
    double pmt_y = pmt.GetPosition(1);
    double pmt_z = pmt.GetPosition(2);
    OD_PMTS->Fill(pmt_x, pmt_y, pmt_z);
    h2_z_orientation->Fill(pmt.GetOrientation(2), pmt_z);
    if( fabs(pmt.GetOrientation(2)) < 0.01 )
      count_OD_barrel++;
    else if( pmt.GetOrientation(2) > 0.01 )
      count_OD_top++;
    else
      count_OD_bottom++;
  }

  std::cout << " count_OD_barrel " << count_OD_barrel << " count_OD_top " << count_OD_top << "  count_OD_bottom " << count_OD_bottom << std::endl;

  for (long ievent=0; ievent<nevent; ievent++){
    tree->GetEntry(ievent);      
    if( has_OD )
      wcsimrootevent_OD = wcsimrootsuperevent_OD->GetTrigger(0);

    int ncherenkovdigihits_OD = 0;
    if( has_OD )
      ncherenkovdigihits_OD = wcsimrootevent_OD->GetNcherenkovdigihits(); 
    
    h_nhits_OD->Fill(ncherenkovdigihits_OD);
    for (int idigi=0;idigi<ncherenkovdigihits_OD;idigi++){

      TObject *element = (wcsimrootevent_OD->GetCherenkovDigiHits())->At(idigi);
      if(!element) continue;
      
      WCSimRootCherenkovDigiHit *wcsimrootcherenkovdigihit = 
	dynamic_cast<WCSimRootCherenkovDigiHit*>(element);

      
      h_hits_time_OD->Fill(wcsimrootcherenkovdigihit->GetT());
      h_hits_charge_OD->Fill(wcsimrootcherenkovdigihit->GetQ());
      int tube_id = wcsimrootcherenkovdigihit->GetTubeId();
      h_hits_PMT_tube_id_OD->Fill(tube_id);
      //      WCSimRootPMT pmt = geo->GetPMT(tube_id - 1,true);
      WCSimRootPMT pmt = geo->GetPMT(geo->GetWCNumPMT() + tube_id - 1);
      float pmt_x = pmt.GetPosition(0);
      float pmt_y = pmt.GetPosition(1);
      float pmt_z = pmt.GetPosition(2);
      float pmt_phi = atan2(pmt_y, pmt_x);
      //      std::cout << "OD hit (" << pmt_x << ", " << pmt_y << ", " << pmt_z << ") id " << tube_id << " orien2 " << pmt.GetOrientation(2) << endl;
      if( fabs(pmt.GetOrientation(2)) < 0.01 )
	h_hits_map_OD->Fill(pmt_phi, pmt_z);
      else if( pmt.GetOrientation(2) > 0.01 )
	h_hits_map_top_OD->Fill(pmt_x, pmt_y);
      else
	h_hits_map_bottom_OD->Fill(pmt_x, pmt_y);
      //      std::cout << " hit " << idigi << " t " << wcsimrootcherenkovdigihit->GetT() << " tube " << tube_id << " x " << pmt_x << std::endl;
    }

    if( has_OD )
      wcsimrootsuperevent_OD->ReInitialize();
    
  } // ievent // End of loop over events

  fout->cd();
  h_nhits_OD->Write();
  h_hits_time_OD->Write();
  h_hits_PMT_tube_id_OD->Write();
  h_hits_charge_OD->Write();
  h_hits_map_OD->Write();
  h_hits_map_top_OD->Write();
  h_hits_map_bottom_OD->Write();
  h2_z_orientation->Write();
  OD_PMTS->Write();
  fout->Close();

  return 0;
}
