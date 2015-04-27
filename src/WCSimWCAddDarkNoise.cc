#include "WCSimWCAddDarkNoise.hh"
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
#include <utility>
// for memset
#include <cstring>
#include <iostream>

WCSimWCAddDarkNoise::WCSimWCAddDarkNoise(G4String name,
                                   WCSimDetectorConstruction* myDetector)
  :G4VDigitizerModule(name)
{
  this->myDetector = myDetector;
  DigiHitMap.clear();
  DarkRateMessenger = new WCSimDarkRateMessenger(this);
}

WCSimWCAddDarkNoise::~WCSimWCAddDarkNoise(){
  DarkRateMessenger = 0;
}

void WCSimWCAddDarkNoise::AddDarkNoise(){
  G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();
  // Get the PMT collection ID                                                                                                                                             
  G4int WCHCID = DigiMan->GetDigiCollectionID("WCRawPMTSignalCollection");
  // Get the PMT Digits collection                                                                                                                                        
  WCSimWCDigitsCollection* WCHCPMT =
    (WCSimWCDigitsCollection*)(DigiMan->GetDigiCollection(WCHCID));
  
  if (WCHCPMT && this->PMTDarkRate>1E-307) {
    //Determine ranges for adding noise
    if(DarkMode == 1)
      FindDarkNoiseRanges(WCHCPMT,this->DarkWindow);
    else if(DarkMode == 0) {
      result.push_back(std::pair<float,float>(DarkLow,DarkHigh));
    }
    //Call routine to add dark noise here.
    //loop over pairs which represent ranges.
    //Add noise to those ranges
    std::vector<std::pair<float, float> >::iterator it2 = result.begin();
    while (it2 != result.end()){
      AddDarkNoiseBeforeDigi(WCHCPMT,it2->first,it2->second);
      it2++;
    }
  }
}

#include "G4LogicalVolumeStore.hh"

