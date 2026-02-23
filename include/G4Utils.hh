//
// Created by Nadrino on 23/02/2026.
//

#ifndef WCSIM_G4UTILS_H
#define WCSIM_G4UTILS_H

#include "G4Version.hh"

#define G4VERSION_ENCODE(major,minor,patch) ((major)*100 + (minor)*10 + (patch))

#define G4VERSION_IS_GREATER_EQUAL(major,minor,patch) (G4VERSION_NUMBER >= G4VERSION_ENCODE(major,minor,patch))


#endif //WCSIM_G4UTILS_H