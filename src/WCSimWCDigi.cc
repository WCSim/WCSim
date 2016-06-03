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
}

const WCSimWCDigi& WCSimWCDigi::operator=(const WCSimWCDigi& right)
{
  TriggerTimes = right.TriggerTimes;
  tubeID = right.tubeID; 
  pe     = right.pe;
  time   = right.time;
  return *this;
}


int WCSimWCDigi::operator==(const WCSimWCDigi& right) const
{ 
 return ( (tubeID==right.tubeID) && (pe==right.pe) && (time==right.time) 
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
  std::vector<int> photon_ids;
#ifdef WCSIMWCDIGI_VERBOSE
  G4cout << "WCSimWCDigi::GetDigiCompositionInfo fDigiComp has size " << fDigiComp.size() << G4endl;
#endif
  for(std::vector< std::pair<int,int> >::iterator it = fDigiComp.begin(); it != fDigiComp.end(); ++it) {
    if(gate == (*it).first) {
      photon_ids.push_back((*it).second);
#ifdef WCSIMWCDIGI_VERBOSE
      G4cout << "WCSimWCDigi::GetDigiCompositionInfo found photon with ID " << (*it).second << G4endl;
#endif
    }
    else if((*it).first > gate)
      break;
  }
  return photon_ids;
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
  std::vector<G4float>::iterator it = std::find(time_float.begin(), time_float.end(), gatetime);
  if(it != time_float.end())
    time_float.erase(it);
  else
    G4cerr << "Could not erase time " << gatetime << " from WCSimWCDigi member time_float" << G4endl;
  //digit composition vector (pair of digit_id, photon_id)
  //first remove the entries corresponding to the current gate
  for(std::vector< std::pair<int,int> >::iterator it = fDigiComp.begin(); it != fDigiComp.end(); ) {
    if(gate == (*it).first)
      it = fDigiComp.erase(it);
    else if((*it).first > gate)
      break;
    else
      ++it;
  }
  //now that the entries concerning this gate have been removed, need to reorder the remaining entries
  std::vector< std::pair<int,int> > temp_comp_vec;
  for(std::vector< std::pair<int,int> >::iterator it = fDigiComp.begin(); it != fDigiComp.end(); ++it) {
    if((*it).first > gate)
      temp_comp_vec.push_back(std::make_pair((*it).first - 1, (*it).second));
    else
      temp_comp_vec.push_back(*it);
  }
  temp_comp_vec.swap(fDigiComp);
  //number of entries counter
  totalPe--;
}
