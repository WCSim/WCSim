#include <iostream>
#include <TH1F.h>
#include <stdio.h>
#include <stdlib.h>
#include <TFile.h>
#include <TTree.h>
#include <TKey.h>
#include <WCSimRootEvent.hh>

/* converts position and direction branches in the fiTQun tree to nuPRISM coordinate system
 * and copies the rest of the branches into a new fiTQun tree
 */
void convertfiTQun(TTree* fiTQun, TTree* fiTQunCv) {

    // copies fiTQun tree to a new tree without any position and direction branches
    fiTQun->SetBranchStatus("*", 1);
    fiTQun->SetBranchStatus("fq*pos*",0);
    fiTQun->SetBranchStatus("fq*dir*",0);
    fiTQunCv = fiTQun->CloneTree();
    fiTQun->CopyAddresses(fiTQunCv, kTRUE);

    // sets branches back to be processed
    fiTQun->SetBranchStatus("fq*pos*",1);
    fiTQun->SetBranchStatus("fq*dir*",1);

    // initialize position and direction variables and their converted counterparts
    const Int_t MAX_TEMP = 100;
    Int_t fqntwnd;
    Float_t fqtwnd_prftpos[MAX_TEMP][3];    // pre-fitter vertex position
    Float_t fqtwnd_prftposCv[MAX_TEMP][3];
    Int_t fqnse;
    Float_t fq1rpos[MAX_TEMP][7][3];        // 1 ring fit vertex
    Float_t fq1rposCv[MAX_TEMP][7][3];
    Float_t fq1rdir[MAX_TEMP][7][3];        // 1 ring fit direction
    Float_t fq1rdirCv[MAX_TEMP][7][3];
    Float_t fqpi0pos[2][3];                 // Pi0 fit vertex position
    Float_t fqpi0posCv[2][3];
    Float_t fqpi0dir1[2][3];                // Pi0 fit direction of the first photon
    Float_t fqpi0dir1Cv[2][3];
    Float_t fqpi0dir2[2][3];                // Pi0 fit direction of the second photon
    Float_t fqpi0dir2Cv[2][3];
    Float_t fqpi0dirtot[2][3];              // Pi0 fit direction of the Pi0
    Float_t fqpi0dirtotCv[2][3];
    Int_t fqnmrfit;
    Float_t fqmrpos[MAX_TEMP][6][3];        // Multi-Ring fit vertex position of each ring
    Float_t fqmrposCv[MAX_TEMP][6][3];
    Float_t fqmrdir[MAX_TEMP][6][3];        // Multi-Ring fit direction of each ring
    Float_t fqmrdirCv[MAX_TEMP][6][3];
    Int_t fqmsnfit;
    Float_t fqmspos[MAX_TEMP][20][3];       // Multi-Segment Muon fit vertex position of each segment
    Float_t fqmsposCv[MAX_TEMP][20][3];
    Float_t fqmsdir[MAX_TEMP][20][3];       // Multi-Segment Muon fit direction of each segment
    Float_t fqmsdirCv[MAX_TEMP][20][3];
    Int_t fqtestn1r;
    Float_t fqtest1rpos[MAX_TEMP][3];       // test 1 ring fit vertex
    Float_t fqtest1rposCv[MAX_TEMP][3];
    Float_t fqtest1rdir[MAX_TEMP][3];       // test 1 ring fit directio
    Float_t fqtest1rdirCv[MAX_TEMP][3];
    Int_t fqtestnpi0;
    Float_t fqtestpi0pos[MAX_TEMP][3];      // test Pi0 fit vertex position
    Float_t fqtestpi0posCv[MAX_TEMP][3];
    Float_t fqtestpi0dir1[MAX_TEMP][3];     // test Pi0 fit direction of the first photon
    Float_t fqtestpi0dir1Cv[MAX_TEMP][3];
    Float_t fqtestpi0dir2[MAX_TEMP][3];     // test Pi0 fit direction of the second photon
    Float_t fqtestpi0dir2Cv[MAX_TEMP][3];
    Float_t fqtestpi0dirtot[MAX_TEMP][3];   // test Pi0 fit direction of the Pi0
    Float_t fqtestpi0dirtotCv[MAX_TEMP][3];

    // initializes branches in fiTQunCv output
    TBranch *bfqtwnd_prftpos = fiTQunCv->Branch("fqtwnd_prftpos",fqtwnd_prftposCv,"fqtwnd_prftpos[fqntwnd][3]/F");
    TBranch *bfq1rpos = fiTQunCv->Branch("fq1rpos",fq1rposCv,"fq1rpos[fqnse][7][3]/F");
    TBranch *bfq1rdir = fiTQunCv->Branch("fq1rdir",fq1rdirCv,"fq1rdir[fqnse][7][3]/F");
    TBranch *bfqpi0pos = fiTQunCv->Branch("fqpi0pos",fqpi0posCv,"fqpi0pos[2][3]/F");
    TBranch *bfqpi0dir1 = fiTQunCv->Branch("fqpi0dir1",fqpi0dir1Cv,"fqpi0dir1[2][3]/F");
    TBranch *bfqpi0dir2 = fiTQunCv->Branch("fqpi0dir2",fqpi0dir2Cv,"fqpi0dir2[2][3]/F");
    TBranch *bfqpi0dirtot = fiTQunCv->Branch("fqpi0dirtot",fqpi0dirtotCv,"fqpi0dirtot[2][3]/F");
    TBranch *bfqmrpos = fiTQunCv->Branch("fqmrpos",fqmrposCv,"fqmrpos[fqnmrfit][6][3]/F");
    TBranch *bfqmrdir = fiTQunCv->Branch("fqmrdir",fqmrdirCv,"fqmrdir[fqnmrfit][6][3]/F");
    TBranch *bfqmspos = fiTQunCv->Branch("fqmspos",fqmsposCv,"fqmspos[fqmsnfit][20][3]/F");
    TBranch *bfqmsdir = fiTQunCv->Branch("fqmsdir",fqmsdirCv,"fqmsdir[fqmsnfit][20][3]/F");
    TBranch *bfqtest1rpos = fiTQunCv->Branch("fqtest1rpos",fqtest1rposCv,"fqtest1rpos[fqtestn1r][3]/F");
    TBranch *bfqtest1rdir = fiTQunCv->Branch("fqtest1rdir",fqtest1rdirCv,"fqtest1rdir[fqtestn1r][3]/F");
    TBranch *bfqtestpi0pos = fiTQunCv->Branch("fqtestpi0pos",fqtestpi0posCv,"fqtestpi0pos[fqtestnpi0][3]/F");
    TBranch *bfqtestpi0dir1 = fiTQunCv->Branch("fqtestpi0dir1",fqtestpi0dir1Cv,"fqtestpi0dir1[fqtestnpi0][3]/F");
    TBranch *bfqtestpi0dir2 = fiTQunCv->Branch("fqtestpi0dir2",fqtestpi0dir2Cv,"fqtestpi0dir2[fqtestnpi0][3]/F");
    TBranch *bfqtestpi0dirtot = fiTQunCv->Branch("fqtestpi0dirtot",fqtestpi0dirtotCv,"fqtestpi0dirtot[fqtestnpi0][3]/F");
    fiTQun->SetBranchAddress("fqntwnd", &fqntwnd);
    fiTQun->SetBranchAddress("fqtwnd_prftpos", &fqtwnd_prftpos);
    fiTQun->SetBranchAddress("fqnse", &fqnse);
    fiTQun->SetBranchAddress("fq1rpos", &fq1rpos);
    fiTQun->SetBranchAddress("fq1rdir", &fq1rdir);
    fiTQun->SetBranchAddress("fqpi0pos", &fqpi0pos);
    fiTQun->SetBranchAddress("fqpi0dir1", &fqpi0dir1);
    fiTQun->SetBranchAddress("fqpi0dir2", &fqpi0dir2);
    fiTQun->SetBranchAddress("fqpi0dirtot", &fqpi0dirtot);
    fiTQun->SetBranchAddress("fqnmrfit", &fqnmrfit);
    fiTQun->SetBranchAddress("fqmrpos", &fqmrpos);
    fiTQun->SetBranchAddress("fqmrdir", &fqmrdir);
    fiTQun->SetBranchAddress("fqmsnfit", &fqmsnfit);
    fiTQun->SetBranchAddress("fqmspos", &fqmspos);
    fiTQun->SetBranchAddress("fqmsdir", &fqmsdir);
    fiTQun->SetBranchAddress("fqtestn1r", &fqtestn1r);
    fiTQun->SetBranchAddress("fqtest1rpos", &fqtest1rpos);
    fiTQun->SetBranchAddress("fqtest1rdir", &fqtest1rdir);
    fiTQun->SetBranchAddress("fqtestnpi0", &fqtestnpi0);
    fiTQun->SetBranchAddress("fqtestpi0pos", &fqtestpi0pos);
    fiTQun->SetBranchAddress("fqtestpi0dir1", &fqtestpi0dir1);
    fiTQun->SetBranchAddress("fqtestpi0dir2", &fqtestpi0dir2);
    fiTQun->SetBranchAddress("fqtestpi0dirtot", &fqtestpi0dirtot);

    // gets the total number of entries
    int fqEntries = fiTQun->GetEntries();

    double offset = 2151.035; // particle gun y-offset of position in centimeters

    // for each entry in the fiTQun input
    // changes the fiTQun output's WCSim coordinates to the global nuPRISM coordinate geometry
    for (int entry=0;entry<fqEntries;entry++) {
        fiTQun->GetEntry(entry);
        // time-window information
        for (int i=0;i<fqntwnd;i++) {
            for (int axis=0;axis<3;axis++) {
                if (axis == 0) { // set the x-axis to the z-axis
                    fqtwnd_prftposCv[i][0] = fqtwnd_prftpos[i][axis];
                } else if (axis == 1) { // set the y-axis to the x-axis
                    fqtwnd_prftposCv[i][1] = fqtwnd_prftpos[i][axis];
                } else if (axis == 2) { // set the z-axis to the y-axis
                    fqtwnd_prftposCv[i][2] = fqtwnd_prftpos[i][axis];
                }
            }
        }
        // 1-ring fits
        for (int i=0;i<fqnse;i++) {
            for (int j=0;j<7;j++) {
                for (int axis=0;axis<3;axis++) {
                    if (axis == 0) {
                        fq1rposCv[i][j][0] = fq1rpos[i][j][axis];
                        fq1rdirCv[i][j][0] = fq1rdir[i][j][axis];
                    } else if (axis == 1) {
                        fq1rposCv[i][j][1] = fq1rpos[i][j][axis];
                        fq1rdirCv[i][j][1] = fq1rdir[i][j][axis];
                    } else if (axis == 2) {
                        fq1rposCv[i][j][2] = fq1rpos[i][j][axis];
                        fq1rdirCv[i][j][2] = fq1rdir[i][j][axis];
                    }
                }
            }
        }
        // Pi0 fits
        for (int i=0;i<2;i++) {
            for (int axis=0;axis<3;axis++) {
                if (axis == 0) {
                    fqpi0posCv[i][0] = fqpi0pos[i][axis];
                    fqpi0dir1Cv[i][0] = fqpi0dir1[i][axis];
                    fqpi0dir2Cv[i][0] = fqpi0dir2[i][axis];
                    fqpi0dirtotCv[i][0] = fqpi0dirtot[i][axis];
                } else if (axis == 1) {
                    fqpi0posCv[i][1] = fqpi0pos[i][axis];
                    fqpi0dir1Cv[i][1] = fqpi0dir1[i][axis];
                    fqpi0dir2Cv[i][1] = fqpi0dir2[i][axis];
                    fqpi0dirtotCv[i][1] = fqpi0dirtot[i][axis];
                } else if (axis == 2) {
                    fqpi0posCv[i][2] = fqpi0pos[i][axis];
                    fqpi0dir1Cv[i][2] = fqpi0dir1[i][axis];
                    fqpi0dir2Cv[i][2] = fqpi0dir2[i][axis];
                    fqpi0dirtotCv[i][2] = fqpi0dirtot[i][axis];
                }
            }
        }
        // multi-ring fits
        for (int i=0;i<fqnmrfit;i++) {
            for (int j=0;j<6;j++) {
                for (int axis=0;axis<3;axis++) {
                    if (axis == 0) {
                        fqmrposCv[i][j][0] = fqmrpos[i][j][axis];
                        fqmrdirCv[i][j][0] = fqmrdir[i][j][axis]; 
                    } else if (axis == 1) {
                        fqmrposCv[i][j][1] = fqmrpos[i][j][axis];
                        fqmrdirCv[i][j][1] = fqmrdir[i][j][axis];
                    } else if (axis == 2) {
                        fqmrposCv[i][j][2] = fqmrpos[i][j][axis];
                        fqmrdirCv[i][j][2] = fqmrdir[i][j][axis];
                    }
                }
            }
        }
        // multi-segment muon fits
        for (int i=0;i<fqmsnfit;i++) {
            for (int j=0;j<20;j++) {
                for (int axis=0;axis<3;axis++) {
                    if (axis == 0) {
                        fqmsposCv[i][j][0] = fqmspos[i][j][axis];
                        fqmsdirCv[i][j][0] = fqmsdir[i][j][axis];
                    } else if (axis == 1) {
                        fqmsposCv[i][j][1] = fqmspos[i][j][axis];
                        fqmsdirCv[i][j][1] = fqmsdir[i][j][axis];
                    } else if (axis == 2) {
                        fqmsposCv[i][j][2] = fqmspos[i][j][axis];
                        fqmsdirCv[i][j][2] = fqmsdir[i][j][axis];
                    }
                }
            }
        }
        // test 1-ring fits
        for (int i=0;i<fqtestn1r;i++) {
            for (int axis=0;axis<3;axis++) {
                if (axis == 0) {
                    fqtest1rposCv[i][0] = fqtest1rpos[i][axis];
                    fqtest1rdirCv[i][0] = fqtest1rdir[i][axis];
                } else if (axis == 1) {
                    fqtest1rposCv[i][1] = fqtest1rpos[i][axis];
                    fqtest1rdirCv[i][1] = fqtest1rdir[i][axis];
                } else if (axis == 2) {
                    fqtest1rposCv[i][2] = fqtest1rpos[i][axis];
                    fqtest1rdirCv[i][2] = fqtest1rdir[i][axis];
                }
            }
        }
        // test Pi0 fits
        for (int i=0;i<fqtestnpi0;i++) {
            for (int axis=0;axis<3;axis++) {
                if (axis == 0) {
                    fqtestpi0posCv[i][0] = fqtestpi0pos[i][axis];
                    fqtestpi0dir1Cv[i][0] = fqtestpi0dir1[i][axis];
                    fqtestpi0dir2Cv[i][0] = fqtestpi0dir2[i][axis];
                    fqtestpi0dirtotCv[i][0] = fqtestpi0dirtot[i][axis];
                } else if (axis == 1) {
                    fqtestpi0posCv[i][1] = fqtestpi0pos[i][axis];
                    fqtestpi0dir1Cv[i][1] = fqtestpi0dir1[i][axis];
                    fqtestpi0dir2Cv[i][1] = fqtestpi0dir2[i][axis];
                    fqtestpi0dirtotCv[i][1] = fqtestpi0dirtot[i][axis];
                } else if (axis == 2) {
                    fqtestpi0posCv[i][2] = fqtestpi0pos[i][axis];
                    fqtestpi0dir1Cv[i][2] = fqtestpi0dir1[i][axis];
                    fqtestpi0dir2Cv[i][2] = fqtestpi0dir2[i][axis];
                    fqtestpi0dirtotCv[i][2] = fqtestpi0dirtot[i][axis];
                }
            }
        }
        // fill all branches for each entry
        bfqtwnd_prftpos->Fill();
        bfq1rpos->Fill();
        bfq1rdir->Fill();
        bfqpi0pos->Fill();
        bfqpi0dir1->Fill();
        bfqpi0dir2->Fill();
        bfqpi0dirtot->Fill();
        bfqmrpos->Fill();
        bfqmrdir->Fill();
        bfqmspos->Fill();
        bfqmsdir->Fill();
        bfqtest1rpos->Fill();
        bfqtest1rdir->Fill();
        bfqtestpi0pos->Fill();
        bfqtestpi0dir1->Fill();
        bfqtestpi0dir2->Fill();
        bfqtestpi0dirtot->Fill();
    }

    fiTQunCv->Write("",TObject::kOverwrite);

}

