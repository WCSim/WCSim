#ifndef WCSim_RootGeom
#define WCSim_RootGeom

//////////////////////////////////////////////////////////////////////////
//                                                                      
// WCSim_RootGeom                                                      
//                                                                      
// This class contains information needed to be passed to reconstruction
//     routines.  It's just simple right now-- only the bare-bones  
//     WC info
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"

class TDirectory;

/** \class WCSimRootPMT
 *  \brief PMT geometry information
 *
 * See the macro sample-root-scripts/read_number_of_PMTs.C for an example of using this class
 */

class WCSimRootPMT : public TObject {

private:
  Int_t fTubeNo; //!< PMT tube ID. Unique number across all PMT types (i.e. runs from 0 to NPMTs, where NPMTs = N20" + NOD + N3"inMPMT)
  Int_t fmPMTNo; //!< The mPMT number this 3" PMT is in (Equivalent to fTubeID for 20" & OD PMTs)
  Int_t fmPMT_PMTNo; //!< The 3" PMT position ID inside this mPMT (runs from 1-19 for 3" PMTs inside mPMTs. Is 0 for 20" & OD PMTs)
  Int_t fCylLoc;  //!< Distinguishes between PMT position p (top cap = 0, wall = 1, bottom cap = 2) & PMT type t (20" PMT in FD or 3" in mPMT in IWCD = 0, mPMT in FD = 2 / OD = 1). CylLoc = 3 * t + p
  Float_t fOrientation[3]; //!< PMT direction unit vector pointing out of the glass into the detector volume
  Float_t fPosition[3]; //!< PMT position vector. The position is defined as the ??? part of the PMT. Units: cm

public:
  WCSimRootPMT();
  WCSimRootPMT(Int_t tubeNo, Int_t cylLoc, Double_t orientation[3], Double_t position[3]);
  WCSimRootPMT(Int_t tubeNo, Int_t mPMTNo, Int_t mPMT_PMTno, Int_t cylLoc, Double_t orientation[3], Double_t position[3]);
  WCSimRootPMT(const WCSimRootPMT & in);
  virtual ~WCSimRootPMT();
  bool CompareAllVariables(const WCSimRootPMT * c) const;

  void  SetTubeNo(Int_t i) {fTubeNo=i;}
  void  SetmPMTNo(Int_t i) {fmPMTNo=i;}
  void  SetmPMT_PMTNo(Int_t i) {fmPMT_PMTNo=i;}
  void  SetCylLoc(Int_t i) {fCylLoc=i;}
  void  SetOrientation(Int_t i, Double_t f) {fOrientation[i]= ( (i<3) ? f : 0);}
  void  SetPosition(Int_t i, Double_t f) {fPosition[i]= ( (i<3) ? f : 0);}

  Int_t GetTubeNo() const {return fTubeNo;}
  Int_t GetmPMTNo() const {return fmPMTNo;}
  Int_t GetmPMT_PMTNo() const {return fmPMT_PMTNo;}
  Int_t GetCylLoc() const {return fCylLoc;}
  Float_t GetOrientation(Int_t i=0) const {return (i<3) ? fOrientation[i] : 0;}
  Float_t GetPosition(Int_t i=0) const {return (i<3) ? fPosition[i] : 0;}

  ClassDef(WCSimRootPMT,2)  //WCSimPMT structure
};


//////////////////////////////////////////////////////////////////////////

/** \class WCSimRootGeom
 *  \brief Detector geometry information (also containing PMT information arrays)
 *
 * PMT information is stored based on the PMT type
 * - ID PMTs of the first type are 20" (HK FD) or 3" PMTs inside mPMTs (IWCD)
 * - ID PMTs of the second type are 3" PMTs inside mPMTs (HK FD)
 * - OD PMTs are OD PMTs
 *
 * See the macro sample-root-scripts/read_number_of_PMTs.C for an example of using this class
 */

class WCSimRootGeom : public TObject {

private:

  Float_t                fWCCylRadius;  //!< Radius of WC tank. Units: cm
  Float_t                fWCCylLength;  //!< Length of WC tank. Units: cm

  Int_t                  fgeo_type;  //!< UNUSED mailbox or cylinder?

  Float_t                fWCPMTRadius; //!< Radius of ID PMT of the first type. Units: cm
  Float_t                fWCPMTRadius2; // Radius of ID PMT of the second type. Units: cm
  Int_t                  fWCNumPMT;   // Number of ID PMTs of the first type
  Int_t                  fWCNumPMT2;   // Number of ID PMTs of the second type
  Float_t                fODWCPMTRadius; // Radius of OD PMT. Units: cm
  Int_t                  fODWCNumPMT; // Number of OD PMTs
  Float_t                fWCOffset[3]; // Offset of barrel center in global coords

