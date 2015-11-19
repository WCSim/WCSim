#include <iostream>
#include <TH1F.h>
#include <TH2F.h>
#include <stdio.h>     
#include <stdlib.h>    
#include <vector>
#include <TFile.h>
#include <TTree.h>

int main(){

  // open input file
  TFile *f = new TFile("output.root","READ");
  
  // create output file
  TFile *of = new TFile("all_plots.root", "RECREATE");

  // geometry tree
  TTree * geom_tree = (TTree*)f->Get("geom_tree");
  Float_t detector_length, detector_radius, pmt_radius;
  Int_t number_of_pmts;
  geom_tree->SetBranchAddress("detector_length",&detector_length);
  geom_tree->SetBranchAddress("detector_radius",&detector_radius);
  geom_tree->SetBranchAddress("pmt_radius",&pmt_radius);
  geom_tree->SetBranchAddress("number_of_pmts",&number_of_pmts);
  geom_tree->GetEntry(0);
  std::clog << " detector_length " << detector_length << " detector_radius " << detector_radius << " pmt_radius " << pmt_radius << " number_of_pmts " << number_of_pmts << std::endl;

  // all pmts tree
  TTree * all_pmts_tree = (TTree*)f->Get("all_pmts_tree");
  Int_t pmt_number, pmt_location;
  Float_t pmt_ux, pmt_uy, pmt_uz, pmt_x, pmt_y, pmt_z;
  all_pmts_tree->SetBranchAddress("pmt_number",&pmt_number);
  all_pmts_tree->SetBranchAddress("pmt_location",&pmt_location);
  all_pmts_tree->SetBranchAddress("pmt_ux",&pmt_ux);
  all_pmts_tree->SetBranchAddress("pmt_uy",&pmt_uy);
  all_pmts_tree->SetBranchAddress("pmt_uz",&pmt_uz);
  all_pmts_tree->SetBranchAddress("pmt_x",&pmt_x);
  all_pmts_tree->SetBranchAddress("pmt_y",&pmt_y);
  all_pmts_tree->SetBranchAddress("pmt_z",&pmt_z);
  for(int ipmt = 0; ipmt < all_pmts_tree->GetEntries(); ipmt++){
    all_pmts_tree->GetEntry(ipmt);
    // std::clog << " pmt_number " << pmt_number << " pmt_location " << pmt_location << " pmt dir: (" << pmt_ux << ", " << pmt_uy << ", " << pmt_uz << "), pos: (" << pmt_x << ", " << pmt_y << ", " << pmt_z << ")" << std::endl;
  }

  // primary events tree
  TTree * primary_events_tree = (TTree*)f->Get("primary_events_tree");

  std::vector<Int_t> *trigger_number = 0; std::vector<Int_t> * trigger_date = 0; std::vector<Int_t> * trigger_mode = 0; std::vector<Int_t> * trigger_vec_rec_number = 0; std::vector<Int_t> * trigger_jmu = 0; std::vector<Int_t> * trigger_jp = 0; std::vector<Int_t> * trigger_npar = 0; std::vector<Int_t> * trigger_ntrack = 0; std::vector<Int_t> * trigger_number_raw_hits = 0; std::vector<Int_t> * trigger_number_digitized_hits = 0; std::vector<Int_t> * trigger_number_times = 0; std::vector<Int_t> * trigger_nvertex = 0;
  std::vector<std::vector<Int_t> > * trigger_vtxvol = 0; 

  std::vector<Float_t> * trigger_sum_q = 0;
  std::vector<std::vector<Float_t> > * trigger_vtx_x = 0; std::vector<std::vector<Float_t> > * trigger_vtx_y = 0; std::vector<std::vector<Float_t> > * trigger_vtx_z = 0;

  std::vector<std::vector<Int_t> > * track_ipnu = 0; std::vector<std::vector<Int_t> > *   track_parent_type = 0; std::vector<std::vector<Int_t> > *   track_flag = 0; std::vector<std::vector<Int_t> > *   track_start_volume = 0; std::vector<std::vector<Int_t> > *   track_stop_volume = 0; std::vector<std::vector<Int_t> > *   track_id = 0;

  std::vector<std::vector<Float_t> > * track_ux = 0; std::vector<std::vector<Float_t> > *   track_uy = 0; std::vector<std::vector<Float_t> > *   track_uz = 0; std::vector<std::vector<Float_t> > *   track_M = 0; std::vector<std::vector<Float_t> > *   track_P = 0; std::vector<std::vector<Float_t> > *   track_E = 0; std::vector<std::vector<Float_t> > *   track_px = 0; std::vector<std::vector<Float_t> > *   track_py = 0; std::vector<std::vector<Float_t> > *   track_pz = 0; std::vector<std::vector<Float_t> > *   track_stop_x = 0; std::vector<std::vector<Float_t> > *   track_stop_y = 0; std::vector<std::vector<Float_t> > *   track_stop_z = 0; std::vector<std::vector<Float_t> > *   track_start_x = 0; std::vector<std::vector<Float_t> > *   track_start_y = 0; std::vector<std::vector<Float_t> > *   track_start_z = 0; std::vector<std::vector<Float_t> > *   track_time = 0;

  std::vector<std::vector<Int_t> > * raw_hit_tube_id = 0; std::vector<std::vector<Int_t> > * raw_hit_tube_times_indexes = 0; std::vector<std::vector<Int_t> > * raw_hit_tube_pe = 0;
  std::vector<std::vector<std::vector<Float_t> > > * raw_hit_times = 0;
  std::vector<std::vector<std::vector<Int_t> > > * raw_hit_parent_ids = 0;

  std::vector<std::vector<Int_t> > * digitized_hit_tube_id = 0;
  std::vector<std::vector<Float_t> > * digitized_hit_Q = 0; std::vector<std::vector<Float_t> > * digitized_hit_time = 0;
  std::vector<std::vector<std::vector<Int_t> > > * digitized_hit_photon_ids = 0;

  primary_events_tree->SetBranchAddress("trigger_number",&trigger_number);
  primary_events_tree->SetBranchAddress("trigger_date",&trigger_date);
  primary_events_tree->SetBranchAddress("trigger_mode",&trigger_mode);
  primary_events_tree->SetBranchAddress("trigger_vtxvol",&trigger_vtxvol);
  primary_events_tree->SetBranchAddress("trigger_vtx_x",&trigger_vtx_x); 
  primary_events_tree->SetBranchAddress("trigger_vtx_y",&trigger_vtx_y);
  primary_events_tree->SetBranchAddress("trigger_vtx_z",&trigger_vtx_z);
  primary_events_tree->SetBranchAddress("trigger_vec_rec_number",&trigger_vec_rec_number);
  primary_events_tree->SetBranchAddress("trigger_jmu",&trigger_jmu);
  primary_events_tree->SetBranchAddress("trigger_jp",&trigger_jp); 
  primary_events_tree->SetBranchAddress("trigger_npar",&trigger_npar);
  primary_events_tree->SetBranchAddress("trigger_ntrack",&trigger_ntrack);
  primary_events_tree->SetBranchAddress("trigger_nvertex",&trigger_nvertex);
  primary_events_tree->SetBranchAddress("trigger_number_raw_hits",&trigger_number_raw_hits);
  primary_events_tree->SetBranchAddress("trigger_number_digitized_hits",&trigger_number_digitized_hits);
  primary_events_tree->SetBranchAddress("trigger_sum_q",&trigger_sum_q);
  primary_events_tree->SetBranchAddress("trigger_number_times",&trigger_number_times);

  primary_events_tree->SetBranchAddress("track_ipnu",&track_ipnu); 
  primary_events_tree->SetBranchAddress("track_parent_type",&track_parent_type); 
  primary_events_tree->SetBranchAddress("track_ux",&track_ux); 
  primary_events_tree->SetBranchAddress("track_uy",&track_uy);
  primary_events_tree->SetBranchAddress("track_uz",&track_uz);
  primary_events_tree->SetBranchAddress("track_px",&track_px); 
  primary_events_tree->SetBranchAddress("track_py",&track_py);
  primary_events_tree->SetBranchAddress("track_pz",&track_pz);
  primary_events_tree->SetBranchAddress("track_flag",&track_flag); 
  primary_events_tree->SetBranchAddress("track_M",&track_M); 
  primary_events_tree->SetBranchAddress("track_P",&track_P); 
  primary_events_tree->SetBranchAddress("track_E",&track_E); 
  primary_events_tree->SetBranchAddress("track_start_volume",&track_start_volume); 
  primary_events_tree->SetBranchAddress("track_stop_volume",&track_stop_volume); 
  primary_events_tree->SetBranchAddress("track_stop_x",&track_stop_x); 
  primary_events_tree->SetBranchAddress("track_stop_y",&track_stop_y);
  primary_events_tree->SetBranchAddress("track_stop_z",&track_stop_z);
  primary_events_tree->SetBranchAddress("track_start_x",&track_start_x); 
  primary_events_tree->SetBranchAddress("track_start_y",&track_start_y);
  primary_events_tree->SetBranchAddress("track_start_z",&track_start_z);
  primary_events_tree->SetBranchAddress("track_time",&track_time); 
  primary_events_tree->SetBranchAddress("track_id",&track_id);

  primary_events_tree->SetBranchAddress("raw_hit_tube_id",&raw_hit_tube_id);
  primary_events_tree->SetBranchAddress("raw_hit_tube_times_indexes",&raw_hit_tube_times_indexes);
  primary_events_tree->SetBranchAddress("raw_hit_tube_pe",&raw_hit_tube_pe);
  primary_events_tree->SetBranchAddress("raw_hit_times",&raw_hit_times);
  primary_events_tree->SetBranchAddress("raw_hit_parent_ids",&raw_hit_parent_ids);

  primary_events_tree->SetBranchAddress("digitized_hit_tube_id",&digitized_hit_tube_id);
  primary_events_tree->SetBranchAddress("digitized_hit_Q",&digitized_hit_Q);
  primary_events_tree->SetBranchAddress("digitized_hit_time",&digitized_hit_time);
  primary_events_tree->SetBranchAddress("digitized_hit_photon_ids",&digitized_hit_photon_ids);


  double max_time = 2.e3;

  TH1F *h_trigger_number = new TH1F("h_trigger_number","h_trigger_number",100,1,-1);
  TH1F *h_trigger_date = new TH1F("h_trigger_date","h_trigger_date",100,1,-1);
  TH1F *h_trigger_mode = new TH1F("h_trigger_mode","h_trigger_mode",100,1,-1);
  TH1F *h_trigger_vtxvol = new TH1F("h_trigger_vtxvol","h_trigger_vtxvol",100,1,-1);
  TH1F *h_trigger_vtx_x = new TH1F("h_trigger_vtx_x","h_trigger_vtx_x",100,1,-1);
  TH1F *h_trigger_vtx_y = new TH1F("h_trigger_vtx_y","h_trigger_vtx_y",100,1,-1);
  TH1F *h_trigger_vtx_z = new TH1F("h_trigger_vtx_z","h_trigger_vtx_z",100,1,-1);
  TH2F *h_trigger_vtx_x_y = new TH2F("h_trigger_vtx_x_y","h_trigger_vtx_x_y",100,1,-1,100,1,-1);
  TH1F *h_trigger_vec_rec_number = new TH1F("h_trigger_vec_rec_number","h_trigger_vec_rec_number",100,1,-1);
  TH1F *h_trigger_jmu = new TH1F("h_trigger_jmu","h_trigger_jmu",100,1,-1);
  TH1F *h_trigger_jp = new TH1F("h_trigger_jp","h_trigger_jp",100,1,-1);
  TH1F *h_trigger_npar = new TH1F("h_trigger_npar","h_trigger_npar",100,1,-1);
  TH1F *h_trigger_ntrack = new TH1F("h_trigger_ntrack","h_trigger_ntrack",100,1,-1);
  TH1F *h_trigger_nvertex = new TH1F("h_trigger_nvertex","h_trigger_nvertex",100,1,-1);
  TH1F *h_trigger_number_raw_hits = new TH1F("h_trigger_number_raw_hits","h_trigger_number_raw_hits",100,1,-1);
  TH1F *h_trigger_number_raw_hits_in_time = new TH1F("h_trigger_number_raw_hits_in_time","h_trigger_number_raw_hits_in_time",100,1,-1);
  TH1F *h_trigger_number_digitized_hits = new TH1F("h_trigger_number_digitized_hits","h_trigger_number_digitized_hits",100,1,-1);
  TH1F *h_trigger_number_digitized_hits_in_time = new TH1F("h_trigger_number_digitized_hits_in_time","h_trigger_number_digitized_hits_in_time",100,1,-1);
  TH1F *h_trigger_sum_q = new TH1F("h_trigger_sum_q","h_trigger_sum_q",100,1,-1);
  TH1F *h_trigger_number_times = new TH1F("h_trigger_number_times","h_trigger_number_times",100,1,-1);

  int n_ipnu_bins = 2300;
  TH1F *h_track_ipnu = new TH1F("h_track_ipnu","h_track_ipnu",n_ipnu_bins,-0.5,n_ipnu_bins-0.5);
  TH1F *h_track_ipnu1 = new TH1F("h_track_ipnu1","h_track_ipnu1",n_ipnu_bins,-0.5,n_ipnu_bins-0.5);
  TH1F *h_track_ipnu2 = new TH1F("h_track_ipnu2","h_track_ipnu2",n_ipnu_bins,-0.5,n_ipnu_bins-0.5);
  TH1F *h_track_ipnu3 = new TH1F("h_track_ipnu3","h_track_ipnu3",n_ipnu_bins,-0.5,n_ipnu_bins-0.5);
  TH1F *h_track_ipnu4 = new TH1F("h_track_ipnu4","h_track_ipnu4",n_ipnu_bins,-0.5,n_ipnu_bins-0.5);
  TH1F *h_track_ipnu5 = new TH1F("h_track_ipnu5","h_track_ipnu5",n_ipnu_bins,-0.5,n_ipnu_bins-0.5);
  TH1F *h_track_ipnu6 = new TH1F("h_track_ipnu6","h_track_ipnu6",n_ipnu_bins,-0.5,n_ipnu_bins-0.5);
  TH1F *h_track_parent_type = new TH1F("h_track_parent_type","h_track_parent_type",100,1,-1);
  TH1F *h_track_ux = new TH1F("h_track_ux","h_track_ux",100,1,-1);
  TH1F *h_track_uy = new TH1F("h_track_uy","h_track_uy",100,1,-1);
  TH1F *h_track_uz = new TH1F("h_track_uz","h_track_uz",100,1,-1);
  TH1F *h_track_px = new TH1F("h_track_px","h_track_px",100,1,-1);
  TH1F *h_track_py = new TH1F("h_track_py","h_track_py",100,1,-1);
  TH1F *h_track_pz = new TH1F("h_track_pz","h_track_pz",100,1,-1);
  TH1F *h_track_flag = new TH1F("h_track_flag","h_track_flag",100,1,-1);
  TH1F *h_track_M = new TH1F("h_track_M","h_track_M",100,1,-1);
  TH1F *h_track_P = new TH1F("h_track_P","h_track_P",100,1,-1);
  TH1F *h_track_E = new TH1F("h_track_E","h_track_E",100,1,-1);
  TH1F *h_track_E1 = new TH1F("h_track_E1","h_track_E1",100,1,-1);
  TH1F *h_track_E2 = new TH1F("h_track_E2","h_track_E2",100,1,-1);
  TH1F *h_track_E3 = new TH1F("h_track_E3","h_track_E3",100,1,-1);
  TH1F *h_track_E4 = new TH1F("h_track_E4","h_track_E4",100,1,-1);
  TH1F *h_track_E5 = new TH1F("h_track_E5","h_track_E5",100,1,-1);
  TH1F *h_track_E6 = new TH1F("h_track_E6","h_track_E6",100,1,-1);
  TH1F *h_track_KE = new TH1F("h_track_KE","h_track_KE",100,1,-1);
  TH1F *h_track_KE1 = new TH1F("h_track_KE1","h_track_KE1",100,1,-1);
  TH1F *h_track_KE2 = new TH1F("h_track_KE2","h_track_KE2",100,1,-1);
  TH1F *h_track_KE3 = new TH1F("h_track_KE3","h_track_KE3",100,1,-1);
  TH1F *h_track_KE4 = new TH1F("h_track_KE4","h_track_KE4",100,1,-1);
  TH1F *h_track_KE5 = new TH1F("h_track_KE5","h_track_KE5",100,1,-1);
  TH1F *h_track_KE6 = new TH1F("h_track_KE6","h_track_KE6",100,1,-1);
  TH1F *h_track_start_volume = new TH1F("h_track_start_volume","h_track_start_volume",100,1,-1);
  TH1F *h_track_stop_volume = new TH1F("h_track_stop_volume","h_track_stop_volume",100,1,-1);
  TH1F *h_track_stop_x = new TH1F("h_track_stop_x","h_track_stop_x",100,1,-1);
  TH1F *h_track_stop_y = new TH1F("h_track_stop_y","h_track_stop_y",100,1,-1);
  TH1F *h_track_stop_z = new TH1F("h_track_stop_z","h_track_stop_z",100,1,-1);
  TH1F *h_track_start_x = new TH1F("h_track_start_x","h_track_start_x",100,1,-1);
  TH1F *h_track_start_y = new TH1F("h_track_start_y","h_track_start_y",100,1,-1);
  TH1F *h_track_start_z = new TH1F("h_track_start_z","h_track_start_z",100,1,-1);
  TH1F *h_track_time = new TH1F("h_track_time","h_track_time",100,1,-1);
  TH1F *h_track_id = new TH1F("h_track_id","h_track_id",100,1,-1);

  TH1F *h_raw_hit_tube_id = new TH1F("h_raw_hit_tube_id","h_raw_hit_tube_id",100,1,-1);
  TH1F *h_raw_hit_tube_times_indexes = new TH1F("h_raw_hit_tube_times_indexes","h_raw_hit_tube_times_indexes",100,1,-1);
  TH1F *h_raw_hit_tube_pe = new TH1F("h_raw_hit_tube_pe","h_raw_hit_tube_pe",100,1,-1);
  TH1F *h_raw_hit_times = new TH1F("h_raw_hit_times","h_raw_hit_times",100,1,-1);
  TH1F *h_raw_hit_parent_ids = new TH1F("h_raw_hit_parent_ids","h_raw_hit_parent_ids",100,1,-1);

  TH1F *h_digitized_hit_tube_id = new TH1F("h_digitized_hit_tube_id","h_digitized_hit_tube_id",100,1,-1);
  TH1F *h_digitized_hit_Q = new TH1F("h_digitized_hit_Q","h_digitized_hit_Q",100,1,-1);
  TH1F *h_digitized_hit_time = new TH1F("h_digitized_hit_time","h_digitized_hit_time",100,1,-1);
  TH1F *h_digitized_hit_photon_ids = new TH1F("h_digitized_hit_photon_ids","h_digitized_hit_photon_ids",100,1,-1);



  for(int ievent=0; ievent<primary_events_tree->GetEntries(); ievent++){
    // loop on primary events
    primary_events_tree->GetEvent(ievent); 

    for(size_t itrigger=0; itrigger<trigger_ntrack->size(); itrigger++){
      // loop on triggers in the event

      h_trigger_number->Fill(trigger_number->at(itrigger));
      h_trigger_date->Fill(trigger_date->at(itrigger));
      h_trigger_mode->Fill(trigger_mode->at(itrigger));

      h_trigger_nvertex->Fill(trigger_nvertex->at(itrigger));
      for(int ivertex=0; ivertex<trigger_nvertex->at(itrigger); ivertex++){
	// loop on triggers in the event
	h_trigger_vtxvol->Fill(trigger_vtxvol->at(itrigger).at(ivertex));
	h_trigger_vtx_x->Fill(trigger_vtx_x->at(itrigger).at(ivertex));
	h_trigger_vtx_y->Fill(trigger_vtx_y->at(itrigger).at(ivertex));
	h_trigger_vtx_z->Fill(trigger_vtx_z->at(itrigger).at(ivertex));
	h_trigger_vtx_x_y->Fill(trigger_vtx_x->at(itrigger).at(ivertex), trigger_vtx_y->at(itrigger).at(ivertex));
	std::cout << " ev " << ievent << " tr " << itrigger << " vtx( " << trigger_vtx_x->at(itrigger).at(ivertex) << ", " << trigger_vtx_y->at(itrigger).at(ivertex) << ", " << trigger_vtx_z->at(itrigger).at(ivertex) << ")" << std::endl;
      }

      h_trigger_vec_rec_number->Fill(trigger_vec_rec_number->at(itrigger));
      h_trigger_jmu->Fill(trigger_jmu->at(itrigger));
      h_trigger_jp->Fill(trigger_jp->at(itrigger));
      h_trigger_npar->Fill(trigger_npar->at(itrigger));
      h_trigger_ntrack->Fill(trigger_ntrack->at(itrigger));
      h_trigger_number_raw_hits->Fill(trigger_number_raw_hits->at(itrigger));
      h_trigger_number_digitized_hits->Fill(trigger_number_digitized_hits->at(itrigger));
      h_trigger_sum_q->Fill(trigger_sum_q->at(itrigger));
      h_trigger_number_times->Fill(trigger_number_times->at(itrigger));

      if( trigger_ntrack->at(itrigger) > 0 ){
       h_track_E1->Fill((track_E->at(itrigger)).at(0));
       h_track_KE1->Fill((track_E->at(itrigger)).at(0) - (track_M->at(itrigger)).at(0));
       h_track_ipnu1->Fill((track_ipnu->at(itrigger)).at(0));
      }
      if( trigger_ntrack->at(itrigger) > 1 ){
       h_track_E2->Fill((track_E->at(itrigger)).at(1));
       h_track_KE2->Fill((track_E->at(itrigger)).at(1) - (track_M->at(itrigger)).at(1));
       h_track_ipnu2->Fill((track_ipnu->at(itrigger)).at(1));
      }
      if( trigger_ntrack->at(itrigger) > 2 ){
       h_track_E3->Fill((track_E->at(itrigger)).at(2));
       h_track_KE3->Fill((track_E->at(itrigger)).at(2) - (track_M->at(itrigger)).at(2));
       h_track_ipnu3->Fill((track_ipnu->at(itrigger)).at(2));
      }
      if( trigger_ntrack->at(itrigger) > 3 ){
       h_track_E4->Fill((track_E->at(itrigger)).at(3));
       h_track_KE4->Fill((track_E->at(itrigger)).at(3) - (track_M->at(itrigger)).at(3));
       h_track_ipnu4->Fill((track_ipnu->at(itrigger)).at(3));
      }
      if( trigger_ntrack->at(itrigger) > 4 ){
       h_track_E5->Fill((track_E->at(itrigger)).at(4));
       h_track_KE5->Fill((track_E->at(itrigger)).at(4) - (track_M->at(itrigger)).at(4));
       h_track_ipnu5->Fill((track_ipnu->at(itrigger)).at(4));
      }
      if( trigger_ntrack->at(itrigger) > 5 ){
       h_track_E6->Fill((track_E->at(itrigger)).at(5));
       h_track_KE6->Fill((track_E->at(itrigger)).at(5) - (track_M->at(itrigger)).at(5));
       h_track_ipnu6->Fill((track_ipnu->at(itrigger)).at(5));
      }

      for(int itrack=0; itrack<trigger_ntrack->at(itrigger); itrack++){
	// loop on tracks in the trigger

	h_track_ipnu->Fill((track_ipnu->at(itrigger)).at(itrack));
	h_track_parent_type->Fill((track_parent_type->at(itrigger)).at(itrack));
	h_track_ux->Fill((track_ux->at(itrigger)).at(itrack));
	h_track_uy->Fill((track_uy->at(itrigger)).at(itrack));
	h_track_uz->Fill((track_uz->at(itrigger)).at(itrack));
	h_track_px->Fill((track_px->at(itrigger)).at(itrack));
	h_track_py->Fill((track_py->at(itrigger)).at(itrack));
	h_track_pz->Fill((track_pz->at(itrigger)).at(itrack));
	h_track_flag->Fill((track_flag->at(itrigger)).at(itrack));
	h_track_M->Fill((track_M->at(itrigger)).at(itrack));
	h_track_P->Fill((track_P->at(itrigger)).at(itrack));
	h_track_E->Fill((track_E->at(itrigger)).at(itrack));
	h_track_KE->Fill((track_E->at(itrigger)).at(itrack) - (track_M->at(itrigger)).at(itrack));
	h_track_start_volume->Fill((track_start_volume->at(itrigger)).at(itrack));
	h_track_stop_volume->Fill((track_stop_volume->at(itrigger)).at(itrack));
	h_track_stop_x->Fill((track_stop_x->at(itrigger)).at(itrack));
	h_track_stop_y->Fill((track_stop_y->at(itrigger)).at(itrack));
	h_track_stop_z->Fill((track_stop_z->at(itrigger)).at(itrack));
	h_track_start_x->Fill((track_start_x->at(itrigger)).at(itrack));
	h_track_start_y->Fill((track_start_y->at(itrigger)).at(itrack));
	h_track_start_z->Fill((track_start_z->at(itrigger)).at(itrack));
	h_track_time->Fill((track_time->at(itrigger)).at(itrack));
	h_track_id->Fill((track_id->at(itrigger)).at(itrack));

	//std::clog << " event " << ievent << " trigger " << itrigger << " track " << itrack << " ipnu " << (track_ipnu->at(itrigger)).at(itrack) << ", dir( " << (track_ux->at(itrigger)).at(itrack) << ", " << (track_uy->at(itrigger)).at(itrack) << ", " << (track_uz->at(itrigger)).at(itrack) << ") E " << (track_E->at(itrigger)).at(itrack) << std::endl;

      }

      size_t n_raw_hits_in_time = 0;
      for(int irawhit=0; irawhit<trigger_number_raw_hits->at(itrigger); irawhit++){
	// loop on raw hits in the trigger

	h_raw_hit_tube_id->Fill((raw_hit_tube_id->at(itrigger)).at(irawhit));
	h_raw_hit_tube_times_indexes->Fill((raw_hit_tube_times_indexes->at(itrigger)).at(irawhit));
	h_raw_hit_tube_pe->Fill((raw_hit_tube_pe->at(itrigger)).at(irawhit));

	//std::clog << " event " << ievent << " trigger " << itrigger << " rawhit " << irawhit << " of " << trigger_number_raw_hits->at(itrigger) << " tubeid s1 " << raw_hit_tube_id->size() << " s2 " << (raw_hit_tube_id->at(itrigger)).size() << " val " << (raw_hit_tube_id->at(itrigger)).at(irawhit) <<  " timeindexes " << (raw_hit_tube_times_indexes->at(itrigger)).at(irawhit) << " totalpe " << (raw_hit_tube_pe->at(itrigger)).at(irawhit) << std::endl;

	for(size_t irawhittime=0; irawhittime<((raw_hit_times->at(itrigger)).at(irawhit)).size(); irawhittime++){
	  // loop on times in raw hit in the trigger

	  if( irawhittime == 0 && ((raw_hit_times->at(itrigger)).at(irawhit)).at(irawhittime) < max_time )
	    n_raw_hits_in_time ++;

	  h_raw_hit_times->Fill(((raw_hit_times->at(itrigger)).at(irawhit)).at(irawhittime));
	  h_raw_hit_parent_ids->Fill(((raw_hit_parent_ids->at(itrigger)).at(irawhit)).at(irawhittime));

	  //std::clog << " event " << ievent << " trigger " << itrigger << " rawhit " << irawhit << " of " << trigger_number_raw_hits->at(itrigger) << " tubeid s1 " << raw_hit_tube_id->size() << " s2 " << (raw_hit_tube_id->at(itrigger)).size() << " val " << (raw_hit_tube_id->at(itrigger)).at(irawhit) <<  " timeindexes " << (raw_hit_tube_times_indexes->at(itrigger)).at(irawhit) << " totalpe " << (raw_hit_tube_pe->at(itrigger)).at(irawhit) << " time " << ((raw_hit_times->at(itrigger)).at(irawhit)).at(irawhittime) << " parentid " << ((raw_hit_parent_ids->at(itrigger)).at(irawhit)).at(irawhittime) << std::endl;

	}
      }
      h_trigger_number_raw_hits_in_time->Fill(n_raw_hits_in_time);
    
      size_t n_digitized_hits_in_time = 0;
      for(size_t idigitizedhit=0; idigitizedhit<(digitized_hit_tube_id->at(itrigger)).size(); idigitizedhit++){
	// loop on digitized hits in the trigger

	h_digitized_hit_tube_id->Fill((digitized_hit_tube_id->at(itrigger)).at(idigitizedhit));
	h_digitized_hit_Q->Fill((digitized_hit_Q->at(itrigger)).at(idigitizedhit));
	h_digitized_hit_time->Fill((digitized_hit_time->at(itrigger)).at(idigitizedhit));

	if( (digitized_hit_time->at(itrigger)).at(idigitizedhit) < max_time )
	  n_digitized_hits_in_time ++;

	//std::clog << " event " << ievent << " trigger " << itrigger << " digitizedhit " << idigitizedhit << " tubeid " << (digitized_hit_tube_id->at(itrigger)).at(idigitizedhit) << " Q " << (digitized_hit_Q->at(itrigger)).at(idigitizedhit) << " time " << (digitized_hit_time->at(itrigger)).at(idigitizedhit) << std::endl;

	for(size_t iphotonid=0; iphotonid<((digitized_hit_photon_ids->at(itrigger)).at(idigitizedhit)).size(); iphotonid++){
	  // loop on photon ids in digitized hit in the trigger
	  
	  h_digitized_hit_photon_ids->Fill(((digitized_hit_photon_ids->at(itrigger)).at(idigitizedhit)).at(iphotonid));
	}

      }
      h_trigger_number_digitized_hits_in_time->Fill(n_digitized_hits_in_time);

      //std::clog << " event " << ievent << " trigger " << itrigger << " rawhits " << trigger_number_raw_hits->at(itrigger) << " tubeid s1 " << raw_hit_tube_id->size() << " s2 " << (raw_hit_tube_id->at(itrigger)).size() << std::endl;

    }

  }

  of->cd();

  if( h_track_ipnu->GetNbinsX() > h_track_ipnu->FindBin(11)) h_track_ipnu->GetXaxis()->SetBinLabel(h_track_ipnu->FindBin(11),"11(e^{-})");
  if( h_track_ipnu->GetNbinsX() > h_track_ipnu->FindBin(12)) h_track_ipnu->GetXaxis()->SetBinLabel(h_track_ipnu->FindBin(12),"12(#nu_{e})");
  if( h_track_ipnu->GetNbinsX() > h_track_ipnu->FindBin(13)) h_track_ipnu->GetXaxis()->SetBinLabel(h_track_ipnu->FindBin(13),"13(#mu^{-})");
  if( h_track_ipnu->GetNbinsX() > h_track_ipnu->FindBin(14)) h_track_ipnu->GetXaxis()->SetBinLabel(h_track_ipnu->FindBin(14),"14(#nu_{#mu})");
  if( h_track_ipnu->GetNbinsX() > h_track_ipnu->FindBin(22)) h_track_ipnu->GetXaxis()->SetBinLabel(h_track_ipnu->FindBin(22),"22(#gamma)");
  if( h_track_ipnu->GetNbinsX() > h_track_ipnu->FindBin(111)) h_track_ipnu->GetXaxis()->SetBinLabel(h_track_ipnu->FindBin(111),"111(#pi^{0})");
  if( h_track_ipnu->GetNbinsX() > h_track_ipnu->FindBin(211)) h_track_ipnu->GetXaxis()->SetBinLabel(h_track_ipnu->FindBin(211),"211(#pi^{+})");
  if( h_track_ipnu->GetNbinsX() > h_track_ipnu->FindBin(2212)) h_track_ipnu->GetXaxis()->SetBinLabel(h_track_ipnu->FindBin(2212),"2212(p^{+})");
  if( h_track_ipnu->GetNbinsX() > h_track_ipnu->FindBin(2112)) h_track_ipnu->GetXaxis()->SetBinLabel(h_track_ipnu->FindBin(2112),"2112(n)");
 
  if( h_track_ipnu1->GetNbinsX() > h_track_ipnu1->FindBin(11)) h_track_ipnu1->GetXaxis()->SetBinLabel(h_track_ipnu1->FindBin(11),"11(e^{-})");
  if( h_track_ipnu1->GetNbinsX() > h_track_ipnu1->FindBin(12)) h_track_ipnu1->GetXaxis()->SetBinLabel(h_track_ipnu1->FindBin(12),"12(#nu_{e})");
  if( h_track_ipnu1->GetNbinsX() > h_track_ipnu1->FindBin(13)) h_track_ipnu1->GetXaxis()->SetBinLabel(h_track_ipnu1->FindBin(13),"13(#mu^{-})");
  if( h_track_ipnu1->GetNbinsX() > h_track_ipnu1->FindBin(14)) h_track_ipnu1->GetXaxis()->SetBinLabel(h_track_ipnu1->FindBin(14),"14(#nu_{#mu})");
  if( h_track_ipnu1->GetNbinsX() > h_track_ipnu1->FindBin(22)) h_track_ipnu1->GetXaxis()->SetBinLabel(h_track_ipnu1->FindBin(22),"22(#gamma)");
  if( h_track_ipnu1->GetNbinsX() > h_track_ipnu1->FindBin(111)) h_track_ipnu1->GetXaxis()->SetBinLabel(h_track_ipnu1->FindBin(111),"111(#pi^{0})");
  if( h_track_ipnu1->GetNbinsX() > h_track_ipnu1->FindBin(211)) h_track_ipnu1->GetXaxis()->SetBinLabel(h_track_ipnu1->FindBin(211),"211(#pi^{+})");
  if( h_track_ipnu1->GetNbinsX() > h_track_ipnu1->FindBin(2212)) h_track_ipnu1->GetXaxis()->SetBinLabel(h_track_ipnu1->FindBin(2212),"2212(p^{+})");
  if( h_track_ipnu1->GetNbinsX() > h_track_ipnu1->FindBin(2112)) h_track_ipnu1->GetXaxis()->SetBinLabel(h_track_ipnu1->FindBin(2112),"2112(n)");

  if( h_track_ipnu2->GetNbinsX() > h_track_ipnu2->FindBin(11)) h_track_ipnu2->GetXaxis()->SetBinLabel(h_track_ipnu2->FindBin(11),"11(e^{-})");
  if( h_track_ipnu2->GetNbinsX() > h_track_ipnu2->FindBin(12)) h_track_ipnu2->GetXaxis()->SetBinLabel(h_track_ipnu2->FindBin(12),"12(#nu_{e})");
  if( h_track_ipnu2->GetNbinsX() > h_track_ipnu2->FindBin(13)) h_track_ipnu2->GetXaxis()->SetBinLabel(h_track_ipnu2->FindBin(13),"13(#mu^{-})");
  if( h_track_ipnu2->GetNbinsX() > h_track_ipnu2->FindBin(14)) h_track_ipnu2->GetXaxis()->SetBinLabel(h_track_ipnu2->FindBin(14),"14(#nu_{#mu})");
  if( h_track_ipnu2->GetNbinsX() > h_track_ipnu2->FindBin(22)) h_track_ipnu2->GetXaxis()->SetBinLabel(h_track_ipnu2->FindBin(22),"22(#gamma)");
  if( h_track_ipnu2->GetNbinsX() > h_track_ipnu2->FindBin(111)) h_track_ipnu2->GetXaxis()->SetBinLabel(h_track_ipnu2->FindBin(111),"111(#pi^{0})");
  if( h_track_ipnu2->GetNbinsX() > h_track_ipnu2->FindBin(211)) h_track_ipnu2->GetXaxis()->SetBinLabel(h_track_ipnu2->FindBin(211),"211(#pi^{+})");
  if( h_track_ipnu2->GetNbinsX() > h_track_ipnu2->FindBin(2212)) h_track_ipnu2->GetXaxis()->SetBinLabel(h_track_ipnu2->FindBin(2212),"2212(p^{+})");
  if( h_track_ipnu2->GetNbinsX() > h_track_ipnu2->FindBin(2112)) h_track_ipnu2->GetXaxis()->SetBinLabel(h_track_ipnu2->FindBin(2112),"2112(n)");

  if( h_track_ipnu3->GetNbinsX() > h_track_ipnu3->FindBin(11)) h_track_ipnu3->GetXaxis()->SetBinLabel(h_track_ipnu3->FindBin(11),"11(e^{-})");
  if( h_track_ipnu3->GetNbinsX() > h_track_ipnu3->FindBin(12)) h_track_ipnu3->GetXaxis()->SetBinLabel(h_track_ipnu3->FindBin(12),"12(#nu_{e})");
  if( h_track_ipnu3->GetNbinsX() > h_track_ipnu3->FindBin(13)) h_track_ipnu3->GetXaxis()->SetBinLabel(h_track_ipnu3->FindBin(13),"13(#mu^{-})");
  if( h_track_ipnu3->GetNbinsX() > h_track_ipnu3->FindBin(14)) h_track_ipnu3->GetXaxis()->SetBinLabel(h_track_ipnu3->FindBin(14),"14(#nu_{#mu})");
  if( h_track_ipnu3->GetNbinsX() > h_track_ipnu3->FindBin(22)) h_track_ipnu3->GetXaxis()->SetBinLabel(h_track_ipnu3->FindBin(22),"22(#gamma)");
  if( h_track_ipnu3->GetNbinsX() > h_track_ipnu3->FindBin(111)) h_track_ipnu3->GetXaxis()->SetBinLabel(h_track_ipnu3->FindBin(111),"111(#pi^{0})");
  if( h_track_ipnu3->GetNbinsX() > h_track_ipnu3->FindBin(211)) h_track_ipnu3->GetXaxis()->SetBinLabel(h_track_ipnu3->FindBin(211),"211(#pi^{+})");
  if( h_track_ipnu3->GetNbinsX() > h_track_ipnu3->FindBin(2212)) h_track_ipnu3->GetXaxis()->SetBinLabel(h_track_ipnu3->FindBin(2212),"2212(p^{+})");
  if( h_track_ipnu3->GetNbinsX() > h_track_ipnu3->FindBin(2112)) h_track_ipnu3->GetXaxis()->SetBinLabel(h_track_ipnu3->FindBin(2112),"2112(n)");

  if( h_track_ipnu4->GetNbinsX() > h_track_ipnu4->FindBin(11)) h_track_ipnu4->GetXaxis()->SetBinLabel(h_track_ipnu4->FindBin(11),"11(e^{-})");
  if( h_track_ipnu4->GetNbinsX() > h_track_ipnu4->FindBin(12)) h_track_ipnu4->GetXaxis()->SetBinLabel(h_track_ipnu4->FindBin(12),"12(#nu_{e})");
  if( h_track_ipnu4->GetNbinsX() > h_track_ipnu4->FindBin(13)) h_track_ipnu4->GetXaxis()->SetBinLabel(h_track_ipnu4->FindBin(13),"13(#mu^{-})");
  if( h_track_ipnu4->GetNbinsX() > h_track_ipnu4->FindBin(14)) h_track_ipnu4->GetXaxis()->SetBinLabel(h_track_ipnu4->FindBin(14),"14(#nu_{#mu})");
  if( h_track_ipnu4->GetNbinsX() > h_track_ipnu4->FindBin(22)) h_track_ipnu4->GetXaxis()->SetBinLabel(h_track_ipnu4->FindBin(22),"22(#gamma)");
  if( h_track_ipnu4->GetNbinsX() > h_track_ipnu4->FindBin(111)) h_track_ipnu4->GetXaxis()->SetBinLabel(h_track_ipnu4->FindBin(111),"111(#pi^{0})");
  if( h_track_ipnu4->GetNbinsX() > h_track_ipnu4->FindBin(211)) h_track_ipnu4->GetXaxis()->SetBinLabel(h_track_ipnu4->FindBin(211),"211(#pi^{+})");
  if( h_track_ipnu4->GetNbinsX() > h_track_ipnu4->FindBin(2212)) h_track_ipnu4->GetXaxis()->SetBinLabel(h_track_ipnu4->FindBin(2212),"2212(p^{+})");
  if( h_track_ipnu4->GetNbinsX() > h_track_ipnu4->FindBin(2112)) h_track_ipnu4->GetXaxis()->SetBinLabel(h_track_ipnu4->FindBin(2112),"2112(n)");

  if( h_track_ipnu5->GetNbinsX() > h_track_ipnu5->FindBin(11)) h_track_ipnu5->GetXaxis()->SetBinLabel(h_track_ipnu5->FindBin(11),"11(e^{-})");
  if( h_track_ipnu5->GetNbinsX() > h_track_ipnu5->FindBin(12)) h_track_ipnu5->GetXaxis()->SetBinLabel(h_track_ipnu5->FindBin(12),"12(#nu_{e})");
  if( h_track_ipnu5->GetNbinsX() > h_track_ipnu5->FindBin(13)) h_track_ipnu5->GetXaxis()->SetBinLabel(h_track_ipnu5->FindBin(13),"13(#mu^{-})");
  if( h_track_ipnu5->GetNbinsX() > h_track_ipnu5->FindBin(14)) h_track_ipnu5->GetXaxis()->SetBinLabel(h_track_ipnu5->FindBin(14),"14(#nu_{#mu})");
  if( h_track_ipnu5->GetNbinsX() > h_track_ipnu5->FindBin(22)) h_track_ipnu5->GetXaxis()->SetBinLabel(h_track_ipnu5->FindBin(22),"22(#gamma)");
  if( h_track_ipnu5->GetNbinsX() > h_track_ipnu5->FindBin(111)) h_track_ipnu5->GetXaxis()->SetBinLabel(h_track_ipnu5->FindBin(111),"111(#pi^{0})");
  if( h_track_ipnu5->GetNbinsX() > h_track_ipnu5->FindBin(211)) h_track_ipnu5->GetXaxis()->SetBinLabel(h_track_ipnu5->FindBin(211),"211(#pi^{+})");
  if( h_track_ipnu5->GetNbinsX() > h_track_ipnu5->FindBin(2212)) h_track_ipnu5->GetXaxis()->SetBinLabel(h_track_ipnu5->FindBin(2212),"2212(p^{+})");
  if( h_track_ipnu5->GetNbinsX() > h_track_ipnu5->FindBin(2112)) h_track_ipnu5->GetXaxis()->SetBinLabel(h_track_ipnu5->FindBin(2112),"2112(n)");

  if( h_track_ipnu6->GetNbinsX() > h_track_ipnu6->FindBin(11)) h_track_ipnu6->GetXaxis()->SetBinLabel(h_track_ipnu6->FindBin(11),"11(e^{-})");
  if( h_track_ipnu6->GetNbinsX() > h_track_ipnu6->FindBin(12)) h_track_ipnu6->GetXaxis()->SetBinLabel(h_track_ipnu6->FindBin(12),"12(#nu_{e})");
  if( h_track_ipnu6->GetNbinsX() > h_track_ipnu6->FindBin(13)) h_track_ipnu6->GetXaxis()->SetBinLabel(h_track_ipnu6->FindBin(13),"13(#mu^{-})");
  if( h_track_ipnu6->GetNbinsX() > h_track_ipnu6->FindBin(14)) h_track_ipnu6->GetXaxis()->SetBinLabel(h_track_ipnu6->FindBin(14),"14(#nu_{#mu})");
  if( h_track_ipnu6->GetNbinsX() > h_track_ipnu6->FindBin(22)) h_track_ipnu6->GetXaxis()->SetBinLabel(h_track_ipnu6->FindBin(22),"22(#gamma)");
  if( h_track_ipnu6->GetNbinsX() > h_track_ipnu6->FindBin(111)) h_track_ipnu6->GetXaxis()->SetBinLabel(h_track_ipnu6->FindBin(111),"111(#pi^{0})");
  if( h_track_ipnu6->GetNbinsX() > h_track_ipnu6->FindBin(211)) h_track_ipnu6->GetXaxis()->SetBinLabel(h_track_ipnu6->FindBin(211),"211(#pi^{+})");
  if( h_track_ipnu6->GetNbinsX() > h_track_ipnu6->FindBin(2212)) h_track_ipnu6->GetXaxis()->SetBinLabel(h_track_ipnu6->FindBin(2212),"2212(p^{+})");
  if( h_track_ipnu6->GetNbinsX() > h_track_ipnu6->FindBin(2112)) h_track_ipnu6->GetXaxis()->SetBinLabel(h_track_ipnu6->FindBin(2112),"2112(n)");



  of->Write();

  delete trigger_number ; delete  trigger_date ; delete  trigger_mode ; delete  trigger_vtxvol ; delete  trigger_vec_rec_number ; delete  trigger_jmu ; delete  trigger_jp ; delete  trigger_npar ; delete  trigger_ntrack ; delete  trigger_number_raw_hits ; delete  trigger_number_digitized_hits ; delete  trigger_number_times ;

  delete  trigger_vtx_x ; delete  trigger_vtx_y ; delete  trigger_vtx_z ; delete  trigger_sum_q ;

  delete  track_ipnu ; delete    track_parent_type ; delete    track_flag ; delete    track_start_volume ; delete    track_stop_volume ; delete    track_id ;

  delete  track_ux ; delete    track_uy ; delete    track_uz ; delete    track_M ; delete    track_P ; delete    track_E ; delete    track_px ; delete    track_py ; delete    track_pz ; delete    track_stop_x ; delete    track_stop_y ; delete    track_stop_z ; delete    track_start_x ; delete    track_start_y ; delete    track_start_z ; delete    track_time ;

  delete raw_hit_tube_id; delete raw_hit_tube_times_indexes; delete raw_hit_tube_pe; delete raw_hit_times; delete raw_hit_parent_ids;

  delete digitized_hit_tube_id;  delete digitized_hit_Q;  delete digitized_hit_time;   delete digitized_hit_photon_ids;

  return 1;

}


