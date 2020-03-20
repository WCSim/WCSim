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

}
//______________________________________________________________________________
WCSimRootGeom::WCSimRootGeom(const WCSimRootGeom & in)
  : TObject()
{
  fWCCylRadius   = in.GetWCCylRadius();
  fWCCylLength   = in.GetWCCylLength();
  fgeo_type      = in.GetGeo_Type();
  fWCPMTRadius   = in.GetWCPMTRadius();
  fWCNumPMT      = in.GetWCNumPMT();
  fODWCPMTRadius = in.GetODWCPMTRadius();
  fODWCNumPMT    = in.GetODWCNumPMT();
  for(int i = 0; i < 3; i++)
    fWCOffset[i] = in.GetWCOffset(i);
  fOrientation   = in.GetOrientation();
  //fill the TClonesArray
  fPMTArray = new TClonesArray("WCSimRootPMT", fWCNumPMT);
  for(int i = 0; i < fWCNumPMT; i++)
    new((*fPMTArray)[i]) WCSimRootPMT(*(in.GetPMTPtr(i)));
}
//______________________________________________________________________________
WCSimRootGeom::~WCSimRootGeom()
{
  fPMTArray->Delete();
  delete fPMTArray;
}
//______________________________________________________________________________
bool WCSimRootGeom::CompareAllVariables(const WCSimRootGeom * c) const
{
  bool failed = false;
  failed = (!ComparisonPassed(fWCCylRadius, c->GetWCCylRadius(), typeid(*this).name(), __func__, "WCCylRadius")) || failed;
  failed = (!ComparisonPassed(fWCCylLength, c->GetWCCylLength(), typeid(*this).name(), __func__, "WCCylLength")) || failed;
  failed = (!ComparisonPassed(fgeo_type, c->GetGeo_Type(), typeid(*this).name(), __func__, "Geo_Type")) || failed;
  failed = (!ComparisonPassed(fWCPMTRadius, c->GetWCPMTRadius(), typeid(*this).name(), __func__, "WCPMTRadius")) || failed;
  failed = (!ComparisonPassed(fWCNumPMT, c->GetWCNumPMT(), typeid(*this).name(), __func__, "WCNumPMT")) || failed;
  for(int i = 0; i < 3; i++) {
    failed = (!ComparisonPassed(fWCOffset[i], c->GetWCOffset(i), typeid(*this).name(), __func__, TString::Format("WCOffset[%d]", i))) || failed;
  }//i
  failed = (!ComparisonPassed(fOrientation, c->GetOrientation(), typeid(*this).name(), __func__, "Orientation")) || failed;
  for(int i = 0; i < TMath::Min(fWCNumPMT, c->GetWCNumPMT()); i++) {
    failed = !(this->GetPMTPtr(i)->CompareAllVariables(c->GetPMTPtr(i))) || failed;
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
{
  fTubeNo = in.GetTubeNo();
  fCylLoc = in.GetCylLoc();
  for(int i = 0; i < 3; i++) {
    fOrientation[i] = in.GetOrientation(i);
    fPosition   [i] = in.GetPosition(i);
  }//i
}
//______________________________________________________________________________
WCSimRootPMT::WCSimRootPMT(Int_t tubeNo, Int_t cylLoc, Float_t orientation[3], Float_t position[3])
{
	fTubeNo = tubeNo;
	fCylLoc = cylLoc;
	int j = 0;
	for(j = 0; j < 3; j++) {
		fOrientation[j] = orientation[j];
		fPosition[j] = position[j];
	}
	// fOrientation = *(orientation);
	// fPositoin = *(position);
  // Create a WCSimRootPMT object.
}

//______________________________________________________________________________
void WCSimRootGeom::SetPMT(Int_t i, Int_t tubeno, Int_t cyl_loc, 
			    Float_t rot[3], Float_t pos[3], bool expand)
{
   if(expand) (*(fPMTArray)).ExpandCreate(i+2);

  // Set PMT values
TClonesArray &pmtArray = *fPMTArray;
    WCSimRootPMT *jPMT = new(pmtArray[i]) WCSimRootPMT(tubeno, cyl_loc, rot, pos);
    //WCSimRootPMT jPMT = *(WCSimRootPMT*)(*fPMTArray)[i];
    // jPMT.SetTubeNo(tubeno);
    // jPMT.SetCylLoc(cyl_loc);
    // int j;
    // for (j=0;j<3;j++){
    //   jPMT.SetOrientation(j,rot[j]);
    //   jPMT.SetPosition(j,pos[j]);
    // }

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
  for(int i = 0; i < 3; i++) {
    failed = (!ComparisonPassed(fOrientation[i], c->GetOrientation(i), typeid(*this).name(), __func__, TString::Format("Orientation[%d]", i))) || failed;
    failed = (!ComparisonPassed(fPosition[i], c->GetPosition(i), typeid(*this).name(), __func__, TString::Format("Position[%d]", i))) || failed;
  }//i
  return !failed;
}
