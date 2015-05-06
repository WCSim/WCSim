#ifndef WCSimEnumerations_h
#define WCSimEnumerations_h 1

typedef enum ETriggerType {
  kTriggerUndefined = -1,
  kTriggerNHits,
  kTriggerNHitsSKDETSIM
} TriggerType_t;

string AsString(TriggerType_t t)
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
