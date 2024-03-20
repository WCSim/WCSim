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

  G4double GetWCODWLSCladdingReflectivity() {return WCODWLSCladdingReflectivity;}
  void SetWCODWLSCladdingReflectivity(G4double tparam) {WCODWLSCladdingReflectivity=tparam;}

  G4double GetWCODTyvekReflectivityInWallTop() {return WCODTyvekReflectivityInWallTop;}
  void SetWCODTyvekReflectivityInWallTop(G4double tparam) {WCODTyvekReflectivityInWallTop=tparam;}

  G4double GetWCODTyvekReflectivityInWallBarrel() {return WCODTyvekReflectivityInWallBarrel;}
  void SetWCODTyvekReflectivityInWallBarrel(G4double tparam) {WCODTyvekReflectivityInWallBarrel=tparam;}

  G4double GetWCODTyvekReflectivityInWallBottom() {return WCODTyvekReflectivityInWallBottom;}
  void SetWCODTyvekReflectivityInWallBottom(G4double tparam) {WCODTyvekReflectivityInWallBottom=tparam;}

  G4double GetWCODTyvekReflectivityOutWallTop() {return WCODTyvekReflectivityOutWallTop;}
  void SetWCODTyvekReflectivityOutWallTop(G4double tparam) {WCODTyvekReflectivityOutWallTop=tparam;}

  G4double GetWCODTyvekReflectivityOutWallBarrel() {return WCODTyvekReflectivityOutWallBarrel;}
  void SetWCODTyvekReflectivityOutWallBarrel(G4double tparam) {WCODTyvekReflectivityOutWallBarrel=tparam;}

  G4double GetWCODTyvekReflectivityOutWallBottom() {return WCODTyvekReflectivityOutWallBottom;}
  void SetWCODTyvekReflectivityOutWallBottom(G4double tparam) {WCODTyvekReflectivityOutWallBottom=tparam;}

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

  G4double WCODWLSCladdingReflectivity;
  G4double WCODTyvekReflectivityInWallTop;
  G4double WCODTyvekReflectivityInWallBarrel;
  G4double WCODTyvekReflectivityInWallBottom;
  G4double WCODTyvekReflectivityOutWallTop;
  G4double WCODTyvekReflectivityOutWallBarrel;
  G4double WCODTyvekReflectivityOutWallBottom;

};

#endif







