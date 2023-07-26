// Based on Root test Event.cxx
////////////////////////////////////////////////////////////////////////

//#include "G4ios.hh"
#include "TObject.h"
#include "TDirectory.h"
#include "TProcessID.h"
#include "TClonesArray.h"

#include "WCSimRootGeom.hh"
#include "WCSimRootTools.hh"

#include <iostream>

#ifndef REFLEX_DICTIONARY
ClassImp(WCSimRootGeom)
ClassImp(WCSimRootPMT)
#endif

//______________________________________________________________________________
WCSimRootGeom::WCSimRootGeom()
{
  // Create a WCSimRootGeom object.
  fWCNumPMT = 0;
  fPMTArray = 0;
  fPMTArray = new TClonesArray("WCSimRootPMT", 500);
  fWCNumPMT2 = 0;
  fPMTArray2 = 0;
  fPMTArray2 = new TClonesArray("WCSimRootPMT", 500);
  fODWCNumPMT = 0;
  fODPMTArray = 0;
  fODPMTArray = new TClonesArray("WCSimRootPMT", 500);
}
//______________________________________________________________________________
WCSimRootGeom::WCSimRootGeom(const WCSimRootGeom & in)
  : TObject()
{
  fWCCylRadius   = in.GetWCCylRadius();
  fWCCylLength   = in.GetWCCylLength();
  fgeo_type      = in.GetGeo_Type();
  fWCPMTRadius   = in.GetWCPMTRadius();
  fWCNumPMT      = in.GetWCNumPMT(false);
  fWCNumPMT2     = in.GetWCNumPMT(true);
  fODWCPMTRadius = in.GetODWCPMTRadius();
  fODWCNumPMT    = in.GetODWCNumPMT();
  for(int i = 0; i < 3; i++)
    fWCOffset[i] = in.GetWCOffset(i);
  fOrientation   = in.GetOrientation();

  //fill the TClonesArray
  fPMTArray = new TClonesArray("WCSimRootPMT", fWCNumPMT);
  for(int i = 0; i < fWCNumPMT; i++)
    new((*fPMTArray)[i]) WCSimRootPMT(*(in.GetPMTPtr(i,false)));
    
  fPMTArray2 = new TClonesArray("WCSimRootPMT", fWCNumPMT2);
  for(int i = 0; i < fWCNumPMT2; i++)
    new((*fPMTArray2)[i]) WCSimRootPMT(*(in.GetPMTPtr(i,true)));

  fODPMTArray = new TClonesArray("WCSimRootPMT", fODWCNumPMT);
  for(int i = 0; i < fODWCNumPMT; i++)
    new((*fODPMTArray)[i]) WCSimRootPMT(*(in.GetODPMTPtr(i)));  
}
//______________________________________________________________________________
WCSimRootGeom::~WCSimRootGeom()
{
  fPMTArray->Delete();
  fPMTArray2->Delete();
  fODPMTArray->Delete();
  delete fPMTArray;
  delete fPMTArray2;
  delete fODPMTArray;
}
//______________________________________________________________________________
bool WCSimRootGeom::CompareAllVariables(const WCSimRootGeom * c) const
{
  bool failed = false;
  failed = (!ComparisonPassed(fWCCylRadius, c->GetWCCylRadius(), typeid(*this).name(), __func__, "WCCylRadius")) || failed;
  failed = (!ComparisonPassed(fWCCylLength, c->GetWCCylLength(), typeid(*this).name(), __func__, "WCCylLength")) || failed;
  failed = (!ComparisonPassed(fgeo_type, c->GetGeo_Type(), typeid(*this).name(), __func__, "Geo_Type")) || failed;
  failed = (!ComparisonPassed(fWCPMTRadius, c->GetWCPMTRadius(), typeid(*this).name(), __func__, "WCPMTRadius")) || failed;
  failed = (!ComparisonPassed(fWCNumPMT, c->GetWCNumPMT(false), typeid(*this).name(), __func__, "WCNumPMT")) || failed;
  failed = (!ComparisonPassed(fWCNumPMT2, c->GetWCNumPMT(true), typeid(*this).name(), __func__, "WCNumPMT2")) || failed;
  failed = (!ComparisonPassed(fODWCPMTRadius, c->GetODWCPMTRadius(), typeid(*this).name(), __func__, "ODWCPMTRadius")) || failed;
  failed = (!ComparisonPassed(fODWCNumPMT, c->GetODWCNumPMT(), typeid(*this).name(), __func__, "ODWCNumPMT")) || failed;
  for(int i = 0; i < 3; i++) {
    failed = (!ComparisonPassed(fWCOffset[i], c->GetWCOffset(i), typeid(*this).name(), __func__, TString::Format("WCOffset[%d]", i))) || failed;
  }//i
  failed = (!ComparisonPassed(fOrientation, c->GetOrientation(), typeid(*this).name(), __func__, "Orientation")) || failed;
  for(int i = 0; i < TMath::Min(fWCNumPMT, c->GetWCNumPMT(false)); i++) {
    failed = !(this->GetPMTPtr(i,false)->CompareAllVariables(c->GetPMTPtr(i,false))) || failed;
  }
  for(int i = 0; i < TMath::Min(fWCNumPMT2, c->GetWCNumPMT(true)); i++) {
    failed = !(this->GetPMTPtr(i,true)->CompareAllVariables(c->GetPMTPtr(i,true))) || failed;
  }

  return !failed;
}
//______________________________________________________________________________
WCSimRootPMT::WCSimRootPMT()
{
  // Create a WCSimRootPMT object.
}
//______________________________________________________________________________
WCSimRootPMT::WCSimRootPMT(const WCSimRootPMT & in)
  : TObject()
{
  fTubeNo = in.GetTubeNo();
  fCylLoc = in.GetCylLoc();
  fmPMTNo = in.GetmPMTNo();
  fmPMT_PMTNo = in.GetmPMT_PMTNo();
  for(int i = 0; i < 3; i++) {
    fOrientation[i] = in.GetOrientation(i);
    fPosition   [i] = in.GetPosition(i);
  }//i
}
//______________________________________________________________________________
WCSimRootPMT::WCSimRootPMT(Int_t tubeNo, Int_t cylLoc, Double_t orientation[3], Double_t position[3])
{
  fTubeNo = tubeNo;
  fmPMTNo = tubeNo;
  fmPMT_PMTNo = 1;
  fCylLoc = cylLoc;
  int j = 0;
  for(j = 0; j < 3; j++) {
    fOrientation[j] = orientation[j];
    fPosition[j] = position[j];
  }//j
}

