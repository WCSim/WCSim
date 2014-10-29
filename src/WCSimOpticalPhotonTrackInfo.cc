#include "WCSimOpticalPhotonTrackInfo.hh"


WCSimOpticalPhotonTrackInfo *WCSimOpticalPhotonTrackInfo::s_instance = NULL;

void WCSimOpticalPhotonTrackInfo::reset() {
  fPId.clear();
  fPPos.clear();
  fPDir.clear();
  fParentIdx.clear();
  fPos.clear();
  fId.clear();
  fIstory.clear();
  fPMTId.clear();
  fPMTPos.clear();
  fOpDir.clear();
}

void WCSimOpticalPhotonTrackInfo::Print() {

  // loop over primary particles
  for (int i=0; i<fPId.size(); i++){
    std::cout<<"<WCSimOpticalPhotonTrackInfo> Parent "
        <<" Id = "<<fPId[i]
        <<" Pos=("<<fPPos[i].x()<<", "<<fPPos[i].y()<<", "<<fPPos[i].z()<<") "
        <<" Dir=("<<fPDir[i].x()<<", "<<fPDir[i].y()<<", "<<fPDir[i].z()<<") "
        << std::endl;
  }
  
  // loop over photons
  for (int i=0; i<fParentIdx.size(); i++){
    std::cout<<"<WCSimOpticalPhotonTrackInfo> Opphoton "
        <<" Id = "<<fId[i]
        <<" Pos=("<<fPos[i].x()<<", "<<fPos[i].y()<<", "<<fPos[i].z()<<") "
        <<" Dir=("<<fOpDir[i].x()<<", "<<fOpDir[i].y()<<", "<<fOpDir[i].z()<<") "
        <<" ParentId="<<fParentIdx[i]
        <<" istory="<<fIstory[i]
        << std::endl;
  }
  
  // Summary info
  std::cout<<"<WCSimOpticalPhotonTrackInfo> Number of primary particles is "<<fPId.size()<<std::endl;
  std::cout<<"<WCSimOpticalPhotonTrackInfo> Number of optical photons is "<<fParentIdx.size()<<std::endl;
}


