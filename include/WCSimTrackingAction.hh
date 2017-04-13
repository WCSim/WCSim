#ifndef WCSimTrackingAction_h
#define WCSimTrackingAction_h

#include <set>
#include "G4UserTrackingAction.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Track;
class WCSimTrackingMessenger;

class WCSimTrackingAction : public G4UserTrackingAction
{
 public:
   WCSimTrackingAction();
  ~WCSimTrackingAction();

  void PreUserTrackingAction (const G4Track* aTrack);
  void PostUserTrackingAction(const G4Track*);

  void SetFractionChPhotons(G4double fraction){percentageOfCherenkovPhotonsToDraw = fraction;}
  
private:
  std::set<G4String> ProcessList;
  std::set<G4int> ParticleList;
  std::set<G4int> pi0List;

  // TF: define in macro now
  G4float percentageOfCherenkovPhotonsToDraw;

  WCSimTrackingMessenger* messenger;
};


#endif


