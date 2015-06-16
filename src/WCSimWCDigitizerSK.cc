#include "WCSimWCDigitizerSK.hh"
#include "WCSimWCPMT.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4ios.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "WCSimDetectorConstruction.hh"
#include "WCSimPmtInfo.hh"
#include "WCSimDarkRateMessenger.hh"

#include <vector>
// for memset
#include <cstring>
#include <iostream>

// changed from 940 (april 2005) by MF
// 960 is unsuitable

//RawSignalHitCollection *collection = new RawSignalHitCollection;

#ifndef WCSIMWCDIGITIZERSK_VERBOSE
//#define WCSIMWCDIGITIZERSK_VERBOSE
#endif

const double WCSimWCDigitizerSK::pmtgate = 200.0 ; // ns

WCSimWCDigitizerSK::WCSimWCDigitizerSK(G4String name,
				       WCSimDetectorConstruction* myDetector,
				       WCSimWCDAQMessenger* myMessenger)
  :WCSimWCDigitizerBase(name, myDetector, myMessenger)
{
}

WCSimWCDigitizerSK::~WCSimWCDigitizerSK(){
}

void WCSimWCDigitizerSK::DigitizeHits(WCSimWCDigitsCollection* WCHCPMT) {
  G4cout << "WCSimWCDigitizerSK::DigitizeHits START WCHCPMT->entries() = " << WCHCPMT->entries() << G4endl;
  //We must first sort hits by PMT in time.  This is very important as the code
  //assumes that each hit is in time order from lowest to highest.
  
  //Sorting done.  Now we integrate the charge on each PMT.
  //SK IV has a 400ns 
  //integration gate, discharges for 350ns to calculate the charge and
  //this is followed by a 150ns veto (500ns total deadtime)
  //Total processing time for a hit is 900ns
  //SK I has 2 capacitors, allowing zero deadtime
  int deadtime = 0;
  if(SKDeadTime)
    deadtime = 350 + 150;

  for (G4int i = 0 ; i < WCHCPMT->entries() ; i++)
    {
      //loop over entires in WCHCPMT, each entry corresponds to
      //the photons on one PMT

      //Sort photons on this pmt
      (*WCHCPMT)[i]->SortArrayByHitTime();
      int tube = (*WCHCPMT)[i]->GetTubeID();
#ifdef WCSIMWCDIGITIZERSK_VERBOSE
      G4cout<<"tube "<<tube<<" totalpe = "<<(*WCHCPMT)[i]->GetTotalPe();
#endif
      /*
	G4cout<<" parents =\t";
      for( G4int ip = 0 ; ip < (*WCHCPMT)[i]->GetTotalPe() ; ip++)
	G4cout << " " << (*WCHCPMT)[i]->GetParentID(ip);
      */
#ifdef WCSIMWCDIGITIZERSK_VERBOSE
      G4cout <<G4endl;
#endif
      //look over all hits on the PMT
      //integrate charge and start digitizing
      float intgr_start=0;
      float upperlimit=0;
      G4double efficiency = 0.985; // with skrn1pe (AP tuning) & 30% QE increase in stacking action
      int ngate=0;

      // Variables to store photon uniqueid that make up a digit
      int digi_unique_id   = 0;
      int photon_unique_id = 0;
      std::vector< std::pair<int,int> > digi_comp; 

      /*
      G4cout << "DEBUGGING TUBE " << tube
	     << " total pe " << (*WCHCPMT)[i]->GetTotalPe();
      for( G4int ip = 0 ; ip < (*WCHCPMT)[i]->GetTotalPe() ; ip++)
	G4cout << " time " << (*WCHCPMT)[i]->GetTime(ip)
	       << " pe " << (*WCHCPMT)[i]->GetPe(ip);
      G4cout << G4endl;
      */

      for( G4int ip = 0 ; ip < (*WCHCPMT)[i]->GetTotalPe() ; ip++)
	{
	  float time = (*WCHCPMT)[i]->GetTime(ip);
          float pe = (*WCHCPMT)[i]->GetPe(ip);

	  //start the integration time as the time of the first hit
	  //Hits must be sorted in time
	  if(ip==0) {
	    intgr_start=time;
	    peSmeared = 0;
	    //Set the limits of the integration window [intgr_start,upperlimit]                                                                                            
	    upperlimit = intgr_start + pmtgate;
	  }
	  
	  bool MakeDigit = false;
	  if(time >= intgr_start && time <= upperlimit) {
	    peSmeared += pe;
	    photon_unique_id = ip;
	    digi_comp.push_back( std::make_pair(digi_unique_id, photon_unique_id) );
      
#ifdef WCSIMWCDIGITIZERSK_VERBOSE
	    std::cout<<"INFO: time "<<time<<" digi_id "<<digi_unique_id<<" p_id "<<photon_unique_id<<std::endl;
#endif
	    //if this is the last digit, make sure to make the digit
	    if(ip + 1 == (*WCHCPMT)[i]->GetTotalPe()){
	      MakeDigit = true;
	    }
	    
	  }
	  else {
	    //this hit is outside the integration time window.
	    //Charge integration is over.  For SKIV, there is now a 350ns + 150ns
	    //time period where no hits can be recorded
	    //Check if previous hit passed the threshold.  If so we will digitize the hit
	    MakeDigit = true;
	    // 	    int iflag;
	    // 	    WCSimWCDigitizerSK::Threshold(peSmeared,iflag);
	    // 	    if(iflag == 0) {
	    // 	      //digitize hit
	    // 	      peSmeared *= efficiency;
	    // }
	    // 	    else {
	    // 	      //reject hit
	    // 	    }
	    //Now we need to reject hits that are after the integration
	    //period to the end of the veto signal
	  }
	  
	  //Make digit here
	  if(MakeDigit) {
	    int iflag;                                                                                                                                                        
	    WCSimWCDigitizerSK::Threshold(peSmeared,iflag);                                                                                                                      
	    if(iflag == 0) {                                                                                                                                             
	      //digitize hit                                                                                                                                  
	      peSmeared *= efficiency;
	      WCSimWCDigitizerBase::AddNewDigit(tube, ngate, intgr_start, peSmeared, digi_comp);
	      ngate++;
	      
	      digi_unique_id++;
	      digi_comp.clear();
	    }
	    else {
	      //reject hit
	      //G4cout << "DIGIT REJECTED" << G4endl;
	    }
	  }
	  
	  //Now try and deal with the next hit
	  if(time > upperlimit && time <= upperlimit + deadtime) {
	    continue;
	  }
	  else if(time > upperlimit + deadtime){
#ifdef WCSIMWCDIGITIZERSK_VERBOSE
	    G4cout<<"*** PREPARING FOR >1 DIGI ***"<<G4endl;
#endif
	    //we now need to start integrating from the hit
	    intgr_start=time;
	    peSmeared = pe;
	    //Set the limits of the integration window [intgr_start,upperlimit]                                                                                 
	    upperlimit = intgr_start + pmtgate;

	    //store the digi composition information
	    photon_unique_id = ip;
            digi_comp.push_back( std::make_pair(digi_unique_id, photon_unique_id) );

	    //if this is the last hit we must handle the creation of the digit 
	    //as the loop will not evaluate again
	    if(ip+1 == (*WCHCPMT)[i]->GetTotalPe()) {
	      int iflag;
	      WCSimWCDigitizerSK::Threshold(peSmeared,iflag);
	      if(iflag == 0) {
		//digitize hit                                                                                                                                                  
		peSmeared *= efficiency;
		WCSimWCDigitizerBase::AddNewDigit(tube, ngate, intgr_start, peSmeared, digi_comp);
		ngate++;

		digi_unique_id++;
		digi_comp.clear();
	      }
	      else {
		//reject hit                                                                                                                           
		//G4cout << "DIGIT REJECTED" << G4endl;
	      }
	    }
	  }
	}//ip (totalpe)
    }//i (WCHCPMT->entries())
  G4cout<<"WCSimWCDigitizerSK::DigitizeHits END DigiStore->entries() " << DigiStore->entries() << "\n";
  
#ifdef WCSIMWCDIGITIZERSK_VERBOSE
  G4cout<<"\n\n\nCHECK DIGI COMP:"<<G4endl;
  for (G4int i = 0 ; i < DigiStore->entries() ; i++){
    std::vector< std::pair<int,int> > comp = (*DigiStore)[i]->GetDigiCompositionInfo();
    int tubeid = (*DigiStore)[i]->GetTubeID();
    for(int i = 0; i < (int) comp.size(); i++){
      G4cout<<"tube " <<tubeid<<" entry "<<i<<" digi "<<comp[i].first<< " p_id "<<comp[i].second<<G4endl;
    }
  }
#endif
}
