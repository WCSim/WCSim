#ifndef WCSimDetectorConstruction_H
#define WCSimDetectorConstruction_H 1

#include "WCSimPmtInfo.hh"
#include "WCSimPMTObject.hh"
#include "WCSimRootOptions.hh"
#include "WCSimWLSProperties.hh"

#include "G4Transform3D.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4OpticalSurface.hh"
#include "globals.hh"

#include "G4SubtractionSolid.hh"
//instead of using forward declaration, just include:
#include "G4Material.hh"

#include <fstream>
#include <map>
#include <vector>
#include <unordered_map>     //--> need to fix the "using" and namespace statements

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

//Move to G4Enumerations
enum mPMT_orientation{
  VERTICAL,
  HORIZONTAL,
  PERPENDICULAR};

//-----------------------------------------------------
//-----------------------------------------------------

void ComputeWCODPMT(G4int NPMT, G4double *NPMTHorizontal, G4double *NPMTVertical);

class WCSimDetectorConstruction : public G4VUserDetectorConstruction
{
public:

  WCSimDetectorConstruction(G4int DetConfig,WCSimTuningParameters* WCSimTuningPars);
  ~WCSimDetectorConstruction();

  void SaveOptionsToOutput(WCSimRootOptions * wcopt);

  G4VPhysicalVolume* Construct();

  // Related to the WC geometry
  void SetSuperKGeometry();
  void InitSinglePMT();
  void SetTestSinglemPMTGeometry();
  void Cylinder_60x74_3inchmPMT_14perCent();
  void Cylinder_60x74_3inchmPMT_40perCent();
  void Cylinder_60x74_3inch_14perCent();
  void Cylinder_60x74_3inch_40perCent();
  void SuperK_20inchPMT_20perCent();
  void SuperK_20inchBandL_20perCent();
  void SuperK_12inchBandL_15perCent();
  void SuperK_20inchBandL_14perCent();
  void Cylinder_60x74_20inchBandL_14perCent();
  void Cylinder_60x74_20inchBandL_40perCent();
  void Cylinder_12inchHPD_15perCent();
  void SetHyperKGeometry();
  void SetHyperKGeometry_20perCent();
  void SetHyperKWithODGeometry();
  void SetHyperK20pcWithODGeometry();
  void SetHyperK_3inchGeometry();//B.Q, 2018/05/03
  void SetHyperK_8inchGeometry();//B.Q, 2018/05/03
  void SetHyperK_10inchGeometry();//B.Q, 2018/05/09
  void SetHyperK_mPMTGeometry();//B.Q, 2017/12/08
  void SetHyperK_HybridmPMTGeometry();//B.Q, 2018/12/10
  void SetHyperK_HybridmPMT10PCGeometry();//B.Q, 2019/01/26
  void SetHyperK_HybridFakeGeometry();//B.Q, 2019/01/26
  void SetHyperK_HybridmPMT_WithOD_Geometry();
  void SetNuPrismGeometry(G4String PMTType, G4double PMTCoverage, G4double detectorHeight, G4double detectorDiameter, G4double verticalPosition);
  void SetNuPrism_mPMTGeometry();
  void SetNuPrismBeamTest_mPMTGeometry();
  void SetNuPrismShort_mPMTGeometry();
  void SetDefaultNuPrismGeometry();
  /**
     Dump the values of many variables used to define geometries including
     - radii, heights, name, etc. of the detector
     - PMT arrangement (number of rings, photocoverage, etc.)
     - radii, name, etc. of the PMTs used in the geometry
     All output is prefixed by "GEOMCHECK", in order for fast searching/grepping
   */
  void DumpDetectorConfigInfo();

  void UpdateGeometry();
  void UpdateODGeo();
  void SetLCType(G4int LightCollectorType)
  {
	  LCType=LightCollectorType;
  };
  G4int GetLCType(){return LCType;};

