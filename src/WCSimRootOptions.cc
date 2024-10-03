////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include <iostream>

#include "WCSimRootOptions.hh"

#ifndef REFLEX_DICTIONARY
ClassImp(WCSimRootOptions)
#endif

using std::endl;
using std::cout;

//______________________________________________________________________________
WCSimRootOptions::WCSimRootOptions()
{
  // Create a WCSimRootOptions object.

}

//______________________________________________________________________________
WCSimRootOptions::~WCSimRootOptions()
{
}

//______________________________________________________________________________
void WCSimRootOptions::Print(Option_t *) const
{
  cout
    << "Detector construction:" << endl
    << "\tDetectorName: " << DetectorName << endl
    << "\tDetectorHasOD: " << (GeomHasOD ? "yes" : "no") << endl
    << "\tSavePi0: " << SavePi0 << endl
    << "\tPMTQEMethod: " << PMTQEMethod << endl
    << "\tPMTCollEff: " << PMTCollEff << endl

    << "Dark Noise options:" << endl;
  for(std::map<string, WCSimDarkNoiseOptions>::const_iterator it=DarkOptMap.begin(); it != DarkOptMap.end(); ++it) {
    cout
      << "\t" << it->first << endl
      << "\t\tPMTDarkRate: " << it->second.PMTDarkRate << " kHz" << endl
      << "\t\tConvRate: " << it->second.ConvRate << " kHz" << endl
      << "\t\tDarkHigh: " << it->second.DarkHigh << " ns" << endl
      << "\t\tDarkLow: " << it->second.DarkLow << " ns" << endl
      << "\t\tDarkWindow: " << it->second.DarkWindow << " ns" << endl
      << "\t\tDarkMode: " << it->second.DarkMode << endl;
    }

    cout << "Digitizer options:" << endl
	 << "\tDigitizerClassName: " << DigitizerClassName << endl
	 << "\tDigitizerDeadTime: " << DigitizerDeadTime << " ns" << endl
	 << "\tDigitizerIntegrationWindow: " << DigitizerIntegrationWindow << " ns" << endl
	 << "\tDigitizerTimingPrecision: " << DigitizerTimingPrecision << " ns" << endl
	 << "\tDigitizerPEPrecision: " << DigitizerPEPrecision << " ns" << endl
      
	 << "Trigger options:" << endl
	 << "\tTriggerClassName: " << TriggerClassName << endl
	 << "\tTriggerOffset: " << TriggerOffset << " ns" << endl
	 << "\tMultiDigitsPerTrigger: " << MultiDigitsPerTrigger << endl
	 << "NDigits-style trigger options:" << endl
	 << "\tNDigitsThreshold: " << NDigitsThreshold << " digitized hits" << endl
	 << "\tNDigitsWindow: " << NDigitsWindow << " ns" << endl
	 << "\tNDigitsAdjustForNoise: " << NDigitsAdjustForNoise << endl
	 << "\tNDigitsPreTriggerWindow: " << NDigitsPreTriggerWindow << " ns" << endl
	 << "\tNDigitsPostTriggerWindow: " << NDigitsPostTriggerWindow << " ns" << endl
	 << "Save failures trigger options:" << endl
	 << "\tSaveFailuresMode: " << SaveFailuresMode << endl
	 << "\tSaveFailuresTime: " << SaveFailuresTime << " ns" << endl
	 << "\tSaveFailuresPreTriggerWindow: " << SaveFailuresPreTriggerWindow << " ns" << endl
	 << "\tSaveFailuresPostTriggerWindow: " << SaveFailuresPostTriggerWindow << " ns" << endl
      
	 << "Tuning parameters:" << endl
	 << "\tRayff: " << Rayff << endl
	 << "\tBsrff: " << Bsrff << endl
	 << "\tAbwff: " << Abwff << endl
	 << "\tRgcff: " << Rgcff << endl
	 << "\tQeff: " << Qeff << endl
	 << "\tMieff: " << Mieff << endl
	 << "\tTtsff: " << Ttsff << endl
      //<< "\tQoiff: " << Qoiff << endl
	 << "\tPMTSatur:" << PmtSatur << endl
	 << "\tTvspacing: " << Tvspacing << endl
	 << "\tTopveto: " << Topveto << endl

	 << "Physics List Factory:" << endl
	 << "\tPhysicsListName: " << PhysicsListName << endl

	 << "WCSimPrimaryGeneratorAction" << endl
	 << "\tVectorFileName: " << VectorFileName << endl
	 << "\tGeneratorType: " << GeneratorType << endl

	 << "WCSimRandomParameters" << endl
 	 << "\tRandomSeed: " << RandomSeed << endl
	 << "\tRandomGenerator: " << WCSimEnumerations::EnumAsString(RandomGenerator) << endl

	 << endl;
}
 
//______________________________________________________________________________
bool WCSimRootOptions::IsValidDarkTag(string tag) const
{
  if(DarkOptMap.find(tag) != DarkOptMap.end())
      return true;
  return false; 
}
//______________________________________________________________________________
double WCSimRootOptions::GetPMTDarkRate(string tag)
{
  if(IsValidDarkTag(tag))
    return DarkOptMap[tag].PMTDarkRate;
  std::cerr << "WCSimDarkNoiseOptions with tag: " << tag << " does not exist in WCSimRootOptions. Returning -999" << std::endl;
  return -999;
}
//______________________________________________________________________________
double WCSimRootOptions::GetConvRate(string tag)
{
  if(IsValidDarkTag(tag))
    return DarkOptMap[tag].ConvRate;
  std::cerr << "WCSimDarkNoiseOptions with tag: " << tag << " does not exist in WCSimRootOptions. Returning -999" << std::endl;
  return -999;
}
//______________________________________________________________________________
double WCSimRootOptions::GetDarkHigh(string tag)
{
  if(IsValidDarkTag(tag))
    return DarkOptMap[tag].DarkHigh;
  std::cerr << "WCSimDarkNoiseOptions with tag: " << tag << " does not exist in WCSimRootOptions. Returning -999" << std::endl;
  return -999;
}
//______________________________________________________________________________
double WCSimRootOptions::GetDarkLow(string tag)
{
  if(IsValidDarkTag(tag))
    return DarkOptMap[tag].DarkLow;
  std::cerr << "WCSimDarkNoiseOptions with tag: " << tag << " does not exist in WCSimRootOptions. Returning -999" << std::endl;
  return -999;
}
//______________________________________________________________________________
double WCSimRootOptions::GetDarkWindow(string tag)
{
  if(IsValidDarkTag(tag))
    return DarkOptMap[tag].DarkWindow;
  std::cerr << "WCSimDarkNoiseOptions with tag: " << tag << " does not exist in WCSimRootOptions. Returning -999" << std::endl;
  return -999;
}
//______________________________________________________________________________
int    WCSimRootOptions::GetDarkMode(string tag)
{
  if(IsValidDarkTag(tag))
    return DarkOptMap[tag].DarkMode;
  std::cerr << "WCSimDarkNoiseOptions with tag: " << tag << " does not exist in WCSimRootOptions. Returning -999" << std::endl;
  return -999;
}
//______________________________________________________________________________

