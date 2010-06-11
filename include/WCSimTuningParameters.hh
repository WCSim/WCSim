#ifndef WCSimTuningParameters_h
#define WCSimTuningParameters_h 1
#include "WCSimTuningMessenger.hh"
#include "globals.hh"

class WCSimTuningParameters
{
  public:
    WCSimTuningParameters();
    ~WCSimTuningParameters();


  // Setters and getters
  G4double GetRayff() {return rayff;}
  void SetRayff(G4double rparam) {rayff=rparam;}

  private:

  // The messenger
  WCSimTuningMessenger* TuningMessenger;


  // The parameters that need to be set before WCSimDetectorConstruction
  // is created

  G4double rayff;

};

#endif







