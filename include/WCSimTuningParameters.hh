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

  //Added by B.Q 2018/07/25
  G4double GetQeff() {return qeff;}
  void SetQeff(G4double rparam) {qeff=rparam;}

  G4double GetMieff() {return mieff;}
  void SetMieff(G4double rparam) {mieff=rparam;}

  //Added by TD 2019/06/22
  G4double GetTtsff() {return ttsff;}
  void SetTtsff(G4double rparam) {ttsff=rparam;}

  //TD 2019.7.16
  G4double GetPMTSatur() {return pmtsatur;}
  void SetPMTSatur(G4double rparam) {pmtsatur=rparam;}
  
// TD 2019.6.26
  // G4double GetQoiff() {return qoiff;}
  // void SetQoiff(G4double rparam) {qoiff=rparam;}
  
  // G4double GetNLTinfo() {return nltinfo;}
  // void SetNLTinfo(G4double rparam) {nltinfo=rparam;}

  //For Top Veto - jl145
  G4double GetTVSpacing() {return tvspacing;}
  void SetTVSpacing(G4double tparam) {tvspacing=tparam;}

  G4bool GetTopVeto() {return topveto;}
  void SetTopVeto(G4double tparam) {topveto=tparam;}

  void SaveOptionsToOutput(WCSimRootOptions * wcopt);

private:

  // The messenger
  WCSimTuningMessenger* TuningMessenger;


  // The parameters that need to be set before WCSimDetectorConstruction
  // is created

  G4double rayff;
  G4double bsrff;
  G4double abwff;
  G4double rgcff;
  G4double qeff;
  G4double mieff;
  G4double ttsff;
  // G4double qoiff; // TD 2019.6.26
  G4double pmtsatur;

  //For Top Veto - jl145
  G4double tvspacing;
  G4bool topveto;

};

#endif







