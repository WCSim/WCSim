#include "WCSimWCTriggerSK4.hh"

// *******************************************
// DERIVED CLASS
// *******************************************

#ifndef WCSIMWCTRIGGER_VERBOSE
#define WCSIMWCTRIGGER_VERBOSE
#endif

const double WCSimWCTriggerNDigitsSK4::offsetSK4 = 950.0; // ns. apply offset to the digit time
const double WCSimWCTriggerNDigitsSK4::LongTimeSK4 = 1E6; // ns = 1ms. event time

WCSimWCTriggerNDigitsSK4::WCSimWCTriggerNDigitsSK4(G4String name,
					 WCSimDetectorConstruction* myDetector,
					 WCSimWCDAQMessenger* myMessenger)
  :WCSimWCTriggerBase(name, myDetector, myMessenger)
{
  triggerClassName = "NDigitsSK4";
  GetVariables();
}

WCSimWCTriggerNDigitsSK4::~WCSimWCTriggerNDigitsSK4()
{
}

void WCSimWCTriggerNDigitsSK4::DoTheWork(WCSimWCDigitsCollection* WCDCPMT) {
  //Apply an NDigits trigger
  bool remove_hits = true;

  //clear trigger vectors
  ClearTriggers();

  //clear additional vectors used for SKIV trigger class
  TriggerStartTimes.clear();
  TriggerEndTimes.clear();

  //SHE trigger
  AddNDigitsSubTrigger(70, 5000, 35000, kTriggerNDigitsSK4_SHE);
  //HE trigger
  AddNDigitsSubTrigger(50, 5000, 35000, kTriggerNDigitsSK4_HE);
  //LE trigger
  AddNDigitsSubTrigger(47, 5000, 35000, kTriggerNDigitsSK4_LE);
  //SLE trigger
  AddNDigitsSubTrigger(34, 500, 1000, kTriggerNDigitsSK4_SLE);

  //Apply the triggers
  AlgNDigits(WCDCPMT, remove_hits);


}

void WCSimWCTriggerNDigitsSK4::AlgNDigits(WCSimWCDigitsCollection* WCDCPMT, bool remove_hits)
{

  //if test is true, we run the algorithm with 1/2 the threshold, and kTriggerNDigitsTest
  //for testing multiple trigger algorithms at once

  std::vector<int> this_ndigitsThreshold_vec = ndigitsThreshold_vec;
  int num_subtriggers = (int)ndigitsThreshold_vec.size();
  std::vector<TriggerType_t> this_triggerType_vec = ndigitsTriggerType_vec;
  // if(test) {
  //   this_ndigitsThreshold /= 2;
  //   this_triggerType = kTriggerNDigitsTest;
  // }

  //Now we will try to find triggers
  //loop over PMTs, and Digits in each PMT.  If ndigits > Threshhold in a time window, then we have a trigger
  int subtrg_num = 0;

  int ntrig = 0;
  int window_start_time = 0;
  int window_end_time   = WCSimWCTriggerNDigitsSK4::LongTimeSK4 - ndigitsWindow;
  int window_step_size  = 5; //step the search window along this amount if no trigger is found
  float lasthit;
  std::vector<int> digit_times;
  bool first_loop = true;

  G4cout << "WCSimWCTriggerNDigitsSK4::AlgNDigits. Number of entries in input digit collection: " << WCDCPMT->entries() << G4endl;
#ifdef WCSIMWCTRIGGER_VERBOSE
  int temp_total_pe = 0;
  for (G4int i = 0 ; i < WCDCPMT->entries() ; i++) {
    temp_total_pe += (*WCDCPMT)[i]->GetTotalPe();
  }
  G4cout << "WCSimWCTriggerNDigitsSK4::AlgNDigits. " << temp_total_pe << " total p.e. input" << G4endl;
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
	float digit_charge = (*WCDCPMT)[i]->GetPe(ip);
	//hit in trigger window?
	if(digit_time >= window_start_time && digit_time <= (window_start_time + ndigitsWindow)
	   && digit_charge > 0) {
	  n_digits++;
	  digit_times.push_back(digit_time);
	}
	//G4cout << digit_time << G4endl;
	//get the time of the last hit (to make the loop shorter)
	if(first_loop && (digit_time > lasthit))
	  lasthit = digit_time;
      }//loop over Digits
    }//loop over PMTs

    //loop over subtriggers
    for(int subtrg = 0; subtrg<num_subtriggers; subtrg++){
    //if over threshold, issue trigger
      if(n_digits > this_ndigitsThreshold_vec.at(subtrg)) {
	ntrig++;
	//The trigger time is the time of the first hit above threshold
	std::sort(digit_times.begin(), digit_times.end());
	triggertime = digit_times[this_ndigitsThreshold_vec.at(subtrg)];
	triggertime -= (int)triggertime % 5;
	TriggerTimes.push_back(triggertime);
	TriggerStartTimes.push_back(triggertime - ndigitsPreTriggerWindow_vec.at(subtrg));
	TriggerEndTimes.push_back(triggertime + ndigitsPostTriggerWindow_vec.at(subtrg));
	TriggerTypes.push_back(this_triggerType_vec.at(subtrg));
	TriggerInfos.push_back(std::vector<Float_t>(1, n_digits));
	triggerfound = true;
	subtrg_num = subtrg;
	break;
      }
    }

