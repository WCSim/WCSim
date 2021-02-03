// C++ Includes
#include <iostream>
//#include <algorithm>
//#include <iomanip>
#include <cmath>

// ROOT Includes
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
//#include "TApplication.h"


// WCSim Includes
#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"

// Defines
#define PI 3.141592654

/*
*How to run:
*
* enter in the terminal root -l llib.C 'read_OD.C("WCSim.root","outputFile.root",false)' to run the code
* where you replace WCSim.root with your file name and outputfile with the name you wish to save it under
*/

// A function to easily format the histograms
void format_histogram(TH1D* h, std::string title ,std::string xtitle, std::string ytitle){
	h->SetTitle( title.c_str() );
	h->GetXaxis()->SetTitle( xtitle.c_str() );
	h->GetYaxis()->SetTitle( ytitle.c_str() );
	h->SetLineColor(kRed);
	h->SetLineWidth(2);
}
// An (overloaded) function to easily format the histograms
void format_histogram(TH2D* h, std::string title ,std::string xtitle, std::string ytitle){
	h->SetTitle( title.c_str() );
	h->GetXaxis()->SetTitle( xtitle.c_str() );
	h->GetYaxis()->SetTitle( ytitle.c_str() );
}

void read_OD(std::string inFileName, std::string outFileName, bool verbosity){


	// Some nicely formatted text options
	std::cout << std::scientific; // This causes all numbers to be displayed in scientific notation.
	std::cout << std::setprecision(2); // Sets the decimal precision (no more than two decimal places)
	std::cout << std::left; // Sets the text justification to left
	const int txtW = 20; // Width of "box" holding text
	const int numW = 10; // Width of "box" holding numbers


	// Open the WCSim file
	TFile *inFile = new TFile(inFileName.c_str(), "READ");
	if ( !inFile->IsOpen() ){
		std::cout << "Error: could not open input file \"" << inFileName << "\"." <<std::endl;

	} else if (verbosity) {
		std::cout << "Input file: " << inFileName << std::endl;
	}



	// Get a pointer to the tree from the input file
	TTree *wcsimTree = (TTree*) inFile->Get("wcsimT");

	// Get the number of events in the tree
	long int nEvent = wcsimTree->GetEntries();
	if (verbosity) { std::cout << "Number of events: "<< nEvent << std::endl;}

	// Create a WCSimRootEvent to put stuff from the tree in
	WCSimRootEvent *wcsimRootID = new WCSimRootEvent();
	WCSimRootEvent *wcsimRootOD = new WCSimRootEvent();

	// Set the branch address for reading from the tree
	TBranch *branch = wcsimTree->GetBranch("wcsimrootevent");
	TBranch *branchOD = wcsimTree->GetBranch("wcsimrootevent_OD");
	branch->SetAddress(&wcsimRootID);
	branchOD->SetAddress(&wcsimRootOD);

	// Force deletion to prevent memory leak
	wcsimTree->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);
	wcsimTree->GetBranch("wcsimrootevent_OD")->SetAutoDelete(kTRUE);

	// Load the geometry tree (only 1 "event")
	TTree *geoTree = (TTree*) inFile->Get("wcsimGeoT");
	WCSimRootGeom *geo = new WCSimRootGeom();
	TBranch *branchG = geoTree->GetBranch("wcsimrootgeom");
	branchG->SetAddress(&geo);

	if (verbosity) {std::cout << "Geotree has " << geoTree->GetEntries() << " entries." << std::endl;}

	geoTree->GetEntry(0);

	// Start with the main trigger as it always exists and contains most of the info
	WCSimRootTrigger *wcsimTriggerID;
	WCSimRootTrigger *wcsimTriggerOD;
	// Create an output file
	TFile *outFile = new TFile(outFileName.c_str(), "RECREATE");
	TTree *outBranch = new TTree("simulation", "simulation");


	// Detector geometry details
	int MAXPMT = geo->GetWCNumPMT(); // Get the maximum number of PMTs in the ID
	int MAXPMTA = geo->GetODWCNumPMT(); // Get the maximum number of PMTs in the OD

	// Variables to read in from the root file ID
	float vtxX, vtxY, vtxZ; // vertex coordinate
	float dirX, dirY, dirZ; // particle momentum direction
	float energy; // particle energy
	float rawHitsID; // number of raw pmt hits
	float digiHitsID; // number of pmt digitised hits
	int numPMTsHitID; // Number of PMTs hit
	int numPMTsDigiHitID; // Number of PMTs with digihits
	// Variables to read in from the root file OD
	float vtxXOD, vtxYOD, vtxZOD; // vertex coordinate
	float dirXOD, dirYOD, dirZOD; // particle momentum direction
	float energyOD; // particle energy
	float rawHitsOD; // number of raw pmt hits
	float digiHitsOD; // number of pmt digitised hits
	int numPMTsHitOD; // Number of PMTs hit
	int numPMTsDigiHitOD; // Number of PMTs with digihits

	// other variables
	int event, trigger;
	float phi, theta;

	// Set the branches to be saved.
	outBranch->Branch("event", &event, "event/I");
	outBranch->Branch("trigger", &trigger, "trigger/I");
	outBranch->Branch("vtxX", &vtxX, "vtxX/F");
	outBranch->Branch("vtxY", &vtxY, "vtxY/F");
	outBranch->Branch("vtxZ", &vtxZ, "vtxZ/F");
	outBranch->Branch("dirX", &dirX, "dirX/F");
	outBranch->Branch("dirY", &dirY, "dirY/F");
	outBranch->Branch("dirZ", &dirZ, "dirZ/F");
	outBranch->Branch("phi", &phi, "phi/F");
	outBranch->Branch("theta", &theta, "theta/F");
	outBranch->Branch("energy", &energy, "energy/F");
	outBranch->Branch("rawHitsID", &rawHitsID, "rawHitsID/F");
	outBranch->Branch("digiHitsID", &digiHitsID, "digiHitsID/F");
	outBranch->Branch("rawHitsOD", &rawHitsOD, "rawHitsOD/F");
	outBranch->Branch("digiHitsOD", &digiHitsOD, "digiHitsOD/F");
	outBranch->Branch("numPMTsHitID", &numPMTsHitID, "numPMTsHitID/I");
	outBranch->Branch("numPMTsDigiHitID", &numPMTsDigiHitID, "numPMTsDigiHitID/I");
	outBranch->Branch("numPMTsHitOD", &numPMTsHitOD, "numPMTsHitOD/I");
	outBranch->Branch("numPMTsDigiHitOD", &numPMTsDigiHitOD, "numPMTsDigiHitOD/I");


	// Declare histograms
	// number of hit PMTs
	TH1D *numRawHitPMTsIDHist = new TH1D("numRawHitPMTsIDHist","numRawHitPMTsIDHist",200, 0, 1000);
	TH1D *numRawHitPMTsODHist = new TH1D("numRawHitPMTsODHist","numRawHitPMTsODHist",200, 0, 1000);
	TH1D *numDigiHitPMTsIDHist = new TH1D("numDigiHitPMTsIDHist","numDigiHitPMTsIDHist",200, 0, 1000);
	TH1D *numDigiHitPMTsODHist = new TH1D("numDigiHitPMTsODHist","numDigiHitPMTsODHist",200, 0, 1000);
	// number of hits (pe)
	TH1D *numRawHitsIDHist = new TH1D("numRawHitsIDHist","numRawHitsIDHist",200, 0, 1000);
	TH1D *numRawHitsODHist = new TH1D("numRawHitsODHist","numRawHitsODHist",200, 0, 1000);
	TH1D *numDigiHitsIDHist = new TH1D("numDigiHitsIDHist","numDigiHitsIDHist",200, 0, 1000);
	TH1D *numDigiHitsODHist = new TH1D("numDigiHitsODHist","numDigiHitsODHist",200, 0, 1000);
	// Simulated particles
	TH2D *thetaVsPhi = new TH2D("thetaVsPhi", "thetaVsPhi", 60, -PI, PI, 60, -PI, PI );

	// Format histograms ( hist, title, x_title, y_title)
	format_histogram(numRawHitPMTsIDHist, "" , "Number of hit ID PMTs [raw]", "");
	format_histogram(numRawHitPMTsODHist, "" , "Number of hit OD PMTs [raw]", "");
	format_histogram(numDigiHitPMTsIDHist, "" , "Number of hit ID PMTs [digi]", "");
	format_histogram(numDigiHitPMTsODHist, "" , "Number of hit OD PMTs [digi]", "");

	format_histogram(numRawHitsIDHist, "" , "Number of ID hits [pe]", "");
	format_histogram(numRawHitsODHist, "" , "Number of OD hits [pe]", "");
	format_histogram(numDigiHitsIDHist, "" , "Number of ID hits [pe]", "");
	format_histogram(numDigiHitsODHist, "" , "Number of OD hits [pe]", "");

	format_histogram(thetaVsPhi, "" , "Particle direction #theta", "Particle direction #phi");

	// Event Analysis
	for (int ev = 0; ev < nEvent; ev++){ // Loop over events

		std::string command;
		wcsimTree->GetEntry(ev);
		wcsimTriggerID = wcsimRootID->GetTrigger(0);
		int numTriggersID = wcsimRootID->GetNumberOfEvents();
		int numSubTriggersID = wcsimRootID->GetNumberOfSubEvents();

		wcsimTriggerOD = wcsimRootOD->GetTrigger(0);
		int numTriggersOD = wcsimRootOD->GetNumberOfEvents();
		int numSubTriggersOD = wcsimRootOD->GetNumberOfSubEvents();
		
		event = ev;

		for (int nTrig = 0; nTrig < numTriggersID; nTrig++){

			trigger = nTrig;

			// ID
			wcsimTriggerID = wcsimRootID->GetTrigger(nTrig);
			int numTracksID = wcsimTriggerID->GetNtrack();
			WCSimRootTrack * trackID = (WCSimRootTrack*) wcsimTriggerID->GetTracks()->At(0);

			vtxX = wcsimTriggerID->GetVtx(0);
			vtxY = wcsimTriggerID->GetVtx(1);
			vtxZ = wcsimTriggerID->GetVtx(2);
			dirX = trackID->GetDir(0);
			dirY = trackID->GetDir(1);
			dirZ = trackID->GetDir(2);
			energy = trackID->GetE();

			theta = atan2( dirX, dirZ ); 
			phi = atan2( dirY, dirX ); 
			thetaVsPhi->Fill(theta, phi);

			rawHitsID = 0;
			digiHitsID = 0;

			numPMTsHitID = wcsimTriggerID->GetNcherenkovhits(); //Returns the number of PMTs with a true hit (photon or dark noise) (QE applied)
			numPMTsDigiHitID = wcsimTriggerID->GetNcherenkovdigihits(); //Returns the number of PMTs with a true hit (photon or dark noise) (QE applied)
			// END OF ID

			// ID
			wcsimTriggerOD = wcsimRootOD->GetTrigger(nTrig);
			int numTracksOD = wcsimTriggerOD->GetNtrack();
			WCSimRootTrack * trackOD = (WCSimRootTrack*) wcsimTriggerOD->GetTracks()->At(0);

			vtxXOD = wcsimTriggerOD->GetVtx(0);
			vtxYOD = wcsimTriggerOD->GetVtx(1);
			vtxZOD = wcsimTriggerOD->GetVtx(2);
			dirXOD = trackOD->GetDir(0);
			dirYOD = trackOD->GetDir(1);
			dirZOD = trackOD->GetDir(2);
			energyOD = trackOD->GetE();
			rawHitsOD = 0;
			digiHitsOD = 0;

			numPMTsHitOD = wcsimTriggerOD->GetNcherenkovhits(); //Returns the number of PMTs with a true hit (photon or dark noise) (QE applied)
			numPMTsDigiHitOD = wcsimTriggerOD->GetNcherenkovdigihits(); //Returns the number of PMTs with a true hit (photon or dark noise) (QE applied)
			// END OF ID


			// Work out the number of photons that hit the PMTs
			for (int i = 0; i < numPMTsHitID; i++){
				WCSimRootCherenkovHit *cherenkovHitID = (WCSimRootCherenkovHit*) wcsimTriggerID->GetCherenkovHits()->At(i);
				float tmpRawHitsID = cherenkovHitID->GetTotalPe(1);
				rawHitsID += tmpRawHitsID;
			} // End of for loop working out number of photons in PMTs

			// Work out the number of photons that hit the PMTs
			for (int i = 0; i < numPMTsHitOD; i++){
				WCSimRootCherenkovHit *cherenkovHitOD = (WCSimRootCherenkovHit*) wcsimTriggerOD->GetCherenkovHits()->At(i);
				float tmpRawHitsOD = cherenkovHitOD->GetTotalPe(1);
				rawHitsOD += tmpRawHitsOD;
			} // End of for loop working out number of photons in PMTs

			// Work out the number of digitised hits in the PMTs
			for (int i = 0; i < numPMTsDigiHitID; i++){
				WCSimRootCherenkovDigiHit *cherenkovDigiHitID = (WCSimRootCherenkovDigiHit*) wcsimTriggerID->GetCherenkovDigiHits()->At(i);
				float tmpDigiHitsID = cherenkovDigiHitID->GetQ();

				digiHitsID += tmpDigiHitsID;
				int tubeID = cherenkovDigiHitID->GetTubeId() -1;
			} // End of for loop working out number of digitized hits in PMTs

			// Work out the number of digitised hits in the PMTs
			for (int i = 0; i < numPMTsDigiHitOD; i++){
				WCSimRootCherenkovDigiHit *cherenkovDigiHitOD = (WCSimRootCherenkovDigiHit*) wcsimTriggerOD->GetCherenkovDigiHits()->At(i);
				float tmpDigiHitsOD = cherenkovDigiHitOD->GetQ();

				digiHitsOD += tmpDigiHitsOD;
				// Find the tube and work out its position
				int tubeOD = MAXPMT + cherenkovDigiHitOD->GetTubeId() -1; 
			} // End of for loop working out number of digitized hits in PMTs


			if (verbosity){
				std::cout << "====================================================" << std::endl;
				std::cout << "Event\t" << ev << std::endl;
				std::cout << "Trigger\t" << nTrig << std::endl;
				std::cout << "vtx\t" << vtxXOD << " " << vtxYOD << " " << vtxZOD << std::endl;  
				std::cout << "dir\t" << dirXOD << " " << dirYOD << " " << dirZOD << std::endl;  
				std::cout << "ene\t" << energyOD << std::endl;
				std::cout << "rawhitsOD\t" << rawHitsOD << std::endl;
				std::cout << "digihitsOD\t" << digiHitsOD << std::endl;
				std::cout << "rawhitsID\t" << rawHitsID << std::endl;
				std::cout << "digihitsID\t" << digiHitsID << std::endl;
				std::cout << "numPMTsHitOD\t" << numPMTsHitOD << std::endl;
				std::cout << "numPMTsDigiHitOD\t" << numPMTsDigiHitOD << std::endl;
				std::cout << "numPMTsHitID\t" << numPMTsHitID << std::endl;
				std::cout << "numPMTsDigiHitID\t" << numPMTsDigiHitID << std::endl;
			}

			numRawHitsIDHist->Fill(rawHitsID);
			numDigiHitsIDHist->Fill(digiHitsID);
			numRawHitsODHist->Fill(rawHitsOD);
			numDigiHitsODHist->Fill(digiHitsOD);

			numRawHitPMTsIDHist->Fill(numPMTsHitID);
			numRawHitPMTsODHist->Fill(numPMTsHitOD);
			numDigiHitPMTsIDHist->Fill(numPMTsDigiHitID);
			numDigiHitPMTsODHist->Fill(numPMTsDigiHitOD);

			outBranch->Fill(); // fill the tree with events

		} // End of loop over triggers


	} // End of loop over events



	outFile->Write(); // Write all of objects to the output file.
	outFile->Close(); // Close the output file.

}




