struct ntupleStruct
{
  int mode;             // interaction mode
  int nvtxs;           // number of vertices
  int vtxsvol[200];           // volume of vertices
  float vtxs[200][3];         // interaction vertices
  int vecRecNumber;     // info event number in inputvetcotfile
  int  jmu;             // index to muon
  int  jp;              // index to proton
  int npar;             // number of final state particles
  int ipnu[200];         // id of ith final state particle
  int flag[200];         // flag: -1 = incoming neutrino
                        //       -2 = target
                        //        1 = outgoing lepton
                        //        2 = most energetic outgoing nucleon
  float m[200];          // mass of ith final state particle
  float p[200];          // momentum of ith final state particle
  float E[200];          // energy of ith final state particle
  int startvol[200];      // starting volume of ith final state particle
  int stopvol[200];      // stopping volume of ith final state particle
  float dir[200][3];     // direction of ith final state particle
  float pdir[200][3];    // momentum-vector of ith final state particle
  float stop[200][3];    // stopping point of ith final state particle
  float start[200][3];   // starting point of ith final state particle
  int parent[200];        // ID of parent of ith particle (0 if primary)
  float time[200];       // creation time of the ith particle

  int   numTubesHit;        // Total number of tubes with hits
  int   totalPe[2000];       // The totalPE recorded at each tube
  float truetime[2000][90]; // The true time of each hit

  int   numDigitizedTubes;  // Number of PMTs with digitized hits
  float q[2000];             // The readout digitized pe
  float t[2000];             // The readout digitized time
  int tubeid[2000];        // The readout tube ID
  float sumq;             // sum of q(readout digitized pe) in event

  int   fvnumTubesHit;        // Total number of inner tubes with hits
  int   fvtotalPe[2000];       // The totalPE recorded at each innertube
  float fvtruetime[2000][90]; // The true time of each inner hit

  int   fvnumDigitizedTubes;  // Number of PMTs with digitized hits
  float fvq[2000];             // The readout digitized pe
  float fvt[2000];             // The readout digitized time
  int fvtubeid[2000];        // The readout tube ID
  float fvsumq;             // sum of q(readout digitized pe) in event
};

extern struct ntupleStruct jhfNtuple;

static const char* ntDesc =
"mode:I,"
"nvtxs[0,200]:I,"
"vtxsvol(npar):I,"
"vtxs(3,npar):R,"
"vecRecNumber:I,"
"jmu:I,"
"jp:I,"
"npar[0,200]:I,"
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
"numTubesHit[0,2000]:I,"        
"totalPe(numTubesHit):I,"
"truetime(90,numTubesHit):R,"
"numDigTubes[0,2000]:I,"
"q(numDigTubes):R,"            
"t(numDigTubes):R,"
"tubeid(numDigTubes):I"
"fvnumTubesHit[0,2000]:I,"        
"fvtotalPe(fvnumTubesHit):I,"
"fvtruetime(90,fvnumTubesHit):R,"
"fvnumDigTubes[0,2000]:I,"
"fvq(fvnumDigTubes):R,"            
"fvt(fvnumDigTubes):R,"
"fvtubeid(fvnumDigTubes):I";


