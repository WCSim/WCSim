#ifndef WCSimDetectorConstruction_H
#define WCSimDetectorConstruction_H 1

#include "WCSimPmtInfo.hh"

#include "G4Transform3D.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4OpticalSurface.hh"
#include "globals.hh"

#include <fstream>
#include <map>
#include <vector>
//#include <hash_map.h>
// warning : hash_map is not part of the standard
#include <ext/hash_map>


using __gnu_cxx::hash;
using __gnu_cxx::hashtable;
using __gnu_cxx::hash_map;
using __gnu_cxx::hash_multimap;

// (JF) We don't need this distinction for DUSEL
//enum cyl_location {endcap1,wall,endcap2};

class G4Box;
class G4Tubs;
class G4Material;
class G4LogicalVolume;
class G4AssemblyVolume;
class G4VPhysicalVolume;
class WCSimTuningParameters;
class WCSimDetectorMessenger;
class WCSimWCSD;
class WCSimLC;
class WCSimWLSP_LC;
class WCSimWLSP_Reflector;

namespace __gnu_cxx  {
  template<> struct hash< std::string >
  {
    size_t operator()( const std::string& x ) const
    {
      return hash< const char* >()( x.c_str() );
    }
  };
}

class WCSimDetectorConstruction : public G4VUserDetectorConstruction
{
public:

  WCSimDetectorConstruction(G4int DetConfig,WCSimTuningParameters* WCSimTuningPars);
  ~WCSimDetectorConstruction();
  
  G4VPhysicalVolume* Construct();

  // Related to the WC geometry
  void SetSuperKGeometry();
  void DUSEL_100kton_10inch_40perCent();
  void DUSEL_100kton_10inch_HQE_12perCent();
  void DUSEL_100kton_10inch_HQE_30perCent();
  void DUSEL_100kton_10inch_HQE_30perCent_Gd();
  void DUSEL_150kton_10inch_HQE_30perCent();
  void DUSEL_200kton_10inch_HQE_12perCent();
  void DUSEL_200kton_12inch_HQE_12perCent();
  void DUSEL_200kton_12inch_HQE_10perCent();
  void DUSEL_200kton_12inch_HQE_13perCent();

  void UpdateGeometry();

//LCs options: K.Z.

  G4bool addWLS;
  void setWLS(G4bool);
  G4bool getWLS(){return addWLS;};

  void ConstructLC();
  G4bool addLC;
  G4bool DaddLC; // default option for LCs
  
  WCSimLC *logiLC;
  G4double  LCoffset;
  void setLC(G4bool);
  G4bool getLC(){return addLC;};
  


  void ConstructWLSP();
  WCSimWLSP_LC *logiWLSPLC;
  WCSimWLSP_Reflector *logiWLSPReflector;
  G4bool addWLSP;
  G4bool DaddWLSP; // default option for LCs

  G4double LC_rmin;
  G4double LC_rmax;
  G4double LC_a;
  G4double LC_b;
  G4double LC_d;

  G4double WLSP_offset;
  G4double WLSP_inradius;
  G4double WLSP_outradius_small;
  G4double WLSP_outradius_large;
  void setWLSP(G4bool);
  G4bool getWLSP(){return addWLSP;};
  G4int Plate_size;
  void setWLSP_size(G4int);

  //Related to MailBox Geometry   aah
  void SetMailBox100kTGeometry();
  void SetMailBox150kTGeometry_10inch_HQE_30perCent();
  void SetMailBox150kTGeometry_10inch_40perCent();
  void SetMailBox300kTGeometry();
  G4double WC_MB_Fid_Length,WC_MB_Fid_Width,WC_MB_Fid_Depth; //Fiducial Volume sizes--note, this "fiducial" should probably be called the "active volume". The PMT's are just outside.
  G4double WC_MB_Tank_Airgap, WC_MB_Dome_Height_fraction,WC_MB_Veto_Thickness,WC_MB_Buffer_Thickness;// Dimensions to determine WaterTank and Cavern Sizes (Veto may or may not be actual veto detector),WC_MB_Buffer_Thickness is distance between wall to PMT top
	//End of Mailbox Variables
	

