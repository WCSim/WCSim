#ifndef WCSimStackingAction_H
#define WCSimStackingAction_H 1

#include "globals.hh"
#include "G4UserStackingAction.hh"
#include "G4ParticleDefinition.hh"
#include "WCSimDetectorConstruction.hh"

class G4Track;

class WCSimStackingAction : public G4UserStackingAction {

  public:
    WCSimStackingAction(WCSimDetectorConstruction*);
    virtual ~WCSimStackingAction();

  public:
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    virtual void NewStage();
    virtual void PrepareNewEvent();

  private:
  WCSimDetectorConstruction*   DetConstruct;
  G4ParticleDefinition*      particleType_ID0; //The primary particle type
};

#endif

