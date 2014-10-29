#ifndef WCSimOpticalPhotonTrackInfo_h
#define WCSimOpticalPhotonTrackInfo_h 1

#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4VParticleChange.hh"
#include "G4SteppingVerbose.hh"
#include "G4SteppingManager.hh"
#include "G4PVParameterised.hh"
#include "G4PVReplica.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4ParticleTypes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4Event.hh"
#include "G4StepPoint.hh"
#include "G4VUserTrackInformation.hh"
#include "WCSimOpticalPhotonMessenger.hh"
#include <vector>

class WCSimOpticalPhotonTrackInfo : public G4VUserTrackInformation
{
private:

    static WCSimOpticalPhotonTrackInfo *s_instance;
    // parent particle information
    std::vector<int>             fPId;  //<Geant particle type of parent
    std::vector<G4ThreeVector>   fPPos; //<Initial position of parent particle
    std::vector<G4ThreeVector>   fPDir; //<Initial direction of parent particle
  
    // optical photon info
    std::vector<int>             fParentIdx;//<Index into info on parent particle of photon
    std::vector<G4ThreeVector>   fPos;      //< Initial position of photon
    std::vector<int>             fId;       //< Geant4 Id number for each optical photon
    std::vector<int>             fIstory;   //< ISTORY=(# of refl)*1000+(# of scat)
    std::vector<int>             fPMTId;    //< PMT number if photon hit PMT, otherwise -1
    std::vector<G4ThreeVector>   fPMTPos;   //< PMT position (0,0,0 if no pmt hit)
    std::vector<G4ThreeVector>   fOpDir;    //< Initial direction of optical particle
    
    bool enabled;
    
    WCSimOpticalPhotonTrackInfo(){ enabled = false; 
                                   messenger = new WCSimOpticalPhotonMessenger(this);
                                   std::cout << "Constructed" << std::endl;}
    WCSimOpticalPhotonMessenger* messenger;
    ~WCSimOpticalPhotonTrackInfo(){delete messenger;}

  public:
    
  
    void reset(); //< reset arrays of photon information
    void Print(); //< print all info stored in this class.
    void enable(){ enabled = true; }
    void disable(){ enabled = false; }
    bool isEnabled(){ return enabled; }
    
    void UserSteppingAction( const G4Step * aStep );
    
    //Parent particle methods
    void addPId(int pId); //Adds parent Id to fPId vector
    void addPPos(G4ThreeVector pPos); //Adds parent position to fPPos vector
    void addPDir(G4ThreeVector pDir); //Adds parent direction to fPDir vector
    std::vector<int> getPId(); //Returns the fPId vector
    std::vector<G4ThreeVector> getPPos(); //Returns the fPPos vector
    std::vector<G4ThreeVector> getPDir(); //Returns the fPDir vector
    int PIdExists( int pId ); // return index into vector if parent Id exists, otherwise -1
  
    
    //Optical photon methods
    void addParentIdx(int parentIdx); //Adds the index of parent particle to fParentIdx vector
    void addPos(G4ThreeVector pos); //Adds position of particle to fPos vector
    void addId(int id); //Adds id of particle to fId vector
    void addIstory(int istory); //Adds ISTORY ((# of refl*1000+(# of scat)) to fIstory vector
    void addPMTId(int pmtId); //Adds the PMT id that the particle hit, if any
    void addPMTPos(G4ThreeVector pos); //Adds the PMT position that the particle hit, if any
    void addOpDir(G4ThreeVector dir); //Adds op particle direction to fOpDir vector
    
    void setParentIdx(int idx, int parentIdx); //Sets the index of parent particle to fParentIdx vector
    void setPos(int idx, G4ThreeVector pos); //Sets position of particle to fPos vector
    void setId(int idx, int id); //Sets id of particle to fId vector
    void setIstory(int idx, int istory); //Sets ISTORY ((# of refl*1000+(# of scat)) to fIstory vector
    void incScatters( int idx );     // increment number of scatters for photon at idx
    void incReflections( int idx);   // increment number of reflections for photon at idx
    void incOther( int idx){ fIstory[idx] += 1000000; }
    void setPMTId(int idx, int pmtId); //Sets the PMT id that the particle hit, if any
    void setPMTPos(int idx, G4ThreeVector pos); //Sets the PMT position that the particle hit, if any
    void setOpDir(int idx, G4ThreeVector dir); //Sets the op particle direction at index idx

    int getNumPhotons() { return fPMTId.size(); }
    std::vector<int> getParentIdx();
    std::vector<G4ThreeVector> getPos();     
    std::vector<int> getId();       
    std::vector<int> getIstory();   
    std::vector<int> getPMTId(); 
    int  IdExists( int pId ); // return index into vector if track Id exists, otherwise -1
    std::vector<G4ThreeVector> getPMTPos();
    std::vector<G4ThreeVector> getOpDir(); //returns fOpDir vector
    static WCSimOpticalPhotonTrackInfo *instance(){
        if (!s_instance)
          s_instance = new WCSimOpticalPhotonTrackInfo;
        return s_instance;
    }
  
     
      
  

};


#endif