void WCSimOpticalPhotonTrackInfo::UserSteppingAction(const G4Step* aStep)
{
  WCSimOpticalPhotonTrackInfo* trackInfo = WCSimOpticalPhotonTrackInfo::instance();

  const G4Track* track       = aStep->GetTrack();
  if(isEnabled()){
    if ( track->GetCurrentStepNumber() == 1 ){
      
      // std::cout<<"SteppingAction trackid="<<track->GetTrackID()
      // 	     <<" step "<<  track->GetCurrentStepNumber()
      // 	     <<" particledef= "<<track->GetParticleDefinition()->GetParticleName()
      // 	     <<" parent="<<track->GetParentID()
      // 	     <<std::endl;
 
        if ( track->GetParticleDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {
          if ( trackInfo->IdExists( track->GetTrackID() ) != -1 ) return; // first step already added!
    
          trackInfo->addPos(track->GetVertexPosition());
          trackInfo->addId(track->GetTrackID());
          int pid = track->GetParentID();
          int pididx = trackInfo->PIdExists( pid );
        if ( pididx < 0 ) {
          G4cout<<"<UserSteppingAction> parent Id wasn't found yet pid="<<pid<<" trackid="<<track->GetTrackID()<<G4endl;
        } 
        trackInfo->addParentIdx( pididx );
        trackInfo->addIstory(0); //initially no scatter or reflection
        trackInfo->addPMTId(-1); //intitially haven't hit pmt yet
        trackInfo->addPMTPos( G4ThreeVector( 0., 0., 0. ) );
        trackInfo->addOpDir( track->GetVertexMomentumDirection() );
           
      } else {
        // if its not an optical photon assume it could be a parent particle for the photons
        trackInfo->addPId( track->GetTrackID()  ); 
        trackInfo->addPPos( track->GetVertexPosition()); 
        trackInfo->addPDir( track->GetVertexMomentumDirection() ); 
      }
 
    }
 } 
  
  G4OpBoundaryProcessStatus boundaryStatus=Undefined;
  static G4OpBoundaryProcess* boundary=NULL;

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

  if ( track->GetParticleDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {
    // get index of this track in trackInfo
    int tridx = trackInfo->IdExists( track->GetTrackID() );
    if (tridx < 0 && isEnabled() ){ 
      std::cout<<"Warning tridx<0, SteppingAction trackid="<<track->GetTrackID()
	       <<" step "<<  track->GetCurrentStepNumber()
	       <<" particledef= "<<track->GetParticleDefinition()->GetParticleName()
	       <<" parent="<<track->GetParentID()
	       <<std::endl;  
      return;
    }
    //aStep 
    G4StepPoint* thePrePoint = aStep->GetPreStepPoint();
    G4VPhysicalVolume* thePrePV = thePrePoint->GetPhysicalVolume();

    G4StepPoint* thePostPoint = aStep->GetPostStepPoint();
    G4VPhysicalVolume* thePostPV = thePostPoint->GetPhysicalVolume();

    if(!thePostPV){//out of world
      std::cout<<"Left world SteppingAction trackid="<<track->GetTrackID()
	       <<" step "<<  track->GetCurrentStepNumber()
	       <<" particledef= "<<track->GetParticleDefinition()->GetParticleName()
	       <<" parent="<<track->GetParentID()
	       <<std::endl;  
      return;
    }
    
    //if (   thePostPoint->GetProcessDefinedStep()->GetProcessName() != "OpRayleigh" &&
	   //thePostPoint->GetProcessDefinedStep()->GetProcessName() != "Transportation" &&
	   //thePostPoint->GetProcessDefinedStep()->GetProcessName() != "OpAbsorption" ){
      //std::cout<<"thePostPoint Process was "<<thePostPoint->GetProcessDefinedStep()->GetProcessName()<<std::endl;
    //}
    if(isEnabled()){
      if ( thePostPoint->GetProcessDefinedStep()->GetProcessName() == "OpRayleigh"){
        trackInfo->incScatters( tridx );
      }
  
      boundaryStatus=boundary->GetStatus();
     switch(boundaryStatus){
     case Absorption:
        break;
      case Detection:
        std::cout<<"Detection at boundary between "<<thePrePV->GetName()<<" and" <<thePostPV->GetName()<<std::endl; 
       break;  
      case FresnelReflection:
      case TotalInternalReflection: 
      case LambertianReflection:
      case LobeReflection:
      case SpikeReflection:
      case BackScattering:
        trackInfo->incReflections( tridx );
        break;
      //case FresnelRefraction:
      //  std::cout<<"Fresnel Refraction between "<<thePrePV->GetName()<<" and " <<thePostPV->GetName()<<std::endl; 
      default:
        break;
      }              
    }
  }
}   
        
void WCSimOpticalPhotonTrackInfo::addPId(int pId) {
  fPId.push_back(pId);
}

void WCSimOpticalPhotonTrackInfo::addPPos(G4ThreeVector pPos) {
  fPPos.push_back(pPos);
}

void WCSimOpticalPhotonTrackInfo::addPDir(G4ThreeVector pDir) {
  fPDir.push_back(pDir);
}

void WCSimOpticalPhotonTrackInfo::addParentIdx(int parentIdx) {
  fParentIdx.push_back(parentIdx);
}

void WCSimOpticalPhotonTrackInfo::addPos(G4ThreeVector pos) {
  fPos.push_back(pos);
}

void WCSimOpticalPhotonTrackInfo::addId(int id) {
  fId.push_back(id);
}

void WCSimOpticalPhotonTrackInfo::addIstory(int istory) {
  fIstory.push_back(istory);
}

void WCSimOpticalPhotonTrackInfo::addPMTId(int pmtId) {
  fPMTId.push_back(pmtId);
}

void WCSimOpticalPhotonTrackInfo::addPMTPos(G4ThreeVector pos) {
  fPMTPos.push_back(pos);
}

void WCSimOpticalPhotonTrackInfo::addOpDir(G4ThreeVector dir) {
  fOpDir.push_back(dir);
}

std::vector<int> WCSimOpticalPhotonTrackInfo::getPId() {
  return fPId;
}
 
std::vector<G4ThreeVector> WCSimOpticalPhotonTrackInfo::getPPos() {
  return fPPos;
}

std::vector<G4ThreeVector> WCSimOpticalPhotonTrackInfo::getPDir() {
  return fPDir;
}

std::vector<int> WCSimOpticalPhotonTrackInfo::getParentIdx() {
  return fParentIdx;
}

std::vector<G4ThreeVector> WCSimOpticalPhotonTrackInfo::getPos() {
  return fPos;
} 
    
std::vector<int> WCSimOpticalPhotonTrackInfo::getId() {
  return fId;
}
       
std::vector<int> WCSimOpticalPhotonTrackInfo::getIstory() {
  return fIstory;
}
   
std::vector<int> WCSimOpticalPhotonTrackInfo::getPMTId() {
  return fPMTId;
}
  
std::vector<G4ThreeVector> WCSimOpticalPhotonTrackInfo::getPMTPos() {
  return fPMTPos;
}

std::vector<G4ThreeVector> WCSimOpticalPhotonTrackInfo::getOpDir() {
  return fOpDir;
}
void WCSimOpticalPhotonTrackInfo::setParentIdx(int idx, int parentIdx){
  fParentIdx[idx]=parentIdx;
} 

void WCSimOpticalPhotonTrackInfo::setPos(int idx, G4ThreeVector pos){
  fPos[idx]=pos;
}

void WCSimOpticalPhotonTrackInfo::setId(int idx, int id) {
  fId[idx]=id;
}

void WCSimOpticalPhotonTrackInfo::setIstory(int idx, int istory) {
  fIstory[idx]=istory;
}
void WCSimOpticalPhotonTrackInfo::setOpDir(int idx, G4ThreeVector dir) {
  fOpDir[idx] = dir;
}


void WCSimOpticalPhotonTrackInfo::incScatters( int idx ){
  if ( idx>=0 && idx<fIstory.size() && fIstory.size()>0){
    fIstory[idx] += 1;
    //std::cout<<"incScatters idx="<<idx<<" istory="<<fIstory[idx]<<std::endl;
  } else {
    std::cout<<"WCSimOpticalPhotonTrackInfo::incScatters idx out of bounds "<<idx
	     <<" Istory.size="<<fIstory.size()<<std::endl;
  }
}

void WCSimOpticalPhotonTrackInfo::incReflections( int idx){
  if ( idx>=0 && idx<fIstory.size() && fIstory.size()>0){
    fIstory[idx] += 1000;
    //std::cout<<"incReflections idx="<<idx<<" istory="<<fIstory[idx]<<std::endl;
  } else {
    std::cout<<"WCSimOpticalPhotonTrackInfo::incReflections idx out of bounds "<<idx
	     <<" Istory.size="<<fIstory.size()<<std::endl;
  }
}


void WCSimOpticalPhotonTrackInfo::setPMTId(int idx, int pmtId) {
  fPMTId[idx]=pmtId;
}

void WCSimOpticalPhotonTrackInfo::setPMTPos(int idx, G4ThreeVector pos) {
  fPMTPos[idx]=pos;
}

int WCSimOpticalPhotonTrackInfo::PIdExists(int pId){
  for (int i = 0; i < fPId.size(); i++){
    if(pId == fPId[i]){
      return i;
    }
  }
  return -1; 
}

int WCSimOpticalPhotonTrackInfo::IdExists(int id){
  for (int i = 0; i < fId.size(); i++){
    if(id == fId[i]){
      return i;
    }
  }
  return -1; 
}
