#ifndef WCSimWCDAQMessenger_h
#define WCSimWCDAQMessenger_h 1


#include "G4UImessenger.hh"


class G4UIcommand;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class WCSimWCDigitizerBase;
class WCSimWCTriggerBase;
class WCSimWCEventAction;

class WCSimWCDAQMessenger: public G4UImessenger
{
public:
  WCSimWCDAQMessenger(WCSimWCDigitizerBase*);
  WCSimWCDAQMessenger(WCSimWCTriggerBase*);
  WCSimWCDAQMessenger(WCSimWCEventAction*);

  ~WCSimWCDAQMessenger();

  void SetNewValue(G4UIcommand* command, G4String newValue);

private:
  WCSimWCEventAction*   WCSimEvent;
  WCSimWCDigitizerBase* WCSimDigitize;
  WCSimWCTriggerBase*   WCSimTrigger;

  G4UIdirectory*      WCSimDAQDir;
  G4UIcmdWithAString* DigitizerChoice;
  G4UIcmdWithAString* TriggerChoice;

  G4int constructor;
};

#endif
