#ifndef WCSimWCDAQMessenger_h
#define WCSimWCDAQMessenger_h 1

#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIdirectory.hh"

class G4UIcommand;
class WCSimEventAction;
class WCSimWCDigitizerBase;
class WCSimWCTriggerBase;

class WCSimWCDAQMessenger: public G4UImessenger
{
public:
  WCSimWCDAQMessenger();

  ~WCSimWCDAQMessenger();

  void SetNewValue(G4UIcommand* command, G4String newValue);

  void SetEventActionOptions();
  void SetTriggerOptions();
  void SetDigitizerOptions();

  void TellMeAboutTheDigitizer  (WCSimWCDigitizerBase* digitizer)   { WCSimDigitize = digitizer; }
  void TellMeAboutTheTrigger    (WCSimWCTriggerBase*   trigger)     { WCSimTrigger  = trigger; }
  void TellMeAboutTheEventAction(WCSimEventAction*     eventaction) { WCSimEvent    = eventaction; }

private:
  WCSimEventAction*     WCSimEvent;
  WCSimWCDigitizerBase* WCSimDigitize;
  WCSimWCTriggerBase*   WCSimTrigger;

  G4UIdirectory*      WCSimDAQDir;
  G4UIcmdWithAString* DigitizerChoice;
  G4String            StoreDigitizerChoice;
  G4UIcmdWithAString* TriggerChoice;
  G4String            StoreTriggerChoice;

  G4UIdirectory*        NHitsTriggerDir;
  G4UIcmdWithAnInteger* NHitsTriggerThreshold;
  G4int                 StoreSetNHitsThreshold;
  G4UIcmdWithAnInteger* NHitsTriggerWindow;
  G4int                 StoreSetNHitsWindow;

};

#endif