  Int_t                  fOrientation; //!< UNUSED Orientation of detector, 0 is 2km horizontal, 1 is Upright

  TClonesArray           *fPMTArray; //!< Array of ID PMTs of the first type
  TClonesArray           *fPMTArray2; //!< Array of ID PMTs of the second type
  TClonesArray           *fODPMTArray; //!< Array of OD PMTs

public:

  WCSimRootGeom();
  WCSimRootGeom(const WCSimRootGeom & in);
  virtual ~WCSimRootGeom();
  bool CompareAllVariables(const WCSimRootGeom * c) const;

  // Sets and gets

  void  SetWCCylRadius(Double_t f) {fWCCylRadius=f;}
  void  SetWCCylLength(Double_t f) {fWCCylLength=f;}

  void SetGeo_Type(Int_t f){fgeo_type = f;}

  void  SetWCNumPMT(Int_t i,bool hybridsecondtype=false){
    if(hybridsecondtype)
      fWCNumPMT2 = i;
    else
      fWCNumPMT= i;
  }
  void  SetODWCNumPMT(Int_t i) {fODWCNumPMT= i;}
  void  SetWCPMTRadius(Double_t f,int hybridsecondtype=false) {(hybridsecondtype?fWCPMTRadius2=f:fWCPMTRadius=f);}
  void  SetODWCPMTRadius(Double_t f) {fODWCPMTRadius = f;}
  void  SetWCOffset(Double_t x, Double_t y, Double_t z) 
           {fWCOffset[0]=x; fWCOffset[1]=y; fWCOffset[2] = z;}

  void  SetODPMT(Int_t i, Int_t tubeno, Int_t cyl_loc, Double_t rot[3], Double_t pos[3], bool expand=true);
  void  SetPMT(Int_t i, Int_t tubeno, Int_t mPMTNo, Int_t mPMT_PMTno, Int_t cyl_loc, Double_t rot[3], Double_t pos[3], bool expand=true, bool hybridsecondtype=false);
  void  SetOrientation(Int_t o) {fOrientation = o;}

  Float_t GetWCCylRadius() const {return fWCCylRadius;}
  Float_t GetWCCylLength() const {return fWCCylLength;}

  Int_t GetGeo_Type() const {return fgeo_type;}
  
  Int_t  GetWCNumPMT(bool hybridsecondtype=false) const {
    if(hybridsecondtype)
      return fWCNumPMT2;
    else
      return fWCNumPMT;
  }
  Int_t GetODWCNumPMT() const {return fODWCNumPMT;}
  
  Float_t GetWCPMTRadius(bool hybridsecondtype=false) const {
    if(hybridsecondtype)
      return fWCPMTRadius2;
    else
      return fWCPMTRadius;
  }
  Float_t GetODWCPMTRadius() const {return fODWCPMTRadius;}
  Float_t GetWCOffset(Int_t i) const {return (i<3) ? fWCOffset[i] : 0.;}
  Int_t GetOrientation() const  { return fOrientation; }

  WCSimRootPMT GetPMT(Int_t i,bool hybridsecondtype=false){
    if(hybridsecondtype)
      return *(WCSimRootPMT*)(*fPMTArray2)[i];
    else
      return *(WCSimRootPMT*)(*fPMTArray)[i];
  }
  const WCSimRootPMT * GetPMTPtr(Int_t i, bool hybridsecondtype=false) const {
    if(hybridsecondtype)
      return (WCSimRootPMT*)(fPMTArray2->At(i));
    else
      return (WCSimRootPMT*)(fPMTArray->At(i));
  }
  TClonesArray * GetPMTs(bool hybridsecondtype=false) {
    if(hybridsecondtype)
      return fPMTArray2;
    return fPMTArray;
  }
  //OD
  WCSimRootPMT GetODPMT(Int_t i) {return *(WCSimRootPMT*)(*fODPMTArray)[i];}
  const WCSimRootPMT * GetODPMTPtr(Int_t i) const {return (WCSimRootPMT*)(fODPMTArray->At(i));}
  TClonesArray * GetODPMTs() {return fODPMTArray;}

  ClassDef(WCSimRootGeom,2)  //WCSimRootEvent structure
};


#endif
