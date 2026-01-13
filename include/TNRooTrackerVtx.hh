//
//
#ifndef NRooTrackerVtx_hh_seen
#define NRooTrackerVtx_hh_seen

#include <vector>

#include "TBits.h"
#include "TObjString.h"
#include "TObject.h"

#include "TJNuBeamFlux.hh"

using std::ostream;

namespace ND {

const int kNStdHepNPmax = 100;
const int kNStdHepIdxPx = 0;
const int kNStdHepIdxPy = 1;
const int kNStdHepIdxPz = 2;
const int kNStdHepIdxE = 3;
const int kNStdHepIdxX = 0;
const int kNStdHepIdxY = 1;
const int kNStdHepIdxZ = 2;
const int kNStdHepIdxT = 3;

const int kNEmaxvc = 100;
const int kNEmaxvert = 100;
const int kNEmaxvertp = 300;

///\brief This is a simple event class which is essentially an objectified
/// version of the NEUT nRooTracker output format.
///\details Because the nRooTracker is
/// forcing NEUT event into a GENIE defined storage care needs to be taken when
/// interpreting the meaning of various data members. For example the
/// StdHepStatus does not have a one to one mapping to GENIE StdHep status.
/// Using this class with GENIE based utils or the GENIE ReWeighting tools can
/// (and most likely will) result in false results!\n
/// Therefore, the native NEUT particle information common blocks
/// (VCWORK, FSIHIST) are included.
class NRooTrackerVtx : public JNuBeamFlux {
  static const int kNFMaxNucleonVert = 200;  ///<\brief Maximum possible number of saved NFNucleonVertices  . This is set to mirror the equivalent parameter in the NEUT   FSI hist header file nucleonfsihist.h

  static const int kNFMaxNucleonSteps = 2000;   ///\brief Maximum possible number of saved NFNucleonSteps  . This is set to mirror the equivalent parameter in the NEUT   FSI hist header file nucleonfsihist.h
 public:
  NRooTrackerVtx();
  ~NRooTrackerVtx();

  void Reset(void);
  void Init(void);

  // Using methods from TObject to remove 'hidden' compiler warnings
  using TObject::Copy;
  void Copy(const NRooTrackerVtx* event);
  void Print(const Option_t* option = "") const;

  //****************** Define the output rootracker tree branches

  TObjString* EvtCode;   ///< Generator-specific string with 'event code'

  int EvtNum;   ///< event num.

  double EvtXSec;   ///< cross section for selected event (1E-38 cm2) 

  double EvtDXSec;   ///< Cross section for selected event kinematics (1E-38 cm2 /{K^n}) 

  double EvtWght;   ///< Weight for event 

  double EvtProb;   ///< Probability  event (given cross section, path lengths, etc)

  double EvtVtx[4];   ///< Event vertex position in detector coord syst (SI) 

  int StdHepN;   ///< Number of particles in particle array


  //******************* stdhep-like particle array

  /// pdg codes (& generator specific codes for pseudoparticles)
  int* StdHepPdg;                    //[StdHepN] dg codes (& generator specific codes for pseudoparticles)
  int StdHepPdgTemp[kNStdHepNPmax];  //!
  /// generator-specific status code
  int* StdHepStatus;                    //[StdHepN] generator-specific status code
  int StdHepStatusTemp[kNStdHepNPmax];  //!
  double StdHepX4[kNStdHepNPmax][4];   ///< 4-x (x, y, z, t) of particle in hit nucleus frame (fm) 
  double StdHepP4[kNStdHepNPmax][4];   ///< 4-p (px,py,pz,E) of particle in LAB frame (GeV) 

  double StdHepPolz[kNStdHepNPmax][3];   ///< polarization vector 


  int StdHepFdTemp[kNStdHepNPmax];  //!
  int StdHepLdTemp[kNStdHepNPmax];  //!
  int StdHepFmTemp[kNStdHepNPmax];  //!
  int StdHepLmTemp[kNStdHepNPmax];  //!
  /// first daughter
  int* StdHepFd;  //[StdHepN] first daughter
  /// last daughter
  int* StdHepLd;  //[StdHepN]  last daughter
  /// first mother
  int* StdHepFm;  //[StdHepN]  first mother 
  /// last mother
  int* StdHepLm;  //[StdHepN]  last mother

  int NEnvc;   ///< NEUT native VCWORK information    Number of particles
  int NEipvcTemp[kNEmaxvc];  //!
  /// PDG particle code
  int* NEipvc;  //[NEnvc] PDG particle code
  float NEpvc[kNEmaxvc][3];   ///< 3-momentum (MeV/c) 

  int NEiorgvcTemp[kNEmaxvc];  //!
  /// Index of parent (Fortran convention: starting at 1)
  int* NEiorgvc;  //[NEnvc] Index of parent (Fortran convention: starting at 1)

