
#include "WCSimWCTrigger.hh"
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



// *******************************************
// BASE CLASS
// *******************************************

#ifndef WCSIMWCTRIGGER_VERBOSE
//#define WCSIMWCTRIGGER_VERBOSE
#endif

const double WCSimWCTriggerBase::offset = 950.0 ; // ns. apply offset to the digit time
const double WCSimWCTriggerBase::eventgateup = 950.0 ; // ns. save eventgateup ns after the trigger time
const double WCSimWCTriggerBase::eventgatedown = -400.0 ; // ns. save eventgateup ns before the trigger time
const double WCSimWCTriggerBase::LongTime = 100000.0 ; // ns = 0.1ms. event time


WCSimWCTriggerBase::WCSimWCTriggerBase(G4String name,
				       WCSimDetectorConstruction* inDetector,
				       WCSimWCDAQMessenger* myMessenger)
  :G4VDigitizerModule(name), myDetector(inDetector), triggerClassName("")
{
  G4String colName = "WCDigitizedCollection";
  collectionName.push_back(colName);

  ReInitialize();

  if(myMessenger != NULL) {
    DAQMessenger = myMessenger;
    DAQMessenger->TellMeAboutTheTrigger(this);
    DAQMessenger->SetTriggerOptions();
  }
  else {
    G4cerr << "WCSimWCDAQMessenger pointer is NULL when passed to WCSimWCTriggerBase constructor. Exiting..."
           << G4endl;
    exit(-1);
  }
  digitizeCalled = false;

  event_number = 1;
  output_txt = false;
}

WCSimWCTriggerBase::~WCSimWCTriggerBase(){
}

void WCSimWCTriggerBase::AdjustNDigitsThresholdForNoise()
{
  int npmts = this->myDetector->GetTotalNumPmts();
  double trigger_window_seconds = ndigitsWindow * 1E-9;
  double dark_rate_Hz = PMTDarkRate * 1000;
  double average_occupancy = dark_rate_Hz * trigger_window_seconds * npmts;
  
  G4cout << "Average number of PMTs in detector active in a " << ndigitsWindow
	 << "ns window with a dark noise rate of " << PMTDarkRate
	 << "kHz is " << average_occupancy
	 << " (" << npmts << " total PMTs)"
	 << G4endl
	 << "Updating the NDigits threshold, from " << ndigitsThreshold
	 << " to " << ndigitsThreshold + round(average_occupancy) << G4endl;
  ndigitsThreshold += round(average_occupancy);
}

void WCSimWCTriggerBase::Digitize()
{
  if(ndigitsAdjustForNoise && !digitizeCalled) {
    AdjustNDigitsThresholdForNoise();
    digitizeCalled = true;
  }

  //Input is collection of all digitized hits that passed the threshold
  //Output is all digitized hits which pass the trigger
  
  ReInitialize();

  //This is the output digit collection
  DigitsCollection = new WCSimWCTriggeredDigitsCollection ("/WCSim/glassFaceWCPMT",collectionName[0]);

  G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();

  // Get the Digitized hits collection ID
  G4int WCDCID = DigiMan->GetDigiCollectionID("WCDigitizedStoreCollection");
  // Get the PMT Digits Collection
  WCSimWCDigitsCollection* WCDCPMT = 
    (WCSimWCDigitsCollection*)(DigiMan->GetDigiCollection(WCDCID));

  // Do the work  
  if (WCDCPMT) {
    DoTheWork(WCDCPMT);
  }
  
  StoreDigiCollection(DigitsCollection);
}