/* extracts the true information from the wcsimT tree
 * the position and direction information is converted to nuPRISM geometry
 * and then all the information are saved in the new wcsimEx tree
 */
void extractTruth(TTree* wcsimT, TTree* wcsimEx) {    

    WCSimRootTrigger * wcsimrootTrigger;
    WCSimRootEvent * wcsimrootEvent;
    wcsimrootEvent = new WCSimRootEvent();
    wcsimT->SetBranchAddress("wcsimrootevent" ,&wcsimrootEvent);

    // initializes variables
    const Int_t maxtrack = 200;
    Int_t ntrack;
    Int_t parent[maxtrack];
    Int_t pid[maxtrack];
    Double_t dir[maxtrack][4];
    Double_t start[maxtrack][4];
    Double_t stop[maxtrack][4];
    Double_t mom[maxtrack];
    Double_t mass[maxtrack];
    Double_t energy[maxtrack];
    Double_t startvol[maxtrack];
    Double_t stopvol[maxtrack];
    Double_t time[maxtrack];

    // sets output Ttree to above variables
    wcsimEx->Branch("ntrack",&ntrack,"ntrack/I");
    wcsimEx->Branch("parent",parent,"parent[ntrack]/I");
    wcsimEx->Branch("pid",pid,"pid[ntrack]/I");
    wcsimEx->Branch("dir",dir,"dir[ntrack][4]/D");
    wcsimEx->Branch("start",start,"start[ntrack][4]/D");
    wcsimEx->Branch("stop",stop,"stop[ntrack][4]/D");
    wcsimEx->Branch("mom",mom,"mom[ntrack]/D");
    wcsimEx->Branch("mass",mass,"mass[ntrack]/D");
    wcsimEx->Branch("energy",energy,"energy[ntrack]/D");
    wcsimEx->Branch("startvol",startvol,"startvol[ntrack]/D");
    wcsimEx->Branch("stopvol",stopvol,"stopvol[ntrack]/D");
    wcsimEx->Branch("time",time,"time[ntrack]/D");

    // for each event in the wcsimT input
    int Entries = wcsimT->GetEntries();
    for (int ev=0; ev<Entries; ev++) {
        wcsimT->GetEvent(ev);
        wcsimrootTrigger = wcsimrootEvent->GetTrigger(0);
        ntrack = wcsimrootTrigger->GetNtrack();
        if(ntrack>0) {
            // Loop through elements in the TClonesArray of WCSimTracks
            // for each particle in the event set the respective variable's information
            for (int par=0; par<ntrack; par++) {
                // wcsimroottrack holds all info about particle
                TObject *element = (wcsimrootTrigger->GetTracks())->At(par);
                WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);
                for (int axis=0;axis<4;axis++) {
                    if (axis == 0) { // set wcsim x-axis to nuPRISM x-axis
                        start[par][0] = wcsimroottrack->GetStart(axis)/100;
                        stop[par][0] = wcsimroottrack->GetStop(axis)/100;
                        dir[par][0] = wcsimroottrack->GetDir(axis);
                    } else if (axis == 1) { // set wcsim y-axis to nuPRISM y-axis
                        start[par][1] = wcsimroottrack->GetStart(axis)/100;
                        stop[par][1] = wcsimroottrack->GetStop(axis)/100;
                        dir[par][1] = wcsimroottrack->GetDir(axis);
                    } else if (axis == 2) { // set wcsim z-axis to nuPRISM z-axis
                        start[par][2] = wcsimroottrack->GetStart(axis)/100;
                        stop[par][2] = wcsimroottrack->GetStop(axis)/100;
                        dir[par][2] = wcsimroottrack->GetDir(axis);
                    } else if (axis == 3) {
                        start[par][3] = wcsimroottrack->GetStart(axis)/100;
                        stop[par][3] = wcsimroottrack->GetStop(axis)/100;
                        dir[par][3] = wcsimroottrack->GetDir(axis);
                    }
                }
                parent[par]=wcsimroottrack->GetParenttype();
                pid[par]=wcsimroottrack->GetIpnu();
                mom[par]=wcsimroottrack->GetP();
                mass[par]=wcsimroottrack->GetM();
                energy[par]=wcsimroottrack->GetE();
                startvol[par]=wcsimroottrack->GetStartvol();
                stopvol[par]=wcsimroottrack->GetStopvol();
                time[par]=wcsimroottrack->GetTime();
            }
            // fill the wcsimEx with all the current event's information in the current loop
            wcsimEx->Fill();
        }
    }

    wcsimEx->Write("",TObject::kOverwrite);

}