  int NEiflgvcTemp[kNEmaxvc];  //!
  int NEicrnvcTemp[kNEmaxvc];  //!
  ///\brief Flag of final state
  /// Values:
  /// - 0 : DETERMINED LATER PROCEDURE
  /// - 1 : DECAY TO OTHER PARTICLE
  /// - 2 : ESCAPE FROM DETECTOR
  /// - 3 : ABSORPTION
  /// - 4 : CHARGE EXCHANGE
  /// - 5 : STOP AND NOT CONSIDER IN M.C.
  /// - 6 : E.M. SHOWER
  /// - 7 : HADRON PRODUCTION
  /// - 8 : QUASI-ELASTIC SCATTER
  /// - 9 : FORWARD (ELASTIC-LIKE) SCATTER
  int* NEiflgvc;  //[NEnvc]  Flag of final state 
  /// Escaped nucleus (1) or not (0)
  int* NEicrnvc;  //[NEnvc] Escaped nucleus (1) or not (0) 

  //******** Rest of the NEUT variables below are mainly for internal
  //********************** reweighting routines

  float NEcrsx;///<\brief Cross section calculation variables (x) (currently used for coherent  interactions) 
  float NEcrsy; ///<\brief Cross section calculation variables (y) (currently used for coherent  interactions) 
  float NEcrsz; ///<\brief Cross section calculation variables  (z) (currently used for coherent    interactions) 
  float NEcrsphi; ///<\brief Cross section calculation variables (phi) (currently used for coherent    interactions) 



  //**************** NEUT FSIHIST pion interaction history

  int NEnvert; ///< Number of vertices (including production and exit points)
  /// Position of vertex within nucleus (fm) 
  float NEposvert[kNEmaxvert][3]; ///< Position of vertex within nucleus (fm)

  int NEiflgvertTemp[kNEmaxvert];  //!
  ///\brief Interaction type
  /// Values:
  /// - (*10 FOR HI-NRG interaction, >~400 MeV/c)
  /// - -1 : ESCAPE
  /// - 0 : INITIAL (or unmatched parent vertex if I>1)
  /// - 3 : ABSORPTION
  /// - 4 : CHARGE EXCHANGE
  /// - 7 : HADRON PRODUCTION (hi-nrg only, i.e. 70)
  /// - 8 : QUASI-ELASTIC SCATTER
  /// - 9 : FORWARD (ELASTIC-LIKE) SCATTER
  int* NEiflgvert;  //[NEnvert]  Interaction type

  int NEnvcvert;   ///< Number of intermediate particles (including initial and final)

  float NEdirvert[kNEmaxvertp][3];   ///< Direction of particle 


  float NEabspvertTemp[kNEmaxvertp];   //! Storage area for NEabspvert read from the RooTracker TTree
  float NEabstpvertTemp[kNEmaxvertp];  //! Storage area for NEabtpvert read from the RooTracker TTree
  int NEipvertTemp[kNEmaxvertp];       //! Storage area for NEipvert read from the RooTracker TTree
  int NEivertiTemp[kNEmaxvertp];       //! Storage area for NEiverti read from the RooTracker TTree
  int NEivertfTemp[kNEmaxvertp];       //! Storage area for NEipvertf read from the RooTracker TTree

  /// Absolute momentum in the lab frame (MeV/c) 
  float* NEabspvert;  //[NEnvcvert] Absolute momentum in the lab frame (MeV/c) 
  /// Absolute momentum in the nucleon rest frame (MeV/c) 
  float* NEabstpvert;  //[NEnvcvert] Absolute momentum in the nucleon rest frame (MeV/c)  
  /// PDG particle code
  int* NEipvert;  //[NEnvcvert] PDG particle code 
  /// Index of initial vertex (pointing to nvert array above)
  int* NEiverti;  //[NEnvcvert] Index of initial vertex (pointing to nvert array above) 
  /// Index of final vertex (pointing to nvert array above)
  int* NEivertf;  //[NEnvcvert] Index of final vertex (pointing to nvert array above)

  //**************** NEUT FSIHIST nucleon interaction history

  int NFnvert;   ///<\brief Number of "vertices"
  ///< Remarks:
  ///<   - a "vertex" is actually better described as a start, end or
  ///<     scattering point of a track
  ///<   - at each scattering point, the first nucleon will be followed in
  ///<     the same track, while the
  ///<     second one will create a new track
  ///<   - each track consists of a series of consecutive vertices. The first
  ///<     vertex has P=0, the last P=4. In between may be any number
  ///<     (including 0) vertices where an actual scattering
  ///<     took place (P=1,2,3).
  ///<   - it is not possible (and not needed) to connect the second track
  ///<     of a scattering vertex with the original one. Note that "first" and
  ///<     "second" is purely arbitrary. For nucleon FSI uncertainties,
  ///<     only the probabilities of the scattering processes have to be
  ///<     calculated, so it is not important to know which tracks belong to
  ///<     each other.

