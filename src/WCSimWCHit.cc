#include "WCSimWCHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4ParticleTypes.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4RotationMatrix.hh"
#include <iomanip>

G4int WCSimWCHit::maxPe = 0;

G4Allocator<WCSimWCHit> WCSimWCHitAllocator;

G4double numbpmthit=0.0;
G4double avePe=0.0;

WCSimWCHit::WCSimWCHit() 
{
  totalPe = 0;
}

WCSimWCHit::~WCSimWCHit() {}

WCSimWCHit::WCSimWCHit(const WCSimWCHit& right)
  : G4VHit()
{
  trackID   = right.trackID;
  tubeID   = right.tubeID;
  edep      = right.edep;
  pos       = right.pos;
}

const WCSimWCHit& WCSimWCHit::operator=(const WCSimWCHit& right)
{
  trackID   = right.trackID;
  tubeID   =  right.tubeID;
  edep      = right.edep;
  pos       = right.pos;
  return *this;
}

G4int WCSimWCHit::operator==(const WCSimWCHit& right) const
{
  return (this==&right) ? 1 : 0;
}

void WCSimWCHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Transform3D trans(rot, pos);
    G4VisAttributes attribs;

    G4String volumeName        = pLogV->GetName();

    //if ( volumeName == "glassFaceWCPMT" ||  volumeName == "glassFaceWCPMT_refl")  
    // volumeName should be compared to ID/OD CollectionName of the SensitiveDetector
    // instead of accessing those exactly here, just grab the substring: It should be "a" glassFaceWCPMT. Later optional check for OD?
    if ( volumeName.find("glassFaceWCPMT") != std::string::npos ||
	 volumeName.find("glassFaceWCPMT_refl") != std::string::npos) //isn't this deprecated??
    { 
    	//G4cout << "PE: " << totalPe << " Max Pe " << maxPe << G4endl;
		//numbpmthit++;
		//avePe += totalPe;
		//G4cout << "Ave PE: " << avePe/numbpmthit << " Max PE: " << maxPe << G4endl;
      if ( totalPe > maxPe*.01 )
      {      
	G4Colour colour(1.,1.-(float(totalPe-.05*maxPe)/float(.95*maxPe)),0.0);
	attribs.SetColour(colour);
	//    attribs.SetForceWireframe(false);
	attribs.SetForceSolid(true);
	pVVisManager->Draw(*pLogV,attribs,trans);
      }

    }
  }
}

void WCSimWCHit::Print()
{

  G4cout.setf(std::ios::fixed);
  G4cout.precision(1);

  G4cout << " Tube:"  << std::setw(4) << tubeID 
	 << " Track:" << std::setw(6) << trackID 
	 << " Pe:"    << totalPe
	 << " Pos:"   << pos/cm << G4endl
	 << "\tTime: "; 

  for (int i = 0; i < totalPe; i++) 
  {
    G4cout << time[i]/ns << " ";
    if ( i%10 == 0 && i != 0) 
      G4cout << G4endl << "\t";
  }
  G4cout << "size: " << time.size() << G4endl;
}

