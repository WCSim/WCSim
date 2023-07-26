#include "WCSimPmtInfo.hh"

#ifndef REFLEX_DICTIONARY
ClassImp(WCSimPmtInfo)
#endif // __CINT__

  WCSimPmtInfo:: WCSimPmtInfo(){
}

WCSimPmtInfo::WCSimPmtInfo(Int_t i, Double_t t1, Double_t t2, Double_t t3, Double_t o1, Double_t o2, Double_t o3, Int_t tubeid){
  cylocation = i;
  trans_x = t1;
  trans_y = t2;
  trans_z = t3;

  orien_x = o1;
  orien_y = o2;
  orien_z = o3;
  
  tube = tubeid;
  mPMT = tubeid;
  mPMT_pmt = 0;
}

WCSimPmtInfo::WCSimPmtInfo(Int_t i, Double_t t1, Double_t t2, Double_t t3, Double_t o1, Double_t o2, Double_t o3, Int_t tubeid, Int_t mPMTid, Int_t mPMT_pmtid){
  cylocation = i;
  trans_x = t1;
  trans_y = t2;
  trans_z = t3;

  orien_x = o1;
  orien_y = o2;
  orien_z = o3;
  
  tube = tubeid;
  mPMT = mPMTid;
  mPMT_pmt = mPMT_pmtid;
}

WCSimPmtInfo::~WCSimPmtInfo(){
}
