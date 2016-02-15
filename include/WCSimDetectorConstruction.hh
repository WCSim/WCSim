#ifndef WCSimDetectorConstruction_H
#define WCSimDetectorConstruction_H 1

#include "WCSimPmtInfo.hh"
#include "WCSimPMTObject.hh"

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
  void SuperK_20inchPMT_20perCent();
  void SuperK_20inchBandL_20perCent();
  void SuperK_12inchBandL_15perCent();
  void SuperK_20inchBandL_14perCent();
  void Cylinder_60x74_20inchBandL_14perCent();
  void Cylinder_60x74_20inchBandL_40perCent();
  void Cylinder_12inchHPD_15perCent();
  void SetHyperKGeometry();
  void UpdateGeometry();
  

  G4String GetDetectorName()      {return WCDetectorName;}
  G4double GetWaterTubeLength()   {return WCLength;}
  G4double GetWaterTubePosition() {return WCPosition;}
  G4double GetPMTSize()           {return WCPMTRadius;}
  G4String GetPMTName()			  {return WCPMTName;}
  G4int    GetMyConfiguration()   {return myConfiguration;}
  G4double GetGeo_Dm(G4int);
  G4int    GetTotalNumPmts() {return totalNumPMTs;}
  
  G4int    GetPMT_QE_Method(){return PMT_QE_Method;}
  G4double GetwaterTank_Length() {return waterTank_Length;} 
  G4int    UsePMT_Coll_Eff(){return PMT_Coll_Eff;}

  G4double GetPMTSize1() {return WCPMTSize;}

  G4float GetPMTQE(G4String,G4float, G4int, G4float, G4float, G4float);
  G4float GetPMTCollectionEfficiency(G4float theta_angle, G4String CollectionName) { return GetPMTPointer(CollectionName)->GetCollectionEfficiency(theta_angle); };

  WCSimPMTObject *CreatePMTObject(G4String, G4String);

  std::map<G4String, WCSimPMTObject*>  CollectionNameMap; 
  WCSimPMTObject * PMTptr;
 
  void SetPMTPointer(WCSimPMTObject* PMT, G4String CollectionName){
    CollectionNameMap[CollectionName] = PMT;
  }

  WCSimPMTObject* GetPMTPointer(G4String CollectionName){
    PMTptr = CollectionNameMap[CollectionName];
    if (PMTptr == NULL) {G4cout << CollectionName << " is not a recognized hit collection. Exiting WCSim." << G4endl; exit(1);}
    return PMTptr;
  }
 
  G4ThreeVector GetWCOffset(){return WCOffset;}
  
  // Related to the WC tube ID
  static G4int GetTubeID(std::string tubeTag){return tubeLocationMap[tubeTag];}
  static G4Transform3D GetTubeTransform(int tubeNo){return tubeIDMap[tubeNo];}

  // Related to Pi0 analysis
  G4bool SavePi0Info()              {return pi0Info_isSaved;}
  void   SavePi0Info(G4bool choice) {pi0Info_isSaved=choice;}
  
  void   SetPMT_QE_Method(G4int choice){PMT_QE_Method = choice;}
  void   SetPMT_Coll_Eff(G4int choice){PMT_Coll_Eff = choice;}
  void   SetVis_Choice(G4String choice){Vis_Choice = choice;}
  G4String GetVis_Choice() {return Vis_Choice;}

  //Partition Length
  void SetwaterTank_Length(G4double length){waterTank_Length = length;}
  void SetWaterTubeLength(G4double length){WCLength = length;}
  // Geometry options
  void   SetIsUpright(G4bool choice) {isUpright = choice;}

  // *** Begin Egg-Shaped HyperK Geometry ***

  void   SetIsEggShapedHyperK(G4bool choice) {isEggShapedHyperK = choice;}
  G4bool GetIsEggShapedHyperK() {return isEggShapedHyperK;}

  void SetEggShapedHyperKGeometry();
  void SetEggShapedHyperKGeometry_withHPD();


  // *** End Egg-Shaped HyperK Geometry ***

  std::vector<WCSimPmtInfo*>* Get_Pmts() {return &fpmts;}

  G4String GetIDCollectionName(){return WCIDCollectionName;}

 
