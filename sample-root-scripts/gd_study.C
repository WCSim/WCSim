// Sam Short (s.short@qmul.ac.uk)
//
// To run this macro on the command lind (with a verbose output) just do: 
//
// 1. Change the path name to include WCSimRootEvent.hh
// 2. export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WCSIMDIR
// 3. g++ -o gd_study.exe gd_study.C `root-config --cflags --glibs`  -L${WCSIMDIR} -I${WCSIMDIR} -lWCSimRoot
// 4. ./gd_study.exe -v true -f /path/to/wcsim/output/file/wcsim.root
//

#include <iostream>
#include <stdio.h>     
#include <stdlib.h>
#include <map>    
#include <algorithm>
#include <vector>
#include <sstream>

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"

#include "/data/short/hk/software_new_geant4/WCSim/include/WCSimRootEvent.hh"

// Function to parse arguments from the command lind
void ParseArgs(int argc, char **argv);


// Useful functions
std::vector<int> GetAllTrackIDsInEvent(WCSimRootTrigger* wcsimrootevent);
int GetParentID(WCSimRootTrigger* wcsimrootevent, int trackID);
int GetParentPDG(WCSimRootTrigger* wcsimrootevent, int trackID);
int GetPDG(WCSimRootTrigger* wcsimrootevent, int trackID);
std::string GetProcessName(WCSimRootTrigger* wcsimrootevent, int trackID);
void MatchTracksInGivenTrajectory(WCSimRootTrigger* wcsimrootevent, 
				  int curr_id, std::vector<int>& chain);
std::vector< std::vector<int> > MatchTracks(WCSimRootTrigger* wcsimrootevent, bool neutrons_only, bool verbose);


// Global variables
char const* gFilename;
bool gVerbose;




