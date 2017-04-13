{
gROOT->ProcessLine(".L $WCSIMDIR/libWCSimRoot.so");
gSystem->AddIncludePath(" -I$WCSIMDIR/include/");
gROOT->ProcessLine(".L nuPRISMconvert.cc+");

}

