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

// compose2 is not part of the C++ standard
// use this kludgy technique to use it
#include <ext/functional>
using __gnu_cxx::identity_element;
using __gnu_cxx::unary_compose;
using __gnu_cxx::binary_compose;
using __gnu_cxx::compose1;
using __gnu_cxx::compose2;
using __gnu_cxx::identity;
using __gnu_cxx::select1st;
using __gnu_cxx::select2nd;
using __gnu_cxx::project1st;
using __gnu_cxx::project2nd;
using __gnu_cxx::constant_void_fun;
using __gnu_cxx::constant_unary_fun;
using __gnu_cxx::constant_binary_fun;
using __gnu_cxx::constant0;
using __gnu_cxx::constant1;
using __gnu_cxx::constant2;
using __gnu_cxx::subtractive_rng;
using __gnu_cxx::mem_fun1;
using __gnu_cxx::mem_fun1_ref;



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
  void SetOrientation  (G4ThreeVector xyz)          { orient = xyz; };
  void SetRot          (G4RotationMatrix rotMatrix) { rot = rotMatrix; };
  void SetLogicalVolume(G4LogicalVolume* logV)      { pLogV = logV;}
  void AddParentID     (G4int primParentID) { primaryParentID.push_back(primParentID); }
  void AddPhotonStartTime (G4float photStartTime) { photonStartTime.push_back(photStartTime); }
  void AddPhotonStartPos  (const G4ThreeVector &photStartPos) { photonStartPos.push_back(photStartPos); }
  void SetTubeType     (G4String tube_type)          { tubeType = tube_type; }; //Added by B.Quilain to transmit on which PMT type the hit happened. For detectors with several PMT types in ID.

  
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
  G4ThreeVector GetOrientation()        { return orient; };
  G4int         GetTotalPe()    { return totalPe;};
  G4double      GetTime(int i)  { return time[i];};
  G4int         GetParentID(int i) { return primaryParentID[i];};
  G4String         GetTubeType()     { return tubeType; };
  G4float       GetPhotonStartTime(int i) { return photonStartTime[i];};
  G4ThreeVector GetPhotonStartPos(int i) { return photonStartPos[i];};
  
  G4LogicalVolume* GetLogicalVolume() {return pLogV;};

  void SortHitTimes() {   sort(time.begin(),time.end()); }


  // low is the trigger time, up is trigger+950ns (end of event)
  G4double GetFirstHitTimeInGate(G4double low,G4double upevent)
  {
    G4double firsttime;
    std::vector<G4double>::iterator tfirst = time.begin();
    std::vector<G4double>::iterator tlast = time.end();
  
    std::vector<G4double>::iterator found = 
      std::find_if(tfirst,tlast,
		   compose2(std::logical_and<bool>(),
			    std::bind2nd(std::greater_equal<G4double>(),low),
			    std::bind2nd(std::less_equal<G4double>(),upevent)
			    )
		   );
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

  G4int GetPeInGate(G4double low, G4double pmtgate,G4double evgate) {
    // M Fechner; april 2005
    // assumes that time has already been sorted
    std::vector<G4double>::iterator tfirst = time.begin();
    std::vector<G4double>::iterator tlast = time.end();
    // select min time
    G4double mintime = (pmtgate < evgate) ? pmtgate : evgate;
    
    // return number of hits in the time window...
    
    G4int number = std::count_if(tfirst,tlast,
				 compose2(std::logical_and<bool>(),
					  std::bind2nd(std::greater_equal<G4double>(),low),
					  std::bind2nd(std::less_equal<G4double>(),mintime)
					  )
				 );
    
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
  
  void HSVtoRGB(double& fR, double& fG, double& fB, double& fH, double& fS, double& fV);

  G4int            tubeID;
  G4int            trackID;
  G4double         edep;
  G4ThreeVector    pos;
  G4ThreeVector    orient;
  G4RotationMatrix rot;
  G4LogicalVolume* pLogV;
  G4String tubeType;//Added by B.Quilain to transmit on which PMT type the hit happened. For detectors with several PMT types in ID.

  // This is temporarily used for the drawing scale
  // Since its static *every* WChit sees the same value for this.

  static G4int     maxPe;

  G4int                 totalPe;
  std::vector<G4double> time;
  std::vector<G4int>    primaryParentID;
  std::vector<G4float>  photonStartTime;
  std::vector<G4ThreeVector> photonStartPos;
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
