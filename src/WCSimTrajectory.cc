#include "WCSimTrajectory.hh"
#include "WCSimTrackInformation.hh"

#include "G4TrajectoryPoint.hh"
#include "G4ParticleTable.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UnitsTable.hh"
#include "G4VProcess.hh"
#include "G4OpProcessSubType.hh"
#include "G4ProcessManager.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <sstream>

//G4Allocator<WCSimTrajectory> aTrajectoryAllocator;
G4Allocator<WCSimTrajectory> myTrajectoryAllocator;

WCSimTrajectory::WCSimTrajectory()
  :  positionRecord(0), fTrackID(0), fParentID(0),
     PDGEncoding( 0 ), PDGCharge(0.0), ParticleName(""),
     initialMomentum( G4ThreeVector() ),SaveIt(false), producesHit(false),
     creatorProcess(""), globalTime(0.0), parentTrajectory(0)
{
  boundaryPoints.clear();
  boundaryKEs.clear();
  boundaryTimes.clear();
  boundaryTypes.clear();

  pRayScatter = 0;
  pMieScatter = 0;
  pReflec.clear();

  fBoundary = NULL;
}

WCSimTrajectory::WCSimTrajectory(const G4Track* aTrack)
{
  G4ParticleDefinition * fpParticleDefinition = aTrack->GetDefinition();
  ParticleName = fpParticleDefinition->GetParticleName();
  PDGCharge = fpParticleDefinition->GetPDGCharge();
  PDGEncoding = fpParticleDefinition->GetPDGEncoding();
  fTrackID = aTrack->GetTrackID();
  fParentID = aTrack->GetParentID();
  initialMomentum = aTrack->GetMomentum();
  positionRecord = new TrajectoryPointContainer();
  // Following is for the first trajectory point
  positionRecord->push_back(new G4TrajectoryPoint(aTrack->GetPosition()));

  stoppingPoint  = aTrack->GetPosition();
  stoppingVolume = aTrack->GetVolume();
  producesHit = false;
  parentTrajectory = 0;
  if ( aTrack->GetUserInformation() != 0 ) {
      WCSimTrackInformation *anInfo = (WCSimTrackInformation *) aTrack->GetUserInformation();
      producesHit = anInfo->GetProducesHit();
      parentTrajectory = anInfo->GetParentTrajectory();
      SaveIt = true;
  }
  else SaveIt = false;
  globalTime = aTrack->GetGlobalTime();
  if (aTrack->GetCreatorProcess() != 0 )
    {
      const G4VProcess* tempproc = aTrack->GetCreatorProcess();
      creatorProcess = tempproc->GetProcessName();
    }
  else 
    creatorProcess = "";

  boundaryPoints.clear();
  boundaryKEs.clear();
  boundaryTimes.clear();
  boundaryTypes.clear();

  pRayScatter = 0;
  pMieScatter = 0;
  pReflec.clear();
  fBoundary = NULL;
#ifdef WCSIM_SAVE_PHOTON_HISTORY
  // Only do search for optical photon
  if (PDGEncoding==0)
  {
    G4ProcessManager* pm = aTrack->GetDefinition()->GetProcessManager();
    G4int nprocesses = pm->GetProcessListLength();
    G4ProcessVector* pv = pm->GetProcessList();
    for(G4int i=0;i<nprocesses;i++){
      if((*pv)[i]->GetProcessName()=="OpBoundary"){
        fBoundary = (WCSimOpBoundaryProcess*)(*pv)[i];
        break;
      }
    }
  }
#endif
}

WCSimTrajectory::WCSimTrajectory(WCSimTrajectory & right):G4VTrajectory()
{
  ParticleName = right.ParticleName;
  PDGCharge = right.PDGCharge;
  PDGEncoding = right.PDGEncoding;
  fTrackID = right.fTrackID;
  fParentID = right.fParentID;
  initialMomentum = right.initialMomentum;
  positionRecord = new TrajectoryPointContainer();
  
  stoppingPoint  = right.stoppingPoint;
  stoppingVolume = right.stoppingVolume;
  SaveIt = right.SaveIt;
  creatorProcess = right.creatorProcess;

  producesHit = right.producesHit;
  parentTrajectory = right.parentTrajectory;

  for(size_t i=0;i<right.positionRecord->size();i++)
  {
    G4TrajectoryPoint* rightPoint = (G4TrajectoryPoint*)((*(right.positionRecord))[i]);
    positionRecord->push_back(new G4TrajectoryPoint(*rightPoint));
  }
  globalTime = right.globalTime;

  boundaryPoints = right.boundaryPoints;
  boundaryKEs = right.boundaryKEs;
  boundaryTimes = right.boundaryTimes;
  boundaryTypes = right.boundaryTypes;

#ifdef WCSIM_SAVE_PHOTON_HISTORY
  pRayScatter = right.pRayScatter;
  pMieScatter = right.pMieScatter;
  pReflec = right.pReflec;
  fBoundary = right.fBoundary;
#endif
}

