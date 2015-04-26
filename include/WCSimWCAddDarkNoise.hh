#ifndef WCSimWCAddDarkNoise_h
#define WCSimWCAddDarkNoise_h 1

#include "WCSimDarkRateMessenger.hh"
#include "WCSimDetectorConstruction.hh"
#include "G4VDigitizerModule.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"
#include "globals.hh"
#include "Randomize.hh"
#include <map>
#include <vector>
#include <utility>

class WCSimWCAddDarkNoise : public G4VDigitizerModule
{
public:
  
  WCSimWCAddDarkNoise(G4String name, WCSimDetectorConstruction*);
  ~WCSimWCAddDarkNoise();
  
  void ReInitialize() { DigiHitMap.clear(); ranges.clear(); result.clear();}

  
public:
  void AddDarkNoise();
  void AddDarkNoiseBeforeDigi(WCSimWCDigitsCollection* WCHCPMT, float num1 ,float num2);
  void FindDarkNoiseRanges(WCSimWCDigitsCollection* WCHCPMT, float width);
  //As it inherits from G4VDigitizerModule it needs a digitize class.  Not used
  void Digitize() { }
  void SetDarkRate(double idarkrate){ PMTDarkRate = idarkrate; }
  void SetConversion(double iconvrate){ ConvRate = iconvrate; }
  void SetDarkMode(int imode){DarkMode = imode;}
  void SetDarkHigh(int idarkhigh){DarkHigh = idarkhigh;}
  void SetDarkLow(int idarklow){DarkLow = idarklow;}
  void SetDarkWindow(int idarkwindow){DarkWindow = idarkwindow;}
private:
  WCSimDarkRateMessenger *DarkRateMessenger;
  double PMTDarkRate; // kHz
  double ConvRate; // kHz
  double DarkHigh; //ns
  double DarkLow; //ns
  double DarkWindow; //ns
  int DarkMode;

  WCSimDetectorConstruction* myDetector;

protected:
  WCSimWCDigitsCollection*  DigitsCollection;
  std::map<int,int> DigiHitMap; // need to check if a hit already exists..
  
  std::vector<std::pair<float, float> > ranges;
  std::vector<std::pair<float, float> > result;
  
};

#endif








