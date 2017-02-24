//
//
#ifndef NRooTrackerVtx_hh_seen
#define NRooTrackerVtx_hh_seen

#include <iostream>
#include <vector>

#include "TObject.h"
#include "TBits.h"
#include "TObjString.h"

#include "TJNuBeamFlux.hh"

using std::ostream;

///
/// This is a simple event class which is essentially an objectified version
/// of the NEUT nRooTracker output format. Because the nRooTracker is forcing 
/// NEUT event into a GENIE defined storage care needs to be taken when
/// interpreting the meaning of various data members. For example the StdHepStatus
/// does not have a one to one mapping to GENIE StdHep status. Using this class 
/// with GENIE based utils or the GENIE ReWeighting tools can (and most likely
/// will result in false results!
///
/// Therefore, the native NEUT particle information common blocks (VCWORK, FSIHIST)
/// are included. Analysis tools under development...

/// DBrailsford 10-2013: All 1D arrays are now in a dynamic format (rather than fixed length). 
/// Due to the class structure of the oaAnalysis NRooTracker object being used to extract the 
/// information from the numc file, two versions of the 1D arrays are initialised.  
/// Each array has a dynamic version, and a 'Temp' fixed length version.  The 'Temp' version 
/// extracts the info from the numc object and feeds the information in the dynamic version.
/// The 'Temp' version of each data member is NOT saved to the rooTracker tree, only the dynamic version is.
/// Note that for each new event, only the 'Temp' version of the data member is reset to a default value.
/// This current implementation reduces the NRooTracker tree by ~10Mb.

const int kNStdHepNPmax = 100;
const int kNStdHepIdxPx = 0;
const int kNStdHepIdxPy = 1;
const int kNStdHepIdxPz = 2;
const int kNStdHepIdxE  = 3;
const int kNStdHepIdxX  = 0;
const int kNStdHepIdxY  = 1;
const int kNStdHepIdxZ  = 2;
const int kNStdHepIdxT  = 3;

const int kNEmaxvc = 100;
const int kNEmaxvert = 100;
const int kNEmaxvertp = 300;

class NRooTrackerVtx : public JNuBeamFlux {

    public:
        NRooTrackerVtx();
        ~NRooTrackerVtx();

        void Reset   (void);
        void Init    (void);

        // Using methods from TObject to remove 'hidden' compiler warnings
        using TObject::Copy;
        using TObject::Print;

        void Copy    (const NRooTrackerVtx * event);
        void Print (const Option_t* = "") const;

        // Define the output rootracker tree branches
        TObjString* EvtCode;                         ///< generator-specific string with 'event code'
        int         EvtNum;                          ///< event num.
        double      EvtXSec;                         ///< cross section for selected event (1E-38 cm2) CORRECT
        double      EvtDXSec;                        ///< cross section for selected event kinematics (1E-38 cm2 /{K^n}) CORRECT
        double      EvtWght;                         ///< weight for that event CORRECT
        double      EvtProb;                         ///< probability for that event (given cross section, path lengths, etc) CORRECT
        double      EvtVtx[4];                       ///< event vertex position in detector coord syst (SI) CORRECT
        int         StdHepN;                         ///< number of particles in particle array 
        //
        //  stdhep-like particle array
        //
        int*        StdHepPdg; //[StdHepN] ///< pdg codes (& generator specific codes for pseudoparticles)
        int         StdHepPdgTemp   [kNStdHepNPmax];     //!

        int*        StdHepStatus; //[StdHepN] ///< generator-specific status code 
        int         StdHepStatusTemp[kNStdHepNPmax];     //!

        double      StdHepX4    [kNStdHepNPmax][4];  ///< 4-x (x, y, z, t) of particle in hit nucleus frame (fm) CORRECT

        double      StdHepP4    [kNStdHepNPmax][4];  ///< 4-p (px,py,pz,E) of particle in LAB frame (GeV) CORRECT

        double      StdHepPolz  [kNStdHepNPmax][3];  ///< polarization vector CORRECT

        int  StdHepFdTemp    [kNStdHepNPmax];     //!
        int  StdHepLdTemp    [kNStdHepNPmax];     //!
        int  StdHepFmTemp    [kNStdHepNPmax];     //!
        int  StdHepLmTemp    [kNStdHepNPmax];     //!
        int*  StdHepFd;    //[StdHepN] ///< first daughter
        int*  StdHepLd;    //[StdHepN] ///< last daughter
        int*  StdHepFm;    //[StdHepN] ///< first mother 
        int*  StdHepLm;    //[StdHepN] ///< last mother

