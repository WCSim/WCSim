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

WCSimWCTriggerNHits::WCSimWCTriggerNHits(G4String name,
				   WCSimDetectorConstruction* myDetector)
  :WCSimWCTriggerBase(name, myDetector)
{
}

WCSimWCTriggerNHits::~WCSimWCTriggerNHits(){
}


void WCSimWCTriggerNHits::DoTheWork(WCSimWCDigitsCollection* WCDCPMT) {
  //Apply an NHits trigger
  bool remove_hits = false;
  AlgNHits(WCDCPMT, remove_hits);
}
