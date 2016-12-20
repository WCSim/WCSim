#ifndef WCSim_RootOptions
#define WCSim_RootOptions

//////////////////////////////////////////////////////////////////////////
//                                                                      
// WCSim_RootOptions                                                      
//                                                                      
// This class contains run option information
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"

class TDirectory;

//////////////////////////////////////////////////////////////////////////

class WCSimRootOptions : public TObject {

public:

  WCSimRootOptions();
  virtual ~WCSimRootOptions();
  void Print();

  //WCSimWCAddDarkNoise sets
  void SetPMTDarkRate(double iPMTDarkRate) {PMTDarkRate = iPMTDarkRate;}
  void SetConvRate(double iConvRate) {ConvRate = iConvRate;}
  void SetDarkHigh(double iDarkHigh) {DarkHigh = iDarkHigh;}
  void SetDarkLow(double iDarkLow) {DarkLow = iDarkLow;}
  void SetDarkWindow(double iDarkWindow) {DarkWindow = iDarkWindow;}
  void SetDarkMode(int iDarkMode) {DarkMode = iDarkMode;}
  //WCSimWCAddDarkNoise gets
  double GetPMTDarkRate() {return PMTDarkRate;}
  double GetConvRate() {return ConvRate;}
  double GetDarkHigh() {return DarkHigh;}
  double GetDarkLow() {return DarkLow;}
  double GetDarkWindow() {return DarkWindow;}
  int    GetDarkMode() {return DarkMode;}
  
private:
  //WCSimWCAddDarkNoise
  double PMTDarkRate; // kHz
  double ConvRate; // kHz
  double DarkHigh; //ns
  double DarkLow; //ns
  double DarkWindow; //ns
  int    DarkMode;
  
  ClassDef(WCSimRootOptions,1)  //WCSimRootEvent structure
};


#endif
