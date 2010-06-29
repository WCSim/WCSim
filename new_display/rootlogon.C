// rootlogon.C  
{
    printf("rootlogon.C: Loading WCSimRoot Library...\n");
    gSystem->Load("libWCSimRoot.so");
    printf("rootlogon.C: Loading WCMap Library...\n");
    gSystem->Load("libWCMap.so");
}
