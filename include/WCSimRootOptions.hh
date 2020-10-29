#ifndef WCSim_RootOptions
#define WCSim_RootOptions

//////////////////////////////////////////////////////////////////////////
//                                                                      
// WCSim_RootOptions                                                      
//                                                                      
// This class contains run option information
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include <string>
#include <map>
#include <iostream>

#include "WCSimEnumerations.hh"

class TDirectory;
using std::string;
using std::map;

//////////////////////////////////////////////////////////////////////////

struct WCSimDarkNoiseOptions {
  double PMTDarkRate; // kHz
  double ConvRate; // kHz
  double DarkHigh; // ns
  double DarkLow; // ns
  double DarkWindow; // ns
  int    DarkMode;  
  WCSimDarkNoiseOptions() :
    PMTDarkRate(-999), ConvRate(-999), DarkHigh(-999), DarkLow(-999),
    DarkWindow(-999), DarkMode(-999)
  {}
};

//////////////////////////////////////////////////////////////////////////

class WCSimRootOptions : public TObject {

public:

  WCSimRootOptions();
  virtual ~WCSimRootOptions();
  void Print(Option_t *option = "") const;

  //WCSimDetector* gets
  void SetDetectorName(string iDetectorName) {DetectorName = iDetectorName;}
  void SetGeomHasOD(bool iGeomHasOD) {GeomHasOD = iGeomHasOD;}
  void SetSavePi0(bool iSavePi0) {SavePi0 = iSavePi0;}
  void SetPMTQEMethod(bool iPMTQEMethod) {PMTQEMethod = iPMTQEMethod;}
  void SetPMTCollEff(bool iPMTCollEff) {PMTCollEff = iPMTCollEff;}
  //WCSimDetector* sets
  string GetDetectorName() {return DetectorName;}
  bool   GetGeomHasOD() {return GeomHasOD;}
  bool   GetSavePi0() {return SavePi0;}
  int    GetPMTQEMethod() {return PMTQEMethod;}
  int    GetPMTCollEff() {return PMTCollEff;}
  //WCSimWCAddDarkNoise sets
  void SetPMTDarkRate(string tag, double iPMTDarkRate) {DarkOptMap[tag].PMTDarkRate = iPMTDarkRate;}
  void SetConvRate(string tag, double iConvRate) {DarkOptMap[tag].ConvRate = iConvRate;}
  void SetDarkHigh(string tag, double iDarkHigh) {DarkOptMap[tag].DarkHigh = iDarkHigh;}
  void SetDarkLow(string tag, double iDarkLow) {DarkOptMap[tag].DarkLow = iDarkLow;}
  void SetDarkWindow(string tag, double iDarkWindow) {DarkOptMap[tag].DarkWindow = iDarkWindow;}
  void SetDarkMode(string tag, int iDarkMode) {DarkOptMap[tag].DarkMode = iDarkMode;}
  //WCSimWCAddDarkNoise gets
  bool IsValidDarkTag(string tag) const;
  double GetPMTDarkRate(string tag);
  double GetConvRate(string tag);
  double GetDarkHigh(string tag);
  double GetDarkLow(string tag);
  double GetDarkWindow(string tag);
  int    GetDarkMode(string tag);
  //WCSimWCDigitizer* sets
  void SetDigitizerClassName(string iDigitizerClassName) {DigitizerClassName = iDigitizerClassName;}
  void SetDigitizerDeadTime(int iDigitizerDeadTime) {DigitizerDeadTime = iDigitizerDeadTime;}
  void SetDigitizerIntegrationWindow(int iDigitizerIntegrationWindow) {DigitizerIntegrationWindow = iDigitizerIntegrationWindow;}
  void SetDigitizerTimingPrecision(double iDigitizerTimingPrecision) {DigitizerTimingPrecision = iDigitizerTimingPrecision;}
  void SetDigitizerPEPrecision(double iDigitizerPEPrecision) {DigitizerPEPrecision = iDigitizerPEPrecision;}
  //WCSimWCDigitizer* gets
  string GetDigitizerClassName() {return DigitizerClassName;}
  int    GetDigitizerDeadTime() {return DigitizerDeadTime;}
  int    GetDigitizerIntegrationWindow() {return DigitizerIntegrationWindow;}
  int    GetDigitizerTimingPrecision() {return DigitizerTimingPrecision;}
  int    GetDigitizerPEPrecision() {return DigitizerPEPrecision;}
  //WCSimWCTrigger* sets
  void SetTriggerClassName(string itriggerClassName) {TriggerClassName = itriggerClassName;};
  void SetMultiDigitsPerTrigger(bool imultiDigitsPerTrigger) {MultiDigitsPerTrigger = imultiDigitsPerTrigger;};
  //ndigits
  void SetNDigitsThreshold(int indigitsThreshold) {NDigitsThreshold = indigitsThreshold;};
  void SetNDigitsWindow(int indigitsWindow) {NDigitsWindow = indigitsWindow;};
  void SetNDigitsAdjustForNoise(bool indigitsAdjustForNoise) {NDigitsAdjustForNoise = indigitsAdjustForNoise;};
  void SetNDigitsPreTriggerWindow(int indigitsPreTriggerWindow) {NDigitsPreTriggerWindow = indigitsPreTriggerWindow;};
  void SetNDigitsPostTriggerWindow(int indigitsPostTriggerWindow) {NDigitsPostTriggerWindow = indigitsPostTriggerWindow;};
  //savefailures
  void SetSaveFailuresMode(int isaveFailuresMode) {SaveFailuresMode = isaveFailuresMode;};
  void SetSaveFailuresTime(double isaveFailuresTime) {SaveFailuresTime = isaveFailuresTime;};
  void SetSaveFailuresPreTriggerWindow(int isaveFailuresPreTriggerWindow) {SaveFailuresPreTriggerWindow = isaveFailuresPreTriggerWindow;};
  void SetSaveFailuresPostTriggerWindow(int isaveFailuresPostTriggerWindow) {SaveFailuresPostTriggerWindow = isaveFailuresPostTriggerWindow;};
  //WCSimWCTrigger* gets
  string GetTriggerClassName() {return TriggerClassName;}
  bool   GetMultiDigitsPerTrigger() {return MultiDigitsPerTrigger;}
  //ndigits
  int    GetNDigitsThreshold() {return NDigitsThreshold;}
  int    GetNDigitsWindow() {return NDigitsWindow;}
  bool   GetNDigitsAdjustForNoise() {return NDigitsAdjustForNoise;}
  int    GetNDigitsPreTriggerWindow() {return NDigitsPreTriggerWindow;}
  int    GetNDigitsPostTriggerWindow() {return NDigitsPostTriggerWindow;}
  //savefailures
  int    GetSaveFailuresMode() {return SaveFailuresMode;}
  double GetSaveFailuresTime() {return SaveFailuresTime;}
  int    GetSaveFailuresPreTriggerWindow() {return SaveFailuresPreTriggerWindow;}
  int    GetSaveFailuresPostTriggerWindow() {return SaveFailuresPostTriggerWindow;}
  //WCSimTuningParameters sets
  void SetRayff(double iRayff) {Rayff = iRayff;}
  void SetBsrff(double iBsrff) {Bsrff = iBsrff;}
  void SetAbwff(double iAbwff) {Abwff = iAbwff;}
  void SetRgcff(double iRgcff) {Rgcff = iRgcff;}
  void SetMieff(double iMieff) {Mieff = iMieff;}
  void SetQeff(double iQeff) {Qeff = iQeff;}//B.Q 2018/07/25
  void SetTtsff(double iTtsff) {Ttsff = iTtsff;}//TD 2019.06.22
  void SetPMTSatur(double iPmtSatur) {PmtSatur = iPmtSatur;}//TD 2019.07.16
  // void SetQoiff(double iQoiff) {Qoiff = iQoiff;}//TD 2019.06.26
  void SetTvspacing(double iTvspacing) {Tvspacing = iTvspacing;}
  void SetTopveto(bool iTopveto) {Topveto = iTopveto;}
  //WCSimTuningParameters gets
  double GetRayff() {return Rayff;}
  double GetBsrff() {return Bsrff;}
  double GetAbwff() {return Abwff;}
  double GetRgcff() {return Rgcff;}
  double GetMieff() {return Mieff;}
  double GetQeff() {return Qeff;}
  double GetTtsff() {return Ttsff;}
  double GetPMTSatur() {return PmtSatur;}//TD 2019.07.16
  // double GetQoiff() {return Qoiff;}//TD 2019.06.26
  double GetTvspacing() {return Tvspacing;}
  bool   GetTopveto() {return Topveto;}
  //WCSimPhysicsListFactory sets
  void SetPhysicsListName(string iPhysicsListName) {PhysicsListName = iPhysicsListName;}
  //WCSimPhysicsListFactory gets
  string GetPhysicsListName() {return PhysicsListName;}
  //WCSimPrimaryGeneratorAction sets
  void SetVectorFileName(string iVectorFileName) {VectorFileName = iVectorFileName;}
  void SetGeneratorType(string iGeneratorType) {GeneratorType = iGeneratorType;}
  //WCSimPrimaryGeneratorAction gets
  string GetVectorFileName() {return VectorFileName;}
  string GetGeneratorType() {return GeneratorType;}
  //WCSimRandomParameters sets
  void SetRandomSeed(int iRandomSeed) {RandomSeed = iRandomSeed;}
  void SetRandomGenerator(WCSimRandomGenerator_t iRandomGenerator) {RandomGenerator = iRandomGenerator;}
  //WCSimRandomParameters gets
  int                    GetRandomSeed() {return RandomSeed;}
  WCSimRandomGenerator_t GetRandomGenerator() {return RandomGenerator;}

private:
  //WCSimDetector*
  string DetectorName;
  bool   GeomHasOD;
  bool   SavePi0;
  int    PMTQEMethod;
  int    PMTCollEff;
  
