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

  G4int GetPMTSurfType() {return pmtsurftype;}
  void SetPMTSurfType(G4double rparam) {pmtsurftype=rparam;}

  void ReadCathodeParaTable(std::string);
  G4double GetCathodeThickness() {return cathodeThickness;}
  G4int GetNCathodePara() {return nCathodePara;}
  std::vector<std::vector<G4double>> GetCathodeParaTable() {return cathodeparaTable;}

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

  G4double GetWCODWLSCladdingReflectivity() {return WCODWLSCladdingReflectivity;}
  void SetWCODWLSCladdingReflectivity(G4double tparam) {WCODWLSCladdingReflectivity=tparam;}

  bool GetUsingNuPrismTyvekReflectivityModel() {return UsingNuPrismTyvekReflectivityModel;}
  void SetUsingNuPrismTyvekReflectivityModel(G4bool tparam) {UsingNuPrismTyvekReflectivityModel = tparam;}
  // default. Only used in ConstructCylinder() (e.g. for IWCD)
  G4double GetWCODDefaultTyvekReflectivity() {return WCODDefaultTyvekReflectivity;}
  void SetWCODDefaultTyvekReflectivity(G4double tparam) {WCODDefaultTyvekReflectivity=tparam;}
  // Specific surfaces. Only used in RealisticPlacement (i.e. for HKFD)
  //inner-wall tyvek
  G4double GetWCODInnerTopCapTyvekReflectivity() {return WCODInnerTopCapTyvekReflectivity;}
  void SetWCODInnerTopCapTyvekReflectivity(G4double tparam) {WCODInnerTopCapTyvekReflectivity=tparam;}
  G4double GetWCODInnerBarrelTyvekReflectivity() {return WCODInnerBarrelTyvekReflectivity;}
  void SetWCODInnerBarrelTyvekReflectivity(G4double tparam) {WCODInnerBarrelTyvekReflectivity=tparam;}
  G4double GetWCODInnerBottomCapTyvekReflectivity() {return WCODInnerBottomCapTyvekReflectivity;}
  void SetWCODInnerBottomCapTyvekReflectivity(G4double tparam) {WCODInnerBottomCapTyvekReflectivity=tparam;}
  //outer-wall tyvek
  G4double GetWCODOuterTopCapTyvekReflectivity() {return WCODOuterTopCapTyvekReflectivity;}
  void SetWCODOuterTopCapTyvekReflectivity(G4double tparam) {WCODOuterTopCapTyvekReflectivity=tparam;}
  G4double GetWCODOuterBarrelTyvekReflectivity() {return WCODOuterBarrelTyvekReflectivity;}
  void SetWCODOuterBarrelTyvekReflectivity(G4double tparam) {WCODOuterBarrelTyvekReflectivity=tparam;}
  G4double GetWCODOuterBottomCapTyvekReflectivity() {return WCODOuterBottomCapTyvekReflectivity;}
  void SetWCODOuterBottomCapTyvekReflectivity(G4double tparam) {WCODOuterBottomCapTyvekReflectivity=tparam;}
  
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

  // PMT photocathode surface properties
  G4int pmtsurftype;
  G4double cathodeThickness;
  G4int nCathodePara;
  std::vector<std::vector<G4double>>  cathodeparaTable;

  //For Top Veto - jl145
  G4double tvspacing;
  G4bool topveto;

  G4double WCODWLSCladdingReflectivity;

  G4bool   UsingNuPrismTyvekReflectivityModel;
  G4double WCODDefaultTyvekReflectivity;
  G4double WCODInnerTopCapTyvekReflectivity;
  G4double WCODInnerBarrelTyvekReflectivity;
  G4double WCODInnerBottomCapTyvekReflectivity;
  G4double WCODOuterTopCapTyvekReflectivity;
  G4double WCODOuterBarrelTyvekReflectivity;
  G4double WCODOuterBottomCapTyvekReflectivity;
};

#endif







