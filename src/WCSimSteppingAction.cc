#include <stdlib.h>
#include <stdio.h>
#include <WCSimRootEvent.hh>
#include <G4SIunits.hh>
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

const double planck = 4.1357e-15; // ev.s
const double lightspeed = 299792458e9; // nm/s

// This function will determine the probability of an emitted photon will being detected by the PMT.
// This function uses only the position of the photon's absorption with respect to the PMT (distance from PMT)
// and lab measurements of the efficiency as a function of distance, taken at Edinburgh University.
double EmittedPhotonDetectedProbability(G4ThreeVector absorbedPhotonPosition){

  // This function depends on the values measured at Edinburgh. Perhaps linear? exponential? something else?

  // absorbedPhotonPosition has to be given in local coordinate, let's compute the distance from the center,
  // where the PMT is position
  // Then will add a probabilty distribution following the solid angle
  double distance = absorbedPhotonPosition.mag();
  return 0.2/(1+distance);
}// End of EmittedPhotonDetectedProbability function.


// This function will determine whether or not a photon will be detected by the PMT. This function
// uses only the position of the photon's absorption with respect to the PMT (distance from PMT) and
// lab measurements of the efficiency as a function of distance, taken at Edinburgh University.
bool EmittedPhotonDetected(G4ThreeVector absorbedPhotonPosition){

  // Check to see whether random number generated is less than or equal to the probability of a photon, emitted
  // from that position, being detected by the PMT.
  if (G4UniformRand() <= EmittedPhotonDetectedProbability(absorbedPhotonPosition) ){
    return true;
  } else {
    return false;
  }// End of if statement.


}// End of EmittedPhotonDetected function.


// This function will convert wavelength(nm) to energy(ev).
double WavelengthToEnergy(double lambda){

  return (planck*lightspeed/lambda); //energy in ev.

}

// Just return the name of the PhysicalVolume containing the WLS plate
bool isPhotonInWLSPlate(G4String name){
  if(name == "WCODWLSPlate")
    return true;
  else
    return false;
}


///////////////////////////////////////////////
///// BEGINNING OF WCSIM STEPPING ACTION //////
///////////////////////////////////////////////


