#ifndef WCSimWCTrigger_h
#define WCSimWCTrigger_h 1

#include "WCSimRootOptions.hh"
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
  WCSimWCTriggerBase(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*, G4String);

  virtual ~WCSimWCTriggerBase();

  ///The main user-callable routine of the class. Gets the input & creates the output WCSimWCTriggeredDigitsCollection's, then calls DoTheWork()
  void Digitize();

  ///Returns the number of trigger gates in the event (i.e. the number of triggers passed)
  int NumberOfGatesInThisEvent() { return fTriggerTimes.size(); }
  ///Get the time of the ith trigger
  Double_t             GetTriggerTime(int i) { return fTriggerTimes[i];}
  ///Get the trigger type of the ith trigger
  TriggerType_t        GetTriggerType(int i) { return fTriggerTypes[i];}
  ///Get the additional trigger information associated with the ith trigger
  std::vector<Float_t> GetTriggerInfo(int i) { return fTriggerInfos[i];}
  ///Get the trigger class name
  G4String GetTriggerClassName(){ return fTriggerClassName; }

  //
  // Trigger algorithm option set methods
  //

  ///Set whether to allow the number of digits per PMT per trigger to go > 1
  void SetMultiDigitsPerTrigger(G4bool allow_multi) { fMultiDigitsPerTrigger = allow_multi; }
  G4bool GetMultiDigitsPerTrigger() { return fMultiDigitsPerTrigger; }

  // NDigits options
  ///Set the threshold for the NDigits trigger
  void SetNDigitsThreshold(G4int threshold) { fNDigitsThreshold = threshold; }
  ///Set the time window for the NDigits trigger
  void SetNDigitsWindow(G4int window) { fNDigitsWindow = window; }
  ///Automatically adjust the NDigits threshold based on the average noise occupancy?
  void SetNDigitsAdjustForNoise    (G4bool adjust)      { fNDigitsAdjustForNoise = adjust; }
  ///Set the pretrigger window for the NDigits trigger (value will be forced negative)
  void SetNDigitsPreTriggerWindow(G4int window)  { fNDigitsPreTriggerWindow  = - abs(window); }
  ///Set the posttrigger window for the NDigits trigger (value will be forced positive)
  void SetNDigitsPostTriggerWindow(G4int window) { fNDigitsPostTriggerWindow = + abs(window); }

  // Save trigger failures options
  ///Set the mode for saving failed triggers (0:save only triggered events, 1:save both triggered events & failed events, 2:save only failed events)
  void SetSaveFailuresMode       (G4int mode )        { fSaveFailuresMode = mode; }
  ///Set the dummy trigger time for the failed triggers
  void SetSaveFailuresTime       (G4double time )     { fSaveFailuresTime = time; }
  ///Set the pretrigger window for the SaveFailures trigger (value will be forced negative)
  void SetSaveFailuresPreTriggerWindow(G4int window)  { fSaveFailuresPreTriggerWindow  = - abs(window); }
  ///Set the posttrigger window for the SaveFailures trigger (value will be forced positive)
  void SetSaveFailuresPostTriggerWindow(G4int window) { fSaveFailuresPostTriggerWindow = + abs(window); }
  
  ///Knowledge of the dark rate (use for automatically adjusting for noise)
  void SetDarkRate(double idarkrate){ fPMTDarkRate = idarkrate; }

  ///Save current values of options
  void SaveOptionsToOutput(WCSimRootOptions * wcopt);
  
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
  double GetPreTriggerWindow(TriggerType_t t);
  ///Get the posttrigger window for a given trigger algorithm
  double GetPostTriggerWindow(TriggerType_t t);

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
  void AlgNoTrigger(WCSimWCDigitsCollection* WCDCPMT, bool remove_hits, bool test=false);


  WCSimWCTriggeredDigitsCollection*   fDigitsCollection; ///< The main output of the class - collection of digits in the trigger window
  std::map<int,int>          fDigiHitMap; ///< Keeps track of the PMTs that have been added to the output WCSimWCTriggeredDigitsCollection

  std::vector<Double_t>               fTriggerTimes; ///< The times of the triggers
  std::vector<TriggerType_t>          fTriggerTypes; ///< The type of the triggers
  std::vector< std::vector<Float_t> > fTriggerInfos; ///< Additional information associated with each trigger

  WCSimWCDAQMessenger*       fDAQMessenger; ///< Get the options from the .mac file
  WCSimDetectorConstruction* fDetector;   ///< Know about the detector, so can add appropriate PMT time smearing
  G4String fDetectorElement;

  /// Clear the Trigger* vectors and fDigiHitMap
  void ReInitialize() {
    fTriggerTimes.clear(); 
    fTriggerTypes.clear(); 
    fTriggerInfos.clear(); 
    fDigiHitMap.clear();
  }

  double fPMTDarkRate;    ///< Dark noise rate of the PMTs

  // Trigger algorithm options
  G4bool fMultiDigitsPerTrigger;    ///< Allow the number of digits per PMT saved in each trigger window to go > 1?
  //NDigits
  G4int  fNDigitsThreshold;         ///< The threshold for the NDigits trigger
  G4int  fNDigitsWindow;            ///< The time window for the NDigits trigger
  G4bool fNDigitsAdjustForNoise;    ///< Automatically adjust the NDigits trigger threshold based on the average dark noise rate?
  G4int  fNDigitsPreTriggerWindow;  ///< The pretrigger window to save before an NDigits trigger
  G4int  fNDigitsPostTriggerWindow; ///< The posttrigger window to save after an NDigits trigger
  //Save failures
  G4int    fSaveFailuresMode;              ///< The mode for saving events which don't pass triggers
  G4double fSaveFailuresTime;              ///< The dummy trigger time for failed events
  G4int    fSaveFailuresPreTriggerWindow;  ///< The pretrigger window to save before an SaveFailures trigger
  G4int    fSaveFailuresPostTriggerWindow; ///< The posttrigger window to save after an SaveFailures trigger

  G4String fTriggerClassName; ///< Save the name of the trigger class

