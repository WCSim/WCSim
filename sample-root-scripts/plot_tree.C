#include <iostream>
#include <TH1F.h>
#include <stdio.h>     
#include <stdlib.h>    
#include <vector>
#include <TPaletteAxis.h>
#include <cfloat>
#include <limits>
#include <algorithm>
#include <math.h>       /* log10 */
#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TPad.h>
#include <TCanvas.h>

using std::cout;
using std::endl;

std::string number_with_digits(int i, int n);

int n_digits = 5;
double time_bin_size = 10.; // ns
bool make_animation = true;
double _time_min=150.;
double _time_max=750.;
bool use_time_range = false;//true;

bool plot_from_txt = false;

#ifdef __MAKECINT__
#pragma link C++ class std::vector<std::vector<Int_t> >+;
#pragma link C++ class std::vector<std::vector<Float_t> >+;
#pragma link C++ class std::vector<std::vector<std::vector<Int_t> > >+;
#pragma link C++ class std::vector<std::vector<std::vector<Float_t> > >+;
#endif

#define ROOTMODE
#ifdef ROOTMODE
int plot_tree(int ievent_to_use = 0, int itriggertype = -10, int itrigger = 0){
  
#else
int main() {
  int ievent_to_use   = 10; 
  int itrigger = 0;
#endif

  // open input and output files
#if !plot_from_txt
  TFile *f = new TFile("output.root","READ");
  TFile *of = new TFile("plot.root","RECREATE");
#else
  TFile *f = new TFile("output_from_txt.root","READ");
  TFile *of = new TFile("plot_from_txt.root","RECREATE");
#endif

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

  double r_limit=detector_radius;
  double z_limit=detector_length/2.;

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

  // primary events tree
  TTree * primary_events_tree = (TTree*)f->Get("primary_events_tree");

  std::vector<Int_t> *event_number = 0;
  std::vector<Int_t> *trigger_number = 0;
  std::vector<Int_t> * trigger_date = 0; std::vector<Int_t> * trigger_mode = 0; std::vector<Int_t> * trigger_vtxvol = 0; std::vector<Int_t> * trigger_vec_rec_number = 0; std::vector<Int_t> * trigger_jmu = 0; std::vector<Int_t> * trigger_jp = 0; std::vector<Int_t> * trigger_npar = 0; std::vector<Int_t> * trigger_ntrack = 0; std::vector<Int_t> * trigger_number_raw_hits = 0; std::vector<Int_t> * trigger_number_digitized_hits = 0; std::vector<Int_t> * trigger_number_times = 0; std::vector<Int_t> * trigger_type = 0;
  std::vector<Float_t> * trigger_vtx_x = 0; std::vector<Float_t> * trigger_vtx_y = 0; std::vector<Float_t> * trigger_vtx_z = 0; std::vector<Float_t> * trigger_sum_q = 0;
  std::vector<std::vector<Float_t> > * trigger_info = 0;

  std::vector<std::vector<Int_t> > * track_ipnu = 0; std::vector<std::vector<Int_t> > *   track_parent_type = 0; std::vector<std::vector<Int_t> > *   track_flag = 0; std::vector<std::vector<Int_t> > *   track_start_volume = 0; std::vector<std::vector<Int_t> > *   track_stop_volume = 0; std::vector<std::vector<Int_t> > *   track_id = 0;

  std::vector<std::vector<Float_t> > * track_ux = 0; std::vector<std::vector<Float_t> > *   track_uy = 0; std::vector<std::vector<Float_t> > *   track_uz = 0; std::vector<std::vector<Float_t> > *   track_M = 0; std::vector<std::vector<Float_t> > *   track_P = 0; std::vector<std::vector<Float_t> > *   track_E = 0; std::vector<std::vector<Float_t> > *   track_px = 0; std::vector<std::vector<Float_t> > *   track_py = 0; std::vector<std::vector<Float_t> > *   track_pz = 0; std::vector<std::vector<Float_t> > *   track_stop_x = 0; std::vector<std::vector<Float_t> > *   track_stop_y = 0; std::vector<std::vector<Float_t> > *   track_stop_z = 0; std::vector<std::vector<Float_t> > *   track_start_x = 0; std::vector<std::vector<Float_t> > *   track_start_y = 0; std::vector<std::vector<Float_t> > *   track_start_z = 0; std::vector<std::vector<Float_t> > *   track_time = 0;

  std::vector<std::vector<Int_t> > * raw_hit_tube_id = 0; std::vector<std::vector<Int_t> > * raw_hit_tube_times_indexes = 0; std::vector<std::vector<Int_t> > * raw_hit_tube_pe = 0;
  std::vector<std::vector<std::vector<Float_t> > > * raw_hit_times = 0;
  std::vector<std::vector<std::vector<Int_t> > > * raw_hit_parent_ids = 0;

  std::vector<std::vector<Int_t> > * digitized_hit_tube_id = 0;
  std::vector<std::vector<Float_t> > * digitized_hit_Q = 0; std::vector<std::vector<Float_t> > * digitized_hit_time = 0;
  std::vector<std::vector<std::vector<Int_t> > > * digitized_hit_photon_ids = 0;

  primary_events_tree->SetBranchAddress("event_number",&event_number);
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
  primary_events_tree->SetBranchAddress("trigger_number_raw_hits",&trigger_number_raw_hits);
  primary_events_tree->SetBranchAddress("trigger_number_digitized_hits",&trigger_number_digitized_hits);
  primary_events_tree->SetBranchAddress("trigger_sum_q",&trigger_sum_q);
  primary_events_tree->SetBranchAddress("trigger_number_times",&trigger_number_times);
  primary_events_tree->SetBranchAddress("trigger_type",&trigger_type);
  primary_events_tree->SetBranchAddress("trigger_info",&trigger_info);

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


  // define space binning so that different pmts are sure to end up in different bins
  int nbins_x = (int)(3.*detector_radius/pmt_radius);
  int nbins_y = nbins_x;
  int nbins_z = (int)(3.*detector_length/(2.*pmt_radius));
  int nbins_phi  = (int)(3.*acos(-1.)*detector_radius/pmt_radius);

  for(int ievent = 0; ievent < primary_events_tree->GetEntries(); ievent++) {
    cout << "start of ievent loop " << ievent << endl;
    if(ievent_to_use >= 0 && ievent != ievent_to_use)
      continue;

    cout << "Getting event " << ievent << endl;

    // get primary event
    primary_events_tree->GetEvent(ievent);

    cout << trigger_number_raw_hits->size() << " triggers" << endl;
    if(trigger_number_raw_hits->size() < (unsigned int)itrigger)
      continue;

    int ndigihits = trigger_number_digitized_hits->at(itrigger);
    int this_triggertype = trigger_type->at(itrigger);
    cout << ndigihits << " digitised hits for trigger type " << this_triggertype << endl;
    if(ndigihits < 1E-6) {
      if(ievent_to_use >= 0 && ievent == ievent_to_use)
	break;
      else
	continue;
    }
    if(itriggertype >= -1 && this_triggertype != itriggertype)
      continue;


    // get number of hits for trigger in the event
    int number_of_raw_cherenkov_hits = trigger_number_raw_hits->at(itrigger);
    int number_of_digitized_cherenkov_hits = (digitized_hit_tube_id->at(itrigger)).size();


    // get time binning
    double time_min = FLT_MAX;
    double time_max = -FLT_MAX;
    for(int idigitizedhit=0; idigitizedhit<number_of_digitized_cherenkov_hits; idigitizedhit++){

      double local_time = (digitized_hit_time->at(itrigger)).at(idigitizedhit);
      if( local_time < time_min ) time_min = local_time;
      if( local_time > time_max ) time_max = local_time;
    }
    int nbins_time = (int)(((int)(time_max+1.) - (int)time_min)/time_bin_size) + 1;
    std::clog << " time min = " << time_min << ", time max = " << time_max << " nbins time = " << nbins_time << std::endl;

    TH2F h_y0_vs_x0_top("h_y0_vs_x0_top","y0_vs_x0_top;x0 (mm);y0 (mm)",nbins_x,-(r_limit*1.1),(r_limit*1.1),nbins_y,-(r_limit*1.1),(r_limit*1.1));
    h_y0_vs_x0_top.SetFillColor(kBlack);
    h_y0_vs_x0_top.SetMarkerColor(kBlack);
    h_y0_vs_x0_top.SetMarkerStyle(7);
    TH2F h_y0_vs_x0_bottom("h_y0_vs_x0_bottom","y0_vs_x0_bottom;x0 (mm);y0 (mm)",nbins_x,-(r_limit*1.1),(r_limit*1.1),nbins_y,-(r_limit*1.1),(r_limit*1.1));
    h_y0_vs_x0_bottom.SetFillColor(kBlack);
    h_y0_vs_x0_bottom.SetMarkerColor(kBlack);
    h_y0_vs_x0_bottom.SetMarkerStyle(7);
    TH2F h_z0_vs_phi("h_z0_vs_phi","z0_vs_phi;phi (degrees);z0 (mm)",nbins_phi,-180., 180.,nbins_z,-(z_limit*1.1),(z_limit*1.1));
    h_z0_vs_phi.SetFillColor(kBlack);
    h_z0_vs_phi.SetMarkerColor(kBlack);
    h_z0_vs_phi.SetMarkerStyle(7);
    TH1F h_n_pmts_vs_time("h_n_pmts_vs_time","n_pmts_vs_time;time (ns);number of pmts",nbins_time,time_min, time_max);
    h_n_pmts_vs_time.SetLineColor(kBlack);
    h_n_pmts_vs_time.SetFillColor(kBlack);
    TH2F h_z0_vs_phi_background("h_z0_vs_phi_background","z0_vs_phi;phi (degrees);z0 (mm)",nbins_phi,-180., 180.,nbins_z,-(z_limit*1.1),(z_limit*1.1));
    h_z0_vs_phi_background.SetFillColor(kBlack);
    h_z0_vs_phi_background.SetMarkerColor(kBlack);
    h_z0_vs_phi_background.SetMarkerStyle(7);
    TH2F h_z0_vs_phi_signal("h_z0_vs_phi_signal","z0_vs_phi;phi (degrees);z0 (mm)",nbins_phi,-180., 180.,nbins_z,-(z_limit*1.1),(z_limit*1.1));
    h_z0_vs_phi_signal.SetFillColor(kRed);
    h_z0_vs_phi_signal.SetMarkerColor(kRed);
    h_z0_vs_phi_signal.SetMarkerStyle(7);

    TH1F h_npe("h_npe","npe; n of photo-electrons / PMT;",20,-0.5,19.5);
    h_npe.SetLineColor(kBlack);
    h_npe.SetFillColor(kBlack);

    
    std::vector<TH2F> h_y0_vs_x0_top_time, h_y0_vs_x0_bottom_time, h_z0_vs_phi_time;
    for(int i=0; i<nbins_time; i++){
      h_y0_vs_x0_top_time.push_back(*(new TH2F(Form("h_y0_vs_x0_top_%d",i),"y0_vs_x0_top;x0 (mm);y0 (mm)",nbins_x,-(r_limit*1.1),(r_limit*1.1),nbins_y,-(r_limit*1.1),(r_limit*1.1))));
      h_y0_vs_x0_top_time[i].SetFillColor(kBlack);
      h_y0_vs_x0_top_time[i].SetMarkerColor(kBlack);
      h_y0_vs_x0_top_time[i].SetMarkerStyle(7);
      h_y0_vs_x0_bottom_time.push_back(*(new TH2F(Form("h_y0_vs_x0_bottom_%d",i),"y0_vs_x0_bottom;x0 (mm);y0 (mm)",nbins_x,-(r_limit*1.1),(r_limit*1.1),nbins_y,-(r_limit*1.1),(r_limit*1.1))));
      h_y0_vs_x0_bottom_time[i].SetFillColor(kBlack);
      h_y0_vs_x0_bottom_time[i].SetMarkerColor(kBlack);
      h_y0_vs_x0_bottom_time[i].SetMarkerStyle(7);
      h_z0_vs_phi_time.push_back(*(new TH2F(Form("h_z0_vs_phi_time_%d",i),"z0_vs_phi;phi (degrees);z0 (mm)",nbins_phi,-180., 180.,nbins_z,-(z_limit*1.1),(z_limit*1.1))));
      h_z0_vs_phi_time[i].SetFillColor(kBlack);
      h_z0_vs_phi_time[i].SetMarkerColor(kBlack);
      h_z0_vs_phi_time[i].SetMarkerStyle(7);
    }

    std::clog << " number_of_raw_cherenkov_hits " << number_of_raw_cherenkov_hits << " number_of_digitized_cherenkov_hits " << number_of_digitized_cherenkov_hits  << std::endl;

    for(int idigitizedhit=0; idigitizedhit<number_of_digitized_cherenkov_hits; idigitizedhit++){
      // loop on digitized hits in the trigger
    
      // get pmt of digitized hit
      int tube_id = (digitized_hit_tube_id->at(itrigger)).at(idigitizedhit);
      all_pmts_tree->GetEntry(tube_id - 1);

      // determine if it's background or signal
      bool is_background = true;
      for(unsigned int iphotonid=0; iphotonid<((digitized_hit_photon_ids->at(itrigger)).at(idigitizedhit)).size(); iphotonid++){
	// loop on photon ids in digitized hit in the trigger
	Int_t irawhittime = ((digitized_hit_photon_ids->at(itrigger)).at(idigitizedhit)).at(iphotonid);

	bool found_raw_hit_id = false;
	int the_raw_hit;
	for(int irawhit=0; irawhit<trigger_number_raw_hits->at(itrigger); irawhit++){
	  // loop on raw hits in the trigger

	  int raw_tube_id = (raw_hit_tube_id->at(itrigger)).at(irawhit);
	  if( raw_tube_id == tube_id ){
	    found_raw_hit_id = true;
	    the_raw_hit = irawhit;
	    break;
	  }
	}
	if( !found_raw_hit_id ){
	  std::clog << " problem: digi tube " << tube_id << " has no raw hit " << std::endl;
	}else{
	  Int_t parent_id = ((raw_hit_parent_ids->at(itrigger)).at(the_raw_hit)).at(irawhittime);
	  if( parent_id == -1 ){
	    is_background = false;
	  }
	}

      }

      double x0 = pmt_x;
      double y0 = pmt_y;
      double z0 = pmt_z;
      double phi0 = atan2(y0, x0)*180./acos(-1.);
      double t0 = (digitized_hit_time->at(itrigger)).at(idigitizedhit);

      int timebin = (int)((t0 - time_min)/time_bin_size);

      h_n_pmts_vs_time.AddBinContent(timebin);
      h_npe.Fill((digitized_hit_Q->at(itrigger)).at(idigitizedhit));

      if( z0 > z_limit ){ // top
	h_y0_vs_x0_top.Fill(x0, y0);
	h_y0_vs_x0_top_time[timebin].Fill(x0, y0);
      }else if( z0 < -z_limit){ // bottom
	h_y0_vs_x0_bottom.Fill(x0, y0);
	h_y0_vs_x0_bottom_time[timebin].Fill(x0, y0);
      }else{ // side
	h_z0_vs_phi.Fill(phi0, z0);
	if( is_background )
	  h_z0_vs_phi_background.Fill(phi0, z0);
	else
	  h_z0_vs_phi_signal.Fill(phi0, z0);
	h_z0_vs_phi_time[timebin].Fill(phi0, z0);
      }

    }


    TString figname;

    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    TCanvas c_display("c_display","c_display",600,600);
    double BottomMargin = .17;   // fraction of pad height
    double LeftMargin = .1;
    double RightMargin = .18;
    c_display.SetBottomMargin(BottomMargin);
    c_display.SetLeftMargin(LeftMargin);
    c_display.SetRightMargin(RightMargin);

    TPad * topPad;
    TPad * sidePad;
    TPad * bottomPad;
    TPad * topPadplot;
    TPad * topPadtime;

    double zmax_top = h_y0_vs_x0_top.GetMaximum();
    double zmax_side = h_z0_vs_phi.GetMaximum();
    double zmax_bottom = h_y0_vs_x0_bottom.GetMaximum();
    double the_max = std::max(zmax_top, zmax_side);
    the_max = std::max(the_max, zmax_bottom);
    h_y0_vs_x0_top.GetZaxis()->SetRangeUser(0, the_max);
    h_z0_vs_phi.GetZaxis()->SetRangeUser(0, the_max);
    h_y0_vs_x0_bottom.GetZaxis()->SetRangeUser(0, the_max);
    double x_min=0.33;
    double x_max=0.67;
    double X_Min=0.005;
    double X_Max=0.995;
    double ymaxtop=0.995;
    double ymintop=0.66;
    double ymaxside=0.66;
    double yminside=0.33;
    double ymaxbottom=0.33;
    double yminbottom=0.005;
    double x_min_right=0.68;
    double x_max_right=0.95;
    c_display.Divide(1,3);
    topPad =  (TPad*)c_display.GetListOfPrimitives()->FindObject("c_display_1");
    topPad->SetPad(x_min, ymintop, x_max, ymaxtop);
    sidePad =  (TPad*)c_display.GetListOfPrimitives()->FindObject("c_display_2");
    sidePad->SetPad(X_Min, yminside, X_Max, ymaxside);
    bottomPad =  (TPad*)c_display.GetListOfPrimitives()->FindObject("c_display_3");
    bottomPad->SetPad(x_min, yminbottom, x_max, ymaxbottom);

    topPad->cd();
    //h_y0_vs_x0_top.Draw("colz");
    h_y0_vs_x0_top.Draw("p");
    sidePad->cd();
    //h_z0_vs_phi.Draw("colz");
    h_z0_vs_phi.Draw("p");
    bottomPad->cd();
    //h_y0_vs_x0_bottom.Draw("colz");
    h_y0_vs_x0_bottom.Draw("p");
    c_display.Modified();
    c_display.Update();


    figname=Form("figure/display_event_%d_trigger_%d_%d.eps",ievent,itrigger,this_triggertype);
    //c_display.Print(figname.Data());
    figname=Form("figure/display_event_%d_trigger_%d_%d.pdf",ievent,itrigger,this_triggertype);
    c_display.Print(figname.Data());

    if( make_animation ){
      topPad->Clear();
      topPad->SetPad(x_min, ymintop, x_max_right, ymaxtop);
      topPad->Divide(2);
      topPadplot =  (TPad*)topPad->GetListOfPrimitives()->FindObject("c_display_1_1");
      topPadplot->SetPad(0.,0.,0.55,1.);
      topPadtime =  (TPad*)topPad->GetListOfPrimitives()->FindObject("c_display_1_2");
      topPadtime->SetPad(0.60,0.25,0.95,0.75);
    
      for(int i=0; i<nbins_time; i++){

	double the_time = time_min + i*time_bin_size;
	if( use_time_range )
	  if( the_time < _time_min || the_time > _time_max ) continue; //select relevant time window
      
	int the_hits = h_y0_vs_x0_top_time[i].GetEntries() +
	  h_z0_vs_phi_time[i].GetEntries() +
	  h_y0_vs_x0_bottom_time[i].GetEntries();

	TPaveText *time_text = new TPaveText(0.,0.,1.,1.,"NDC");
	time_text->SetFillColor(0);
	time_text->AddText(Form(" t = %.0f ns", the_time));
	time_text->AddText(Form(" NDigits = %d", the_hits));
      
	zmax_top = h_y0_vs_x0_top_time[i].GetMaximum();
	zmax_side = h_z0_vs_phi_time[i].GetMaximum();
	zmax_bottom = h_y0_vs_x0_bottom_time[i].GetMaximum();
	the_max = std::max(zmax_top, zmax_side);
	the_max = std::max(the_max, zmax_bottom);
	h_y0_vs_x0_top_time[i].GetZaxis()->SetRangeUser(0, the_max);
	h_z0_vs_phi_time[i].GetZaxis()->SetRangeUser(0, the_max);
	h_y0_vs_x0_bottom_time[i].GetZaxis()->SetRangeUser(0, the_max);
      
	topPadplot->Clear();
	topPadtime->Clear();
	sidePad->Clear();
	bottomPad->Clear();
      
	topPadplot->cd();
	h_y0_vs_x0_top_time[i].Draw("p");
	topPadtime->cd();
	time_text->Draw();
	sidePad->cd();
	h_z0_vs_phi_time[i].Draw("p");
	bottomPad->cd();
	h_y0_vs_x0_bottom_time[i].Draw("p");
	c_display.Modified();
	c_display.Update();
      
	figname=Form("figure/display_event_%d_trigger_%d_%d_time_%s.gif",ievent,itrigger,this_triggertype,number_with_digits(i,n_digits).c_str());
	c_display.Print(figname.Data());
	figname=Form("figure/display_event_%d_trigger_%d_%d.gif+25",ievent,itrigger,this_triggertype);
	//c_display.Print(figname.Data());
      }
    }//make_animation
    
    of->cd();
    h_y0_vs_x0_top.Write();
    h_y0_vs_x0_bottom.Write();
    h_z0_vs_phi.Write();
    h_z0_vs_phi_background.Write();
    h_z0_vs_phi_signal.Write();
    h_n_pmts_vs_time.Write();
    h_npe.Write();

    if(ievent_to_use >= 0 && ievent == ievent_to_use)
      break;
  }//ievent

  f->Close();

  delete trigger_number ; delete  trigger_date ; delete  trigger_mode ; delete  trigger_vtxvol ; delete  trigger_vec_rec_number ; delete  trigger_jmu ; delete  trigger_jp ; delete  trigger_npar ; delete  trigger_ntrack ; delete  trigger_number_raw_hits ; delete  trigger_number_digitized_hits ; delete  trigger_number_times ;

  delete  trigger_vtx_x ; delete  trigger_vtx_y ; delete  trigger_vtx_z ; delete  trigger_sum_q ; delete trigger_type ; delete trigger_info ;

  delete  track_ipnu ; delete    track_parent_type ; delete    track_flag ; delete    track_start_volume ; delete    track_stop_volume ; delete    track_id ;

  delete  track_ux ; delete    track_uy ; delete    track_uz ; delete    track_M ; delete    track_P ; delete    track_E ; delete    track_px ; delete    track_py ; delete    track_pz ; delete    track_stop_x ; delete    track_stop_y ; delete    track_stop_z ; delete    track_start_x ; delete    track_start_y ; delete    track_start_z ; delete    track_time ;

  delete raw_hit_tube_id; delete raw_hit_tube_times_indexes; delete raw_hit_tube_pe; delete raw_hit_times; delete raw_hit_parent_ids;

  delete digitized_hit_tube_id;  delete digitized_hit_Q;  delete digitized_hit_time;  

  return 1;

}

std::string number_with_digits(int i, int n){

  std::string output="";

  int number_of_non_zero_digits = (i == 0 ? 0 : (int)(log10(i)));

  for(int j=0; j<(n - number_of_non_zero_digits - 1); j++)
    output+="0";

  char c[20];
  sprintf(c,"%d",i);
  std::string sval(c);
  output+=sval;


  return output;

}