// Main function to run
int main(int argc, char** argv){

  // Parse the command line arguments
  ParseArgs(argc,argv);
  char const* filename = gFilename; 
  //char const* filename = "/data/t2k/short/hyperk/gd_new_wcsim_Cylinder_60x74_20inchBandL_14perCent_neutron_10MeV.root";
  bool verbose = gVerbose; //true;

  // ******************************************
  // Load the library with class dictionary info
  // (create with "gmake shared")
  char* wcsimdirenv;
  wcsimdirenv = getenv("WCSIMDIR");
  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
    std::cout<<"Loaded file"<<std::endl;
  }else{
    gSystem->Load("../libWCSimRoot.so");
  }

  // ******************************************
  TFile *file;
  // Open the file
  if (filename==NULL){
    file = new TFile("../wcsim.root","read");
  }else{
    file = new TFile(filename,"read");
  }
  if(!file->IsOpen()){
    std::cout<<"ERROR: Could not open input file: "<<filename<<std::endl;
    exit(0);
  }
  
  // ******************************************
  // Get the a pointer to the tree from the file
  TTree *tree = (TTree*)file->Get("wcsimT");
  
  // Get the number of events
  int nevent = tree->GetEntries();
  std::cout<<"Number of events in file = "<<nevent<<std::endl;
  
  // Create a WCSimRootEvent to put stuff from the tree in
  WCSimRootEvent* wcsimrootsuperevent = new WCSimRootEvent();

  // Set the branch address for reading from the tree
  TBranch *branch = tree->GetBranch("wcsimrootevent");
  branch->SetAddress(&wcsimrootsuperevent);

  // Force deletion to prevent memory leak 
  tree->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);

  // ******************************************
  // Counters for the whole file
  int nNeutronsFile = 0;
  int nCapturesFile = 0;
  int nNonCapturesFile = 0;
  std::map<int,int> captureNucleusFile;
  std::map<std::string, int> nonCaptureNucleusFile;

  int nEventsWithAtLeastOneNeutron = 0;
  int nEventsWithAtLeastOneCapture = 0;
  std::map<int,int> eventCaptureMap;

  // ******************************************
  // Now loop over events
  for(int ev=0; ev<nevent; ev++){

    tree->GetEntry(ev);
    if(verbose){std::cout<<"\n***************** event "<<ev<<" *****************"<<std::endl;}

    // ******************************************
    // Loop over triggers (sub events)
    // NB: matching currently can't handle particle trajectories
    //     with tracks split over more than one sub event.
    for(int trig = 0; trig < 1; trig++){
      WCSimRootTrigger* wcsimrootevent = wcsimrootsuperevent->GetTrigger(trig);
      if(verbose){std::cout<<"\n ******* sub event "<<trig<<" *******"<<std::endl;}
      if(verbose){std::cout<<"  - Number of tracks in sub event =  "<<wcsimrootevent->GetNtrack()<<std::endl;}

      // ******************************************
      // Loop over tracks in sub event to print info if required
      // (a track is not the same as a particle - a physical particle
      // has multiple tracks - a new track is created when the physical
      // particle changes momentum, for example)
      if(verbose){
	for(int i=0; i<wcsimrootevent->GetNtrack(); i++){
	  WCSimRootTrack* wcsimroottrack = dynamic_cast<WCSimRootTrack*>((wcsimrootevent->GetTracks())->At(i));   
	  
	  int pdg = wcsimroottrack->GetIpnu(); 
	  int id = wcsimroottrack->GetId();
	  int parentpdg = wcsimroottrack->GetParenttype();
	  int parentid = wcsimroottrack->GetParentId();
	  std::string processName = wcsimroottrack->GetProcessName();
	  
	  std::cout<<"    - Track "<<i<<": id = "<<id<<" parentid = "<<parentid
		   <<", pdg = "<<pdg<<" parentpdg = "<<parentpdg
		   <<" ("<<processName<<")"<<std::endl;
	}
      }


      // ******************************************
      // Match the GEANT4/WCSim tracks to make a real particle trajectory
      // i.e. if a particle has tracks with IDs: 0 1 4 7 19
      //      fill a vector with those numbers
      // Using this method, a trajectory can be: 
      //   0 (neutron) 1 (same neutron) 4 (same neutron) 7 (same neutron) 19 (Gd that neutron captures on)
      // So, want to get all trajectories (i.e. 0 1 4 7 19) and neutron only (i.e. 0 1 4 7)
      // so that can see what the neutron captured on.
      // (Need to do it this way because if the neutron stops existing because of 
      // an interaction on a nucleus that isn't a capture nucleus, the ProcessName isn't "nCapture")
      if(verbose){std::cout<<"\n  - ALL particle trajectories in event..."<<std::endl;}
      std::vector< std::vector<int> > trajectories = MatchTracks(wcsimrootevent, false, verbose);

      if(verbose){std::cout<<"\n  - NEUTRON particle trajectories in event..."<<std::endl;}
      std::vector< std::vector<int> > neutron_trajectories = MatchTracks(wcsimrootevent, true, verbose);

      
      // ******************************************
      // Number of neutrons is the number of neutron_trajectories
      int nNeutrons = (int) neutron_trajectories.size(); 
      nNeutronsFile += nNeutrons;
      if(nNeutrons != 0){nEventsWithAtLeastOneNeutron++; eventCaptureMap[ev] = 0;}
     
      // Number of neutron captures is the number of vectors in
      // neutron_trajectories that are a subset of a vector in
      // trajectories (provided the PDG is that of a nucleus)
      int nCaptures = 0;
      std::map<int,int> captureNucleus;

      // Save the PDG codes for particles produced when a neutron
      // stops existing (i.e. does not get captured, but 
      // undergoes some other sort of interaction)
      int nNonCaptures = 0;
      std::map<std::string,int> nonCaptureNucleus;

      // Loop over trajectories to find match neutron trajectories
      // to the other trajectories to find out what happened to the neutrons
      for(unsigned int i = 0; i < neutron_trajectories.size(); i++){
	std::vector<int> compare_neutron_traj = neutron_trajectories[i];

	std::vector<int> nonCaptureNucleusTraj;

	for(unsigned int j = 0; j < trajectories.size(); j++){
	  std::vector<int> compare_traj = trajectories[j];

	  bool subset = false;
	  if(compare_neutron_traj.size() > compare_traj.size()){
	    subset = std::includes(compare_neutron_traj.begin(), compare_neutron_traj.end(), compare_traj.begin(), compare_traj.end());
	  }
	  else{
	    subset = std::includes(compare_traj.begin(), compare_traj.end(), compare_neutron_traj.begin(), compare_neutron_traj.end());
	  }
	  
	  // ******************************************
	  // If the neutron trajectory is part of the trajectory
	  // need to use the next ID after the neutron has ended
	  // i.e. if have neutron_traj = 0 1 7, traj = 0 1 7 13 19
	  // want to get ID 13 and see what the PDG of that ID is
	  int nucleus_pdg = -999;
	  int nucleus_id = -999;
	  if(subset){
	    if(compare_neutron_traj.size() < compare_traj.size()){
	      int size = (int) compare_neutron_traj.size();
	      nucleus_id = compare_traj[size]; // Don't need to add 1 to size because vector indexing begins at 0  
	      nucleus_pdg = GetPDG(wcsimrootevent, nucleus_id);
	    }
	    else{ // Shouldn't really enter this loop, based on my logic...
	      int size = (int) compare_traj.size();
	      nucleus_id = compare_neutron_traj[size]; 
	      nucleus_pdg = GetPDG(wcsimrootevent, nucleus_id);
	    }
	    
	    // Check have a valid id. 
	    // Also require nucleus_pdg > 5000 to ensure get the nucleus and not gamma
	    if(nucleus_pdg != -999 && nucleus_pdg > 5000 && nucleus_id != -999){ 
	      // Neutron capture:
	      if(GetProcessName(wcsimrootevent, nucleus_id) == "nCapture"){
		nCaptures++; 
		captureNucleus[nucleus_pdg]++;
		nCapturesFile++;
		captureNucleusFile[nucleus_pdg]++;
		eventCaptureMap[ev]++;
		break;
	      }
	      // Neutron stops existing in some other elastic / inelastic interaction
	      //  - save all the particles involved
	      else{
		nonCaptureNucleusTraj.push_back(nucleus_pdg);

		// Do not break out of this loop, as want to 
		// find all particles produced in interaction (whereas for
		// the neutron capture, know that that's it, in terms of other particles
		// other than gammas)
	      }
	    }

	  }
  
	} // j
	
	// Need to save the non-capture nuclei for this specific neutron
	// Sort the vector in terms of size
	// and then add together as a string.
	std::sort(nonCaptureNucleusTraj.begin(), nonCaptureNucleusTraj.end(), std::less<int>());      
	std::string particle_list = "";
	for(unsigned int t = 0; t < nonCaptureNucleusTraj.size(); t++){
	  std::stringstream sstream;
	  sstream << nonCaptureNucleusTraj[t]; 
	  std::string particle = sstream.str();
	  particle_list += particle;
	  particle_list += "-";
	}
	if(particle_list != ""){
	  nNonCaptures++;
	  nNonCapturesFile++;
	  nonCaptureNucleus[particle_list]++;
	  nonCaptureNucleusFile[particle_list]++;
	}
	
      } // i
      if(nCaptures != 0){nEventsWithAtLeastOneCapture++;}


      if(verbose){
	std::cout<<"\n ******** Stats ********"<<std::endl;
	std::cout<<"  Number of neutrons = "<<nNeutrons<<std::endl;
	std::cout<<"  Number of neutrons captured = "<<nCaptures<<std::endl;
	std::cout<<"   - Capture nucleus summary:"<<std::endl;                   
	for(std::map<int,int>::iterator iter = captureNucleus.begin();
	    iter != captureNucleus.end(); iter++){
	  std::cout<<"      pdg "<<iter->first<<": "<<iter->second<<std::endl;
	}   
	std::cout<<"  Number of neutrons non-captured = "<<nNonCaptures<<std::endl;
	std::cout<<"  (but undergo another elastic/inelastic process involving nuclei)"<<std::endl;
	std::cout<<"   - Non-capture nuclei summary:"<<std::endl;
	for(std::map<std::string,int>::iterator iter = nonCaptureNucleus.begin();
	    iter != nonCaptureNucleus.end(); iter++){
	  std::cout<<"      pdgs "<<iter->first<<": "<<iter->second<<std::endl;
	}
      }        

      // Sanity check:
      if(nCaptures > nNeutrons){std::cout<<"ERROR: nCaptures > nNeutrons"<<std::endl; exit(1);}

    } // Trigger

  } // Event
  
  if(verbose){
    std::cout<<"\n\n**************************************************"<<std::endl;
    std::cout<<"**************** Whole file stats ****************"<<std::endl;
    std::cout<<"**************************************************"<<std::endl;
    std::cout<<"  Number of neutrons = "<<nNeutronsFile<<std::endl;
    
    std::cout<<"  Number of neutrons captured = "<<nCapturesFile
	     <<" ("<<100.0 * (double) nCapturesFile / (double) nNeutronsFile<<"%)"<<std::endl;
    std::cout<<"   - Capture nucleus summary:"<<std::endl;                   
    for(std::map<int,int>::iterator iter = captureNucleusFile.begin();
	iter != captureNucleusFile.end(); iter++){
      std::cout<<"      pdg "<<iter->first<<": "<<iter->second<<std::endl;
    }   
    
    std::cout<<"  Number of neutrons not captured = "<<nNonCapturesFile
	     <<" ("<<100.0 * (double) nNonCapturesFile / (double) nNeutronsFile<<"%)"<<std::endl;
    std::cout<<"  (but undergo another elastic/inelastic process involving nuclei)"<<std::endl;
    std::cout<<"   - Non-capture nuclei summary:"<<std::endl;
    for(std::map<std::string,int>::iterator iter = nonCaptureNucleusFile.begin();
	iter != nonCaptureNucleusFile.end(); iter++){
      std::cout<<"     pdgs "<<iter->first<<": "<<iter->second<<std::endl;
    }

    std::cout<<"**************************************************"<<std::endl;
    std::cout<<"  Number of events with at least 1 neutron = "<<nEventsWithAtLeastOneNeutron<<std::endl;
    std::cout<<"  Number of events with at least 1 capture = "<<nEventsWithAtLeastOneCapture
	     <<" ("<<100.0 * (double) nEventsWithAtLeastOneCapture / (double) nEventsWithAtLeastOneNeutron<<"%)"<<std::endl;

    std::cout<<"**************************************************"<<std::endl;
    std::cout<<"  Events with neutrons but no captures: ";
    for(std::map<int,int>:: iterator iter = eventCaptureMap.begin();
	iter != eventCaptureMap.end(); iter++){
      if(iter->second == 0){std::cout<<iter->first<<", ";}
    }
    std::cout<<""<<std::endl;
    std::cout<<"**************************************************"<<std::endl;
    std::cout<<"**************************************************"<<std::endl;
  }    
  
  //  0, 6, 9, 11, 12, 13, 15, 16, 18, 19, 21, 24, 25, 
  //  26, 31, 32, 33, 35, 36, 39, 40, 43, 49, 56, 57, 59, 
  //  60, 64, 67, 68, 71, 74, 75, 79, 81, 83, 84, 87, 90, 91, 92, 93, 96, 97, 98, 99,
  
  return 0;

}


