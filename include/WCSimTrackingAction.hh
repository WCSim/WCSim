#ifndef WCSimTrackingAction_h
#define WCSimTrackingAction_h

#include <set>
#include "G4UserTrackingAction.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Track;

class WCSimTrackingAction : public G4UserTrackingAction
{
 public:
   WCSimTrackingAction();
  ~WCSimTrackingAction();

   void PreUserTrackingAction (const G4Track* aTrack);
  void PostUserTrackingAction(const G4Track*);

private:
  std::set<G4String> ProcessList;
  std::set<G4int> ParticleList;
  std::set<G4int> pi0List;
  
  G4double fTime_birth;
  G4double fMaxTime;
};


#endif


