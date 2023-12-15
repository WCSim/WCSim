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
  case (kTriggerTestVertices) :
    return "TestVertices";
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

std::string WCSimEnumerations::EnumAsString(WCSimRandomGenerator_t r)
{
  switch(r) {
  case (RANDOM_E_RANLUX) :
    return "RANLUX";
    break;
  case (RANDOM_E_RANECU) :
    return "RANECU";
    break;
  case (RANDOM_E_HEPJAMES) :
    return "HEPJAMES";
    break;
  default:
    return "";
    break;
  }
  return "";
}

std::string WCSimEnumerations::EnumAsString(BoundaryType_t b)
{
  switch(b) {
  case (kBlackSheet) :
    return "Blacksheet";
    break;
  case (kTyvek) :
    return "Tyvek";
    break;
  case (kCave) :
    return "Cave";
    break;
  default:
    return "";
    break;
  }
  return "";
}

std::string WCSimEnumerations::EnumAsString(ReflectionSurface_t r)
{
  switch(r) {
  case (kOtherS) :
    return "Others";
    break;
  case (kBlackSheetS) :
    return "Blacksheet";
    break;
  case (kReflectorS) :
    return "Reflector";
    break;
  case (kPhotocathodeS) :
    return "Photocathode";
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
