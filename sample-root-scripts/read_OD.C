#include <stdio.h>     
#include <stdlib.h>    

  /* A simple script to plot aspects of phototube hits 
   * 
   * I like to run this macro as 
   * $ root -l -x 'read_OD.C("OD.root")'
   */

double err(double p, double q, double t){
  return sqrt(p)/(q*t);
}

double errBinomial(double k, double N){
  return sqrt(k*(1-k/N))/N;
}

Double_t simpleExp(Double_t *x, Double_t *par) {
  Double_t X = x[0];

  Double_t tau = par[0];
  Double_t CExp = par[1];

  return CExp*TMath::Exp(-tau*X);;

}

Double_t simpleGaus(Double_t *x, Double_t *par) {
  Double_t X = x[0];

  Double_t C0 = par[0];
  Double_t Q0 = par[1];
  Double_t sigma0 = par[2];

  Double_t result = 0;
  result = C0*TMath::Exp(-(X-Q0)*(X-Q0)/(2*sigma0*sigma0));

  return result;
}

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress (double percentage){
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush (stdout);
}

// A function to load the libraries required, from WCSim.
void loadlibs(){

  if(const char* wcsimdirenv = std::getenv("WCSIMDIR"))
    std::cout << "Your WCSIMDIR is: " << wcsimdirenv << '\n';

  char libspath[1000];   // array to hold the result.

  const char* libWCSimRootSO = "/libWCSimRoot.so";
  const char* libWCSimRootROOTMAP = "/libWCSimRoot.rootmap";
  const char* libWCSimRootDICT = "/src/WCSimRootDict_rdict.pcm";
  char *wcsimdirenv; 
  wcsimdirenv = getenv("WCSIMDIR"); 
	
  if (wcsimdirenv != NULL) {
    strcpy(libspath,wcsimdirenv); // copy string one into the result.
    strcat(libspath,libWCSimRootSO); // append string two to the result.  
    gSystem->Load(libspath);

    strcpy(libspath,wcsimdirenv); // copy string one into the result.
    strcat(libspath,libWCSimRootROOTMAP); // append string two to the result.  
    gSystem->Load(libspath);

    strcpy(libspath,wcsimdirenv); // copy string one into the result.
    strcat(libspath,libWCSimRootDICT); // append string two to the result.  
    gSystem->Load(libspath);
  } else {
    std::cout << "Error: WCSIMDIR environment variable not set." << std::endl;
  }


}// End of loadlibs function

std::string extractFilename(std::string inFileName){
  
  int index = inFileName.find_last_of("/\\");
  std::string input_trace_filename = inFileName.substr(index+1);
  size_t lastindex = input_trace_filename.find_last_of(".");
  input_trace_filename = input_trace_filename.substr(0, lastindex);

  return input_trace_filename;

} // END of extractFilename()

