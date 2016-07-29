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

class WCSimEnumerations
{
public:

  static std::string EnumAsString(DigitizerType_t d);
  static std::string EnumAsString(TriggerType_t t);
  static TriggerType_t TriggerTypeFromString(std::string s);

};

#endif