#ifdef WCSIMWCTRIGGER_VERBOSE
    if(n_digits)
      G4cout << n_digits << " digits found in 200nsec trigger window ["
	     << window_start_time << ", " << window_start_time + ndigitsWindow
	     << "]. Threshold is: " << this_ndigitsThreshold_vec.at(subtrg_num) << G4endl;
#endif

    //move onto the next go through the timing loop
    if(triggerfound) {
      //      window_start_time = triggertime + GetPostTriggerWindow(TriggerTypes.back());
      window_start_time = triggertime + ndigitsPostTriggerWindow_vec.at(subtrg_num);
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
  
  G4cout << "Found " << ntrig << " NDigit triggers" << G4endl;
  //call FillDigitsCollection() whether any triggers are found or not
  // (what's saved depends on saveFailuresMode)
  FillDigitsCollection(WCDCPMT, remove_hits);
}

void WCSimWCTriggerNDigitsSK4::FillDigitsCollection(WCSimWCDigitsCollection* WCDCPMT, bool remove_hits)
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

  TriggerType_t  save_triggerType;

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

  //Loop over trigger times
  for(unsigned int itrigger = 0; itrigger < TriggerTimes.size(); itrigger++) {
    TriggerType_t triggertype = TriggerTypes[itrigger];
    //check if we've already saved this trigger
    //    if(triggertype != save_triggerType && save_triggerType != kTriggerUndefined)
      //continue;
      float         triggertime = TriggerTimes[itrigger];
    std::vector<Float_t> triggerinfo = TriggerInfos[itrigger];
    
    //these are the boundary of the trigger gate: we want to add all digits within these bounds to the output collection
    float lowerbound = TriggerStartTimes[itrigger]; //triggertime + GetPreTriggerWindow(triggertype);
    float upperbound = TriggerEndTimes[itrigger]; //triggertime + GetPostTriggerWindow(triggertype);

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

	  //first apply time offsets
	  float peSmeared = (*WCDCPMT)[i]->GetPe(ip);
	  G4double digihittime = -triggertime
	    + WCSimWCTriggerNDigitsSK4::offsetSK4
	    + digit_time;

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
	    //this PMT has no digits saved yet; create a new WCSimWCDigiTrigger
	    WCSimWCDigiTrigger* Digi = new WCSimWCDigiTrigger();
	    Digi->SetTubeID(tube);
	    Digi->AddGate  (itrigger);
	    Digi->SetTime  (itrigger,digihittime);
	    Digi->SetPe    (itrigger,peSmeared);
	    Digi->AddPe    ();
	    Digi->AddDigiCompositionInfo(itrigger,triggered_composition);
	    DigiHitMap[tube] = DigitsCollection->insert(Digi);
	  }
	  else {
	    //this PMT has digits saved already; add information to the WCSimWCDigiTrigger
	    (*DigitsCollection)[DigiHitMap[tube]-1]->AddGate(itrigger);
	    (*DigitsCollection)[DigiHitMap[tube]-1]->SetTime(itrigger, digihittime);
	    (*DigitsCollection)[DigiHitMap[tube]-1]->SetPe  (itrigger, peSmeared);
	    (*DigitsCollection)[DigiHitMap[tube]-1]->AddPe  ();
	    (*DigitsCollection)[DigiHitMap[tube]-1]->AddDigiCompositionInfo(itrigger,triggered_composition);
	  }
	  if(remove_hits)
	    (*WCDCPMT)[i]->RemoveDigitizedGate(ip);

	  //we've found a digit on this PMT. If we're restricting to just 1 digit per trigger window (e.g. SKI)
	  // then ignore later digits and break. This takes us to the next PMT
	  if(!multiDigitsPerTrigger)
	    break;
	}//digits within trigger window
      }//loop over Digits
    }//loop over PMTs
  }//loop over Triggers
  G4cout << "WCSimWCTriggerNDigitsSK4::FillDigitsCollection. Number of entries in output digit collection: " << DigitsCollection->entries() << G4endl;

}