void WCSimWCTriggerBase::AlgNDigits(WCSimWCDigitsCollection* WCDCPMT, bool remove_hits, bool test)
{

  //if test is true, we run the algorithm with 1/2 the threshold, and kTriggerNDigitsTest
  //for testing multiple trigger algorithms at once
  int this_ndigitsThreshold = ndigitsThreshold;
  TriggerType_t this_triggerType = kTriggerNDigits;
  if(test) {
    this_ndigitsThreshold /= 2;
    this_triggerType = kTriggerNDigitsTest;
  }

  //Now we will try to find triggers
  //loop over PMTs, and Digits in each PMT.  If ndigits > Threshhold in a time window, then we have a trigger

  int ntrig = 0;
  int window_start_time = 0;
  int window_end_time   = WCSimWCTriggerBase::LongTime - ndigitsWindow;
  int window_step_size  = 5; //step the search window along this amount if no trigger is found
  float lasthit;
  std::vector<int> digit_times;
  bool first_loop = true;

  G4cout << "WCSimWCTriggerBase::AlgNDigits. Number of entries in input digit collection: " << WCDCPMT->entries() << G4endl;
#ifdef WCSIMWCTRIGGER_VERBOSE
  int temp_total_pe = 0;
  for (G4int i = 0 ; i < WCDCPMT->entries() ; i++) {
    temp_total_pe += (*WCDCPMT)[i]->GetTotalPe();
  }
  G4cout << "WCSimWCTriggerBase::AlgNDigits. " << temp_total_pe << " total p.e. input" << G4endl;
#endif

  // the upper time limit is set to the final possible full trigger window
  while(window_start_time <= window_end_time) {
    int n_digits = 0;
    float triggertime; //save each digit time, because the trigger time is the time of the first hit above threshold
    bool triggerfound = false;
    digit_times.clear();
    
    //Loop over each PMT
    for (G4int i = 0 ; i < WCDCPMT->entries() ; i++) {
      //int tube=(*WCDCPMT)[i]->GetTubeID();
      //Loop over each Digit in this PMT
      for ( G4int ip = 0 ; ip < (*WCDCPMT)[i]->GetTotalPe() ; ip++) {
	int digit_time = (*WCDCPMT)[i]->GetTime(ip);
	//hit in trigger window?
	if(digit_time >= window_start_time && digit_time <= (window_start_time + ndigitsWindow)) {
	  n_digits++;
	  digit_times.push_back(digit_time);
	}
	//G4cout << digit_time << G4endl;
	//get the time of the last hit (to make the loop shorter)
	if(first_loop && (digit_time > lasthit))
	  lasthit = digit_time;
      }//loop over Digits
    }//loop over PMTs

    //if over threshold, issue trigger
    if(n_digits > this_ndigitsThreshold) {
      ntrig++;
      //The trigger time is the time of the first hit above threshold
      std::sort(digit_times.begin(), digit_times.end());
      triggertime = digit_times[this_ndigitsThreshold];
      triggertime -= (int)triggertime % 5;
      TriggerTimes.push_back(triggertime);
      TriggerTypes.push_back(this_triggerType);
      TriggerInfos.push_back(std::vector<Float_t>(1, n_digits));
      TriggerInfos.back().push_back(triggertime);
      triggerfound = true;
    }

#ifdef WCSIMWCTRIGGER_VERBOSE
    if(n_digits)
      G4cout << n_digits << " digits found in 200nsec trigger window ["
	     << window_start_time << ", " << window_start_time + ndigitsWindow
	     << "]. Threshold is: " << this_ndigitsThreshold << G4endl;
#endif

    //move onto the next go through the timing loop
    if(triggerfound) {
      window_start_time = triggertime + WCSimWCTriggerBase::eventgateup;
    }//triggerfound
    else {
      window_start_time += window_step_size;
    }

    //shorten the loop using the time of the last hit
    if(first_loop) {
#ifdef WCSIMWCTRIGGER_VERBOSE
      G4cout << "Last hit found to be at " << lasthit
	     << ". Changing window_end_time from " << window_end_time
	     << " to " << lasthit - (ndigitsWindow - 10)
	     << G4endl;
#endif
      window_end_time = lasthit - (ndigitsWindow - 10);
      first_loop = false;
    }
  }
  
  G4cout << "Found " << ntrig << " NHit triggers" << G4endl;
  //call FillDigitsCollection() whether any triggers are found or not
  // (what's saved depends on saveFailuresMode)
  FillDigitsCollection(WCDCPMT, remove_hits, this_triggerType);
}

void WCSimWCTriggerBase::FillDigitsCollectionNoTrigger(WCSimWCDigitsCollection* WCDCPMT, bool remove_hits, TriggerType_t save_triggerType)
{
  //Adds the digits within the trigger window to the output WCSimWCDigitsCollection
  // optionally removes digits from the input digits collection (when running different Alg* methods concurently) 
  // so they are not used in subsequent trigger decisions or saved twice
  //Also, only save digits of a specific type (again for when running different Alg* methods concurently)

  // Add dummy triggers / exit without saving triggers as required
  //
  //saveFailuresMode = 0 - save only triggered events
  //saveFailuresMode = 1 - save both triggered & not triggered events
  //saveFailuresMode = 2 - save only not triggered events
  if(TriggerTimes.size()) {
    if(saveFailuresMode == 2)
      return;
  }
  else {
    if(saveFailuresMode == 0)
      return;
    TriggerTypes.push_back(kTriggerFailure);
    TriggerTimes.push_back(saveFailuresTime);
    TriggerInfos.push_back(std::vector<Float_t>(1, -1));
    save_triggerType = kTriggerFailure;
  }

  //Get the PMT info for hit time smearing
  G4String WCIDCollectionName = myDetector->GetIDCollectionName();
  WCSimPMTObject * PMT = myDetector->GetPMTPointer(WCIDCollectionName);

  //Loop over trigger times
  for(unsigned int itrigger = 0; itrigger < TriggerTimes.size(); itrigger++) {
    TriggerType_t triggertype = TriggerTypes[itrigger];
    //check if we've already saved this trigger
    if(triggertype != save_triggerType && save_triggerType != kTriggerUndefined)
      continue;
    float         triggertime = TriggerTimes[itrigger];
    std::vector<Float_t> triggerinfo = TriggerInfos[itrigger];

    //these are the boundary of the trigger gate: we want to add all digits within these bounds to the output collection
    float lowerbound = triggertime;
    float upperbound = triggertime + WCSimWCTriggerBase::LongTime;

#ifdef WCSIMWCTRIGGER_VERBOSE
    G4cout << "Saving trigger " << itrigger << " of type " << WCSimEnumerations::EnumAsString(triggertype)
	   << " in time range [" << lowerbound << ", " << upperbound << "]"
	   << " with trigger time " << triggertime
	   << " and additional trigger info";
    for(std::vector<Float_t>::iterator it = triggerinfo.begin(); it != triggerinfo.end(); ++it)
      G4cout << " " << *it;
    G4cout << G4endl;
#endif

    //loop over PMTs
    for (G4int i = 0; i < WCDCPMT->entries(); i++) {
      int tube=(*WCDCPMT)[i]->GetTubeID();
      //loop over digits in this PMT
      for ( G4int ip = 0; ip < (*WCDCPMT)[i]->GetTotalPe(); ip++){
	int digit_time  = (*WCDCPMT)[i]->GetTime(ip);
	if(digit_time >= lowerbound && digit_time <= upperbound) {
	  //hit in event window
	  //add it to DigitsCollection

	  //first smear the charge & time
	  float peSmeared = (*WCDCPMT)[i]->GetPe(ip);
	  float Q = (peSmeared > 0.5) ? peSmeared : 0.5;
	  G4double digihittime = -triggertime
	    + digit_time;
	  if(digihittime < 0)
	    continue;

	  //int parentID    = (*WCDCPMT)[i]->GetParentID(ip);

	  //get the composition information for the triggered digit
	  //WCDCPMT stores this information in pairs of (digit id, photon id)
	  //need to loop to ensure we get all the photons associated with the current digit (digit ip)
	  std::vector< std::pair<int,int> > digitized_composition = (*WCDCPMT)[i]->GetDigiCompositionInfo();
	  std::vector<int> triggered_composition;
	  for(std::vector< std::pair<int,int> >::iterator it = digitized_composition.begin(); it != digitized_composition.end(); ++it) {
	    if((*it).first == ip) {
	      triggered_composition.push_back((*it).second);
	    }
	    else if ((*it).first > ip)
	      break;
	  }//loop over digitized_composition

#ifdef WCSIMWCTRIGGER_VERBOSE
	  G4cout << "Saving digit on PMT " << tube
		 << " time " << digihittime
		 << " pe "   << peSmeared
		 << " digicomp";
	  for(unsigned int iv = 0; iv < triggered_composition.size(); iv++)
	    G4cout << " " << triggered_composition[iv];
	  G4cout << G4endl;
#endif
	  assert(triggered_composition.size());

	  //add hit
	  if ( DigiHitMap[tube] == 0) {
	    //this PMT has no digits saved yet; create a new WCSimWCDigi
	    WCSimWCDigiTrigger* Digi = new WCSimWCDigiTrigger();
	    Digi->SetTubeID(tube);
	    //Digi->AddParentID(parentID);
	    Digi->AddGate  (itrigger);
	    Digi->SetTime  (itrigger,digihittime);
	    Digi->SetPe    (itrigger,peSmeared);
	    Digi->AddPe    ();
	    Digi->AddDigiCompositionInfo(itrigger,triggered_composition);
	    DigiHitMap[tube] = DigitsCollection->insert(Digi);
	  }
	  else {
	    //this PMT has digits saved already; add information to the WCSimWCDigi
	    //(*DigitsCollection)[DigiHitMap[tube]-1]->AddParentID(parentID);
	    (*DigitsCollection)[DigiHitMap[tube]-1]->AddGate(itrigger);
	    (*DigitsCollection)[DigiHitMap[tube]-1]->SetTime(itrigger, digihittime);
	    (*DigitsCollection)[DigiHitMap[tube]-1]->SetPe  (itrigger, peSmeared);
	    (*DigitsCollection)[DigiHitMap[tube]-1]->AddPe  ();
	    (*DigitsCollection)[DigiHitMap[tube]-1]->AddDigiCompositionInfo(itrigger,triggered_composition);
	  }
	  if(remove_hits)
	    (*WCDCPMT)[i]->RemoveDigitizedGate(ip);
	}//digits within trigger window
      }//loop over Digits
    }//loop over PMTs
  }//loop over Triggers
  G4cout << "WCSimWCTriggerBase::FillDigitsCollection. Number of entries in output digit collection: " << DigitsCollection->entries() << G4endl;

}


