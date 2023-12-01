#include "WCSimWCHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4ParticleTypes.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4RotationMatrix.hh"
#include <iomanip>

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

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
  tubeType = right.tubeType;
}

const WCSimWCHit& WCSimWCHit::operator=(const WCSimWCHit& right)
{
  trackID   = right.trackID;
  tubeID   =  right.tubeID;
  edep      = right.edep;
  pos       = right.pos;
  tubeType = right.tubeType;

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

    // volumeName should be compared to ID/OD CollectionName of the SensitiveDetector
    // instead of accessing those exactly here, just grab the substring: It should be "a" glassFaceWCPMT. Later optional check for OD?
    if ( volumeName.find("glassFaceWCPMT") != std::string::npos ||
	 volumeName.find("glassFaceWCPMT2") != std::string::npos || 
	 volumeName.find("glassFaceWCPMT_refl") != std::string::npos) //isn't this deprecated??
    { 

    	//G4cout << "PE: " << totalPe << " Max Pe " << maxPe << G4endl;
		//numbpmthit++;
		//avePe += totalPe;
		//G4cout << "Ave PE: " << avePe/numbpmthit << " Max PE: " << maxPe << G4endl;
      if ( totalPe > maxPe*.01 )
      {      
	//Don't like this colour scheme (not enough visual gradient between yellow and red)
	//G4Colour colour(1.,1.-(double(totalPe-.05*maxPe)/double(.95*maxPe)),0.0);

	// Scale the charge using the HSV or HSL colour scheme
	// We want to only vary hue, setting saturation to 1 and value to 1 (or lightness to .5)
	double hue = (1.-(double(totalPe-.05*maxPe)/double(.95*maxPe)))*230;   //go from hue = 230 to 0
	double saturation = 1.;
	double value = 1.;

	//convert to rgb
	double red = 0.;
	double green = 0.;
	double blue = 0.;
	HSVtoRGB(red, green, blue, hue, saturation, value);
	G4Colour colour(red, green, blue);
	
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
	 << " Tube type:"  << tubeType 
	 << " Track:" << std::setw(6); 
  for (int i = 0; i < totalPe; i++) 
  {
    G4cout << trackID[i] << " ";
    if ( i%10 == 0 && i != 0) 
      G4cout << G4endl << "\t";
  }
  G4cout
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

/*! \brief Convert HSV to RGB color space (from https://gist.github.com/fairlight1337/4935ae72bcbcc1ba5c72)
  
  Converts a given set of HSV values `h', `s', `v' into RGB
  coordinates. The output RGB values are in the range [0, 1], and
  the input HSV values are in the ranges h = [0, 360], and s, v =
  [0, 1], respectively.
  
  \param fR Red component, used as output, range: [0, 1]
  \param fG Green component, used as output, range: [0, 1]
  \param fB Blue component, used as output, range: [0, 1]
  \param fH Hue component, used as input, range: [0, 360]
  \param fS Saturation component, used as input, range: [0, 1]
  \param fV Value component, used as input, range: [0, 1]
  
*/
void WCSimWCHit::HSVtoRGB(double& fR, double& fG, double& fB, double& fH, double& fS, double& fV) {
  double fC = fV * fS; // Chroma
  double fHPrime = fmod(fH / 60.0, 6);
  double fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
  double fM = fV - fC;
  
  if(0 <= fHPrime && fHPrime < 1) {
    fR = fC;
    fG = fX;
    fB = 0;
  } else if(1 <= fHPrime && fHPrime < 2) {
    fR = fX;
    fG = fC;
    fB = 0;
  } else if(2 <= fHPrime && fHPrime < 3) {
    fR = 0;
    fG = fC;
    fB = fX;
  } else if(3 <= fHPrime && fHPrime < 4) {
    fR = 0;
    fG = fX;
    fB = fC;
  } else if(4 <= fHPrime && fHPrime < 5) {
    fR = fX;
    fG = 0;
    fB = fC;
  } else if(5 <= fHPrime && fHPrime < 6) {
    fR = fC;
    fG = 0;
    fB = fX;
  } else {    
    fR = 0;
    fG = 0;
    fB = 0;
  }
  
  fR += fM;
  fG += fM;
  fB += fM;
}

// G. Pronost:	
// Sort function by Hit Time (using first time, assuming hit time within a hit object are sorted)
bool WCSimWCHit::SortFunctor_Hit::operator() (
		const WCSimWCHit * const &a,
		const WCSimWCHit * const &b) const {

	G4double ta, tb;
	if ( a->time.size() > 0 ) 	
		ta = a->time[0];
	else return false;

	if ( b->time.size() > 0 )
		tb = b->time[0];
	else return true;

	return ta < tb;
}
