#include <iostream>
#include <algorithm>
#include <vector>
#include <stdio.h>     
#include <stdlib.h>    
#include <TH1.h>
#include <TH2.h>
#include <THStack.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TString.h>
#include <TTree.h>
#include <TBranch.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TROOT.h>

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "include/WCSimRootEvent.hh"
#include "include/WCSimRootGeom.hh"
#include "include/WCSimEnumerations.hh"
#endif

using namespace std;

TString create_filename(const char * prefix, TString& filename_string)
{
  //std::cout << "Creating filename from prefix " << prefix << " and filename_string " << filename_string << std::endl;
  TString prefix_string(prefix);
  TString outfilename = prefix_string + filename_string;
  return outfilename;
}

int GLOBALTIME = 0;
int GLOBALTIMEWINDOW = 200;

bool IsInRange(int i) {
  return (i >= GLOBALTIME && i < GLOBALTIME + GLOBALTIMEWINDOW);
}

void integrate_digit_times(vector<double> & digit_times, TH1F * h) {
  if(!digit_times.size())
    return;
  //sort the digit hit times
  std::sort(digit_times.begin(), digit_times.end());
  //the max 'itime' in the loop is the time of the last hit - 200
  // (use -199 to ensure we actually go through the loop at -200)
  int max = digit_times[digit_times.size()-1];
  max -= max % 5;
  max -= 199;
  GLOBALTIMEWINDOW = 200;
  for(int itime = 0; itime < max; itime += 5) {
    GLOBALTIME = itime;
    int counter = std::count_if(digit_times.begin(), digit_times.end(), IsInRange);
    h->Fill(counter);
  }
}


// Simple example of reading a generated Root file
int tom_readfile(char *filename=NULL, bool verbose=false, Long64_t max_nevents = 999999999999, bool create_pdfs = false, bool hists_per_event = false)
{
#if !defined(__MAKECINT__)
  // Load the library with class dictionary info
  // (create with "gmake shared")
  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIMDIR");
  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
  }else{
    gSystem->Load("../libWCSimRoot.so");
  }
