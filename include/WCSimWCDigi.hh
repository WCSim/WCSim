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
  std::vector<double> TriggerTimes;

  //lists (meaning vector/map) of information for each hit/digit created on the PMT
  std::map<int,double> pe;   ///< Charge of each Digi
  std::map<int,double> time_presmear; ///< Time of each Digi, before smearing
  std::map<int,double> time; ///< Time of each Digi
  std::vector<G4double>  time_double; ///< Same information as "time" but stored in a vector for quick time sorting
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
  std::map<int, G4float>    photonStartTime; ///< Primary parent ID of the Hit (do not use for Digits)
  std::map<int, G4ThreeVector>    photonStartPos; ///< Start point of the photon of the Hit (do not use for Digits)
  std::map<int, G4ThreeVector>    photonEndPos; ///< End point of the photon of the Hit (do not use for Digits)
  std::map<int, G4ThreeVector>    photonStartDir; ///< Start dir of the photon of the Hit (do not use for Digits)
  std::map<int, G4ThreeVector>    photonEndDir; ///< End dir of the photon of the Hit (do not use for Digits)
  std::map<int, std::string> photonCreatorProcess; 
  

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

  inline void AddGate(int g,double t) { Gates.insert(g); TriggerTimes.push_back(t);}
  inline void SetPe(G4int gate,  G4double Q)      {pe[gate]     = Q;};
  inline void SetTime(G4int gate, G4double T)    {time[gate]   = T;};
  inline void SetPreSmearTime(G4int gate, G4double T)    {time_presmear[gate]   = T;};
  inline void SetParentID(G4int gate, G4int parent) { primaryParentID[gate] = parent; };
  inline void SetPhotonStartTime(G4int gate, G4float starttime) { photonStartTime[gate] = starttime; };
  inline void SetPhotonStartPos(G4int gate, const G4ThreeVector &position) { photonStartPos[gate] = position; };
  inline void SetPhotonEndPos(G4int gate, const G4ThreeVector &position) { photonEndPos[gate] = position; };
  inline void SetPhotonStartDir(G4int gate, const G4ThreeVector &direction) { photonStartDir[gate] = direction; };
  inline void SetPhotonEndDir(G4int gate, const G4ThreeVector &direction) { photonEndDir[gate] = direction; };
  inline void SetPhotonCreatorProcess(G4int gate, G4String creatorProcess) { photonCreatorProcess[gate] = creatorProcess; };

  // Add a digit number and unique photon number to fDigiComp
  inline void AddPhotonToDigiComposition(int digi_number, int photon_number){
    fDigiComp.at(digi_number).push_back(photon_number);
  }
  // Add a whole vector for one digit to fDigiComp. Clear input vector once added.
  void AddDigiCompositionInfo(std::vector<int> & digi_comp){
    const size_t size = fDigiComp.size();
    fDigiComp.emplace(size,digi_comp);
    digi_comp.clear();
  }

  inline G4int          GetParentID(int gate)    { return primaryParentID[gate];};
  inline G4float        GetPhotonStartTime(int gate)    { return photonStartTime[gate];};
  inline G4ThreeVector  GetPhotonStartPos(int gate)    { return photonStartPos[gate];};
  inline G4ThreeVector  GetPhotonEndPos(int gate)    { return photonEndPos[gate];};
  inline G4ThreeVector  GetPhotonStartDir(int gate)    { return photonStartDir[gate];};
  inline G4ThreeVector  GetPhotonEndDir(int gate)    { return photonEndDir[gate];};
  inline G4String       GetPhotonCreatorProcess(int gate) { return photonCreatorProcess[gate];};
  inline G4int          GetTrackID()    { return trackID;};
  inline G4double GetGateTime(int gate) { return TriggerTimes[gate];}
  inline G4int   GetTubeID() {return tubeID;};
  inline G4String   GetTubeType() {return tubeType;};
  inline G4ThreeVector GetPos(){ return pos;};
  inline G4ThreeVector GetOrientation(){ return orient;};
  inline G4RotationMatrix GetRot(){ return rot;};
  inline G4double GetPe(int gate)     {return pe[gate];};
  inline G4double GetTime(int gate)   {
    try {
      return time.at(gate);
    }
    catch (...) {
      G4cerr<<"Exception occurred while attempting to use WCSimWCDigi::GetTime to retrieve time for pe "
            << gate << " from map of times. The time map has "<<time.size()<<" entries:" << G4endl;
      for(std::map<int,double>::iterator time_element=time.begin(); time_element!=time.end(); time_element++){
        try{
          G4cerr << time_element->first << ": ";
          G4cerr << time_element->second << G4endl;
        }
        catch (...) {
          G4cerr << G4endl << "Exception reading map entry!!"<<G4endl;
          break;
        }
      }
      throw;
    }
  };
  inline std::map<int,double>::const_iterator GetTimeMapBegin() {return time.cbegin();}
  inline std::map<int,double>::const_iterator GetTimeMapEnd() {return time.cend();}
  inline G4double GetPreSmearTime(int gate)   {return time_presmear.at(gate);};
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

  void AddPe(G4double hitTime)
  {
    // Increment the totalPe number
    totalPe++; 
        
    time_double.push_back(hitTime);
  }

  void SortHitTimes() {   sort(time_double.begin(),time_double.end()); }


  void SortDigiMapsByHitTime() {
    int i, j;

    double index_time,index_timepresmear,index_pe;
    int index_primaryparentid;
    std::vector<int> index_digicomp;
    float index_photonstarttime;
    G4ThreeVector index_photonstartpos;
    G4ThreeVector index_photonendpos;
    G4ThreeVector index_photonstartdir;
    G4ThreeVector index_photonenddir;
    G4String      index_photoncreatorprocess;
    bool sort_digi_compositions = (fDigiComp.size()==time.size());
    // SortDigiMapsByHitTime is called by WCSimWCDigitizerSKI::DigitizeHits to sort the WCRawPMTSignalCollection.
    // Each entry in WCRawPMTSignalCollection represents the set of photon hits on a PMT.
    // Since a photon hit has no "composition", fDigiComp is empty at this time and needn't be sorted.
    // for generality, sort if the digi composition map has the same size as other maps

    for (i = 1; i < (int) time.size(); ++i)
      {
        index_time  = time.at(i);
        index_timepresmear  = time_presmear.at(i);
        index_pe = pe.at(i);
        if(sort_digi_compositions) index_digicomp = fDigiComp.at(i);
        index_primaryparentid = primaryParentID.at(i);
	      index_photonstarttime = photonStartTime[i];
	      index_photonstartpos = photonStartPos[i];
	      index_photonendpos = photonEndPos[i];
	      index_photonstartdir = photonStartDir[i];
	      index_photonenddir = photonEndDir[i];
        index_photoncreatorprocess = photonCreatorProcess[i];
        for (j = i; j > 0 && time.at(j-1) > index_time; j--) {
          time.at(j) = time.at(j-1);
          time_presmear.at(j) = time_presmear.at(j-1);
          pe.at(j) = pe.at(j-1);
          if(sort_digi_compositions) fDigiComp.at(j) = fDigiComp.at(j-1);
          primaryParentID.at(j) = primaryParentID.at(j-1);
	        photonStartTime.at(j) = photonStartTime.at(j-1);
	        photonStartPos.at(j) = photonStartPos.at(j-1);
	        photonEndPos.at(j) = photonEndPos.at(j-1);
	        photonStartDir.at(j) = photonStartDir.at(j-1);
	        photonEndDir.at(j) = photonEndDir.at(j-1);
          photonCreatorProcess.at(j) = photonCreatorProcess.at(j-1);
          //G4cout <<"swapping "<<time.at(j-1)<<" "<<index_time<<G4endl;
        }
        time.at(j) = index_time;
        time_presmear.at(j) = index_timepresmear;
        pe.at(j) = index_pe;
        if(sort_digi_compositions) fDigiComp.at(j) = index_digicomp;
        primaryParentID.at(j) = index_primaryparentid;
	      photonStartTime.at(j) = index_photonstarttime;
	      photonStartPos.at(j) = index_photonstartpos;
	      photonEndPos.at(j) = index_photonendpos;
	      photonStartDir.at(j) = index_photonstartdir;
	      photonEndDir.at(j) = index_photonenddir;
        photonCreatorProcess.at(j) = index_photoncreatorprocess;
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


  G4double GetFirstHitTimeInGate(G4double low,G4double upevent)
  {
    G4double firsttime;
    std::vector<G4double>::iterator tfirst = time_double.begin();
    std::vector<G4double>::iterator tlast = time_double.end();

	std::vector<G4double>::iterator found =
		std::find_if(tfirst,tlast,
					 [&](const G4double& val) {
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

  // G. Pronost:	
  // Sort function by Hit Time (using first time, assuming hit time in a hit are sorted)
  struct SortFunctor_Hit {
    bool operator() (const WCSimWCDigi * const &a,
                     const WCSimWCDigi * const &b) const;
  };
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
