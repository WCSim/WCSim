#ifndef WCSim_RootOptions
#define WCSim_RootOptions

//////////////////////////////////////////////////////////////////////////
//                                                                      
// WCSim_RootOptions                                                      
//                                                                      
// This class contains run option information
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"

class TDirectory;

//////////////////////////////////////////////////////////////////////////

class WCSimRootOptions : public TObject {

private:


public:

  WCSimRootOptions();
  virtual ~WCSimRootOptions();

  // Sets and gets


  ClassDef(WCSimRootOptions,1)  //WCSimRootEvent structure
};


#endif
