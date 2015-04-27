#include "WCSimWCTriggerBase.hh"
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

const double WCSimWCTriggerBase::calibdarknoise = 1.37676;

const double WCSimWCTriggerBase::offset = 950.0 ; // ns
const double WCSimWCTriggerBase::pmtgate = 200.0 ; // ns
const double WCSimWCTriggerBase::eventgateup = 950.0 ; // ns
const double WCSimWCTriggerBase::eventgatedown = -400.0 ; // ns
const double WCSimWCTriggerBase::LongTime = 100000.0 ; // ns
// value in skdetsim
const int WCSimWCTriggerBase::GlobalThreshold = 22 ; // # hit PMTs
//const int WCSimWCTriggerBase::GlobalThreshold = 12 ; // # hit PMTs
// try to trigger early to reduce the width.
//const int WCSimWCTriggerBase::GlobalThreshold = 10 ; // # hit PMTs


WCSimWCTriggerBase::WCSimWCTriggerBase(G4String name,
				   WCSimDetectorConstruction* myDetector)
  :G4VDigitizerModule(name)
{
  G4String colName = "WCDigitizedCollection";
  this->myDetector = myDetector;
  collectionName.push_back(colName);
  DigiStoreHitMap.clear();
}

WCSimWCTriggerBase::~WCSimWCTriggerBase(){
}

void WCSimWCTriggerBase::Digitize()
{
  //Input is collection of all digitized hits that passed the threshold
  //Output is all digitized hits which pass the trigger
  
  DigiStore = new WCSimWCDigitsCollection(collectionName[0],collectionName[0]);
  DigiStoreHitMap.clear();

  //This is the output digit collection
  DigitsCollection = new WCSimWCDigitsCollection ("/WCSim/glassFaceWCPMT",collectionName[0]);

  G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();

  /*
  // Get the PMT collection ID
  G4int WCHCID = DigiMan->GetDigiCollectionID("WCRawPMTSignalCollection");
  // Get the PMT Digits collection
  WCSimWCDigitsCollection* WCHCPMT = 
    (WCSimWCDigitsCollection*)(DigiMan->GetDigiCollection(WCHCID));
  */
  
  // Get the Digitized hits collection ID
  G4int WCDCID = DigiMan->GetDigiCollectionID("WCDigitizedStoreCollection");
  // Get the PMT Digits Collection
  WCSimWCDigitsCollection* WCDCPMT = 
    (WCSimWCDigitsCollection*)(DigiMan->GetDigiCollection(WCDCID));

  // Do the work  
  if (WCDCPMT) {
    DoTheWork(WCDCPMT);
  }
  
  StoreDigiCollection(DigiStore);

}

void WCSimWCTriggerBase::ApplyTrigger(WCSimWCDigitsCollection* WCDCPMT) {
  // In the base class this does nothing
}

void WCSimWCTriggerBase::DoTheWork(WCSimWCDigitsCollection* WCDCPMT) {
  ApplyTrigger(WCDCPMT);
}