        // NEUT native VCWORK information
        int    NEnvc;                         ///< Number of particles 

        int    NEipvcTemp[kNEmaxvc];          //! PDG particle code
        int*   NEipvc; //[NEnvc]              ///< PDG particle code

        float  NEpvc[kNEmaxvc][3];            ///< 3-momentum (MeV/c) CORRECT

        int    NEiorgvcTemp[kNEmaxvc];        //!
        int*   NEiorgvc; //[NEnvc]            ///<  Index of parent (Fortran convention: starting at 1)


        /// Flag of final state:   
        ///     0 : DETERMINED LATER PROCEDURE
        ///     1 : DECAY TO OTHER PARTICLE
        ///     2 : ESCAPE FROM DETECTOR
        ///     3 : ABSORPTION
        ///     4 : CHARGE EXCHANGE
        ///     5 : STOP A NOT CONSIDER IN M.C. 
        ///     6 : E.M. SHOWER
        ///     7 : HADRON PRODUCTION
        ///     8 : QUASI-ELASTIC SCATTER
        ///     9 : FORWARD (ELASTIC-LIKE) SCATTER

        int    NEiflgvcTemp[kNEmaxvc];          //! 
        int    NEicrnvcTemp[kNEmaxvc];          //!
        int*    NEiflgvc; //[NEnvc]             ///< Flag of final state         
        int*    NEicrnvc; //[NEnvc]             ///< Escaped nucleus (1) or not (0)


        // Rest of the NEUT variables below are mainly for internal reweighting routines 

        float NEcrsx;    	///< Cross section calculation variables (currently used for coherent interactions) CORRECT

        float NEcrsy;    	///< Cross section calculation variables (currently used for coherent interactions) CORRECT

        float NEcrsz;    	///< Cross section calculation variables (currently used for coherent interactions) CORRECT

        float NEcrsphi;    	///< Cross section calculation variables (currently used for coherent interactions) CORRECT


        // NEUT FSIHIST pion interaction history 
        int    NEnvert;                     ///< Number of vertices (including production and exit points)
        float  NEposvert[kNEmaxvert][3];    ///< Position of vertex within nucleus (fm) CORRECT

        ///< Interaction type 
        ///< (*10 FOR HI-NRG interaction, >~400 MeV/c)
        ///<     -1 : ESCAPE
        ///<     0 : INITIAL (or unmatched parent vertex if I>1)
        ///<     3 : ABSORPTION
        ///<     4 : CHARGE EXCHANGE
        ///<     7 : HADRON PRODUCTION (hi-nrg only, i.e. 70)
        ///<     8 : QUASI-ELASTIC SCATTER
        ///<     9 : FORWARD (ELASTIC-LIKE) SCATTER 

        int    NEiflgvertTemp[kNEmaxvert];      //!
        int*   NEiflgvert; //[NEnvert]          ///< Interaction type



        int    NEnvcvert;                   ///< Number of intermediate particles (including initial and final)
        float  NEdirvert[kNEmaxvertp][3];   ///< Direction of particle CORRECT

        float  NEabspvertTemp[kNEmaxvertp];     //! 
        float  NEabstpvertTemp[kNEmaxvertp];    //! 
        int    NEipvertTemp[kNEmaxvertp];       //! 
        int    NEivertiTemp[kNEmaxvertp];       //! 
        int    NEivertfTemp[kNEmaxvertp];       //! 

        float*  NEabspvert;  //[NEnvcvert]   ///<  Absolute momentum in the lab frame (MeV/c) CORRECT
        float*  NEabstpvert; //[NEnvcvert]   ///<  Absolute momentum in the nucleon rest frame (MeV/c) CORRECT
        int*    NEipvert;    //[NEnvcvert]   ///<  PDG particle code
        int*    NEiverti;    //[NEnvcvert]   ///<  Index of initial vertex (pointing to nvert array above)
        int*    NEivertf;    //[NEnvcvert]   ///<  Index of final vertex (pointing to nvert array above)

        //
        // etc 
        //
        TObjString* GeomPath;                        //

        // Some pass through info
        TObjString* GeneratorName;                   //
        TObjString* OrigFileName;                   //
        TObjString* OrigTreeName;                   //
        int OrigEvtNum;
        int OrigTreeEntries;
        double OrigTreePOT;
        double TimeInSpill;

        int TruthVertexID;

        ClassDef(NRooTrackerVtx, 1);
};

#endif
