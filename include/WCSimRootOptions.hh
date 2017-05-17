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

#include "WCSimEnumerations.hh"

class TDirectory;
using std::string;

//////////////////////////////////////////////////////////////////////////

class WCSimRootOptions : public TObject {

public:

  WCSimRootOptions();
  virtual ~WCSimRootOptions();
  void Print(Option_t *option = "") const;

  //WCSimDetector* gets
  void SetDetectorName(string iDetectorName) {DetectorName = iDetectorName;}
  void SetSavePi0(bool iSavePi0) {SavePi0 = iSavePi0;}
  void SetPMTQEMethod(bool iPMTQEMethod) {PMTQEMethod = iPMTQEMethod;}
  void SetPMTCollEff(bool iPMTCollEff) {PMTCollEff = iPMTCollEff;}
  //WCSimDetector* sets
  string GetDetectorName() {return DetectorName;}
  bool   GetSavePi0() {return SavePi0;}
  int    GetPMTQEMethod() {return PMTQEMethod;}
  int    GetPMTCollEff() {return PMTCollEff;}
  //WCSimWCAddDarkNoise sets
  void SetPMTDarkRate(double iPMTDarkRate) {PMTDarkRate = iPMTDarkRate;}
  void SetConvRate(double iConvRate) {ConvRate = iConvRate;}
  void SetDarkHigh(double iDarkHigh) {DarkHigh = iDarkHigh;}
  void SetDarkLow(double iDarkLow) {DarkLow = iDarkLow;}
  void SetDarkWindow(double iDarkWindow) {DarkWindow = iDarkWindow;}
  void SetDarkMode(int iDarkMode) {DarkMode = iDarkMode;}
  //WCSimWCAddDarkNoise gets
  double GetPMTDarkRate() {return PMTDarkRate;}
  double GetConvRate() {return ConvRate;}
  double GetDarkHigh() {return DarkHigh;}
  double GetDarkLow() {return DarkLow;}
  double GetDarkWindow() {return DarkWindow;}
  int    GetDarkMode() {return DarkMode;}
  //WCSimWCDigitizer* sets
  void SetDigitizerClassName(string iDigitizerClassName) {DigitizerClassName = iDigitizerClassName;}
  void SetDigitizerDeadTime(int iDigitizerDeadTime) {DigitizerDeadTime = iDigitizerDeadTime;}
  void SetDigitizerIntegrationWindow(int iDigitizerIntegrationWindow) {DigitizerIntegrationWindow = iDigitizerIntegrationWindow;}
  //WCSimWCDigitizer* gets
  string GetDigitizerClassName() {return DigitizerClassName;}
  int    GetDigitizerDeadTime() {return DigitizerDeadTime;}
  int    GetDigitizerIntegrationWindow() {return DigitizerIntegrationWindow;}
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
  void SetTvspacing(double iTvspacing) {Tvspacing = iTvspacing;}
  void SetTopveto(bool iTopveto) {Topveto = iTopveto;}
  //WCSimTuningParameters gets
  double GetRayff() {return Rayff;}
  double GetBsrff() {return Bsrff;}
  double GetAbwff() {return Abwff;}
  double GetRgcff() {return Rgcff;}
  double GetMieff() {return Mieff;}
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
  bool   SavePi0;
  int    PMTQEMethod;
  int    PMTCollEff;
  
  //WCSimWCAddDarkNoise
  double PMTDarkRate; // kHz
  double ConvRate; // kHz
  double DarkHigh; // ns
  double DarkLow; // ns
  double DarkWindow; // ns
  int    DarkMode;

  //WCSimWCDigitizer*
  string DigitizerClassName;
  int    DigitizerDeadTime; // ns
  int    DigitizerIntegrationWindow; // ns

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
  double Mieff;
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
  
  ClassDef(WCSimRootOptions,1)  //WCSimRootEvent structure
};


#endif
