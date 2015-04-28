#include "WCSimWCTriggerNHits.hh"
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
#include <iostream>

// changed from 940 (april 2005) by MF
// 960 is unsuitable

//RawSignalHitCollection *collection = new RawSignalHitCollection;

const double WCSimWCTriggerNHits::calibdarknoise = 1.37676;

const double WCSimWCTriggerNHits::offset = 950.0 ; // ns
const double WCSimWCTriggerNHits::pmtgate = 200.0 ; // ns
const double WCSimWCTriggerNHits::eventgateup = 950.0 ; // ns
const double WCSimWCTriggerNHits::eventgatedown = -400.0 ; // ns
const double WCSimWCTriggerNHits::LongTime = 100000.0 ; // ns
// value in skdetsim
const int WCSimWCTriggerNHits::GlobalThreshold = 22 ; // # hit PMTs
//const int WCSimWCTriggerNHits::GlobalThreshold = 12 ; // # hit PMTs
// try to trigger early to reduce the width.
//const int WCSimWCTriggerNHits::GlobalThreshold = 10 ; // # hit PMTs


WCSimWCTriggerNHits::WCSimWCTriggerNHits(G4String name,
				   WCSimDetectorConstruction* myDetector)
  :G4VDigitizerModule(name)
{
  Initialize();
}

WCSimWCTriggerNHits::~WCSimWCTriggerNHits(){
  Terminate();
}


void WCSimWCTriggerNHits::ApplyTrigger(WCSimWCDigitsCollection* WCDCPMT) {
  //Apply an NHits trigger

  
}
