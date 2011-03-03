#include "WCSimLBNEDigitizer.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"

#include "TRandom.h"
#include <algorithm>
#include "G4DigiManager.hh"

#include <cmath>

WCSimLBNEDigitizer::WCSimLBNEDigitizer(G4String name)
  : WCSimAbstractDigitizer(name),
    fNumPmts(0),
    fTimeDist(0),
    fChargeDist(0)
{
  // We can't set fNumPmts because the detector might not be constructed yet
  G4String colName = "WCDigitizedCollection";
  collectionName.push_back(colName);
}

WCSimLBNEDigitizer::~WCSimLBNEDigitizer()
{
}

void WCSimLBNEDigitizer::Digitize()
{
  WCSimWCDigitsCollection *DigitsCollection = new WCSimWCDigitsCollection
    ("/WCSim/glassFaceWCPMT","WCDigitizedCollection"); 
   
  G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();
  
  // Get the Associated Hit collection IDs
  G4int WCHCID = DigiMan->GetHitsCollectionID("glassFaceWCPMT");

  // The Hits collection
  WCSimWCHitsCollection* WCHC = 
    (WCSimWCHitsCollection*)(DigiMan->GetHitsCollection(WCHCID));
    
  // Create a channel for every PMT, hit or not
  std::vector<WCSimDaqChannel> channels(fNumPmts);
  for (unsigned i=0; i < channels.size(); i++)
    channels[i].SetConfig(fChannelConfig);
  
  // Add detected photons to hit PMTs
  for (int ihit=0; ihit < WCHC->entries(); ihit++) {
    WCSimWCHit *hit = (*WCHC)[ihit];
    int tubeID = hit->GetTubeID();
    int totalPE = hit->GetTotalPe();
    
    WCSimDaqChannel &channel = channels[tubeID];
    
    for (int ipe=0; ipe < totalPE; ipe++) {
      // Arrival time + PMT transit jitter + cable propagation
      double time = hit->GetTime(ipe) + PickTime() + fConfig.cableDelay;
      double charge = PickCharge();
      channel.AddPulse(time, charge);
    } // loop over photons in hit
  } // loop over hit channels
  
  std::vector<WCSimLBNEDigitizerEvent> events = SimulateDaq(channels);
  
  // Create WCSimWCDigi objects for each channel hit in any event
  std::map<int, WCSimWCDigi*> digiMap; // lookup digi objects by tubeID
  
  fTriggerTimes.resize(events.size());
  
  for (unsigned iEvent=0; iEvent < events.size(); iEvent++) {
    WCSimLBNEDigitizerEvent &event = events[iEvent];

    fTriggerTimes[iEvent] = event.triggerTime;
    
    // Loop over hits
    std::map<int,WCSimDaqIntegral>::const_iterator iHit = event.hits.begin();
    for (; iHit != event.hits.end(); iHit++) {
      int tubeID = iHit->first;
      const WCSimDaqIntegral &integral = iHit->second;
      
      WCSimWCDigi *digi;
      if (digiMap.count(tubeID) == 0) {
        digi = new WCSimWCDigi();
        digi->SetTubeID(tubeID);
        digiMap[tubeID] = digi;
      } else
        digi = digiMap[tubeID];
        
      digi->AddGate(iEvent, event.triggerTime);
      digi->SetTime(iEvent, integral.time);
      digi->SetPe(iEvent, integral.charge);
    } // loop over hit channels
  } // loop over events
  
  
  // Populate DigitsCollection with all the WCSimWCDigi objects
  std::map<int, WCSimWCDigi*>::iterator iDigi = digiMap.begin();
  for (; iDigi != digiMap.end(); iDigi++)
    DigitsCollection->insert(iDigi->second);
  
  StoreDigiCollection(DigitsCollection);
}

G4double WCSimLBNEDigitizer::GetTriggerTime(int i)
{
  return fTriggerTimes[i];
}

int WCSimLBNEDigitizer::NumberOfGatesInThisEvent()
{
  return fTriggerTimes.size();
}

