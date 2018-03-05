#include <stdio.h>     
#include <stdlib.h>    

  /* A simple script to visualize photons
   * 
   * I like to run this macro as 
   * $ root -l -x 'visuPhotons.C("OD.root")'
   */

double probHitPMT(double distance){
  const double PMTOffset = 100.; // PMT radius, 100mm for 8inch for example
  const double C = 1; // Norm factor from measurements
  if(std::abs(distance)>PMTOffset)
    return C*1/(1+std::pow(std::abs(distance)-PMTOffset,2));
  else
    return 0;
}

void visuPhotons(char const *filename=NULL) {

  gROOT->Reset();
  gStyle->SetOptStat(1);

  //////////////////////////////////////////////////////
  // Load required libraries for processing the macro //
  //////////////////////////////////////////////////////
  
  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIMDIR"); // Recover $WCSIMDIR if define
  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so"); // Load shared WCSim lib
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.rootmap"); // Load rootmap (>ROOT6)
    gSystem->Load("${WCSIMDIR}/src/WCSimRootDict_rdict.pcm"); // Load root dict (>ROOT6)
  }else{
    std::cout << "Can't load WCSim ROOT dictionaries" << std::endl;
  }

  ////////////////////////////
  // Open output WCSim file // 
  ////////////////////////////
  
  TFile *f;
  char fTest[128];
  
  if (filename==NULL){ // If no filename provided try to open a default wcsim.root
    std::cout << "Please provide filename in option" << std::endl; // Please mate
    std::cout << "Will load auto wcsim.root in WCSIMDIR ..." << std::endl;
    char const *name = "wcsim.root";
    strncpy(fTest, wcsimdirenv, sizeof(fTest));
    strncat(fTest, "/", (sizeof(fTest) - strlen(fTest) -1) );
    strncat(fTest, name, (sizeof(fTest) - strlen(fTest) -1) );
    f = new TFile(fTest);
  }else{
    f = new TFile(filename);
  }
  if (!f->IsOpen()){
    std::cout << "ERROR, could not open input file: " << filename << std::endl;
    return -1;
  }else{
    if (filename==NULL) std::cout << "File open : " << fTest << std::endl;
    else std::cout << "File open : " << filename << std::endl;
  }

  /////////////////////////////////////////////////////////
  // Access photons information inside WCSim output file //
  /////////////////////////////////////////////////////////
  
  TTree  *wcsimT = (TTree*)f->Get("wcsimT");
  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
  wcsimT->SetBranchAddress("wcsimrootevent_OD",&wcsimrootsuperevent);
  const int nbEvtsGene = wcsimT->GetEntries();
  std::cout << "TOTAL NB of evts gene " << nbEvtsGene << std::endl;
  std::cout << "Retrieving the average nb of hits per events..." << std::endl;
  const int nbBins = 100;
  const int nbPEMax = 10000;
  TH1D *hPEByEvts = new TH1D("hPEByEvts","Total RAW PE by Evts",nbBins,0,nbPEMax);
  hPEByEvts->GetXaxis()->SetTitle("raw PE");
  hPEByEvts->SetLineColor(kBlue+1);
  hPEByEvts->SetMarkerColor(kBlue+1);
  hPEByEvts->SetFillColor(kBlue+1);  
  for(long unsigned int iEntry = 0; iEntry < wcsimT->GetEntries(); iEntry++){
    // Point to event iEntry inside WCSimTree
    wcsimT->GetEvent(iEntry);
    // Nb of Trigger inside the event
    const unsigned int nbTriggers = wcsimrootsuperevent->GetNumberOfEvents();
    const unsigned int nbSubTriggers = wcsimrootsuperevent->GetNumberOfSubEvents();
    for(long unsigned int iTrig = 0; iTrig < nbTriggers; iTrig++){
      WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(iTrig);
      // RAW HITS
      int rawMax = wcsimrootevent->GetNcherenkovhits();
      int totRawPE = 0;
      for (int i = 0; i < rawMax; i++){
	WCSimRootCherenkovHit *chit = (WCSimRootCherenkovHit*)wcsimrootevent->GetCherenkovHits()->At(i);
	totRawPE+=chit->GetTotalPe(1);	
      }
      hPEByEvts->Fill(totRawPE);
    }
  }
  std::cout << "DONE ! Average raw hits per evts : " << hPEByEvts->GetMean() << std::endl;

  TTree  *WCSimPhotons = (TTree*)f->Get("photons");  // Open photon tree inside file
  if(WCSimPhotons){ // Test if tree is really open
    cout << "Photons tree open" << endl;
  }else{
    std::cout << "Tree NOT open try again mate" << std::endl;
    return -1;
  }

  photonEvt *pEvt = new photonEvt(); // Create new photon structure to access data inside tree
  WCSimPhotons->SetBranchAddress("phoEvt",pEvt); // "phoEvt" is the name of the structure inside output file


  //////////////////////////////////////////
  // Define OF PMT QE for 3inch and 8inch //
  //////////////////////////////////////////

  double PMT8inchQE[20] = { 0.00, .0139, .0854, .169, .203, .206, .211, .202,.188, .167, .140, .116, .0806, .0432, .0265, .0146, .00756, .00508, .00158, 0.00};
  double PMT8inchWL[20] = { 280., 300., 320., 340., 360., 380., 400., 420., 440., 460., 480., 500., 520., 540., 560., 580., 600., 620., 640., 660.};
  TGraph *grPMT8inchQE = new TGraph(20,PMT8inchWL,PMT8inchQE);

  double PMT3inchQE[20] = { 0.00, .005, .09, .21, .28, .30, .29, .28, .26, .24, .22, .18, .13, .075, .04, .02, .008, 0.00, 0.00, 0.00};
  double PMT3inchWL[20] = { 260., 280., 300., 320., 340., 360., 380., 400., 420., 440., 460., 480., 500., 520., 540., 560., 580., 600., 620., 640.};
  TGraph *grPMT3inchQE = new TGraph(20,PMT3inchWL,PMT3inchQE);

  TH1D *emHist;  
  const int emitArraySize = 15; 
  double emitWavelength[emitArraySize] = {390,400,410,420,430,440,450,460,470,480,490,500,510,520,530}; 
  double emit[emitArraySize] = 
      {0.0, 0.1, 0.71, 0.98, 
       0.91, 0.63, 0.49, 0.35, 
       0.20, 0.13, 0.081, 0.052, 
       0.03, 0.021, 0};  
  emHist = new TH1D("emHist","emHist", 15, 390., 530.); 
  emHist->Sumw2(); 
  for (int i = 0 ; i < emitArraySize; i++) { 
    emHist->Fill(emitWavelength[i], emit[i]); 
    emHist->SetBinError(i+1, 0); 
  } 

  const int absArraySize = 15; 
  double absWavelength[absArraySize] = {260,270,280,290,300,310,320,330,340,350,360,370,380,390,400}; 
  double absPlate[absArraySize] =
    {0.54, 0.71, 0.89, 0.97, 0.99,
     0.999, 0.999, 0.999, 0.999, 0.999,
     0.999, 0.93, 0.64, 0.05, 0.01};
  TGraph *grAbs = new TGraph(absArraySize,absWavelength,absPlate);


  //////////////////////////////////////////////////////////
  // Loop over tree entry and create histograms and stuff //
  //////////////////////////////////////////////////////////

  // Photons trajectory
  const double maxX = 40000.; // in mm
  const double maxY = 40000.; // in mm
  const double maxZ = 30000.; // in mm
  const int NbBins = 100;
  TH3D *hTraj = new TH3D("hTraj","Photons trajectory",
			 NbBins,-maxX,maxX, // x in mm
			 NbBins,-maxY,maxY, // y in mm
			 NbBins,-maxZ,maxZ); // z in mm

  TH1D *hDistance = new TH1D("hDistance","Distance from PMT when absorbed",1000,0.,1000.); // in mm
  TH1D *hProbAbs  = new TH1D("hProbAbs","Convoluted distance with Abs Prob",1000,0.,1000.); // in mm
  TH1D *hProbQE   = new TH1D("hProbQE","Prob of photon being detected after reemission",100,0.,.1);
  TH1D *hAbsPMT   = new TH1D("hAbsPMT","Spectrum of WL shifted photon absorbed by the PMT ",30,300,600.);

  
  // Nb of Cer photons produced
  int nbPhCer = 0;
  int previousTrackID = -1;

  // Nb of WL shifted photons produced
  int nbPhWLS = 0;

  // WLS photons wavelength
  TH1D *hWL = new TH1D("hWL","Wavelength shifted photons",30,300,600.);

  // Norm factor for estimate AbsProb by PMT from WLS plate measurements
  double normAbs = 1000;
  TRandom *r = new TRandom(0);
  // Study norm factor
  const int nbFactor = 100;
  TH1D *hAbsPMTNORM[nbFactor];
  double normFactor[nbFactor];
  
  for(int i=0;i<nbFactor;i++){
    hAbsPMTNORM[i] = new TH1D(Form("hAbsPMT%d",i),
			      "Spectrum of WL shifted photon absorbed by the PMT ",
			      30,300,600.);
    normFactor[i] = (i+1)*100;
  }


  const long unsigned int NbEntries = WCSimPhotons->GetEntries(); // Recover nb of tree entries
  std::cout << "Nb of tree entries : " << NbEntries << std::endl;
  
  for(int iEntry=0;iEntry<NbEntries;iEntry++){ // Loop over tree entry
    WCSimPhotons->GetEntry(iEntry);

    hTraj->Fill(pEvt->pos[0],pEvt->pos[1],pEvt->pos[2]);

    if(pEvt->proc == 0){ // ID for photons created via Cerenkov process
      if(previousTrackID != pEvt->trackID){
	nbPhCer++; // Each time trackID changes, we start tracking a new photon
	hDistance->Fill(pEvt->distance);
	hProbAbs->Fill(pEvt->distance,probHitPMT(pEvt->distance));
	double prob =
	  probHitPMT(pEvt->distance) *   // prob associated with photon pos on WLS plate
	  grAbs->Eval(pEvt->wl)      *   // Abs of WLS plate
	  grPMT8inchQE->Eval(pEvt->wl);  // QE of the PMT
	hProbQE->Fill(prob);
	const double trig = r->Uniform();
	double wlEm = emHist->GetRandom();
	if(normAbs * prob > trig) hAbsPMT->Fill(pEvt->wl);
	for(int i=0;i<nbFactor;i++){
	  if(normFactor[i] * prob > trig) hAbsPMTNORM[i]->Fill(wlEm);
	}
      }
      previousTrackID = pEvt->trackID;
    }

    if(pEvt->proc == 1){ // ID for photons created via OpWLS process
      if(previousTrackID != pEvt->trackID){
	nbPhWLS++; // Each time trackID changes, we start tracking a new photon
	hWL->Fill(pEvt->wl);
      }
      previousTrackID = pEvt->trackID;
    }

  }

  //////////////////////////////////////////////////////////
  // Project TH3 to have density of photons per segment   //
  //////////////////////////////////////////////////////////
  // hTraj->GetXaxis()->SetRange(10,NbBins-10);
  // hTraj->GetYaxis()->SetRange(10,NbBins-10);
  TH2D *hXY = (TH2D *)hTraj->Project3D("xy");

  //////////////////////////////////////////////////////////
  // ##################### OUTPUTS ###################### //
  //////////////////////////////////////////////////////////

  // PRINTOUTS
  std::cout << "Nb of Cer photons produced : " << nbPhCer << std::endl;
  std::cout << "Nb of WLS photons produced : " << nbPhWLS << std::endl;
  std::cout << " RATIO : " << 100.*(double)(nbPhWLS)/(double)(nbPhCer) << "%" <<std::endl;
  
  // DRAWINGS
  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  hTraj->Draw();

  TCanvas *cPhoton = new TCanvas("cPhoton","cPhoton",1200,900);
  cPhoton->Divide(2,2);
  cPhoton->cd(1); hWL->Draw();
  cPhoton->cd(2); hDistance->Draw();
  cPhoton->cd(3); gPad->SetLogy(); hProbAbs->Draw();
  cPhoton->cd(4); gPad->SetLogy(); hProbQE->Draw();

  c1 = new TCanvas("cProj","cProj",1200,900);
  hXY->Draw("COLZ");

  c1 = new TCanvas("cP","cP",800,600);
  hAbsPMT->Draw("COLZ");

  std::cout << "Extra photon collected by the PMT : " << hAbsPMT->GetEntries() << std::endl;

  TGraph *gr = new TGraph();
  gr->SetTitle("Evo nb of photon collected VS normFactor; normFactor; Nb Photons");
  gr->SetLineStyle(1);
  gr->SetMarkerStyle(kPlus);
  gr->SetMarkerSize(1.);
  gr->SetMarkerColor(kBlue-4);
  gr->SetLineColor(kBlue-4);
  for(int i=0;i<nbFactor;i++){
    gr->SetPoint(i,(i+1)*100,hAbsPMTNORM[i]->GetEntries()/hPEByEvts->GetMean());
  }
  c1 = new TCanvas("cNorm","cNorm",800,600);
  gPad->SetGrid();
  gr->Draw("APC");
  
} // END MACRO