  G4String GetDetectorName()      {return WCDetectorName;}
  G4double GetWaterTubeLength()   {return WCLength;}
  G4double GetWaterTubePosition() {return WCPosition;}
  G4double GetPMTSize()           {return WCPMTRadius;}
  G4double GetODPMTSize()         {return WCPMTODRadius;}
  G4String GetPMTName()			  {return WCPMTName;}
  G4int    GetMyConfiguration()   {return myConfiguration;}
  G4double GetGeo_Dm(G4int);
  G4int    GetTotalNumPmts() {return totalNumPMTs;}
  G4int    GetTotalNum_mPmts() {return totalNum_mPMTs;}         
  G4int    GetTotalNumPmts2() {return totalNumPMTs2;}//For the hybrid config
  G4int    GetTotalNum_mPmts2() {return totalNum_mPMTs2;}//For the hybrid config         
  G4int    GetTotalNumODPmts() {return totalNumODPMTs;}

  G4int    GetPMT_QE_Method(){return PMT_QE_Method;}
  G4double GetwaterTank_Length() {return waterTank_Length;} 
  G4int    UsePMT_Coll_Eff(){return PMT_Coll_Eff;}

  G4double GetPMTSize1() {return WCPMTSize;}

  G4double GetPMTQE(G4String,G4double, G4int, G4double, G4double, G4double);
  G4double GetPMTCollectionEfficiency(G4double theta_angle, G4String CollectionName) { return GetPMTPointer(CollectionName)->GetCollectionEfficiency(theta_angle); };
  G4double GetStackingPMTQE(G4double PhotonWavelength, G4int flag, G4double low_wl, G4double high_wl, G4double ratio);

  WCSimPMTObject *CreatePMTObject(G4String, G4String);

  void CreateCombinedPMTQE(std::vector<G4String>);
  WCSimBasicPMTObject *BasicPMT;
  void SetBasicPMTObject(WCSimBasicPMTObject *PMT){BasicPMT=PMT;}
  WCSimBasicPMTObject* GetBasicPMTObject(){ return BasicPMT;}

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

  WCSimWLSProperties *CreateWLSObject(G4String);
  WCSimWLSProperties *WLSptr;
  void SetWLSPointer(WCSimWLSProperties *WLS){WLSptr=WLS;}
  WCSimWLSProperties* GetWLSPointer(){
    return WLSptr;
  }

  G4ThreeVector GetWCOffset(){return WCOffset;}
  G4ThreeVector GetWCXRotation(){return WCXRotation;}
  G4ThreeVector GetWCYRotation(){return WCYRotation;}
  G4ThreeVector GetWCZRotation(){return WCZRotation;}
  G4ThreeVector GetWCDetCentre(){return WCDetCentre;}

  void SetWCDetCentre(double x, double y, double z){
      WCDetCentre[0] = x;
      WCDetCentre[1] = y;
      WCDetCentre[2] = z;
  }

  // Related to the WC tube IDs
  static G4int GetTubeID(std::string tubeTag){return tubeLocationMap[tubeTag];}
  static G4Transform3D GetTubeTransform(int tubeNo){return tubeIDMap[tubeNo];}
  //For the hybrid configuration
  static G4int GetTubeID2(std::string tubeTag){return tubeLocationMap2[tubeTag];}
  static G4Transform3D GetTubeTransform2(int tubeNo){return tubeIDMap2[tubeNo];}
  // OD PMTs
  static G4int GetODTubeID(std::string tubeTag){return ODtubeLocationMap[tubeTag];}
  static G4Transform3D GetODTubeTransform(int tubeNo){return ODtubeIDMap[tubeNo];}

  // Related to Pi0 analysis
  G4bool SavePi0Info()              {return pi0Info_isSaved;}
  void   SavePi0Info(G4bool choice) {pi0Info_isSaved=choice;}

  // Related to neutron capture analysis
  G4bool SaveCaptureInfo()              {return captureInfo_isSaved;}
  void   SaveCaptureInfo(G4bool choice) {captureInfo_isSaved=choice;}

