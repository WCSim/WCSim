#ifndef WCSimTuningParameters_h
#define WCSimTuningParameters_h 1
#include "WCSimTuningMessenger.hh"
#include "WCSimRootOptions.hh"
#include "globals.hh"

class WCSimTuningParameters
{
public:
  WCSimTuningParameters();
  ~WCSimTuningParameters();


  // Setters and getters
  G4double GetRayff() {return rayff;}
  void SetRayff(G4double rparam) {rayff=rparam;}

  G4double GetBsrff() {return bsrff;}
  void SetBsrff(G4double rparam) {bsrff=rparam;}

  G4double GetAbwff() {return abwff;}
  void SetAbwff(G4double rparam) {abwff=rparam;}

  G4double GetRgcff() {return rgcff;}
  void SetRgcff(G4double rparam) {rgcff=rparam;}

  G4double GetMieff() {return mieff;}
  void SetMieff(G4double rparam) {mieff=rparam;}

  //For Top Veto - jl145
  G4double GetTVSpacing() {return tvspacing;}
  void SetTVSpacing(G4double tparam) {tvspacing=tparam;}

  G4bool GetTopVeto() {return topveto;}
  void SetTopVeto(G4double tparam) {topveto=tparam;}

  void SaveOptionsToOutput(WCSimRootOptions * wcopt);

  G4double GetWCCladdingReflectivity() {return WCCladdingReflectivity;}
  void SetWCCladdingReflectivity(G4double tparam) {WCCladdingReflectivity=tparam;}

  G4double GetWCODTyvekReflectivity() {return WCODTyvekReflectivity;}
  void SetWCODTyvekReflectivity(G4double tparam) {WCODTyvekReflectivity=tparam;}

private:

  // The messenger
  WCSimTuningMessenger* TuningMessenger;


  // The parameters that need to be set before WCSimDetectorConstruction
  // is created

  G4double rayff;
  G4double bsrff;
  G4double abwff;
  G4double rgcff;
  G4double mieff;

  //For Top Veto - jl145
  G4double tvspacing;
  G4bool topveto;

  G4double WCCladdingReflectivity;
  G4double WCODTyvekReflectivity;

};

#endif







