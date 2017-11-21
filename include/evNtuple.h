// Should at some point replace jhfNtuple, but that is now a global, extern,
// while it should be a member variable. I don't want to break WCSim too much yet.

// Makes filling branches from EventAction to RunAction very easy, while keeping flat structure.
#include "WCSimEnumerations.hh"

struct eventNtuple{

  //event info:
  InteractionType_t interaction_mode;          // interaction mode
  Char_t vtxVolume[100];         // volume of vertex
  //already flatten array because have to initialize index by index anyway
  double vtx_x;                  // interaction vertex
  double vtx_y;
  double vtx_z;

  // Beam and target move to eventInfo from Tracks? 
  // Both don't have start and stop, only vertex and PiD and direction and flag
  // While pGun is one primary with start and stop if generated inside the detector




  //tracks:
  int nTracks;             // number of final state particles
  int pid[100];             // id of ith final state particle
  int flag[100];            // flag: -1 = beam particle (incoming neutrino)
                           //       -2 = target
                           //        0 = pGun or Cosmic primary
                           //        1 = outgoing lepton
                           //        2 = most energetic outgoing nucleon
  float mass[100];          // mass of ith final state particle
  float p[100];             // momentum of ith final state particle
  float energy[100];        // energy of ith final state particle
  int startvol[100];      // starting volume of ith final state particle
  int stopvol[100];      // stopping volume of ith final state particle
  float dir_x[100];     // direction of ith final state particle
  float dir_y[100];     // direction of ith final state particle
  float dir_z[100];     // direction of ith final state particle
  float pdir_x[100];    // momentum-vector of ith final state particle
  float pdir_y[100];    // momentum-vector of ith final state particle
  float pdir_z[100];    // momentum-vector of ith final state particle
  float start_x[100];   // starting point of ith final state particle
  float start_y[100];   // starting point of ith final state particle
  float start_z[100];   // starting point of ith final state particle

  float stop_x[100];    // stopping point of ith final state particle
  float stop_y[100];    // stopping point of ith final state particle
  float stop_z[100];    // stopping point of ith final state particle
  float length[100];    // TrackLength

  int parent[100];      // ID of parent of ith particle (0 if primary)
  int trackID[100];     // trackID of ith particle (1 if primary)
  float time[100];      // creation time of the ith particle
  //ToDo: add distance start to wall and stop to wall



  //hits: from WCDC_hits (= WCHC + tagged noise)
  // ToDo: extend to OD/ID when OD is implemented
  int  numTubesHit;           // Total number of tubes with hits
  int  numTubesHit_noNoise;   // Total number of tubes with physics hits
  int totalNumHits;           // hits per event = hits per Tube x numTubesHit
  int totalNumHits_noNoise;   // hits per event = hits per Tube x numTubesHit
  

  int   totalPe[10000000];        // The totalPE recorded at each tube
  int   totalPe_noNoise[10000000];// The totalPE recorded at each tube, without DN
  float truetime[10000000];      // The true time of each hit
  int   vector_index[10000000];  // Index in vector of hits
  int   tubeid[10000000];        // Readout tube ID
  int   mPMTid[10000000];        // Readout tube mPMT ID
  int   mPMT_pmtid[10000000];    // Readout tube mPMT-PMT ID
  int   parentid[10000000];      // Track ID of originating parent, 1 = initial primary particle ( NOT parentID of parent!!)
  int   trackid[10000000];       // Track ID 
  // Optional?
  float   tube_x[10000000];      // Tube position
  float   tube_y[10000000];        
  float   tube_z[10000000];        
  float   tube_dirx[10000000];   // Hit tube orientation
  float   tube_diry[10000000];        
  float   tube_dirz[10000000];        
  

  //digits: from WCDC (= WCDC_hits: but triggered and digitized)
  // ToDo: extend to OD/ID when OD is implemented
  int  numDigiTubesHit;     // Number of PMTs with digitized hits
  int totalNumDigiHits;     // Digitized hits per event = hits per Tube x numTubesHit
  float  sumq;              // Sum of q(readout digitized pe) in event

  float q[10000000];                  // The readout digitized pe
  float t[10000000];                  // The readout digitized time
  int   digivector_index[10000000];   // Index in vector of digits
  int   digitubeid[10000000];         // Readout tube ID
  int   digimPMTid[10000000];             // Readout tube mPMT ID
  int   digimPMT_pmtid[10000000];         // Readout tube mPMT-PMT ID
  // Optional?
  float   digitube_x[10000000];       // Tube position
  float   digitube_y[10000000];        
  float   digitube_z[10000000];        
  float   digitube_dirx[10000000];    // Hit tube orientation
  float   digitube_diry[10000000];        
  float   digitube_dirz[10000000];        
  // Save DigiCompositionInfo? Not now: ToDo, needs to be a 2D array, even in flat_root

};
