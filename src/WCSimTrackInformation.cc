#include "WCSimTrackInformation.hh"
#include "G4ios.hh"

G4Allocator<WCSimTrackInformation> aWCSimTrackInfoAllocator;

WCSimTrackInformation::WCSimTrackInformation(const G4Track* atrack)
{
  saveit = true;
}

void WCSimTrackInformation::Print() const
{
  G4cout << "WCSimTrackInformation : " << saveit << G4endl;
}
