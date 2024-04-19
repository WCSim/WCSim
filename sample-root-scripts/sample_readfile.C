#include <iostream>
#include <stdio.h>     
#include <stdlib.h>

#include "TTree.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TString.h"

#include "WCSimRootOptions.hh"
#include "WCSimRootGeom.hh"
#include "WCSimRootEvent.hh"

// Only access the photon history class when -DWCSim_DEBUG_COMPILE_FLAG=ON is defined in compilation
//#define WCSIM_SAVE_PHOTON_HISTORY

// Simple example of reading a generated Root file
int sample_readfile(const char *filename="../wcsim.root", TString events_tree_name="wcsimrootevent", const int verbose=0)
{
  // Clear global scope
  //gROOT->Reset();

  if(!events_tree_name.EqualTo("wcsimrootevent") &&
     !events_tree_name.EqualTo("wcsimrootevent2") &&
     !events_tree_name.EqualTo("wcsimrootevent_OD")) {
    cerr << "Second argument events_tree_name MUST equal one of: wcsimrootevent wcsimrootevent2 wcsimrootevent_OD" << endl;
    return -1;
  }
  const bool true_tracks_expected = events_tree_name.EqualTo("wcsimrootevent");

  // Open the file
  TFile * file = new TFile(filename,"read");
  if (!file->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    return -1;
  }
  
  // Get the a pointer to the tree from the file
  TTree *tree = (TTree*)file->Get("wcsimT");
  
  // Get the number of events
  const long nevent = tree->GetEntries();
  if(verbose) printf("Number of Event Tree Entries: %ld\n",nevent);
  
  // Create a WCSimRootEvent to put stuff from the tree in
  WCSimRootEvent* wcsimrootsuperevent = new WCSimRootEvent();

  // Set the branch address for reading from the tree
  TBranch *branch = tree->GetBranch(events_tree_name);
  branch->SetAddress(&wcsimrootsuperevent);

  // Force deletion to prevent memory leak 
  tree->GetBranch(events_tree_name)->SetAutoDelete(kTRUE);

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

  // start with the main "subevent", as it contains most of the info
  // and always exists.
  WCSimRootTrigger* wcsimrootevent;

  const float detR = geo->GetWCCylRadius();
  const float detZ = geo->GetWCCylLength();
  TH1F *h1 = new TH1F("h1", "NDigits;NDigits in Trigger 0;Entries in bin", 8000, 0, 8000);
  TH1F *hvtxX = new TH1F("hvtxX", "Event VTX X;True vertex X (cm);Entries in bin", 200, -detR, +detR);
  TH1F *hvtxY = new TH1F("hvtxY", "Event VTX Y;True vertex Y (cm);Entries in bin", 200, -detR, +detR);
  TH1F *hvtxZ = new TH1F("hvtxZ", "Event VTX Z;True vertex Z (cm);Entries in bin", 200, -detZ/2, +detZ/2);
  
  int num_trig=0;
  
  // Now loop over events
  for (long ievent=0; ievent<nevent; ievent++)
  {
    // Read the event from the tree into the WCSimRootEvent instance
    tree->GetEntry(ievent);      
    wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
    if(verbose){
      printf("********************************************************\n");
      printf("Event Number (from loop): %ld\n", ievent);
      printf("Event Number (from WCSimRootEventHeader): %d\n", wcsimrootevent->GetHeader()->GetEvtNum());
      printf("Trigger Time [ns]: %ld\n", wcsimrootevent->GetHeader()->GetDate());
      cout << "Trigger Type: " << wcsimrootevent->GetTriggerType()
           << " " << WCSimEnumerations::EnumAsString(wcsimrootevent->GetTriggerType()) << endl;
      printf("Interaction Nuance Code: %d\n", wcsimrootevent->GetMode());
      printf("Number of Delayed Triggers (sub events): %d\n",
       wcsimrootsuperevent->GetNumberOfSubEvents());

      if(true_tracks_expected) {
	printf("Neutrino Vertex Geometry Volume Code: %d\n", wcsimrootevent->GetVtxvol());
	printf("Neutrino Vertex Location [cm]: %f %f %f\n", wcsimrootevent->GetVtx(0),
	       wcsimrootevent->GetVtx(1),wcsimrootevent->GetVtx(2));
	printf("Index of muon in WCSimRootTracks %d\n", wcsimrootevent->GetJmu());
	printf("Number of final state particles %d\n", wcsimrootevent->GetNpar());
      }
    }
    hvtxX->Fill(wcsimrootevent->GetVtx(0));
    hvtxY->Fill(wcsimrootevent->GetVtx(1));
    hvtxZ->Fill(wcsimrootevent->GetVtx(2));

    // Now read the tracks in the event
    
    // Get the number of tracks
    const int ntrack = wcsimrootevent->GetNtrack();
    const int ntrack_slots = wcsimrootevent->GetNtrack_slots();
    if(verbose) {
      cout << "SAVED TRACKS" << endl
	   << "Number of Saved WCSimRootTracks: " << ntrack << endl;
      if(!true_tracks_expected)
	cout << "No saved true tracks in branch: " << events_tree_name
	     << ". You can find them in: wcsimrootevent" << endl;
    }
    if(ntrack && !true_tracks_expected) {
      cerr << ntrack << " true tracks found in branch: " << events_tree_name
	   << ". There should be none here. Don't trust them. You can find them in: wcsimrootevent" << endl;
    }
      
    // Loop through elements in the TClonesArray of WCSimTracks
    for (int itrack=0; itrack<ntrack_slots; itrack++)
    {
      TObject *element = (wcsimrootevent->GetTracks())->At(itrack);
      if(!element)
	continue;
      WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);

      if(verbose > 1){
        cout<<"Track: "<<itrack<<endl << "  ";
        int trackflag = wcsimroottrack->GetFlag();
        if(trackflag==-1) cout<<"Primary neutrino track"<<endl;
        else if(trackflag==-2) cout<<"Neutrino target nucleus track"<<endl;
        else cout<<"Final state particle track"<<endl;
        printf("  Track ipnu (PDG code): %d\n",wcsimroottrack->GetIpnu());
        printf("  PDG code of parent particle (0 for primary, 999 for parent PDG code not found in the specific logic used): %d\n",wcsimroottrack->GetParenttype());
	printf("  Track ID of parent particle (0 for primary): %d\n",wcsimroottrack->GetParentId());
            
        cout<<"  Track initial dir [unit 3-vector]: ("
            <<wcsimroottrack->GetDir(0)<<", "
            <<wcsimroottrack->GetDir(1)<<", "
            <<wcsimroottrack->GetDir(2)<<")"<<endl;
        printf("  Track initial relativistic energy [MeV]: %f\n", wcsimroottrack->GetE());
        printf("  Track initial momentum magnitude [MeV/c]: %f\n", wcsimroottrack->GetP());
        printf("  Track mass [MeV/c2]: %f\n", wcsimroottrack->GetM());
        printf("  Track ID: %d\n", wcsimroottrack->GetId());
        printf("  Number of ID/OD crossings: %zu\n", wcsimroottrack->GetBoundaryPoints().size());
        if (wcsimroottrack->GetBoundaryPoints().size()>0)
          printf("  First crossing position [mm]: (%f %f %f), KE [MeV]: %f, time [ns]: %f, type: %d\n", 
                  wcsimroottrack->GetBoundaryPoints().at(0).at(0),
                  wcsimroottrack->GetBoundaryPoints().at(0).at(1),
                  wcsimroottrack->GetBoundaryPoints().at(0).at(2),
                  wcsimroottrack->GetBoundaryKEs().at(0),
                  wcsimroottrack->GetBoundaryTimes().at(0),
                  wcsimroottrack->GetBoundaryTypes().at(0)
                );
      }//verbose
    }  // itrack // End of loop over tracks
    
    // Now look at the Cherenkov hits
    
    // Get the number of Cherenkov hits.
    // Note... this is *NOT* the number of photons that hit tubes.
    // It is the number of tubes hit with Cherenkov photons.
    // The number of digitized tubes will be smaller because of the threshold.
    // Each hit "raw" tube has several photon hits.  The times are recorded.
    // See chapter 5 of ../doc/DetectorDocumentation.pdf
    // for more information on the structure of the root file.
    //  
    // The following code prints out the hit times for the first 10 tubes and also
    // adds up the total pe.
    // 
    // For digitized info (one time/charge tube after a trigger) use
    // the digitized information.
    //

    int ncherenkovhits     = wcsimrootevent->GetNcherenkovhits();
    int ncherenkovdigihits = wcsimrootevent->GetNcherenkovdigihits(); 
    
    h1->Fill(ncherenkovdigihits);
    if(verbose){
      cout << "RAW HITS:" << endl;
      printf("Number of PMTs with a true hit %d\n",     ncherenkovhits);
    }

    // Grab the big arrays of times and parent IDs
    TClonesArray *timeArray = wcsimrootevent->GetCherenkovHitTimes();
#ifdef WCSIM_SAVE_PHOTON_HISTORY
    TClonesArray *historyArray = wcsimrootevent->GetCherenkovHitHistories(); // scattering and reflection history
#endif

    int totalPe = 0;
    // Loop through elements in the TClonesArray of WCSimRootCherenkovHits
    for (int itruepmt=0; itruepmt < ncherenkovhits; itruepmt++)
    {
      TObject *Hit = (wcsimrootevent->GetCherenkovHits())->At(itruepmt);
      WCSimRootCherenkovHit *wcsimrootcherenkovhit = 
	dynamic_cast<WCSimRootCherenkovHit*>(Hit);

      int tubeNumber     = wcsimrootcherenkovhit->GetTubeID();
      int timeArrayIndex = wcsimrootcherenkovhit->GetTotalPe(0);
      int peForTube      = wcsimrootcherenkovhit->GetTotalPe(1);
      WCSimRootPMT pmt   = geo->GetPMT(tubeNumber-1);
      totalPe += peForTube;
     
      if ( itruepmt < 10 ) { // Only print first XX=10 tubes
	if(verbose > 1) printf("photon hits on tube %d : %d, times: ( ",tubeNumber,peForTube);
	for (int itruehit = timeArrayIndex; itruehit < timeArrayIndex + peForTube; itruehit++) {
	  WCSimRootCherenkovHitTime* HitTime = 
	    dynamic_cast<WCSimRootCherenkovHitTime*>(timeArray->At(itruehit));
    
	  if(verbose > 1) printf("%6.2f ", HitTime->GetTruetime() );
	}//itruehit
	if(verbose > 1) cout << ")" << endl;
      }//itruepmt < 10

    } // itruepmt // End of loop over Cherenkov hits
    if(verbose) cout << "Total p.e. on all tubes: " << totalPe << endl
		     << " average of " << (double)totalPe/(double)ncherenkovhits << " p.e. per tube" << endl;
    
    // Look at digitized hit info
    // Loop over sub events
    if(verbose) cout << "DIGITIZED HITS:" << endl;
    for (int itrigger = 0 ; itrigger < wcsimrootsuperevent->GetNumberOfEvents(); itrigger++) 
    {
      wcsimrootevent = wcsimrootsuperevent->GetTrigger(itrigger);
      if(verbose) cout << "Sub event number = " << itrigger << "\n";
      
      int ncherenkovdigihits = wcsimrootevent->GetNcherenkovdigihits();
      if(verbose) printf("Number of digits in sub-event: %d\n", ncherenkovdigihits);
      int ncherenkovdigihits_slots = wcsimrootevent->GetNcherenkovdigihits_slots();
      if(ncherenkovdigihits>0)
	num_trig++;

      int idigifound = 0;
      for (int idigi=0;idigi<ncherenkovdigihits_slots;idigi++)
      {
        // Loop through elements in the TClonesArray of WCSimRootCherenkovDigHits
        TObject *element = (wcsimrootevent->GetCherenkovDigiHits())->At(idigi);
	if(!element) continue;
	idigifound++;
    
        WCSimRootCherenkovDigiHit *wcsimrootcherenkovdigihit = 
          dynamic_cast<WCSimRootCherenkovDigiHit*>(element);
        
        if(verbose > 1){
          if ( idigi < 10 ){ // Only print first XX=10 tubes
            printf("idigi, q [p.e.], time+950 [ns], tubeid: %d %f %f %d \n",idigi,wcsimrootcherenkovdigihit->GetQ(),
              wcsimrootcherenkovdigihit->GetT(),wcsimrootcherenkovdigihit->GetTubeId());
            
            // print the parents of each photon in the digit
	    // retrieve the indices of the photons in this digit within the HitTimes array
	    std::vector<int> photonids=wcsimrootcherenkovdigihit->GetPhotonIds();
	    // loop over photons within the digit
	    int photonid=0;
	    for(auto thephotonsid : photonids){
	      WCSimRootCherenkovHitTime *thehittimeobject = 
		dynamic_cast<WCSimRootCherenkovHitTime*>(timeArray->At(thephotonsid));
	      if(thehittimeobject){
		cout<<"  digit "<<idigi<<" photon "<<photonid<<": ";
		cout<<" HitTime index "<<thephotonsid<<", pre-smear time "<<thehittimeobject->GetTruetime()
		    <<", parent TrackID: "<<thehittimeobject->GetParentID()<<";";
	      }
#ifdef WCSIM_SAVE_PHOTON_HISTORY
	      // use the same index as WCSimRootCherenkovHitTime
	      WCSimRootCherenkovHitHistory *thehithistoryobject =  dynamic_cast<WCSimRootCherenkovHitHistory*>(historyArray->At(thephotonsid));
	      if(thehithistoryobject) {
		// Number of scattering, and types of reflection surface (WCSimEnumerations)
		cout<<" Rayleigh Scattering: "<<thehithistoryobject->GetNRayScatters()<<", Mie Scattering: "<<thehithistoryobject->GetNMieScatters()<<", Reflection: ";
		for (auto r: thehithistoryobject->GetReflectionSurfaces()) cout<<WCSimEnumerations::EnumAsString(r)<<" ";
		cout<<";";
	      }
#endif
	      cout<<endl;
	      photonid++;
	    } // end loop over photons in digit
          } // end test for first 10 tubes
        } // end verbosity check
      } // idigi // End of loop over Cherenkov digihits
      if(verbose || idigifound != ncherenkovdigihits)
	cout << idigifound << " digits found; expected " << ncherenkovdigihits << endl;
    } // itrigger // End of loop over triggers
    
    // reinitialize super event between loops.
    wcsimrootsuperevent->ReInitialize();
    
  } // ievent // End of loop over events

  float win_scale = 0.75;
  int n_wide(2);
  int n_high(2);
  TCanvas* c1 = new TCanvas("c1", "First canvas", 500*n_wide*win_scale, 500*n_high*win_scale);
  c1->Draw();
  c1->Divide(2,2);
  c1->cd(1); hvtxX->Draw();
  c1->cd(2); hvtxY->Draw();
  c1->cd(3); hvtxZ->Draw();
  c1->cd(4); h1->Draw();
  
  cout << "********************" << endl
       << num_trig << " triggers found with at least one digitised hit" << endl
       << " when run over " << nevent << " events" << endl
       << " giving average of " << (double)num_trig / (double)nevent << " triggers per event" << endl;

  return 0;
}