  //WCSimWCAddDarkNoise
  map<string, WCSimDarkNoiseOptions> DarkOptMap;

  //WCSimWCDigitizer*
  string DigitizerClassName;
  int    DigitizerDeadTime; // ns
  int    DigitizerIntegrationWindow; // ns
  int    DigitizerTimingPrecision; // 
  int    DigitizerPEPrecision; // 

  //WCSimWCTrigger*
  string TriggerClassName;
  bool   MultiDigitsPerTrigger;
  //ndigits
  int    NDigitsThreshold; // digitized hits
  int    NDigitsWindow; // ns
  bool   NDigitsAdjustForNoise;
  int    NDigitsPreTriggerWindow; // ns
  int    NDigitsPostTriggerWindow; // ns
  //savefailures
  int    SaveFailuresMode;
  double SaveFailuresTime; // ns
  int    SaveFailuresPreTriggerWindow; // ns
  int    SaveFailuresPostTriggerWindow; // ns

  //WCSimTuningParameters
  double Rayff;
  double Bsrff;
  double Abwff;
  double Rgcff;
  double Qeff;
  double Mieff;
  double Ttsff;
  // double Qoiff; //TD 2019.6.26
  double PmtSatur; //TD 2019.07.16
  double Tvspacing;
  bool   Topveto;

  //WCSimPhysicsListFactory
  string PhysicsListName;

  //WCSimPrimaryGeneratorAction
  string VectorFileName;
  string GeneratorType;

  //WCSimRandomParameters
  int                    RandomSeed;
  WCSimRandomGenerator_t RandomGenerator;
  
  ClassDef(WCSimRootOptions,4)
};


//////////////////////////////////////////////////////////////////////////

#endif //WCSim_RootOptions