  void   SetPMT_QE_Method(G4int choice){PMT_QE_Method = choice;}
  void   SetPMT_Coll_Eff(G4int choice){PMT_Coll_Eff = choice;}
  void   SetVis_Choice(G4String choice){Vis_Choice = choice;}
  G4String GetVis_Choice() {return Vis_Choice;}

  //Set PMT coverage of first PMT type
 void   SetPMTCoverage(G4double cover) {
    WCPMTPercentCoverage = cover;
  }
  G4double GetPMTCoverage() {return WCPMTPercentCoverage;}
 
  //Set PMT coverage of second PMT type
 void   SetPMTCoverage2(G4double cover) {
    WCPMTPercentCoverage2 = cover;
  }
  G4double GetPMTCoverage2() {return WCPMTPercentCoverage2;}
 
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

  // Getters and Setters for MultiPMT details from macro.
  void SetmPMT_VesselRadius(G4double radius){
    mPMT_vessel_radius = radius;

    // WCBarrelPMTOffset is affected, so need to be updated!!
    G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;
    // If  no acrylic cover/pressure vessel
    if(mPMT_vessel_tot_height < WCPMTRadius)
      mPMT_vessel_tot_height = WCPMTRadius;
    WCBarrelPMTOffset = mPMT_vessel_tot_height;                          // BarrelPMTOffset needs PMT/mPMT height
    if(!fix_nModules){
      WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(CLHEP::pi*WCPMTPercentCoverage)/(10.0*mPMT_vessel_radius));
      if(WCBarrelNumPMTHorizontal < 1)
	G4cerr << "Bug in dimensions as less than 1 BarrelPMT specified" << G4endl;
      WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(CLHEP::pi*WCIDDiameter)))
      					/WCPMTperCellVertical));
      WCCapPMTSpacing       = (CLHEP::pi*WCIDDiameter/WCBarrelNumPMTHorizontal); 
      WCCapEdgeLimit = WCIDDiameter/2.0 - mPMT_vessel_tot_height;          // CapEdgeLimit needs PMT/mPMT height
    }
  }                          

  void SetmPMT_VesselRadiusCurv(G4double radius){
    mPMT_vessel_radius_curv = radius;
  }                          

  void SetmPMT_VesselCylHeight(G4double height){
    mPMT_vessel_cyl_height = height;

    G4double mPMT_vessel_tot_height = mPMT_vessel_radius + mPMT_vessel_cyl_height;
    // If no acrylic cover/pressure mPMT_vessel
    if(mPMT_vessel_tot_height < WCPMTRadius)
      mPMT_vessel_tot_height = WCPMTRadius;
    WCBarrelPMTOffset = mPMT_vessel_tot_height;            
    if(!fix_nModules)
      WCCapEdgeLimit = WCIDDiameter/2.0 - mPMT_vessel_tot_height;

  }
  void SetmPMT_DistPMTVessel(G4double dist){dist_pmt_vessel = dist;}
  void SetmPMT_Orientation(mPMT_orientation orient){orientation = orient;}
  void SetmPMT_ReflectorHeight(G4double ref_height){id_reflector_height = ref_height;}
  void SetmPMT_ReflectorZoffset(G4double ref_zoffset){id_reflector_z_offset = ref_zoffset;}
  void SetmPMT_ReflectorAngle(G4double ref_angle){id_reflector_angle = ref_angle;}
  void SetmPMT_MaterialOuter(G4String outer_material){
    mPMT_outer_material = outer_material;
    if(outer_material == "Acrylic")
      mPMT_outer_material = "G4_PLEXIGLASS";
    }
  void SetmPMT_MaterialInner(G4String inner_material){mPMT_inner_material = inner_material;}
  void SetmPMT_PMTtype_inner(G4String type){
    mPMT_ID_PMT = type;
    
    //ToDo: replace by common function
    WCSimPMTObject * PMT = CreatePMTObject(mPMT_ID_PMT, WCIDCollectionName);
    WCPMTName = PMT->GetPMTName();
    WCPMTExposeHeight = PMT->GetExposeHeight(); 
    WCPMTRadius = PMT->GetRadius(); 
   }
  void SetmPMT_PMTtype_outer(G4String type){mPMT_OD_PMT = type;}
  void SetmPMT_MaterialOuterThickness(G4double thickness){
    mPMT_outer_material_d = thickness;
  }
  void SetmPMT_nID(G4int nPMTs){nID_PMTs = nPMTs;}
  void SetmPMT_nID2(G4int nPMTs){nID_PMTs2 = nPMTs;}//Added by B.Quilain for the second type of photosensor in the hybrid version
  void SetmPMT_Config(G4String inputfile){config_file = inputfile;}

  int GetmPMT_nID(void){return nID_PMTs;};
  int GetmPMT_nID2(void){return nID_PMTs2;};//Added by B.Quilain for the second type of photosensor in the hybrid version
  G4String GetPMTtype_ID(void){return mPMT_ID_PMT;};
  G4String GetPMTtype_OD(void){return mPMT_OD_PMT;};   //might want to replace the name mPMT by general var name

  void SetmPMT_MaterialPMTassembly(G4String material){mPMT_material_pmtAssembly = material;}
  void SetmPMT_FixModules(G4bool fix){fix_nModules = fix;}
  void SetmPMT_OpeningAngle(G4double angle){mPMT_pmt_openingAngle = angle;}

  //Filling mPMT
  G4int         FillCircles(void);
  // DEPRECATED:
  G4int	        CountPMT(G4int NoPmt);
  G4double	ComputeEta (G4int NoPmt);
  G4double	ComputeAlpha (G4double alphaOfPrevC, G4double Eta);
  G4int	        ComputeNiC (G4double alphaOfCircle, G4double Eta);




  // Set if nuPRISM
  void   SetIsNuPrism(G4bool choice) {isNuPrism = choice;}
  G4bool GetIsNuPrism() {return isNuPrism;}

  //Set if hybrid detector
  void   SetHybridPMT(G4bool choice) {hybrid = choice;}
  
  // Set if nuPRISM for WCTE (NuPRISMBeamTest)
  // M.Shinoki added Jun.04,2020
  void   SetIsNuPrismBeamTest(G4bool choice) {isNuPrismBeamTest = choice;}
  G4bool GetIsNuPrismBeamTest() {return isNuPrismBeamTest;}

  void   SetPMTType(G4String type) {
    WCPMTType = type;
    //And update everything that is affected by a new PMT
    WCSimPMTObject * PMT = CreatePMTObject(WCPMTType, WCIDCollectionName);
    WCPMTName = PMT->GetPMTName();
    WCPMTExposeHeight = PMT->GetExposeHeight();
    WCPMTRadius = PMT->GetRadius();
    
    WCBarrelPMTOffset     = WCPMTRadius;
    WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(CLHEP::pi*WCPMTPercentCoverage/100.0)/WCPMTRadius);
    WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(CLHEP::pi*WCIDDiameter)))/WCPMTperCellVertical));
    WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
    
  }
  bool GetHybridPMT() {return hybrid;}
  G4String GetPMTType() {return WCPMTType;}

  /*
  void   SetPMTCoverage(G4double cover) {
    // TF: do the same as for mPMTs to make "Update" deprecated:
    //     change the variables from WCDetectorConstruction that
    //     are affected.
    WCPMTPercentCoverage = cover;
    if(WCDetectorName == "NuPRISM_mPMT" || WCDetectorName == "HyperK_mPMT"){
      WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(CLHEP::pi*WCPMTPercentCoverage/100.0)/mPMT_vessel_radius);
    } else
      WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(CLHEP::pi*WCPMTPercentCoverage/100.0)/WCPMTRadius);
    WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(CLHEP::pi*WCIDDiameter)))/WCPMTperCellVertical));
    WCCapPMTSpacing       = (CLHEP::pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
  }
  G4double GetPMTCoverage() {return WCPMTPercentCoverage;}
  */
  
  void SetDopedWater(G4bool dopedWater){WCAddGd = dopedWater; }
  void AddDopedWater(G4double percentGd = 0.1);

  std::vector<WCSimPmtInfo*>* Get_Pmts() {return &fpmts;}
  std::vector<WCSimPmtInfo*>* Get_Pmts2() {return &fpmts2;}//For the hybrid config
  std::vector<WCSimPmtInfo*>* Get_ODPmts() {return &fODpmts;}

  void   SetDetectorHeight(G4double height) {
    WCIDHeight = height;
    // Affects Number of Barrel rings:
    WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(CLHEP::pi*WCIDDiameter)))/WCPMTperCellVertical));
  }
  G4double GetWCIDHeight(){ return WCIDHeight; }

  void   SetDetectorVerticalPosition(G4double position) {WCIDVerticalPosition = position;}
  G4double GetWCIDVerticalPosition(){ return WCIDVerticalPosition; }

  void   SetDetectorDiameter(G4double diameter) {
    WCIDDiameter = diameter;
    // Affects several cylinder parameters:
    if(WCDetectorName == "NuPRISM_mPMT"){
      WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(CLHEP::pi*WCPMTPercentCoverage/100.0)/mPMT_vessel_radius);
      WCCapEdgeLimit        = WCIDDiameter/2.0 - mPMT_vessel_radius;
    } else{
      WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(CLHEP::pi*WCPMTPercentCoverage/100.0)/WCPMTRadius);
      WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
    }
    
    WCBarrelNRings        = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(CLHEP::pi*WCIDDiameter)))/WCPMTperCellVertical));
    WCCapPMTSpacing       = (CLHEP::pi*WCIDDiameter/WCBarrelNumPMTHorizontal);
    

}
  G4double GetWCIDDiameter(){ return WCIDDiameter; }

  G4String GetIDCollectionName(){return WCIDCollectionName;}
  G4String GetIDCollectionName2(){return WCIDCollectionName2;}//Added by B.Quilain for hybrid PMT configuration
  G4String GetODCollectionName(){return WCODCollectionName;}
  WCSimTuningParameters* GetParameters(){return WCSimTuningParams;}//Added by TD to set up a parameter on PMT TTS uncertainty and use it in WCSimWCPMT
  
  G4double GetIDRadius()     {return WCIDRadius;}
  G4double GetIDHeight()     {return WCIDHeight;}

  bool GetIsODConstructed(){return isODConstructed;}
  bool GetIsCombinedPMTCollectionDefined(){return isCombinedPMTCollectionDefined;}

  ///////////////////////////////
  // MESSENGER methods for OD ///
  ///////////////////////////////

  void SetWCPMTODSize(G4String WCPMTODSize){
    if(WCPMTODSize == "PMT8inch" || WCPMTODSize == "PMT5inch" || WCPMTODSize == "PMT3inch_ETEL9302B"){
      WCSimPMTObject *PMTOD = CreatePMTObject(WCPMTODSize, WCODCollectionName);
      WCPMTODName           = PMTOD->GetPMTName();
      WCPMTODExposeHeight   = PMTOD->GetExposeHeight();
      WCPMTODRadius         = PMTOD->GetRadius();
    }
  }

  void SetWCODLateralWaterDepth(G4double val){WCODLateralWaterDepth = val;}
  void SetWCODHeightWaterDepth(G4double val){WCODHeightWaterDepth = val;}
  void SetWCODDeadSpace(G4double val){WCODDeadSpace = val;}
  void SetWCODTyvekSheetThickness(G4double val){WCODTyvekSheetThickness = val;}
  void SetWCODWLSPlatesThickness(G4double val){WCODWLSPlatesThickness = val;}
  void SetWCODWLSPlatesLength(G4double val){WCODWLSPlatesLength = val;}
  void SetWCPMTODperCellHorizontal(G4double val){WCPMTODperCellHorizontal = val;}
  void SetWCPMTODperCellVertical(G4double val){WCPMTODperCellVertical = val;}
  void SetWCPMTODPercentCoverage(G4double val){WCPMTODPercentCoverage = val;}
  void SetWCODPMTShift(G4double val){WCODPMTShift = val;}
  void SetODEdited(G4bool val){odEdited = val;}
  void SetIsWLSFilled(G4bool val){isWLSFilled = val;}
  void SetBuildODWLSCladding(G4bool val){BuildODWLSCladding = val;}
  G4bool GetODEdited(){return odEdited;}

  ////////// END OD /////////////
  ///////////////////////////////

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

  //Reflector skin surface -tf
  G4OpticalSurface * ReflectorSkinSurface;

  //Foam for mPMT: Gel - absorbing support structure -tf
  G4OpticalSurface * OpGelFoamSurface;

  //TF fix for blacksheet errors "missing refractive index"
  // or "photon travelling faster than c_light" when trying to refract
  // through blacksheet due to undefined border crossing (after setting n_BS)
  G4OpticalSurface * BSSkinSurface;

  //WLS surface - jl145
  G4OpticalSurface * OpWaterWLSSurface;
  G4OpticalSurface * OpWLSTySurface;

  //Cladding
  G4OpticalSurface * WlsOdOpCladdingSurface;

  // The messenger we use to change the geometry.
  WCSimDetectorMessenger* messenger;

  // The Construction routines
  G4LogicalVolume*   ConstructCylinder();

  G4LogicalVolume* ConstructPMT(G4String,G4String,G4String detectorElement="tank",G4int nIDPMTs=1);//Modified by B.Quilain 2018/12 to implement hybrid detector
  G4LogicalVolume* ConstructMultiPMT(G4String,G4String, G4String detectorElement="tank",G4int nIDPMTs=1); 

  G4LogicalVolume* ConstructPMT(G4String,G4String,G4String detectorElement="tank",bool WLS=false);
  G4LogicalVolume* ConstructPMTAndWLSPlate(G4String,G4String,G4String detectorElement="OD");

  G4LogicalVolume* ConstructCaps(G4int zflip);

  void  ConstructMaterials();

  G4LogicalVolume* logicWCBarrelCellODTyvek;
  G4LogicalVolume* logicWCTowerODTyvek;

  G4LogicalVolume* logicWCODWLSAndPMT;
  G4LogicalVolume* logicWCODWLSPlate;
  G4LogicalVolume* logicWCODWLSPlateCladding;

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
    G4bool captureInfo_isSaved;


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
  G4String WCIDCollectionName2;
  G4String WCODCollectionName;
  G4bool hybrid = false;

  // WC PMT parameters
  G4String WCPMTName;
  G4String WCPMTName2;//B. Quilain: for Hybrid configuration
  typedef std::pair<G4String, G4String> PMTKey_t;
  typedef std::map<PMTKey_t, G4LogicalVolume*> PMTMap_t;

  static PMTMap_t PMTLogicalVolumes;

  // WC geometry parameters
  G4double WCPMTRadius;
  G4double WCPMTExposeHeight;
  G4double WCBarrelPMTOffset;
  G4double WCBorderPMTOffset;

  G4double WCPMTRadius2;//B. Quilain: for Hybrid configuration
  G4double WCPMTExposeHeight2;//B. Quilain: for Hybrid configuration
  G4double WCBarrelPMTOffset2;//B. Quilain: for Hybrid configuration

  G4double WCIDDiameter;

  G4double WCCapLength;
  G4double WCBackODLength;
  G4double WCFrontODLength;
  G4double WCIDHeight;
  G4double WCIDVerticalPosition;

  G4double WCBarrelRingRadius;

  G4double WCBarrelRingNPhi;
  G4double WCBarrelNRings;
  G4double WCPMTperCellHorizontal;
  G4double WCPMTperCellVertical;

  G4double WCPMTPercentCoverage = -1.;
  G4double WCPMTPercentCoverage2 = -1.;//Added by B.Q for hybrid

  G4double WCBarrelNumPMTHorizontal;
  G4double WCCapPMTSpacing;
  G4double WCCapEdgeWidth;//jh ToDo: not used
  
  G4double WCCapEdgeLimit;
  G4double WCBlackSheetThickness;

  // ################### //
  // # Cave parameters # //
  // ################### //

  G4double CaveTyvekSheetThickness;

  // ############################### //
  // # *** END Cave Parameters *** # //
  // ############################### //

  // ############################# //
  // # Outer Detector parameters # //
  // ############################# //

  bool isODConstructed;
  bool isCombinedPMTCollectionDefined;

  // Parameters controlled by user
  G4double WCODDiameter;
  G4double WCPMTODperCellHorizontal;
  G4double WCPMTODperCellVertical;
  G4double WCPMTODPercentCoverage;

  G4double WCODLateralWaterDepth;
  G4double WCODHeightWaterDepth;
  G4double WCODDeadSpace;
  G4double WCODTyvekSheetThickness;

  G4double WCODWLSPlatesThickness;
  G4double WCODWLSPlatesLength;

  G4double WCODCapPMTSpacing;
  G4double WCODCapEdgeLimit;

  G4double WCODPMTShift;

  // We just need these variables to be global, ease things
  G4double WCODRadius;
  G4double WCBarrelNumPMTODHorizontal;

  // OD PMTs parameters
  G4String WCPMTODName;
  G4double WCPMTODRadius;
  G4double WCPMTODExposeHeight;

  // WLS material name
  bool isWLSFilled;
  bool BuildODWLSCladding;

  // ############################# //
  // # *** END OD Parameters *** # //
  // ############################# //

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


  // Add bool to indicate whether we load nuPRISM geometry  
  G4bool isNuPrism;
  G4bool isNuPrismBeamTest;
  G4String WCPMTType;
 // G4double WCPMTCoverage; //TF: already using this variable "WCPMTPercentCoverage

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
    G4bool odEdited;


  G4double blackSheetThickness;

    G4int innerPMT_TopN;
    G4int innerPMT_BotN;

    G4bool checkOverlaps;
    G4bool checkOverlapsPMT;

    G4bool placeBarrelPMTs;
    G4bool placeCapPMTs;
    G4bool placeBorderPMTs;
    G4LogicalVolume* waterTankLV;

    G4int PMTCopyNo;
    G4int wallSlabCopyNo;

  G4int  LCType;     // 0: No LC, 1: Old Branch(Mirror), 2: 2018Oct(Mirror)

  // *** End egg-shaped HyperK Geometry ***

  // amb79: debug to display all parts
  bool debugMode;

  // Variables related to the geometry

  std::ofstream geoFile;   // File for text output

  G4int totalNumPMTs=0;      // The number of PMTs for this configuration     
  G4int totalNum_mPMTs=0;   // The number of mPMTs (+1 for single PMT, +1 for mPMT)
  G4int totalNumPMTs2=0;      // The number of PMTs for this configuration, hybrid config     
  G4int totalNum_mPMTs2=0;   // The number of mPMTs (+1 for single PMT, +1 for mPMT), hybrid config
  G4int totalNumODPMTs=0;      // The number of OD PMTs for this configuration

  G4double WCCylInfo[3];    // Info for the geometry tree: radius & length or mail box, length, width and depth
  G4double WCPMTSize;       // Info for the geometry tree: pmt size
  G4double WCPMTSize2;       // Info for the geometry tree: pmt size
  G4ThreeVector WCOffset;   // Info for the geometry tree: WC center offset
  G4ThreeVector WCXRotation;   // Info for the geometry tree: WC detector local X axis in the global coordinate system 
  G4ThreeVector WCYRotation;   // Info for the geometry tree: WC detector local Y axis in the global coordinate system 
  G4ThreeVector WCZRotation;   // Info for the geometry tree: WC detector local Z axis in the global coordinate system 
  G4ThreeVector WCDetCentre;

  // Tube map information

  //static std::map<int, G4Transform3D> tubeIDMap;
  //nding mPMT and mPMT_pmt ID
