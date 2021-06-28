void llib(){

        char *wcsimdirenv;
        wcsimdirenv = getenv("WCSIMDIR");

        if (wcsimdirenv != NULL) {
                gSystem->Load("$WCSIMDIR/libWCSimRoot.so");
                gSystem->Load("$WCSIMDIR/libWCSimRoot.rootmap");
                gSystem->Load("$WCSIMDIR/libWCSimRootDict_rdict.pcm");
        } else {
                std::cout << "Error: WCSIMDIR environment variable not set." << std::endl;
        }


}// End of loadlibs function

