
#include "RVersion.h"
#include "TRandom.h"
#include "TDirectory.h"
#include "TProcessID.h"

#include "Event.h"


ClassImp(Event)

//______________________________________________________________________________
Event::Event(){
  fNseg=0;
}

//______________________________________________________________________________
Event::~Event()
{
}
