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
#define topPMT 19

/*
*This macro is adapted from read_OD.C to read the output from the light injector simulation. It produces a simplified tree with OD hit information useful for the LI analysis for OD calibration, including the digitised charge and time, hit PMT position and cylinder location.
*
*How to run:
*
* enter in the terminal root -l 'read_LIGen_OD_output.C("WCSim.root","outputFile.root",false)' to run
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

void read_LIGen_OD_output(std::string inFileName, std::string outFileName, bool verbosity=0){


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
        WCSimRootEvent *wcsimRootMPMT = new WCSimRootEvent();

	// Set the branch address for reading from the tree
	TBranch *branch = wcsimTree->GetBranch("wcsimrootevent");
	TBranch *branchOD = wcsimTree->GetBranch("wcsimrootevent_OD");
	TBranch *branchMPMT = wcsimTree->GetBranch("wcsimrootevent2");
	branch->SetAddress(&wcsimRootID);
	branchOD->SetAddress(&wcsimRootOD);
	branchMPMT->SetAddress(&wcsimRootMPMT);

	// Force deletion to prevent memory leak
	wcsimTree->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);
	wcsimTree->GetBranch("wcsimrootevent_OD")->SetAutoDelete(kTRUE);
	wcsimTree->GetBranch("wcsimrootevent2")->SetAutoDelete(kTRUE);

	// Load the geometry tree (only 1 "event")
        TTree *geoTree = (TTree*) inFile->Get("wcsimGeoT");
        WCSimRootGeom *geo = nullptr;//new WCSimRootGeom();
        geoTree->SetBranchAddress("wcsimrootgeom",&geo);
        if (verbosity) {std::cout << "Geotree has " << geoTree->GetEntries() << " entries." << std::endl;}
        geoTree->GetEntry(0);

	// Start with the main trigger as it always exists and contains most of the info
	WCSimRootTrigger *wcsimTriggerID;
	WCSimRootTrigger *wcsimTriggerOD;
	WCSimRootTrigger *wcsimTriggerMPMT;

	// Create an output file
	TFile *outFile = new TFile(outFileName.c_str(), "RECREATE");
	TTree *outBranch = new TTree("simulation", "simulation");

	// Detector geometry details
	int MAXPMT = geo->GetWCNumPMT(); // Get the maximum number of PMTs in the ID
	int MAXPMTA = geo->GetODWCNumPMT(); // Get the maximum number of PMTs in the OD

	// Injector variables to read in from the root file
	float injectorVtxX, injectorVtxY, injectorVtxZ; // injector location
	float injectorDirX, injectorDirY, injectorDirZ; // injector direction (axis)
	float rawHitsID; // number of raw pmt hits
	float digiHitsID; // number of pmt digitised hits
	int numPMTsHitID; // Number of PMTs hit
	int numPMTsDigiHitID; // Number of PMTs with digihits
	float rawHitsOD; // number of raw pmt hits
	float digiHitsOD; // number of pmt digitised hits
	int numPMTsHitOD; // Number of PMTs hit
	int numPMTsDigiHitOD; // Number of PMTs with digihits
	float rawHitsMPMT; // number of raw pmt hits
	float digiHitsMPMT; // number of pmt digitised hits
	int numPMTsHitMPMT; // Number of PMTs hit
	int numPMTsDigiHitMPMT; // Number of PMTs with digihits

        // Variables to read in from the root file - OD only for OD LI
	float pmtX;
        float pmtY;
        float pmtZ;
        int cylLoc;
        int tubeNumber;
        float digiTime;
        float digiCharge;

	// other variables
	int event, trigger;

	// Set the branches to be saved.
	outBranch->Branch("event", &event, "event/I");
	outBranch->Branch("trigger", &trigger, "trigger/I");
	outBranch->Branch("injectorVtxX", &injectorVtxX, "injectorVtxX/F");
	outBranch->Branch("injectorVtxY", &injectorVtxY, "injectorVtxY/F");
	outBranch->Branch("injectorVtxZ", &injectorVtxZ, "injectorVtxZ/F");
	outBranch->Branch("injectorDirX", &injectorDirX, "injectorDirX/F");
	outBranch->Branch("injectorDirY", &injectorDirY, "injectorDirY/F");
	outBranch->Branch("injectorDirZ", &injectorDirZ, "injectorDirZ/F");
	outBranch->Branch("rawHitsID", &rawHitsID, "rawHitsID/F");
	outBranch->Branch("digiHitsID", &digiHitsID, "digiHitsID/F");
	outBranch->Branch("numPMTsHitID", &numPMTsHitID, "numPMTsHitID/I");
	outBranch->Branch("numPMTsDigiHitID", &numPMTsDigiHitID, "numPMTsDigiHitID/I");
	outBranch->Branch("rawHitsMPMT", &rawHitsMPMT, "rawHitsMPMT/F");
	outBranch->Branch("digiHitsMPMT", &digiHitsMPMT, "digiHitsMPMT/F");
	outBranch->Branch("numPMTsHitMPMT", &numPMTsHitMPMT, "numPMTsHitMPMT/I");
	outBranch->Branch("numPMTsDigiHitMPMT", &numPMTsDigiHitMPMT, "numPMTsDigiHitMPMT/I");
	outBranch->Branch("rawHitsOD", &rawHitsOD, "rawHitsOD/F");
	outBranch->Branch("digiHitsOD", &digiHitsOD, "digiHitsOD/F");
	outBranch->Branch("numPMTsHitOD", &numPMTsHitOD, "numPMTsHitOD/I");
	outBranch->Branch("numPMTsDigiHitOD", &numPMTsDigiHitOD, "numPMTsDigiHitOD/I");
        outBranch->Branch("cylLoc",&cylLoc,"cylLoc/I");
        outBranch->Branch("tubeNumber",&tubeNumber,"tubeNumber/I");
        outBranch->Branch("pmtX",&pmtX,"pmtX/F");
        outBranch->Branch("pmtY",&pmtY,"pmtY/F");
        outBranch->Branch("pmtZ",&pmtZ,"pmtZ/F");
        outBranch->Branch("digiTime",&digiTime,"digiTime/F");
        outBranch->Branch("digiCharge",&digiCharge,"digiCharge/F");

	// Declare histograms
	// number of hit PMTs
	TH1D *numRawHitPMTsIDHist = new TH1D("numRawHitPMTsIDHist","numRawHitPMTsIDHist",200, 0, 1000);
	TH1D *numDigiHitPMTsIDHist = new TH1D("numDigiHitPMTsIDHist","numDigiHitPMTsIDHist",200, 0, 1000);
	TH1D *numRawHitPMTsMPMTHist = new TH1D("numRawHitPMTsMPMTHist","numRawHitPMTsMPMTHist",200, 0, 1000);
	TH1D *numDigiHitPMTsMPMTHist = new TH1D("numDigiHitPMTsMPMTHist","numDigiHitPMTsMPMTHist",200, 0, 1000);
	TH1D *numRawHitPMTsODHist = new TH1D("numRawHitPMTsODHist","numRawHitPMTsODHist",200, 0, 1000);
	TH1D *numDigiHitPMTsODHist = new TH1D("numDigiHitPMTsODHist","numDigiHitPMTsODHist",200, 0, 1000);
	// number of hits (pe)
	TH1D *numRawHitsIDHist = new TH1D("numRawHitsIDHist","numRawHitsIDHist",200, 0, 1000);
	TH1D *numDigiHitsIDHist = new TH1D("numDigiHitsIDHist","numDigiHitsIDHist",200, 0, 1000);
	TH1D *numRawHitsMPMTHist = new TH1D("numRawHitsMPMTHist","numRawHitsMPMTHist",200, 0, 1000);
	TH1D *numDigiHitsMPMTHist = new TH1D("numDigiHitsMPMTHist","numDigiHitsMPMTHist",200, 0, 1000);
	TH1D *numRawHitsODHist = new TH1D("numRawHitsODHist","numRawHitsODHist",200, 0, 1000);
	TH1D *numDigiHitsODHist = new TH1D("numDigiHitsODHist","numDigiHitsODHist",200, 0, 1000);

	// Format histograms ( hist, title, x_title, y_title)
	format_histogram(numRawHitPMTsIDHist, "" , "Number of hit ID PMTs [raw]", "");
	format_histogram(numDigiHitPMTsIDHist, "" , "Number of hit ID PMTs [digi]", "");
	format_histogram(numRawHitPMTsMPMTHist, "" , "Number of hit MPMT PMTs [raw]", "");
	format_histogram(numDigiHitPMTsMPMTHist, "" , "Number of hit MPMT PMTs [digi]", "");
	format_histogram(numRawHitPMTsODHist, "" , "Number of hit OD PMTs [raw]", "");
	format_histogram(numDigiHitPMTsODHist, "" , "Number of hit OD PMTs [digi]", "");

	format_histogram(numRawHitsIDHist, "" , "Number of ID hits [pe]", "");
	format_histogram(numDigiHitsIDHist, "" , "Number of ID hits [pe]", "");
	format_histogram(numRawHitsMPMTHist, "" , "Number of MPMT hits [pe]", "");
	format_histogram(numDigiHitsMPMTHist, "" , "Number of MPMT hits [pe]", "");
	format_histogram(numRawHitsODHist, "" , "Number of OD hits [pe]", "");
	format_histogram(numDigiHitsODHist, "" , "Number of OD hits [pe]", "");

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
		
		wcsimTriggerMPMT = wcsimRootMPMT->GetTrigger(0);
		int numTriggersMPMT = wcsimRootMPMT->GetNumberOfEvents();
		int numSubTriggersMPMT = wcsimRootMPMT->GetNumberOfSubEvents();
		event = ev;

		for (int nTrig = 0; nTrig < numTriggersID; nTrig++){

			trigger = nTrig;

			// ID
			wcsimTriggerID = wcsimRootID->GetTrigger(nTrig);
			int numTracksID = wcsimTriggerID->GetNtrack();
			WCSimRootTrack * trackID = (WCSimRootTrack*) wcsimTriggerID->GetTracks()->At(nTrig);
                        
                        // Get the injector position and injectorDirection
                        // This is the same for all ID and OD hits
			injectorVtxX = wcsimTriggerID->GetVtx(0);
			injectorVtxY = wcsimTriggerID->GetVtx(1);
			injectorVtxZ = wcsimTriggerID->GetVtx(2);
			injectorDirX = trackID->GetDir(0);
			injectorDirY = trackID->GetDir(1);
			injectorDirZ = trackID->GetDir(2);

			rawHitsID = 0;
			digiHitsID = 0;

			numPMTsHitID = wcsimTriggerID->GetNcherenkovhits(); //Returns the number of PMTs with a true hit (photon or dark noise) (QE applied)
			numPMTsDigiHitID = wcsimTriggerID->GetNcherenkovdigihits(); //Returns the number of PMTs with a true hit (photon or dark noise) (QE applied)
			
                        // Work out the number of photons that hit the ID PMTs
			for (int i = 0; i < numPMTsHitID; i++){
				WCSimRootCherenkovHit *cherenkovHitID = (WCSimRootCherenkovHit*) wcsimTriggerID->GetCherenkovHits()->At(i);
				float tmpRawHitsID = cherenkovHitID->GetTotalPe(1);
				rawHitsID += tmpRawHitsID;
			} // End of for loop working out number of photons in ID PMTs
			
                        // Work out the number of digitised hits in the ID PMTs
			for (int i = 0; i < numPMTsDigiHitID; i++){
				WCSimRootCherenkovDigiHit *cherenkovDigiHitID = (WCSimRootCherenkovDigiHit*) wcsimTriggerID->GetCherenkovDigiHits()->At(i);
				float tmpDigiHitsID = cherenkovDigiHitID->GetQ();

				digiHitsID += tmpDigiHitsID;
			} // End of for loop working out number of digitized hits in PMTs
                } // END OF loop over ID triggers

		for (int nTrig = 0; nTrig < numTriggersMPMT; nTrig++){

			trigger = nTrig;

			wcsimTriggerMPMT = wcsimRootMPMT->GetTrigger(nTrig);
			int numTracksMPMT = wcsimTriggerMPMT->GetNtrack();
			WCSimRootTrack * trackMPMT = (WCSimRootTrack*) wcsimTriggerMPMT->GetTracks()->At(nTrig);
                        
			rawHitsMPMT = 0;
			digiHitsMPMT = 0;

			numPMTsHitMPMT = wcsimTriggerMPMT->GetNcherenkovhits(); //Returns the number of PMTs with a true hit (photon or dark noise) (QE applied)
			numPMTsDigiHitMPMT = wcsimTriggerMPMT->GetNcherenkovdigihits(); //Returns the number of PMTs with a true hit (photon or dark noise) (QE applied)
			
                        // Work out the number of photons that hit the MPMT PMTs
			for (int i = 0; i < numPMTsHitMPMT; i++){
				WCSimRootCherenkovHit *cherenkovHitMPMT = (WCSimRootCherenkovHit*) wcsimTriggerMPMT->GetCherenkovHits()->At(i);
				float tmpRawHitsMPMT = cherenkovHitMPMT->GetTotalPe(1);
				rawHitsMPMT += tmpRawHitsMPMT;
			} // End of for loop working out number of photons in MPMT PMTs
			
                        // Work out the number of digitised hits in the MPMT PMTs
			for (int i = 0; i < numPMTsDigiHitMPMT; i++){
				WCSimRootCherenkovDigiHit *cherenkovDigiHitMPMT = (WCSimRootCherenkovDigiHit*) wcsimTriggerMPMT->GetCherenkovDigiHits()->At(i);
				float tmpDigiHitsMPMT = cherenkovDigiHitMPMT->GetQ();

				digiHitsMPMT += tmpDigiHitsMPMT;
			} // End of for loop working out number of digitized hits in PMTs
                } // END OF loop over MPMT triggers

		// OD 
                for (int nTrigOD = 0; nTrigOD < numTriggersOD; nTrigOD++){

			wcsimTriggerOD = wcsimRootOD->GetTrigger(nTrigOD);
			int numTracksOD = wcsimTriggerOD->GetNtrack();
			WCSimRootTrack * trackOD = (WCSimRootTrack*) wcsimTriggerOD->GetTracks()->At(nTrigOD);

			rawHitsOD = 0;
			digiHitsOD = 0;

			event = ev;

			numPMTsHitOD = wcsimTriggerOD->GetNcherenkovhits(); //number of PMTs with a true hit (photon or dark noise) (QE applied)
			numPMTsDigiHitOD = wcsimTriggerOD->GetNcherenkovdigihits(); // number of PMTs with a true hit (photon or dark noise) (QE applied)
                        // Work out the number of photons that hit the OD PMTs
			for (int i = 0; i < numPMTsHitOD; i++){
				WCSimRootCherenkovHit *cherenkovHitOD = (WCSimRootCherenkovHit*) wcsimTriggerOD->GetCherenkovHits()->At(i);
				float tmpRawHitsOD = cherenkovHitOD->GetTotalPe(1);
				rawHitsOD += tmpRawHitsOD;
			} // End of for loop working out number of photons in OD PMTs
			
                        // Work out the number of digitised hits in the OD PMTs
			for (int i = 0; i < numPMTsDigiHitOD; i++){
				WCSimRootCherenkovDigiHit *cherenkovDigiHitOD = (WCSimRootCherenkovDigiHit*) wcsimTriggerOD->GetCherenkovDigiHits()->At(i);
				float tmpDigiHitsOD = cherenkovDigiHitOD->GetQ();

				digiHitsOD += tmpDigiHitsOD;
			} // End of for loop working out number of digitized hits in PMTs

        		int ncherenkovdigihits_slots = wcsimTriggerOD->GetNcherenkovdigihits_slots();
                        
			// Loop through elements in the TClonesArray of WCSimRootCherenkovHits
			for (int i=0; i< ncherenkovdigihits_slots; i++){
				TObject *Digi = (wcsimTriggerOD->GetCherenkovDigiHits())->At(i);
				if(!Digi)
				continue;
				WCSimRootCherenkovDigiHit *wcsimrootcherenkovdigihit =
				dynamic_cast<WCSimRootCherenkovDigiHit*>(Digi);
				tubeNumber = wcsimrootcherenkovdigihit->GetTubeId();
				digiCharge = wcsimrootcherenkovdigihit->GetQ();
				digiTime = wcsimrootcherenkovdigihit->GetT();
				WCSimRootPMT pmt = geo->GetODPMT(tubeNumber-1);
				pmtX = pmt.GetPosition(0);
				pmtY = pmt.GetPosition(1);
				pmtZ = pmt.GetPosition(2);
				cylLoc = pmt.GetCylLoc();
				outBranch->Fill(); // fill the tree with events
			} // end of loop over the OD digitised hits



			if (verbosity){
				std::cout << "====================================================" << std::endl;
				std::cout << "Event\t" << ev << std::endl;
				std::cout << "Trigger\t" << nTrigOD << std::endl;
				std::cout << "injectorVtx\t" << injectorVtxX << " " << injectorVtxY << " " << injectorVtxZ << std::endl;  
				std::cout << "injectorDir\t" << injectorDirX << " " << injectorDirY << " " << injectorDirZ << std::endl;  
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
			numRawHitsMPMTHist->Fill(rawHitsMPMT);
			numDigiHitsMPMTHist->Fill(digiHitsMPMT);
			numRawHitsODHist->Fill(rawHitsOD);
			numDigiHitsODHist->Fill(digiHitsOD);

			numRawHitPMTsIDHist->Fill(numPMTsHitID);
			numDigiHitPMTsIDHist->Fill(numPMTsDigiHitID);
			numRawHitPMTsMPMTHist->Fill(numPMTsHitMPMT);
			numDigiHitPMTsMPMTHist->Fill(numPMTsDigiHitMPMT);
			numRawHitPMTsODHist->Fill(numPMTsHitOD);
			numDigiHitPMTsODHist->Fill(numPMTsDigiHitOD);


		} // End of loop over OD triggers


	} // End of loop over events

//	outBranch->Write();


	outFile->Write(); // Write all of objects to the output file.
	outFile->Close(); // Close the output file.

}




