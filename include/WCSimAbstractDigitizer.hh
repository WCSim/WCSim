#ifndef WCSimAbstractDigitizer_h
#define WCSimAbstractDigitizer_h 1

#include "G4VDigitizerModule.hh"

// An abstract base class that the two digitizer implementations can subclass
// allowing them to be integerchangeable in the event action
class WCSimAbstractDigitizer : public G4VDigitizerModule {
  
public:
  WCSimAbstractDigitizer(G4String name) : G4VDigitizerModule(name) { };
  virtual void Digitize() = 0; // From G4DigitizerModule
  
  // These two used in the existing event action
  virtual int NumberOfGatesInThisEvent() = 0;
  virtual G4double GetTriggerTime(int i) = 0;
};

#endif