{
  TString wcsim_topdir = gSystem->Getenv("WCSIM_BUILD_DIR");

  TString mp = gROOT->GetMacroPath();
  TString ip;

  TString wcsim_inc = wcsim_topdir + "/include";

  const char* p = wcsim_inc.Data();
  if (p) {
    mp += ":";
    mp += p;
    ip += " -I";
    ip += p;
  }

  mp += ":/usr/local/include/";
  ip += "  -I/usr/local/include/";

  gROOT->SetMacroPath(mp.Data());
  gSystem->SetIncludePath(ip);

  // additions to .include must be done individually or CINT will
  // try to quote all the spaces as a single path

  TString dip = ".include ";
  dip += wcsim_inc.Data();
  gROOT->ProcessLine(dip.Data());

  dip = ".include /usr/local/include/";
  gROOT->ProcessLine(dip.Data());
}