WCSimTrajectory::~WCSimTrajectory()
{
  //  positionRecord->clearAndDestroy();
  size_t i;
  for(i=0;i<positionRecord->size();i++){
    delete  (*positionRecord)[i];
  }
  positionRecord->clear();
  
  delete positionRecord;

  boundaryPoints.clear();
  boundaryKEs.clear();
  boundaryTimes.clear();
  boundaryTypes.clear();

  pReflec.clear();
  fBoundary = NULL;
}

void WCSimTrajectory::ShowTrajectory(std::ostream& os) const
{
  // Invoke the default implementation in G4VTrajectory...
  G4VTrajectory::ShowTrajectory(os);
  // ... or override with your own code here.
}

void WCSimTrajectory::DrawTrajectory(/*G4int i_mode*/) const
{
  // Invoke the default implementation in G4VTrajectory...
  G4VTrajectory::DrawTrajectory();
  // ... or override with your own code here.
}

const std::map<G4String,G4AttDef>* WCSimTrajectory::GetAttDefs() const
{
  G4bool isNew;
  std::map<G4String,G4AttDef>* store
    = G4AttDefStore::GetInstance("WCSimTrajectory",isNew);
  if (isNew) {
    
    G4String ID("ID");
    (*store)[ID] = G4AttDef(ID,"Track ID","Bookkeeping","","G4int");
    
    G4String PID("PID");
    (*store)[PID] = G4AttDef(PID,"Parent ID","Bookkeeping","","G4int");
    
    G4String PN("PN");
    (*store)[PN] = G4AttDef(PN,"Particle Name","Physics","","G4String");
    
    G4String Ch("Ch");
    (*store)[Ch] = G4AttDef(Ch,"Charge","Physics","","G4double");
    
    G4String PDG("PDG");
    (*store)[PDG] = G4AttDef(PDG,"PDG Encoding","Physics","","G4int");
    
    G4String IMom("IMom");
    (*store)[IMom] = G4AttDef(IMom, "Momentum of track at start of trajectory",
			      "Physics","","G4ThreeVector");
    
    G4String NTP("NTP");
    (*store)[NTP] = G4AttDef(NTP,"No. of points","Physics","","G4int");
    
  }
  return store;
}

std::vector<G4AttValue>* WCSimTrajectory::CreateAttValues() const
{
  char c[100];
  //std::ostrstream ss(c,100);
  std::ostringstream ss(c);
  
  std::vector<G4AttValue>* values = new std::vector<G4AttValue>;
  
  ss.seekp(std::ios::beg);
  ss << fTrackID << std::ends;
  values->push_back(G4AttValue("ID",c,""));
  
  ss.seekp(std::ios::beg);
  ss << fParentID << std::ends;
  values->push_back(G4AttValue("PID",c,""));
  
  values->push_back(G4AttValue("PN",ParticleName,""));
  
  ss.seekp(std::ios::beg);
  ss << PDGCharge << std::ends;
  values->push_back(G4AttValue("Ch",c,""));

  ss.seekp(std::ios::beg);
  ss << PDGEncoding << std::ends;
  values->push_back(G4AttValue("PDG",c,""));

  ss.seekp(std::ios::beg);
  ss << G4BestUnit(initialMomentum,"Energy") << std::ends;
  values->push_back(G4AttValue("IMom",c,""));

  ss.seekp(std::ios::beg);
  ss << GetPointEntries() << std::ends;
  values->push_back(G4AttValue("NTP",c,""));

  return values;
}

