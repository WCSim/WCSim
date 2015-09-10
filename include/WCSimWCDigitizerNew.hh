#ifndef WCSimWCDigitizerNew_h
#define WCSimWCDigitizerNew_h 1

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
#include <utility> //for std::pair


// *******************************************
// BASE CLASS
// *******************************************

class WCSimWCDigitizerBase : public G4VDigitizerModule
{
public:
  
  WCSimWCDigitizerBase(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*);
  virtual ~WCSimWCDigitizerBase();
  
  bool AddNewDigit(int tube, int gate, float digihittime, float peSmeared, std::vector< std::pair<int,int> > digi_comp);
  virtual void DigitizeHits(WCSimWCDigitsCollection* WCHCPMT) = 0;
  void DigitizeGate(WCSimWCDigitsCollection* WCHC,G4int G);
  void Digitize();

  //.mac file option setting methods
  void SetDigitizerDeadTime(int deadtime) { DigitizerDeadTime = deadtime; };

protected:
  void ReInitialize() { DigiStoreHitMap.clear(); }
  
  G4double peSmeared;

  WCSimDetectorConstruction* myDetector;
  WCSimWCDAQMessenger* DAQMessenger;

  WCSimWCDigitsCollection*  DigiStore;
  std::map<int,int> DigiStoreHitMap; // need to check if a hit already exists..

  //.mac file option setting variables (need to be put in the base class)
  int DigitizerDeadTime;

};



// *******************************************
// DERIVED CLASSES
// *******************************************


// SKI-based digitizer class
class WCSimWCDigitizerSKI : public WCSimWCDigitizerBase 
{
public:
  
  WCSimWCDigitizerSKI(G4String name, WCSimDetectorConstruction*, WCSimWCDAQMessenger*);
  ~WCSimWCDigitizerSKI();

  void DigitizeHits(WCSimWCDigitsCollection* WCHCPMT);

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

  static const double pmtgate; // ns  
};


#endif //WCSimWCDigitizerNew_h