std::vector<WCSimLBNEDigitizerEvent> WCSimLBNEDigitizer::SimulateDaq(std::vector<WCSimDaqChannel> &channels)
{
  std::vector<WCSimLBNEDigitizerEvent> events;
  
  // Find time range of all hits
  double beginTime = 1e9;
  double endTime = -1e9;
  for (unsigned i=0; i < channels.size(); i++) {
    if (channels[i].GetPulseCount() > 0) {
      beginTime = fmin(beginTime, channels[i].GetFirstPulseTime());
      endTime = fmax(endTime, channels[i].GetLastPulseTime());
    }
  }
  
  // Add dark hits over full time range plus some padding
  beginTime -= fConfig.pmtDarkHitsTimeBefore;
  endTime += fConfig.pmtDarkHitsTimeAfter;
  for (unsigned i=0; i < channels.size(); i++)
    AddDarkHits(channels[i], beginTime, endTime);

  // Simulate channel NHIT pulses and readout of charge integrals
  std::vector<WCSimIntervalList> nhitPulses(channels.size());
  std::vector< std::vector<WCSimDaqIntegral> > channelIntegrals(channels.size());
  for (unsigned i=0; i < channels.size(); i++)
    channels[i].Simulate(nhitPulses[i], channelIntegrals[i]);

  // Find triggers
  std::vector<double> triggerTimes = GetNhitTriggerTimes(nhitPulses);
  
  // Create an event for each trigger
  for (unsigned iTrigger=0; iTrigger < triggerTimes.size(); iTrigger++) {
    double triggerTime = triggerTimes[iTrigger];
    // gateBeginDelta negative implies gate before trigger time
    double gateBegin = triggerTime + fConfig.gateBeginDelta;
    double gateEnd = triggerTime + fConfig.gateEndDelta;
    
    WCSimLBNEDigitizerEvent event;
    event.triggerTime = triggerTime;
    event.gateBegin = gateBegin;
    
    // Gather hit times and charge integrals for each channel
    for(unsigned iChannel=0; iChannel < channels.size(); iChannel++) {
      WCSimDaqIntegral summedIntegral;
      bool hit = GetIntegral(gateBegin, gateEnd, channelIntegrals[iChannel], summedIntegral);
      
      if (hit) {
        //summedIntegral.time -= gateBegin; // using relative times in hits
        event.hits[iChannel] = summedIntegral;
      }
    } // loop over channels
    
    events.push_back(event);
  } // loop over triggers
  
  return events;
}


void WCSimLBNEDigitizer::SetConfig(const WCSimLBNEDigitizerConfig &digiConfig)
{
  fConfig = digiConfig;
}

void WCSimLBNEDigitizer::SetChannelConfig(const WCSimDaqChannelConfig &channelConfig)
{
  fChannelConfig = channelConfig;
}

void WCSimLBNEDigitizer::SetTimeDistribution(TH1D *timeDist)
{
  fTimeDist = timeDist;
}

void WCSimLBNEDigitizer::SetChargeDistribution(TH1D *chargeDist)
{
  fChargeDist = chargeDist;
}


double WCSimLBNEDigitizer::PickCharge()
{
  return fChargeDist->GetRandom();
}
  

double WCSimLBNEDigitizer::PickTime()
{
  return fTimeDist->GetRandom();
}

void WCSimLBNEDigitizer::AddDarkHits(WCSimDaqChannel &channel, double windowBegin, double windowEnd)
{
  
  double meanDarkHitsInWindow = (windowEnd - windowBegin) * 1e-9/*s/ns*/ * fConfig.pmtDarkRate;
  int darkHits = gRandom->Poisson(meanDarkHitsInWindow);
  
  // uniformly distribute dark hits over window
  for (int i=0; i < darkHits; i++) {
    double darkTime = gRandom->Uniform(windowBegin, windowEnd);
    double darkCharge = PickCharge();
    channel.AddPulse(darkTime, darkCharge);
  }
}

std::vector<double> WCSimLBNEDigitizer::GetNhitTriggerTimes(std::vector<WCSimIntervalList> &nhitPulses)
{
  // Implementation note: Triggers can only be issued at the rising edge of NHIT pulses
  // so we will only check the NHIT condition at those times.
  
  std::vector<double> triggerTimes;
  
  // Collect list of rising edges from all channels
  std::vector<double> nhitBeginTimes;
  for (unsigned i=0; i < nhitPulses.size(); i++) {
    std::vector<double> channelNhitBeginTimes = nhitPulses[i].GetBeginPoints();
    nhitBeginTimes.insert(nhitBeginTimes.end(), channelNhitBeginTimes.begin(), channelNhitBeginTimes.end()); 
  }
  std::sort(nhitBeginTimes.begin(), nhitBeginTimes.end());
  
  // Loop over rising edges and test NHIT condition, while respecting lockout for duration of gate
  double lockoutUntil = -1e9;
  for (unsigned iEdge=0; iEdge < nhitBeginTimes.size(); iEdge++) {
    // Jump to this time in event
    double time = nhitBeginTimes[iEdge];
    
    // If we are in lockout period, keep going to next time
    if (time < lockoutUntil)
      continue;
    
    // Count coincident channels
    int nhit = 0;
    for (unsigned iChannel=0; iChannel < nhitPulses.size(); iChannel++) {
      if (nhitPulses[iChannel].Contains(time))
        nhit++;
    }
    
    // If trigger condition met, generate trigger and lockout
    if (nhit >= fConfig.nhitThreshold) {
      triggerTimes.push_back(time);
      // disallow overlapping events
      lockoutUntil = time + (fConfig.gateEndDelta - fConfig.gateBeginDelta);
    }
  } // loop over rising edges
  
  return triggerTimes;
}

bool WCSimLBNEDigitizer::GetIntegral(double gateBegin, double gateEnd, 
                     const std::vector<WCSimDaqIntegral> &integrals, 
                     WCSimDaqIntegral &summedIntegral)
{
  bool hit = false;

  for (unsigned i=0; i < integrals.size(); i++) {
    if (integrals[i].time >= gateBegin && integrals[i].time <= gateEnd) {
      if (hit) // already recorded a hit
        summedIntegral.charge += integrals[i].charge;
      else { // first hit so far
        hit = true;
        summedIntegral.time = integrals[i].time;
        summedIntegral.charge = integrals[i].charge;
      }
    }
  } // loop over channel integrals
  
  return hit;
}
