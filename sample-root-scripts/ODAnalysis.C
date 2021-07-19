// C++ Includes
#include <iostream>


// ROOT Includes
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"

// WCSim Includes
#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"

// Defines
#define PI 3.141592654


/*
* To run this script just type:  $WCSIMDIR/rootwc/rootwc -l -x 'ODAnalysis.C("wcsim.root", "ODHits.root", false)' 
* or replace wcsim.root and ODHits.root for your input and output filenames respectively
* The final true can be changed to true to turn on verbosity.
* The default: root -l -x llib.C ODAnalysis.C will run with the variables mentioned above
*/



// A function to convert radians to degress
float RadToDeg(float x){
        return x*180/PI;
}
// A function to convert degress to radians
float DegToRad(float x){
        return x*PI/180;
}


void ODAnalysis( const char *inFileName = "wcsim.root", const char *outFileName = "ODHits.root", bool verbosity = 0){ 
	

	bool createCanvases = true;  // If true then all 2D histograms will be drawn onto canvases with the cols options and saved to the output root file.


	// Some nicely formatted text options
	std::cout << std::scientific; // This causes all numbers to be displayed in scientific notation.
	std::cout << std::setprecision(2); // Sets the decimal precision (no more than two decimal places)
	std::cout << std::left; // Sets the text justification to left
	const int txtW = 20; // Width of "box" holding text
	const int numW = 10; // Width of "box" holding numbers


	// Open the WCSim file
	TFile *inFile = new TFile(inFileName, "READ"); 
	if ( !inFile->IsOpen() ){
		std::cout << "Error: could not open input file \"" << inFileName << "\"." <<std::endl; 
	
	} else if (verbosity) {
		std::cout << "Input file: " << inFileName << std::endl;
	}
	


	// Get a pointer to the tree from the input file
	TTree *wcsimTree = (TTree*) inFile->Get("wcsimT");
	
	// Get the number of events in the tree
	const long int nEvent = wcsimTree->GetEntries();
	if (verbosity) { std::cout << "Number of events: "<< nEvent << std::endl;}

	// Create a WCSimRootEvent to put stuff from the tree in
	WCSimRootEvent *wcsimRoot = new WCSimRootEvent();
	
	// Set the branch address for reading from the tree
	TBranch *branch = wcsimTree->GetBranch("wcsimrootevent_OD");
	branch->SetAddress(&wcsimRoot);

	// Force deletion to prevent memory leak
	wcsimTree->GetBranch("wcsimrootevent_OD")->SetAutoDelete(kTRUE); 

	// Load the geometry tree (only 1 "event")
	TTree* geoTree = (TTree*) inFile->Get("wcsimGeoT"); 
	WCSimRootGeom *geo = 0;
	geoTree->SetBranchAddress("wcsimrootgeom", &geo);
	if (verbosity) {std::cout << "Geotree has " << geoTree->GetEntries() << " entries." << std::endl;}	
	
	// Start with the main trigger as it always exists and contains most of the info 
	WCSimRootTrigger *wcsimTrigger; 
	
	// Create an output file
	TFile *outFile = new TFile(outFileName, "RECREATE");
	TTree *outBranch = new TTree("simulation", "simulation");

	

	// Variables to store in the root file	
	int event; // event number
	float vtxX, vtxY, vtxZ; // vertex coordinate
	float dirX, dirY, dirZ; // particle momentum direction
	float phi, theta; // angles of the particle's momentum
	float energy; // particle energy
	int rawHits; // number of raw pmt hits
	int digiHits; // number of pmt digitised hits
	int numPMTsHit; // Number of PMTs hit	
	int numPMTsDigiHit; // Number of PMTs with digihits	
	
	// Set the branches to be saved
	outBranch->Branch("event", &event, "event/I");
	outBranch->Branch("vtxX", &vtxX, "vtxX/F");
	outBranch->Branch("vtxY", &vtxY, "vtxY/F");
	outBranch->Branch("vtxZ", &vtxZ, "vtxZ/F");
	outBranch->Branch("dirX", &dirX, "dirX/F");
	outBranch->Branch("dirY", &dirY, "dirY/F");
	outBranch->Branch("dirZ", &dirZ, "dirZ/F");
	outBranch->Branch("phi", &phi, "phi/F");
	outBranch->Branch("theta", &theta, "theta/F");
	outBranch->Branch("energy", &energy, "energy/F");
	outBranch->Branch("rawHits", &rawHits, "rawHits/I");
	outBranch->Branch("digiHits", &digiHits, "digiHits/I");
	outBranch->Branch("numPMTsHit", &numPMTsHit, "numPMTsHit/I");

	// Create histograms of interest
	const int nBinsHits = 500; // number of bins for the hits histograms
	const int hitsMin = 500; // min number of hits for the hits histograms
	const int hitsMax = 500; // max number of hits for the hits histograms
	const int nBinsAngle = 64; // number of bins for the theta and phi angle histograms
	const float angleMin = 0; // minimum angle for the theta and phi angle histograms
	const float angleMax = 1.6; // maximum angle for the theta and phi angle histograms

	// 1D histograms
	TH1F *totalRawHitsPerEvent = new TH1F("totalRawHitsPerEvent", "totalRawHitsPerEvent", nBinsHits, hitsMin, hitsMax );
	TH1F *totalDigiHitsPerEvent = new TH1F("totalDigiHitsPerEvent", "totalDigiHitsPerEvent", nBinsHits, hitsMin, hitsMax );
	TH1F *rawHitsPerPMT = new TH1F("rawHitsPerPMT", "rawHitsPerPMT", nBinsHits, hitsMin, hitsMax );
	TH1F *digiHitsPerPMT = new TH1F("digiHitsPerPMT", "digiHitsPerPMT", nBinsHits, hitsMin, hitsMax );
	TH1F *numPMTsHitHist = new TH1F("numPMTsHit", "numPMTsHit", nBinsHits, hitsMin, hitsMax );
	// 2D histograms
	TH2F *rawHitsPMTVsTheta = new TH2F("rawHitsPMTVsTheta", "rawHitsPMTVsTheta", nBinsAngle , angleMin, angleMax, nBinsHits, hitsMin, hitsMax );
	TH2F *rawHitsPMTVsPhi = new TH2F("rawHitsPMTVsPhi", "rawHitsPMTVsPhi", nBinsAngle , angleMin, angleMax, nBinsHits, hitsMin, hitsMax );
	TH2F *digiHitsPMTVsTheta = new TH2F("digiHitsPMTVsTheta", "digiHitsPMTVsTheta", nBinsAngle , angleMin, angleMax, nBinsHits, hitsMin, hitsMax );
	TH2F *digiHitsPMTVsPhi = new TH2F("digiHitsPMTVsPhi", "digiHitsPMTVsPhi", nBinsAngle , angleMin, angleMax, nBinsHits, hitsMin, hitsMax );
	TH2F *numHitPMTsVsTheta = new TH2F("numHitPMTsVsTheta", "numHitPMTsVsTheta", nBinsAngle , angleMin, angleMax, nBinsHits, hitsMin, hitsMax );
	TH2F *numHitPMTsVsPhi = new TH2F("numHitPMTsVsPhi", "numHitPMTsVsPhi", nBinsAngle , angleMin, angleMax, nBinsHits, hitsMin, hitsMax );
	TH2F *totalRawHitsVsTheta = new TH2F("totalRawHitsVsTheta", "totalRawHitsVsTheta", nBinsAngle , angleMin, angleMax, nBinsHits, hitsMin, hitsMax );
	TH2F *totalRawHitsVsPhi = new TH2F("totalRawHitsVsPhi", "totalRawHitsVsPhi", nBinsAngle , angleMin, angleMax, nBinsHits, hitsMin, hitsMax );
	TH2F *totalDigiHitsVsTheta = new TH2F("totalDigiHitsVsTheta", "totalDigiHitsVsTheta", nBinsAngle , angleMin, angleMax, nBinsHits, hitsMin, hitsMax );
	TH2F *totalDigiHitsVsPhi = new TH2F("totalDigiHitsVsPhi", "totalDigiHitsVsPhi", nBinsAngle , angleMin, angleMax, nBinsHits, hitsMin, hitsMax );




	for (long int ev = 0; ev < nEvent; ev++){ // Loop over events

		wcsimTree->GetEntry(ev);
		wcsimTrigger = wcsimRoot->GetTrigger(0);
		int numTriggers = wcsimRoot->GetNumberOfEvents();
		int numSubTriggers = wcsimRoot->GetNumberOfSubEvents();	
		event = ev;

		if (verbosity) { // output the information of each event
	
			 std::cout << "=================================================" << std::endl; 
			 std::cout <<  std::left << std::setw(txtW) << "Event:" << std::right << std::setw(numW) << ev << std::endl; 
			 std::cout <<  std::left << std::setw(txtW) << "Triggers:     " <<  std::right << std::setw(numW) << numTriggers << std::endl; 
			 std::cout <<  std::left << std::setw(txtW) << "Sub Triggers: " <<  std::right << std::setw(numW) << numSubTriggers << std::endl; 

		} // End of if statement
		
		for (int nTrig = 0; nTrig < numTriggers; nTrig++){

			wcsimTrigger = wcsimRoot->GetTrigger(nTrig); 
			int numTracks = wcsimTrigger->GetNtrack();
			WCSimRootTrack * track = (WCSimRootTrack*) wcsimTrigger->GetTracks()->At(0);
		
			vtxX = wcsimTrigger->GetVtx(0);	
			vtxY = wcsimTrigger->GetVtx(1);	
			vtxZ = wcsimTrigger->GetVtx(2);	
			dirX = track->GetDir(0);
			dirY = track->GetDir(1);
			dirZ = track->GetDir(2);
			energy = track->GetE();	
			theta = abs( atan( dirX/dirZ ) );
			phi = abs( atan( dirY/dirX) );
			rawHits = 0;
			digiHits = 0;

			numPMTsHit = wcsimTrigger->GetNcherenkovhits(); //Returns the number of PMTs with a true hit (photon or dark noise) (QE applied)
			numPMTsDigiHit = wcsimTrigger->GetNcherenkovdigihits(); //Returns the number of PMTs with a true hit (photon or dark noise) (QE applied)

			// Work out the number of photons that hit the PMTs
			for (int i = 0; i < numPMTsHit; i++){
			
			WCSimRootCherenkovHit *cherenkovHit = (WCSimRootCherenkovHit*) wcsimTrigger->GetCherenkovHits()->At(i);
			float tmpRawHits = cherenkovHit->GetTotalPe(1);
			rawHits += tmpRawHits;
		
			rawHitsPerPMT->Fill(tmpRawHits);
			rawHitsPMTVsTheta->Fill(theta, tmpRawHits);			
			rawHitsPMTVsPhi->Fill(phi, tmpRawHits);			

			} // End of for loop working out number of photons in PMTs

			// Work out the number of digitised hits in the PMTs
			for (int i = 0; i < numPMTsDigiHit; i++){
			
			WCSimRootCherenkovDigiHit *cherenkovDigiHit = (WCSimRootCherenkovDigiHit*) wcsimTrigger->GetCherenkovDigiHits()->At(i);
			float tmpDigiHits = cherenkovDigiHit->GetQ();
			
			digiHits += tmpDigiHits;
			
			digiHitsPerPMT->Fill(tmpDigiHits);
			digiHitsPMTVsTheta->Fill(theta, tmpDigiHits);			
			digiHitsPMTVsPhi->Fill(phi, tmpDigiHits);			

			} // End of for loop working out number of digitized hits in PMTs

			if (verbosity) { // output the information of each event

				std::cout <<  std::left << std::setw(txtW) << "VTX (x y z)[cm]:" <<  std::right << std::setw(numW) << vtxX << " " <<  std::setw(numW) << vtxY << " " <<  std::setw(numW) << vtxZ << std::endl; 
				std::cout <<  std::left << std::setw(txtW) << "Dir (x y z)[norm]:" <<  std::right << std::setw(numW) << dirX << " " <<  std::setw(numW) << dirY << " " <<  std::setw(numW) << dirZ << std::endl; 
				std::cout <<  std::left << std::setw(txtW) << "Theta [rad]:" <<  std::right << std::setw(numW) << theta << std::endl; 
				std::cout <<  std::left << std::setw(txtW) << "Phi [rad]:" <<  std::right << std::setw(numW) << phi << std::endl; 
				std::cout <<  std::left << std::setw(txtW) << "PMTs Hit Digi:" <<  std::right << std::setw(numW) << numPMTsDigiHit << std::endl; 
				std::cout <<  std::left << std::setw(txtW) << "PMTs Hit Raw:" <<  std::right << std::setw(numW) << numPMTsHit << std::endl; 
				//std::cout << "PMTs Hit:     " << numPMTsHit << std::endl; 
				std::cout <<  std::left << std::setw(txtW) << "Raw Hits:" <<  std::right << std::setw(numW) << rawHits << std::endl; 
				std::cout <<  std::left << std::setw(txtW) << "Digi Hits:" <<  std::right << std::setw(numW) << digiHits << std::endl; 
				std::cout <<  std::left << std::setw(txtW) << "Tracks:" <<  std::right << std::setw(numW) << numTracks << std::endl; 
				std::cout <<  std::left << std::setw(txtW) << "Energy [MeV]:" <<  std::right << std::setw(numW) << energy << std::endl; 
				std::cout << std::left ; // change text justification back to left.	 
			} // End of if statement


		totalRawHitsPerEvent->Fill(rawHits);
		totalDigiHitsPerEvent->Fill(digiHits);
		numPMTsHitHist->Fill(numPMTsHit);
		numHitPMTsVsTheta->Fill(theta, numPMTsHit);
		numHitPMTsVsPhi->Fill(phi, numPMTsHit);
		totalRawHitsVsTheta->Fill(theta, rawHits);
		totalRawHitsVsPhi->Fill(phi, rawHits);
		totalDigiHitsVsTheta->Fill(theta, digiHits);
		totalDigiHitsVsPhi->Fill(phi, digiHits);
		outBranch->Fill();
		


		} // End of loop over triggers



	} // End of loop over events

	if (createCanvases) {
		
		// Lets show the 2D histograms on some canvases.
		TCanvas *c1 = new TCanvas ("rawHitsPMTVsThetaC", "rawHitsPMTVsThetaC");
		rawHitsPMTVsTheta->Draw("cols");
		c1->Write();
	
		TCanvas *c2 = new TCanvas ("rawHitsPMTVsPhi","rawHitsPMTVsPhi");
	        rawHitsPMTVsPhi->Draw("cols");
		c2->Write();
	 
		TCanvas *c3 = new TCanvas ("digiHitsPMTVsTheta","digiHitsPMTVsTheta");
	        digiHitsPMTVsTheta->Draw("cols");
		c3->Write();
	
		TCanvas *c4 = new TCanvas ("digiHitsPMTVsPhi","digiHitsPMTVsPhi");
	        digiHitsPMTVsPhi->Draw("cols"); 
		c4->Write();
	
		TCanvas *c5 = new TCanvas ("numHitPMTsVsTheta","numHitPMTsVsTheta");
	        numHitPMTsVsTheta->Draw("cols"); 
		c5->Write();
	
		TCanvas *c6 = new TCanvas ("numHitPMTsVsPhi","numHitPMTsVsPhi");
	        numHitPMTsVsPhi->Draw("cols"); 
		c6->Write();
	
		TCanvas *c7 = new TCanvas ("totalRawHitsVsTheta","totalRawHitsVsTheta");
	        totalRawHitsVsTheta->Draw("cols");
		c7->Write();
	
		TCanvas *c8 = new TCanvas ("totalRawHitsVsPhi","totalRawHitsVsPhi");
	        totalRawHitsVsPhi->Draw("cols");
		c8->Write();
	
		TCanvas *c9 = new TCanvas ("totalDigiHitsVsTheta","totalDigiHitsVsTheta");
	        totalDigiHitsVsTheta->Draw("cols");
		c9->Write();
	
		TCanvas *c10 = new TCanvas ("totalDigiHitsVsPhi","totalDigiHitsVsPhi");
	        totalDigiHitsVsPhi->Draw("cols");
	
	}


	outFile->Write(); // Write all of objects to the output file.
	outFile->Close(); // Close the output file.






}




/*
* Notes, 
* Add number of raw/digi hits per PMT
*
*
*/
