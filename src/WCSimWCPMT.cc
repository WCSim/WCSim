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
#include "WCSimPMTObject.hh"


#include <vector>
// for memset
#include <cstring>
//#define HYPER_VERBOSITY
//#define DEBUG

extern "C" void skrn1pe_(double* );
//extern "C" void rn1pe_(double* ); // 1Kton

G4double WCSimWCPMT::fFirst_Time = 0 ;
G4bool WCSimWCPMT::fFirst_Time_Flag = false;

WCSimWCPMT::WCSimWCPMT(G4String name,
                       WCSimDetectorConstruction* myDetector,
                       G4String detectorElement)
  :G4VDigitizerModule(name), detectorElement(detectorElement)
{
  // G4String colName = "WCRawPMTSignalCollection";
  // collectionName.push_back(colName);

  if(detectorElement=="tank") collectionName.push_back("WCRawPMTSignalCollection");
  else if(detectorElement=="tankPMT2") collectionName.push_back("WCRawPMTSignalCollection2");
  else G4cout << "detectorElement undefined..." << G4endl;
  this->myDetector = myDetector;
  DigiHitMapPMT.clear();

  //#ifdef HYPER_VERBOSITY
  if(detectorElement=="tank") G4cout<<"WCSimWCPMT::WCSimWCPMT recording collection name "<<collectionName[0]<<G4endl;
  if(detectorElement=="tankPMT2") G4cout<<"WCSimWCPMT::WCSimWCPMT recording collection name "<<collectionName[0]<<G4endl;
  //#endif

}

WCSimWCPMT::~WCSimWCPMT(){
 
}

G4double WCSimWCPMT::rn1pe(){

  G4String WCCollectionName;
  if(detectorElement=="tank") WCCollectionName = myDetector->GetIDCollectionName();
  else if(detectorElement=="tankPMT2") WCCollectionName = myDetector->GetIDCollectionName2();

  WCSimPMTObject * PMT = myDetector->GetPMTPointer(WCCollectionName);
  G4int i;
  G4double random = G4UniformRand();
  G4double random2 = G4UniformRand(); 
  G4double *qpe0;
  qpe0 = PMT->Getqpe();
  for(i = 0; i < 501; i++){
    
    if (random <= *(qpe0+i)) break;
  }
  if(i==500)
    random = G4UniformRand();
  
  return (G4double(i-50) + random2)/22.83;
  
}


void WCSimWCPMT::Digitize()
{
  G4String WCCollectionName;
  G4String DigitsCollectionName;
  if(detectorElement=="tank"){
    DigitsCollectionName="WCDigitizedCollection";
    WCCollectionName = myDetector->GetIDCollectionName();
  }else if(detectorElement=="tankPMT2"){
    DigitsCollectionName="WCDigitizedCollection2";
    WCCollectionName = myDetector->GetIDCollectionName2();
  }
  DigitsCollection = new WCSimWCDigitsCollection (DigitsCollectionName,collectionName[0]);
  G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();
 
  // Get the Associated Hit collection IDs
  G4int WCHCID = DigiMan->GetHitsCollectionID(WCCollectionName);

  // The Hits collection
  WCSimWCHitsCollection* WCHC =
    (WCSimWCHitsCollection*)(DigiMan->GetHitsCollection(WCHCID));
#ifdef HYPER_VERBOSITY
  //if(detectorElement=="tankPMT2"){
  G4cout<<"WCSimWCPMT::Digitize Making digits collection (WCSimWCDigitsCollection*)"<<DigitsCollectionName<<" for "<<detectorElement
  <<" and calling MakePeCorrection on "<<WCCollectionName<<" to fill it."<<G4endl;
    //}
#endif

  if (WCHC) {
    
    MakePeCorrection(WCHC);

  }
  
#ifdef HYPER_VERBOSITY
  //if(detectorElement=="tankPMT2"){
  G4cout<<"WCSimWCPMT::Digitize Storing "<<DigitsCollectionName<<" for "<<detectorElement
  <<", which has "<<DigitsCollection->entries()<<" entries"<<G4endl;
    //}
#endif

  StoreDigiCollection(DigitsCollection);

}


