#ifndef WCSimEnumerations_h
#define WCSimEnumerations_h 1

#include <string>

typedef enum ETriggerType {
  kTriggerUndefined = -1,
  kTriggerNHits,
  kTriggerNHitsSKDETSIM
} TriggerType_t;

static std::string EnumAsString(TriggerType_t t)
{
  switch(t) {
    case (kTriggerNHits) :
      return "NHits";
      break;
    case (kTriggerNHitsSKDETSIM) :
      return "NHits_SKDETSIM";
      break;
    default:
      return "";
      break;
  }
  return "";
}

#endif