void WCSimTrajectory::AppendStep(const G4Step* aStep)
{
  positionRecord->push_back( new G4TrajectoryPoint(aStep->GetPostStepPoint()->
						   GetPosition() ));

  // Save boundary points
  G4StepPoint* thePrePoint = aStep->GetPreStepPoint();
  G4VPhysicalVolume* thePrePV = thePrePoint->GetPhysicalVolume();

  G4StepPoint* thePostPoint = aStep->GetPostStepPoint();
  G4VPhysicalVolume* thePostPV = thePostPoint->GetPhysicalVolume();

  if (thePrePV && thePostPV && thePrePV->GetName()!=thePostPV->GetName())
  {
    G4String thePrePVName = thePrePV->GetName();
    G4String thePostPVName = thePostPV->GetName();
    BoundaryType_t ty = kNull;
    if (thePrePVName.contains("BlackSheet") || thePostPVName.contains("BlackSheet")) ty = kBlackSheet;
    else if (thePrePVName.contains("Cave") || thePostPVName.contains("Cave")) ty = kCave;
    else if (thePrePVName.contains("Tyvek") || thePostPVName.contains("Tyvek")) ty = kTyvek;
    if (ty!=kNull)
    {
      const G4Track* track       = aStep->GetTrack();
      std::vector<G4float> bPs(3);
      bPs[0] = track->GetPosition().x(); bPs[1] = track->GetPosition().y(); bPs[2] = track->GetPosition().z();
      AddBoundaryPoint(bPs, track->GetKineticEnergy(), track->GetGlobalTime(), ty);
    }
  }

#ifdef WCSIM_SAVE_PHOTON_HISTORY
  // Add photon history
  if (PDGEncoding==0)
  {
    const G4VProcess* pds = thePostPoint->GetProcessDefinedStep();
    //G4cout<<"Having optical photon in AppendStep "<<pds->GetProcessName()<<G4endl;
    if ( pds->GetProcessType() == fOptical )
    {
      if ( pds->GetProcessSubType() == fOpRayleigh )
      {
        AddPhotonRayScatter(1);
      }
      else if ( pds->GetProcessSubType() == fOpMieHG )
      {
        AddPhotonMieScatter(1);
      }
    }
    else
    {
      if((fBoundary->GetStatus() >= FresnelReflection && fBoundary->GetStatus() <=BackScattering) || 
          (fBoundary->GetStatus() >= PolishedLumirrorAirReflection && fBoundary->GetStatus() <=GroundVM2000GlueReflection) ||
          fBoundary->GetStatus() == CoatedDielectricReflection
        )
      {
        G4String thePostPVName = thePostPV->GetName();
        ReflectionSurface_t rType = kOtherS;
        if (thePostPVName.contains("BlackSheet")) rType = kBlackSheetS;
        else if (thePostPVName.contains("reflector")) rType = kReflectorS;
        else if (thePostPVName.contains("InteriorWCPMT")) rType = kPhotocathodeS;
        AddPhotonReflection(rType);
      }
    }
  }
#endif
}

G4ParticleDefinition* WCSimTrajectory::GetParticleDefinition()
{
  return (G4ParticleTable::GetParticleTable()->FindParticle(ParticleName));
}

void WCSimTrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  if(!secondTrajectory) return;

  WCSimTrajectory* seco         = (WCSimTrajectory*)secondTrajectory;

  stoppingPoint  = seco->stoppingPoint;
  stoppingVolume = seco->stoppingVolume;

  producesHit |= seco->producesHit;
  
  G4int ent = seco->GetPointEntries();
  for(G4int i=1;i<ent;i++) // initial point of the second trajectory should not be merged
  { 
    positionRecord->push_back((*(seco->positionRecord))[i]);
    //    positionRecord->push_back(seco->positionRecord->removeAt(1));
  }
  delete (*seco->positionRecord)[0];
  seco->positionRecord->clear();

  ent = (seco->GetBoundaryPoints()).size();
  for (G4int i=0;i<ent;i++)
  {
    AddBoundaryPoint((seco->GetBoundaryPoints()).at(i),
                     (seco->GetBoundaryKEs()).at(i),
                     (seco->GetBoundaryTimes()).at(i),
                     (seco->GetBoundaryTypes()).at(i));
  }

#ifdef WCSIM_SAVE_PHOTON_HISTORY
  AddPhotonRayScatter(seco->GetPhotonRayScatter());
  AddPhotonMieScatter(seco->GetPhotonMieScatter());
  for (auto i: seco->GetPhotonReflection()) AddPhotonReflection(i);
#endif
}