void WCSimWCPMT::MakePeCorrection(WCSimWCHitsCollection* WCHC)
{ 

  // Sort Hit times
  std::sort(WCHC->GetVector()->begin(), WCHC->GetVector()->end(), WCSimWCHit::SortFunctor_Hit());
  
  // Get the info for pmt positions
  std::vector<WCSimPmtInfo*> *pmts;

  //Get the PMT info for hit time smearing
  G4String WCCollectionName;
  if(detectorElement=="tank"){
    WCCollectionName = myDetector->GetIDCollectionName();
    pmts  = myDetector->Get_Pmts();
    // It works out that the pmts here are ordered !
    // pmts->at(i) has tubeid i+1
  }
  else if(detectorElement=="tankPMT2"){
    WCCollectionName = myDetector->GetIDCollectionName2();
    pmts  = myDetector->Get_Pmts2();
  // It works out that the pmts here are ordered !
  // pmts->at(i) has tubeid i+1
  }
  WCSimPMTObject * PMT = myDetector->GetPMTPointer(WCCollectionName);

 #ifdef HYPER_VERBOSITY
  if(detectorElement=="tankPMT2"){
    G4cout<<"WCSimWCPMT::MakePeCorrection making PE correction for ";
  }
  if(WCHC){
    G4cout<<WCHC->entries()<<" hits"<<G4endl;
  } else {
    G4cout<<"0 hits"<<G4endl;
  }
  G4cout << "Type of PMT used for pe correction = " << PMT->GetPMTName() <<G4endl;
#endif

  double maxTotalPe = 1;
  G4int bqDigiHitCounter = 0;
  
  for (G4int i=0; i < WCHC->entries(); i++)
    {

      //G4double peCutOff = .3;
      // MF, based on S.Mine's suggestion : global scaling factor applied to
      // all the smeared charges.
      // means that we need to increase the collected light by
      // (efficiency-1)*100% to
      // match K2K 1KT data  : maybe due to PMT curvature ?

      //G4double efficiency = 0.985; // with skrn1pe (AP tuning) & 30% QE increase in stacking action

      // Get the information from the hit
      G4int   tube         = (*WCHC)[i]->GetTubeID();
      G4double peSmeared = 0.0;
      G4double time_PMT, time_true;
      G4int  track_id      = (*WCHC)[i]->GetTrackID();
      
      
      // Set the position and rotation of the pmt (from WCSimWCAddDarkNoise.cc)
      Double_t hit_pos[3];
      Double_t hit_rot[3];
#ifdef DEBUG
	  std::cout << "tube : " << i << " (ID=" << tube << ")" << std::endl; //TD debug
#endif
      
      WCSimPmtInfo* pmtinfo = (WCSimPmtInfo*)pmts->at( tube -1 );
      hit_pos[0] = 10*pmtinfo->Get_transx()/CLHEP::cm;
      hit_pos[1] = 10*pmtinfo->Get_transy()/CLHEP::cm;
      hit_pos[2] = 10*pmtinfo->Get_transz()/CLHEP::cm;
      hit_rot[0] = pmtinfo->Get_orienx();
      hit_rot[1] = pmtinfo->Get_orieny();
      hit_rot[2] = pmtinfo->Get_orienz();
      G4ThreeVector pmt_orientation(hit_rot[0], hit_rot[1], hit_rot[2]);
      G4ThreeVector pmt_position(hit_pos[0], hit_pos[1], hit_pos[2]);

      //
      //double Qout;
      double ttsfactor = myDetector->GetParameters()->GetTtsff(); //TD 2019.07.02
      //double linearity = myDetector->GetParameters()->GetNLTinfo();
      double QinTOT = 0;

	  for (G4int ip =0; ip < (*WCHC)[i]->GetTotalPe(); ip++){
	  
	    time_true = (*WCHC)[i]->GetTime(ip);
	    
	    // Reset the time to have "reasonnable" timing
	    // This modification is important in case of very late hit physics (such as in radioactive decays)     
	    // for which time easy goes > 1e9 ns and cause bug in digitizer
	    // should not use /grdm/decayBiasProfile biasprofile.dat as it messes up all the timing of the decays, and force to use only one nucleus
	    if ( i == 0 && ip == 0 && RelativeHitTime && !fFirst_Time_Flag /*&& (*WCHC)[i]->GetTime(ip) > 1e5*/ ) { // Set Max at 10 musec
	      //G4cout << " Apply time correction to event hits of " << (*WCHC)[i]->GetTime(ip) << " ns" << G4endl;
	      fFirst_Time_Flag = true;
	      fFirst_Time = time_true;
	    } 

	    time_PMT  = time_true - fFirst_Time; //currently no PMT time smearing applied
	    //std::cout<<"First time = "<<fFirst_Time<<", time true = "<<time_true<<", PMT time = "<<time_PMT<<std::endl;	    
	    peSmeared = rn1pe();
#ifdef DEBUG
	    std::cout << "tube : " << i << " (ID=" << tube << ")" << " hit in tube : "<< ip << " (time=" << time_true << "ns)"  << " pe value : " << peSmeared << std::endl; //TD debug
#endif
	    int parent_id = (*WCHC)[i]->GetParentID(ip);

	    float photon_starttime = (*WCHC)[i]->GetPhotonStartTime(ip);
	    G4ThreeVector photon_startpos = (*WCHC)[i]->GetPhotonStartPos(ip);
	    G4ThreeVector photon_endpos = (*WCHC)[i]->GetPhotonEndPos(ip);
	    G4ThreeVector photon_startdir = (*WCHC)[i]->GetPhotonStartDir(ip);
	    G4ThreeVector photon_enddir = (*WCHC)[i]->GetPhotonEndDir(ip);
	    
	    if ( DigiHitMapPMT[tube] == 0) {
	      WCSimWCDigi* Digi = new WCSimWCDigi();
	      Digi->SetLogicalVolume((*WCHC)[0]->GetLogicalVolume());
	      Digi->AddPe(time_PMT);
	      Digi->SetTubeID(tube);
	      //Digi->SetTubeType((*WCHC)[0]->GetTubeType());
	      Digi->SetPos(pmt_position);
	      Digi->SetOrientation(pmt_orientation);
	      Digi->SetPe(ip,peSmeared);
	      Digi->SetTime(ip,time_PMT);
	      Digi->SetTrackID(track_id);
	      Digi->SetPreSmearTime(ip,time_true);
	      Digi->SetParentID(ip,parent_id);
	      Digi->SetPhotonStartTime(ip,photon_starttime);
	      Digi->SetPhotonStartPos(ip,photon_startpos);
	      Digi->SetPhotonEndPos(ip,photon_endpos);
	      Digi->SetPhotonStartDir(ip,photon_startdir);
	      Digi->SetPhotonEndDir(ip,photon_enddir);
	      DigiHitMapPMT[tube] = DigitsCollection->insert(Digi);
	      bqDigiHitCounter++;
	    }	
	    else {
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->AddPe(time_PMT);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetLogicalVolume((*WCHC)[0]->GetLogicalVolume());
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPe(ip,peSmeared);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetTime(ip,time_PMT);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetTubeID(tube); 
	      //(*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetTubeType((*WCHC)[0]->GetTubeType()); 
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPos(pmt_position);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetOrientation(pmt_orientation);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetTrackID(track_id);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPreSmearTime(ip,time_true);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetParentID(ip,parent_id);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPhotonStartTime(ip,photon_starttime);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPhotonStartPos(ip,photon_startpos);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPhotonEndPos(ip,photon_endpos);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPhotonStartDir(ip,photon_startdir);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPhotonEndDir(ip,photon_enddir);
	    }
	    
	    maxTotalPe = (maxTotalPe < ip) ? ip : maxTotalPe;

	  } // Loop over hits in each PMT	
#ifdef DEBUG
	  std::cout << "tube : " << i << " (ID=" << tube << ")" << " total digitized pe in : " << QinTOT << std::endl; //TD debug
#endif
    }// Loop over PMTs
#ifdef DEBUG
  std::cout << std::endl << std::endl << "The maximum amount of pe stored by a PMT is : " << maxTotalPe << std::endl << std::endl;
#endif
}


