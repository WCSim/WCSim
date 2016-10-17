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

  ///The main user-callable routine of the class. Gets the input & creates the output WCSimWCTriggeredDigitsCollection's, then calls DoTheWork()
  void Digitize();

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

  ///Set whether to allow the number of digits per PMT per trigger to go > 1
  void SetMultiDigitsPerTrigger(G4bool allow_multi) { multiDigitsPerTrigger = allow_multi; }

  // NDigits options
  ///Set the threshold for the NDigits trigger
  void SetNDigitsThreshold(G4int threshold) { ndigitsThreshold = threshold; }
  ///Set the time window for the NDigits trigger
  void SetNDigitsWindow(G4int window) { ndigitsWindow = window; }
  ///Automatically adjust the NDigits threshold based on the average noise occupancy?
  void SetNDigitsAdjustForNoise    (G4bool adjust)      { ndigitsAdjustForNoise = adjust; }
  ///Set the pretrigger window for the NDigits trigger (value will be forced negative)
  void SetNDigitsPreTriggerWindow(G4int window)  { ndigitsPreTriggerWindow  = - abs(window); }
  ///Set the posttrigger window for the NDigits trigger (value will be forced positive)
  void SetNDigitsPostTriggerWindow(G4int window) { ndigitsPostTriggerWindow = + abs(window); }

  // Save trigger failures options
  ///Set the mode for saving failed triggers (0:save only triggered events, 1:save both triggered events & failed events, 2:save only failed events)
  void SetSaveFailuresMode       (G4int mode )        { saveFailuresMode = mode; }
  ///Set the dummy trigger time for the failed triggers
  void SetSaveFailuresTime       (G4double time )     { saveFailuresTime = time; }
  ///Set the pretrigger window for the SaveFailures trigger (value will be forced negative)
  void SetSaveFailuresPreTriggerWindow(G4int window)  { saveFailuresPreTriggerWindow  = - abs(window); }
  ///Set the posttrigger window for the SaveFailures trigger (value will be forced positive)
  void SetSaveFailuresPostTriggerWindow(G4int window) { saveFailuresPostTriggerWindow = + abs(window); }
  
  ///Knowledge of the dark rate (use for automatically adjusting for noise)
  void SetDarkRate(double idarkrate){ PMTDarkRate = idarkrate; }


protected:

  ///This should call the trigger algorithms, and handle any temporary DigitsCollection's
  virtual void DoTheWork(WCSimWCDigitsCollection* WCDCPMT) = 0;

  /// Get the default threshold, etc. from the derived class, and override with read from the .mac file
  void GetVariables();

  ///Set the default trigger class specific decision of whether to save multiple digits per PMT per trigger (overridden by .mac)
  virtual bool GetDefaultMultiDigitsPerTrigger()   { return true; }
  ///Set the default trigger class specific NDigits window (in ns) (overridden by .mac)
  virtual int GetDefaultNDigitsWindow()            { return 200; }
  ///Set the default trigger class specific NDigits threshold (in ns) (overridden by .mac)
  virtual int GetDefaultNDigitsThreshold()         { return 25; }
  ///Set the default trigger class specific NDigits pretrigger window (in ns) (overridden by .mac)
  virtual int GetDefaultNDigitsPreTriggerWindow()  { return -400; }
  ///Set the default trigger class specific NDigits posttrigger window (in ns) (overridden by .mac)
  virtual int GetDefaultNDigitsPostTriggerWindow() { return 950; }

  ///Get the pretrigger window for a given trigger algorithm
  int GetPreTriggerWindow(TriggerType_t t);
  ///Get the posttrigger window for a given trigger algorithm
  int GetPostTriggerWindow(TriggerType_t t);

  //these are the algorithms that perform triggering
  //they are stored here so that different trigger classes can use the same algorithms without copying code
  /**
   * \brief An NDigits trigger algorithm
   *
   * Looks through the input WCSimWCDigitsCollection and integrates the number of hits in a (specified) time window
   * If the integral passes above a (specified) threshold, a trigger is issued
   *
   * The trigger type is kTriggerNDigits
   *
   * The trigger time is the time of the first digit above threshold
   *
   * The trigger information is the number of hits in the time window (i.e. the number of hits that caused the trigger to fire)
   *
   * Currently setup with the optional 'test' argument which runs the algorithm with half the hit threshold
   * for testing purposes. Triggers issued in this mode have type kTriggerNDigitsTest
   */
  void AlgNDigits(WCSimWCDigitsCollection* WCDCPMT, bool remove_hits, bool test=false);

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
  G4bool multiDigitsPerTrigger;    ///< Allow the number of digits per PMT saved in each trigger window to go > 1?
  //NDigits
  G4int  ndigitsThreshold;         ///< The threshold for the NDigits trigger
  G4int  ndigitsWindow;            ///< The time window for the NDigits trigger
  G4bool ndigitsAdjustForNoise;    ///< Automatically adjust the NDigits trigger threshold based on the average dark noise rate?
  G4int  ndigitsPreTriggerWindow;  ///< The pretrigger window to save before an NDigits trigger
  G4int  ndigitsPostTriggerWindow; ///< The posttrigger window to save after an NDigits trigger
  //Save failures
  G4int    saveFailuresMode;              ///< The mode for saving events which don't pass triggers
  G4double saveFailuresTime;              ///< The dummy trigger time for failed events
  G4int    saveFailuresPreTriggerWindow;  ///< The pretrigger window to save before an SaveFailures trigger
  G4int    saveFailuresPostTriggerWindow; ///< The posttrigger window to save after an SaveFailures trigger

  G4String triggerClassName; ///< Save the name of the trigger class