// Function to pass command line arguments to the int main()
void ParseArgs(int argc, char **argv){
  int nargs = 1;
  if(argc<(nargs*2+1)){exit(1);}
     for(int i = 1; i < argc; i+=2){
       if(std::string(argv[i]) == "-f"){gFilename = argv[i+1];}
       else if(std::string(argv[i]) == "-v"){gVerbose = argv[i+1];}
       else{
	 std::cout<<"ERROR: invalid argument:"<<argv[i]<<" "<<argv[i+1]<<std::endl;
	 exit(1);
       }
     }
     return;
}


std::vector<int> GetAllTrackIDsInEvent(WCSimRootTrigger* wcsimrootevent){

  std::vector<int> ids;
  
  // Get all IDs
  for(int i = 0; i < wcsimrootevent->GetNtrack(); i++){
    WCSimRootTrack* wcsimroottrack = dynamic_cast<WCSimRootTrack*>((wcsimrootevent->GetTracks())->At(i));
    ids.push_back(wcsimroottrack->GetId());
  }

  // Sort all IDs in descending order
  std::sort(ids.begin(), ids.end(), std::greater<int>());

  return ids;
}


int GetParentID(WCSimRootTrigger* wcsimrootevent, int trackID){

  int parentID = -999;

  for(int track = 0; track < wcsimrootevent->GetNtrack(); track++){

    WCSimRootTrack* wcsimroottrack = dynamic_cast<WCSimRootTrack*>((wcsimrootevent->GetTracks())->At(track));
    int curr_id = wcsimroottrack->GetId();

    if(curr_id == trackID){
      parentID = wcsimroottrack->GetParentId();
      return parentID;
    }

  }

  return parentID;
}

