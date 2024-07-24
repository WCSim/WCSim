#ifndef WCSimEnumerations_h
#define WCSimEnumerations_h 1

#include <string>

typedef enum ETriggerType {
  kTriggerUndefined = -1,
  kTriggerNDigits,
  kTriggerNDigitsTest,
  kTriggerTestVertices,
  kTriggerNoTrig,
  kTriggerFailure // this should always be the last entry (for looping)
} TriggerType_t;

typedef enum EDigitizerType {
  kDigitizerUndefined = -1,
  kDigitizerSKI
} DigitizerType_t;

typedef enum EInteractionMode {
  UNKNOWN = -1,
  BEAM = 0,
  COSMICS = 10,
  PARTICLEGUN = 20,
  LASER = 30
} InteractionType_t;

typedef enum ERandomGeneratorType {
  RANDOM_E_RANLUX=1,
  RANDOM_E_RANECU=2,
  RANDOM_E_HEPJAMES=3
} WCSimRandomGenerator_t;

typedef enum EBoundaryType {
  kNull=0,
  kBlackSheet,
  kMPMT,
  kTyvek,
  kCave
} BoundaryType_t;

typedef enum EReflectionSurface {
  kOtherS=0,
  kBlackSheetS,
  kReflectorS,
  kPhotocathodeS
} ReflectionSurface_t;

typedef enum EProcessType { // Diego Costas 27/02/2024
  kUnknownProcess,
  kDarkNoise,
  kHadElastic,
  kNCapture,
  kCompt,
  kPhot,
  kCCerenkov,
  kSScintillation,
  kEBrem,
  kAnnihil,
  kConv,
  kEIoni,
  kNeutronInelastic
} ProcessType_t;

class WCSimEnumerations
{
public:

  static std::string EnumAsString(DigitizerType_t d);
  static std::string EnumAsString(TriggerType_t t);
  static std::string EnumAsString(WCSimRandomGenerator_t r);
  static std::string EnumAsString(BoundaryType_t b);
  static std::string EnumAsString(ReflectionSurface_t r);
  static std::string EnumAsString(ProcessType_t p);
  static ProcessType_t ProcessTypeStringToEnum(const std::string& str);
  static TriggerType_t TriggerTypeFromString(std::string s);

};

#endif
