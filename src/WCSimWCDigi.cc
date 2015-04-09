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
  fUniqueID = -9999;
  fIsNoise = false; // Initialise digit as non-noise
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
