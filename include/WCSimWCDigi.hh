#ifndef WCSimWCDigi_h
#define WCSimWCDigi_h 1

#include "G4VDigi.hh"
#include "G4TDigiCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4ios.hh"

#include <set>
#include <map>
#include <vector>

// for sort, find, count_if
#include <algorithm>
//for less_equal, bind2nd,...
#include <functional>

// compose2 is not part of the C++ standard
#include <ext/functional>
using __gnu_cxx::compose2;



class WCSimWCDigi : public G4VDigi
{

public:
  
  WCSimWCDigi();
  ~WCSimWCDigi();
  WCSimWCDigi(const WCSimWCDigi&);
  const WCSimWCDigi& operator=(const WCSimWCDigi&);
  int operator==(const WCSimWCDigi&) const;
  
  inline void* operator new(size_t);
  inline void  operator delete(void*);
  
  void Draw();
  void Print();

private:

  //PMT parameters
  G4int   tubeID;
  G4RotationMatrix rot;
  G4ThreeVector    pos;
  G4ThreeVector    orient;
  G4LogicalVolume* pLogV;
  G4String tubeType;//Added by B.Q for hybrid PMT configuration
  
  //'Gates' is a digit counter or specifies subevent
  //'TriggerTimes' specifies e.g. the subevent trigger time
  std::set<int> Gates; // list of gates that were hit  
  std::vector<float> TriggerTimes;

  //lists (meaning vector/map) of information for each hit/digit created on the PMT
  std::map<int,float> pe;   ///< Charge of each Digi
  std::map<int,float> time_presmear; ///< Time of each Digi, before smearing
  std::map<int,float> time; ///< Time of each Digi
  std::vector<G4float>  time_float; ///< Same information as "time" but stored in a vector for quick time sorting
  /** \brief IDs of the hits that make up this Digit (do not use for Hits)
   *
   * Stores the unique IDs of each photon making up a digit
   * Each digit can be made from multiple photons, therefore a vector is used
   * For example: 0:[3,4,6]; 1:[10,11,13,14]
   *  The first digit in the event is made of of photons 3,4,6;
   *  The second digit is made up of photons: 10,11,13,14
   */
  std::map<int, std::vector<int> > fDigiComp;
  std::map<int, G4int>    primaryParentID; ///< Primary parent ID of the Hit (do not use for Digits)
  

  //integrated hit/digit parameters
  G4int                 totalPe;

  //parameters not actually used?
  G4int                 totalPeInGate;
  G4double         edep;
  static G4int     maxPe;
  G4int            trackID;

public:
  void RemoveDigitizedGate(G4int gate);
  
  inline void SetTubeID(G4int tube) {tubeID = tube;};
  inline void SetTubeType(G4String tube_type) {tubeType = tube_type;};
  inline void AddGate(int g,float t) { Gates.insert(g); TriggerTimes.push_back(t);}
  inline void SetPe(G4int gate,  G4float Q)      {pe[gate]     = Q;};
  inline void SetTime(G4int gate, G4float T)    {time[gate]   = T;};
  inline void SetPreSmearTime(G4int gate, G4float T)    {time_presmear[gate]   = T;};
  inline void SetParentID(G4int gate, G4int parent) { primaryParentID[gate] = parent; };

  // Add a digit number and unique photon number to fDigiComp
  inline void AddPhotonToDigiComposition(int digi_number, int photon_number){
    fDigiComp[digi_number].push_back(photon_number);
  }
  // Add a whole vector for one digit to fDigiComp. Clear input vector once added.
  void AddDigiCompositionInfo(std::vector<int> & digi_comp){
    const size_t size = fDigiComp.size();
    fDigiComp[size] = digi_comp;
    digi_comp.clear();
  }