#endif

  // Open the input file
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
  TTree *tree = (TTree*)file->Get("wcsimT");
  
  // Get the number of events
  Long64_t nevent = tree->GetEntries();
  printf("nevent %d\n",nevent);
  nevent = TMath::Min(nevent, max_nevents); //cut the loop earlier
 
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
  if(verbose) std::cout << "Geotree has " << geotree->GetEntries() << " entries" << std::endl;
  if (geotree->GetEntries() == 0) {
    cout << "geotree not found!" << endl;
      exit(9);
  }
  geotree->GetEntry(0);

  // start with the main "subevent", as it contains most of the info
  // and always exists.
  WCSimRootTrigger* wcsimrootevent;

  //book histograms
  TH1F *h1 = new TH1F("h1", "PMT Hits", 8000, 0, 8000);
  TH1F *hvtx0 = new TH1F("hvtx0", "Event VTX0", 200, -1500, 1500);
  TH1F *hvtx1 = new TH1F("hvtx1", "Event VTX1", 200, -1500, 1500);
  TH1F *hvtx2 = new TH1F("hvtx2", "Event VTX2", 200, -1500, 1500);

  TH1I *h1triggertype = new TH1I("h1triggertype", "Trigger type;Trigger type;Entries", 10, 0, 10);
  for(int i = -1; i <= kTriggerNHitsTest; i++)
    h1triggertype->GetXaxis()->SetBinLabel(i+2, WCSimEnumerations::EnumAsString((TriggerType_t)i).c_str());

  TH2I *h2nhits = new TH2I("h2nhits", "NDigits from subevent window vs NDigits from 200nsec trigger window;NDigits saved in subevent;NDigits in 200nsec window", 1001, -0.5, 1000.5, 1001, -0.5, 1000.5);
  TH1I *h1nhits = new TH1I("h1nhits", "NDigits in the subevent window;NDigits;Entries", 10001, -0.5, 10000.5);
  TH1I *h1nhitstrigger = new TH1I("h1nhitstrigger", "NDigits in the trigger window;NDigits;Entries", 10001, -0.5, 10000.5);
  TH1F *h1pe = new TH1F("h1pe", "Total p.e. in the subevent window;Total p.e.;Entries", 10001, -0.5, 10000.5);
  TH1F *h1time = new TH1F("h1time", "Digit time;Digit time (ns);Entries", 1350, -400, 9500);
  TH1F *h1time_noise = new TH1F("h1time_noise", "Digit time (digits from noise hits only);Digit time (ns);Entries", 1350, -400, 9500);
  TH1F *h1time_photon = new TH1F("h1time_photon", "Digit time (digits from photon hits only);Digit time (ns);Entries", 1350, -400, 9500);
  TH1F *h1time_mix = new TH1F("h1time_mix", "Digit time (digits from a mix of noise and photon hits);Digit time (ns);Entries", 1350, -400, 9500);
  THStack *hStime = new THStack("hStime", "Digit time;Digits time (ns);Entries");
  h1time_noise->SetLineColor(kBlack);
  h1time_photon->SetLineColor(kRed);
  h1time_mix->SetLineColor(kCyan);
  hStime->Add(h1time_noise);
  hStime->Add(h1time_photon);
  hStime->Add(h1time_mix);
  TH1F *h1time2 = new TH1F("h1time2", "Digit time + trigger time;Digit time + trigger time (ns);Entries", 1350, -400, 9500);
  TH1F *h1peperdigi = new TH1F("h1peperdigi", "Total p.e. in the subevent window;Total p.e. / NDigits;Entries", 1001, -0.5, 1000.5);
  TH1F *h1timeperdigi = new TH1F("h1timeperdigi", "Hit time (relative to trigger time);(Hit time - trigger time) / NDigits (ns);Entries", 1350, -400, 9500);

  TH2F *h2nhits_sep = new TH2F("h2nhits_sep", "NDigits from subevent window vs fraction that are noise;NDigits;Noise fraction", 1001, -0.5, 1000.5, 100, 0, 1);
  TH2F *h2nhitstrigger_sep = new TH2F("h2nhitstrigger_sep", "NDigits from 200ns trigger window vs fraction that are noise;NDigits;Noise fraction", 1001, -0.5, 1000.5, 100, 0, 1);
  TH1F *h1noisefrac = new TH1F("h1noisefrac", "Fraction of hits in this digi that are noise;Noise fraction;Entries", 110,0,1.1);
  TH1F *h1noisefrac_trigger = new TH1F("h1noisefrac_trigger", "Fraction of hits in this trigger that are noise;Noise fraction;Entries", 110,0,1.1);
  TH1F *h1triggertime = new TH1F("h1triggertime", "Time of trigger;Trigger time;Entries", 1000,0,1000);
  TH1F *h1inttime = new TH1F("h1inttime", "Number of digits in a sliding 200nsec window;NDigits in 200nsec window;Entries", 1000,0,1000);
  TH1F *h1inttimenoise = new TH1F("h1inttimenoise", "Number of noise digits in a sliding 200nsec window;NDigits in 200nsec window;Entries", 1000,0,1000);
  TH1F *h1inttimephoton = new TH1F("h1inttimephoton", "Number of photon digits in a sliding 200nsec window;NDigits in 200nsec window;Entries", 1000,0,1000);
  TH1F *h1inttimemix = new TH1F("h1inttimemix", "Number of mixed (noise+photon) digits in a sliding 200nsec window;NDigits in 200nsec window;Entries", 1000,0,1000);

  TH1F *h1eventALL_hittime[2];
  h1eventALL_hittime[0] = new TH1F("h1eventALL_hittime_photon", "Raw hit time for photons, event ALL;Hit time (ns);Entries",    1000, 0, 1000);
  h1eventALL_hittime[1] = new TH1F("h1eventALL_hittime_noise",  "Raw hit time for dark noise, event ALL;Hit time (ns);Entries", 1000, 0, 1000);
  h1eventALL_hittime[0]->SetLineColor(kBlue);
  //h1eventALL_hittime[0]->SetFillColor(kBlue);
  h1eventALL_hittime[1]->SetLineColor(kRed);
  //h1eventALL_hittime[1]->SetFillColor(kRed);
  THStack *hSeventALL_hittime = new THStack("hSeventALL_hittime", "Raw hit time, event ALL;Hit time (ns);Entries");
  hSeventALL_hittime->Add(h1eventALL_hittime[1]);
  hSeventALL_hittime->Add(h1eventALL_hittime[0]);

  //hit times per event
  TH1F *h1event_hittime[nevent][2];
  THStack *hSevent_hittime[nevent];
  if(hists_per_event) {
    for (int ev=0; ev<nevent; ev++) {
      h1event_hittime[ev][0] = new TH1F(TString::Format("h1event%d_hittime_photon", ev), TString::Format("Raw hit time for photons, event %d;Hit time (ns);Entries", ev), 1000, 0, 1000);
      h1event_hittime[ev][1] = new TH1F(TString::Format("h1event%d_hittime_noise",  ev), TString::Format("Raw hit time for dark noise, event %d;Hit time (ns);Entries", ev), 1000, 0, 1000);
      h1event_hittime[ev][1]->SetLineColor(kRed);
      h1event_hittime[ev][1]->SetFillColor(kRed);
      hSevent_hittime[ev] = new THStack(TString::Format("hSevent%d_hittime", ev), TString::Format("Raw hit time, event %d;Hit time (ns);Entries", ev));
      hSevent_hittime[ev]->Add(h1event_hittime[ev][1]);
      hSevent_hittime[ev]->Add(h1event_hittime[ev][0]);
    }//ev
  }
  //digit times per event
  TH1F *h1event_digittime[nevent][3];
  THStack *hSevent_digittime[nevent];
  if(hists_per_event) {
    for (int ev=0; ev<nevent; ev++) {
      h1event_digittime[ev][0] = new TH1F(TString::Format("h1event%d_digittime_photon", ev), TString::Format("Digit time (digits from photon hits only), event %d;Digit time (ns);Entries", ev), 1350, -400, 9500);
      h1event_digittime[ev][1] = new TH1F(TString::Format("h1event%d_digittime_noise",  ev), TString::Format("Digit time (digits from noise hits only), event %d;Digit time (ns);Entries", ev), 1350, -400, 9500);
      h1event_digittime[ev][2] = new TH1F(TString::Format("h1event%d_digittime_mix",    ev), TString::Format("Digit time (digits from a mix of noise and photon hits), event %d;Digit time (ns);Entries", ev), 1350, -400, 9500);
      h1event_digittime[ev][0]->SetLineColor(kBlack);
      h1event_digittime[ev][0]->SetFillColor(kBlack);
      h1event_digittime[ev][1]->SetLineColor(kRed);
      h1event_digittime[ev][1]->SetFillColor(kRed);
      h1event_digittime[ev][2]->SetLineColor(kCyan);
      h1event_digittime[ev][2]->SetFillColor(kCyan);
      hSevent_digittime[ev] = new THStack(TString::Format("hSevent%d_digittime", ev), TString::Format("Digit time, event %d;Digit time (ns);Entries", ev));
      hSevent_digittime[ev]->Add(h1event_digittime[ev][1]);
      hSevent_digittime[ev]->Add(h1event_digittime[ev][0]);
      hSevent_digittime[ev]->Add(h1event_digittime[ev][2]);
    }//ev
  }

  int num_trig = 0;
  
  // Now loop over events
  for (int ev=0; ev<nevent; ev++)
  {
    // Read the event from the tree into the WCSimRootEvent instance
    tree->GetEntry(ev);
    wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

    if(verbose || ((ev % 100) == 0))
      cout << "Event " << ev << " of " << nevent << " has " << wcsimrootsuperevent->GetNumberOfEvents() << " triggers" << endl;

    if(verbose){
      printf("********************************************************");
      printf("Evt, date %d %d\n", wcsimrootevent->GetHeader()->GetEvtNum(),
	     wcsimrootevent->GetHeader()->GetDate());
      printf("Mode %d\n", wcsimrootevent->GetMode());
      printf("Number of subevents %d\n",
	     wcsimrootsuperevent->GetNumberOfSubEvents());
      
      printf("Vtxvol %d\n", wcsimrootevent->GetVtxvol());
      printf("Vtx %f %f %f\n", wcsimrootevent->GetVtx(0),
	     wcsimrootevent->GetVtx(1),wcsimrootevent->GetVtx(2));
    }
    hvtx0->Fill(wcsimrootevent->GetVtx(0));
    hvtx1->Fill(wcsimrootevent->GetVtx(1));
    hvtx2->Fill(wcsimrootevent->GetVtx(2));

    if(verbose){
      printf("Jmu %d\n", wcsimrootevent->GetJmu());
      printf("Npar %d\n", wcsimrootevent->GetNpar());
      printf("Ntrack %d\n", wcsimrootevent->GetNtrack());
    }
    // Now read the tracks in the event
    
    // Get the number of tracks
    int ntrack = wcsimrootevent->GetNtrack();
    if(verbose) printf("ntracks=%d\n",ntrack);
    
    int i;
    // Loop through elements in the TClonesArray of WCSimTracks
    for (i=0; i<ntrack; i++)
    {
      TObject *element = (wcsimrootevent->GetTracks())->At(i);      
      WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);
      if(verbose){
	printf("Track ipnu: %d\n",wcsimroottrack->GetIpnu());
	printf("Track parent ID: %d\n",wcsimroottrack->GetParenttype());
	for (int j=0; j<3; j++)
	  printf("Track dir: %d %f\n",j, wcsimroottrack->GetDir(j));
      }
    }  // End of loop over tracks
    
    // Now look at the Cherenkov hits
    
    // Get the number of Cherenkov hits.
    // Note... this is *NOT* the number of photons that hit tubes.
    // It is the number of tubes hit with Cherenkov photons.
    // The number of digitized tubes will be smaller because of the threshold.
    // Each hit "raw" tube has several photon hits.  The times are recorded.
    // See http://nwg.phy.bnl.gov/DDRD/cgi-bin/private/ShowDocument?docid=245
    // for more information on the structure of the root file.
    //  
    // The following code prints out the hit times for the first 10 tubes and also
    // adds up the total pe.
    // 
    // For digitized info (one time/charge tube after a trigger) use
    // the digitized information.
    //

    int ncherenkovhits     = wcsimrootevent->GetNcherenkovhits();
    int ncherenkovhittimes = wcsimrootevent->GetNcherenkovhittimes();
    int ncherenkovdigihits = wcsimrootevent->GetNcherenkovdigihits(); 
    
    h1->Fill(ncherenkovdigihits);
    if(verbose){
      printf("node id: %i\n", ev);
      printf("Ncherenkovhits (unique PMTs with hits)  %d\n", ncherenkovhits);
      printf("Ncherenkovhittimes (number of raw hits) %d\n", ncherenkovhittimes);
      printf("Ncherenkovdigihits (number of digits)   %d\n", ncherenkovdigihits);
      printf("NumTubesHit       %d\n", wcsimrootevent->GetNumTubesHit());
      printf("NumDigitizedTubes %d\n", wcsimrootevent->GetNumDigiTubesHit());
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

     
      //if ( i < 10 ) // Only print first XX=10 tubes
      {
	if(verbose) printf("Total pe for tube %d: %d times( ", tubeNumber, peForTube);
	for (int j = timeArrayIndex; j < timeArrayIndex + peForTube; j++)
	{

	  TObject *element = (wcsimrootevent->GetCherenkovHitTimes())->At(j);
	  WCSimRootCherenkovHitTime* HitTime = 
	    dynamic_cast<WCSimRootCherenkovHitTime*>(element);
	  
	  if(verbose) printf("%6.2f ", HitTime->GetTruetime() );	     
	}
	if(verbose) cout << ")" << endl;
      }

    } // End of loop over Cherenkov hits
    if(verbose) cout << "Total Pe : " << totalPe << endl;
    
    // Look at digitized hit info

    //Store the digit times of pure noise or pure photon digits
    // to make NHits in 200nsec window plots
    vector<double> digit_times;
    vector<double> digit_times_noise;
    vector<double> digit_times_photon;
    vector<double> digit_times_mix;

    // Get the number of digitized hits
    // Loop over sub events
   
    if(verbose) cout << "DIGITIZED HITS:" << endl;
    for (int index = 0 ; index < 1; index++) //wcsimrootsuperevent->GetNumberOfEvents(); index++) 
    {
      int n_noise_hits_total = 0, n_photon_hits_total = 0;
      wcsimrootevent = wcsimrootsuperevent->GetTrigger(index);
      if(verbose) cout << "Sub event number = " << index << "\n";

      int ncherenkovdigihits = wcsimrootevent->GetNcherenkovdigihits();
      if(verbose) printf("Ncherenkovdigihits %d\n", ncherenkovdigihits);

      int trigger_time = wcsimrootevent->GetHeader()->GetDate();
      TriggerType_t trigger_type = wcsimrootevent->GetTriggerType();
      std::vector<Float_t> trigger_info = wcsimrootevent->GetTriggerInfo();

      h1triggertime->Fill(trigger_time);
      if(trigger_info.size() > 0) {
	if((trigger_type == kTriggerNHits) || (trigger_type == kTriggerNHitsSKDETSIM) || (trigger_type == kTriggerNHitsTest)) {
	  h2nhits->Fill(ncherenkovdigihits, trigger_info[0]);
	  h1nhitstrigger->Fill(trigger_info[0]);
	}
      }
      h1nhits->Fill(ncherenkovdigihits);

      h1triggertype->Fill(WCSimEnumerations::EnumAsString(trigger_type).c_str(), 1);
      if(true || verbose) {
	cout << "Passed trigger "
	     << WCSimEnumerations::EnumAsString(trigger_type)
	     << " with timestamp " << trigger_time
	     << " and " << ncherenkovdigihits
	     << " hits in the saved subevent region";
	if(trigger_info.size() > 0) {
	  if((trigger_type == kTriggerNHits) || (trigger_type == kTriggerNHitsSKDETSIM) || (trigger_type == kTriggerNHitsTest))
	    cout << " (" << trigger_info[0]
		 << " in the 200nsec region)";
	}
	cout << endl;
      }

      if(ncherenkovdigihits>0)
	num_trig++;

      // Loop through elements in the TClonesArray of WCSimRootCherenkovDigHits
      float totalpe = 0, totaltime = 0;
      for (i=0;i<ncherenkovdigihits;i++) {
	//get the digit
	if(verbose)
	  cout << "Getting digit " << i << endl;
	TObject *element = (wcsimrootevent->GetCherenkovDigiHits())->At(i);
    	WCSimRootCherenkovDigiHit *wcsimrootcherenkovdigihit = 
    	  dynamic_cast<WCSimRootCherenkovDigiHit*>(element);
	
	//find out whether this is due to noise, a real photon, or both
	int tube_id = wcsimrootcherenkovdigihit->GetTubeId();
	int timeArrayIndex = -1;
	int peForTube      = -1;
	//loop through the WCSimRootCherenkovHit array to find the entry for this tube ID
	for(int ipmt = 0; ipmt < ncherenkovhits; ipmt++) {
	  if(verbose)
	    cout << "Getting hit " << ipmt << " of " << ncherenkovhits << endl;
	  TObject *Hit = (wcsimrootevent->GetCherenkovHits())->At(ipmt);
	  WCSimRootCherenkovHit *wcsimrootcherenkovhit =
	    dynamic_cast<WCSimRootCherenkovHit*>(Hit);
	  int tubeNumber     = wcsimrootcherenkovhit->GetTubeID();
	  if(tube_id == tubeNumber) {
	    timeArrayIndex = wcsimrootcherenkovhit->GetTotalPe(0);
	    peForTube      = wcsimrootcherenkovhit->GetTotalPe(1);
	    break;
	  }
	}//ipmt
	int n_noise_hits = 0, n_photon_hits = 0, n_unknown_hits = 0;
	if(timeArrayIndex == -1) {
	  if(verbose)
	    cout << "No PMT hits found for digit " << i << " with tube ID " << tube_id << endl;
	}
	else {
	  if(verbose)
	    cout << peForTube << " PMT hits found for digit " << i << " with tube ID " << tube_id << endl;
	  //loop over the photons ids of hits that made up the digit
	  vector<int> photon_ids = wcsimrootcherenkovdigihit->GetPhotonIds();
	  if(verbose)
	    cout << photon_ids.size() << " photons made up this digit" << endl;
	  for(unsigned int iphoton = 0; iphoton < photon_ids.size(); iphoton++) {
	    int this_photon = photon_ids[iphoton];
	    if(verbose)
	      cout << "Attempting to look for photon " << this_photon+1 << " in WCSimRootCherenkovHitTime array...";
	    if(this_photon >= peForTube) {
	      if(verbose)
		cout << " There are only " << peForTube << " photons in this PMT" << endl;
	      continue;
	    }
	    //now look in the WCSimRootCherenkovHitTime array to count the number of photon / dark noise hits
	    TObject *Hit = (wcsimrootevent->GetCherenkovHitTimes())->At(timeArrayIndex + this_photon);
	    WCSimRootCherenkovHitTime *wcsimrootcherenkovhittime =
	      dynamic_cast<WCSimRootCherenkovHitTime*>(Hit);
	    if(verbose)
	      cout << " hit time " << wcsimrootcherenkovhittime->GetTruetime() << " " << wcsimrootcherenkovhittime->GetParentID() << endl;
	    if(wcsimrootcherenkovhittime->GetParentID() == -1) {
	      n_noise_hits++;
	      if(hists_per_event)
		h1event_hittime[ev][1]->Fill(wcsimrootcherenkovhittime->GetTruetime());
	      h1eventALL_hittime[1]->Fill(wcsimrootcherenkovhittime->GetTruetime());
	    }
	    else if(wcsimrootcherenkovhittime->GetParentID() < 0) {
	      n_unknown_hits++;
	      cout << "HIT PARENT UNKNOWN " << wcsimrootcherenkovhittime->GetParentID() << endl;
	    }
	    else {
	      n_photon_hits++;
	      if(hists_per_event)
		h1event_hittime[ev][0]->Fill(wcsimrootcherenkovhittime->GetTruetime());
	      h1eventALL_hittime[0]->Fill(wcsimrootcherenkovhittime->GetTruetime());
	    }
	  }//iphoton
	}//hits in this PMT found
	if(hists_per_event) {
	  hSevent_hittime[ev]->SetTitle(TString::Format("Raw hit time, event %d, %s;Hit time (ns);Entries", ev, WCSimEnumerations::EnumAsString(trigger_type).c_str()));
	  hSevent_digittime[ev]->SetTitle(TString::Format("Digit time, event %d, %s;Digit time (ns);Entries", ev, WCSimEnumerations::EnumAsString(trigger_type).c_str()));
	}

	if(verbose){
	  //if ( i < 10 ) // Only print first XX=10 tubes
	    printf("q, t, tubeid, nphotonhits, nnoisehits, nunknownhits: %f %f %d  %d %d %d\n",
		   wcsimrootcherenkovdigihit->GetQ(),
		   wcsimrootcherenkovdigihit->GetT(),
		   wcsimrootcherenkovdigihit->GetTubeId(),
		   n_photon_hits,
		   n_noise_hits,
		   n_unknown_hits);
	}
	if(n_noise_hits && !n_photon_hits) {
	  digit_times_noise.push_back(wcsimrootcherenkovdigihit->GetT());
	  h1time_noise->Fill(wcsimrootcherenkovdigihit->GetT());
	  if(hists_per_event)
	    h1event_digittime[ev][1]->Fill(wcsimrootcherenkovdigihit->GetT());
	}
	else if(!n_noise_hits && n_photon_hits) {
	  digit_times_photon.push_back(wcsimrootcherenkovdigihit->GetT());
	  h1time_photon->Fill(wcsimrootcherenkovdigihit->GetT());
	  if(hists_per_event)
	    h1event_digittime[ev][0]->Fill(wcsimrootcherenkovdigihit->GetT());
	}
	else {
	  digit_times_mix.push_back(wcsimrootcherenkovdigihit->GetT());
	  h1time_mix->Fill(wcsimrootcherenkovdigihit->GetT());
	  if(hists_per_event)
	    h1event_digittime[ev][2]->Fill(wcsimrootcherenkovdigihit->GetT());
	}
	digit_times.push_back(wcsimrootcherenkovdigihit->GetT());
	totalpe   += wcsimrootcherenkovdigihit->GetQ();
	totaltime += wcsimrootcherenkovdigihit->GetT();
	h1time->Fill(wcsimrootcherenkovdigihit->GetT());
	h1time2->Fill(wcsimrootcherenkovdigihit->GetT() + trigger_time);
	float noise_fraction = (float)n_noise_hits / (float)(n_noise_hits + n_photon_hits);
	h1noisefrac->Fill(noise_fraction);
	h2nhits_sep->Fill(ncherenkovdigihits, noise_fraction);
	if(trigger_info.size() > 0)
	  h2nhitstrigger_sep->Fill(trigger_info[0], noise_fraction);
	n_noise_hits_total += n_noise_hits;
	n_photon_hits_total += n_photon_hits;
      }//i // End of loop over Cherenkov digihits
      h1pe->Fill(totalpe);
      if(ncherenkovdigihits) {
	h1peperdigi->Fill(totalpe / ncherenkovdigihits);
	h1timeperdigi->Fill((totaltime) / ncherenkovdigihits);
	integrate_digit_times(digit_times, h1inttime);
	integrate_digit_times(digit_times_noise, h1inttimenoise);
	integrate_digit_times(digit_times_photon, h1inttimephoton);
	integrate_digit_times(digit_times_mix, h1inttimemix);
      }
      float noise_fraction_total = (float)n_noise_hits_total / (float)(n_noise_hits_total + n_photon_hits_total);
      h1noisefrac_trigger->Fill(noise_fraction_total);
    }//index // End of loop over triggers
    
    // reinitialize super event between loops.
    wcsimrootsuperevent->ReInitialize();
    
  }//ev // End of loop over events
  std::cout<<"num_trig "<<num_trig<<"\n";

  //save histograms in .root file
  TString filenameout(filename);
  TFile * fout = new TFile(create_filename("analysed_", filenameout).Data(), "RECREATE");
  fout->cd();
  hvtx0->Write();
  hvtx1->Write();
  hvtx2->Write();
  h1->Write();
  h2nhits->Write();
  h1noisefrac->Write();
  h1noisefrac_trigger->Write();
  h2nhits_sep->Write();
  h2nhitstrigger_sep->Write();
  h1nhits->Write();
  h1nhitstrigger->Write();
  h1pe->Write();
  h1time->Write();
  h1time_photon->Write();
  h1time_noise->Write();
  h1time_mix->Write();
  hStime->Write();
  h1time2->Write();
  h1triggertime->Write();
  h1peperdigi->Write();
  h1timeperdigi->Write();
  h1inttime->Write();
  h1inttimenoise->Write();
  h1inttimephoton->Write();
  h1inttimemix->Write();
  if(hists_per_event) {
    for (int ev=0; ev<nevent; ev++) {
      h1event_hittime[ev][0]->Write();
      h1event_hittime[ev][1]->Write();
      hSevent_hittime[ev]->Write();
    }
  }
  h1eventALL_hittime[0]->Write();
  h1eventALL_hittime[1]->Write();
  hSeventALL_hittime->Write();
  if(hists_per_event) {
    for (int ev=0; ev<nevent; ev++) {
      h1event_digittime[ev][0]->Write();
      h1event_digittime[ev][1]->Write();
      hSevent_digittime[ev]->Write();
    }
  }
  h1triggertype->Write();
  fout->Close();

  if(create_pdfs) {
    //draw some histograms & save as pdfs
    filenameout.ReplaceAll(".root", ".pdf");
    TCanvas * c2 = new TCanvas();
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    c2->SetBottomMargin(0.15);
    c2->SetLeftMargin(0.15);

    c2->cd();
    h2nhits->Draw("COLZ");
    c2->SaveAs(create_filename("h2nhits_", filenameout).Data());
    h1nhits->Draw();
    c2->SaveAs(create_filename("h1nhits_", filenameout).Data());
    h1nhitstrigger->Draw();
    c2->SaveAs(create_filename("h1nhitstrigger_", filenameout).Data());
    h1pe->Draw();
    c2->SaveAs(create_filename("h1pe_", filenameout).Data());
    h1time->Draw();
    c2->SaveAs(create_filename("h1time_", filenameout).Data());
    hStime->Draw();
    TLegend * l = new TLegend(0.4,0.9,0.9,1.0);
    l->SetTextSize(0.042);
    l->SetTextFont(132);
    l->AddEntry(h1time_photon, "Photon", "l");
    l->AddEntry(h1time_noise, "Noise",  "l");
    l->AddEntry(h1time_mix, "Mixed",  "l");
    l->Draw();
    c2->SaveAs(create_filename("hStime_", filenameout).Data());
    h1time2->Draw();
    c2->SaveAs(create_filename("h1time2_", filenameout).Data());
    h1peperdigi->Draw();
    c2->SaveAs(create_filename("h1peperdigi_", filenameout).Data());
    h1timeperdigi->Draw();
    c2->SaveAs(create_filename("h1timeperdigi_", filenameout).Data());
    h1noisefrac->Draw();
    c2->SaveAs(create_filename("h1noisefrac_", filenameout).Data());
    h1noisefrac_trigger->Draw();
    c2->SaveAs(create_filename("h1noisefrac_trigger_", filenameout).Data());

    h2nhits_sep->Draw("COLZ");
    c2->SaveAs(create_filename("h2nhits_sep_", filenameout).Data());
    h2nhitstrigger_sep->Draw("COLZ");
    c2->SaveAs(create_filename("h2nhitstrigger_sep_", filenameout).Data());

    hSeventALL_hittime->Draw();
    l = new TLegend(0.4,0.9,0.9,1.0);
    l->SetTextSize(0.042);
    l->SetTextFont(132);
    l->AddEntry(h1eventALL_hittime[0], "Photon", "l");
    l->AddEntry(h1eventALL_hittime[1], "Noise",  "l");
    l->Draw();
    c2->SaveAs(create_filename("hSeventALL_hittime_", filenameout).Data());

    h1triggertype->Draw();
    c2->SaveAs(create_filename("h1triggertype_", filenameout).Data());
    h1triggertime->Draw();
    c2->SaveAs(create_filename("h1triggertime_", filenameout).Data());
  }
  
  return 0;
}
