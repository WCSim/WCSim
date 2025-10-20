{
  {
    const char *wcsimdirenv = gSystem->Getenv("WCSIM_BUILD_DIR");
    if (!(wcsimdirenv && wcsimdirenv[0])) { // make sure it's non-empty
      wcsimdirenv = "."; // the "default" value
      std::cout << "Note: WCSIM_BUILD_DIR not set, assuming: " << wcsimdirenv << std::endl;
      gSystem->Setenv("WCSIM_BUILD_DIR", wcsimdirenv);
    }
  }
  
  gInterpreter->AddIncludePath("${WCSIM_BUILD_DIR}/include");
  gSystem->Load("${WCSIM_BUILD_DIR}/lib/libWCSimRoot");
  gROOT->ProcessLine(".L nuPRISMconvert.cc+");
}
