#ifndef WCSimEnumerations_h
#define WCSimEnumerations_h 1

#include <string>

typedef enum ETriggerType {
  kTriggerUndefined = -1,
  kTriggerNDigits,
  kTriggerNDigitsTest,
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

class WCSimEnumerations
{
public:

  static std::string EnumAsString(DigitizerType_t d);
  static std::string EnumAsString(TriggerType_t t);
  static std::string EnumAsString(WCSimRandomGenerator_t r);
  static TriggerType_t TriggerTypeFromString(std::string s);

};

#endif