  G4double GetWaterTubeLength()   {return WCLength;}
  G4double GetWaterTubePosition() {return WCPosition;}
  G4double GetPMTSize()           {return WCPMTRadius;}
  G4String GetPMTName()			  {return WCPMTName;}
  G4int    GetMyConfiguration()   {return myConfiguration;}
  G4double GetGeo_Dm(G4int);
  G4int    GetTotalNumPmts() {return totalNumPMTs;}
  
  G4int    GetPMT_QE_Method(){return PMT_QE_Method;}
  G4int    UsePMT_Coll_Eff(){return PMT_Coll_Eff;}
  G4int    GetPMT_Coll_Eff_Method(){return PMT_Coll_Eff_Method;}
  G4int GetPMT_Timing_Var(){return PMT_Timing_Var;}

  G4double GetPMTSize1() {return WCPMTSize;}

  G4float GetPMTQE(G4float, G4int, G4float, G4float, G4float);

  G4ThreeVector GetWCOffset(){return WCOffset;}
  
  // Related to the WC tube IDs
  static G4int GetTubeID(std::string tubeTag){return tubeLocationMap[tubeTag];}
  static G4Transform3D GetTubeTransform(int tubeNo){return tubeIDMap[tubeNo];}

  // Related to Pi0 analysis
  G4bool SavePi0Info()              {return pi0Info_isSaved;}
  void   SavePi0Info(G4bool choice) {pi0Info_isSaved=choice;}
  
  void   SetPMT_QE_Method(G4int choice){PMT_QE_Method = choice;}
  void   SetPMT_Coll_Eff(G4int choice){PMT_Coll_Eff = choice;}
  void   SetPMT_Coll_Eff_Method(G4int choice){PMT_Coll_Eff_Method = choice;}
  void   SetPMT_Timing_Var(G4int choice){PMT_Timing_Var = choice;}

  // Geometry options
  void   SetIsUpright(G4bool choice) {isUpright = choice;}
  void   SetIsMailbox(G4bool choice) {isMailbox = choice;}
  G4bool GetIsMailbox() {return isMailbox;}

  std::vector<WCSimPmtInfo*>* Get_Pmts() {return &fpmts;}

private:

  // Tuning parameters

  WCSimTuningParameters* WCSimTuningParams;

  // Sensitive Detectors. We declare the pointers here because we need
  // to check their state if we change the geometry.

  WCSimWCSD*  aWCPMT;

  //Water, Blacksheet surface
  G4OpticalSurface * OpWaterBSSurface;

  //Glass, Cathode surface in PMTs

  G4OpticalSurface * OpGlassCathodeSurface;

  //Tyvek surface - jl145
  G4OpticalSurface * OpWaterTySurface;

  // The messenger we use to change the geometry.

  WCSimDetectorMessenger* messenger;

  // The Construction routines
  G4LogicalVolume*   ConstructMailboxWC();
  G4LogicalVolume*   ConstructWC();
  void  ConstructPMT();

  void  ConstructMaterials();

  G4bool WCAddGd;

  G4int SK_flag;

  // Code for traversing the geometry and assigning tubeIDs.

  // First make a typedef for the pointer to the member fcn.  The
  // syntax is too wacked out to be using all over.

  typedef void (WCSimDetectorConstruction::*DescriptionFcnPtr)
    (G4VPhysicalVolume*, int, int, const G4Transform3D&);

  // Now Funcs for traversing the geometry
  void TraverseReplicas(G4VPhysicalVolume*, int, const G4Transform3D&,
			DescriptionFcnPtr);

  void DescribeAndDescendGeometry(G4VPhysicalVolume*, int, int, 
				  const G4Transform3D&, DescriptionFcnPtr);

  // Functions that the traversal routines call or we use to manipulate the
  // data we accumulate.
  void DumpGeometryTableToFile();

