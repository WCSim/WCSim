{
  {
    const char *wcsimdirenv = gSystem->Getenv("WCSIMDIR");
    if (!(wcsimdirenv && wcsimdirenv[0])) { // make sure it's non-empty
      wcsimdirenv = "."; // the "default" value
      std::cout << "Note: WCSIMDIR not set, assuming: " << wcsimdirenv << std::endl;
      gSystem->Setenv("WCSIMDIR", wcsimdirenv);
    }
  }
  
  gInterpreter->AddIncludePath("${WCSIMDIR}/include");
  gSystem->Load("${WCSIMDIR}/libWCSimRoot");
  gROOT->ProcessLine(".L nuPRISMconvert.cc+");
}