int GetParentPDG(WCSimRootTrigger* wcsimrootevent, int trackID){

  int parentPDG = -999;

  for(int track = 0; track < wcsimrootevent->GetNtrack(); track++){

    WCSimRootTrack* wcsimroottrack = dynamic_cast<WCSimRootTrack*>((wcsimrootevent->GetTracks())->At(track));
    int curr_id = wcsimroottrack->GetId();

    if(curr_id == trackID){
      parentPDG = wcsimroottrack->GetParenttype();
      return parentPDG;
    }

  }

  return parentPDG;
}


int GetPDG(WCSimRootTrigger* wcsimrootevent, int trackID){

  int pdg = -999;

  for(int track = 0; track < wcsimrootevent->GetNtrack(); track++){

    WCSimRootTrack* wcsimroottrack = dynamic_cast<WCSimRootTrack*>((wcsimrootevent->GetTracks())->At(track));
    int curr_id = wcsimroottrack->GetId();

    if(curr_id == trackID){
      pdg = wcsimroottrack->GetIpnu();
      return pdg;
    }

  }

  return pdg;
}

std::string GetProcessName(WCSimRootTrigger* wcsimrootevent, int trackID){

  std::string processName = "none";

  for(int track = 0; track < wcsimrootevent->GetNtrack(); track++){

    WCSimRootTrack* wcsimroottrack = dynamic_cast<WCSimRootTrack*>((wcsimrootevent->GetTracks())->At(track));
    int curr_id = wcsimroottrack->GetId();

    if(curr_id == trackID){
      processName = wcsimroottrack->GetProcessName();
      return processName;
    }

  }

  return processName;
}