//  static std::map<int, cyl_location> tubeCylLocation;
  //static hash_map<std::string, int, hash<std::string> >  tubeLocationMap_old;                //Deprecated
  
  static std::map<int, G4Transform3D> tubeIDMap;
  //  static std::map<int, cyl_location> tubeCylLocation;
  static std::unordered_map<std::string, int, std::hash<std::string> >  tubeLocationMap;
  static std::map<int, std::pair< int, int > > mPMTIDMap; //maps tubeID to correspo
  
  // ID PMTs hybrid version
  static std::map<int, G4Transform3D> tubeIDMap2;
  static std::unordered_map<std::string, int, std::hash<std::string> >  tubeLocationMap2;
  static std::map<int, std::pair< int, int > > mPMTIDMap2; //maps tubeID to correspo
  //static std::unordered_map<std::string, int, std::hash<std::string> >  tubeLocationMap; 
 
  // OD PMTs
  static std::map<int, G4Transform3D> ODtubeIDMap;
  static std::unordered_map<std::string, int, std::hash<std::string> >  ODtubeLocationMap;
  static std::map<int, std::pair< int, int > > mPMTODMap;

  // Variables related to configuration

  G4int myConfiguration;   // Detector Config Parameter
  G4double innerradius;

  // Variables related to MultiPMTs
  G4double vessel_cyl_height;
  G4double vessel_radius_curv;                        // radius of the sphere to determine curvature of cap of pressure vessel
  G4double vessel_radius;                             // radius of the pressure vessel (spherical cap)

  G4double mPMT_vessel_cyl_height;
  G4double mPMT_vessel_radius_curv;                        // radius of the sphere to determine curvature of cap of pressure vessel
  G4double mPMT_vessel_radius;                             // radius of the pressure vessel (spherical cap)
  G4double dist_pmt_vessel;                           // distance between glass of pmt and inner radius of pressure vessel (region where water/gel lives)
  mPMT_orientation orientation;
  G4String mPMT_outer_material;
  G4String mPMT_inner_material;
  G4double mPMT_outer_material_d;
  G4double id_reflector_height;
  G4double id_reflector_z_offset;
  G4double id_reflector_angle;
  G4int nID_PMTs;  // number of PMTs per mPMT module (1 for non-mPMT PMT e.g. standard 20")
  G4int nID_PMTs2; // number of PMTs per mPMT module (1 for non-mPMT PMT e.g. standard 20")
  G4String wcsimdir_path;
  G4String config_file;
  G4String mPMT_ID_PMT; //or ToDo: ideally ENUM
  G4String mPMT_OD_PMT;
  G4bool fix_nModules;
  G4double mPMT_pmt_openingAngle;
  G4String mPMT_material_pmtAssembly;

  //Filling mPMT
  std::vector<G4int>		vNiC;	        // Nb of Chambers in each circle
  std::vector<G4double>	        vAlpha;	        // Tilt angle for each circle
  std::vector<G4int>		vCircle;	// Circle numbers
  std::vector<G4double>		vAzimOffset;	// Azimuthal offset of first PMT in each circle.

 
  std::vector<WCSimPmtInfo*> fpmts;
  std::vector<WCSimPmtInfo*> fpmts2;//For the hybrid config
  std::vector<WCSimPmtInfo*> fODpmts;
};

#endif


//  LocalWords:  val
