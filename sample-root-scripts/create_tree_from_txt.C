#include <iostream>
#include <TH1F.h>
#include <stdio.h>     
#include <stdlib.h>    
#include <vector>


int read_number_of_pmts();
bool read_pmts();
int read_number_of_input_hits();
bool read_input();
bool read_detector();
bool read_the_pmts();
bool read_the_detector();
bool read_the_input();

/////////////////////////////
// define global variables //
/////////////////////////////
/// pmts
int n_PMTs; // number of pmts in the detector
double * PMT_x, *PMT_y, *PMT_z; // coordinates of the pmts in the detector
/// detector
double pmt_radius;
double detector_height; // detector height
double detector_radius; // detector radius
/// hits
int n_hits; // number of input hits from the detector
size_t * host_ids; // pmt id of a hit
int * host_times;  // time of a hit
bool create_from_emerald = true;


void create_tree_from_txt()
{



  // create output file
  TFile *fo = new TFile("output_from_txt.root", "RECREATE");

  // geometry tree
  TTree geom_tree("geom_tree","geometry tree");
  Float_t detector_length, _detector_radius, _pmt_radius;
  Int_t number_of_pmts;
  geom_tree.Branch("detector_length",&detector_length,"detector_length/F");
  geom_tree.Branch("detector_radius",&_detector_radius,"detector_radius/F");
  geom_tree.Branch("pmt_radius",&_pmt_radius,"pmt_radius/F");
  geom_tree.Branch("number_of_pmts",&number_of_pmts,"number_of_pmts/I");
  if( !read_the_detector() ) return 0;
  if( !read_the_pmts() ) return 0;
  detector_length = detector_height;
  _pmt_radius = pmt_radius;
  _detector_radius = detector_radius;
  number_of_pmts = n_PMTs;
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
  std::cout << " detector_height " << detector_height << " detector_radius " << detector_radius << " n_PMTs " << n_PMTs << std::endl;
  for(int i=0; i<n_PMTs; i++){
    pmt_number = i+1;
    pmt_x = PMT_x[i];
    pmt_y = PMT_y[i];
    pmt_z = PMT_z[i];
    all_pmts_tree.Fill();
  }


  // primary events tree
  TTree primary_events_tree("primary_events_tree","primary events tree");

  int n_primary_events = 1;
  int number_of_tracks;
  int number_of_raw_cherenkov_hits;
  int number_of_digitized_cherenkov_hits;

  std::vector<Int_t> trigger_number, trigger_date, trigger_mode, trigger_vtxvol, trigger_vec_rec_number, trigger_jmu, trigger_jp, trigger_npar, trigger_ntrack, trigger_number_raw_hits, trigger_number_digitized_hits, trigger_number_times;

  std::vector<Float_t> trigger_vtx_x, trigger_vtx_y, trigger_vtx_z, trigger_sum_q;

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

  if( !read_the_input() ) return 0;

  for(int ievent=0; ievent<n_primary_events; ievent++){ 
    // loop on primary events

    // init trigger variables
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


    for(int itrigger=0; itrigger<1; itrigger++){
      // loop on triggers in the event

      trigger_number_raw_hits.push_back(0);

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


      for(int idigitizedhit=0; idigitizedhit<n_hits; idigitizedhit++){
	// loop on digitized hits in the trigger

	v_digitized_hit_tube_id.push_back(host_ids[idigitizedhit]);
	v_digitized_hit_Q.push_back(0);
	v_digitized_hit_time.push_back(host_times[idigitizedhit]);

	// init trigger raw hit time variables
	vv_digitized_hit_photon_ids.clear();
	vv_digitized_hit_photon_ids.push_back(0);

	v_digitized_hit_photon_ids.push_back(vv_digitized_hit_photon_ids);

      }

      digitized_hit_tube_id.push_back(v_digitized_hit_tube_id);
      digitized_hit_Q.push_back(v_digitized_hit_Q);
      digitized_hit_time.push_back(v_digitized_hit_time);
      digitized_hit_photon_ids.push_back(v_digitized_hit_photon_ids);

    }

    primary_events_tree.Fill();
  }


  fo->Write();

  exit(0);

}




int read_number_of_pmts(){

  FILE *f=fopen("all_pmts.txt", "r");
  if (f == NULL){
    printf(" cannot read pmts file \n");
    fclose(f);
    return -1;
  }

  int n_pmts = 0;

  for (char c = getc(f); c != EOF; c = getc(f))
    if (c == '\n')
      n_pmts ++;

  fclose(f);
  return n_pmts;

}

bool read_pmts(){

  FILE *f=fopen("all_pmts.txt", "r");

  double x, y, z;
  size_t id;
  for( size_t i=0; i<n_PMTs; i++){
    if( fscanf(f, "%d %lf %lf %lf", &id, &x, &y, &z) != 4 ){
      printf(" problem scanning pmt %d \n", i);
      fclose(f);
      return false;
    }
    PMT_x[id-1] = x;
    PMT_y[id-1] = y;
    PMT_z[id-1] = z;
  }

  fclose(f);
  return true;

}


int read_number_of_input_hits(){

#if !create_from_emerald
  FILE *f=fopen("all_hits.txt", "r");
#else
  FILE *f=fopen("all_hits_emerald.txt", "r");
#endif

  if (f == NULL){
    printf(" cannot read input file \n");
    fclose(f);
    return -1;
  }

  int n_hits = 0;

  for (char c = getc(f); c != EOF; c = getc(f))
    if (c == '\n')
      n_hits ++;

  fclose(f);
  return n_hits;

}

bool read_input(){

#if !create_from_emerald
  FILE *f=fopen("all_hits.txt", "r");
#else
  FILE *f=fopen("all_hits_emerald.txt", "r");
#endif

  int time;
  double timef;
  size_t id;
  for( size_t i=0; i<n_hits; i++){
    if( fscanf(f, "%d %lf", &id, &timef) != 2 ){
      printf(" problem scanning hit %d from input \n", i);
      fclose(f);
      return false;
    }
    time = int(floor(timef));
    host_times[i] = time;
    host_ids[i] = id;
  }


  fclose(f);
  return true;

}


bool read_detector(){

  FILE *f=fopen("detector.txt", "r");
  if( fscanf(f, "%lf %lf %lf", &detector_height, &detector_radius, &pmt_radius) != 3 ){
    printf(" problem scanning detector \n");
    fclose(f);
    return false;
  }

  fclose(f);
  return true;

}


bool read_the_pmts(){

  printf(" --- read pmts \n");
  n_PMTs = read_number_of_pmts();
  if( !n_PMTs ) return false;
  printf(" detector contains %d PMTs \n", n_PMTs);
  PMT_x = (double *)malloc(n_PMTs*sizeof(double));
  PMT_y = (double *)malloc(n_PMTs*sizeof(double));
  PMT_z = (double *)malloc(n_PMTs*sizeof(double));
  if( !read_pmts() ) return false;
  //print_pmts();
  return true;

}

bool read_the_detector(){

  printf(" --- read detector \n");
  if( !read_detector() ) return false;
  printf(" detector height %f cm, radius %f cm \n", detector_height, detector_radius);
  return true;

}


bool read_the_input(){

  printf(" --- read input \n");
  n_hits = read_number_of_input_hits();
  if( !n_hits ) return false;
  printf(" input contains %d hits \n", n_hits);
  host_ids = (size_t *)malloc(n_hits*sizeof(size_t));
  host_times = (int *)malloc(n_hits*sizeof(int));
  if( !read_input() ) return false;

  return true;
}
