// Simple example of reading a generated Root file
{
  // Clear global scope
  gROOT->Reset();

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

  // Load the library with class dictionary info
  // (create with "gmake shared")
  gSystem.Load("../libWCSimRoot.so");

  // Open the file
  TFile file("../wcsim.root");
  
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

  // start with the main "subevent", as it contains most of the info
  // and always exists.
  WCSimRootTrigger* wcsimrootevent;

 TH1F *h1 = new TH1F("PMT Hits on 1.5 GeV Muon in Geant4", "PMT Hits on 1.5 GeV Muon in Geant4", 3000, 3000, 9000);


  // Now loop over events
  int ev;
  for (ev=0;ev<nevent; ev++)
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
      for (i=0;i<ntrack;i++){
	
	TObject *element = (wcsimrootevent->GetTracks())->At(i);

	WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);
	printf("Track ipnu: %d\n",wcsimroottrack->GetIpnu());
	printf("Track parent ID: %d\n",wcsimroottrack->GetParenttype());
	int j;
		for (j=0;j<3;j++)
		  {
		    printf("Track dir: %d %f\n",j, wcsimroottrack->GetDir(j));
		  }
      }  // End of loop over tracks
      
      // Now look at the Cherenkov hits

      // Get the number of Cherenkov hits
		int ncherenkovhits = wcsimrootevent->GetNcherenkovhits();
		int ncherenkovdigihits = wcsimrootevent->GetNcherenkovdigihits(); //GetNcherenkovhits();
		h1.Fill(ncherenkovdigihits);
		printf("node id: %i\n", ev);
	   printf("Ncherenkovhits %d\n", ncherenkovhits);
	   printf("Ncherenkovdigihits %d\n", ncherenkovdigihits);

      // Loop through elements in the TClonesArray of WCSimRootCherenkovHits

      /*      for (i=0;i<(ncherenkovhits>4 ? 4 : ncherenkovhits);i++){

	TObject *element = (wcsimrootevent->GetCherenkovHits())->At(i);
	
	WCSimRootCherenkovHit *wcsimrootcherenkovhit = dynamic_cast<WCSimRootCherenkovHit*>(element);
	int totpe = wcsimrootcherenkovhit->GetTotalPe();
	printf("Total pe: %d\n",totpe);
	int j;
	for (j=0;j<(totpe<3 ? totpe : 3);j++)
	{
	  printf("Truetime: %d %f\n",j, wcsimrootcherenkovhit->GetTruetime(j));
	}
      } // End of loop over Cherenkov hits
      */
      // Look at digitized hit info

      // Get the number of digitized hits
      /*
      for (int index = 0 ; index < wcsimrootsuperevent->GetNumberOfEvents() ; 
	   index++) {

	wcsimrootevent = wcsimrootsuperevent->GetTrigger(index);
	cout << "Sub event number = " << index << "\n";
	
	int ncherenkovdigihits = wcsimrootevent->GetNcherenkovdigihits();
	printf("Ncherenkovdigihits %d\n", ncherenkovdigihits);
	
	//	for (i=0;i<(ncherenkovdigihits>4 ? 4 : ncherenkovdigihits);i++){
	for (i=0;i<ncherenkovdigihits;i++){
	  
	  // Loop through elements in the TClonesArray of WCSimRootCherenkovDigHits
	  
	  TObject *element = (wcsimrootevent->GetCherenkovDigiHits())->At(i);
	  
	  WCSimRootCherenkovDigiHit *wcsimrootcherenkovdigihit = dynamic_cast<WCSimRootCherenkovDigiHit*>(element);
	  printf("q, t, tubeid: %f %f %d \n",wcsimrootcherenkovdigihit->GetQ(),
		 wcsimrootcherenkovdigihit->GetT(),wcsimrootcherenkovdigihit->GetTubeId());
	  
	} // End of loop over Cherenkov digihits
	
      } // End of loop over trigger
      */
      // reinitialize super event between loops.
      wcsimrootsuperevent->ReInitialize();
      
    } // End of loop over events
	h1.Draw();
}
