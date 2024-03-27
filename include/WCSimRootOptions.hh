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

#include "WCSimEnumerations.hh"

class TDirectory;
using std::string;
using std::map;

//////////////////////////////////////////////////////////////////////////

/**
 * \struct WCSimDarkNoiseOptions
 * Class to hold information about the chosen dark noise options.
 * There will be one copy of this class for each active PMT type in the output file.
 */
struct WCSimDarkNoiseOptions {
  double PMTDarkRate; //!< Dark rate for this PMT type (unit: kHz)
  double ConvRate; //!< Conversion factor used to ensure that the dark rate is as requested. More true dark hits are generated, such that the final digitised dark noise rate is correct (hits are reduced due to thresholds, integration, etc. in the PMT & electronics simulation)
  double DarkHigh; //!< In dark mode 0, the end time of the fixed dark noise window (unit: ns)
  double DarkLow; //!< In dark mode 0, the start time of the fixed dark noise window (unit: ns)
  double DarkWindow; //!< In dark mode 1, dark hits are added to a window of size ±DarkWindow/2 around true physics hits (unit: ns)
  int    DarkMode;  //!< Mode 0: add dark noise in a fixed window. Mode 1: add dark noise around true physics hits
  WCSimDarkNoiseOptions() :
    PMTDarkRate(-999), ConvRate(-999), DarkHigh(-999), DarkLow(-999),
    DarkWindow(-999), DarkMode(-999)
  {}
};

//////////////////////////////////////////////////////////////////////////

/** 
 * \class WCSimRootOptions
 * List of WCSim running options.
 * Note that not all options are currently included in this class
 */
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
  void SetTriggerOffset(double value) {TriggerOffset = value;};
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
  double GetTriggerOffset() {return TriggerOffset;}
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
  string DetectorName; //!< Name of the detector geometry
  bool   GeomHasOD; //!< Does the detector contain active OD PMTs?
  bool   SavePi0; //!< Is more information about Pi0 decays saved?
  int    PMTQEMethod; //!< 
  int    PMTCollEff; //!< Is the PMT collection efficiency turned on?
  
  //WCSimWCAddDarkNoise
  map<string, WCSimDarkNoiseOptions> DarkOptMap; //!< Maps active PMT types (string) with information on that PMT types' dark noise running options (WCSimDarkNoiseOptions)

  //WCSimWCDigitizer*
  string DigitizerClassName; //!< The active digitiser
  int    DigitizerDeadTime; //!< The digitiser dead time (digitiser cannot integrate hits for this duration after a hit) (unit: ns)
  int    DigitizerIntegrationWindow; //!< The digitiser integration time (unit: ns)
  int    DigitizerTimingPrecision; //!< The digitiser timing precision. Hit times are truncated to this level (unit: ns)
  int    DigitizerPEPrecision; //!< The digitiser charge precision. Hit charges are truncated to this level (unit: same as digitised charge (p.e.?))

  //WCSimWCTrigger*
  string TriggerClassName; //!< The active trigger
  bool   MultiDigitsPerTrigger; //!< Are multiple digits from the same PMT allowed to be read out in the same trigger readout window?
  double TriggerOffset; //!< Triggered digit time = digit time - trigger time + this offset (unit: ns)
  //ndigits
  int    NDigitsThreshold; //!< For the NDigits trigger, how many digitised hits in the trigger decision window will make the trigger fire?
  int    NDigitsWindow; //!< For the NDigits trigger, what is the length of the sliding trigger decision window? (unit: ns)
  bool   NDigitsAdjustForNoise; //!< Should the NDigits threshold be automatically increased based on the average dark noise contribution in the trigger decision window?
  int    NDigitsPreTriggerWindow; //!< For a positive trigger, how long before the issued trigger time should be read out? (unit: ns)
  int    NDigitsPostTriggerWindow; //!< For a positive trigger, how long after the issued trigger time should be read out? (unit: ns)
  //savefailures
  int    SaveFailuresMode; //!< 0: Digits that are part of a trigger readout window are saved, those that aren't are lost. 1: Both are saved. 2: Only triggers that are not part of a trigger readout window (but are in the special SaveFailures trigger window) are saved.
  double SaveFailuresTime; //!< Trigger time for the special SaveFailures trigger (unit: ns)
  int    SaveFailuresPreTriggerWindow; //!< How long before the special SaveFailures trigger time should digitised hits not associated with a real trigger be read out? (unit: ns)
  int    SaveFailuresPostTriggerWindow; //!< How long after the special SaveFailures trigger time should digitised hits not associated with a real trigger be read out? (unit: ns)

  //WCSimTuningParameters
  double Rayff; //!< Rayleigh scattering parameter
  double Bsrff; //!< Blacksheet reflection parameter
  double Abwff; //!< Water attenuation parameter
  double Rgcff; //!< Cathode reflectivity parameter
  double Qeff;  //!< Correction to cathode quantum efficiency parameter
  double Mieff; //!< Mie scattering parameter
  double Ttsff; //!< Transit time smearing for PMTs
  // double Qoiff; //!< Uncertainty on amount of charge created in a PMT
  double PmtSatur; //!< p.e. threshold where saturation starts to occur
  double Tvspacing; //!< Top veto PMT spacing
  bool   Topveto; //!< Is the top veto defined in the simulation?

  //WCSimPhysicsListFactory
  string PhysicsListName; //!< The active physics list

  //WCSimPrimaryGeneratorAction
  string VectorFileName; //!< The name of the input vector file (used for some generators e.g. muline, rootracker, datatable, ...)
  string GeneratorType; //!< The active generator type

  //WCSimRandomParameters
  int                    RandomSeed; //!< The initial seed for one of the random number generators
  WCSimRandomGenerator_t RandomGenerator; //!< The random number generator type
  
  ClassDef(WCSimRootOptions,5)
};


//////////////////////////////////////////////////////////////////////////

#endif //WCSim_RootOptions
