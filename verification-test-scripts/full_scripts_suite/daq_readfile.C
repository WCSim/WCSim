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
#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"
#include "WCSimEnumerations.hh"
#endif

using namespace std;

TString create_filename(const char * prefix, TString& filename_string)
{
  //std::cout << "Creating filename from prefix " << prefix << " and filename_string " << filename_string << std::endl;
  TString prefix_string(prefix);
  TString outfilename = prefix_string + filename_string;
  return outfilename;
}

// Simple example of reading a generated Root file
int daq_readfile(char *filename=NULL, bool verbose=false, Long64_t max_nevents = 999999999999, int max_ntriggers = -1, bool create_pdfs = false, bool hists_per_event = false)
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
  if(geotree->GetEntries() == 0) {
    cout << "geotree not found!" << endl;
      exit(9);
  }
  geotree->GetEntry(0);

  // start with the main "subevent", as it contains most of the info
  // and always exists.
  WCSimRootTrigger* wcsimrootevent;

  //book histograms
  TH1F *h1vtx0 = new TH1F("h1vtx0", "Event VTX0", 200, -1500, 1500);
  TH1F *h1vtx1 = new TH1F("h1vtx1", "Event VTX1", 200, -1500, 1500);
  TH1F *h1vtx2 = new TH1F("h1vtx2", "Event VTX2", 200, -1500, 1500);

  TH1I *h1triggertype = new TH1I("h1triggertype", "Trigger type;Trigger type;Entries", 10, 0, 10);
  for(int i = -1; i <= kTriggerFailure; i++) {
    TriggerType_t ttype = (TriggerType_t)i;
    h1triggertype->GetXaxis()->SetBinLabel(i+2, WCSimEnumerations::EnumAsString(ttype).c_str());
  }

  TH1I *h1ndigihits = new TH1I("h1ndigihits", "NDigits in the subevent window;NDigits;Entries", 10001, -0.5, 10000.5);
  TH1I *h1nrawhits = new TH1I("h1nrawhits", "NHits in the subevent window;NHits;Entries", 10001, -0.5, 10000.5);
  TH1I *h1ntubeshitraw  = new TH1I("h1ntubeshitraw",  "Number of PMTs with raw hit in the subevent window;NPMTs with raw hits;Entries", 10001, -0.5, 10000.5);
  TH1I *h1ntubeshitdigi = new TH1I("h1ntubeshitdigi", "Number of PMTs with digits in the subevent window;NPMTs with digits;Entries", 10001, -0.5, 10000.5);
  TH1I *h1ndigihitstrigger = new TH1I("h1ndigihitstrigger", "NDigits in the trigger window;NDigits;Entries", 10001, -0.5, 10000.5);

  TH1F *h1digipe = new TH1F("h1digipe", "Total p.e. in the subevent window;Total p.e.;Entries", 100001, -0.5, 100000.5);

  TH1F *h1digitime = new TH1F("h1digitime", "Digit time;Digit time (ns);Entries", 18000, -3000, 15000);
  TH1F *h1digitime_noise = new TH1F("h1digitime_noise", "Digit time (digits from noise hits only);Digit time (ns);Entries", 18000, -3000, 15000);
  TH1F *h1digitime_photon = new TH1F("h1digitime_photon", "Digit time (digits from photon hits only);Digit time (ns);Entries", 18000, -3000, 15000);
  TH1F *h1digitime_mix = new TH1F("h1digitime_mix", "Digit time (digits from a mix of noise and photon hits);Digit time (ns);Entries", 18000, -3000, 15000);
  THStack *hSdigitime = new THStack("hSdigitime", "Digit time;Digits time (ns);Entries");
  h1digitime_noise->SetLineColor(kBlack);
  h1digitime_photon->SetLineColor(kRed);
  h1digitime_mix->SetLineColor(kCyan);
  hSdigitime->Add(h1digitime_noise);
  hSdigitime->Add(h1digitime_photon);
  hSdigitime->Add(h1digitime_mix);
  TH1F *h1digiplustriggertime = new TH1F("h1digiplustriggertime", "Digit time + trigger time;Digit time + trigger time (ns);Entries", 18000, -3000, 15000);
  TH1F *h1triggertime = new TH1F("h1triggertime", "Time of trigger;Trigger time;Entries", 10000,0,10000);

  TH1F *h1digipeperdigi = new TH1F("h1digipeperdigi", "Total p.e. in the subevent window;Total p.e. / NDigits;Entries", 1000, 0, 50);
  TH1F *h1digitimeperdigi = new TH1F("h1digitimeperdigi", "Hit time (relative to trigger time);(Hit time - trigger time) / NDigits (ns);Entries", 18000, -3000, 15000);

  TH2F *h2nhits_sep = new TH2F("h2nhits_sep", "NDigits from subevent window vs fraction that are noise;NDigits;Noise fraction", 1001, -0.5, 10000.5, 100, 0, 1);
  TH2F *h2nhitstrigger_sep = new TH2F("h2nhitstrigger_sep", "NDigits from 200ns trigger window vs fraction that are noise;NDigits;Noise fraction", 1001, -0.5, 10000.5, 100, 0, 1);
  TH1F *h1diginoisefrac = new TH1F("h1diginoisefrac", "Fraction of hits in this digi that are noise;Noise fraction;Entries", 110,0,1.1);
  TH1F *h1diginoisefrac_trigger = new TH1F("h1diginoisefrac_trigger", "Fraction of hits in this trigger that are noise;Noise fraction;Entries", 110,0,1.1);

  TH1F *h1hittime = new TH1F("h1hittime", "Raw hit time;Hit time (ns);Entries",    20000, -10000, 10000);
  TH1F *h1hittime_photon = new TH1F("h1hittime_photon", "Raw hit time (photons);Hit time (ns);Entries",    20000, -10000, 10000);
  TH1F *h1hittime_noise  = new TH1F("h1hittime_noise",  "Raw hit time (dark noise);Hit time (ns);Entries",    20000, -10000, 10000);

  //hit times per event
  TH1F *h1event_hittime[nevent][2];
  THStack *hSevent_hittime[nevent];
  if(hists_per_event) {
    for (int ev=0; ev<nevent; ev++) {
      h1event_hittime[ev][0] = new TH1F(TString::Format("h1event%d_hittime_photon", ev), TString::Format("Raw hit time for photons in digits, event %d;Hit time (ns);Entries", ev), 10000, 0, 10000);
      h1event_hittime[ev][1] = new TH1F(TString::Format("h1event%d_hittime_noise",  ev), TString::Format("Raw hit time for dark noise digits, event %d;Hit time (ns);Entries", ev), 10000, 0, 10000);
      h1event_hittime[ev][1]->SetLineColor(kRed);
      h1event_hittime[ev][1]->SetFillColor(kRed);
      hSevent_hittime[ev] = new THStack(TString::Format("hSevent%d_hittime", ev), TString::Format("Raw hit time in digits, event %d;Hit time (ns);Entries", ev));
      hSevent_hittime[ev]->Add(h1event_hittime[ev][1]);
      hSevent_hittime[ev]->Add(h1event_hittime[ev][0]);
    }//ev
  }
  //digit times per event
  TH1F *h1event_digittime[nevent][3];
  THStack *hSevent_digittime[nevent];
  if(hists_per_event) {
    for (int ev=0; ev<nevent; ev++) {
      h1event_digittime[ev][0] = new TH1F(TString::Format("h1event%d_digittime_photon", ev), TString::Format("Digit time (digits from photon hits only), event %d;Digit time (ns);Entries", ev), 18000, -3000, 15000);
      h1event_digittime[ev][1] = new TH1F(TString::Format("h1event%d_digittime_noise",  ev), TString::Format("Digit time (digits from noise hits only), event %d;Digit time (ns);Entries", ev), 18000, -3000, 15000);
      h1event_digittime[ev][2] = new TH1F(TString::Format("h1event%d_digittime_mix",    ev), TString::Format("Digit time (digits from a mix of noise and photon hits), event %d;Digit time (ns);Entries", ev), 18000, -3000, 15000);
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

  //create output file for storing histograms and tree
  TString filenameout(filename);
  TFile * fout = new TFile(create_filename("analysed_", filenameout).Data(), "RECREATE");
  fout->cd();

  //create tree to save information on an event-by-event basis
  TTree * tout = new TTree("validation_per_event", "Event by event validation variables");
  TTree * tout_trig = new TTree("validation_per_trigger", "Trigger by trigger validation variables");
  //event id variables
  int teventnumber, ttriggernumber;
  tout->Branch("eventnumber", &teventnumber);
  tout_trig->Branch("eventnumber", &teventnumber);
  tout_trig->Branch("triggernumber", &ttriggernumber);
  //per event variables
  double tvtx0, tvtx1, tvtx2;
  int    tntriggers, tntracks, tnrawhits, tntubeshitraw;
  vector<double> tvhittime, tvhittime_noise, tvhittime_photon;
  tout->Branch("vtx0", &tvtx0);
  tout->Branch("vtx1", &tvtx1);
  tout->Branch("vtx2", &tvtx2);
  tout->Branch("ntriggers", &tntriggers);
  tout->Branch("ntracks", &tntracks);
  tout->Branch("nrawhits", &tnrawhits);
  tout->Branch("ntubeshitraw", &tntubeshitraw);
  tout->Branch("hittime", &tvhittime);
  tout->Branch("hittime_noise", &tvhittime_noise);
  tout->Branch("hittime_photon", &tvhittime_photon);
  //per trigger variables
  int ttriggertype;
  int tndigihits, tntubeshitdigi, tndigihitstrigger;
  double ttriggertime;
  double tdigipeperdigi, tdigitimeperdigi;
  double ttotaldigipe;
  double ttotaldiginoisefrac;
  vector<double> tvdigipe;
  vector<double> tvdigitime, tvdigitime_noise, tvdigitime_photon, tvdigitime_mix;
  vector<double> tvdigiplustriggertime;
  vector<double> tvdiginoisefrac;
  tout_trig->Branch("triggertype", &ttriggertype);
  tout_trig->Branch("ndigihits", &tndigihits);
  tout_trig->Branch("ntubeshitdigi", &tntubeshitdigi);
  tout_trig->Branch("ndigihitstrigger", &tndigihitstrigger);
  tout_trig->Branch("triggertime", &ttriggertime);
  tout_trig->Branch("digipeperdigi", &tdigipeperdigi);
  tout_trig->Branch("digitimeperdigi", &tdigitimeperdigi);
  tout_trig->Branch("totaldigipe", &ttotaldigipe);
  tout_trig->Branch("totaldiginoisefrac", &ttotaldiginoisefrac);
  tout_trig->Branch("digipe", &tvdigipe);
  tout_trig->Branch("digitime", &tvdigitime);
  tout_trig->Branch("digitime_noise", &tvdigitime_noise);
  tout_trig->Branch("digitime_photon", &tvdigitime_photon);
  tout_trig->Branch("digitime_mix", &tvdigitime_mix);
  tout_trig->Branch("digiplustriggertime", &tvdigiplustriggertime);
  tout_trig->Branch("diginoisefrac", &tvdiginoisefrac);

  int num_trig = 0;
  
  // Now loop over events
  for (int ev=0; ev<nevent; ev++)
  {
    teventnumber = ev;

    //Reset per event tree variables
    tvtx0                 = -999999;
    tvtx1                 = -999999;
    tvtx2                 = -999999;
    tntriggers            = -1;
    tntracks              = -1;
    tnrawhits             = -1;
    tntubeshitraw         = -1;
    tvhittime.clear();
    tvhittime_noise.clear();
    tvhittime_photon.clear();

    // Read the event from the tree into the WCSimRootEvent instance
    tree->GetEntry(ev);
    wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

    const int ntriggers = wcsimrootsuperevent->GetNumberOfEvents();
    tntriggers = ntriggers;

    if(verbose || ((ev % 100) == 0))
      cout << "Event " << ev << " of " << nevent << " has " << ntriggers << " triggers" << endl;

    const double vtx0 = wcsimrootevent->GetVtx(0);
    const double vtx1 = wcsimrootevent->GetVtx(1);
    const double vtx2 = wcsimrootevent->GetVtx(2);
    if(verbose){
      printf("********************************************************");
      printf("Evt, date %d %d\n", wcsimrootevent->GetHeader()->GetEvtNum(),
	     wcsimrootevent->GetHeader()->GetDate());
      printf("Mode %d\n", wcsimrootevent->GetMode());
      printf("Number of subevents %d\n",
	     wcsimrootsuperevent->GetNumberOfSubEvents());
      
      printf("Vtxvol %d\n", wcsimrootevent->GetVtxvol());
      printf("Vtx %f %f %f\n", vtx0, vtx1, vtx2);
    }
    h1vtx0->Fill(vtx0);
    h1vtx1->Fill(vtx1);
    h1vtx2->Fill(vtx2);
    tvtx0 = vtx0;
    tvtx1 = vtx1;
    tvtx2 = vtx2;

    if(verbose){
      printf("Jmu %d\n", wcsimrootevent->GetJmu());
      printf("Npar %d\n", wcsimrootevent->GetNpar());
    }
    // Now read the tracks in the event
    
    // Get the number of tracks
    const int ntracks = wcsimrootevent->GetNtrack();
    if(verbose) printf("ntracks=%d\n",ntracks);
    tntracks = ntracks;
    
    // Loop through elements in the TClonesArray of WCSimTracks
    for (int itrack = 0; itrack < ntracks; itrack++)
    {
      TObject *element = (wcsimrootevent->GetTracks())->At(itrack);      
      WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);
      if(verbose){
	printf("Track ipnu: %d\n",wcsimroottrack->GetIpnu());
	printf("Track parent ID: %d\n",wcsimroottrack->GetParenttype());
	for (int j=0; j<3; j++)
	  printf("Track dir: %d %f\n", j, wcsimroottrack->GetDir(j));
      }
    }  //itrack // End of loop over tracks
    
    //get number of hits and digits
    const int ncherenkovhits      = wcsimrootevent->GetNcherenkovhits();
    const int ncherenkovhittimes  = wcsimrootevent->GetNcherenkovhittimes();
    const int ntubeshit           = wcsimrootevent->GetNumTubesHit();
    const int ncherenkovdigihits0 = wcsimrootevent->GetNcherenkovdigihits(); 
    const int ntubesdigihit0      = wcsimrootevent->GetNumDigiTubesHit();
    if(verbose){
      printf("node id: %i\n", ev);
      printf("Ncherenkovhits (unique PMTs with hits)  %d\n", ncherenkovhits);
      printf("Ncherenkovhittimes (number of raw hits) %d\n", ncherenkovhittimes);
      printf("Ncherenkovdigihits (number of digits) in trigger 0   %d\n", ncherenkovdigihits0);
      printf("NumTubesHit       %d\n", wcsimrootevent->GetNumTubesHit());
      printf("NumDigitizedTubes in trigger 0 %d\n", wcsimrootevent->GetNumDigiTubesHit());
    }
    h1nrawhits->Fill(ncherenkovhittimes);
    h1ntubeshitraw->Fill(ntubeshit);
    tnrawhits     = ncherenkovhittimes;
    tntubeshitraw = ntubeshit;

    //
    // Now look at the raw Cherenkov+noise hits
    //
    if(verbose)
      cout << "RAW HITS:" << endl;

    // Grab the big arrays of times and parent IDs
    TClonesArray *timeArray = wcsimrootevent->GetCherenkovHitTimes();
    
    //calculate total p.e. in event
    int totalPe = 0;
    // Loop through elements in the TClonesArray of WCSimRootCherenkovHits
    for(int ipmt = 0; ipmt < ncherenkovhits; ipmt++) {
      TObject * Hit = (wcsimrootevent->GetCherenkovHits())->At(ipmt);
      WCSimRootCherenkovHit * wcsimrootcherenkovhit =
	dynamic_cast<WCSimRootCherenkovHit*>(Hit);
      int timeArrayIndex = wcsimrootcherenkovhit->GetTotalPe(0);
      int peForTube      = wcsimrootcherenkovhit->GetTotalPe(1);
      int tubeNumber     = wcsimrootcherenkovhit->GetTubeID();
      WCSimRootPMT pmt   = geo->GetPMT(tubeNumber-1);
      totalPe += peForTube;
      if(verbose)
	printf("Total pe for tube %d: %d times( ", tubeNumber, peForTube);
      for(int irawhit = 0; irawhit < peForTube; irawhit++) {
	TObject * HitTime = (wcsimrootevent->GetCherenkovHitTimes())->At(timeArrayIndex + irawhit);
	WCSimRootCherenkovHitTime * wcsimrootcherenkovhittime =
	  dynamic_cast<WCSimRootCherenkovHitTime*>(HitTime);
	double truetime = wcsimrootcherenkovhittime->GetTruetime();
	if(verbose)
	  printf("%6.2f ", truetime);
	h1hittime->Fill(truetime);
	tvhittime.push_back(truetime);
	if(wcsimrootcherenkovhittime->GetParentID() == -1) {
	  h1hittime_noise->Fill(truetime);
	  tvhittime_noise.push_back(truetime);
	  if(hists_per_event)
	    h1event_hittime[ev][1]->Fill(truetime);
	}
	else {
	  h1hittime_photon->Fill(truetime);
	  tvhittime_photon.push_back(truetime);
	  if(hists_per_event)
	    h1event_hittime[ev][0]->Fill(truetime);
	}
      }//irawhit
      if(verbose)
	cout << ")" << endl;
    }//ipmt
    if(verbose)
      cout << "Total Pe : " << totalPe << endl;

    //    
    // Now look at digitized hit info
    //
    if(verbose)
      cout << "DIGITIZED HITS:" << endl;

    //
    // Digi hits are arranged in subevents, so loop over these first
    //
    const int ntriggers_loop = max_ntriggers > 0 ? TMath::Min(max_ntriggers, ntriggers) : ntriggers;
    for (int itrigger = 0 ; itrigger < ntriggers_loop; itrigger++) {

      ttriggernumber = itrigger;

      //Reset per trigger tree variables
      ttriggertype          = -1;
      tndigihits            = -1;
      tntubeshitdigi        = -1;
      tndigihitstrigger     = -1;
      ttriggertime          = -999999;
      tdigipeperdigi        = -1;
      tdigitimeperdigi      = -999999;
      ttotaldigipe          = -1;
      ttotaldiginoisefrac   = -1;
      tvdigipe.clear();
      tvdigitime.clear();
      tvdigitime_noise.clear();
      tvdigitime_photon.clear();
      tvdigitime_mix.clear();
      tvdigiplustriggertime.clear();
      tvdiginoisefrac.clear();

      //count the number of noise/photon hits making up all the digits in this trigger
      int n_noise_hits_total = 0, n_photon_hits_total = 0;

      wcsimrootevent = wcsimrootsuperevent->GetTrigger(itrigger);
      if(verbose)
	cout << "Sub event number = " << itrigger << "\n";

      const int ncherenkovdigihits = wcsimrootevent->GetNcherenkovdigihits();
      const int ntubeshitdigi      = wcsimrootevent->GetNumDigiTubesHit();
      if(verbose)
	printf("Ncherenkovdigihits %d\n", ncherenkovdigihits);

      const int            trigger_time = wcsimrootevent->GetHeader()->GetDate();
      const TriggerType_t  trigger_type = wcsimrootevent->GetTriggerType();
      std::vector<Float_t> trigger_info = wcsimrootevent->GetTriggerInfo();

      h1triggertime->Fill(trigger_time);
      if(trigger_info.size() > 0) {
	if((trigger_type == kTriggerNDigits) || (trigger_type == kTriggerNDigitsTest)) {
	  h1ndigihitstrigger->Fill(trigger_info[0]);
	}
	tndigihitstrigger = trigger_info[0];
      }
      h1ndigihits->Fill(ncherenkovdigihits);
      h1ntubeshitdigi->Fill(ntubeshitdigi);

      h1triggertype->Fill(WCSimEnumerations::EnumAsString(trigger_type).c_str(), 1);
      if(verbose) {
	cout << "Passed trigger "
	     << WCSimEnumerations::EnumAsString(trigger_type)
	     << " with timestamp " << trigger_time
	     << " and " << ncherenkovdigihits
	     << " hits in the saved subevent region";
	if(trigger_info.size() > 0) {
	  if((trigger_type == kTriggerNDigits) || (trigger_type == kTriggerNDigitsTest))
	    cout << " (" << trigger_info[0]
		 << " in the 200nsec region)";
	}
	cout << endl;
      }
      ttriggertype   = (int)trigger_type;
      tndigihits     = ncherenkovdigihits;
      tntubeshitdigi = ntubeshitdigi;
      ttriggertime   = trigger_time;

      if(ncherenkovdigihits > 0)
	num_trig++;

      // Loop through elements in the TClonesArray of WCSimRootCherenkovDigHits
      float totaldigipe = 0, totaldigitime = 0;
      for(int idigipmt = 0; idigipmt < ncherenkovdigihits; idigipmt++) {
	//get the digit
	if(verbose)
	  cout << "Getting digit " << idigipmt << endl;
	TObject *element = (wcsimrootevent->GetCherenkovDigiHits())->At(idigipmt);
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
	    cout << "No PMT hits found for digit " << idigipmt << " with tube ID " << tube_id << endl;
	}
	else {
	  if(verbose)
	    cout << peForTube << " PMT hits found for digit " << idigipmt << " with tube ID " << tube_id << endl;
	  //loop over the rawhits ids of hits that made up the digit
	  vector<int> rawhit_ids = wcsimrootcherenkovdigihit->GetPhotonIds();
	  if(verbose)
	    cout << rawhit_ids.size() << " rawhits made up this digit" << endl;
	  for(unsigned int irawhit = 0; irawhit < rawhit_ids.size(); irawhit++) {
	    int this_rawhit = rawhit_ids[irawhit];
	    if(verbose)
	      cout << "Attempting to look for rawhit " << this_rawhit+1 << " in WCSimRootCherenkovHitTime array...";
	    if(this_rawhit >= peForTube) {
	      //if(verbose)
	      cerr << " There are only " << peForTube << " rawhits in this PMT" << endl;
	      continue;
	    }
	    //now look in the WCSimRootCherenkovHitTime array to count the number of photon / dark noise hits
	    TObject *Hit = (wcsimrootevent->GetCherenkovHitTimes())->At(timeArrayIndex + this_rawhit);
	    WCSimRootCherenkovHitTime *wcsimrootcherenkovhittime =
	      dynamic_cast<WCSimRootCherenkovHitTime*>(Hit);
	    const double hittime  = wcsimrootcherenkovhittime->GetTruetime();
	    const int    parentid = wcsimrootcherenkovhittime->GetParentID();
	    if(verbose)
	      cout << " hit time " << hittime << " " << parentid << endl;
	    if(parentid == -1) {
	      n_noise_hits++;
	    }
	    else if(parentid < 0) {
	      n_unknown_hits++;
	    }
	    else {
	      n_photon_hits++;
	    }
	  }//irawhit
	}//hits in this PMT found
	if(hists_per_event) {
	  hSevent_hittime[ev]->SetTitle(TString::Format("Raw hit time, event %d, %s;Hit time (ns);Entries", ev, WCSimEnumerations::EnumAsString(trigger_type).c_str()));
	  hSevent_digittime[ev]->SetTitle(TString::Format("Digit time, event %d, %s;Digit time (ns);Entries", ev, WCSimEnumerations::EnumAsString(trigger_type).c_str()));
	}

	const double digitime = wcsimrootcherenkovdigihit->GetT();
	const double digipe   = wcsimrootcherenkovdigihit->GetQ();
	tvdigipe.push_back(digipe);
	tvdigitime.push_back(digitime);
	tvdigiplustriggertime.push_back(digitime + trigger_time);

	if(verbose){
	  printf("q, t, tubeid, nphotonhits, nnoisehits, nunknownhits: %f %f %d  %d %d %d\n",
		 digipe,
		 digitime,
		 wcsimrootcherenkovdigihit->GetTubeId(),
		 n_photon_hits,
		 n_noise_hits,
		 n_unknown_hits);
	}
	if(n_noise_hits && !n_photon_hits) {
	  h1digitime_noise->Fill(digitime);
	  if(hists_per_event)
	    h1event_digittime[ev][1]->Fill(digitime);
	  tvdigitime_noise.push_back(digitime);
	}
	else if(!n_noise_hits && n_photon_hits) {
	  h1digitime_photon->Fill(digitime);
	  if(hists_per_event)
	    h1event_digittime[ev][0]->Fill(digitime);
	  tvdigitime_photon.push_back(digitime);
	}
	else {
	  h1digitime_mix->Fill(digitime);
	  if(hists_per_event)
	    h1event_digittime[ev][2]->Fill(digitime);
	  tvdigitime_mix.push_back(digitime);
	}
	totaldigipe   += digipe;
	totaldigitime += digitime;
	h1digitime->Fill(digitime);
	h1digiplustriggertime->Fill(digitime + trigger_time);
	float noise_fraction = (float)n_noise_hits / (float)(n_noise_hits + n_photon_hits);
	h1diginoisefrac->Fill(noise_fraction);
	h2nhits_sep->Fill(ncherenkovdigihits, noise_fraction);
	tvdiginoisefrac.push_back(noise_fraction);
	if(trigger_info.size() > 0) {
	  if((trigger_type == kTriggerNDigits) || (trigger_type == kTriggerNDigitsTest)) {
	    h2nhitstrigger_sep->Fill(trigger_info[0], noise_fraction);
	  }
	}
	n_noise_hits_total += n_noise_hits;
	n_photon_hits_total += n_photon_hits;
      }//idigipmt // End of loop over Cherenkov digihits
      h1digipe->Fill(totaldigipe);
      ttotaldigipe = totaldigipe;
      if(ncherenkovdigihits) {
	double peperdigi   = totaldigipe   / ncherenkovdigihits;
	double timeperdigi = totaldigitime / ncherenkovdigihits;
	h1digipeperdigi->Fill(peperdigi);
	h1digitimeperdigi->Fill(timeperdigi);
	tdigipeperdigi   = peperdigi;
	tdigitimeperdigi = timeperdigi;
      }
      float noise_fraction_total = (float)n_noise_hits_total / (float)(n_noise_hits_total + n_photon_hits_total);
      h1diginoisefrac_trigger->Fill(noise_fraction_total);
      ttotaldiginoisefrac = noise_fraction_total;

      //fill the per trigger output tree
      tout_trig->Fill();
    }//itrigger // End of loop over triggers
    
    // reinitialize super event between loops.
    wcsimrootsuperevent->ReInitialize();

    //fill the per event output tree
    tout->Fill();
  }//ev // End of loop over events
  std::cout<<"num_trig "<<num_trig<<"\n";

  //write the trees
  fout->cd();
  tout->Write();
  tout_trig->Write();

  //save histograms in .root file
  fout->cd();
  h1vtx0->Write();
  h1vtx1->Write();
  h1vtx2->Write();
  h1diginoisefrac->Write();
  h1diginoisefrac_trigger->Write();
  h2nhits_sep->Write();
  h2nhitstrigger_sep->Write();
  h1ndigihits->Write();
  h1nrawhits->Write();
  h1ntubeshitraw->Write();
  h1ntubeshitdigi->Write();
  h1ndigihitstrigger->Write();
  h1digipe->Write();
  h1digitime->Write();
  h1digitime_photon->Write();
  h1digitime_noise->Write();
  h1digitime_mix->Write();
  hSdigitime->Write();
  h1digiplustriggertime->Write();
  h1triggertime->Write();
  h1digipeperdigi->Write();
  h1digitimeperdigi->Write();
  if(hists_per_event) {
    for (int ev=0; ev<nevent; ev++) {
      h1event_hittime[ev][0]->Write();
      h1event_hittime[ev][1]->Write();
      hSevent_hittime[ev]->Write();
    }
  }
  h1hittime->Write();
  h1hittime_photon->Write();
  h1hittime_noise->Write();
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
    h1ndigihits->Draw();
    c2->SaveAs(create_filename("h1ndigihits_", filenameout).Data());
    h1nrawhits->Draw();
    c2->SaveAs(create_filename("h1nrawhits_", filenameout).Data());
    h1ndigihitstrigger->Draw();
    c2->SaveAs(create_filename("h1ndigihitstrigger_", filenameout).Data());
    h1digipe->Draw();
    c2->SaveAs(create_filename("h1digipe_", filenameout).Data());
    h1digitime->Draw();
    c2->SaveAs(create_filename("h1digitime_", filenameout).Data());
    hSdigitime->Draw();
    TLegend * l = new TLegend(0.4,0.9,0.9,1.0);
    l->SetTextSize(0.042);
    l->SetTextFont(132);
    l->AddEntry(h1digitime_photon, "Photon", "l");
    l->AddEntry(h1digitime_noise, "Noise",  "l");
    l->AddEntry(h1digitime_mix, "Mixed",  "l");
    l->Draw();
    c2->SaveAs(create_filename("hSdigitime_", filenameout).Data());
    h1digiplustriggertime->Draw();
    c2->SaveAs(create_filename("h1digiplustriggertime_", filenameout).Data());
    h1digipeperdigi->Draw();
    c2->SaveAs(create_filename("h1digipeperdigi_", filenameout).Data());
    h1digitimeperdigi->Draw();
    c2->SaveAs(create_filename("h1digitimeperdigi_", filenameout).Data());
    h1diginoisefrac->Draw();
    c2->SaveAs(create_filename("h1diginoisefrac_", filenameout).Data());
    h1diginoisefrac_trigger->Draw();
    c2->SaveAs(create_filename("h1diginoisefrac_trigger_", filenameout).Data());

    h2nhits_sep->Draw("COLZ");
    c2->SaveAs(create_filename("h2nhits_sep_", filenameout).Data());
    h2nhitstrigger_sep->Draw("COLZ");
    c2->SaveAs(create_filename("h2nhitstrigger_sep_", filenameout).Data());

    h1triggertype->Draw();
    c2->SaveAs(create_filename("h1triggertype_", filenameout).Data());
    h1triggertime->Draw();
    c2->SaveAs(create_filename("h1triggertime_", filenameout).Data());
  }
  
  return 0;
}
