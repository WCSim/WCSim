#ifndef WCSimWCDigitizerBase_h
#define WCSimWCDigitizerBase_h 1

#include "WCSimDarkRateMessenger.hh"
#include "WCSimWCDAQMessenger.hh"
#include "WCSimDetectorConstruction.hh"
#include "G4VDigitizerModule.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"
#include "globals.hh"
#include "Randomize.hh"
#include <map>
#include <vector>


class WCSimWCDigitizerBase : public G4VDigitizerModule
{
public:
  
  WCSimWCDigitizerBase(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*);
  ~WCSimWCDigitizerBase();
  
  void ReInitialize() { DigiStoreHitMap.clear(); }
  
  void AddNewDigit(int tube, int gate, float digihittime, float peSmeared);
  virtual void DigitizeHits(WCSimWCDigitsCollection* WCHCPMT);
  void DigitizeGate(WCSimWCDigitsCollection* WCHC,G4int G);
  void Digitize();

  //.mac file option setting methods
  void SKDigitizerType(G4String); 

  static G4double GetLongTime() { return LongTime;}
  static G4double GetEventGateDown() { return eventgatedown;}
  static G4double GetEventGateUp() { return eventgateup;}

private:
  static void Threshold(double& pe,int& iflag){
    //   CLHEP::HepRandom::setTheSeed(pe+2014);
    double x = pe+0.1; iflag=0;
    double thr; double RDUMMY,err;
    if ( x<1.1) {
      thr = std::min(1.0,
		     -0.06374+x*(3.748+x*(-63.23+x*(452.0+x*(-1449.0+x*(2513.0
									+x*(-2529.+x*(1472.0+x*(-452.2+x*(51.34+x*2.370))))))))));
    } else {
      thr = 1.0;
    }
    RDUMMY = G4UniformRand();
    if (thr < RDUMMY) {
      pe = 0.0;
      iflag = 1;
    }
    else {
      err = G4RandGauss::shoot(0.0,0.03);
      /////      call rngaus(0.0, 0.03, err);
      pe = pe+err;
    }
  }

  static const double offset; // hit time offset
  static const double pmtgate; // ns
  static const double eventgateup; // ns
  static const double eventgatedown; // ns
  static const double calibdarknoise; // ns
  static const double LongTime; // ns

protected:
  G4double peSmeared;

  WCSimDetectorConstruction* myDetector;
  WCSimWCDAQMessenger* DAQMessenger;

  WCSimWCDigitsCollection*  DigiStore;
  std::map<int,int> DigiStoreHitMap; // need to check if a hit already exists..

  bool SKDeadTime;

};

#endif








