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
  
  void AddProcess(const G4String &process){ProcessList.insert(process);}
  void AddParticle(G4int pid){ParticleList.insert(pid);}
  void SetSaveHitProducingTracks(G4bool save){saveHitProducingTracks = save;}
  G4bool GetSaveHitProducingTracks(){return saveHitProducingTracks;}

private:
  std::set<G4String> ProcessList;
  std::set<G4int> ParticleList;
  G4bool saveHitProducingTracks = true;
  
  G4double fTime_birth;
  G4double fMaxTime;
  // TF: define in macro now
  G4double percentageOfCherenkovPhotonsToDraw;

  bool SAVE_PHOTON_HISTORY;

  WCSimTrackingMessenger* messenger;
  
  G4int primaryID;
};


#endif