WCSimSteppingAction::WCSimSteppingAction(WCSimRunAction *myRun, WCSimDetectorConstruction *myDet) : runAction(myRun), det(myDet) {

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
  bool WLS=false;
  WLS=true;
  if(WLS){
    CollectPhotonsInfoInWLSPlates(aStep);
    // WLSPhysicsProcess(aStep);
  }

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

void DebugWLSPrintouts(const G4Step *aStep){

  G4Track *aTrack = aStep->GetTrack();
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  G4StepPoint* postStepPoint = aStep->GetPostStepPoint();
  G4String preLogicalVolName = preStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();
  G4String postLogicalVolName = postStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();
  bool isInitialParticle = false;
  if (aTrack->GetCreatorProcess() == NULL) isInitialParticle = true;

  G4cout << "OpPhoton in " << aTrack->GetVolume()->GetName()
         << " and going to " << aTrack->GetNextVolume()->GetName()
         << G4endl;
  if (isInitialParticle) {
    G4cout << "Creator process : " << "None" << G4endl;
  }
  else {
    G4cout << "Creator process : " << aTrack->GetCreatorProcess()->GetProcessName() << G4endl;
  }
  G4cout << "# Track ID : " << aTrack->GetTrackID() << G4endl;
  G4cout << "# Parent ID : " << aTrack->GetParentID() << G4endl;
  G4cout << "# Pos : "
         << aTrack->GetPosition().x() << " "
         << aTrack->GetPosition().y() << " "
         << aTrack->GetPosition().z() << " " << G4endl;
  G4cout << "WL : " << ((2.0*M_PI*197.3)/(aTrack->GetTotalEnergy()/CLHEP::eV)) << G4endl;
  G4cout << "###" << G4endl;
  G4cout << "STEP POINT INFO" << G4endl;
  G4cout << "postLogicalVolName : " << postLogicalVolName << G4endl;
  G4cout << "procName : " << postStepPoint->GetProcessDefinedStep()->GetProcessName() << G4endl;
  G4cout << G4endl;

}

void WCSimSteppingAction::CollectPhotonsInfoInWLSPlates(const G4Step *aStep) {

  G4Track *aTrack = aStep->GetTrack();
  G4ParticleDefinition* particleType = aTrack->GetDefinition();

  bool printouts = false;
  // printouts = true;
  bool isInitialParticle = false;
  if (aTrack->GetCreatorProcess() == NULL) isInitialParticle = true;

  if(particleType == G4OpticalPhoton::OpticalPhotonDefinition()){

    G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
    G4StepPoint* postStepPoint = aStep->GetPostStepPoint();

    G4String preVol = preStepPoint->GetPhysicalVolume()->GetName();
    G4String postVol = postStepPoint->GetPhysicalVolume()->GetName();

    G4String preLogicalVolName = preStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();
    G4String postLogicalVolName = postStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();
    G4String creaProc;
    if (!isInitialParticle){ creaProc = aTrack->GetCreatorProcess()->GetProcessName();}

    // if(postVol == "WCPMTOD" && creaProc == "OpWLS"){
    if(preLogicalVolName=="WCODWLSPlate"){

      G4TouchableHandle theTouchable = postStepPoint->GetTouchableHandle();
      G4ThreeVector worldPosition = postStepPoint->GetPosition();
      G4ThreeVector localPosition = theTouchable->GetHistory()->GetTopTransform().TransformPoint(worldPosition);

      photonEvt *pEvt = GetRunAction()->GetPhotonEvt();

      pEvt->trackID = aTrack->GetTrackID();
      pEvt->parentID = aTrack->GetParentID();

      pEvt->pos[0] = worldPosition.x();
      pEvt->pos[1] = worldPosition.y();
      pEvt->pos[2] = worldPosition.z();

      pEvt->distance = localPosition.mag();

      pEvt->wl = ((2.0*M_PI*197.3)/(aTrack->GetTotalEnergy()/CLHEP::eV));

      if(isInitialParticle) pEvt->proc = -1;
      else if(aTrack->GetCreatorProcess()->GetProcessName()=="Cerenkov") pEvt->proc = 0;
      else if(aTrack->GetCreatorProcess()->GetProcessName()=="OpWLS") pEvt->proc = 1;

      // Fill only if photons enter the WLS material
      runAction->GetPhotonTree()->Fill();

      // DEBUG Printouts
      if(printouts) DebugWLSPrintouts(aStep);

      // KILL the damn particle
      // aTrack->SetTrackStatus(fStopAndKill);
    }

  } // END if photon

} // END DebugWLSPlates

// This function will select a wavelength for the emitted photon and return it as a wavelength in nm.
double WCSimSteppingAction::EmittedPhotonWavelength(){


  return det->GetWLSPointer()->emissionHist->GetRandom();

}// End of EmittedPhotonWavelength function.


// This funtion will generate an emitted photon with a random 3D direction and energy value (from the
// Eljen EJ-286 emission spectra). It will use only the position of absorption and output a direction
// (G4ThreeVector) and an energy (ev).
EmittedPhoton WCSimSteppingAction::EmitPhoton(G4ThreeVector absorbedPhotonPosition){

  EmittedPhoton phot;
  phot.position = absorbedPhotonPosition;
//	phot->position->SetX(absorbedPhotonPosition->x());
//	phot->position->SetY(absorbedPhotonPosition->y());
//	phot->position->SetZ(absorbedPhotonPosition->z());

  // Here we can use TF1 and the GetRandom() function to selected from the emission spectra.

  // We don't know the spatial distribution of the emitted photon so we will just keep this distribution uniform
  // over the surface of a sphere.
  double theta = 2*TMath::Pi()*G4UniformRand();
  double phi = acos(2*G4UniformRand() - 1);
  double x = sin(phi)*cos(theta);
  double y = sin(phi)*sin(theta);
  double z = cos(phi);


  // Generate random points on a unit sphere. http://mathworld.wolfram.com/SpherePointPicking.html


  G4ThreeVector dir(x, y, z);

  phot.direction = dir;

  phot.wavelength = EmittedPhotonWavelength();
  phot.energy = WavelengthToEnergy(phot.wavelength);


  return phot;
}// End of EmitPhoton function.



void WCSimSteppingAction::WLSPhysicsProcess(const G4Step *aStep){

  G4Track *aTrack = aStep->GetTrack();
  G4ParticleDefinition* particleType = aTrack->GetDefinition();

  if(particleType == G4OpticalPhoton::OpticalPhotonDefinition()){

    G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
    G4StepPoint* postStepPoint = aStep->GetPostStepPoint();

    // Retrive basic step info
    G4String postLogicalVolName = postStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();
    G4String procName = postStepPoint->GetProcessDefinedStep()->GetProcessName();

    double wl = ((2.0*M_PI*197.3)/(aTrack->GetTotalEnergy()/CLHEP::eV));
    double absProb = det->GetWLSPointer()->GetgAbs()->Eval(wl,0,"S");

    if(isPhotonInWLSPlate(postLogicalVolName) && absProb > G4UniformRand()){

      // KILL the damn particle
      aTrack->SetTrackStatus(fStopAndKill);

      G4TouchableHandle theTouchable = postStepPoint->GetTouchableHandle();

      G4int copyNo = theTouchable->GetCopyNumber();
      G4int motherCopyNo = theTouchable->GetCopyNumber(1);

      G4cout << "copyno : " << copyNo << " mothercopyno : " << motherCopyNo << G4endl;

      std::vector<WCSimPmtInfo*> *pmts=det->Get_ODPmts();
      WCSimPmtInfo* pmtinfo = (WCSimPmtInfo*)pmts->at((unsigned long) (motherCopyNo - 1));
      Double_t hit_pos[3];
      hit_pos[0] = 10*pmtinfo->Get_transx();
      hit_pos[1] = 10*pmtinfo->Get_transy();
      hit_pos[2] = 10*pmtinfo->Get_transz();
      G4ThreeVector pmt_position(hit_pos[0]*mm, hit_pos[1]*mm, hit_pos[2]*mm);

      G4cout << "PMT center pos : "
             << pmt_position.x() << " "
             << pmt_position.y() << " "
             << pmt_position.z() << " " << G4endl;


      G4ThreeVector worldPosition = postStepPoint->GetPosition();
      // Local position inside WLS plate of the photon !!!
      G4ThreeVector localPosition =
          theTouchable->GetHistory()->GetTopTransform().TransformPoint(worldPosition);

      G4cout << "photon pos : "
             << worldPosition.x() << " "
             << worldPosition.y() << " "
             << worldPosition.z() << " " << G4endl;


      const G4ThreeVector &dir = aTrack->GetMomentumDirection(); // Recover photon direction

      if(EmittedPhotonDetected(localPosition)){
        EmittedPhoton p = EmitPhoton(localPosition);
      }


      G4LogicalVolume *mother = postStepPoint->GetPhysicalVolume()->GetMotherLogical();
      G4VPhysicalVolume *daughter;
      for(int i=0;i<mother->GetNoDaughters();i++){ // ID 0 is WLS plate and ID 1 is PMT
        daughter = mother->GetDaughter(i);
        G4ThreeVector daughterPosition = daughter->GetTranslation();
        // Recover daughter volume of PMT
        if(daughter->GetName() == "WCPMTOD"){
          for(int j=0;j<daughter->GetLogicalVolume()->GetNoDaughters();j++){ // ID 0 is InteriorPMT and ID 1 is HyperK-glassFaceWCPMT_OD
            if(daughter->GetLogicalVolume()->GetDaughter(j)->GetName() == "HyperK-glassFaceWCPMT_OD"){
              G4ThreeVector newPos(daughter->GetLogicalVolume()->GetDaughter(j)->GetTranslation().x(),
                                   daughter->GetLogicalVolume()->GetDaughter(j)->GetTranslation().y(),
                                   daughter->GetLogicalVolume()->GetDaughter(j)->GetTranslation().z());
              aTrack->SetPosition(newPos);
              aStep->GetPostStepPoint()->SetPosition(newPos);
              aStep->GetPostStepPoint()->SetGlobalTime(-1);//Help recognize WLS photon in analysis
            }
          }
        }
      }

    } // END if in WLS
  } // END if opticalphoton

}
