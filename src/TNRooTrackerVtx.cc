#include <iomanip>
#include <iostream>

#include "TNRooTrackerVtx.hh"

using std::endl;
using std::cout;
using std::setw;
using std::setprecision;
using std::setfill;
using std::ios;

ClassImp(NRooTrackerVtx)

NRooTrackerVtx::NRooTrackerVtx() : JNuBeamFlux() { this->Init(); }

NRooTrackerVtx::~NRooTrackerVtx() {
  if (GeomPath != NULL) {
    delete GeomPath;
    GeomPath = NULL;
  }
  if (GeneratorName != NULL) {
    delete GeneratorName;
    GeneratorName = NULL;
  }
  if (EvtCode != NULL) {
    delete EvtCode;
    EvtCode = NULL;
  }
  if (OrigFileName != NULL) {
    delete OrigFileName;
    OrigFileName = NULL;
  }
  if (OrigTreeName != NULL) {
    delete OrigTreeName;
    OrigTreeName = NULL;
  }

  if (StdHepPdg) delete [] StdHepPdg;
  if (StdHepStatus) delete [] StdHepStatus;
  if (StdHepFd) delete [] StdHepFd;
  if (StdHepLd) delete [] StdHepLd;
  if (StdHepFm) delete [] StdHepFm;
  if (StdHepLm) delete [] StdHepLm;

  if (NEipvc) delete [] NEipvc;
  if (NEiorgvc) delete [] NEiorgvc;
  if (NEiflgvc) delete [] NEiflgvc;
  if (NEicrnvc) delete [] NEicrnvc;

  if (NEiflgvert) delete [] NEiflgvert;
  if (NEabspvert) delete [] NEabspvert;
  if (NEabstpvert) delete [] NEabstpvert;
  if (NEipvert) delete [] NEipvert;
  if (NEiverti) delete [] NEiverti;
  if (NEivertf) delete [] NEivertf;

  if (NFiflag) delete [] NFiflag;
  if (NFx) delete [] NFx;
  if (NFy) delete [] NFy;
  if (NFz) delete [] NFz;
  if (NFpx) delete [] NFpx;
  if (NFpy) delete [] NFpy;
  if (NFpz) delete [] NFpz;
  if (NFe) delete [] NFe;
  if (NFfirststep) delete [] NFfirststep;
  if (NFecms2) delete [] NFecms2;
  if (Prob) delete [] Prob;
  if (VertFlagStep) delete [] VertFlagStep;
  if (VertFsiRhon) delete [] VertFsiRhon;
  if (StepPel) delete [] StepPel;
  if (StepPsp) delete [] StepPsp;
  if (StepPdp) delete [] StepPdp;
  if (StepScaleFactor) delete [] StepScaleFactor;
  if (StepEkin) delete [] StepEkin;
}
void NRooTrackerVtx::Copy(const NRooTrackerVtx* event) {
  GeomPath->SetString(event->GeomPath->GetString());
  GeneratorName->SetString(event->GeneratorName->GetString());
  EvtCode->SetString(event->EvtCode->GetString());
  OrigFileName->SetString(event->OrigFileName->GetString());
  OrigTreeName->SetString(event->OrigTreeName->GetString());

  EvtNum = event->EvtNum;
  EvtXSec = event->EvtXSec;
  EvtDXSec = event->EvtDXSec;
  EvtWght = event->EvtWght;
  EvtProb = event->EvtProb;
  OrigTreePOT = event->OrigTreePOT;
  OrigEvtNum = event->OrigEvtNum;
  TimeInSpill = event->TimeInSpill;
  OrigTreeEntries = event->OrigTreeEntries;
  TruthVertexID = event->TruthVertexID;

  for (int i = 0; i < 4; i++) {
    EvtVtx[i] = event->EvtVtx[i];
  }

  NuNorm = event->NuNorm;
  NuEnusk = event->NuEnusk;
  NuNormsk = event->NuNormsk;
  NuAnorm = event->NuAnorm;

  StdHepN = event->StdHepN;

  // Allocate the arrays which will be saved
  if (StdHepPdg) delete [] StdHepPdg;
  StdHepPdg = new int[StdHepN];
  if (StdHepStatus) delete [] StdHepStatus;
  StdHepStatus = new int[StdHepN];
  if (StdHepFd) delete [] StdHepFd;
  StdHepFd = new int[StdHepN];
  if (StdHepLd) delete [] StdHepLd;
  StdHepLd = new int[StdHepN];
  if (StdHepFm) delete [] StdHepFm;
  StdHepFm = new int[StdHepN];
  if (StdHepLm) delete [] StdHepLm;
  StdHepLm = new int[StdHepN];

  for (int i = 0; i < StdHepN; i++) {
    StdHepPdg[i] = event->StdHepPdgTemp[i];
    StdHepStatus[i] = event->StdHepStatusTemp[i];

    StdHepFd[i] = event->StdHepFdTemp[i];
    StdHepLd[i] = event->StdHepLdTemp[i];
    StdHepFm[i] = event->StdHepFmTemp[i];
    StdHepLm[i] = event->StdHepLmTemp[i];

    for (int j = 0; j < 4; j++) {
      StdHepX4[i][j] = event->StdHepX4[i][j];
      StdHepP4[i][j] = event->StdHepP4[i][j];
    }
    for (int j = 0; j < 3; j++) {
      StdHepPolz[i][j] = event->StdHepPolz[i][j];
    }
  }

  NEnvc = event->NEnvc;

  // Allocate the arrays which will be saved
  if (NEipvc) delete [] NEipvc;
  NEipvc = new int[NEnvc];
  if (NEiorgvc) delete [] NEiorgvc;
  NEiorgvc = new int[NEnvc];
  if (NEiflgvc) delete [] NEiflgvc;
  NEiflgvc = new int[NEnvc];
  if (NEicrnvc) delete [] NEicrnvc;
  NEicrnvc = new int[NEnvc];

  for (int i = 0; i < NEnvc; i++) {
    NEipvc[i] = event->NEipvcTemp[i];
    NEiorgvc[i] = event->NEiorgvcTemp[i];
    NEiflgvc[i] = event->NEiflgvcTemp[i];
    NEicrnvc[i] = event->NEicrnvcTemp[i];
    for (int j = 0; j < 3; j++) {
      NEpvc[i][j] = event->NEpvc[i][j];
    }
  }

  NEcrsx = event->NEcrsx;
  NEcrsy = event->NEcrsy;
  NEcrsz = event->NEcrsz;
  NEcrsphi = event->NEcrsphi;
  NEnvert = event->NEnvert;

  if (NEiflgvert) delete [] NEiflgvert;
  NEiflgvert = new int[NEnvert];
  for (int i = 0; i < NEnvert; i++) {
    NEiflgvert[i] = event->NEiflgvertTemp[i];
    for (int j = 0; j < 3; j++) {
      NEposvert[i][j] = event->NEposvert[i][j];
    }
  }

  NEnvcvert = event->NEnvcvert;

  // define the dynamic arrays
  if (NEabspvert) delete [] NEabspvert;
  NEabspvert = new float[NEnvcvert];
  if (NEabstpvert) delete [] NEabstpvert;
  NEabstpvert = new float[NEnvcvert];
  if (NEipvert) delete [] NEipvert;
  NEipvert = new int[NEnvcvert];
  if (NEiverti) delete [] NEiverti;
  NEiverti = new int[NEnvcvert];
  if (NEivertf) delete [] NEivertf;
  NEivertf = new int[NEnvcvert];

  for (int i = 0; i < NEnvcvert; i++) {
    NEabspvert[i] = event->NEabspvertTemp[i];
    NEabstpvert[i] = event->NEabstpvertTemp[i];
    NEipvert[i] = event->NEipvertTemp[i];
    NEiverti[i] = event->NEivertiTemp[i];
    NEivertf[i] = event->NEivertfTemp[i];
    for (int j = 0; j < 3; j++) {
      NEdirvert[i][j] = event->NEdirvert[i][j];
    }
  }

  /**** START Nucleon FSI PASSTHROUGH ******/

  NFnvert = event->NFnvert;
  if (NFiflag) delete [] NFiflag;
  NFiflag = new int[NFnvert];
  if (NFx) delete [] NFx;
  NFx = new float[NFnvert];
  if (NFy) delete [] NFy;
  NFy = new float[NFnvert];
  if (NFz) delete [] NFz;
  NFz = new float[NFnvert];
  if (NFpx) delete [] NFpx;
  NFpx = new float[NFnvert];
  if (NFpy) delete [] NFpy;
  NFpy = new float[NFnvert];
  if (NFpz) delete [] NFpz;
  NFpz = new float[NFnvert];
  if (NFe) delete [] NFe;
  NFe = new float[NFnvert];
  if (NFfirststep) delete [] NFfirststep;
  NFfirststep = new int[NFnvert];

  for (int i = 0; i < NFnvert; i++) {
    NFiflag[i] = event->NFiflagTEMP[i];
    NFx[i] = event->NFxTEMP[i];
    NFy[i] = event->NFyTEMP[i];
    NFz[i] = event->NFzTEMP[i];
    NFpx[i] = event->NFpxTEMP[i];
    NFpy[i] = event->NFpyTEMP[i];
    NFpz[i] = event->NFpzTEMP[i];
    NFe[i] = event->NFeTEMP[i];
    NFfirststep[i] = event->NFfirststepTEMP[i];
  }
  PCascProb = event->PCascProb;
  NFnstep = event->NFnstep;

  if(NFnstep!=0){
    if (NFecms2) delete [] NFecms2;
    NFecms2 = new float[NFnstep];
    if (Prob) delete [] Prob;
    Prob = new float[NFnstep];
    if (VertFlagStep) delete [] VertFlagStep;
    VertFlagStep = new float[NFnstep];
    if (VertFsiRhon) delete [] VertFsiRhon;
    VertFsiRhon = new float[NFnstep];
    if (StepPel) delete [] StepPel;
    StepPel = new float[NFnstep];
    if (StepPsp) delete [] StepPsp;
    StepPsp = new float[NFnstep];
    if (StepPdp) delete [] StepPdp;
    StepPdp = new float[NFnstep];
    if (StepScaleFactor) delete [] StepScaleFactor;
    StepScaleFactor = new float[NFnstep];
    if (StepEkin) delete [] StepEkin;
    StepEkin = new float[NFnstep];
    
    for (int k = 0; k < NFnstep; k++) {
      NFecms2[k]         = event->NFecms2TEMP[k];
      Prob[k]            = event->ProbTEMP[k];
      VertFlagStep[k]    = event->VertFlagStepTEMP[k];
      VertFsiRhon[k]     = event->VertFsiRhonTEMP[k];
      StepPel[k]         = event->StepPelTEMP[k];
      StepPsp[k]         = event->StepPspTEMP[k];
      StepPdp[k]         = event->StepPdpTEMP[k];
      StepScaleFactor[k] = event->StepScaleFactorTEMP[k];
      StepEkin[k]        = event->StepEkinTEMP[k];
    }
  }

  SPIDelta = event->SPIDelta;
  IRadCorrPht = event->IRadCorrPht;

  /**** END Nucleon FSI PASSTHROUGH ******/

  // Remember to copy the flux info
  JNuBeamFlux* flux = (JNuBeamFlux*)event;
  JNuBeamFlux::Copy(flux);
}
void NRooTrackerVtx::Reset() {
  if (GeomPath) GeomPath->SetString("Not-set");
  if (GeneratorName) GeneratorName->SetString("Not-set");
  if (EvtCode) EvtCode->SetString("Not-set");
  if (OrigFileName) OrigFileName->SetString("Not-set");
  if (OrigTreeName) OrigTreeName->SetString("Not-set");

  EvtNum = -1;
  EvtXSec = -1.0;
  EvtDXSec = -1.0;
  EvtWght = -1.0;
  EvtProb = -1.0;
  OrigTreePOT = -1.0;
  OrigEvtNum = -1;
  TimeInSpill = -1.0;
  OrigTreeEntries = -1;
  TruthVertexID = -1;

  for (int i = 0; i < 4; i++) {
    EvtVtx[i] = 0.0;
  }

  StdHepN = 0;
  for (int i = 0; i < kNStdHepNPmax; i++) {
    // If the arrays are dynamic, only reset the Temp version of the data member
    StdHepPdgTemp[i] = -1;
    StdHepStatusTemp[i] = -1;
    StdHepFdTemp[i] = -1;
    StdHepLdTemp[i] = -1;
    StdHepFmTemp[i] = -1;
    StdHepLmTemp[i] = -1;
    for (int j = 0; j < 4; j++) {
      StdHepX4[i][j] = 0;
    }
    for (int j = 0; j < 4; j++) {
      StdHepP4[i][j] = 0;
    }
    for (int j = 0; j < 3; j++) {
      StdHepPolz[i][j] = 0;
    }
  }

  NEnvc = 0;
  for (int ip = 0; ip < kNEmaxvc; ip++) {
    NEipvcTemp[ip] = -1;
    NEiorgvcTemp[ip] = -1;
    NEiflgvcTemp[ip] = -1;
    NEicrnvcTemp[ip] = -1;
    for (int ip2 = 0; ip2 < 3; ip2++) {
      NEpvc[ip][ip2] = -99999.9;
    }
  }

  NEcrsx = -99999.9;
  NEcrsy = -99999.9;
  NEcrsz = -99999.9;
  NEcrsphi = -99999.9;

  NEnvert = 0;
  for (int iv = 0; iv < kNEmaxvert; iv++) {
    NEiflgvertTemp[iv] = -999;
    for (int ip2 = 0; ip2 < 3; ip2++) {
      NEposvert[iv][ip2] = -999;
    }
  }

  NEnvcvert = 0;
  for (int ip = 0; ip < kNEmaxvertp; ip++) {
    NEabspvertTemp[ip] = -99999.9;
    NEabstpvertTemp[ip] = -99999.9;
    NEipvertTemp[ip] = -1;
    NEivertiTemp[ip] = -1;
    NEivertfTemp[ip] = -1;
    for (int ip2 = 0; ip2 < 3; ip2++) {
      NEdirvert[ip][ip2] = -99999.9;
    }
  }

  /**** START Nucleon FSI PASSTHROUGH ******/

  NFnvert = 0;
  for (int i = 0; i < kNFMaxNucleonVert; i++) {
    NFiflagTEMP[i] = -1;
    NFxTEMP[i] = -99999.9;
    NFyTEMP[i] = -99999.9;
    NFzTEMP[i] = -99999.9;
    NFpxTEMP[i] = -99999.9;
    NFpyTEMP[i] = -99999.9;
    NFpzTEMP[i] = -99999.9;
    NFeTEMP[i] = -99999.9;
    NFfirststepTEMP[i] = -1;
  }
  PCascProb = 0;

  NFnstep = 0;
  for (int k = 0; k < kNFMaxNucleonSteps; k++) {
    NFecms2TEMP[k] = -99999.9;
    ProbTEMP[k] = -99999.9;
    VertFlagStepTEMP[k] = -99999.9;
    VertFsiRhonTEMP[k] = -99999.9;
    StepPelTEMP[k] = -99999.9;
    StepPspTEMP[k] = -99999.9;
    StepPdpTEMP[k] = -99999.9;
    StepScaleFactorTEMP[k] = -99999.9;
    StepEkinTEMP[k] = -99999.9;
  }

  /**** END Nucleon FSI PASSTHROUGH ******/

  SPIDelta = -999;
  IRadCorrPht = -999;
}
void NRooTrackerVtx::Init() {
  StdHepPdg = 0;
  StdHepStatus = 0;
  StdHepFd = 0;
  StdHepLd = 0;
  StdHepFm = 0;
  StdHepLm = 0;

  NEipvc = 0;
  NEiorgvc = 0;
  NEiflgvc = 0;
  NEicrnvc = 0;

  NEiflgvert = 0;

  NEabspvert = 0;
  NEabstpvert = 0;
  NEipvert = 0;
  NEiverti = 0;
  NEivertf = 0;

  NFiflag = 0;
  NFx = 0;
  NFy = 0;
  NFz = 0;
  NFpx = 0;
  NFpy = 0;
  NFpz = 0;
  NFe = 0;
  NFfirststep = 0;

  NFecms2 = 0;
  Prob = 0;
  VertFlagStep = 0;
  VertFsiRhon = 0;
  StepPel = 0;
  StepPsp = 0;
  StepPdp = 0;
  StepScaleFactor = 0;
  StepEkin = 0;

  GeomPath = new TObjString("");
  GeneratorName = new TObjString("");
  EvtCode = new TObjString("");
  OrigFileName = new TObjString("");
  OrigTreeName = new TObjString("");

  this->Reset();
}
void NRooTrackerVtx::Print(const Option_t*) const {
  cout << "\nNRooTrackerMCTruth: " << endl;
  cout << "  --> GeneratorName = " << GeneratorName->GetString() << endl;
  cout << "  --> EvtCode       = " << EvtCode->GetString() << endl;
  cout << "  --> GeomPath      = " << GeomPath->GetString() << endl;
  cout << "  --> EvtNum        = " << EvtNum << endl;
  cout << "  --> EvtXSec       = " << EvtXSec << endl;
  cout << "  --> EvtDXSec      = " << EvtDXSec << endl;
  cout << "  --> EvtWght       = " << EvtWght << endl;
  cout << "  --> EvtProb       = " << EvtProb << endl;
  cout << "  --> EvtVtx = ";
  for(int i = 0; i < 4; i++) {
    cout << EvtVtx[i] << ", ";
  }
  cout << endl;
  cout << "  --> TruthVertexID = " << TruthVertexID << endl;

  cout << "NRooTrackerStdHep: " << StdHepN << " values" << endl;

  for (int i = 0; i < StdHepN; i++) {
    cout << setfill(' ') << setw(4) << i << " -> pdgc: "
         //        << setfill(' ') << setw(20) << StdHepPdg[i]
         << " / ist: " << setfill(' ') << setw(3) << StdHepStatus[i]
         << " / mom: " << setfill(' ') << setw(3) << StdHepFm[i]
         << " / daughters: (" << setfill(' ') << setw(3) << StdHepFd[i] << ", "
         << setfill(' ') << setw(3) << StdHepLd[i] << ") / E = " << setfill(' ')
         << setw(10) << StdHepP4[i][kNStdHepIdxE] << ", px = " << setfill(' ')
         << setw(10) << StdHepP4[i][kNStdHepIdxPx] << ", py = " << setfill(' ')
         << setw(10) << StdHepP4[i][kNStdHepIdxPy] << ", pz = " << setfill(' ')
         << setw(10) << StdHepP4[i][kNStdHepIdxPz] << endl;
  }

  cout << endl << "Jnu-beam pass through info:" << endl;
  cout << "  -->  NuParentPdg      = " << NuParentPdg << endl;
  cout << "  -->  NuParentDecMode  = " << NuParentDecMode << endl;
  /////   cout << "  -->  NuParentProNVtx  = " << NuParentProNVtx << endl;
  cout << "  -->  NuParentDecP4[4] = " << NuParentDecP4[0] << ", "
       << NuParentDecP4[1] << ", " << NuParentDecP4[2] << ", "
       << NuParentDecP4[3] << endl;
  cout << "  -->  NuParentDecX4[4] = " << NuParentDecX4[0] << ", "
       << NuParentDecX4[1] << ", " << NuParentDecX4[2] << ", "
       << NuParentDecX4[3] << endl;
  cout << "  -->  NuParentProP4[4] = " << NuParentProP4[0] << ", "
       << NuParentProP4[1] << ", " << NuParentProP4[2] << ", "
       << NuParentProP4[3] << endl;
  cout << "  -->  NuParentProX4[4] = " << NuParentProX4[0] << ", "
       << NuParentProX4[1] << ", " << NuParentProX4[2] << ", "
       << NuParentProX4[3] << endl;
  /////   cout << "  -->  NuPlanePos[2] = " << NuPlanePos[0] << ", "<<
  /// NuPlanePos[1] << endl;
  cout << "  -->  NuGipart         = " << NuGipart << endl;
  cout << "  -->  NuGamom0         = " << NuGamom0 << endl;
  cout << "  -->  NuGpos0[3] = " << NuGpos0[0] << ", " << NuGpos0[1] << ", "
       << NuGpos0[2] << endl;
  cout << "  -->  NuGvec0[3] = " << NuGvec0[0] << ", " << NuGvec0[1] << ", "
       << NuGvec0[2] << endl;

  cout << "NuEnusk = " << NuEnusk << endl;
  cout << "NuNorm = " << NuNorm << endl;
  cout << "NuNormsk = " << NuNormsk << endl;
  cout << "NuAnorm = " << NuAnorm << endl;

  cout << "NuVersion = " << NuVersion << endl;
  cout << "NuTuneid = " << NuTuneid << endl;
  ;
  cout << "NuPint = " << NuPint << endl;

  cout << "NuBpos[2] = ";
  for (int ip = 0; ip < 2; ip++) cout << NuBpos[ip] << " ";
  cout << endl;

  cout << "NuBtilt[2] = ";
  for (int ip = 0; ip < 2; ip++) cout << NuBtilt[ip] << " ";
  cout << endl;

  cout << "NuBrms[2] = ";
  for (int ip = 0; ip < 2; ip++) cout << NuBrms[ip] << " ";
  cout << endl;

  cout << "NuEmit[2] = ";
  for (int ip = 0; ip < 2; ip++) cout << NuEmit[ip] << " ";
  cout << endl;

  cout << "NuAlpha[2] = ";
  for (int ip = 0; ip < 2; ip++) cout << NuAlpha[ip] << " ";
  cout << endl;

  cout << "NuHcur[3] = ";
  for (int ip = 0; ip < 3; ip++) cout << NuHcur[ip] << " ";
  cout << endl;

  cout << endl << "NEUT pass through info:" << endl << endl;

  cout << "NEnvc = " << NEnvc << endl;
  cout << "ip NEipvc[ip] NEiorgvc[ip] NEiflgvc[ip] NEicrnvc[ip] NEpvc[ip][3]"
       << endl;
  for (int ip = 0; ip < NEnvc; ip++) {
    cout << ip << " " << NEipvc[ip] << " " << NEiorgvc[ip] << " ";
    cout << NEiflgvc[ip] << " " << NEicrnvc[ip] << " ";
    for (int ip2 = 0; ip2 < 3; ip2++) {
      cout << NEpvc[ip][ip2] << " ";
    }
    cout << endl;
  }
  cout << endl;

  cout << "NEcrsx NEcrsy NEcrsz NEcrsphi" << endl;
  cout << NEcrsx << " " << NEcrsy << " " << NEcrsz << " " << NEcrsphi << endl;

  cout << "NEnvert = " << NEnvert << endl;
  cout << "iv NEiflgvert[iv] NEposvert[iv][3]" << endl;

  for (int iv = 0; iv < NEnvert; iv++) {
    cout << iv << " " << NEiflgvert[iv] << " ";
    for (int ip2 = 0; ip2 < 3; ip2++) {
      cout << NEposvert[iv][ip2] << " ";
    }
    cout << endl;
  }
  cout << endl;

  cout << "NEnvcvert = " << NEnvcvert << endl;
  cout << "ip NEabspvert[ip] NEabstpvert[ip] NEipvert[ip] NEiverti[ip] "
          "NEivertf[ip] NEdirvert[ip][3]"
       << endl;
  for (int ip = 0; ip < NEnvcvert; ip++) {
    cout << ip << " " << NEabspvert[ip] << " " << NEabstpvert[ip] << " "
         << NEipvert[ip] << " " << NEiverti[ip] << " " << NEivertf[ip] << " ";
    for (int ip2 = 0; ip2 < 3; ip2++) {
      cout << NEdirvert[ip][ip2] << " ";
    }
    cout << endl;
  }
  cout << endl;

  cout << endl << "Pass-through file info:" << endl;
  cout << "  --> OrigFileName       = " << OrigFileName->GetString() << endl;
  cout << "  --> OrigTreeName       = " << OrigTreeName->GetString() << endl;
  cout << "  --> OrigEvtNum         = " << OrigEvtNum << endl;
  cout << "  --> TimeInSpill         = " << TimeInSpill << endl;
  cout << "  --> OrigTreeEntries    = " << OrigTreeEntries << endl;
  cout << "  --> OrigTreePOT        = " << OrigTreePOT << endl;
}
