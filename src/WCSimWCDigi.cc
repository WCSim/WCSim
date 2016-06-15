#include "WCSimWCDigi.hh"
#include "G4RotationMatrix.hh"

#ifndef WCSIMWCDIGI_VERBOSE
//#define WCSIMWCDIGI_VERBOSE
#endif

G4Allocator<WCSimWCDigi> WCSimWCDigiAllocator;

WCSimWCDigi::WCSimWCDigi()
{
  tubeID = 0; 
  Gates.clear();
  TriggerTimes.clear();
  //  TriggerTimes.reserve(10);
  pe.clear();
  time.clear();
  time_presmear.clear();
  totalPe = 0;
}

WCSimWCDigi::~WCSimWCDigi(){;}




WCSimWCDigi::WCSimWCDigi(const WCSimWCDigi& right)
  :G4VDigi()
{
  // in principle assignment = is defined for containers...
  Gates = right.Gates;
  tubeID = right.tubeID; 
  pe     = right.pe;
  time   = right.time;
  time_presmear = right.time_presmear;
}

const WCSimWCDigi& WCSimWCDigi::operator=(const WCSimWCDigi& right)
{
  TriggerTimes = right.TriggerTimes;
  tubeID = right.tubeID; 
  pe     = right.pe;
  time   = right.time;
  time_presmear = right.time_presmear;
  return *this;
}


int WCSimWCDigi::operator==(const WCSimWCDigi& right) const
{ 
 return ( (tubeID==right.tubeID) && (pe==right.pe) && (time==right.time) 
	  && (time_presmear==right.time_presmear)
	  && (TriggerTimes==right.TriggerTimes) && (tubeID==right.tubeID) ); 
}

void WCSimWCDigi::Draw() {;}

void WCSimWCDigi::Print()
{
  G4cout << "TubeID: " << tubeID 
	 <<"Number of Gates " << NumberOfGates();
  for (unsigned int i = 0 ; i < pe.size() ; i++) {
    G4cout  << "Gate = " << i 
	    << " PE: "    << pe[i]
	    << " Time:"   << time[i] << G4endl;
  }
}

std::vector<int> WCSimWCDigi::GetDigiCompositionInfo(int gate)
{
#ifdef WCSIMWCDIGI_VERBOSE
  G4cout << "WCSimWCDigi::GetDigiCompositionInfo fDigiComp has size " << fDigiComp.size() << G4endl;
  for(int i = 0; i < fDigiComp[gate].size(); i++)
    G4cout << "WCSimWCDigi::GetDigiCompositionInfo found photon with ID " << fDigiComp[gate][i] << G4endl;
#endif
  return fDigiComp[gate];
}

void WCSimWCDigi::RemoveDigitizedGate(G4int gate)
{
  //this removes an element from the maps, vectors, and sets, and counters that were filled by WCSimWCDigitizerBase::AddNewDigit()
  //Gates and TriggerTimes are NOT set

  //pe map
  pe.erase(gate);
  //time map and time_float vector
  float gatetime = time[gate];
  time.erase(gate);
  time_presmear.erase(gate);
  std::vector<G4float>::iterator it = std::find(time_float.begin(), time_float.end(), gatetime);
  if(it != time_float.end())
    time_float.erase(it);
  else
    G4cerr << "Could not erase time " << gatetime << " from WCSimWCDigi member time_float" << G4endl;

  // the following are not necessarily filled, so need to check that they exist before trying to erase them
  //
  //digit composition vector (pair of digit_id, photon_id)
  if(fDigiComp.find(gate) != fDigiComp.end())
    fDigiComp.erase(gate);
  //parent id
  if(primaryParentID.find(gate) != primaryParentID.end())
    primaryParentID.erase(gate);

  //number of entries counter
  totalPe--;
}
