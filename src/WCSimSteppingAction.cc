#include <stdlib.h>
#include <stdio.h>
#include <WCSimRootEvent.hh>
#include <G4SIunits.hh>
#include <TRandom3.h>
#include <G4OpticalPhoton.hh>

#include "WCSimSteppingAction.hh"

#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4VParticleChange.hh"
#include "G4SteppingVerbose.hh"
#include "G4SteppingManager.hh"
#include "G4PVParameterised.hh"
#include "G4PVReplica.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"

#define PI 3.141592654
const double planck = 4.1357e-15; // ev.s
const double lightspeed = 299792458e9; // nm/s

TH1F *emissionHist;

inline void CreateEmissionHistogram(){


  static const int emitArraySize = 15;
  float emitWavelength[emitArraySize] = {390,400,410,420,430,440,450,460,470,480,490,500,510,520,530};
  float emit[emitArraySize] =
      {0.0, 0.1, 0.71, 0.98,
       0.91, 0.63, 0.49, 0.35,
       0.20, 0.13, 0.081, 0.052,
       0.03, 0.021, 0};

  emissionHist = new TH1F("hist","hist", 15, 390., 530.);
  emissionHist->Sumw2();

  for (int i = 0 ; i < emitArraySize; i++) {

    emissionHist->Fill(emitWavelength[i], emit[i]);
    emissionHist->SetBinError(i+1, 0);
  }


}

// First we have a structure to hold the values of the emitted photon.
typedef struct {

  G4ThreeVector direction; // The direction of the emitted photon.
  G4ThreeVector position; // The position where the emitted photon was created (same as where the incident photon was absorbed).
  double energy; // The energy value of the emitted photon.
  double wavelength; // The wavelength of the emitted photon.

} EmittedPhoton;

// This function will determine the probability of an emitted photon will being detected by the PMT.
// This function uses only the position of the photon's absorption with respect to the PMT (distance from PMT)
// and lab measurements of the efficiency as a function of distance, taken at Edinburgh University.
double EmittedPhotonDetectedProbability(G4ThreeVector absorbedPhotonPosition){

  // This function depends on the values measured at Edinburgh. Perhaps linear? exponential? something else?

  return 0.2;
}// End of EmittedPhotonDetectedProbability function.


// This function will determine whether or not a photon will be detected by the PMT. This function
// uses only the position of the photon's absorption with respect to the PMT (distance from PMT) and
// lab measurements of the efficiency as a function of distance, taken at Edinburgh University.
bool EmittedPhotonDetected(G4ThreeVector absorbedPhotonPosition){
  TRandom3 *r = new TRandom3(time(0)); // This can be changed to whatever random number generator WCSim is using.

  // Check to see whether random number generated is less than or equal to the probability of a photon, emitted
  // from that position, being detected by the PMT.
  if (r->Uniform(0.,1.) <= EmittedPhotonDetectedProbability(absorbedPhotonPosition) ){
    return true;
  } else {
    return false;
  }// End of if statement.


}// End of EmittedPhotonDetected function.


// This function will convert wavelength(nm) to energy(ev).
double WavelengthToEnergy(double lambda){

  return (planck*lightspeed/lambda); //energy in ev.

}

// This function will select a wavelength for the emitted photon and return it as a wavelength in nm.
double EmittedPhotonWavelength(){


  return emissionHist->GetRandom();

}// End of EmittedPhotonWavelength function.


// This funtion will generate an emitted photon with a random 3D direction and energy value (from the
// Eljen EJ-286 emission spectra). It will use only the position of absorption and output a direction
// (G4ThreeVector) and an energy (ev).
EmittedPhoton EmitPhoton(G4ThreeVector absorbedPhotonPosition){

  EmittedPhoton phot;
  phot.position = absorbedPhotonPosition;
//	phot->position->SetX(absorbedPhotonPosition->x());
//	phot->position->SetY(absorbedPhotonPosition->y());
//	phot->position->SetZ(absorbedPhotonPosition->z());

  // Here we can use TF1 and the GetRandom() function to selected from the emission spectra.

  // We don't know the spatial distribution of the emitted photon so we will just keep this distribution uniform
  // over the surface of a sphere.

  TRandom3 *r = new TRandom3(time(0)); // This can be changed to whatever random number generator WCSim is using.

  // Generate random points on a unit sphere. http://mathworld.wolfram.com/SpherePointPicking.html
  double x, y, z;
  r->Sphere(x,y,z,1.);

  G4ThreeVector dir(x, y, z);

  phot.direction = dir;

  phot.wavelength = EmittedPhotonWavelength();
  phot.energy = WavelengthToEnergy(phot.wavelength);


  return phot;
}// End of EmitPhoton function.


