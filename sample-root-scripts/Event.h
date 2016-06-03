#ifndef ROOT_Event
#define ROOT_Event

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Event                                                                //
//                                                                      //
// Description of the event and track parameters                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class Event : public TObject {

private:
   Int_t          fNseg;              //Number of track segments

public:
   Event();
   virtual ~Event();
   void          SetNseg(Int_t n) { fNseg = n; }

   Int_t         GetNseg() const { return fNseg; }

   ClassDef(Event,1)  //Event structure
};


#endif