  void PrintGeometryTree(G4VPhysicalVolume*, int, int, const G4Transform3D&);
  void DescribeAndRegisterPMT(G4VPhysicalVolume*, int, int, 
			      const G4Transform3D&);
  void DescribeAndRegisterPMT_1KT(G4VPhysicalVolume*, int, int, 
				  const G4Transform3D&);
  void GetWCGeom(G4VPhysicalVolume*, int, int, 
			      const G4Transform3D&);

  //---Volume lengths

  // These are shared between the different member functions 
  // constructWC, constructFGD, constructlArD, constuctMRD
  // toggle between FGD(0) and lArD(1)
  // toggle between lArD readout types
  // toggle between MRDScint and MRDk2k

  G4bool pi0Info_isSaved;


  // XQ 08/17/10
  //   PMT_QE_Method == 1
  //   Only use it in the stacking function (no WLS)
  //   PMT_QE_Method == 2
  //   Use Part of it in the stacking function (MAX QE)
  //   Put the rest of it in the sensitive detector according to QE/Max_QE
  //   PMT_QE_Method == 3
  //   Put all of it in the sensitive detector according to QE
  //   Good for low energy photons
  G4int PMT_QE_Method;

  //XQ 03/31/11
  // 0 to not use collection efficiency
  // 1 to use
  G4int PMT_Coll_Eff;
  // 1 for 10 inch PMT
  // 2 for 12 inch Penn's measurement
  G4int PMT_Coll_Eff_Method;
  G4int PMT_Timing_Var;

  G4double WCLength;

  G4double WCPosition;

  // WC PMT parameters
  G4String WCPMTName;
  G4double sphereRadius;
  G4double PMTOffset;
  G4LogicalVolume* logicWCPMT;
  G4LogicalVolume* logicGlassFaceWCPMT;

  //WLS parameters
  G4LogicalVolume* logicWLSFaceWCPMT;

 // WC geometry parameters

  G4double WCPMTRadiusEff;
  G4double WCPMTRadius;
  G4double WCPMTExposeHeight;
  G4double WCPMTGlassThickness;
  G4double WCBarrelPMTOffset;

  G4double WCIDDiameter;

  G4double WCCapLength;
  G4double WCBackODLength;
  G4double WCFrontODLength;
  G4double WCIDHeight;

  G4double WCBarrelRingRadius;

  G4double WCBarrelRingNPhi;
  G4double WCBarrelNRings;
  G4double WCPMTperCellHorizontal;
  G4double WCPMTperCellVertical;

  G4double WCPMTPercentCoverage;

  G4double WCBarrelNumPMTHorizontal;
  G4double WCCapPMTSpacing;
  G4double WCCapEdgeWidth;//jh TODO: not used
  
  G4double WCCapEdgeLimit;
  G4double WCBlackSheetThickness;

 //for 1kt
  G4double WCDiameter;
  G4double WCRadius;
  G4double WCBarrelPMTRadius;
  G4double WCBarrelRingdPhi;
  G4double WCBarrelCellLength;
  G4double WCCapNCell;
  G4double WCBarrelLength;

  // amb79: to universally make changes in structure and geometry
  bool isUpright, isMailbox;
  

  // amb79: debug to display all parts
  bool debugMode;

  // Variables related to the geometry

  std::ofstream geoFile;   // File for text output

  G4int totalNumPMTs;      // The number of PMTs for this configuration     
  G4double WCCylInfo[3];    // Info for the geometry tree: radius & length or mail box, length, width and depth
  G4double WCPMTSize;       // Info for the geometry tree: pmt size
  G4ThreeVector WCOffset;   // Info for the geometry tree: WC center offset

  // Tube map information

  static std::map<int, G4Transform3D> tubeIDMap;
//  static std::map<int, cyl_location> tubeCylLocation;
  static hash_map<std::string, int, hash<std::string> >  tubeLocationMap; 

  // Variables related to configuration

  G4int myConfiguration;   // Detector Config Parameter
  G4double innerradius;
 
  std::vector<WCSimPmtInfo*> fpmts;
  
};

#endif