void read_OD(std::string inFileName = "wcsim.root", std::string outFileName = "") {

  // LOADING WCSim libs
  gROOT->Reset();
  loadlibs();
  gStyle->SetOptStat(1);

  TFile *f;
  f = new TFile(inFileName.c_str());

  if (!f->IsOpen()){
    cout << "Error, could not open input file: " << inFileName << endl;
    return -1;
  } else {
    cout << "File open: " << inFileName << endl;
  }

  // Extract filename from path
  std::string input_trace_filename = extractFilename(inFileName);

  TTree  *wcsimT = (TTree*)f->Get("wcsimT");

  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
  wcsimT->SetBranchAddress("wcsimrootevent_OD",&wcsimrootsuperevent);

  // Force deletion to prevent memory leak when issuing multiple
  // calls to GetEvent()
  wcsimT->GetBranch("wcsimrootevent_OD")->SetAutoDelete(kTRUE);

  const long unsigned int nbEntries = wcsimT->GetEntries();
  std::cout << "Nb of entries " << wcsimT->GetEntries() << std::endl;

  // Create an output file
  TFile *output=NULL;

  if(outFileName == ""){
    outFileName = input_trace_filename + "_PROCESSED.root";
    output = new TFile(outFileName.c_str(),"recreate");
  } else {
    output = new TFile(outFileName.c_str(),"recreate");
  }

  std::cout << "outFileName: " << outFileName << std::endl;

  TTree *outTree = new TTree("simulation", "simulation");

  int event;
  float vtxX, vtxY, vtxZ; // vertex coordinate
  float dirX, dirY, dirZ; // particle momentum direction
  float phi, theta; // angles of the particle's momentum
  float energy;

  // Set the branches to be saved
  outTree->Branch("event", &event, "event/I");
  outTree->Branch("vtxX", &vtxX, "vtxX/F");
  outTree->Branch("vtxY", &vtxY, "vtxY/F");
  outTree->Branch("vtxZ", &vtxZ, "vtxZ/F");
  outTree->Branch("dirX", &dirX, "dirX/F");
  outTree->Branch("dirY", &dirY, "dirY/F");
  outTree->Branch("dirZ", &dirZ, "dirZ/F");
  outTree->Branch("phi", &phi, "phi/F");
  outTree->Branch("theta", &theta, "theta/F");
  outTree->Branch("energy", &energy, "energy/F");
      
  //////////////////////////////////////////
  // VETO POWER            /////////////////
  //////////////////////////////////////////

  int nDet = 0;

  //////////////////////////////////////////
  // HISTOGRAMS DEFINITION /////////////////
  //////////////////////////////////////////

  const int nbBins = 10001;
  const int nbPEMax = 10000.5;
  const int nbBinsByPMT = 501;
  const int nbPEMaxByPMT = 500.5;

  const double cutPEByPMT = 0.;
  
  TH1D *hPEByEvtsByPMT = new TH1D("hPEByEvtsByPMT","RAW PE by Evts by PMT",
				  nbBinsByPMT,-0.5,nbPEMaxByPMT);
  hPEByEvtsByPMT->GetXaxis()->SetTitle("raw PE");
  hPEByEvtsByPMT->SetLineColor(kBlue-4);
  hPEByEvtsByPMT->SetMarkerColor(kBlue-4);  
  TH1D *hPECollectedByEvtsByPMT = new TH1D("hPECollectedByEvtsByPMT","collected PE by Evts by PMT",
					   nbBinsByPMT,-0.5,nbPEMaxByPMT);
  hPECollectedByEvtsByPMT->GetXaxis()->SetTitle("digi PE");
  hPECollectedByEvtsByPMT->SetLineColor(kRed-4);
  hPECollectedByEvtsByPMT->SetMarkerColor(kRed-4);
  
  TH1D *hPEByEvts = new TH1D("hPEByEvts","Total RAW PE by Evts",nbBins,-0.5,nbPEMax);
  hPEByEvts->GetXaxis()->SetTitle("raw PE");
  hPEByEvts->SetLineColor(kBlue+1);
  hPEByEvts->SetMarkerColor(kBlue+1);
  hPEByEvts->SetFillColor(kBlue+1);  
  TH1D *hPECollectedByEvts = new TH1D("hPECollectedByEvts","Total collected PE by Evts",nbBins,-0.5,nbPEMax);
  hPECollectedByEvts->GetXaxis()->SetTitle("digi PE");
  hPECollectedByEvts->SetLineColor(kRed+1);
  hPECollectedByEvts->SetMarkerColor(kRed+1);
  hPECollectedByEvts->SetFillColor(kRed+1);

  TH1D *hNbTubesHit = new TH1D("hNbTubesHit","Nb of Tubes Hit",10001,-0.5,10000.5);

  TH2D *hQvsT = new TH2D("hQvsT","charge vs. time",
			 160, 0, 2000,
			 nbBins, -0.5, nbPEMax);
  hQvsT->SetXTitle("time");
  hQvsT->SetYTitle("charge");

  TH2D *hQvsNbPMTsHits = new TH2D("hQvsNbPMTsHits","Nb PE vs. Nb PMTs Hits",
				  1001, -0.5, 1000.5,
				  nbBins, -0.5, nbPEMax);
  hQvsNbPMTsHits->SetXTitle("Nb PMTs Hits");
  hQvsNbPMTsHits->SetYTitle("Nb PE");

 
  // END HISTOGRAMS DEFINITION /////////////
  //////////////////////////////////////////

  // FILL TTree
  // Make a first loop so ->Fill method doesn't copy histogram over and over again

  std::cout << "Start First loop for filling TTree" << std::endl;

  for(long unsigned int iEntry = 0; iEntry < nbEntries; iEntry++){
    // Point to event iEntry inside WCSimTree
    wcsimT->GetEvent(iEntry);
    event = iEntry;
    // Nb of Trigger inside the event
    const unsigned int nbTriggers = wcsimrootsuperevent->GetNumberOfEvents();
    const unsigned int nbSubTriggers = wcsimrootsuperevent->GetNumberOfSubEvents();

    // cout << "iEntry : " << iEntry << endl;
    // cout << "nbTrig : " << nbTriggers << endl;
    // cout << "nbSubTrig : " << nbSubTriggers << endl;
    
    for(long unsigned int iTrig = 0; iTrig < nbTriggers; iTrig++){
      WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(iTrig);

      int numTracks = wcsimrootevent->GetNtrack();
      WCSimRootTrack * track = (WCSimRootTrack*) wcsimrootevent->GetTracks()->At(0);
		
      vtxX = wcsimrootevent->GetVtx(0);	
      vtxY = wcsimrootevent->GetVtx(1);	
      vtxZ = wcsimrootevent->GetVtx(2);	
      dirX = track->GetDir(0);
      dirY = track->GetDir(1);
      dirZ = track->GetDir(2);
      energy = track->GetE();	
      theta = abs( atan( dirX/dirZ ) );
      phi = abs( atan( dirY/dirX) );


      outTree->Fill();
    }

    printProgress((double)(iEntry)/(double)(nbEntries));
    
  }

  std::cout << std::endl;

  outTree->Write();

  // THEN make an other loop to recover the info for plotting the hists

  std::cout << "Start Second loop for making plots" << std::endl;

  for(long unsigned int iEntry = 0; iEntry < nbEntries; iEntry++){
    // Point to event iEntry inside WCSimTree
    wcsimT->GetEvent(iEntry);
    
    // Nb of Trigger inside the event
    const unsigned int nbTriggers = wcsimrootsuperevent->GetNumberOfEvents();
    const unsigned int nbSubTriggers = wcsimrootsuperevent->GetNumberOfSubEvents();

    // cout << "iEntry : " << iEntry << endl;
    // cout << "nbTrig : " << nbTriggers << endl;
    // cout << "nbSubTrig : " << nbSubTriggers << endl;
    
    for(long unsigned int iTrig = 0; iTrig < nbTriggers; iTrig++){
      WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(iTrig);
		
      // RAW HITS
      int rawMax = wcsimrootevent->GetNcherenkovhits();
      int NbOfTubesWithHits = 0;
      int totRawPE = 0;
      for (int i = 0; i < rawMax; i++){
	WCSimRootCherenkovHit *chit = (WCSimRootCherenkovHit*)wcsimrootevent->GetCherenkovHits()->At(i);
	
	if(chit->GetTotalPe(1) > cutPEByPMT) hPEByEvtsByPMT->Fill(chit->GetTotalPe(1));
	totRawPE+=chit->GetTotalPe(1);
	NbOfTubesWithHits++;
      } // END FOR RAW HITS

      // hNbTubesHit->Fill(rawMax);
      hNbTubesHit->Fill(NbOfTubesWithHits);
      hPEByEvts->Fill(totRawPE);

      if(totRawPE>0) nDet++;

      // DIGI HITS
      int digiMax = wcsimrootevent->GetNcherenkovdigihits();
      int totDigiPE = 0;
      for (int i = 0; i < digiMax; i++){
	WCSimRootCherenkovDigiHit *cDigiHit =
	  (WCSimRootCherenkovDigiHit*)wcsimrootevent->GetCherenkovDigiHits()->At(i);
	//WCSimRootChernkovDigiHit has methods GetTubeId(), GetT(), GetQ()
	WCSimRootCherenkovHitTime *cHitTime =
	  (WCSimRootCherenkovHitTime*)wcsimrootevent->GetCherenkovHitTimes()->At(i);
	//WCSimRootCherenkovHitTime has methods GetTubeId(), GetTruetime()

	hQvsT->Fill(cDigiHit->GetT(), cDigiHit->GetQ());

	if(cDigiHit->GetQ() > cutPEByPMT) hPECollectedByEvtsByPMT->Fill(cDigiHit->GetQ());
	totDigiPE+=cDigiHit->GetQ();
      } // END FOR DIGI HITS

      hPECollectedByEvts->Fill(totDigiPE);

      hQvsNbPMTsHits->Fill(digiMax, totDigiPE);

      printProgress((double)(iEntry)/(double)(nbEntries));

    } // END FOR iTRIG
    
  } // END FOR iENTRY

  std::cout << std::endl;

  //////////////////////////////////////////
  // FIT ///////////////////////////////
  //////////////////////////////////////////
  
  TF1 *fPEByEvt = new TF1("FitPEByEvt",simpleExp,cutPEByPMT,30,2);
  fPEByEvt->SetParameter(0,hPEByEvtsByPMT->GetMaximum());
  fPEByEvt->SetParameter(1,1/hPEByEvtsByPMT->GetMean());

  fPEByEvt->SetParNames ("C0","Tau");

  //////////////////////////////////////////
  // DRAWING ///////////////////////////////
  //////////////////////////////////////////

  TCanvas *c1;

  c1 = new TCanvas("cPE","cPE",800,600);
  c1->Divide(2,2);
  c1->cd(1);
  gPad->SetLogy();
  hPEByEvtsByPMT->Draw("HIST");
  // hPEByEvtsByPMT->Fit("FitPEByEvt","LEMR+");
  hPEByEvtsByPMT->Fit("expo","+","SAME");
  c1->cd(2);
  hPEByEvts->Draw(""); // hPEByEvts->Fit("gaus");
  c1->cd(3);
  gPad->SetLogy();
  hPECollectedByEvtsByPMT->Draw("HIST");
  hPECollectedByEvtsByPMT->Fit("expo","+","SAME");
  c1->cd(4);  
  hPECollectedByEvts->Draw("HIST");

  // TF1 *fit = hPEByEvts->GetFunction("gaus");
  
  c1 = new TCanvas("cNbTubesHit","cNbTubesHit",800,600);
  hNbTubesHit->Draw("HIST");

  c1 = new TCanvas("cQvsT","cQvsT",800,600);
  hQvsT->Draw("COLZ");

  c1 = new TCanvas("cQvsNbPMTsHits","cQvsNbPMTsHits",800,600);
  hQvsNbPMTsHits->Draw("COLZ");

  cout << "Mean nb of tubes hit by events : " << hNbTubesHit->GetMean()
       << " +- " << hNbTubesHit->GetRMS() << endl;
  cout << "Mean raw PE by events by PMT : " << hPEByEvtsByPMT->GetMean()
       << " +- " << hPEByEvtsByPMT->GetRMS() << endl;  
  cout << "Mean PE collected by events by PMT : " << hPECollectedByEvtsByPMT->GetMean()
       << " +- " << hPECollectedByEvtsByPMT->GetRMS() << endl;
  cout << "Mean raw PE by events : " << hPEByEvts->GetMean()
       << " +- " << hPEByEvts->GetRMS() << endl;  
  cout << "Mean PE collected by events : " << hPECollectedByEvts->GetMean()
       << " +- " << hPECollectedByEvts->GetRMS() << endl;
  cout << "VETO Efficiency : " << (double)(nDet)/nbEntries
       << " +- " << errBinomial((double)(nDet),nbEntries) << endl;
  // cout << "FIT : " << fit->GetParameter(1)
  //      << " +- " << fit->GetParError(1) << endl;  

 
  hPEByEvtsByPMT->Write();
  hPEByEvts->Write();
  hPECollectedByEvtsByPMT->Write();
  hPECollectedByEvts->Write();
  hNbTubesHit->Write();
  hQvsT->Write();
  hQvsNbPMTsHits->Write();
  
  // output->Close();

} // END MACRO