void WCSimWCTriggerBase::AlgNoTrigger(WCSimWCDigitsCollection* WCDCPMT, bool remove_hits, bool test) {

  if( output_txt ){
    ofstream myfile_hits;
    int nchar = (ceil(log10(event_number))+1);
    char * num =  (char*)malloc(sizeof(char)*nchar);
    sprintf(num, "%d", event_number);
    std::string event_file_base = "all_hits_";
    std::string event_file_suffix = ".txt";
    std::string event_file = event_file_base + num + event_file_suffix;
    event_number++;
    myfile_hits.open (event_file.c_str());
    
    for (G4int i = 0 ; i < WCDCPMT->entries() ; i++) {
      int tube=(*WCDCPMT)[i]->GetTubeID();
      for ( G4int ip = 0 ; ip < (*WCDCPMT)[i]->GetTotalPe() ; ip++) {
	int digit_time = (*WCDCPMT)[i]->GetTime(ip);
	myfile_hits << tube << "  " << digit_time << " \n";
      }
    }
    myfile_hits.close();
  }

  if( output_txt ){
    std::vector<WCSimPmtInfo*> *pmts = myDetector->Get_Pmts();
    
    ofstream myfile_pmts;
    myfile_pmts.open ("all_pmts.txt");
    
    for (G4int i = 0 ; i < pmts->size() ; i++) {
      
      WCSimPmtInfo* pmtinfo;
      pmtinfo = (WCSimPmtInfo*)pmts->at( i );
      
      myfile_pmts << pmtinfo->Get_tubeid() << "  " << pmtinfo->Get_transx() << "  " << pmtinfo->Get_transy() << "  " << pmtinfo->Get_transz() << " \n";
    }
    myfile_pmts.close();
  }

  if( output_txt ){
    float radius,height;
    height = 0;
    radius = 0;
    if(myDetector->GetDetectorName() == "SuperK" || myDetector->GetDetectorName() == "SuperK_20inchPMT_20perCent" ||
       myDetector->GetDetectorName() == "SuperK_20inchBandL_20perCent" || myDetector->GetDetectorName() == "SuperK_12inchBandL_15perCent" ||
       myDetector->GetDetectorName() == "SuperK_20inchBandL_14perCent" ||
       myDetector->GetDetectorName() == "Cylinder_60x74_20inchBandL_14perCent()" ||
       myDetector->GetDetectorName() == "Cylinder_60x74_20inchBandL_40perCent()") {
      height = myDetector->GetWaterTubeLength()/2.;
      radius = myDetector->GetWaterTubeRadius();
    }
    
    ofstream myfile_detector;
    myfile_detector.open ("detector.txt");
    myfile_detector << height*2./10. << "  " << radius/10. << " 25.4 \n";
    myfile_detector.close();
  }

  //Does not doanything, just writes out all hits
  TriggerType_t this_triggerType = kTriggerNoTrig;
  std::vector<Float_t> triggerinfo;
  Int_t Ndigits=0;
  for (G4int i = 0 ; i < WCDCPMT->entries() ; i++) {
    for ( G4int ip = 0 ; ip < (*WCDCPMT)[i]->GetTotalPe() ; ip++) {
      Ndigits++;
    }
  }
  triggerinfo.push_back(Ndigits);
  TriggerTypes.push_back(kTriggerNoTrig);
  TriggerInfos.push_back(triggerinfo);
  TriggerTimes.push_back(0.);

  FillDigitsCollectionNoTrigger(WCDCPMT, remove_hits, this_triggerType);
}