private:
  ///modify the NDigits threshold based on the average dark noise rate
  void AdjustNDigitsThresholdForNoise();

  ///takes all trigger times, then loops over all Digits & fills the output DigitsCollection
  void FillDigitsCollection(WCSimWCDigitsCollection* WCDCPMT, bool remove_hits, TriggerType_t save_triggerType);

  ///sort the Trigger vectors (Time, Type, Info) by Trigger Time
  void SortTriggersByTime() {
    int i, j;
    TriggerType_t index_type;
    double index_time;
    std::vector<float> index_info;
    for (i = 1; i < (int) fTriggerTimes.size(); ++i) {
      index_time = fTriggerTimes[i];
      index_type = fTriggerTypes[i];
      index_info = fTriggerInfos[i];
      for (j = i; j > 0 && fTriggerTimes[j-1] > index_time; j--) {
	fTriggerTimes[j] = fTriggerTimes[j-1];
	fTriggerTypes[j] = fTriggerTypes[j-1];
	fTriggerInfos[j] = fTriggerInfos[j-1];
      }//j
      fTriggerTimes[j] = index_time;
      fTriggerTypes[j] = index_type;
      fTriggerInfos[j] = index_info;
    }//i
  }
  
  static const double fOffset;        ///< Hit time offset (ns)
  static const double fLongTime;      ///< An arbitrary long time to use in loops (ns)

  bool   fDigitizeCalled; ///< Has Digitize() been called yet?
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

  inline void SetTubeID(G4int tube) { fTubeID = tube; }
  inline void AddGate  (G4int gate) { fGates.insert(gate); }
  inline void AddPe    ()           { fTotalPe++; }
  inline void SetPe    (G4int gate, G4double Q) {   fPe.insert(std::pair<int,double>(gate,Q)); }
  inline void SetTime  (G4int gate, G4double T) { fTime.insert(std::pair<int,double>(gate,T)); }

  /// Add a whole vector for one digit to fDigiComp. Clear input vector once added.
  void AddDigiCompositionInfo(G4int gate, std::vector<int> &digi_comp){
    fDigiComp.insert(std::pair<int, std::vector<int> >(gate, digi_comp));
    digi_comp.clear();
  }

  inline G4int   GetTubeID() {return fTubeID;}
  inline std::vector<G4double> GetPe      (int gate) { return FindInMultimap(gate, fPe); }
  inline std::vector<G4double> GetTime    (int gate) { return FindInMultimap(gate, fTime); }
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

  inline int NumberOfGates()     { return fGates.size();      }
  inline int NumberOfSubEvents() { return fGates.size() - 1;  }
  inline bool HasHitsInGate(int gate) { return fGates.count(gate) > 0; }

private:
  G4int fTubeID; ///< PMT id of the digit

  std::set<int> fGates;  ///< 'Gates' specifies subevent

  //lists (meaning multimap) of information for each digit created on the PMT
  std::multimap<int,double> fPe;   ///< Digit charge
  std::multimap<int,double> fTime; ///< Digit time
  std::multimap<int, std::vector<int> > fDigiComp;   ///< Stores the unique IDs of each photon making up a digit

  //integrated hit/digit parameters
  G4int                 fTotalPe; ///< Total charge on digit

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
  WCSimWCTriggerNDigits(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*, G4String fDetectorElement);

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

class WCSimWCTriggerNoTrigger : public WCSimWCTriggerBase
{
public:
  
  ///Create WCSimWCTriggerNoTrigger instance with knowledge of the detector and DAQ options
  WCSimWCTriggerNoTrigger(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*,  G4String fDetectorElement);
  
  ~WCSimWCTriggerNoTrigger();
  
private:
  ///Calls the workhorse of this class: AlgNoTrigger
  void DoTheWork(WCSimWCDigitsCollection* WCDCPMT);
  
  bool GetDefaultMultiDigitsPerTrigger()    { return true;  } ///< We want to save everything
  int  GetDefaultNDigitsWindow()            { return 0;     } ///< This is not an NDigits trigger
  int  GetDefaultNDigitsThreshold()         { return 0;     } ///< This is not an NDigits trigger
  int  GetDefaultNDigitsPreTriggerWindow()  { return 0;     } ///< This is not an NDigits trigger
  int  GetDefaultNDigitsPostTriggerWindow() { return 0;     } ///< This is not an NDigits trigger
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
  WCSimWCTriggerNDigits2(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*, G4String fDetectorElement);
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
