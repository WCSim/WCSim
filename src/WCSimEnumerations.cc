#include <string>
#include <iostream>
#include "G4ios.hh"

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
 
std::string WCSimEnumerations::EnumAsString(ProcessType_t p)
{
  switch(p) {
  case (kDarkNoise) :
    return "darkNoise";
    break;
  case (kHadElastic) :
    return "hadronElastic";
    break;
  case (kNCapture) :
    return "neutronCapture";
    break;
  case (kCompt) :
    return "Compton";
    break;
  case (kPhot) :
    return "Photoelectric";
    break;
  case (kCCerenkov) :
    return "Cherenkov";
    break;
  case (kSScintillation) :
    return "Scintillation";
    break;
  case (kEBrem) :
    return "eBremsstrahlung";
    break;
  case (kAnnihil) :
    return "Annihilation";
    break;
  case (kConv) :
    return "Conversion";
    break;
  case (kEIoni) :
    return "eIonisation";
    break;
  case (kNeutronInelastic) :
    return "neutronInelastic";
    break;
  default: 
    return "";
    break;
  }
  return "";
}

ProcessType_t WCSimEnumerations::ProcessTypeStringToEnum(const std::string& str) {
  if (str == "darkNoise") {
    return kDarkNoise;
  } else if (str == "hadElastic") {
    return kHadElastic;
  } else if (str == "nCapture") {
    return kNCapture;
  } else if (str == "compt") {
    return kCompt;
  } else if (str == "phot") {
    return kPhot;
  } else if (str == "Cerenkov") {
    return kCCerenkov;
  } else if (str == "Scintillation") {
    return kSScintillation;
  } else if (str == "eBrem") {
    return kEBrem;
  } else if (str == "annihil") {
    return kAnnihil;
  } else if (str == "conv") {
    return kConv;
  } else if (str == "eIoni") {
    return kEIoni;
  } else if (str == "neutronInelastic") {
    return kNeutronInelastic;
  } else {
    // Default value in case of unknown string
    return kUnknownProcess;
  }
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