void WCSimWCTriggerBase::AlgTestVertexTrigger(WCSimWCDigitsCollection* WCDCPMT, bool remove_hits, bool test)
{
 
  if( output_txt ){
    ofstream myfile_hits;
    int nchar = (ceil(log10(event_number))+1);
    char * num =  (char*)malloc(sizeof(char)*nchar);
    sprintf(num, "%d", event_number);
    std::string event_file_base = "all_hits_";
    std::string event_file_suffix = ".txt";
    std::string event_file = event_file_base + num + event_file_suffix;
    event_number++;
    myfile_hits.open (event_file.c_str());
    
    for (G4int i = 0 ; i < WCDCPMT->entries() ; i++) {
      int tube=(*WCDCPMT)[i]->GetTubeID();
      for ( G4int ip = 0 ; ip < (*WCDCPMT)[i]->GetTotalPe() ; ip++) {
	int digit_time = (*WCDCPMT)[i]->GetTime(ip);
	myfile_hits << tube << "  " << digit_time << " \n";
      }
    }
    myfile_hits.close();
  }

  nhitsVTXmap.clear();
  vtxVector.clear();
  digit_times.clear();
 
  // Get the info for pmt positions                                                                                                                                              
  std::vector<WCSimPmtInfo*> *pmts = myDetector->Get_Pmts();

  if( output_txt ){
    ofstream myfile_pmts;
    myfile_pmts.open ("all_pmts.txt");
    
    for (G4int i = 0 ; i < pmts->size() ; i++) {
      
      WCSimPmtInfo* pmtinfo;
      pmtinfo = (WCSimPmtInfo*)pmts->at( i );
      
      myfile_pmts << pmtinfo->Get_tubeid() << "  " << pmtinfo->Get_transx() << "  " << pmtinfo->Get_transy() << "  " << pmtinfo->Get_transz() << " \n";
    }
    myfile_pmts.close();
  }

  //Now we will try to find triggers
  //loop over PMTs, and Digits in each PMT. 
  // If nhits > Threshhold in a time window, then we have a trigger
  // If this cut fails, attempt an ITC ratio trigger

  int ntrig = 0;
  int window_start_time = 0;
  int window_end_time   = WCSimWCTriggerBase::LongTime - ndigitsWindow;
  int window_step_size  = 10; //step the search window along this amount if no trigger is found
  float lasthit;
  
  int time_bins = int(floor(WCSimWCTriggerBase::LongTime/window_step_size));
  
  G4cout << "WCSimWCTriggerBase::SubNHits. Number of entries in input digit collection: " << WCDCPMT->entries() << G4endl;
#ifdef WCSIMWCTRIGGERBASE_VERBOSE
  int temp_total_pe = 0;
  for (G4int i = 0 ; i < WCDCPMT->entries() ; i++) {
    temp_total_pe += (*WCDCPMT)[i]->GetTotalPe();
  }
  G4cout << "WCSimWCTriggerBase::AlgNHits. " << temp_total_pe << " total p.e. input" << G4endl;
#endif

  //Generate verticies
  //skip for now to make the algorithm

  //generate vertices automatically.
  //superK
  float radius,height;
  //SK
  //height = 36000./2.; //mm middle of tank == 0 so half the height
  //radius = 33681.5/2.; //mm
  //HK
  //Automatically get height and width of the tank
  //SuperK and other cylindrical derived tanks
  height = 0;
  radius = 0;
  if(myDetector->GetDetectorName() == "SuperK" || myDetector->GetDetectorName() == "SuperK_20inchPMT_20perCent" ||
     myDetector->GetDetectorName() == "SuperK_20inchBandL_20perCent" || myDetector->GetDetectorName() == "SuperK_12inchBandL_15perCent" ||
     myDetector->GetDetectorName() == "SuperK_20inchBandL_14perCent" ||
     myDetector->GetDetectorName() == "Cylinder_60x74_20inchBandL_14perCent()" ||
     myDetector->GetDetectorName() == "Cylinder_60x74_20inchBandL_40perCent") {
    height = myDetector->GetWaterTubeLength()/2.-1640-1000.;
    radius = myDetector->GetWaterTubeRadius()-1604-1000.;
  }
  std::cout<< "HEIGHT "<<height*2.<<" Radius "<<radius<<"\n";
  //  height = 60000./2.; //mm middle of tank == 0 so half the height                                                                       
  //radius = 74000./2.; //mm 
  //generate a box of verticies.  Save only those within the radius given.
  //5m = 5000mm spacing
  //



  if( output_txt ){
    ofstream myfile_detector;
    myfile_detector.open ("detector.txt");
    myfile_detector << height*2./10. << "  " << radius/10. << " 25.4 \n";
    myfile_detector.close();
  }

  ofstream myfile_log;
  myfile_log.open ("trigger_log.txt");


  int n_vtx = 0;
  for(int i=-1*height;i <= height;i=i+5000) {
    for(int j=-1*radius;j<=radius;j=j+5000) {
      for(int k=-1*radius;k<=radius;k=k+5000) {
	if(sqrt(pow(j,2)+pow(k,2)) > radius)
	  continue;
	vtxVector.push_back(G4ThreeVector(j*1.,k*1.,i*1.));
	n_vtx++;
      }
    }
  }
 

   nhitsVTXmap.resize(n_vtx);
   for(int i = 0;i<n_vtx;i++) 
     nhitsVTXmap.at(i).resize(time_bins);

   Float_t hit_pos[3];
  
  float triggertime; //save each digit time, because the trigger time is the time of the first hit above threshold
  bool triggerfound = false;
 
  //internal to this routine we want to save trigger windows from the std Nhits trigger so to search for triggers 
  //using test vertices outside this window.

  //loop over all hits to fill maps in 5ns time windows.

  //digit_times.clear();
   
   
  //speed of light in water
  double cLightH20 = CLHEP::c_light/1.3330; 
  myfile_log << " c_light " << CLHEP::c_light << " cLightH20 " << cLightH20 << " \n";
  
  int tube;
  WCSimPmtInfo* pmtinfo;
  Double_t diff_x,diff_y,diff_z;
  Double_t dist_tube;
  Double_t tofPMT;
  for (G4int i = 0 ; i < WCDCPMT->entries() ; i++) {
    tube=(*WCDCPMT)[i]->GetTubeID();
    //get PMT position
    pmtinfo = (WCSimPmtInfo*)pmts->at( tube - 1 );
    //hit positions in cm, conveet to mm as the CLHEP untis are in mm and ns
    hit_pos[0] = 10.*pmtinfo->Get_transx();
    hit_pos[1] = 10.*pmtinfo->Get_transy();
    hit_pos[2] = 10.*pmtinfo->Get_transz();
    
    //loop over vertices, calculate distance from vertex to PMT.
    for(int j=0;j<vtxVector.size();j++) {
      diff_x = hit_pos[0]-vtxVector.at(j).x();                                                                                                                  
      diff_y = hit_pos[1]-vtxVector.at(j).y();                                                                                                               
      diff_z = hit_pos[2]-vtxVector.at(j).z();
      //distance in mm
      dist_tube = sqrt(pow(diff_x,2)+pow(diff_y,2)+pow(diff_z,2));
      //time for light to travel this distance in ns
      tofPMT = dist_tube/cLightH20;
      //Loop over each Digit in this PMT                                                                                                         
      for ( G4int ip = 0 ; ip < (*WCDCPMT)[i]->GetTotalPe() ; ip++) {
	int digit_time = (*WCDCPMT)[i]->GetTime(ip);
	if(digit_time >= WCSimWCTriggerBase::LongTime)
	  continue;
	double cor_time = (digit_time - tofPMT + 600)/window_step_size;
	//time is shifted by 600ns in order to ensure non-zero times (correcting for speed of light)
	if(int(floor(cor_time)) < 0) {
	  continue;
	  
	}
	nhitsVTXmap.at(j)[int(floor(cor_time))]++;
	//Also add a hit to the next bin
	//nhitsVTXmap.at(j)[int(floor(digit_time_5ns-tofPMT/window_step_size + 600./window_step_size)) + 1]++;
	
      }    
    }
  }
   

    //Now we want to use the tof corrected times to try to find other triggers
    //instead of a 200ns window we will use a smaller one 20ns.
    //This will have to be done in a slightly different manner looping over all 5ns time windows from
    //0 to Longtime.  This is due to wanting to check the same time window for each test vertex
    
    int count = 0;
    //    std::vector<std::pair<int,int> > PossibleTrigger;
    //std::vector<int> PossibleTriggerCount;
    //std::vector<std::pair<int, std::pair<int,int> > > Possibletrigger2;//vtxindex, time, count
    PossibleTrigger.clear();
    PossibleTriggerCount.clear();
    //Possibletrigger2.clear();
    //Possibletrigger2.resize(int(floor(WCSimWCTriggerBase::LongTime/(window_step_size*2))));
    
   
    while(count<time_bins - 1) { //time_bins - 1 as we are checking the i-th
      //std::cout<<"count "<<count<<"\n";
      //and ith+1 bin at the same time.
      //
      //hits have been added to their time bin plus the next time bin 
      //(giving a sum of the two bins in the second bin)
      int maxnpmt=0;
      int vtxindex=-1;
      int npmt=0;
      for(int j=0;j<vtxVector.size();j++) {                                                                                                                   
	//sum the number of hit PMTs in this time window                                                                                                            
	npmt = nhitsVTXmap.at(j).at(count) + nhitsVTXmap.at(j).at(count + 1);
        if(npmt > maxnpmt) {
          maxnpmt = npmt;
          vtxindex = j;
        }
      }
      if(maxnpmt > ndigitsThreshold) {//trigger
	//std::cout<<"Trigger "<<" "<<vtxindex<<" "<<(count+2)<<" "<<floor((count+2)/2)<<" maxpmt "<<maxnpmt<<"\n";
	//count = count + 2;
	PossibleTrigger.push_back(std::make_pair(vtxindex,(count+2)*window_step_size));
	PossibleTriggerCount.push_back(maxnpmt);
	//20ns bins
	//std::cout<<"Possible "<<(count+2)*window_step_size<<" "<<maxnpmt<<"\n";
	//Possibletrigger2[floor(count/2)]=(std::make_pair(vtxindex,std::make_pair(count*window_step_size,maxnpmt)));
	myfile_log << " time " << (count+2)*window_step_size << " vertex (" << vtxVector.at(vtxindex).x() << ", " << vtxVector.at(vtxindex).y() << ", " << vtxVector.at(vtxindex).z() << ") npmts " << maxnpmt << " \n";
      }
      //      else {
      count++;
      //}
    }

    myfile_log << " n candidates: " << PossibleTrigger.size() << " \n";

//     //sort the possible triggers by time  
//    std::sort(PossibleTrigger.begin(), PossibleTrigger.end(), sort_pair_second<int, int>());
    //std::vector<std::pair<int,int> > PossibleTrigger2;
    PossibleTrigger2.clear();
    //std::vector<int> PossibleTriggerCount2;
    PossibleTriggerCount2.clear(); 
    int counter=0;
     int index, time_lower, time_upper, numpmt;
     int index_prev=-1, time_lower_prev=-1, time_upper_prev=-1, numpmt_prev=-1;
     int max_pmt=-1,max_index=-1,max_time=-1;
     for(int i=0;i<PossibleTrigger.size();i++) {
       index =      PossibleTrigger.at(i).first;
       time_upper = PossibleTrigger.at(i).second/window_step_size;
       time_lower = time_upper-1;
      
       numpmt = PossibleTriggerCount.at(i);
       
       if(i == 0 && numpmt > 0) {
	 max_pmt = numpmt;
	 max_index = index;
	 max_time = time_upper;
	 if(i == PossibleTrigger.size()-1) {
           PossibleTrigger2.push_back(std::make_pair(max_index,max_time*window_step_size));
           PossibleTriggerCount2.push_back(max_pmt);
	 }
       }
       else if(i!=0 && abs(max_time - time_upper) < 500/window_step_size && numpmt > max_pmt) {
	 max_pmt = numpmt;
	 max_index = index;
	 max_time = time_upper;
	 if(i == PossibleTrigger.size()-1) {
	   PossibleTrigger2.push_back(std::make_pair(max_index,max_time*window_step_size));
	   PossibleTriggerCount2.push_back(max_pmt);
	 }
       }
       else if(i!=0 && abs(max_time - time_upper) > 500/window_step_size) {
	 PossibleTrigger2.push_back(std::make_pair(max_index,max_time*window_step_size));
	 PossibleTriggerCount2.push_back(max_pmt);
	 max_pmt = numpmt;
	 max_index = index;
	 max_time = time_upper;     
	 if(i == PossibleTrigger.size()-1) {
	   PossibleTrigger2.push_back(std::make_pair(max_index,max_time*window_step_size));
	   PossibleTriggerCount2.push_back(max_pmt);
	   //std::cout<<"TRIGGER "<< <<"\n";
	 }
       }
	 else if(i == PossibleTrigger.size()-1) {
           PossibleTrigger2.push_back(std::make_pair(max_index,max_time*window_step_size));
           PossibleTriggerCount2.push_back(max_pmt);
           //std::cout<<"TRIGGER "<< <<"\n";                                                                                                                                   
	 }
     
       //  std::cout<<"max_pmt "<<max_time<<" "<<max_pmt<<"\n";
       time_upper_prev = time_upper;
       time_lower_prev = time_lower;
       index_prev = index; 
       numpmt_prev = numpmt;
     }


     TriggerPairsCorT.clear();
     TriggerNormT.clear();

     int time_start=-9999;
     std::vector<Float_t> triggerinfo;
    for(int i=0;i<PossibleTrigger2.size();i++) {
      //once a trigger is found, we must jump to 950ns in the future before searching for the next
      if(PossibleTrigger2.at(i).second > time_start) {
	TriggerPairsCorT.push_back(std::make_pair(PossibleTrigger2.at(i).first,PossibleTrigger2.at(i).second));
	time_start = PossibleTrigger2.at(i).second + WCSimWCTriggerBase::eventgateup;
    
	//set trigger details to be passed onto fill routine
	//trigger time in non-corrected times is timeTrig.
	int triggertime = PossibleTrigger2.at(i).second; 
	triggertime -= (int)triggertime % 5;
	std::cout<<"TRIGGER "<<triggertime<<" "<<PossibleTriggerCount2.at(i)<<"\n";

	myfile_log << " triggertime: " << triggertime << ", npmts: " << PossibleTriggerCount2.at(i) << ", x: " << vtxVector.at((Float_t)PossibleTrigger2.at(i).first).x() << ", y: " << vtxVector.at((Float_t)PossibleTrigger2.at(i).first).y() << ", z: " << vtxVector.at((Float_t)PossibleTrigger2.at(i).first).z() << " \n";

	//std::vector<Float_t> triggerinfo;
	triggerinfo.clear();
	triggerinfo.push_back(vtxVector.at((Float_t)PossibleTrigger2.at(i).first).x());
	triggerinfo.push_back(vtxVector.at((Float_t)PossibleTrigger2.at(i).first).y());
	triggerinfo.push_back(vtxVector.at((Float_t)PossibleTrigger2.at(i).first).z());
	triggerinfo.push_back(PossibleTriggerCount2.at(i));
	triggerinfo.push_back(PossibleTrigger2.at(i).second - 200.);
	TriggerTypes.push_back(kTriggerTestVertex);
	TriggerInfos.push_back(triggerinfo);
	TriggerTimes.push_back(triggertime-200.);
      }
    }

    FillDigitsCollection(WCDCPMT, remove_hits, kTriggerTestVertex);

    myfile_log.close();

}


