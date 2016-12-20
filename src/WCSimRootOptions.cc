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
void WCSimRootOptions::Print()
{
  cout
    << "Dark Noise options:" << endl
    << "\tPMTDarkRate: " << PMTDarkRate << " kHz" << endl
    << "\tConvRate: " << ConvRate << " kHz" << endl
    << "\tDarkHigh: " << DarkHigh << " ns" << endl
    << "\tDarkLow: " << DarkLow << " ns" << endl
    << "\tDarkWindow: " << DarkWindow << " ns" << endl
    << "\tDarkMode: " << DarkMode << endl
    << "Digitizer options:" << endl
    << "\tDigitizerClassName: " << DigitizerClassName << endl
    << "\tDigitizerDeadTime: " << DigitizerDeadTime << " ns" << endl
    << "\tDigitizerIntegrationWindow: " << DigitizerIntegrationWindow << " ns" << endl
    << "Trigger options:" << endl
    << "\tTriggerClassName: " << TriggerClassName << endl
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
    << endl;
}