  inline G4int          GetParentID(int gate)    { return primaryParentID[gate];};
  inline G4int          GetTrackID()    { return trackID;};
  inline G4float GetGateTime(int gate) { return TriggerTimes[gate];}
  inline G4int   GetTubeID() {return tubeID;};
  inline G4String   GetTubeType() {return tubeType;};
  inline G4ThreeVector GetPos(){ return pos;};
  inline G4ThreeVector GetOrientation(){ return orient;};
  inline G4RotationMatrix GetRot(){ return rot;};
  inline G4float GetPe(int gate)     {return pe[gate];};
  inline G4float GetTime(int gate)   {return time[gate];};
  inline G4float GetPreSmearTime(int gate)   {return time_presmear[gate];};
  std::vector<int> GetDigiCompositionInfo(int gate);
  inline std::map< int, std::vector<int> > GetDigiCompositionInfo(){return fDigiComp;}

  inline int NumberOfGates() { return Gates.size();}
  inline int NumberOfSubEvents() { return (Gates.size()-1);}
  inline bool HasHitsInGate(int number) { return (Gates.count(number)>0); }
  
  G4LogicalVolume* GetLogicalVolume() {return pLogV;};


  void SetEdep         (G4double de)                { edep = de; };
  void SetPos          (G4ThreeVector xyz)          { pos = xyz; };
  void SetOrientation  (G4ThreeVector xyz)          { orient = xyz; };
  void SetLogicalVolume(G4LogicalVolume* logV)      { pLogV = logV;}
  void SetTrackID      (G4int track)                { trackID = track; };
  void SetRot          (G4RotationMatrix rotMatrix) { rot = rotMatrix; };
  G4int         GetTotalPe()    { return totalPe;};
  
  void SetMaxPe(G4int number = 0)  {maxPe   = number;};

  void AddPe(G4float hitTime)  
  {
    // Increment the totalPe number
    totalPe++; 
        
    time_float.push_back(hitTime);
  }

  void SortHitTimes() {   sort(time_float.begin(),time_float.end()); }


  void SortArrayByHitTime() {
    int i, j;
    float index_time,index_timepresmear,index_pe;
    std::vector<int> index_digicomp;
    int index_primaryparentid;
    for (i = 1; i < (int) time.size(); ++i)
      {
        index_time  = time[i];
        index_timepresmear  = time_presmear[i];
        index_pe = pe[i];
	index_digicomp = fDigiComp[i];
	index_primaryparentid = primaryParentID[i];
        for (j = i; j > 0 && time[j-1] > index_time; j--) {
          time[j] = time[j-1];
          pe[j] = pe[j-1];
	  fDigiComp[j] = fDigiComp[j-1];
	  primaryParentID[j] = primaryParentID[j-1];
          //G4cout <<"swapping "<<time[j-1]<<" "<<index_time<<G4endl;
        }
        
        time[j] = index_time;
        time_presmear[j] = index_timepresmear;
        pe[j] = index_pe;
	fDigiComp[j] = index_digicomp;
	primaryParentID[j] = index_primaryparentid;
      }    
  }
  
  void insertionSort(int a[], int array_size)
  {
    int i, j, index;
    for (i = 1; i < array_size; ++i)
      {
        index = a[i];
        for (j = i; j > 0 && a[j-1] > index; j--)
          a[j] = a[j-1];
        
        a[j] = index;
      }
  }


  G4float GetFirstHitTimeInGate(G4float low,G4float upevent)
  {
    G4float firsttime;
    std::vector<G4float>::iterator tfirst = time_float.begin();
    std::vector<G4float>::iterator tlast = time_float.end();
    
    std::vector<G4float>::iterator found = 
      std::find_if(tfirst,tlast,
		   compose2(std::logical_and<bool>(),
			    std::bind2nd(std::greater_equal<G4float>(),low),
			    std::bind2nd(std::less_equal<G4float>(),upevent)
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

 
};

typedef G4TDigiCollection<WCSimWCDigi> WCSimWCDigitsCollection;
extern G4Allocator<WCSimWCDigi> WCSimWCDigiAllocator;

inline void* WCSimWCDigi::operator new(size_t)
{
  void* aDigi;
  aDigi = (void*) WCSimWCDigiAllocator.MallocSingle();
  return aDigi;
}

inline void WCSimWCDigi::operator delete(void* aDigi)
{
  WCSimWCDigiAllocator.FreeSingle((WCSimWCDigi*) aDigi);
}

#endif