void WCSimWCTriggerBase::FillDigitsCollection(WCSimWCDigitsCollection* WCDCPMT, bool remove_hits, TriggerType_t save_triggerType)
{
  //Adds the digits within the trigger window to the output WCSimWCDigitsCollection
  // optionally removes digits from the input digits collection (when running different Alg* methods concurently) 
  // so they are not used in subsequent trigger decisions or saved twice
  //Also, only save digits of a specific type (again for when running different Alg* methods concurently)

  // Add dummy triggers / exit without saving triggers as required
  //
  //saveFailuresMode = 0 - save only triggered events
  //saveFailuresMode = 1 - save both triggered & not triggered events
  //saveFailuresMode = 2 - save only not triggered events
  if(TriggerTimes.size()) {
    if(saveFailuresMode == 2)
      return;
  }
  else {
    if(saveFailuresMode == 0)
      return;
    TriggerTypes.push_back(kTriggerFailure);
    TriggerTimes.push_back(saveFailuresTime);
    TriggerInfos.push_back(std::vector<Float_t>(1, -1));
    save_triggerType = kTriggerFailure;
  }

  //Get the PMT info for hit time smearing
  G4String WCIDCollectionName = myDetector->GetIDCollectionName();
  WCSimPMTObject * PMT = myDetector->GetPMTPointer(WCIDCollectionName);

  //Loop over trigger times
  for(unsigned int itrigger = 0; itrigger < TriggerTimes.size(); itrigger++) {
    TriggerType_t triggertype = TriggerTypes[itrigger];
    //check if we've already saved this trigger
    if(triggertype != save_triggerType && save_triggerType != kTriggerUndefined)
      continue;
    float         triggertime = TriggerTimes[itrigger];
    std::vector<Float_t> triggerinfo = TriggerInfos[itrigger];

    //these are the boundary of the trigger gate: we want to add all digits within these bounds to the output collection
    float lowerbound = triggertime + WCSimWCTriggerBase::eventgatedown;
    float upperbound = triggertime + WCSimWCTriggerBase::eventgateup;

#ifdef WCSIMWCTRIGGER_VERBOSE
    G4cout << "Saving trigger " << itrigger << " of type " << WCSimEnumerations::EnumAsString(triggertype)
	   << " in time range [" << lowerbound << ", " << upperbound << "]"
	   << " with trigger time " << triggertime
	   << " and additional trigger info";
    for(std::vector<Float_t>::iterator it = triggerinfo.begin(); it != triggerinfo.end(); ++it)
      G4cout << " " << *it;
    G4cout << G4endl;
#endif

    //loop over PMTs
    for (G4int i = 0; i < WCDCPMT->entries(); i++) {
      int tube=(*WCDCPMT)[i]->GetTubeID();
      //loop over digits in this PMT
      for ( G4int ip = 0; ip < (*WCDCPMT)[i]->GetTotalPe(); ip++){
	int digit_time  = (*WCDCPMT)[i]->GetTime(ip);
	if(digit_time >= lowerbound && digit_time <= upperbound) {
	  //hit in event window
	  //add it to DigitsCollection

	  //first smear the charge & time
	  float peSmeared = (*WCDCPMT)[i]->GetPe(ip);
	  float Q = (peSmeared > 0.5) ? peSmeared : 0.5;
	  G4double digihittime = -triggertime
	    + WCSimWCTriggerBase::offset
	    + digit_time
	    + PMT->HitTimeSmearing(Q);
	  if(digihittime < 0)
	    continue;

	  //int parentID    = (*WCDCPMT)[i]->GetParentID(ip);

	  //get the composition information for the triggered digit
	  //WCDCPMT stores this information in pairs of (digit id, photon id)
	  //need to loop to ensure we get all the photons associated with the current digit (digit ip)
	  std::vector< std::pair<int,int> > digitized_composition = (*WCDCPMT)[i]->GetDigiCompositionInfo();
	  std::vector<int> triggered_composition;
	  for(std::vector< std::pair<int,int> >::iterator it = digitized_composition.begin(); it != digitized_composition.end(); ++it) {
	    if((*it).first == ip) {
	      triggered_composition.push_back((*it).second);
	    }
	    else if ((*it).first > ip)
	      break;
	  }//loop over digitized_composition

#ifdef WCSIMWCTRIGGER_VERBOSE
	  G4cout << "Saving digit on PMT " << tube
		 << " time " << digihittime
		 << " pe "   << peSmeared
		 << " digicomp";
	  for(unsigned int iv = 0; iv < triggered_composition.size(); iv++)
	    G4cout << " " << triggered_composition[iv];
	  G4cout << G4endl;
#endif
	  assert(triggered_composition.size());

	  //add hit
	  if ( DigiHitMap[tube] == 0) {
	    //this PMT has no digits saved yet; create a new WCSimWCDigi
	    WCSimWCDigiTrigger* Digi = new WCSimWCDigiTrigger();
	    Digi->SetTubeID(tube);
	    //Digi->AddParentID(parentID);
	    Digi->AddGate  (itrigger);
	    Digi->SetTime  (itrigger,digihittime);
	    Digi->SetPe    (itrigger,peSmeared);
	    Digi->AddPe    ();
	    Digi->AddDigiCompositionInfo(itrigger,triggered_composition);
	    DigiHitMap[tube] = DigitsCollection->insert(Digi);
	  }
	  else {
	    //this PMT has digits saved already; add information to the WCSimWCDigi
	    //(*DigitsCollection)[DigiHitMap[tube]-1]->AddParentID(parentID);
	    (*DigitsCollection)[DigiHitMap[tube]-1]->AddGate(itrigger);
	    (*DigitsCollection)[DigiHitMap[tube]-1]->SetTime(itrigger, digihittime);
	    (*DigitsCollection)[DigiHitMap[tube]-1]->SetPe  (itrigger, peSmeared);
	    (*DigitsCollection)[DigiHitMap[tube]-1]->AddPe  ();
	    (*DigitsCollection)[DigiHitMap[tube]-1]->AddDigiCompositionInfo(itrigger,triggered_composition);
	  }
	  if(remove_hits)
	    (*WCDCPMT)[i]->RemoveDigitizedGate(ip);
	}//digits within trigger window
      }//loop over Digits
    }//loop over PMTs
  }//loop over Triggers
  G4cout << "WCSimWCTriggerBase::FillDigitsCollection. Number of entries in output digit collection: " << DigitsCollection->entries() << G4endl;

}




