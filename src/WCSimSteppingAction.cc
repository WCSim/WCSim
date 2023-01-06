#include "WCSimSteppingAction.hh"

#include <stdlib.h>
#include <stdio.h>
#include <WCSimRootEvent.hh>
#include <G4SIunits.hh>
#include <G4OpticalPhoton.hh>

#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4VParticleChange.hh"
#include "G4SteppingVerbose.hh"
#include "G4SteppingManager.hh"
#include "G4PVParameterised.hh"
#include "G4PVReplica.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4OpBoundaryProcess.hh"

G4int WCSimSteppingAction::n_photons_through_mPMTLV = 0;
G4int WCSimSteppingAction::n_photons_through_acrylic = 0;
G4int WCSimSteppingAction::n_photons_through_gel = 0;
G4int WCSimSteppingAction::n_photons_on_blacksheet = 0;
G4int WCSimSteppingAction::n_photons_on_smallPMT = 0;

///////////////////////////////////////////////
///// BEGINNING OF WCSIM STEPPING ACTION //////
///////////////////////////////////////////////


WCSimSteppingAction::WCSimSteppingAction(WCSimRunAction *myRun, WCSimDetectorConstruction *myDet) : runAction(myRun), det(myDet) {

}

void WCSimSteppingAction::UserSteppingAction(const G4Step* aStep)
{
  //DISTORTION must be used ONLY if INNERTUBE or INNERTUBEBIG has been defined in BidoneDetectorConstruction.cc
  
  const G4Track* track       = aStep->GetTrack();
  
  // Not used:
  //const G4Event* evt = G4RunManager::GetRunManager()->GetCurrentEvent();
  //G4VPhysicalVolume* volume  = track->GetVolume();
  //G4SDManager* SDman   = G4SDManager::GetSDMpointer();
  //G4HCofThisEvent* HCE = evt->GetHCofThisEvent();

  
  // Debug for photon tracking
  G4StepPoint* thePrePoint = aStep->GetPreStepPoint();
  G4VPhysicalVolume* thePrePV = thePrePoint->GetPhysicalVolume();

  G4StepPoint* thePostPoint = aStep->GetPostStepPoint();
  G4VPhysicalVolume* thePostPV = thePostPoint->GetPhysicalVolume();

  //G4OpBoundaryProcessStatus boundaryStatus=Undefined;
  //static G4ThreadLocal G4OpBoundaryProcess* boundary=NULL;  //doesn't work and needs #include tls.hh from Geant4.9.6 and beyond
  G4OpBoundaryProcess* boundary=NULL;
  
  //find the boundary process only once
  if(!boundary){
    G4ProcessManager* pm
      = aStep->GetTrack()->GetDefinition()->GetProcessManager();
    G4int nprocesses = pm->GetProcessListLength();
    G4ProcessVector* pv = pm->GetProcessList();
    G4int i;
    for( i=0;i<nprocesses;i++){
      if((*pv)[i]->GetProcessName()=="OpBoundary"){
	boundary = (G4OpBoundaryProcess*)(*pv)[i];
	break;
      }
    }
  }

  G4ParticleDefinition *particleType = track->GetDefinition();
  if(particleType == G4OpticalPhoton::OpticalPhotonDefinition() && thePrePV && thePostPV){
    if( (thePrePV->GetName().find("MultiPMT") != std::string::npos) &&
	(thePostPV->GetName().find("vessel") != std::string::npos) )
      n_photons_through_mPMTLV++;
    
    if( (thePostPV->GetName().find("container") != std::string::npos) &&
	(thePrePV->GetName().find("vessel") != std::string::npos))
      n_photons_through_acrylic++;
    
    if( (thePrePV->GetName().find("container") != std::string::npos) )
      n_photons_through_gel++;

    if( (thePrePV->GetName().find("container") != std::string::npos) &&
	(thePostPV->GetName().find("inner") != std::string::npos) )
      n_photons_on_blacksheet++;

    if( (thePrePV->GetName().find("container") != std::string::npos) &&
	(thePostPV->GetName().find("pmt") != std::string::npos) )
      n_photons_on_smallPMT++;
	

    /*
    if( (thePrePV->GetName().find("pmt") != std::string::npos)){
      std::cout << "Photon between " << thePrePV->GetName() <<
	" and " << thePostPV->GetName() << " because " << 
	thePostPoint->GetProcessDefinedStep()->GetProcessName() << 
	" and boundary status: " <<  boundary->GetStatus() << " with track status " << track->GetTrackStatus() << std::endl;
      
	}*/



    if(track->GetTrackStatus() == fStopAndKill){
      if(boundary->GetStatus() == NoRINDEX){
	std::cout << "Optical photon is killed because of missing refractive index in either " << thePrePoint->GetMaterial()->GetName() << " or " << thePostPoint->GetMaterial()->GetName() <<
	  " (transition from " << thePrePV->GetName() << " to " << thePostPV->GetName() << ")" <<
	  " : could also be caused by Overlaps with volumes with logicalBoundaries." << std::endl;
	
      }
      /* Debug :  
      if( (thePrePV->GetName().find("PMT") != std::string::npos) ||
	  (thePrePV->GetName().find("pmt") != std::string::npos)){
	
	if(boundary->GetStatus() != StepTooSmall){
	  //	if(thePostPoint->GetProcessDefinedStep()->GetProcessName() != "Transportation")
	  std::cout << "Killed photon between " << thePrePV->GetName() <<
	    " and " << thePostPV->GetName() << " because " << 
	    thePostPoint->GetProcessDefinedStep()->GetProcessName() << 
	    " and boundary status: " <<  boundary->GetStatus() <<
	    std::endl;
	}
	}	*/
      
    }
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


  //ret[0]=5;
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
