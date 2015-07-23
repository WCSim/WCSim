#include "WCSimWCDigitizerBase.hh"
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

#ifndef WCSIMWCDIGITIZERBASE_VERBOSE
//#define WCSIMWCDIGITIZERBASE_VERBOSE
#endif

// changed from 940 (april 2005) by MF
// 960 is unsuitable

//RawSignalHitCollection *collection = new RawSignalHitCollection;

WCSimWCDigitizerBase::WCSimWCDigitizerBase(G4String name,
					   WCSimDetectorConstruction* inDetector,
					   WCSimWCDAQMessenger* myMessenger)
  :G4VDigitizerModule(name), myDetector(inDetector)
{
  G4String colName = "WCDigitizedStoreCollection";
  collectionName.push_back(colName);
  DigiStoreHitMap.clear();

  //  DarkRateMessenger = new WCSimDarkRateMessenger(this);
  if(myMessenger != NULL) {
    DAQMessenger = myMessenger;
    DAQMessenger->TellMeAboutTheDigitizer(this);
    DAQMessenger->SetDigitizerOptions();
  }
  else {
    G4cerr << "WCSimWCDAQMessenger pointer is NULL when passed to WCSimWCDigitizerBase constructor. Exiting..." 
	   << G4endl;
    exit(-1);
  }
}

WCSimWCDigitizerBase::~WCSimWCDigitizerBase(){
  //DarkRateMessenger = 0;
}

void WCSimWCDigitizerBase::Digitize()
{
  //Temporary Storage of Digitized hits which is passed to the trigger
  DigiStore = new WCSimWCDigitsCollection(collectionName[0],collectionName[0]);
  DigiStoreHitMap.clear();

  //DigitsCollection = new WCSimWCDigitsCollection ("/WCSim/glassFaceWCPMT",collectionName[0]);

  G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();
  
  // Get the PMT collection ID
   G4int WCHCID = DigiMan->GetDigiCollectionID("WCRawPMTSignalCollection");

  // Get the PMT Digits collection
  WCSimWCDigitsCollection* WCHCPMT = 
    (WCSimWCDigitsCollection*)(DigiMan->GetDigiCollection(WCHCID));
  
  if (WCHCPMT) {
    DigitizeHits(WCHCPMT);
  }
  
  StoreDigiCollection(DigiStore);

}

void WCSimWCDigitizerBase::AddNewDigit(int tube, int gate, float digihittime, float peSmeared, std::vector< std::pair<int,int> > digi_comp)
{
  //gate is not a trigger, but just the position of the digit in the array
  //inside the WCSimWCDigi object
#ifdef WCSIMWCDIGITIZERBASE_VERBOSE
  G4cout<<"Adding hit "<<gate<<" in tube "<<tube
	<< " with time " << digihittime << " charge " << peSmeared
	<< " (made of " << digi_comp.size() << " raw hits)";
#endif

  //  if ( digihittime > 0.0 && peSmeared>0.0)
  if (peSmeared > 0.0) {
      if ( DigiStoreHitMap[tube] == 0) {
	WCSimWCDigi* Digi = new WCSimWCDigi();
	Digi->AddParentID(1);
	
	Digi->SetTubeID(tube);
	//	Digi->AddGate(gate,digihittime);
	Digi->SetPe(gate,peSmeared);
	Digi->AddPe(digihittime);
	Digi->SetTime(gate,digihittime);
	Digi->AddDigiCompositionInfo(digi_comp);
	DigiStoreHitMap[tube] = DigiStore->insert(Digi);
#ifdef WCSIMWCDIGITIZERBASE_VERBOSE
	G4cout << " NEW HIT" << G4endl;
#endif
      }
      else {
	//G4cout << "deja vu " << tube << " " << G << "  " << TriggerTimes[G] << " " << digihittime
	//     << "  " <<   peSmeared <<" ";
	//(*DigitsCollection)[GigiStoreHitMap[tube]-1]->AddParentID(parentID);
	//(*DigiStore)[DigiStoreHitMap[tube]-1]->AddGate(gate,digihittime);
	(*DigiStore)[DigiStoreHitMap[tube]-1]->SetPe(gate,peSmeared);
	(*DigiStore)[DigiStoreHitMap[tube]-1]->SetTime(gate,digihittime);
	(*DigiStore)[DigiStoreHitMap[tube]-1]->AddPe(digihittime);
	(*DigiStore)[DigiStoreHitMap[tube]-1]->AddDigiCompositionInfo(digi_comp);
#ifdef WCSIMWCDIGITIZERBASE_VERBOSE
	G4cout << " DEJA VU" << G4endl;
#endif
      }
  }//peSmeared > 0
  //else { G4cout << "DIGIT REJECTED" << G4endl; }
}

void WCSimWCDigitizerBase::SKDigitizerType(G4String type) {
  if(type == "SKI")
    SKDeadTime = false;
  else if(type == "SKIV")
    SKDeadTime = true;
}