// *******************************************
// CONTAINER CLASS
// *******************************************

G4Allocator<WCSimWCDigiTrigger> WCSimWCDigiTriggerAllocator;

WCSimWCDigiTrigger::WCSimWCDigiTrigger()
{
  Gates.clear();
  tubeID = 0;
  pe.clear();
  time.clear();
  fDigiComp.clear();
  totalPe = 0;
}

WCSimWCDigiTrigger::~WCSimWCDigiTrigger(){;}

WCSimWCDigiTrigger::WCSimWCDigiTrigger(const WCSimWCDigiTrigger& right)
  :G4VDigi()
{
  // in principle assignment = is defined for containers...
  Gates = right.Gates;
  tubeID = right.tubeID;
  pe     = right.pe;
  time   = right.time;
  fDigiComp = right.fDigiComp;
  totalPe = right.totalPe;
}

const WCSimWCDigiTrigger& WCSimWCDigiTrigger::operator=(const WCSimWCDigiTrigger& right)
{
  Gates = right.Gates;
  tubeID = right.tubeID;
  pe     = right.pe;
  time   = right.time;
  fDigiComp = right.fDigiComp;
  totalPe = right.totalPe;
  return *this;
}

void WCSimWCDigiTrigger::Print()
{
  G4cout << "TubeID: " << tubeID
         << ", Number of Gates: " << NumberOfGates()
	 << G4endl;
  std::multimap<int,float>::iterator it_pe   = pe.begin();
  std::multimap<int,float>::iterator it_time = time.begin();
  for( ; it_pe != pe.end(), it_time != time.end(); ++it_pe, ++it_time) {
    if(it_pe->first != it_time->first) {
      G4cerr << "WCSimWCDigiTrigger::Print() pe and time gate counters disagree!" << G4endl;
      exit(-1);
    }
    G4cout  << "Gate = " << it_pe->first
            << " PE: "   << it_pe->second
            << " Time: " << it_time->second
	    << G4endl;
  }
}