void MatchTracksInGivenTrajectory(WCSimRootTrigger* wcsimrootevent, int curr_id, std::vector<int>& chain){

  // Avoid vectors made of zeros
  if(chain.size() == 1){
    if(chain[0] == 0){return;}
  }

  int parentID = GetParentID(wcsimrootevent, curr_id);
  chain.push_back(parentID);

  if(parentID == -999 || parentID == 0){
    return;
  }
  else{
    MatchTracksInGivenTrajectory(wcsimrootevent, parentID, chain);
  }

  return;
}


std::vector< std::vector<int> > MatchTracks(WCSimRootTrigger* wcsimrootevent, bool neutrons_only, bool verbose){

  std::vector< std::vector<int> > tracks;

  // Get all the IDs in the event
  std::vector<int> ids = GetAllTrackIDsInEvent(wcsimrootevent);    
  if(ids.size() == 0){return tracks;}

  // Loop over IDs and find parents and grandparents, etc
  // (ids are ordered from big -> small numbers)
  for(unsigned int i = 0; i < ids.size(); i++){
    int curr_id = ids[i];
    int curr_pdg = GetPDG(wcsimrootevent, curr_id);

    // If only interested in neutrons,
    // skip if not a neutron or unknown PDG
    if(neutrons_only && (curr_pdg != 0 && curr_pdg != 2112)){continue;}

    std::vector<int> chain;
    chain.push_back(curr_id);

    // Find all the track IDs for a particle trajectory
    MatchTracksInGivenTrajectory(wcsimrootevent, curr_id, chain);

    // Sort it so the ids go from small -> big
    std::sort(chain.begin(), chain.end(), std::less<int>());
    tracks.push_back(chain);
  }

  // Get rid of particle trajectories that are subsets of another particle trajectory
  // Need to do this because the tracks vector will look like:
  //  0, 1, 6, 2078, 
  //  0, 1, 6, 2077, 
  //  0, 1, 2, 67, 
  //  0, 1, 2, 65, 
  //  0, 1, 9, 
  //  0, 1, 8, 
  //  0, 1, 6, 
  //  0, 1, 2, 
  //  0, 1, 
  //  0, 
  // And we want it to look like:
  //  0, 1, 6, 2078, 
  //  0, 1, 6, 2077, 
  //  0, 1, 2, 67, 
  //  0, 1, 2, 65, 
  //  0, 1, 9, 
  //  0, 1, 8, 
  std::vector< std::vector<int> > non_repeating_tracks = tracks;
  for(unsigned int i = 0; i < tracks.size(); i++){
    std::vector<int> compare_i = tracks[i];

    for(unsigned int j = 0; j < tracks.size(); j++){
      if(j <= i){continue;} // avoid double counting
      
      std::vector<int> compare_j = tracks[j];

      // Is this vector a subset of another vector?
      // If yes, remove it
      bool subset = false;
      if(compare_i.size() > compare_j.size()){
	subset = std::includes(compare_i.begin(), compare_i.end(), compare_j.begin(), compare_j.end());
      }
      else{
	subset = std::includes(compare_j.begin(), compare_j.end(), compare_i.begin(), compare_i.end());
      }

      if(subset){

	// Remove the shorter trajectory
	std::vector<int> to_remove; 
	if(compare_i.size() > compare_j.size()){to_remove = compare_j;}
	else{to_remove = compare_i;}

	// Check the vector hasn't already been removed
	// and do the actual removing
	bool remove = false;
	for(unsigned int k = 0; k < non_repeating_tracks.size(); k++){
	  if(non_repeating_tracks[k] == to_remove){remove = true;}
	}
	if(remove){
	  non_repeating_tracks.erase(std::remove(non_repeating_tracks.begin(), non_repeating_tracks.end(), to_remove));
	}

      }

    } // j
  } // i

  /*
  std::cout<<"Repeating tracks: "<<std::endl;
  for(unsigned int i = 0; i < tracks.size(); i++){
    std::cout<<"Track "<<i<<" contains: ";
    for(unsigned int j = 0; j < tracks[i].size(); j++){
      std::cout<<(tracks[i])[j]<<", ";
    }
    std::cout<<""<<std::endl;
  }
  */
  
  //std::cout<<"NON-Repeating tracks: "<<std::endl;
  if(verbose){
    for(unsigned int i = 0; i < non_repeating_tracks.size(); i++){
      std::cout<<"    - Track "<<i<<" contains: ";
      for(unsigned int j = 0; j < non_repeating_tracks[i].size(); j++){
	std::cout<<(non_repeating_tracks[i])[j]<<", ";
      }
      std::cout<<""<<std::endl;
    }
  }

  return non_repeating_tracks;
}
