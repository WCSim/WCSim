#ifndef JHFNTUPLE_H
#define JHFNTUPLE_H
static const int MAX_N_PRIMARIES = 900;
static const int MAX_N_ACTIVE_TUBES = 2000;
static const int MAX_N_HITS_PER_TUBE = 90;
struct ntupleStruct
{
  int mode;             // interaction mode
  int nvtxs;           // number of vertices
  int vtxsvol[MAX_N_PRIMARIES];           // volume of vertices
  float vtxs[MAX_N_PRIMARIES][3];         // interaction vertices
  int vecRecNumber;     // info event number in inputvetcotfile
  int  jmu;             // index to muon
  int  jp;              // index to proton
  int npar;             // number of final state particles
  int ipnu[MAX_N_PRIMARIES];         // id of ith final state particle
  int flag[MAX_N_PRIMARIES];         // flag: -1 = incoming neutrino
                        //       -2 = target
                        //        1 = outgoing lepton
                        //        2 = most energetic outgoing nucleon
  float m[MAX_N_PRIMARIES];          // mass of ith final state particle
  float p[MAX_N_PRIMARIES];          // momentum of ith final state particle
  float E[MAX_N_PRIMARIES];          // energy of ith final state particle
  int startvol[MAX_N_PRIMARIES];      // starting volume of ith final state particle
  int stopvol[MAX_N_PRIMARIES];      // stopping volume of ith final state particle
  float dir[MAX_N_PRIMARIES][3];     // direction of ith final state particle
  float pdir[MAX_N_PRIMARIES][3];    // momentum-vector of ith final state particle
  float stop[MAX_N_PRIMARIES][3];    // stopping point of ith final state particle
  float start[MAX_N_PRIMARIES][3];   // starting point of ith final state particle
  int parent[MAX_N_PRIMARIES];        // ID of parent of ith particle (0 if primary)
  float time[MAX_N_PRIMARIES];       // creation time of the ith particle

  int   numTubesHit;        // Total number of tubes with hits
  int   totalPe[MAX_N_ACTIVE_TUBES];       // The totalPE recorded at each tube
  float truetime[MAX_N_ACTIVE_TUBES][MAX_N_HITS_PER_TUBE]; // The true time of each hit

  int   numDigitizedTubes;  // Number of PMTs with digitized hits
  float q[MAX_N_ACTIVE_TUBES];             // The readout digitized pe
  float t[MAX_N_ACTIVE_TUBES];             // The readout digitized time
  int tubeid[MAX_N_ACTIVE_TUBES];        // The readout tube ID
  float sumq;             // sum of q(readout digitized pe) in event

  int   fvnumTubesHit;        // Total number of inner tubes with hits
  int   fvtotalPe[MAX_N_ACTIVE_TUBES];       // The totalPE recorded at each innertube
  float fvtruetime[MAX_N_ACTIVE_TUBES][MAX_N_HITS_PER_TUBE]; // The true time of each inner hit

  int   fvnumDigitizedTubes;  // Number of PMTs with digitized hits
  float fvq[MAX_N_ACTIVE_TUBES];             // The readout digitized pe
  float fvt[MAX_N_ACTIVE_TUBES];             // The readout digitized time
  int fvtubeid[MAX_N_ACTIVE_TUBES];        // The readout tube ID
  float fvsumq;             // sum of q(readout digitized pe) in event
};

extern struct ntupleStruct jhfNtuple;

static const char* ntDesc =
"mode:I,"
"nvtxs[0,MAX_N_PRIMARIES]:I,"
"vtxsvol(npar):I,"
"vtxs(3,npar):R,"
"vecRecNumber:I,"
"jmu:I,"
"jp:I,"
"npar[0,MAX_N_PRIMARIES]:I,"
"ipnu(npar):I,"
"flag(npar):I,"
"m(npar):R,"
"p(npar):R,"
"E(npar):R,"
"startvol(npar):I,"
"stopvol(npar):I,"
"dir(3,npar):R,"
"pdir(3,npar):R,"
"stoppos(3,npar):R,"
"startpos(3,npar):R,"
"parent(npar):I,"
"time(npar):R,"
"numTubesHit[0,MAX_N_ACTIVE_TUBES]:I,"        
"totalPe(numTubesHit):I,"
"truetime(MAX_N_HITS_PER_TUBE,numTubesHit):R,"
"numDigTubes[0,MAX_N_ACTIVE_TUBES]:I,"
"q(numDigTubes):R,"            
"t(numDigTubes):R,"
"tubeid(numDigTubes):I"
"fvnumTubesHit[0,MAX_N_ACTIVE_TUBES]:I,"        
"fvtotalPe(fvnumTubesHit):I,"
"fvtruetime(MAX_N_HITS_PER_TUBE,fvnumTubesHit):R,"
"fvnumDigTubes[0,MAX_N_ACTIVE_TUBES]:I,"
"fvq(fvnumDigTubes):R,"            
"fvt(fvnumDigTubes):R,"
"fvtubeid(fvnumDigTubes):I";
#endif


