#ifndef WCSimWCTrigger_h
#define WCSimWCTrigger_h 1

#include "WCSimEnumerations.hh"
#include "WCSimWCDAQMessenger.hh"
#include "WCSimDetectorConstruction.hh"
#include "G4VDigitizerModule.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"
#include "globals.hh"
#include "Randomize.hh"
#include <map>
#include <vector>

class WCSimWCDigiTrigger;
typedef G4TDigiCollection<WCSimWCDigiTrigger> WCSimWCTriggeredDigitsCollection;

// *******************************************
// BASE CLASS
// *******************************************

/**
 * \class WCSimWCTriggerBase
 *
 * \brief The base class for WCSim triggering algorithms
 *
 * Concrete implementations of a trigger class should inherit from this class.
 * Minimally, only DoTheWork() needs to be implemented in the implementation class.
 *
 */

class WCSimWCTriggerBase : public G4VDigitizerModule
{
  
public:

  ///Create WCSimWCTriggerBase instance with knowledge of the detector and DAQ options
  WCSimWCTriggerBase(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*);
  
  virtual ~WCSimWCTriggerBase();

  /**
   * \brief The main user-callable routine of the class. Gets the input & creates the output WCSimWCTriggeredDigitsCollection's, then calls DoTheWork()
   *
   * The virtual keyword for this method is DEPRECATED
   * It is only defined virtual now because it is overridden in the old class (WCSimWCDigitizer)
   */
  virtual void Digitize();

  ///Returns the number of trigger gates in the event (i.e. the number of triggers passed)
  int NumberOfGatesInThisEvent() { return TriggerTimes.size(); }
  ///Get the time of the ith trigger
  Float_t              GetTriggerTime(int i) { return TriggerTimes[i];}
  ///Get the trigger type of the ith trigger
  TriggerType_t        GetTriggerType(int i) { return TriggerTypes[i];}
  ///Get the additional trigger information associated with the ith trigger
  std::vector<Float_t> GetTriggerInfo(int i) { return TriggerInfos[i];}

  //
  // Trigger algorithm option set methods
  //

  // NHits options
  ///Set the threshold for the NHits trigger
  void SetNHitsThreshold(G4int threshold) { nhitsThreshold = threshold; }
  ///Set the time window for the NHits trigger
  void SetNHitsWindow(G4int window) { nhitsWindow = window; }
  ///Automatically adjust the NHits threshold based on the average noise occupancy?
  void SetNHitsAdjustForNoise    (G4bool adjust)      { nhitsAdjustForNoise = adjust; }

  // Save trigger failures options
  ///Set the mode for saving failed triggers (0:save only triggered events, 1:save both triggered events & failed events, 2:save only failed events)
  void SetSaveFailuresMode       (G4int mode )        { saveFailuresMode = mode; }
  ///Set the dummy trigger time for the failed triggers
  void SetSaveFailuresTime       (G4double time )     { saveFailuresTime = time; }
  
  ///Knowledge of the dark rate (use for automatically adjusting for noise)
  void SetDarkRate(double idarkrate){ PMTDarkRate = idarkrate; }

  ///DEPRECATED function used in old class (WCSimWCDigitizer), and called in WCSimEventAction
  virtual void SetPMTSize(G4float /*inputSize*/) {};

protected:

  ///This should call the trigger algorithms, and handle any temporary DigitsCollection's
  virtual void DoTheWork(WCSimWCDigitsCollection* WCDCPMT) = 0;

  //these are the algorithms that perform triggering
  //they are stored here so that different trigger classes can use the same algorithms without copying code
  /**
   * \brief An NHits trigger algorithm
   *
   * Looks through the input WCSimWCDigitsCollection and integrates the number of hits in a (specified) time window
   * If the integral passes above a (specified) threshold, a trigger is issued
   *
   * The trigger type is kTriggerNHits
   *
   * The trigger time is the time of the first digit above threshold
   *
   * The trigger information is the number of hits in the time window (i.e. the number of hits that caused the trigger to fire)
   *
   * Currently setup with the optional 'test' argument which runs the algorithm with half the hit threshold
   * for testing purposes. Triggers issued in this mode have type kTriggerNHitsTest
   */
  void AlgNHits(WCSimWCDigitsCollection* WCDCPMT, bool remove_hits, bool test=false);

  WCSimWCTriggeredDigitsCollection*   DigitsCollection; ///< The main output of the class - collection of digits in the trigger window
  std::map<int,int>          DigiHitMap; ///< Keeps track of the PMTs that have been added to the output WCSimWCTriggeredDigitsCollection

  std::vector<Float_t>                TriggerTimes; ///< The times of the triggers
  std::vector<TriggerType_t>          TriggerTypes; ///< The type of the triggers
  std::vector< std::vector<Float_t> > TriggerInfos; ///< Additional information associated with each trigger

  WCSimWCDAQMessenger*       DAQMessenger; ///< Get the options from the .mac file
  WCSimDetectorConstruction* myDetector;   ///< Know about the detector, so can add appropriate PMT time smearing

  /// Clear the Trigger* vectors and DigiHitMap
  void ReInitialize() {
    TriggerTimes.clear(); 
    TriggerTypes.clear(); 
    TriggerInfos.clear(); 
    DigiHitMap.clear();
  }

  double PMTDarkRate;    ///< Dark noise rate of the PMTs

  // Trigger algorithm options
  //NHits
  G4int  nhitsThreshold;      ///< The threshold for the NHits trigger
  G4int  nhitsWindow;         ///< The time window for the NHits trigger
  G4bool nhitsAdjustForNoise; ///< Automatically adjust the NHits trigger threshold based on the average dark noise rate?
  //Save failures
  G4int    saveFailuresMode; ///< The mode for saving events which don't pass triggers
  G4double saveFailuresTime; ///< The dummy trigger time for failed events