private:
  ///modify the NDigits threshold based on the average dark noise rate
  void AdjustNDigitsThresholdForNoise();

  ///takes all trigger times, then loops over all Digits & fills the output DigitsCollection
  void FillDigitsCollection(WCSimWCDigitsCollection* WCDCPMT, bool remove_hits, TriggerType_t save_triggerType);

  ///sort the Trigger vectors (Time, Type, Info) by Trigger Time
  void SortTriggersByTime() {
    int i, j;
    TriggerType_t index_type;
    float index_time;
    std::vector<float> index_info;
    for (i = 1; i < (int) TriggerTimes.size(); ++i) {
      index_time = TriggerTimes[i];
      index_type = TriggerTypes[i];
      index_info = TriggerInfos[i];
      for (j = i; j > 0 && TriggerTimes[j-1] > index_time; j--) {
	TriggerTimes[j] = TriggerTimes[j-1];
	TriggerTypes[j] = TriggerTypes[j-1];
	TriggerInfos[j] = TriggerInfos[j-1];
      }//j
      TriggerTimes[j] = index_time;
      TriggerTypes[j] = index_type;
      TriggerInfos[j] = index_info;
    }//i
  }
  
  static const double offset;        ///< Hit time offset (ns)
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
  inline void AddGate  (G4int gate) { Gates.insert(gate); }
  inline void AddPe    ()           { totalPe++; }
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
  G4int tubeID; ///< PMT id of the digit

  std::set<int> Gates;  ///< 'Gates' specifies subevent

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
 * \class WCSimWCTriggerNDigits
 *
 * \brief A simple NDigits trigger class
 *
 */

class WCSimWCTriggerNDigits : public WCSimWCTriggerBase
{
public:

  ///Create WCSimWCTriggerNDigits instance with knowledge of the detector and DAQ options
  WCSimWCTriggerNDigits(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*);

  ~WCSimWCTriggerNDigits();
  
private:
  ///Calls the workhorse of this class: AlgNDigits
  void DoTheWork(WCSimWCDigitsCollection* WCDCPMT);

  bool GetDefaultMultiDigitsPerTrigger()    { return false; } ///< SKI saves only earliest digit on a PMT in the trigger window
  int  GetDefaultNDigitsWindow()            { return 200;   } ///< SK max light travel time ~200 ns
  int  GetDefaultNDigitsThreshold()         { return 25;    } ///< SK NDigits threshold ~25
  int  GetDefaultNDigitsPreTriggerWindow()  { return -400;  } ///< SK SLE trigger window ~-400
  int  GetDefaultNDigitsPostTriggerWindow() { return 950;   } ///< SK SLE trigger window ~+950
};


/**
 * \class WCSimWCTriggerNDigits2
 *
 * \brief An (incomplete) example of running two trigger algorithms, one after the other
 *
 */

class WCSimWCTriggerNDigits2 : public WCSimWCTriggerBase
{
public:

  //not recommended to override these methods
  WCSimWCTriggerNDigits2(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*);
  ~WCSimWCTriggerNDigits2();
  
private:
  void DoTheWork(WCSimWCDigitsCollection* WCDCPMT);

  bool GetDefaultMultiDigitsPerTrigger()    { return false; } ///< SKI saves only earliest digit on a PMT in the trigger window
  int  GetDefaultNDigitsWindow()            { return 200;   } ///< SK max light travel time ~200 ns
  int  GetDefaultNDigitsThreshold()         { return 50;    } ///< 2 * SK NDigits threshold ~25
  int  GetDefaultNDigitsPreTriggerWindow()  { return -400;  } ///< SK SLE trigger window ~-400
  int  GetDefaultNDigitsPostTriggerWindow() { return 950;   } ///< SK SLE trigger window ~+950
};



#endif //WCSimWCTrigger_h