// *******************************************
// DERIVED CLASS
// *******************************************

WCSimWCTriggerNDigits::WCSimWCTriggerNDigits(G4String name,
					 WCSimDetectorConstruction* myDetector,
					 WCSimWCDAQMessenger* myMessenger)
  :WCSimWCTriggerBase(name, myDetector, myMessenger)
{
  triggerClassName = "NDigits";
}

WCSimWCTriggerNDigits::~WCSimWCTriggerNDigits()
{
}

void WCSimWCTriggerNDigits::DoTheWork(WCSimWCDigitsCollection* WCDCPMT) {
  //Apply an NDigits trigger
  bool remove_hits = false;
  AlgNDigits(WCDCPMT, remove_hits);
}


// *******************************************
// DERIVED CLASS
// *******************************************


WCSimWCTriggerNoTrigger::WCSimWCTriggerNoTrigger(G4String name,
					 WCSimDetectorConstruction* myDetector,
					 WCSimWCDAQMessenger* myMessenger)
  :WCSimWCTriggerBase(name, myDetector, myMessenger)
{
  triggerClassName = "NoTrigger";
}

WCSimWCTriggerNoTrigger::~WCSimWCTriggerNoTrigger()
{
}

void WCSimWCTriggerNoTrigger::DoTheWork(WCSimWCDigitsCollection* WCDCPMT) {
  //Apply an NDigits trigger
  bool remove_hits = false;
  AlgNoTrigger(WCDCPMT, remove_hits);
}