void WLSModel(){

  G4cout << "This is just a test." << G4endl;

  for (int i = 0; i<10; i++){
    G4ThreeVector v(i*0.2, i*1, i*30);
    EmittedPhoton p = EmitPhoton(v);
    G4cout << "Absorbed: " << v.x() << " " << v.y() << " " << v.z() << G4endl;
    G4cout << "PhotAbsorb: " << p.position.x() << " " << p.position.y() << " " << p.position.z() << G4endl;
    G4cout << "PhotDir: " << p.direction.x() << " " << p.direction.y() << " " << p.direction.z() << G4endl;
    G4cout << "PhotWavelength: " << p.wavelength << G4endl;
    G4cout << "PhotEnergy: " << p.energy << G4endl;
    G4cout << "-----------------------------------------------" << G4endl;

  }

}


///////////////////////////////////////////////
///// BEGINNING OF WCSIM STEPPING ACTION //////
///////////////////////////////////////////////


WCSimSteppingAction::WCSimSteppingAction(WCSimRunAction *myRun, WCSimDetectorConstruction *myDet) : runAction(myRun), det(myDet) {
  WLS = new EljenEJ286;
}

void WCSimSteppingAction::UserSteppingAction(const G4Step* aStep)
{
  //DISTORTION must be used ONLY if INNERTUBE or INNERTUBEBIG has been defined in BidoneDetectorConstruction.cc
  
  const G4Event* evt = G4RunManager::GetRunManager()->GetCurrentEvent();

  const G4Track* track       = aStep->GetTrack();
  G4VPhysicalVolume* volume  = track->GetVolume();


  G4SDManager* SDman   = G4SDManager::GetSDMpointer();
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();

  // WLS
//  if(WLS){
//    CreateEmissionHistogram();
//    DebugWLSPlates(aStep);
//    WLSPhysicsProcess(aStep);
//  }

}


G4int WCSimSteppingAction::G4ThreeVectorToWireTime(G4ThreeVector *pos3d,
						    G4ThreeVector lArPos,
						    G4ThreeVector start,
						    G4int i)
{
  G4double x0 = start[0]-lArPos[0];
  G4double y0 = start[1]-lArPos[1];
//   G4double y0 = 2121.3;//mm
  G4double z0 = start[2]-lArPos[2];

  G4double dt=0.8;//mm
//   G4double midt = 2651.625;
  G4double pitch = 3;//mm
//   G4double midwir = 1207.10;
  G4double c45 = 0.707106781;
  G4double s45 = 0.707106781;

  G4double w1;
  G4double w2;
  G4double t;

//   G4double xField(0.);
//   G4double yField(0.);
//   G4double zField(0.);

//   if(detector->getElectricFieldDistortion())
//     {
//       Distortion(pos3d->getX(),
// 		 pos3d->getY());
      
//       xField = ret[0];
//       yField = ret[1];
//       zField = pos3d->getZ();
      
//       w1 = (int)(((zField+z0)*c45 + (x0-xField)*s45)/pitch); 
//       w2 = (int)(((zField+z0)*c45 + (x0+xField)*s45)/pitch); 
//       t = (int)(yField+1);

//       //G4cout<<" x orig "<<pos3d->getX()<<" y orig "<<(pos3d->getY()+y0)/dt<<G4endl;
//       //G4cout<<" x new "<<xField<<" y new "<<yField<<G4endl;
//     }
//   else 
//     {
      
  w1 = (int) (((pos3d->getZ()+z0)*c45 + (x0-pos3d->getX())*s45)/pitch); 
  w2 = (int)(((pos3d->getZ()+z0)*c45 + (x0+pos3d->getX())*s45)/pitch); 
  t  = (int)((pos3d->getY()+y0)/dt +1);
//     }

  if (i==0)
    return (int)w1;
  else if (i==1)
    return (int)w2;
  else if (i==2)
    return (int)t;
  else return 0;
} 


