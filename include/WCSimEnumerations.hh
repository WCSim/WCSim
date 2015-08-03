#ifndef WCSimEnumerations_h
#define WCSimEnumerations_h 1

#include <string>

typedef enum ETriggerType {
  kTriggerUndefined = -1,
  kTriggerNHits,
  kTriggerNHitsSKDETSIM,
  kTriggerNHitsTest,
  kTriggerFailure // this should always be the last entry (for looping)
} TriggerType_t;

class WCSimEnumerations
{
public:

  static std::string EnumAsString(TriggerType_t t)
  {
    switch(t) {
    case (kTriggerNHits) :
      return "NHits";
      break;
    case (kTriggerNHitsSKDETSIM) :
      return "NHits_SKDETSIM";
      break;
    case (kTriggerNHitsTest) :
      return "NHits_TEST";
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
