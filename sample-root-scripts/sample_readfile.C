#include <iostream>
#include <TH1F.h>
#include <stdio.h>     
#include <stdlib.h>    
// Simple example of reading a generated Root file
void sample_readfile(char *filename=NULL, bool verbose=false)
{
  // Clear global scope
  //gROOT->Reset();
  /*
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetTitleColor(1);
  gStyle->SetStatColor(0);
  gStyle->SetFrameFillColor(0);
  gStyle->SetPadColor(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetTitleSize(0.04);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);
  gStyle->SetTitleAlign(23);
  gStyle->SetTitleX(.5);
  gStyle->SetTitleY(0.99);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetHatchesLineWidth(2);
  gStyle->SetLineWidth(1.5);
  gStyle->SetTitleFontSize(0.07);
  gStyle->SetLabelSize(0.05,"X");
  gStyle->SetLabelSize(0.05,"Y");
  gStyle->SetTitleSize(0.04,"X");
  gStyle->SetTitleSize(0.04,"Y");
  gStyle->SetTitleBorderSize(0);
  gStyle->SetCanvasBorderMode(0);
  */
  // Load the library with class dictionary info
  // (create with "gmake shared")
  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIMDIR");
  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
  }else{
    gSystem->Load("../libWCSimRoot.so");
  }

  TFile *file;
  // Open the file
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
  TTree *tree = (TTree*)file->Get("wcsimT");
  
  // Get the number of events
  int nevent = tree->GetEntries();
  if(verbose) printf("Number of Tree Entries or Events: %d\n",nevent);
  
  // Create a WCSimRootEvent to put stuff from the tree in

  WCSimRootEvent* wcsimrootsuperevent = new WCSimRootEvent();

  // Set the branch address for reading from the tree
  TBranch *branch = tree->GetBranch("wcsimrootevent");
  branch->SetAddress(&wcsimrootsuperevent);

  // Force deletion to prevent memory leak 
  tree->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);


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
  if(verbose) std::cout << "Options tree has: " << opttree->GetEntries() << " entries" << std::endl;
  if (opttree->GetEntries() == 0) {
    exit(9);
  }
  opttree->GetEntry(0);
  opt->Print();

  // start with the main "subevent", as it contains most of the info
  // and always exists.
  WCSimRootTrigger* wcsimrootevent;

  TH1F *h1 = new TH1F("PMT Hits", "PMT Hits", 8000, 0, 8000);
  TH1F *hvtx0 = new TH1F("Event VTX0", "Event VTX0", 200, -1500, 1500);
  TH1F *hvtx1 = new TH1F("Event VTX1", "Event VTX1", 200, -1500, 1500);
  TH1F *hvtx2 = new TH1F("Event VTX2", "Event VTX2", 200, -1500, 1500);
  
  int num_trig=0;
  
  // Now loop over events
  for (int ev=0; ev<nevent; ev++)
  {
    // Read the event from the tree into the WCSimRootEvent instance
    tree->GetEntry(ev);      
    wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
    if(verbose){
      printf("********************************************************");
      printf("Event Number, Trigger Time [ns]: %d %d\n", wcsimrootevent->GetHeader()->GetEvtNum(),
       wcsimrootevent->GetHeader()->GetDate());
      printf("Interaction Nuance Code: %d\n", wcsimrootevent->GetMode());
      printf("Number of Delayed Triggers (sub events): %d\n",
       wcsimrootsuperevent->GetNumberOfSubEvents());
      
      printf("Neutrino Vertex Geometry Volume Code: %d\n", wcsimrootevent->GetVtxvol());
      printf("Neutrino Vertex Location [cm]: %f %f %f\n", wcsimrootevent->GetVtx(0),
       wcsimrootevent->GetVtx(1),wcsimrootevent->GetVtx(2));
    }
    hvtx0->Fill(wcsimrootevent->GetVtx(0));
    hvtx1->Fill(wcsimrootevent->GetVtx(1));
    hvtx2->Fill(wcsimrootevent->GetVtx(2));

    if(verbose){
      printf("Index of muon in WCSimRootTracks %d\n", wcsimrootevent->GetJmu());
      printf("Number of final state particles %d\n", wcsimrootevent->GetNpar());
      printf("Number of Saved WCSimRootTracks %d\n", wcsimrootevent->GetNtrack());
    }
    // Now read the tracks in the event
    
    // Get the number of tracks
    int ntrack = wcsimrootevent->GetNtrack();
    
    int i;
    // Loop through elements in the TClonesArray of WCSimTracks
    cout<<"Recorded Tracks:"<<endl;
    for (i=0; i<ntrack; i++)
    {
      TObject *element = (wcsimrootevent->GetTracks())->At(i);
      
      WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);

      if(verbose){
        cout<<"Track: "<<i<<endl;
        int trackflag = wcsimroottrack->GetFlag();
        if(trackflag==-1) cout<<"  Primary neutrino track"<<endl;
        else if(trackflag==-2) cout<<"Neutrino target nucleus track"<<endl;
        else cout<<"Final state particle track"<<endl;
        printf("  Track ipnu (PDG code): %d\n",wcsimroottrack->GetIpnu());
        printf("  PDG code of parent particle (0 for primary): %d\n",wcsimroottrack->GetParenttype());
            
        cout<<"  Track initial dir [unit 3-vector]: ("
            <<wcsimroottrack->GetDir(0)<<", "
            <<wcsimroottrack->GetDir(1)<<", "
            <<wcsimroottrack->GetDir(2)<<")"<<endl;
        printf("  Track initial relativistic energy [MeV]: %f\n", wcsimroottrack->GetE());
        printf("  Track initial momentum magnitude [MeV/c]: %f\n", wcsimroottrack->GetP());
        printf("  Track mass [MeV/c2]: %f\n", wcsimroottrack->GetM());
        printf("  Track ID: %d\n", wcsimroottrack->GetId());
      }

      
    }  // End of loop over tracks
    
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
      printf("Event ID: %i\n", ev);
      printf("Number of Detected Photons %d\n",     ncherenkovhits);
      printf("Number of Digits %d\n", ncherenkovdigihits);
      cout << "RAW HITS:" << endl;
    }

    // Grab the big arrays of times and parent IDs
    TClonesArray *timeArray = wcsimrootevent->GetCherenkovHitTimes();
    
    int totalPe = 0;
    // Loop through elements in the TClonesArray of WCSimRootCherenkovHits
    for (i=0; i< ncherenkovhits; i++)
    {
      TObject *Hit = (wcsimrootevent->GetCherenkovHits())->At(i);
      WCSimRootCherenkovHit *wcsimrootcherenkovhit = 
  dynamic_cast<WCSimRootCherenkovHit*>(Hit);

      int tubeNumber     = wcsimrootcherenkovhit->GetTubeID();
      int timeArrayIndex = wcsimrootcherenkovhit->GetTotalPe(0);
      int peForTube      = wcsimrootcherenkovhit->GetTotalPe(1);
      WCSimRootPMT pmt   = geo->GetPMT(tubeNumber-1);
      totalPe += peForTube;

     
      if ( i < 10 ) // Only print first XX=10 tubes
      {
  if(verbose) printf("photon hits on tube %d : %d, times: ( ",tubeNumber,peForTube);
  for (int j = timeArrayIndex; j < timeArrayIndex + peForTube; j++)
  {
    WCSimRootCherenkovHitTime* HitTime = 
      dynamic_cast<WCSimRootCherenkovHitTime*>(timeArray->At(j));
    
    if(verbose) printf("%6.2f ", HitTime->GetTruetime() );       
  }
  if(verbose) cout << ")" << endl;
      }

    } // End of loop over Cherenkov hits
    if(verbose) cout << "Total Pe on all tubes: " << totalPe << endl;
    
    // Look at digitized hit info

    // Get the number of digitized hits
    // Loop over sub events
   
    if(verbose) cout << "DIGITIZED HITS:" << endl;
    for (int index = 0 ; index < wcsimrootsuperevent->GetNumberOfEvents(); index++) 
    {
      wcsimrootevent = wcsimrootsuperevent->GetTrigger(index);
      if(verbose) cout << "Sub event number = " << index << "\n";
      
      int ncherenkovdigihits = wcsimrootevent->GetNcherenkovdigihits();
      if(verbose) printf("Number of digits in sub-event: %d\n", ncherenkovdigihits);
     
      if(ncherenkovdigihits>0)
  num_trig++;
      //for (i=0;i<(ncherenkovdigihits>4 ? 4 : ncherenkovdigihits);i++){
      for (i=0;i<ncherenkovdigihits;i++)
      {
        // Loop through elements in the TClonesArray of WCSimRootCherenkovDigHits
    
        TObject *element = (wcsimrootevent->GetCherenkovDigiHits())->At(i);
    
        WCSimRootCherenkovDigiHit *wcsimrootcherenkovdigihit = 
          dynamic_cast<WCSimRootCherenkovDigiHit*>(element);
        
        if(verbose){
          if ( i < 10 ){ // Only print first XX=10 tubes
            printf("index, q [pe], time+950 [ns], tubeid: %d %f %f %d \n",i,wcsimrootcherenkovdigihit->GetQ(),
              wcsimrootcherenkovdigihit->GetT(),wcsimrootcherenkovdigihit->GetTubeId());
            
            // for first digit, print the parents of each photon in the digit
            if(i<3){
              // retrieve the indices of the photons in this digit within the HitTimes array
              std::vector<int> photonids=wcsimrootcherenkovdigihit->GetPhotonIds();
              // loop over photons within the digit
              int photonid=0;
              for(auto thephotonsid : photonids){
                WCSimRootCherenkovHitTime *thehittimeobject = 
                  dynamic_cast<WCSimRootCherenkovHitTime*>(timeArray->At(thephotonsid));
                if(thehittimeobject){
                  cout<<"  digit "<<i<<" photon "<<photonid<<": ";
                  cout<<" HitTime index "<<thephotonsid<<", pre-smear time "<<thehittimeobject->GetTruetime()
                      <<", parent TrackID: "<<thehittimeobject->GetParentID()<<";";
                }
                cout<<endl;
                photonid++;
              } // end loop over photons in digit
            }   // end test for first 3 digits
          }     // end test for first 10 tubes
        }       // end verbosity check
        
      } // End of loop over Cherenkov digihits
    } // End of loop over trigger
    
    // reinitialize super event between loops.
    wcsimrootsuperevent->ReInitialize();
    
  } // End of loop over events
  //  TCanvas c1("c1"); 
  float win_scale = 0.75;
  int n_wide(2);
  int n_high(2);
  TCanvas* c1 = new TCanvas("c1", "First canvas", 500*n_wide*win_scale, 500*n_high*win_scale);
  c1->Draw();
  c1->Divide(2,2);
  c1->cd(1); hvtx0->Draw();
  c1->cd(2); hvtx1->Draw();
  c1->cd(3); hvtx2->Draw();
  c1->cd(4); h1->Draw();
  
  std::cout<<"num_trig "<<num_trig<<"\n";
}