  ///<\brief 4-digit flag for interaction type at i-th vertex, in the form
  ///<"BNTP":
  ///< Values:
  ///< - N: charge nucleon propagated through nucleus (0 = neutron, 1 = proton)
  ///< - T: charge "target" nucleon the interaction is taking place on
  ///< - P: scattering process:
  ///<     - P=0: start tracking of nucleon (i.e. gets "created")
  ///<     - P=1: elastic scattering
  ///<     - P=2: single pion production
  ///<     - P=3: double pion production
  ///<     - P=4: stop tracking of nucleon (i.e. leaves nucleus)
  ///< - B: Pauli blocking flag (0 = not blocked, 1 = interaction was Pauli
  ///<      blocked and actually did not take place)
  ///< Examples:
  ///<    - 103 means double pion production when a proton scattered on a neutron
  ///<    - 1011 means elastic scattering of a neutron on a proton did not take
  ///<     place due to Pauli blocking
  ///< \note For P=0 and P=4, "T" is without meaning and always set to 0. 
  int* NFiflag;  //[NFnvert]  4-digit flag for interaction type at i-th vertex

  /// x-component of i-th vertex position inside nucleus
  float* NFx;  //[NFnvert]  x-component of i-th vertex position inside nucleus
  /// y-component of i-th vertex position inside nucleus
  float* NFy;  //[NFnvert] y-component of i-th vertex position inside nucleus
  ///z-component of i-th vertex position inside nucleus 
  float* NFz;  //[NFnvert] z-component of i-th vertex position inside nucleus 
  ///x-component of momentum of nucleon leaving the i-th vertex
  float* NFpx;  //[NFnvert] x-component of momentum of nucleon leaving the i-th vertex
  /// y-component of momentum of nucleon leaving the i-th vertex
  float* NFpy;  //[NFnvert] y-component of momentum of nucleon leaving the i-th vertex
  /// z-component of momentum of nucleon leaving the i-th vertex 
  float* NFpz;  //[NFnvert] z-component of momentum of nucleon leaving the i-th vertex 
  ///energy of nucleon leaving the i-th vertex 
  float* NFe;  //[NFnvert] energy of nucleon leaving the i-th vertex 
  ///
  int* NFfirststep;  //[NFnvert]  first step index of this track (to obtain the CMS energies for each step)
  int NFnstep;   ///< number of steps
 
  /// CMS energy squared of collision at k-th step (i.e. before interacting).
  /// The sign of this value indicates the charge of the target nucleon:
  ///  NFecms2 > 0: proton,  NFecms2 < 0: neutron (same as "T" in NFiflag)
  float* NFecms2;  //[NFnstep] CMS energy squared of collision at k-th step (i.e. before interacting).

  //******Tree Readout fixed size arrays
  int NFiflagTEMP[kNFMaxNucleonVert];  //!  Temporary RooTracker Tree readout variable, corresponds to NFiflag

  float NFxTEMP[kNFMaxNucleonVert];  //!   Temporary RooTracker Tree readout variable, corresponds to NFx

  float NFyTEMP[kNFMaxNucleonVert];  //!   Temporary RooTracker Tree readout variable, corresponds to NFy

  float NFzTEMP[kNFMaxNucleonVert];  //!   Temporary RooTracker Tree readout variable, corresponds to NFz

  float NFpxTEMP[kNFMaxNucleonVert];  //!   Temporary RooTracker Tree readout variable, corresponds to NFpx

  float NFpyTEMP[kNFMaxNucleonVert];  //!   /Temporary RooTracker Tree readout variable, corresponds to NFpy

  float NFpzTEMP[kNFMaxNucleonVert];  //!   Temporary RooTracker Tree readout variable, corresponds to NFpz

  float NFeTEMP[kNFMaxNucleonVert];  //!   Temporary RooTracker Tree readout variable, corresponds to NFe

  int NFfirststepTEMP[kNFMaxNucleonVert];  //!   Temporary RooTracker Tree readout variable, corresponds to NFfirststep

  float NFecms2TEMP[kNFMaxNucleonSteps];  //!   Temporary RooTracker Tree readout variable, corresponds to NFecms2

//
  // Extra nucleon fsi history (incl. step info) for systematic reweighting
  // and SPIDelta info added 2022-03-13 by S. Dolan
  //

  
  float PCascProb;///< The overall probability for the nucelon FSI cascade. Used for nucleon FSI reweighting.
  ///< It is the product of the probability of each step.
  ///< This is the variable we actually use to calculate a nucleon FSI reweight
  ///< (we take the ratio of this for nominal and new dial values).

