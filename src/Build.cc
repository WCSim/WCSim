#include "Build.h"
// define type for FD OD WLS plate
// HKFD WLS plate thickness is automatically set based on the chosen plate type. 
//const std::string HKFD_OD_WLS_PLATE_TYPE = "EljenEJ286";  // Definition
//const std::string HKFD_OD_WLS_PLATE_TYPE = "Kuraray";  // Definition
const std::string HKFD_OD_WLS_PLATE_TYPE = "Inr";  // Definition

// define type for IWCD WLS plate
// all IWCD WLS plates are currently set to 1 cm by default. These defaults can be overridden with /WCSim/HyperKOD/ODWLSPlatesThickness
const std::string IWCD_OD_WLS_PLATE_TYPE = "EljenEJ286";  // Definition
//const std::string IWCD_OD_WLS_PLATE_TYPE = "Kuraray";  // Definition