void WCSimSteppingAction::Distortion(G4double /*x*/,G4double /*y*/)
{
 
//   G4double theta,steps,yy,y0,EvGx,EvGy,EField,velocity,tSample,dt;
//   y0=2121.3;//mm
//   steps=0;//1 mm steps
//   tSample=0.4; //micros
//   dt=0.8;//mm
//   LiquidArgonMedium medium;  
//   yy=y;
//   while(y<y0 && y>-y0 )
//     {
//       EvGx=FieldLines(x,y,1);
//       EvGy=FieldLines(x,y,2);
//       theta=atan(EvGx/EvGy);
//       if(EvGy>0)
// 	{
// 	  x+=sin(theta);
// 	  y+=cos(theta);
// 	}
//       else
// 	{
// 	  y-=cos(theta);
// 	  x-=sin(theta);
// 	}
//       EField=sqrt(EvGx*EvGx+EvGy*EvGy);//kV/mm
//       velocity=medium.DriftVelocity(EField*10);// mm/microsec
//       steps+=1/(tSample*velocity);

//       //G4cout<<" step "<<steps<<" x "<<x<<" y "<<y<<" theta "<<theta<<" Gx "<<eventaction->Gx->Eval(x,y)<<" Gy "<<eventaction->Gy->Eval(x,y)<<" EField "<<EField<<" velocity "<<velocity<<G4endl;
//     }

//   //numbers
//   //EvGx=FieldLines(0,1000,1);
//   //EvGy=FieldLines(0,1000,2);
//   //EField=sqrt(EvGx*EvGx+EvGy*EvGy);//kV/mm
//   //velocity=medium.DriftVelocity(EField*10);// mm/microsec
//   //G4double quenching;
//   //quenching=medium.QuenchingFactor(2.1,EField*10);
//   //G4cout<<" Gx "<<EvGx<<" Gy "<<EvGy<<" EField "<<EField<<" velocity "<<velocity<<" quenching "<<quenching<<G4endl;


//   ret[0]=x;
//   if(yy>0)
//     ret[1]=2*y0/dt -steps;
//   else
//     ret[1]=steps; 
}


double WCSimSteppingAction::FieldLines(G4double /*x*/,G4double /*y*/,G4int /*coord*/)
{ //0.1 kV/mm = field
  //G4double Radius=302;//mm
//   G4double Radius=602;//mm
//   if(coord==1) //x coordinate
//     return  (0.1*(2*Radius*Radius*x*abs(y)/((x*x+y*y)*(x*x+y*y))));
//   else //y coordinate
//     return 0.1*((abs(y)/y)*(1-Radius*Radius/((x*x+y*y)*(x*x+y*y))) + abs(y)*(2*Radius*Radius*y/((x*x+y*y)*(x*x+y*y))));
  return 0;
}
void WCSimSteppingAction::DebugWLSPlates(const G4Step *aStep) {

  G4Track *aTrack = aStep->GetTrack();
  G4ParticleDefinition* particleType = aTrack->GetDefinition();

  bool printouts = false;
  printouts = true;

  if(particleType == G4OpticalPhoton::OpticalPhotonDefinition()){

    G4String postMaterialName = aStep->GetPostStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetName();
    G4String procName = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

    photonEvt *pEvt = GetRunAction()->GetPhotonEvt();

    pEvt->trackID = aTrack->GetTrackID();
    pEvt->parentID = aTrack->GetParentID();

    pEvt->pos[0] = aTrack->GetPosition().x();
    pEvt->pos[1] = aTrack->GetPosition().y();
    pEvt->pos[2] = aTrack->GetPosition().z();

    pEvt->wl = ((2.0*M_PI*197.3)/(aTrack->GetTotalEnergy()/CLHEP::eV));

    if(aTrack->GetCreatorProcess()->GetProcessName()=="Cerenkov") pEvt->proc = 0;
    else if(aTrack->GetCreatorProcess()->GetProcessName()=="OpWLS") pEvt->proc = 1;

    // Fill only if photons enter the WLS material
    if(postMaterialName=="WCODWLSPlate") runAction->GetPhotonTree()->Fill();

    ////// PRINTOUTS //////
    if(printouts){
      G4cout << "OpPhoton in " << aTrack->GetVolume()->GetName()
                << " and going to " << aTrack->GetNextVolume()->GetName()
                << G4endl;
      G4cout << "Creator process : " << aTrack->GetCreatorProcess()->GetProcessName() << G4endl;
      G4cout << "# Track ID : " << aTrack->GetTrackID() << G4endl;
      G4cout << "# Parent ID : " << aTrack->GetParentID() << G4endl;
      G4cout << "# Pos : "
                << aTrack->GetPosition().x() << " "
                << aTrack->GetPosition().y() << " "
                << aTrack->GetPosition().z() << " " << G4endl;
      G4cout << "WL : " << pEvt->wl << G4endl;
      G4cout << "###" << G4endl;
      G4cout << "STEP POINT INFO" << G4endl;
      G4cout << "postMaterialName : " << postMaterialName << G4endl;
      G4cout << "procName : " << procName << G4endl;
      G4cout << G4endl;
    } // END if printouts

  } // END if photon
} // END DebugWLSPlates