private:

  // Tuning parameters

  WCSimTuningParameters* WCSimTuningParams;

  // Sensitive Detectors. We declare the pointers here because we need
  // to check their state if we change the geometry, otherwise will segfault
  // between events!
  WCSimWCSD* aWCPMT;

  //Water, Blacksheet surface
  G4OpticalSurface * OpWaterBSSurface;

  //Glass, Cathode surface in PMTs

  G4OpticalSurface * OpGlassCathodeSurface;

  //Tyvek surface - jl145
  G4OpticalSurface * OpWaterTySurface;

  // The messenger we use to change the geometry.

  WCSimDetectorMessenger* messenger;

  // The Construction routines
  G4LogicalVolume*   ConstructCylinder();
  G4LogicalVolume* ConstructPMT(G4String,G4String);

  G4LogicalVolume* ConstructCaps(G4int zflip);

  void  ConstructMaterials();

  G4LogicalVolume* logicWCBarrelCellBlackSheet;
  G4LogicalVolume* logicWCTowerBlackSheet;
  G4double capAssemblyHeight;

  G4bool WCAddGd;

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

  //NP 06/17/15
  // "OGLSX" for classic visualization
  // "RayTracer" for RayTracer visualization
  G4String Vis_Choice;
  

  G4double WCLength;

  G4double WCPosition;
  
  // Hit collection name parameters
  G4String WCDetectorName;
  G4String WCIDCollectionName;
  G4String WCODCollectionName;


  // WC PMT parameters
  G4String WCPMTName;
  typedef std::pair<G4String, G4String> PMTKey_t;
  typedef std::map<PMTKey_t, G4LogicalVolume*> PMTMap_t;

  static PMTMap_t PMTLogicalVolumes;

  // WC geometry parameters

  G4double WCPMTRadius;
  G4double WCPMTExposeHeight;
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

// raise scope of derived parameters
  G4double WCIDRadius;
  G4double totalAngle;
  G4double dPhi;
  G4double barrelCellHeight;
  G4double mainAnnulusHeight;
  G4double innerAnnulusRadius;
  G4double outerAnnulusRadius;
  G4String water;


 //for 1kt
  G4double WCDiameter;
  G4double WCRadius;
  G4double WCBarrelPMTRadius;
  G4double WCBarrelRingdPhi;
  G4double WCBarrelCellLength;
  G4double WCCapNCell;
  G4double WCBarrelLength;

  // amb79: to universally make changes in structure and geometry
  bool isUpright;

  // *** Begin egg-shaped HyperK Geometry ***

    void MatchWCSimAndEggShapedHyperK();
    G4LogicalVolume* ConstructEggShapedHyperK();

    G4Material* FindMaterial(G4String);

    G4VSolid* ConstructHalf(G4double, G4double);

    G4LogicalVolume* ConstructRadialPMT(G4bool,
                                        G4double, G4double,
                                        G4double, G4double,
                                        G4double, G4double);

    G4LogicalVolume* ConstructEndWallPMT();

    G4LogicalVolume* ConstructCeilingPMT(G4bool,
                                         G4double, G4double,
                                         G4double, G4double);

    G4bool isEggShapedHyperK;

    G4double waterTank_TopR;
    G4double waterTank_BotR;
    G4double waterTank_Height;
    G4double waterTank_UpperA;
    G4double waterTank_LowerB;
    G4double waterTank_Length;

    G4double innerPMT_TopR;
    G4double innerPMT_BotR;
    G4double innerPMT_TopW;
    G4double innerPMT_BotW;
    G4double innerPMT_Height;
    G4double innerPMT_Radius;
    G4double innerPMT_Expose;
    G4double innerPMT_Rpitch;
    G4double innerPMT_Apitch;

    G4double outerPMT_TopR;
    G4double outerPMT_BotR;
    G4double outerPMT_TopW;
    G4double outerPMT_BotW;
    G4double outerPMT_Height;
    G4double outerPMT_Radius;
    G4double outerPMT_Expose;
    G4String outerPMT_Name;
    G4double outerPMT_TopRpitch;
    G4double outerPMT_BotRpitch;
    G4double outerPMT_Apitch;

    G4double blackSheetThickness;

    G4int innerPMT_TopN;
    G4int innerPMT_BotN;

    G4bool checkOverlaps;
    G4LogicalVolume* waterTankLV;

    G4int PMTCopyNo;
    G4int wallSlabCopyNo;

  // *** End egg-shaped HyperK Geometry ***

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

