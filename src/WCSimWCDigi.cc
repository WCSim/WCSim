#include "WCSimWCDigi.hh"
#include "G4RotationMatrix.hh"


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
  for (int i = 0 ; i < pe.size() ; i++) {
    G4cout  << "Gate = " << i 
	    << " PE: "    << pe[i]
	    << " Time:"   << time[i] << G4endl;
  }
}

std::vector<int> WCSimWCDigi::GetDigiCompositionInfo(int gate)
{
  std::vector<int> photon_ids;
  for(std::vector< std::pair<int,int> >::iterator it = fDigiComp.begin(); it != fDigiComp.end(); ++it) {
    if(gate == (*it).first)
      photon_ids.push_back((*it).second);
    else if((*it).first > gate)
	break;
    }
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
  std::vector<int>::iterator it = std::find(time_float.begin(), time_float.end(), time);
  if(it != time_float.end())
    time_float.erase(it);
  else
    G4cout << "Could not erase time " << time << " from WCSimWCDigi member time_float" << G4endl;
  //digit composition vector
  for(std::vector< std::pair<int,int> >::iterator it = fDigiComp.begin(); it != fDigiComp.end(); ++it) {
    if(gate == (*it).first)
      fDigiComp.erase(it);
    else if((*it).first > gate)
	break;
  }
  //number of entries counter
  totalPe--;
}