  G4String triggerClassName; ///< Save the name of the trigger class

private:
  ///modify the NHits threshold based on the average dark noise rate
  void AdjustNHitsThresholdForNoise();

  ///takes all trigger times, then loops over all Digits & fills the output DigitsCollection
  void FillDigitsCollection(WCSimWCDigitsCollection* WCDCPMT, bool remove_hits, TriggerType_t save_triggerType);
  
  static const double offset;        ///< Hit time offset (ns)
  static const double eventgateup;   ///< Digits are saved up to trigger time + eventgateup (ns)
  static const double eventgatedown; ///< Digits are saved starting from trigger time - eventgatedown (ns)
  static const double LongTime;      ///< An arbitrary long time to use in loops (ns)

  bool   digitizeCalled; ///< Has Digitize() been called yet?
};

// *******************************************
// CONTAINER CLASS
// *******************************************

class WCSimWCDigiTrigger : public G4VDigi
{
public:

  WCSimWCDigiTrigger();
  ~WCSimWCDigiTrigger();
  WCSimWCDigiTrigger(const WCSimWCDigiTrigger&);
  const WCSimWCDigiTrigger& operator=(const WCSimWCDigiTrigger&);
  int operator==(const WCSimWCDigiTrigger&) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  void Draw() {}
  void Print();

  inline void SetTubeID(G4int tube) { tubeID = tube; }
  inline void AddGate  (G4int gate, float t)   { Gates.insert(gate); } //TriggerTimes.insert(t);   }
  inline void AddPe    (G4float hitTime)  { totalPe++; } //time_float.insert(hitTime); }
  inline void SetPe    (G4int gate, G4float Q) {   pe.insert(std::pair<int,float>(gate,Q)); }
  inline void SetTime  (G4int gate, G4float T) { time.insert(std::pair<int,float>(gate,T)); }

  /// Add a whole vector for one digit to fDigiComp. Clear input vector once added.
  void AddDigiCompositionInfo(G4int gate, std::vector<int> &digi_comp){
    fDigiComp.insert(std::pair<int, std::vector<int> >(gate, digi_comp));
    digi_comp.clear();
  }

  inline G4int   GetTubeID() {return tubeID;}
  inline std::vector<G4float> GetPe      (int gate) { return FindInMultimap(gate, pe); }
  inline std::vector<G4float> GetTime    (int gate) { return FindInMultimap(gate, time); }
  //inline std::vector<std::pair<int,int> > GetDigiCompositionInfo(int gate) { return FindInMultimap(gate, fDigiComp); }
  std::vector<std::vector<int> > GetDigiCompositionInfo(int gate)
  {
    std::vector<std::vector<int> > v;
    std::multimap<int, std::vector<int> >::iterator it = fDigiComp.begin();
    for (; it != fDigiComp.end(); ++it) {
      if((it->first) == gate)
	v.push_back(it->second);
    }
    return v;
  }

  inline int NumberOfGates()     { return Gates.size();      }
  inline int NumberOfSubEvents() { return Gates.size() - 1;  }
  inline bool HasHitsInGate(int gate) { return Gates.count(gate) > 0; }

private:
  //PMT parameters
  G4int   tubeID;

  //'Gates' is specifies subevent
  std::set<int> Gates;
  //'TriggerTimes' specifies e.g. the subevent trigger time
  //std::vector<float> TriggerTimes;

  //lists (meaning multimap) of information for each digit created on the PMT
  std::multimap<int,float> pe;   ///< Digit charge
  std::multimap<int,float> time; ///< Digit time
  std::multimap<int, std::vector<int> > fDigiComp;   ///< Stores the unique IDs of each photon making up a digit

  //integrated hit/digit parameters
  G4int                 totalPe; ///< Total charge on digit

  template <typename T> std::vector<T> FindInMultimap(const int compare, typename std::multimap<int,T> &map)
  {
    typename std::vector<T> v;
    typename std::multimap<int,T>::iterator it = map.begin();
    for (; it != map.end(); ++it) {
      if((it->first) == compare)
	v.push_back(it->second);
    }
    return v;
  }

};

extern G4Allocator<WCSimWCDigiTrigger> WCSimWCDigiTriggerAllocator;

inline void* WCSimWCDigiTrigger::operator new(size_t)
{
  void* aDigi;
  aDigi = (void*) WCSimWCDigiTriggerAllocator.MallocSingle();
  return aDigi;
}

inline void WCSimWCDigiTrigger::operator delete(void* aDigi)
{
  WCSimWCDigiTriggerAllocator.FreeSingle((WCSimWCDigiTrigger*) aDigi);
}



// *******************************************
// DERIVED CLASSES
// *******************************************


/**
 * \class WCSimWCTriggerNHits
 *
 * \brief A simple NHits trigger class
 *
 */

class WCSimWCTriggerNHits : public WCSimWCTriggerBase
{
public:

  ///Create WCSimWCTriggerNHits instance with knowledge of the detector and DAQ options
  WCSimWCTriggerNHits(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*);

  ~WCSimWCTriggerNHits();
  
private:
  ///Calls the workhorse of this class: AlgNHits
  void DoTheWork(WCSimWCDigitsCollection* WCDCPMT);

};


/**
 * \class WCSimWCTriggerNHits2
 *
 * \brief An (incomplete) example of running two trigger algorithms, one after the other
 *
 */

class WCSimWCTriggerNHits2 : public WCSimWCTriggerBase
{
public:

  //not recommended to override these methods
  WCSimWCTriggerNHits2(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*);
  ~WCSimWCTriggerNHits2();
  
private:
  void DoTheWork(WCSimWCDigitsCollection* WCDCPMT);

};



#endif //WCSimWCTrigger_h
