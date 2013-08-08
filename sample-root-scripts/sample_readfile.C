// Simple example of reading a generated Root file
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
  gSystem.Load("../libWCSimRoot.so");

  // Open the file
  TFile file("wcsim.root");
  
  // Get the a pointer to the tree from the file
  TTree *tree = (TTree*)file->Get("wcsimT");
  
  // Get the number of events
  int nevent = tree->GetEntries();
  printf("nevent %d\n",nevent);
  
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
  geotree.SetBranchAddress("wcsimrootgeom", &geo);
  std::cout << "Geotree has " << geotree.GetEntries() << " entries" << std::endl;
  if (geotree.GetEntries() == 0) {
      exit(9);
  }
  geotree.GetEntry(0);

  // start with the main "subevent", as it contains most of the info
  // and always exists.
  WCSimRootTrigger* wcsimrootevent;

  TH1F *h1 = new TH1F("PMT Hits", "PMT Hits from 1.0 GeV Electron in Geant4", 100, 1000, 9000);
  
  // Now loop over events
  for (int ev=0; ev<nevent; ev++)
  {
    // Read the event from the tree into the WCSimRootEvent instance
    tree->GetEntry(ev);      
    wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
    printf("********************************************************");
    printf("Evt, date %d %d\n", wcsimrootevent->GetHeader()->GetEvtNum(),
	   wcsimrootevent->GetHeader()->GetDate());
    printf("Mode %d\n", wcsimrootevent->GetMode());
    printf("Number of subevents %d\n",
	   wcsimrootsuperevent->GetNumberOfSubEvents());
    
    printf("Vtxvol %d\n", wcsimrootevent->GetVtxvol());
    printf("Vtx %f %f %f\n", wcsimrootevent->GetVtx(0),
	   wcsimrootevent->GetVtx(1),wcsimrootevent->GetVtx(2));
    printf("Jmu %d\n", wcsimrootevent->GetJmu());
    printf("Npar %d\n", wcsimrootevent->GetNpar());
    printf("Ntrack %d\n", wcsimrootevent->GetNtrack());
    
    // Now read the tracks in the event
    
    // Get the number of tracks
    int ntrack = wcsimrootevent->GetNtrack();
    printf("ntracks=%d\n",ntrack);
    
    int i;
    // Loop through elements in the TClonesArray of WCSimTracks
    for (i=0; i<ntrack; i++)
    {
      TObject *element = (wcsimrootevent->GetTracks())->At(i);
      
      WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);
      printf("Track ipnu: %d\n",wcsimroottrack->GetIpnu());
      printf("Track parent ID: %d\n",wcsimroottrack->GetParenttype());
      
      for (int j=0; j<3; j++)
	printf("Track dir: %d %f\n",j, wcsimroottrack->GetDir(j));
      
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
    int ncherenkovdigihits = wcsimrootevent->GetNcherenkovdigihits(); 
    
    h1->Fill(ncherenkovdigihits);
    printf("node id: %i\n", ev);
    printf("Ncherenkovhits %d\n",     ncherenkovhits);
    printf("Ncherenkovdigihits %d\n", ncherenkovdigihits);

    cout << "RAW HITS:" << endl;

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
	printf("Total pe: %d times( ",peForTube);
	for (int j = timeArrayIndex; j < timeArrayIndex + peForTube; j++)
	{
	  WCSimRootCherenkovHitTime HitTime = 
	    dynamic_cast<WCSimRootCherenkovHitTime>(timeArray->At(j));
	  
	  printf("%6.2f ", HitTime->GetTruetime() );	     
	}
	cout << ")" << endl;
      }

    } // End of loop over Cherenkov hits
    cout << "Total Pe : " << totalPe << endl;
    
    // Look at digitized hit info

    // Get the number of digitized hits
    // Loop over sub events
    cout << "DIGITIZED HITS:" << endl;
    for (int index = 0 ; index < wcsimrootsuperevent->GetNumberOfEvents(); index++) 
    {
      wcsimrootevent = wcsimrootsuperevent->GetTrigger(index);
      cout << "Sub event number = " << index << "\n";
      
      int ncherenkovdigihits = wcsimrootevent->GetNcherenkovdigihits();
      printf("Ncherenkovdigihits %d\n", ncherenkovdigihits);
      
      //for (i=0;i<(ncherenkovdigihits>4 ? 4 : ncherenkovdigihits);i++){
      for (i=0;i<ncherenkovdigihits;i++)
      {
    	// Loop through elements in the TClonesArray of WCSimRootCherenkovDigHits
	
    	TObject *element = (wcsimrootevent->GetCherenkovDigiHits())->At(i);
	
    	WCSimRootCherenkovDigiHit *wcsimrootcherenkovdigihit = 
    	  dynamic_cast<WCSimRootCherenkovDigiHit*>(element);

	if ( i < 10 ) // Only print first XX=10 tubes
    	printf("q, t, tubeid: %f %f %d \n",wcsimrootcherenkovdigihit->GetQ(),
    	       wcsimrootcherenkovdigihit->GetT(),wcsimrootcherenkovdigihit->GetTubeId());
	
      } // End of loop over Cherenkov digihits
    } // End of loop over trigger
    
    // reinitialize super event between loops.
    wcsimrootsuperevent->ReInitialize();
    
  } // End of loop over events
  h1.Draw();
}
