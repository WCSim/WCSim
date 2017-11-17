#include "WCSimEnumerations.hh"
struct ntupleStruct
{
  //int mode;             // interaction mode
  InteractionType_t mode;             // interaction mode
  int vtxvol;           // volume of vertex
  float vtx[3];         // interaction vertex
  int vecRecNumber;     // info event number in inputvetcotfile
  int  jmu;             // index to muon
  int  jp;              // index to proton
  int npar;             // number of final state particles
  int ipnu[50];         // id of ith final state particle
  int flag[50];         // flag: -1 = incoming neutrino
                        //       -2 = target
                        //        1 = outgoing lepton
                        //        2 = most energetic outgoing nucleon
  float m[50];          // mass of ith final state particle
  float p[50];          // momentum of ith final state particle
  float E[50];          // energy of ith final state particle
  int startvol[50];      // starting volume of ith final state particle
  int stopvol[50];      // stopping volume of ith final state particle
  float dir[50][3];     // direction of ith final state particle
  float pdir[50][3];    // momentum-vector of ith final state particle
  float stop[50][3];    // stopping point of ith final state particle
  float start[50][3];   // starting point of ith final state particle
  int parent[50];        // ID of parent of ith particle (0 if primary)
  float time[50];       // creation time of the ith particle

  int   numTubesHit;        // Total number of tubes with hits
  int   totalPe[500];       // The totalPE recorded at each tube
  float truetime[500][90]; // The true time of each hit

  int   numDigitizedTubes;  // Number of PMTs with digitized hits
  float q[500];             // The readout digitized pe
  float t[500];             // The readout digitized time
  int tubeid[500];        // The readout tube ID
  float sumq;             // sum of q(readout digitized pe) in event

  int   fvnumTubesHit;        // Total number of inner tubes with hits
  int   fvtotalPe[500];       // The totalPE recorded at each innertube
  float fvtruetime[500][90]; // The true time of each inner hit

  int   fvnumDigitizedTubes;  // Number of PMTs with digitized hits
  float fvq[500];             // The readout digitized pe
  float fvt[500];             // The readout digitized time
  int fvtubeid[500];        // The readout tube ID
  float fvsumq;             // sum of q(readout digitized pe) in event
};

extern struct ntupleStruct jhfNtuple;

/* Not used:
static const char* ntDesc =
"mode:I,"
"vtxvol:I,"
"vtx(3):R,"
"vecRecNumber:I,"
"jmu:I,"
"jp:I,"
"npar[0,50]:I,"
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
"numTubesHit[0,500]:I,"        
"totalPe(numTubesHit):I,"
"truetime(90,numTubesHit):R,"
"numDigTubes[0,500]:I,"
"q(numDigTubes):R,"            
"t(numDigTubes):R,"
"tubeid(numDigTubes):I"
"fvnumTubesHit[0,500]:I,"        
"fvtotalPe(fvnumTubesHit):I,"
"fvtruetime(90,fvnumTubesHit):R,"
"fvnumDigTubes[0,500]:I,"
"fvq(fvnumDigTubes):R,"            
"fvt(fvnumDigTubes):R,"
"fvtubeid(fvnumDigTubes):I";

*/