// *******************************************
// DERIVED CLASS
// *******************************************


WCSimWCTriggerTestVertex::WCSimWCTriggerTestVertex(G4String name,
					 WCSimDetectorConstruction* myDetector,
					 WCSimWCDAQMessenger* myMessenger)
  :WCSimWCTriggerBase(name, myDetector, myMessenger)
{
  triggerClassName = "TestVertex ";
}

WCSimWCTriggerTestVertex::~WCSimWCTriggerTestVertex()
{
}

void WCSimWCTriggerTestVertex::DoTheWork(WCSimWCDigitsCollection* WCDCPMT) {
  //Apply an NDigits trigger
  bool remove_hits = false;
  AlgTestVertexTrigger(WCDCPMT, remove_hits);
}


// *******************************************
// DERIVED CLASS
// *******************************************

WCSimWCTriggerNDigits2::WCSimWCTriggerNDigits2(G4String name,
					 WCSimDetectorConstruction* myDetector,
					 WCSimWCDAQMessenger* myMessenger)
  :WCSimWCTriggerBase(name, myDetector, myMessenger)
{
  triggerClassName = "NDigits2";
}

WCSimWCTriggerNDigits2::~WCSimWCTriggerNDigits2(){
}


void WCSimWCTriggerNDigits2::DoTheWork(WCSimWCDigitsCollection* WCDCPMT) {
  //This calls 2 trigger algorithms; the second algorithm is called on hits that failed the first algorithm
  //(for a second trigger working on hits that passed a pretrigger, FillDigitsCollection() needs to have a new option)

  //Make a copy of the input DigitsCollection, so we can remove hits from the copy
  WCSimWCDigitsCollection* WCDCPMTCopy = new WCSimWCDigitsCollection(*WCDCPMT);
  
  //Apply an NDigits trigger
  bool remove_hits = true;
  AlgNDigits(WCDCPMTCopy, remove_hits);

  //Apply an NDigits trigger with a lower threshold & different saved trigger type
  remove_hits = false;
  bool ndigits_test = true;
  AlgNDigits(WCDCPMTCopy, remove_hits, ndigits_test);
}