void WCSimWCAddDarkNoise::AddDarkNoiseBeforeDigi(WCSimWCDigitsCollection* WCHCPMT, float num1 ,float num2) {
    // Introduces dark noise into each PMT during an event window
    // This won't introduce noise only events, and isn't written
    // to handle different rates for each PMT (although this shouldn't
    // be too difficult to add at a later time)
    // 
    // Added by: Morgan Askins (maskins@ucdavis.edu)

    G4int number_entries = WCHCPMT->entries();
    const G4int number_pmts = myDetector->GetTotalNumPmts();
    int *PMTindex = new int [number_pmts+1];
    //   int PMTindex[number_pmts];
    //initialize PMTindex
    for (int l=0; l<number_pmts+1; l++){
      PMTindex[l] =0;
    }
    int num_hit_b4=0;
    int num_hit_after=0;
    //    std::cout<<"entries before "<<WCHCPMT->entries()<<"\n";
    //Set up proper indices for tubes which have already been hit
    for (int g=0; g<number_entries; g++){
      G4int tube = (*WCHCPMT)[g]->GetTubeID();
      //std::cout<<"totalpe "<<tube<<" "<<(*WCHCPMT)[g]->GetTotalPe()<<"\n";
      for (int gp=0; gp<(*WCHCPMT)[g]->GetTotalPe(); gp++){
	//should this be tube-1?
	PMTindex[tube]++;
	num_hit_b4++;
	//	std::cout<<"TotalPe "<<(*WCHCPMT)[g]->GetTotalPe()<<" "<<PMTindex[tube]<<"\n";
      }
      
    }

    // Get the info for pmt positions
    std::vector<WCSimPmtInfo*> *pmts = myDetector->Get_Pmts();
    // It works out that the pmts here are ordered !
    // pmts->at(i) has tubeid i+1
    
    //removed this if to allow noise only events
      
//     // Don't add noise if there are zero events!
//     if( number_entries == 0 )
//         return;

    std::vector<int> list;
    list.assign( number_pmts+1, 0 );

    for( int h = 0; h < number_entries; h++ )
      {
        list[(*WCHCPMT)[h]->GetTubeID()] = h+1;
       
      }


   
    // Add noise to PMT's here, do so for time < LongTime
    double current_time = 0;
    double pe = 0.0;
    //Calculate the time window size
    double windowsize = num2 - num1;
    //    double poisson_mean = 1 / (this->PMTDarkRate * calibdarknoise * 1E-6 * number_pmts);
    double poisson_mean = 1 / (this->PMTDarkRate * this->ConvRate * 1E-6 * number_pmts);
    G4DigiManager* DMman = G4DigiManager::GetDMpointer();
    WCSimWCPMT* WCPMT = (WCSimWCPMT*)DMman->FindDigitizerModule("WCReadoutPMT");
    // Only add noise to triggered time windows!
   
    //average number of PMTs with noise
    //for now only add noise to first 1000ns
    
        double ave=number_pmts * this->PMTDarkRate * this->ConvRate * windowsize * 1E-6; 
    //double ave=number_pmts * this->PMTDarkRate * this->ConvRate * 4000. * 1E-6;

    //poisson distributed noise, number of noise hits to add
    int nnoispmt = CLHEP::RandPoisson::shoot(ave);
    for( int i = 0; i < nnoispmt; i++ )
      {
	//time of noise hit to be generated
	//A time from t=num1 to num2
	current_time = num1 + G4UniformRand()*windowsize;
	//current_time = 0. + G4UniformRand()*4000;
	//now a random PMT.  Assuming noise levels are the same for
	//each PMT.
	int noise_pmt = static_cast<int>( G4UniformRand() * number_pmts ) + 1; //so that pmt numbers runs from 1 to Npmt
      
	if( list[ noise_pmt ] == 0 )
	{
	    //	      WCSimWCHit* ahit = new WCSimWCHit();
	    WCSimWCDigi* ahit = new WCSimWCDigi();
	    ahit->SetTubeID( noise_pmt);
	    //std::cout<<"setting new noise pmt "<<noise_pmt<<" "<<ahit->GetTubeID()<<"\n";
	    // This Logical volume is GlassFaceWCPMT
	    //ahit->SetLogicalVolume((*WCHCPMT)[0]->GetLogicalVolume());
	    ahit->SetLogicalVolume(G4LogicalVolumeStore::GetInstance()->GetVolume("glassFaceWCPMT"));
	    //std::cout<<"1 "<<(G4LogicalVolumeStore::GetInstance()->GetVolume("glassFaceWCPMT"))->GetName()<<"\n";
	    //std::cout<<"2 "<<(*WCHCPMT)[0]->GetLogicalVolume()->GetName()<<"\n";
	    ahit->SetTrackID(-1);
	    ahit->AddParentID(-1);
	    // Set the position and rotation of the pmt
	    Float_t hit_pos[3];
	    Float_t hit_rot[3];
	    //Int_t hit_cylLoc;
	    // TODO: need to change the format of hit_pos to G4ThreeVector
	    // and change hit_rot to G4RotationMatrix
	    
	    WCSimPmtInfo* pmtinfo = (WCSimPmtInfo*)pmts->at( noise_pmt -1 );
	    hit_pos[0] = 10*pmtinfo->Get_transx();
	    hit_pos[1] = 10*pmtinfo->Get_transy();
	    hit_pos[2] = 10*pmtinfo->Get_transz();
	    hit_rot[0] = pmtinfo->Get_orienx();
	    hit_rot[1] = pmtinfo->Get_orieny();
	    hit_rot[2] = pmtinfo->Get_orienz();
	    //hit_cylLoc = pmtinfo->Get_cylocation();
	    G4RotationMatrix pmt_rotation(hit_rot[0], hit_rot[1], hit_rot[2]);
	    G4ThreeVector pmt_position(hit_pos[0], hit_pos[1], hit_pos[2]);
	    ahit->SetRot(pmt_rotation);
	    ahit->SetPos(pmt_position);
	    ahit->SetTime(PMTindex[noise_pmt],current_time);
	    pe = WCPMT->rn1pe();
	    ahit->SetPe(PMTindex[noise_pmt],pe);
	    //Added this line to increase the totalPe by 1
	    ahit->AddPe(current_time);
	    WCHCPMT->insert(ahit);
	    PMTindex[noise_pmt]++;
	    //	    list[ noise_pmt ] = WCHCPMT->entries();
	    number_entries ++;
	    list[ noise_pmt ] = number_entries; // Add this PMT to the end of the list                                                                                         
	  }
	else{
	  (*WCHCPMT)[ list[noise_pmt]-1 ]->AddPe(current_time);
	  pe = WCPMT->rn1pe();
	  (*WCHCPMT)[ list[noise_pmt]-1 ]->SetPe(PMTindex[noise_pmt],pe);
	  (*WCHCPMT)[ list[noise_pmt]-1 ]->SetTime(PMTindex[noise_pmt],current_time);
	  PMTindex[noise_pmt]++;
	  
	}
		
      }
    
    delete [] PMTindex;
    return;
 }



void WCSimWCAddDarkNoise::FindDarkNoiseRanges(WCSimWCDigitsCollection* WCHCPMT, float width) {
  //Loop over all Hits and assign a time window around each hit
  //store these in the ranges vector as pairs
  for (int g=0; g<WCHCPMT->entries(); g++){
    for (int gp=0; gp<(*WCHCPMT)[g]->GetTotalPe(); gp++){
      float time = (*WCHCPMT)[g]->GetTime(gp);
      //lets assume a 5us window.  So we centre this on the hit time.
      //t1 is the lower limit of the window.
      float t1=time - width/2.;
      float t2=time + width/2.;
      //if t1 is negative set to 0
      if(t1 < 0.)
	t1=0.;
      ranges.push_back(std::pair<float, float>(t1, t2));
    }
  }
  
  //we need to ensure that the ranges found above are sorted first
  //for the algorithm below to work
  //output are pairs stored in the result vector
  sort(ranges.begin(),ranges.end());
  std::vector<std::pair<float, float> >::iterator it = ranges.begin();
  std::pair<float, float> current = *(it)++;
  while (it != ranges.end()){
    if (current.second >= it->first){
      current.second = std::max(current.second, it->second); 
    } else {
      result.push_back(current);
      current = *(it);
    }
    it++;
  }
  result.push_back(current);
  //now we should have a vector of range pairs to pass to the
  //dark noise routine
}
