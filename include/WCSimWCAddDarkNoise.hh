#ifndef WCSimWCAddDarkNoise_h
#define WCSimWCAddDarkNoise_h 1

#include "WCSimDarkRateMessenger.hh"
#include "WCSimDetectorConstruction.hh"
#include "G4VDigitizerModule.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"
#include "WCSimRootOptions.hh"
#include "globals.hh"
#include "Randomize.hh"
#include <map>
#include <vector>
#include <utility>

class WCSimWCAddDarkNoise : public G4VDigitizerModule
{
public:
  
  // WCSimWCAddDarkNoise(G4String name, WCSimDetectorConstruction*);
  WCSimWCAddDarkNoise(G4String name, WCSimDetectorConstruction*, G4String);
  ~WCSimWCAddDarkNoise();
  
public:
  void AddDarkNoise();
  void AddDarkNoiseBeforeDigi(WCSimWCDigitsCollection* WCHCPMT, double num1 ,double num2);
  void FindDarkNoiseRanges(WCSimWCDigitsCollection* WCHCPMT, double width);
  //As it inherits from G4VDigitizerModule it needs a digitize class.  Not used
  void Digitize() { }
  void SetDarkRate(double idarkrate){ fPMTDarkRate = idarkrate; }
  double GetDarkRate() { return fPMTDarkRate; }
  void SetConversion(double iconvrate){ fConvRate = iconvrate; }
  void SetDarkMode(int imode){fDarkMode = imode;}
  void SetDarkHigh(double idarkhigh){fDarkHigh = idarkhigh;}
  void SetDarkLow(double idarklow){fDarkLow = idarklow;}
  void SetDarkWindow(int idarkwindow){fDarkWindow = idarkwindow;}
  int GetDarkWindow(){return (int)(fDarkWindow);}
  void SaveOptionsToOutput(WCSimRootOptions * wcopt, string tag);
  
private:
  void ReInitialize() { franges.clear(); fresult.clear();}
  void SetPMTDarkDefaults();

  WCSimDarkRateMessenger *fDarkRateMessenger;
  double fPMTDarkRate; // kHz
  double fConvRate; // kHz
  double fDarkHigh; //ns
  double fDarkLow; //ns
  double fDarkWindow; //ns
  int fDarkMode;
  bool fCalledAddDarkNoise;

  WCSimDetectorConstruction* fDetector;
  G4String fDetectorElement;

  std::vector<std::pair<double, double> > franges;
  std::vector<std::pair<double, double> > fresult;
  
};

#endif








