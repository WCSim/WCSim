#ifndef WCSimEnumerations_h
#define WCSimEnumerations_h 1

#include <string>

typedef enum ETriggerType {
  kTriggerUndefined = -1,
  kTriggerNDigits,
  kTriggerNHitsSKDETSIM,
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

  static std::string EnumAsString(DigitizerType_t d)
  {
    switch(d) {
    case (kDigitizerSKI) :
      return "SKI";
      break;
    default:
      return "";
      break;
    }
    return "";
  }

  static std::string EnumAsString(TriggerType_t t)
  {
    switch(t) {
    case (kTriggerNDigits) :
      return "NDigits";
      break;
    case (kTriggerNHitsSKDETSIM) :
      return "NHits_SKDETSIM";
      break;
    case (kTriggerNDigitsTest) :
      return "NDigits_TEST";
      break;
    case (kTriggerFailure) :
      return "No_trigger_passed";
      break;
    default:
      return "";
      break;
    }
    return "";
  }

};

#endif
