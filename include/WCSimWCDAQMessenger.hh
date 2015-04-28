#ifndef WCSimWCDAQMessenger_h
#define WCSimWCDAQMessenger_h 1

#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"

class G4UIcommand;
class WCSimEventAction;
class WCSimWCDigitizerBase;
class WCSimWCTriggerBase;

class WCSimWCDAQMessenger: public G4UImessenger
{
public:
  WCSimWCDAQMessenger(WCSimEventAction *);
  WCSimWCDAQMessenger(WCSimWCDigitizerBase*);
  WCSimWCDAQMessenger(WCSimWCTriggerBase*);

  ~WCSimWCDAQMessenger();

  void SetNewValue(G4UIcommand* command, G4String newValue);

private:
  WCSimEventAction*   WCSimEvent;
  WCSimWCDigitizerBase* WCSimDigitize;
  WCSimWCTriggerBase*   WCSimTrigger;

  G4UIdirectory*      WCSimDAQDir;
  G4UIcmdWithAString* DigitizerChoice;
  G4UIcmdWithAString* TriggerChoice;

  G4int constructor;
};

#endif
