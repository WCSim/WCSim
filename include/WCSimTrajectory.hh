class WCSimTrajectory;

#ifndef WCSimTrajectory_h
#define WCSimTrajectory_h 1

#include "G4VTrajectory.hh"
#include "G4Allocator.hh"
#include <stdlib.h>                 // Include from 'system'
#include "G4ios.hh"               // Include from 'system'
#include <vector>            // G4RWTValOrderedVector
#include "globals.hh"               // Include from 'global'
#include "G4ParticleDefinition.hh"  // Include from 'particle+matter'
#include "G4TrajectoryPoint.hh"     // Include from 'tracking'
#include "G4Track.hh"
#include "G4Step.hh"

class G4Polyline;                   // Forward declaration.

typedef std::vector<G4VTrajectoryPoint*>  TrajectoryPointContainer;
///////////////////
class WCSimTrajectory : public G4VTrajectory
///////////////////
{

//--------
public: // with description
//--------

// Constructor/Destrcutor

   WCSimTrajectory();

   WCSimTrajectory(const G4Track* aTrack);
   WCSimTrajectory(WCSimTrajectory &);
   virtual ~WCSimTrajectory();

// Operators
   inline void* operator new(size_t);
   inline void  operator delete(void*);
   inline int operator == (const WCSimTrajectory& right) const
   {return (this==&right);} 

// Get/Set functions 
   inline G4int GetTrackID() const
   { return fTrackID; }
   inline G4int GetParentID() const
   { return fParentID; }
   inline G4String GetParticleName() const
   { return ParticleName; }
   inline G4double GetCharge() const
   { return PDGCharge; }
   inline G4int GetPDGEncoding() const
   { return PDGEncoding; }
   inline G4ThreeVector GetInitialMomentum() const
   { return initialMomentum; }
  inline G4String GetCreatorProcessName() const {
    return creatorProcess;
  }
  
  inline G4double GetGlobalTime() const
  { return globalTime; }
  inline G4bool GetSaveFlag() const { return SaveIt; }
  inline void SetSaveFlag(G4bool value) { SaveIt = value; }

// New function we have added
   inline G4ThreeVector GetStoppingPoint() const
   { return stoppingPoint; }
   inline G4VPhysicalVolume* GetStoppingVolume() const
   { return stoppingVolume;}
   inline void SetStoppingPoint(G4ThreeVector& currentPosition) 
   { stoppingPoint = currentPosition;}
   inline void SetStoppingVolume(G4VPhysicalVolume* currentVolume)
   { stoppingVolume = currentVolume;}


// Other member functions
   virtual void ShowTrajectory(std::ostream& os=G4cout) const;
   virtual void DrawTrajectory(G4int i_mode=0) const;
   virtual void AppendStep(const G4Step* aStep);
   virtual int GetPointEntries() const { return positionRecord->size(); }
   virtual G4VTrajectoryPoint* GetPoint(G4int i) const 
   { return (*positionRecord)[i]; }
   virtual void MergeTrajectory(G4VTrajectory* secondTrajectory);

   G4ParticleDefinition* GetParticleDefinition();

   virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
   virtual std::vector<G4AttValue>* CreateAttValues() const;

//---------
   private:
//---------

  TrajectoryPointContainer* positionRecord;
  G4int                     fTrackID;
  G4int                     fParentID;
  G4int                     PDGEncoding;
  G4double                  PDGCharge;
  G4String                  ParticleName;
  G4ThreeVector             initialMomentum;

  // These are new variables
  G4ThreeVector             stoppingPoint;
  G4VPhysicalVolume         *stoppingVolume;

  // M Fechner : new saving mechanism
  G4bool SaveIt;
  G4String creatorProcess;
  G4double                  globalTime;
};

/***            TEMP  : M FECHNER ***********
** modification by Chris Walter that works for geant4 >= 4.6.2p01
** does not compile with 4.6.1
#if defined G4TRACKING_ALLOC_EXPORT
  extern G4DLLEXPORT G4Allocator<WCSimTrajectory> myTrajectoryAllocator;
#else
  extern G4DLLIMPORT G4Allocator<WCSimTrajectory> myTrajectoryAllocator;
#endif
*/

extern G4Allocator<WCSimTrajectory> myTrajectoryAllocator;

inline void* WCSimTrajectory::operator new(size_t)
{
  void* aTrajectory;
  aTrajectory = (void*)myTrajectoryAllocator.MallocSingle();
  return aTrajectory;
}

inline void WCSimTrajectory::operator delete(void* aTrajectory)
{
  myTrajectoryAllocator.FreeSingle((WCSimTrajectory*)aTrajectory);
}

#endif

