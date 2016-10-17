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



// *******************************************
// BASE CLASS
// *******************************************

#ifndef WCSIMWCTRIGGER_VERBOSE
//#define WCSIMWCTRIGGER_VERBOSE
#endif

const double WCSimWCTriggerBase::offset = 950.0; // ns. apply offset to the digit time
const double WCSimWCTriggerBase::LongTime = 1E6; // ns = 1ms. event time


WCSimWCTriggerBase::WCSimWCTriggerBase(G4String name,
				       WCSimDetectorConstruction* inDetector,
				       WCSimWCDAQMessenger* myMessenger)
  :G4VDigitizerModule(name), DAQMessenger(myMessenger), myDetector(inDetector), triggerClassName("")
{
  G4String colName = "WCDigitizedCollection";
  collectionName.push_back(colName);

  ReInitialize();

  if(DAQMessenger == NULL) {
    G4cerr << "WCSimWCDAQMessenger pointer is NULL when passed to WCSimWCTriggerBase constructor. Exiting..."
           << G4endl;
    exit(-1);
  }

  digitizeCalled = false;
}

WCSimWCTriggerBase::~WCSimWCTriggerBase(){
}


void WCSimWCTriggerBase::GetVariables()
{
  //set the options to class-specific defaults
  multiDigitsPerTrigger    = GetDefaultMultiDigitsPerTrigger();
  ndigitsThreshold         = GetDefaultNDigitsThreshold();
  ndigitsWindow            = GetDefaultNDigitsWindow();
  ndigitsPreTriggerWindow  = GetDefaultNDigitsPreTriggerWindow();
  ndigitsPostTriggerWindow = GetDefaultNDigitsPostTriggerWindow();

  //read the .mac file to override them
  if(DAQMessenger != NULL) {
    DAQMessenger->TellMeAboutTheTrigger(this);
    DAQMessenger->SetTriggerOptions();
  }
  else {
    G4cerr << "WCSimWCDAQMessenger pointer is NULL when used in WCSimWCTriggerBase::GetVariables(). Exiting..." 
	   << G4endl;
    exit(-1);
  }

  G4cout << (multiDigitsPerTrigger ? "Using mutiple digits per PMT per trigger" : "Using a maximum of 1 digit per PMT per trigger" ) << G4endl
	 << "Using NDigits threshold " << ndigitsThreshold
	 << (ndigitsAdjustForNoise ? " (will be adjusted for noise)" : "") << G4endl
	 << "Using NDigits trigger window " << ndigitsWindow << " ns" << G4endl
	 << "Using NDigits event pretrigger window " << ndigitsPreTriggerWindow << " ns" << G4endl
	 << "Using NDigits event posttrigger window " << ndigitsPostTriggerWindow << " ns" << G4endl;
  if(saveFailuresMode == 0)
    G4cout << "Saving only triggered digits" << G4endl;
  else if(saveFailuresMode == 1)
    G4cout << "Saving both triggered and not-triggered digits" << G4endl;
  else if(saveFailuresMode == 2)
    G4cout << "Saving only not-triggered digits" << G4endl;
  if(saveFailuresMode > 0)
    G4cout << "Using SaveFailures trigger time" << saveFailuresTime << " ns" << G4endl
	   << "Using SaveFailures event pretrigger window " << saveFailuresPreTriggerWindow << " ns" << G4endl
	   << "Using SaveFailures event posttrigger window " << saveFailuresPostTriggerWindow << " ns" << G4endl;
}

int WCSimWCTriggerBase::GetPreTriggerWindow(TriggerType_t t)
{
  switch(t) {
  case kTriggerNDigits:
  case kTriggerNDigitsTest:
    return ndigitsPreTriggerWindow;
    break;
  case kTriggerFailure:
    return saveFailuresPreTriggerWindow;
    break;
  default:
    G4cerr << "WCSimWCTriggerBase::GetPreTriggerWindow() Unknown trigger type " << t
	   << "\t" << WCSimEnumerations::EnumAsString(t) << G4endl;
    exit(-1);
    break;
  }
}

int WCSimWCTriggerBase::GetPostTriggerWindow(TriggerType_t t)
{
  switch(t) {
  case kTriggerNDigits:
  case kTriggerNDigitsTest:
    return ndigitsPostTriggerWindow;
    break;
  case kTriggerFailure:
    return saveFailuresPostTriggerWindow;
    break;
  default:
    G4cerr << "WCSimWCTriggerBase::GetPostTriggerWindow() Unknown trigger type " << t
	   << "\t" << WCSimEnumerations::EnumAsString(t) << G4endl;
    exit(-1);
    break;
  }
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
      window_start_time = triggertime + GetPostTriggerWindow(TriggerTypes.back());
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
  FillDigitsCollection(WCDCPMT, remove_hits, this_triggerType);
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

  //make sure the triggers are in time order
  SortTriggersByTime();

  //Loop over trigger times
  for(unsigned int itrigger = 0; itrigger < TriggerTimes.size(); itrigger++) {
    TriggerType_t triggertype = TriggerTypes[itrigger];
    //check if we've already saved this trigger
    if(triggertype != save_triggerType && save_triggerType != kTriggerUndefined)
      continue;
    float         triggertime = TriggerTimes[itrigger];
    std::vector<Float_t> triggerinfo = TriggerInfos[itrigger];

    //these are the boundary of the trigger gate: we want to add all digits within these bounds to the output collection
    float lowerbound = triggertime + GetPreTriggerWindow(triggertype);
    float upperbound = triggertime + GetPostTriggerWindow(triggertype);
    //need to check for double-counting - check if the previous upperbound is above the lowerbound
    if(itrigger) {
      float upperbound_previous = TriggerTimes[itrigger - 1] + GetPostTriggerWindow(TriggerTypes[itrigger - 1]);
      if(upperbound_previous > lowerbound) {
	//also need to check whether the previous upperbound is above the lowerbound
	//(different trigger windows for different trigger types can mean this trigger is completely contained within another)
	// if it is, we skip it
	if(upperbound_previous >= upperbound)
	  continue;
	lowerbound = upperbound_previous;
      }
    }

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
	    + WCSimWCTriggerBase::offset
	    + digit_time;

	  //get the composition information for the triggered digit
	  std::vector<int> triggered_composition = (*WCDCPMT)[i]->GetDigiCompositionInfo(ip);

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
  GetVariables();
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

WCSimWCTriggerNDigits2::WCSimWCTriggerNDigits2(G4String name,
					 WCSimDetectorConstruction* myDetector,
					 WCSimWCDAQMessenger* myMessenger)
  :WCSimWCTriggerBase(name, myDetector, myMessenger)
{
  triggerClassName = "NDigits2";
  GetVariables();
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
