#include "WCSimWCTriggerNHits2.hh"
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

#ifndef WCSIMWCTRIGGERBASE_VERBOSE
#define WCSIMWCTRIGGERBASE_VERBOSE
#endif

WCSimWCTriggerNHits2::WCSimWCTriggerNHits2(G4String name,
					 WCSimDetectorConstruction* myDetector,
					 WCSimWCDAQMessenger* myMessenger)
  :WCSimWCTriggerBase(name, myDetector, myMessenger)
{
  triggerClassName = "NHits2";
}

WCSimWCTriggerNHits2::~WCSimWCTriggerNHits2(){
}


void WCSimWCTriggerNHits2::DoTheWork(WCSimWCDigitsCollection* WCDCPMT) {
  //This calls 2 trigger algorithms; the second algorithm is called on hits that failed the first algorithm
  //(for a second trigger working on hits that passed a pretrigger, FillDigitsCollection() needs to have a new option)

  //Make a copy of the input DigitsCollection, so we can remove hits from the copy
  WCSimWCDigitsCollection* WCDCPMTCopy = new WCSimWCDigitsCollection(*WCDCPMT);
  
  //Apply an NHits trigger
  bool remove_hits = true;
  AlgNHits(WCDCPMTCopy, remove_hits);

  //Apply an NHits trigger with a lower threshold & different saved trigger type
  remove_hits = false;
  bool nhits_test = true;
  AlgNHits(WCDCPMTCopy, remove_hits, nhits_test);
}