int convertNuPrism(char* input, char* output, bool dofiTQun=0, bool copy=0){

    char *filename = input;
    char *outfilename = output;
    bool toCopy = copy;
    bool useFiTQun = dofiTQun;

    TFile *file = new TFile(filename,"read");
    if (!file->IsOpen()){
        std::cout << "Error, could not open input file: " << filename << std::endl;
        exit(-1);
    }

    // creates new trees for output file
    TFile* outFile = TFile::Open(outfilename, "RECREATE");
    TTree* fiTQunCv = new TTree("fiTQunCv", "fiTQun Converted Geometry");
    TTree* wcsimEx = new TTree("wcsimEx","Converted Particle Info");

    // gets the old trees from the fiTQun file
    TTree* wcsimT = (TTree*) file->Get("wcsimT");
    TTree* wcsimGeoT = (TTree*) file->Get("wcsimGeoT");
    TTree* fiTQun = (TTree*) file->Get("fiTQun"); // gets the old fiTQun information
    TTree* Settings = (TTree*) file->Get("Settings");

    // prepares trees to be copied
    TTree* newEventTree;
    TTree* newGeomTree;
    TTree* newSettings;
    if (toCopy) {
        newEventTree = wcsimT->CloneTree();
        newGeomTree = wcsimGeoT->CloneTree();
        if (Settings) {
            newSettings = Settings->CloneTree();
        }
    }

    if(useFiTQun) convertfiTQun(fiTQun, fiTQunCv);
    extractTruth(wcsimT, wcsimEx);

    // copies the other trees
    if (toCopy){
        newEventTree->Write();
        newGeomTree->Write();
        if (Settings) {
            newSettings->Write();
        }
    }

    outFile->Close();
    return 0;
}

