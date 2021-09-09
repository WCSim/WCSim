#ifndef WCSimWCHit_h
#define WCSimWCHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4ios.hh"
// for sort, find, count_if
#include <algorithm>
//for less_equal, bind2nd,...
#include <functional>

class WCSimWCHit : public G4VHit
{
 public:
  
  WCSimWCHit();
  ~WCSimWCHit();
  WCSimWCHit(const WCSimWCHit&);
  const WCSimWCHit& operator=(const WCSimWCHit&);
  G4int operator==(const WCSimWCHit&) const;
  
  inline void* operator new(size_t);
  inline void  operator delete(void*);
  
  void Draw();
  void Print();
  
 public:
  
  void SetTubeID       (G4int tube)                 { tubeID = tube; };
  void SetTrackID      (G4int track)                { trackID = track; };
  void SetEdep         (G4double de)                { edep = de; };
  void SetPos          (G4ThreeVector xyz)          { pos = xyz; };
  void SetRot          (G4RotationMatrix rotMatrix) { rot = rotMatrix; };
  void SetLogicalVolume(G4LogicalVolume* logV)      { pLogV = logV;}
  void AddParentID     (G4int primParentID)
  { primaryParentID.push_back(primParentID); }

  // This is temporarily used for the drawing scale
  void SetMaxPe(G4int number = 0)  {maxPe   = number;};

  void AddPe(G4double hitTime)  
  {
    // First increment the totalPe number
    totalPe++; 

    if (totalPe > maxPe) 
      maxPe = totalPe;

    time.push_back(hitTime);
  }
 
  G4int         GetTubeID()     { return tubeID; };
  G4int         GetTrackID()    { return trackID; };
  G4ThreeVector GetPos()        { return pos; };
  G4int         GetTotalPe()    { return totalPe;};
  G4double       GetTime(int i)  { return time[i];};
  G4int         GetParentID(int i) { return primaryParentID[i];};
  
  G4LogicalVolume* GetLogicalVolume() {return pLogV;};

  void SortHitTimes() {   sort(time.begin(),time.end()); }


  // low is the trigger time, up is trigger+950ns (end of event)
  G4double GetFirstHitTimeInGate(G4double low, G4double upevent)
  {
    G4double firsttime;
    std::vector<G4double>::iterator tfirst = time.begin();
    std::vector<G4double>::iterator tlast = time.end();

	std::vector<G4double>::iterator found =
		std::find_if(tfirst,tlast,
					 [&](const G4double& val){
					   return val >= low && val <= upevent;
					 });
    if ( found != tlast ) {
      firsttime = *found; // first hit time
    }
    else {
      firsttime = -10000.; //error code.
    }
    //G4cout << "firsthit time " << firsttime << "\n";
    return firsttime;
  }


  // pmtgate  and evgate are durations, ie not absolute times

  G4int GetPeInGate(double low, double pmtgate,double evgate) {
    // M Fechner; april 2005
    // assumes that time has already been sorted
    std::vector<G4double>::iterator tfirst = time.begin();
    std::vector<G4double>::iterator tlast = time.end();
    // select min time
    G4double mintime = (pmtgate < evgate) ? pmtgate : evgate;
    
    // return number of hits in the time window...

	G4int number = std::count_if(tfirst,tlast,
								 [&](const G4double& val){
								   return val >= low && val <= mintime;
								 });
    
    totalPeInGate = number;
    //    G4cout << "numer = " <<  number <<"\n";
    return number;
  }

  // G. Pronost:	
  // Sort function by Hit Time (using first time, assuming hit time within a hit object are sorted)
  struct SortFunctor_Hit {
    bool operator() (const WCSimWCHit * const &a,
                     const WCSimWCHit * const &b) const;
  };

 private:
  
  G4int            tubeID;
  G4int            trackID;
  G4double         edep;
  G4ThreeVector    pos;
  G4RotationMatrix rot;
  G4LogicalVolume* pLogV;

  // This is temporarily used for the drawing scale
  // Since its static *every* WChit sees the same value for this.

  static G4int     maxPe;

  G4int                 totalPe;
  std::vector<G4double>  time;
  std::vector<G4int>    primaryParentID;
  G4int                 totalPeInGate;
};

typedef G4THitsCollection<WCSimWCHit> WCSimWCHitsCollection;

extern G4Allocator<WCSimWCHit> WCSimWCHitAllocator;

inline void* WCSimWCHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) WCSimWCHitAllocator.MallocSingle();
  return aHit;
}

inline void WCSimWCHit::operator delete(void *aHit)
{
  WCSimWCHitAllocator.FreeSingle((WCSimWCHit*) aHit);
}

#endif