  /// A step-wise probability for the nucleon FSI cascade. 
  /// Not directly used for nucleon FSI reweighting. 
  float* Prob;  //[NFnstep] A step-wise probability for the nucleon FSI cascade. 
  
  /// Gives the interaction type of a step:
  /// 5 = non event, 4 = survival, 3 = douple pi, 2 = single pi, 1 = elastic
  /// Not used for nucleon FSI reweighting. 
  float* VertFlagStep;  //[NFnstep] Gives the interaction type of a step: 5 = non event, 4 = survival, 3 = douple pi, 2 = single pi, 1 = elastic

  /// Prob multiplied by rhon from nrfermi.F in NEUT. Apparently this might be useful one day. 
  /// Not used for nucleon FSI reweighting. 
  float* VertFsiRhon;  //[NFnstep] Prob multiplied by rhon from nrfermi.F in NEUT.

  /// A step-wise probability for elastic nucleon scatter scattering.
  /// This is by far the most common interaction.
  /// Used for nucleon FSI reweighting.
  float* StepPel;  //[NFnstep] A step-wise probability for elastic nucleon scatter scattering.

  /// A step-wise probability for single pion production via nucleon FSI.
  /// This rarely happens.
  /// Used for nucleon FSI reweighting.
  float* StepPsp;  //[NFnstep] A step-wise probability for single pion production via nucleon FSI.

  /// A step-wise probability for double pion production via nucleon FSI.
  /// This almost never happens.
  /// Used for nucleon FSI reweighting.
  float* StepPdp;  //[NFnstep]  A step-wise probability for double pion production via nucleon FSI.

  /// A step-wise scaling factor for Pauli blocking when using hybrid or full scale factor PB.
  /// Used for nucleon FSI reweighting.
  float* StepScaleFactor;  //[NFnstep]  A step-wise scaling factor for Pauli blocking when using hybrid or full scale factor PB.

  /// A step-wise kinetic energy of the incoming nucleon for an FSI step.
  /// Used for nucleon FSI reweighting.
  float* StepEkin;  //[NFnstep]  A step-wise kinetic energy of the incoming nucleon for an FSI step.

  float ProbTEMP[kNFMaxNucleonSteps];  //!   Temporary RooTracker Tree readout variable, corresponds to Prob

  float VertFlagStepTEMP[kNFMaxNucleonSteps];  //!   Temporary RooTracker Tree readout variable, corresponds to VertFlagStep   

  float VertFsiRhonTEMP[kNFMaxNucleonSteps];  //!  Temporary RooTracker Tree readout variable, corresponds to VertFsiRhon  

  float StepPelTEMP[kNFMaxNucleonSteps];  //!    Temporary RooTracker Tree readout variable, corresponds to StepPel  

  float StepPspTEMP[kNFMaxNucleonSteps];  //!    Temporary RooTracker Tree readout variable, corresponds to StepPsp  

  
  float StepPdpTEMP[kNFMaxNucleonSteps];  //! Temporary RooTracker Tree readout variable, corresponds to StepPdp 

  float StepScaleFactorTEMP[kNFMaxNucleonSteps];  //! Temporary RooTracker Tree readout variable, corresponds to StepScaleFactor 

  float StepEkinTEMP[kNFMaxNucleonSteps];  //! Temporary RooTracker Tree readout variable, corresponds to StepEkin

  
  int SPIDelta; ///< SPIDelta flag for resonance decay reweighting
  ///< This is documented in T2K-TN-414
  ///< 0 means the resonance decay was sampled isotropicaly 
  ///< 1 means the resonance decay was sampled like a Delta resonance 
  ///< 2 means the resonance decay was sampled including multiple interfering resonances
  ///< 3 means an old NEUT defauly which is a mix of 0 and 1  

  //
  // End of new stuff added by S. Dolan
  //

  int IRadCorrPht; ///< Event-level flag of whether real photon production was
                     ///< added





  //**************** NEUT Passthrough info

  TObjString* GeomPath;  ///< Geometry path of where the vertex is
  // Some pass through info
  TObjString* GeneratorName; ///< NEUT
  TObjString* OrigFileName;///< Filename of the NEUTmc file
  TObjString* OrigTreeName;///< Tree name within the NEUT mc file 
  int OrigEvtNum; ///< Event number within the NEUT mc file 
  int OrigTreeEntries;  ///< Number of entries in the  NEUT mc file 
  double OrigTreePOT;  ///< Number of POT in the  NEUT mc file 
  double TimeInSpill; ///< Time of the vertex, relative to spill time

  int TruthVertexID; ///< A link to the TTruthVertex tree

  ClassDef(NRooTrackerVtx, 3);
};

}
#endif
