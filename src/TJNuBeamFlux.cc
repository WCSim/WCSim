#include "TJNuBeamFlux.hh"
#include <iostream>

//_______________________________________________________________________________________
RooTrackerVtxBase::RooTrackerVtxBase() :
    TObject()
{
}
//_______________________________________________________________________________________
JNuBeamFlux::JNuBeamFlux() :
    RooTrackerVtxBase()
{
    NuFileName = new TObjString("Not-set");
    this->Reset();
}
//_______________________________________________________________________________________
JNuBeamFlux::~JNuBeamFlux()
{
    if(NuFileName != NULL) { delete NuFileName; NuFileName = NULL; }     
}
//_______________________________________________________________________________________
void JNuBeamFlux::Copy(const JNuBeamFlux * flux)
{ 
    NuFileName->SetString(flux->NuFileName->GetString()); 
    NuFluxEntry = flux->NuFluxEntry;
    NuParentPdg = flux->NuParentPdg; 
    NuParentDecMode = flux->NuParentDecMode;
    NuCospibm = flux->NuCospibm;
    NuNorm = flux->NuNorm;
    NuCospi0bm = flux->NuCospi0bm;
    NuRnu = flux->NuRnu;
    NuIdfd = flux->NuIdfd;
    NuGipart = flux->NuGipart;
    NuGamom0 = flux->NuGamom0;
    NuEnusk = flux->NuEnusk; 
    NuNormsk = flux->NuNormsk; 
    NuAnorm = flux->NuAnorm; 
    NuVersion = flux->NuVersion; 
    NuTuneid = flux->NuTuneid; 
    NuNtrig = flux->NuNtrig; 
    NuPint = flux->NuPint;  
    NuRand = flux->NuRand; 
    for(int i=0;i<2;i++){
        NuXnu [i] = flux->NuXnu[i];  
        NuBpos[i] = flux->NuBpos[i]; 
        NuBtilt[i] = flux->NuBtilt[i]; 
        NuBrms[i] = flux->NuBrms[i]; 
        NuEmit[i] = flux->NuEmit[i];
        NuAlpha[i] = flux->NuAlpha[i]; 
    } 
    for(int i=0;i<3;i++){
        NuGpos0[i] = flux->NuGpos0[i]; 
        NuGvec0[i] = flux->NuGvec0[i];
        NuHcur[i] = flux->NuHcur[i];
    } 
    for(int i=0;i<4;i++){
        NuParentDecP4[i] = flux->NuParentDecP4[i]; 
        NuParentDecX4[i] = flux->NuParentDecX4[i]; 
        NuParentProP4[i] = flux->NuParentProP4[i]; 
        NuParentProX4[i] = flux->NuParentProX4[i];
    }
    NuNg = flux->NuNg;
    for(int i=0;i<kNgmax;i++){
        NuGcosbm[i] = flux->NuGcosbm[i]; 
        NuGdistc[i] = flux->NuGdistc[i]; 
        NuGdistal[i] = flux->NuGdistal[i]; 
        NuGdistti[i] = flux->NuGdistti[i]; 
        NuGdistfe[i] = flux->NuGdistfe[i];   
        NuGpid[i] = flux->NuGpid[i];
        NuGmec[i] = flux->NuGmec[i]; 
        NuGmat[i] = flux->NuGmat[i]; 
        for(int j=0;j<3;j++) {
            NuGp[i][j] = flux->NuGp[i][j];
            NuGv[i][j] = flux->NuGv[i][j];
        }
    }
}
//_______________________________________________________________________________________
void JNuBeamFlux::Reset()
{
    NuFluxEntry = -1;
    NuFileName->SetString("Not-set"); 

    NuParentPdg = 0; 
    NuParentDecMode = -1;
    NuCospibm = NuNorm = NuCospi0bm = NuRnu = -999999.;
    NuIdfd = NuGipart = -999999;
    NuGamom0 = NuEnusk = NuNormsk = NuAnorm = NuVersion = -999999.;
    NuTuneid = NuNtrig = NuPint = NuRand = -999999;
    for(int i=0;i<2;i++){
        NuXnu [i] = NuBpos[i] = NuBtilt[i] = NuBrms[i] = NuEmit[i] = NuAlpha[i] = -999999.;
    }
    for(int i=0;i<3;i++) NuGpos0[i] = NuGvec0[i] = NuHcur[i] = -999999.; 
    for(int i=0;i<4;i++) NuParentDecP4[i] = NuParentDecX4[i] = NuParentProP4[i] = NuParentProX4[i] = -999999.; 
    NuNg = -1;
    for(int i=0;i<kNgmax;i++){
        NuGcosbm[i] = NuGdistc[i] = NuGdistal[i] = NuGdistti[i] = NuGdistfe[i] = -999999.;  
        NuGpid[i] = 0;
        NuGmec[i] = NuGmat[i] = -999999;
        for(int j=0;j<3;j++){ 
            NuGv[i][j] = -999999.;
            NuGp[i][j] = -999999.;
        }
    }
}
//_______________________________________________________________________________________

ClassImp(RooTrackerVtxBase)
ClassImp(JNuBeamFlux)


