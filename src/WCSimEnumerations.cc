#include <string>
#include <iostream>

#include "WCSimEnumerations.hh"

std::string WCSimEnumerations::EnumAsString(DigitizerType_t d)
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

std::string WCSimEnumerations::EnumAsString(TriggerType_t t)
{
  switch(t) {
  case (kTriggerNDigits) :
    return "NDigits";
    break;
  case (kTriggerNDigitsTest) :
    return "NDigits_TEST";
    break;
  case (kTriggerFailure) :
    return "No_trigger_passed";
    break;
  case (kTriggerNoTrig) :
    return "NoTrigger";
    break;
  default:
    return "";
    break;
    }
  return "";
}

TriggerType_t WCSimEnumerations::TriggerTypeFromString(std::string s)
{
  for(int i = int(kTriggerUndefined)+1; i <= kTriggerFailure; i++) {
    if(s.compare(WCSimEnumerations::EnumAsString((TriggerType_t)i)) == 0) {
      return (TriggerType_t)i;
    }
  }
  std::cerr << "WCSimEnumerations::TriggerTypeFromString() Unknown string value " << s << std::endl;
  return kTriggerUndefined;
}
