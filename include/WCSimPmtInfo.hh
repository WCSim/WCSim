#ifndef WCSim_PmtInfo
#define WCSim_PmtInfo

/////////////////////////////////////////////////////////////////
//
// Save the Pmt info 
//
/////////////////////////////////////////////////////////////////

#include "TObject.h"

class TDirectory;

class WCSimPmtInfo : public TObject {
private:
  Int_t cylocation;
  Double_t trans_x;
  Double_t trans_y;
  Double_t trans_z;
  Double_t orien_x; 
  Double_t orien_y;
  Double_t orien_z;
  Int_t tube;
  Int_t mPMT;
  Int_t mPMT_pmt;
  
public: 
  WCSimPmtInfo();
  
  WCSimPmtInfo(Int_t i, Double_t t1, Double_t t2, Double_t t3, Double_t o1, Double_t o2, Double_t o3, Int_t tubeid);

  WCSimPmtInfo(Int_t i, Double_t t1, Double_t t2, Double_t t3, Double_t o1, Double_t o2, Double_t o3, Int_t tubeid,  Int_t mPMTid, Int_t mPMT_pmtid);
  
  virtual ~WCSimPmtInfo();

  Int_t Get_cylocation()const {return cylocation;}
  Double_t Get_transx() const {return trans_x;}
  Double_t Get_transy() const {return trans_y;}
  Double_t Get_transz() const {return trans_z;}
  Double_t Get_orienx() const {return orien_x;}
  Double_t Get_orieny() const {return orien_y;}
  Double_t Get_orienz() const {return orien_z;}
  Int_t Get_tubeid() const {return tube;}
  Int_t Get_mPMTid() const {return mPMT;}
  Int_t Get_mPMT_pmtid() const {return mPMT_pmt;}

  ClassDef(WCSimPmtInfo,2);
};

#endif
