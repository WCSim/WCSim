#include <iostream>
#include <TH1F.h>
#include <stdio.h>     
#include <stdlib.h>    
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObject.h"
#include "TSystem.h"

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"
#include "WCSimEnumerations.hh"
#endif

#ifdef __MAKECINT__
#pragma link C++ class std::vector<std::vector<Int_t> >+;
#pragma link C++ class std::vector<std::vector<Float_t> >+;
#pragma link C++ class std::vector<std::vector<std::vector<Int_t> > >+;
#pragma link C++ class std::vector<std::vector<std::vector<Float_t> > >+;
#endif

void load_library();
TFile * get_input_file(char *filename);
TString create_filename(const char * prefix, TString& filename_string);

void create_tree(char *filename=NULL, bool verbose = false)
{

  // load library
  load_library();

  // open input file
  TFile *f = get_input_file(filename);
  
  // load geometry tree
  WCSimRootGeom* geom = new WCSimRootGeom();
  TTree *input_geom_tree = (TTree*)f->Get("wcsimGeoT");
  TBranch *gb = input_geom_tree->GetBranch("wcsimrootgeom");
  gb->SetAddress(&geom);
  input_geom_tree->GetEntry(0);

  // load events tree
  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
  TTree  *wcsimT = (TTree*)f->Get("wcsimT");
  wcsimT->SetBranchAddress("wcsimrootevent",&wcsimrootsuperevent);
  // Force deletion to prevent memory leak when issuing multiple
  // calls to GetEvent()
  wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);


  // create output file
  TString filenameout(filename);
  TFile * fo = new TFile(create_filename("tree_", filenameout).Data(), "RECREATE");

  // geometry tree
  TTree geom_tree("geom_tree","geometry tree");
  Float_t detector_length, detector_radius, pmt_radius;
  Int_t number_of_pmts;
  geom_tree.Branch("detector_length",&detector_length,"detector_length/F");
  geom_tree.Branch("detector_radius",&detector_radius,"detector_radius/F");
  geom_tree.Branch("pmt_radius",&pmt_radius,"pmt_radius/F");
  geom_tree.Branch("number_of_pmts",&number_of_pmts,"number_of_pmts/I");
  detector_length = geom->GetWCCylLength();
  detector_radius = geom->GetWCCylRadius();
  pmt_radius = geom->GetWCPMTRadius();
  number_of_pmts = geom->GetWCNumPMT();
  geom_tree.Fill();


  // all pmts tree
  TTree all_pmts_tree("all_pmts_tree","all pmts tree");
  Int_t pmt_number, pmt_location;
  Float_t pmt_ux, pmt_uy, pmt_uz, pmt_x, pmt_y, pmt_z;
  all_pmts_tree.Branch("pmt_number",&pmt_number,"pmt_number/I");
  all_pmts_tree.Branch("pmt_location",&pmt_location,"pmt_location/I"); // 0 = top cap, 1 = wall, 2 = bottom cap
  all_pmts_tree.Branch("pmt_ux",&pmt_ux,"pmt_ux/F"); // (ux, uy, uz) direction of the PMT face
  all_pmts_tree.Branch("pmt_uy",&pmt_uy,"pmt_uy/F");
  all_pmts_tree.Branch("pmt_uz",&pmt_uz,"pmt_uz/F");
  all_pmts_tree.Branch("pmt_x",&pmt_x,"pmt_x/F"); // (x, y, z) of the center of the sphere which forms the PMT
  all_pmts_tree.Branch("pmt_y",&pmt_y,"pmt_y/F");
  all_pmts_tree.Branch("pmt_z",&pmt_z,"pmt_z/F");
  for(int i=0; i<number_of_pmts; i++){
    WCSimRootPMT pmt = geom->GetPMT(i);
    pmt_number = pmt.GetTubeNo();
    pmt_location = pmt.GetCylLoc();
    pmt_ux = pmt.GetOrientation(0);
    pmt_uy = pmt.GetOrientation(1);
    pmt_uz = pmt.GetOrientation(2);
    pmt_x = pmt.GetPosition(0);
    pmt_y = pmt.GetPosition(1);
    pmt_z = pmt.GetPosition(2);
    all_pmts_tree.Fill();
  }


  // primary events tree
  TTree primary_events_tree("primary_events_tree","primary events tree");
  WCSimRootTrigger *wcsimrootevent;

  int n_primary_events = wcsimT->GetEntries();
  int number_of_tracks;
  int number_of_raw_cherenkov_hits;
  int number_of_digitized_cherenkov_hits;

  std::vector<Int_t> event_number, trigger_number, trigger_date, trigger_mode, trigger_vtxvol, trigger_vec_rec_number, trigger_jmu, trigger_jp, trigger_npar, trigger_ntrack, trigger_number_raw_hits, trigger_number_digitized_hits, trigger_number_times, trigger_type;
  std::vector<Float_t> trigger_vtx_x, trigger_vtx_y, trigger_vtx_z, trigger_sum_q;
  std::vector<std::vector<Float_t> > trigger_info;

  std::vector<Int_t> v_track_ipnu, v_track_parent_type, v_track_flag, v_track_start_volume, v_track_stop_volume, v_track_id;
  std::vector<std::vector<Int_t> > track_ipnu,   track_parent_type,   track_flag,   track_start_volume,   track_stop_volume,   track_id;

  std::vector<Float_t> v_track_ux, v_track_uy, v_track_uz, v_track_M, v_track_P, v_track_E, v_track_px, v_track_py, v_track_pz, v_track_stop_x, v_track_stop_y, v_track_stop_z, v_track_start_x, v_track_start_y, v_track_start_z, v_track_time;
  std::vector<std::vector<Float_t> > track_ux,   track_uy,   track_uz,   track_M,   track_P,   track_E,   track_px,   track_py,   track_pz,   track_stop_x,   track_stop_y,   track_stop_z,   track_start_x,   track_start_y,   track_start_z,   track_time;

  std::vector<Int_t> v_raw_hit_tube_id, v_raw_hit_tube_times_indexes, v_raw_hit_tube_pe;
  std::vector<std::vector<Int_t> > raw_hit_tube_id, raw_hit_tube_times_indexes, raw_hit_tube_pe;

  std::vector<Float_t> vv_raw_hit_times;
  std::vector<std::vector<Float_t> > v_raw_hit_times;
  std::vector<std::vector<std::vector<Float_t> > > raw_hit_times;
  std::vector<Int_t> vv_raw_hit_parent_ids;
  std::vector<std::vector<Int_t> > v_raw_hit_parent_ids;
  std::vector<std::vector<std::vector<Int_t> > > raw_hit_parent_ids;

  std::vector<Int_t> v_digitized_hit_tube_id;
  std::vector<std::vector<Int_t> > digitized_hit_tube_id;
  std::vector<Float_t> v_digitized_hit_Q, v_digitized_hit_time;
  std::vector<std::vector<Float_t> > digitized_hit_Q, digitized_hit_time;
  std::vector<Int_t> vv_digitized_hit_photon_ids;
  std::vector<std::vector<Int_t> > v_digitized_hit_photon_ids;
  std::vector<std::vector<std::vector<Int_t> > > digitized_hit_photon_ids;

  primary_events_tree.Branch("event_number",&event_number);
  primary_events_tree.Branch("trigger_number",&trigger_number);
  primary_events_tree.Branch("trigger_date",&trigger_date);
  primary_events_tree.Branch("trigger_mode",&trigger_mode); // interaction mode
  primary_events_tree.Branch("trigger_vtxvol",&trigger_vtxvol); // volume of vertex
  primary_events_tree.Branch("trigger_vtx_x",&trigger_vtx_x); // interaction vertex
  primary_events_tree.Branch("trigger_vtx_y",&trigger_vtx_y);
  primary_events_tree.Branch("trigger_vtx_z",&trigger_vtx_z);
  primary_events_tree.Branch("trigger_vec_rec_number",&trigger_vec_rec_number); // info event number in inputvetcotfile
  primary_events_tree.Branch("trigger_jmu",&trigger_jmu); // index to muon
  primary_events_tree.Branch("trigger_jp",&trigger_jp); // index to proton
  primary_events_tree.Branch("trigger_npar",&trigger_npar); // number of final state particles
  primary_events_tree.Branch("trigger_ntrack",&trigger_ntrack);
  primary_events_tree.Branch("trigger_number_raw_hits",&trigger_number_raw_hits); // Total number of tubes with hits
  primary_events_tree.Branch("trigger_number_digitized_hits",&trigger_number_digitized_hits); // Number of PMTs with digitized hits
  primary_events_tree.Branch("trigger_sum_q",&trigger_sum_q); // sum of q(readout digitized pe) in event
  primary_events_tree.Branch("trigger_type",&trigger_type); // enumeration of trigger type (stored as int)
  primary_events_tree.Branch("trigger_info",&trigger_info); // info about why the trigger was passed
  primary_events_tree.Branch("trigger_number_times",&trigger_number_times);

  primary_events_tree.Branch("track_ipnu",&track_ipnu); // id of final state particle
  primary_events_tree.Branch("track_parent_type",&track_parent_type); // ID of parent of ith particle (0 if primary)
  primary_events_tree.Branch("track_ux",&track_ux); // direction of ith final state particle
  primary_events_tree.Branch("track_uy",&track_uy);
  primary_events_tree.Branch("track_uz",&track_uz);
  primary_events_tree.Branch("track_px",&track_px); // momentum of ith final state particle
  primary_events_tree.Branch("track_py",&track_py);
  primary_events_tree.Branch("track_pz",&track_pz);
  primary_events_tree.Branch("track_flag",&track_flag); // flag: -1 = incoming neutrino
  //       -2 = target
  //        1 = outgoing lepton
  //        2 = most energetic outgoing nucleon
  primary_events_tree.Branch("track_M",&track_M); // mass of ith final state particle
  primary_events_tree.Branch("track_P",&track_P); // momentum of ith final state particle
  primary_events_tree.Branch("track_E",&track_E); // energy of ith final state particle
  primary_events_tree.Branch("track_start_volume",&track_start_volume); // starting volume of ith final state particle
  primary_events_tree.Branch("track_stop_volume",&track_stop_volume); // stopping volume of ith final state particle
  primary_events_tree.Branch("track_stop_x",&track_stop_x); // stopping point of ith final state particle 
  primary_events_tree.Branch("track_stop_y",&track_stop_y);
  primary_events_tree.Branch("track_stop_z",&track_stop_z);
  primary_events_tree.Branch("track_start_x",&track_start_x); // starting point of ith final state particle
  primary_events_tree.Branch("track_start_y",&track_start_y);
  primary_events_tree.Branch("track_start_z",&track_start_z);
  primary_events_tree.Branch("track_time",&track_time); // creation time of ith final state particle
  primary_events_tree.Branch("track_id",&track_id);

  primary_events_tree.Branch("raw_hit_tube_id",&raw_hit_tube_id);
  primary_events_tree.Branch("raw_hit_tube_times_indexes",&raw_hit_tube_times_indexes); // The indexes of times recorded at each tube
  primary_events_tree.Branch("raw_hit_tube_pe",&raw_hit_tube_pe); // The totalPE recorded at each tube
  primary_events_tree.Branch("raw_hit_times",&raw_hit_times); // The time of each hit of each tube
  primary_events_tree.Branch("raw_hit_parent_ids",&raw_hit_parent_ids); // The parent id of each hit of each tube

  primary_events_tree.Branch("digitized_hit_tube_id",&digitized_hit_tube_id); // The readout tube ID
  primary_events_tree.Branch("digitized_hit_Q",&digitized_hit_Q); // The readout digitized pe
  primary_events_tree.Branch("digitized_hit_time",&digitized_hit_time);  // The readout digitized time
  primary_events_tree.Branch("digitized_hit_photon_ids",&digitized_hit_photon_ids);

  for(int ievent=0; ievent<n_primary_events; ievent++){ 
    // loop on primary events
    wcsimT->GetEvent(ievent); 

    // init trigger variables
    event_number.clear();
    trigger_number.clear();
    trigger_date.clear();
    trigger_mode.clear();
    trigger_vtxvol.clear();
    trigger_vtx_x.clear();
    trigger_vtx_y.clear();
    trigger_vtx_z.clear();
    trigger_vec_rec_number.clear();
    trigger_jmu.clear();
    trigger_jp.clear();
    trigger_npar.clear();
    trigger_ntrack.clear();
    trigger_number_raw_hits.clear();
    trigger_number_digitized_hits.clear();
    trigger_number_times.clear();
    trigger_sum_q.clear();
    trigger_type.clear();
    trigger_info.clear();

    // init track variables
    track_ipnu.clear();
    track_parent_type.clear();
    track_ux.clear();
    track_uy.clear();
    track_uz.clear();
    track_px.clear();
    track_py.clear();
    track_pz.clear();
    track_flag.clear();
    track_M.clear();
    track_P.clear();
    track_E.clear();
    track_start_volume.clear();
    track_stop_volume.clear();
    track_stop_x.clear();
    track_stop_y.clear();
    track_stop_z.clear();
    track_start_x.clear();
    track_start_y.clear();
    track_start_z.clear();
    track_time.clear();
    track_id.clear();

    // init raw hits variables
    raw_hit_tube_id.clear();
    raw_hit_tube_times_indexes.clear();
    raw_hit_tube_pe.clear();
    raw_hit_times.clear();
    raw_hit_parent_ids.clear();

    // init digitized hits variables
    digitized_hit_tube_id.clear();
    digitized_hit_Q.clear();
    digitized_hit_time.clear();
    digitized_hit_photon_ids.clear();


    for(int itrigger=0; itrigger<wcsimrootsuperevent->GetNumberOfEvents(); itrigger++){
      // loop on triggers in the event

      if( itrigger > 0 ) continue; // seg faults

      wcsimrootevent = wcsimrootsuperevent->GetTrigger(itrigger);

      event_number.push_back(wcsimrootevent->GetHeader()->GetEvtNum());
      trigger_number.push_back(itrigger); 
      trigger_date.push_back(wcsimrootevent->GetHeader()->GetDate());
      trigger_mode.push_back(wcsimrootevent->GetMode());
      trigger_vtxvol.push_back(wcsimrootevent->GetVtxvol());
      trigger_vtx_x.push_back(wcsimrootevent->GetVtx(0));
      trigger_vtx_y.push_back(wcsimrootevent->GetVtx(1));
      trigger_vtx_z.push_back(wcsimrootevent->GetVtx(2));
      trigger_vec_rec_number.push_back(wcsimrootevent->GetVecRecNumber());
      trigger_jmu.push_back(wcsimrootevent->GetJmu());
      trigger_jp.push_back(wcsimrootevent->GetJp());
      trigger_npar.push_back(wcsimrootevent->GetNpar());
      number_of_tracks = wcsimrootevent->GetNtrack();
      trigger_ntrack.push_back(number_of_tracks);
      number_of_raw_cherenkov_hits = wcsimrootevent->GetNumTubesHit();
      trigger_number_raw_hits.push_back(number_of_raw_cherenkov_hits);
      number_of_digitized_cherenkov_hits = wcsimrootevent->GetNcherenkovdigihits();
      trigger_number_digitized_hits.push_back(number_of_digitized_cherenkov_hits);
      trigger_number_times.push_back(wcsimrootevent->GetNcherenkovhittimes());
      trigger_sum_q.push_back(wcsimrootevent->GetSumQ());

      trigger_type.push_back(wcsimrootevent->GetTriggerType());
      trigger_info.push_back(wcsimrootevent->GetTriggerInfo());

      // init trigger track variables
      v_track_ipnu.clear();
      v_track_parent_type.clear();
      v_track_flag.clear();
      v_track_start_volume.clear();
      v_track_stop_volume.clear();
      v_track_id.clear();
      v_track_ux.clear();
      v_track_uy.clear();
      v_track_uz.clear();
      v_track_M.clear();
      v_track_P.clear();
      v_track_E.clear();
      v_track_px.clear();
      v_track_py.clear();
      v_track_pz.clear();
      v_track_stop_x.clear();
      v_track_stop_y.clear();
      v_track_stop_z.clear();
      v_track_start_x.clear();  
      v_track_start_y.clear();
      v_track_start_z.clear();
      v_track_time.clear();

      for(int itrack=0; itrack<number_of_tracks; itrack++){
	// loop on tracks in the trigger
	TObject *element = (wcsimrootevent->GetTracks())->At(itrack);
	WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);

	v_track_ipnu.push_back(wcsimroottrack->GetIpnu());
	v_track_parent_type.push_back(wcsimroottrack->GetParenttype());
	v_track_ux.push_back(wcsimroottrack->GetDir(0));
	v_track_uy.push_back(wcsimroottrack->GetDir(1));
	v_track_uz.push_back(wcsimroottrack->GetDir(2));
	v_track_px.push_back(wcsimroottrack->GetPdir(0));
	v_track_py.push_back(wcsimroottrack->GetPdir(1));
	v_track_pz.push_back(wcsimroottrack->GetPdir(2));
	v_track_flag.push_back(wcsimroottrack->GetFlag());
	v_track_M.push_back(wcsimroottrack->GetM());
	v_track_P.push_back(wcsimroottrack->GetP());
	v_track_E.push_back(wcsimroottrack->GetE());
	v_track_start_volume.push_back(wcsimroottrack->GetStartvol());
	v_track_stop_volume.push_back(wcsimroottrack->GetStopvol());
	v_track_stop_x.push_back(wcsimroottrack->GetStop(0));
	v_track_stop_y.push_back(wcsimroottrack->GetStop(1));
	v_track_stop_z.push_back(wcsimroottrack->GetStop(2));
	v_track_start_x.push_back(wcsimroottrack->GetStart(0));
	v_track_start_y.push_back(wcsimroottrack->GetStart(1));
	v_track_start_z.push_back(wcsimroottrack->GetStart(2));
	v_track_time.push_back(wcsimroottrack->GetTime());
	v_track_id.push_back(wcsimroottrack->GetId());
      }
      
      track_ipnu.push_back(v_track_ipnu);
      track_parent_type.push_back(v_track_parent_type);
      track_ux.push_back(v_track_ux);
      track_uy.push_back(v_track_uy);
      track_uz.push_back(v_track_uz);
      track_px.push_back(v_track_px);
      track_py.push_back(v_track_py);
      track_pz.push_back(v_track_pz);
      track_flag.push_back(v_track_flag);
      track_M.push_back(v_track_M);
      track_P.push_back(v_track_P);
      track_E.push_back(v_track_E);
      track_start_volume.push_back(v_track_start_volume);
      track_stop_volume.push_back(v_track_stop_volume);
      track_stop_x.push_back(v_track_stop_x);
      track_stop_y.push_back(v_track_stop_y);
      track_stop_z.push_back(v_track_stop_z);
      track_start_x.push_back(v_track_start_x);
      track_start_y.push_back(v_track_start_y);
      track_start_z.push_back(v_track_start_z);
      track_time.push_back(v_track_time);
      track_id.push_back(v_track_id);
        
      // init trigger raw hits variables
      v_raw_hit_tube_id.clear();
      v_raw_hit_tube_times_indexes.clear();
      v_raw_hit_tube_pe.clear();
      v_raw_hit_times.clear();
      v_raw_hit_parent_ids.clear();

      // init trigger digitized hits variables
      v_digitized_hit_tube_id.clear();
      v_digitized_hit_Q.clear();
      v_digitized_hit_time.clear();
      v_digitized_hit_photon_ids.clear();

      // Grab the big arrays of times and parent IDs
      TClonesArray *timeArray = wcsimrootevent->GetCherenkovHitTimes();
    
      for(int irawhit=0; irawhit<number_of_raw_cherenkov_hits; irawhit++){
	// loop on raw hits in the trigger
	TObject *Hit = (wcsimrootevent->GetCherenkovHits())->At(irawhit);
	WCSimRootCherenkovHit *wcsimrootcherenkovhit = dynamic_cast<WCSimRootCherenkovHit*>(Hit);

	int tubeNumber     = wcsimrootcherenkovhit->GetTubeID();
	int timeArrayIndex = wcsimrootcherenkovhit->GetTotalPe(0);
	int peForTube      = wcsimrootcherenkovhit->GetTotalPe(1);
	v_raw_hit_tube_id.push_back(tubeNumber);
	v_raw_hit_tube_times_indexes.push_back(timeArrayIndex);
	v_raw_hit_tube_pe.push_back(peForTube);

	// extract the PMT object
	//WCSimRootPMT pmt   = geom->GetPMT(tubeNumber-1);

	// init trigger raw hit time variables
	vv_raw_hit_times.clear();
	vv_raw_hit_parent_ids.clear();

	for (int irawhittimes = timeArrayIndex; irawhittimes < timeArrayIndex + peForTube; irawhittimes++)
	  {
	    WCSimRootCherenkovHitTime * HitTime = dynamic_cast<WCSimRootCherenkovHitTime*>(timeArray->At(irawhittimes));
	    
	    vv_raw_hit_times.push_back(HitTime->GetTruetime());
	    vv_raw_hit_parent_ids.push_back(HitTime->GetParentID());
	  }
	
	v_raw_hit_times.push_back(vv_raw_hit_times);
	v_raw_hit_parent_ids.push_back(vv_raw_hit_parent_ids);

      } // End of loop over Cherenkov hits

      raw_hit_tube_id.push_back(v_raw_hit_tube_id);
      raw_hit_tube_times_indexes.push_back(v_raw_hit_tube_times_indexes);
      raw_hit_tube_pe.push_back(v_raw_hit_tube_pe);
      raw_hit_times.push_back(v_raw_hit_times);
      raw_hit_parent_ids.push_back(v_raw_hit_parent_ids);


      // init trigger digitized hits variables
      v_digitized_hit_tube_id.clear();
      v_digitized_hit_Q.clear();
      v_digitized_hit_time.clear();

      for(int idigitizedhit=0; idigitizedhit<number_of_digitized_cherenkov_hits; idigitizedhit++){
	// loop on digitized hits in the trigger
	TObject *Hit = (wcsimrootevent->GetCherenkovDigiHits())->At(idigitizedhit);
    	WCSimRootCherenkovDigiHit *wcsimrootcherenkovdigihit = dynamic_cast<WCSimRootCherenkovDigiHit*>(Hit);

	v_digitized_hit_tube_id.push_back(wcsimrootcherenkovdigihit->GetTubeId());
	v_digitized_hit_Q.push_back(wcsimrootcherenkovdigihit->GetQ());
	v_digitized_hit_time.push_back(wcsimrootcherenkovdigihit->GetT());

	// init trigger raw hit time variables
	vv_digitized_hit_photon_ids.clear();
	wcsimrootcherenkovdigihit->GetPhotonIds();
	for(unsigned int iph=0; iph<(wcsimrootcherenkovdigihit->GetPhotonIds()).size(); iph++)
	  vv_digitized_hit_photon_ids.push_back((wcsimrootcherenkovdigihit->GetPhotonIds()).at(iph));

	v_digitized_hit_photon_ids.push_back(vv_digitized_hit_photon_ids);

	//std::clog << " ievent " << ievent << " itrigger " << itrigger << " idigitizedhit " << idigitizedhit << " tubeid " << wcsimrootcherenkovdigihit->GetTubeId() << " Q " << wcsimrootcherenkovdigihit->GetQ() << " time " << wcsimrootcherenkovdigihit->GetT() << std::endl;

      }

      digitized_hit_tube_id.push_back(v_digitized_hit_tube_id);
      digitized_hit_Q.push_back(v_digitized_hit_Q);
      digitized_hit_time.push_back(v_digitized_hit_time);
      digitized_hit_photon_ids.push_back(v_digitized_hit_photon_ids);

      std::clog << " ievent " << ievent << " itrigger " << itrigger << " number_of_raw_cherenkov_hits " << number_of_raw_cherenkov_hits << std::endl;
    
    
    }

    primary_events_tree.Fill();
  }


  fo->Write();

  exit(0);

}


void load_library(){

#if !defined(__MAKECINT__)
  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIMDIR");
  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
  }else{
    gSystem->Load("../libWCSimRoot.so");
  }
#endif
  return;

}

TString create_filename(const char * prefix, TString& filename_string)
{
  //std::cout << "Creating filename from prefix " << prefix << " and filename_string " << filename_string << std::endl;                                                                                                                                                  
  TString prefix_string(prefix);
  TString outfilename = prefix_string + filename_string;
  return outfilename;
}

TFile * get_input_file(char *filename){

  TFile *f;
  // Open the file                                                                                                            
  if (filename==NULL){
    f = new TFile("../wcsim.root","read");
  }else{
    f = new TFile(filename,"read");
  }
  if (!f->IsOpen()){
    std::cerr << "Error, could not open input file: " << filename << std::endl;
    exit(0);
  }
  
  return f;

}