//______________________________________________________________________________
WCSimRootPMT::WCSimRootPMT(Int_t tubeNo, Int_t mPMTNo, Int_t mPMT_PMTNo, Int_t cylLoc, Double_t orientation[3], Double_t position[3])
{
  fTubeNo = tubeNo;
  fmPMTNo = mPMTNo;
  fmPMT_PMTNo = mPMT_PMTNo;
  fCylLoc = cylLoc;
  int j = 0;
  for(j = 0; j < 3; j++) {
    fOrientation[j] = orientation[j];
    fPosition[j] = position[j];
  }//j
}

//______________________________________________________________________________
void WCSimRootGeom::SetODPMT(Int_t i, Int_t tubeno, Int_t cyl_loc, 
			   Double_t rot[3], Double_t pos[3], bool expand)
{
  // Set PMT values
  TClonesArray &pmtArray = *fODPMTArray;
  if(expand)
    pmtArray.ExpandCreate(i+2);
  new(pmtArray[i]) WCSimRootPMT(tubeno, cyl_loc, rot, pos);
}

//______________________________________________________________________________
void WCSimRootGeom::SetPMT(Int_t i, Int_t tubeno, Int_t mPMTNo, Int_t mPMT_PMTNo, Int_t cyl_loc, 
			   Double_t rot[3], Double_t pos[3], bool expand, bool hybridsecondtype)
{
  // Set PMT values
  TClonesArray &pmtArray = hybridsecondtype?*fPMTArray2:*fPMTArray;
  if(expand)
    pmtArray.ExpandCreate(i+2);
  new(pmtArray[i]) WCSimRootPMT(tubeno, mPMTNo, mPMT_PMTNo, cyl_loc, rot, pos);
}

//______________________________________________________________________________
WCSimRootPMT::~WCSimRootPMT()
{
}

//______________________________________________________________________________
bool WCSimRootPMT::CompareAllVariables(const WCSimRootPMT * c) const
{
  bool failed = false;
  failed = (!ComparisonPassed(fTubeNo, c->GetTubeNo(), typeid(*this).name(), __func__, "TubeNo")) || failed;
  failed = (!ComparisonPassed(fCylLoc, c->GetCylLoc(), typeid(*this).name(), __func__, "CylLoc")) || failed;
  failed = (!ComparisonPassed(fmPMTNo, c->GetmPMTNo(), typeid(*this).name(), __func__, "mPMTNo")) || failed;
  failed = (!ComparisonPassed(fmPMT_PMTNo, c->GetmPMT_PMTNo(), typeid(*this).name(), __func__, "mPMT_PMTNo")) || failed;
  for(int i = 0; i < 3; i++) {
    failed = (!ComparisonPassed(fOrientation[i], c->GetOrientation(i), typeid(*this).name(), __func__, TString::Format("Orientation[%d]", i))) || failed;
    failed = (!ComparisonPassed(fPosition[i], c->GetPosition(i), typeid(*this).name(), __func__, TString::Format("Position[%d]", i))) || failed;
  }//i
  return !failed;
}
