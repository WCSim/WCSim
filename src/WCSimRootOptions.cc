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
    << "Dark Noise options" << endl
    << "\tPMTDarkRate: " << PMTDarkRate << " kHz" << endl
    << "\tConvRate: " << ConvRate << " kHz" << endl
    << "\tDarkHigh: " << DarkHigh << " ns" << endl
    << "\tDarkLow: " << DarkLow << " ns" << endl
    << "\tDarkWindow: " << DarkWindow << " ns" << endl
    << "\tDarkMode: " << DarkMode << endl
    << endl;
}
