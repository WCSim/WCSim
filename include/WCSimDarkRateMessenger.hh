#ifndef WCSimDarkRateMessenger_h
#define WCSimDarkRateMessenger_h 1


#include "G4UImessenger.hh"
#include <map>
#include "G4UIcmdWithAString.hh"

class G4UIcommand;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class WCSimWCAddDarkNoise;

class WCSimDarkRateMessenger: public G4UImessenger
{
public:
  static WCSimDarkRateMessenger* iInstance;
  void Initialize();

  static WCSimDarkRateMessenger* GetInstance();
  void AddDarkRateInstance(WCSimWCAddDarkNoise* darkratepoint, G4String detectorElement);
  void RemoveDarkRateInstance(G4String detectorElement);
  ~WCSimDarkRateMessenger();

  void SetNewValue(G4UIcommand* command, G4String newValue);

private:
  WCSimDarkRateMessenger(WCSimWCAddDarkNoise*);
  WCSimWCAddDarkNoise* WCSimAddDarkNoise;
  std::map<std::string, WCSimWCAddDarkNoise*> darknoiseptrs;

  G4UIdirectory* WCSimDir;
  G4UIcmdWithADoubleAndUnit* SetFrequency;
  G4UIcmdWithADouble* SetConversionRate;
  G4UIcmdWithAnInteger* SetDarkMode;
  G4UIcmdWithADoubleAndUnit* SetDarkLow;
  G4UIcmdWithADoubleAndUnit* SetDarkHigh;
  G4UIcmdWithADoubleAndUnit* SetDarkWindow;
  G4UIcmdWithAString* SetDetectorElement;
  
  G4String initaliseString;
  G4String detectorElement;

};

#endif
