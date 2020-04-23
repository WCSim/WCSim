{
  TString libs0 = gSystem->GetDynamicPath();
  TString libswc = gSystem->Getenv("WCSIMDIR");
  TString libsg4 = gSystem->Getenv("GEANT4_BASE_DIR/install/lib");
  TString libs  = libs0 + ":" + libswc + ":" + libsg4 + ":/usr/lib:/usr/local/lib:/opt/lib:/opt/local/lib";
  gSystem->SetDynamicPath(libs.Data());

  gSystem->Load("libGpad");
  gSystem->Load("libPhysics");
  gSystem->Load("libMatrix");
  gSystem->Load("libHist");
  gSystem->Load("libGraf");
  gSystem->Load("libTree");
  gSystem->Load("libRIO");
  gSystem->Load("libXMLIO");
  gSystem->Load("libMinuit");
  //gSystem->Load("libMinuit2");
  gSystem->Load("libMathMore"); 

  gSystem->Load("libG4global.so");
  gSystem->Load("libWCSimRoot.so");

}

