#ifndef WCSimDarkRateMessenger_h
#define WCSimDarkRateMessenger_h 1


#include "G4UImessenger.hh"


class G4UIcommand;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class WCSimWCDigitizer;

class WCSimDarkRateMessenger: public G4UImessenger
{
public:
  WCSimDarkRateMessenger(WCSimWCDigitizer*);
  ~WCSimDarkRateMessenger();

  void SetNewValue(G4UIcommand* command, G4String newValue);

private:
  WCSimWCDigitizer* WCSimDigitize;


  G4UIdirectory* WCSimDir;
  G4UIcmdWithADoubleAndUnit* SetFrequency;
  G4UIcmdWithADouble* SetConversionRate;

};

#endif
