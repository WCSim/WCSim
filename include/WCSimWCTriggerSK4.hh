#ifndef WCSimWCTriggerSK4_h
#define WCSimWCTriggerSK4_h 1

#include "WCSimWCTrigger.hh"

class WCSimWCTriggerNDigitsSK4 : public WCSimWCTriggerBase
{
public:

  ///Create WCSimWCTriggerNDigits instance with knowledge of the detector and DAQ options
  WCSimWCTriggerNDigitsSK4(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*);

  ~WCSimWCTriggerNDigitsSK4();


private:

  //Maybe replace vectors with single object to maintain 
  //connection between entries
  std::vector<G4int> ndigitsThreshold_vec;
  std::vector<G4int> ndigitsPreTriggerWindow_vec;
  std::vector<G4int> ndigitsPostTriggerWindow_vec;
  std::vector<TriggerType_t> ndigitsTriggerType_vec;

  std::vector<float> TriggerStartTimes;
  std::vector<float> TriggerEndTimes;

  ///Calls the workhorse of this class: AlgNDigits
  void DoTheWork(WCSimWCDigitsCollection* WCDCPMT);

  // bool GetDefaultMultiDigitsPerTrigger()    { return false; } ///< SKI saves only earliest digit on a PMT in the trigger window
  // int  GetDefaultNDigitsWindow()            { return 200;   } ///< SK max light travel time ~200 ns
  // int  GetDefaultNDigitsThreshold()         { return 25;    } ///< SK NDigits threshold ~25
  // int  GetDefaultNDigitsPreTriggerWindow()  { return -400;  } ///< SK SLE trigger window ~-400
  // int  GetDefaultNDigitsPostTriggerWindow() { return 950;   } ///< SK SLE trigger window ~+950

  // NDigits options
  ///Set the threshold for the NDigits trigger
  void AddNDigitsThreshold(G4int threshold) { ndigitsThreshold_vec.push_back(threshold); }
  ///Set the pretrigger window for the NDigits trigger (value will be forced negative)
  void AddNDigitsPreTriggerWindow(G4int window)  { ndigitsPreTriggerWindow_vec.push_back(- abs(window)); }
  ///Set the posttrigger window for the NDigits trigger (value will be forced positive)
  void AddNDigitsPostTriggerWindow(G4int window) { ndigitsPostTriggerWindow_vec.push_back(+ abs(window)); }
  ///Set the threshold for the NDigits trigger
  void AddTriggerType( TriggerType_t trigger_type){ ndigitsTriggerType_vec.push_back(trigger_type); }
  
  void ClearTriggers(){
    ndigitsThreshold_vec.clear();
    ndigitsPreTriggerWindow_vec.clear();
    ndigitsPostTriggerWindow_vec.clear();
    ndigitsTriggerType_vec.clear();
  }
  
  void AddNDigitsSubTrigger(G4int threshold, G4int prewindow, G4int postwindow, TriggerType_t trigger_type) { 
    ndigitsThreshold_vec.push_back(+abs(threshold)); 
    ndigitsPreTriggerWindow_vec.push_back(+abs(prewindow)); 
    ndigitsPostTriggerWindow_vec.push_back(+abs(postwindow));
    ndigitsTriggerType_vec.push_back(trigger_type);
  }
  
  
  void AlgNDigits(WCSimWCDigitsCollection* WCDCPMT, bool remove_hits);
  
  void FillDigitsCollection(WCSimWCDigitsCollection* WCDCPMT, bool remove_hits);

  static const double offsetSK4;        ///< Hit time offset (ns)
  static const double LongTimeSK4;      ///< An arbitrary long time to use in loops (ns)
  
};

#endif //WCSimWCTriggerSK4_h