bool isPhotonInWLSPlate(G4String name){
  if(name == "WCODWLSPlate")
    return true;
  else
    return false;
}

void WCSimSteppingAction::WLSPhysicsProcess(const G4Step *aStep){

  G4Track *aTrack = aStep->GetTrack();
  G4ParticleDefinition* particleType = aTrack->GetDefinition();

  if(particleType == G4OpticalPhoton::OpticalPhotonDefinition()){
    G4String postMaterialName = aStep->GetPostStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetName();
    G4String procName = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

    double wl = ((2.0*M_PI*197.3)/(aTrack->GetTotalEnergy()/CLHEP::eV));
    double absProb = WLS->GetgAbs()->Eval(wl,0,"S");

    if(isPhotonInWLSPlate(postMaterialName) && absProb > G4UniformRand()){

      const G4ThreeVector &pos = aTrack->GetPosition(); // Recover photon position
      const G4ThreeVector &dir = aTrack->GetMomentumDirection(); // Recover photon direction

      if(EmittedPhotonDetected(pos)){
        EmittedPhoton p = EmitPhoton(pos);
      }

      // Add this photon to the hit collection
      G4int WLSplateID = aStep->GetPostStepPoint()->GetPhysicalVolume()->GetCopyNo();
      // Find PMT with the same ID, and find its position
      std::vector<WCSimPmtInfo*> *pmts = det->Get_ODPmts();
      // It works out that the pmts here are ordered !
      // pmts->at(i) has tubeid i+1
      WCSimPmtInfo* pmtinfo = (WCSimPmtInfo*)pmts->at( WLSplateID - 1 );
      G4cout << "WLS ID : " << WLSplateID << " PMT ID : " << pmtinfo->Get_tubeid() << G4endl;
      G4ThreeVector newPos(10*pmtinfo->Get_transx(),10*pmtinfo->Get_transy(),10*pmtinfo->Get_transz()); // recorded as cm in odtubemap...

      G4ThreeVector WLSpos = aStep->GetPostStepPoint()->GetPhysicalVolume()->GetTranslation();

      G4cout << "WLS center pos : "
             << WLSpos.x() << " "
             << WLSpos.y() << " "
             << WLSpos.z() << " " << G4endl;
      G4cout << "PMT center pos : "
             << newPos.x() << " "
             << newPos.y() << " "
             << newPos.z() << " " << G4endl;
      G4cout << G4endl;

      // TODO : Find where is the nearest PMT

//      aTrack->SetPosition(newPos);
//      aStep->GetPostStepPoint()->SetPosition(newPos);
//      aStep->GetPostStepPoint()->SetGlobalTime(-1);//Help recognize WLS photon in analysis

    } // END if in WLS
  } // END if opticalphoton

}
