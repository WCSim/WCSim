#include <iostream>
#include "skqtfitscat.hh"
#include "TFile.h"
#include "TTree.h"
#include "WCSimOpticalPhotonTrackInfo.hh"

//TFile* outfile;
TTree* sttree;

float srcpos[3],srcdir[3];
float oppos[3], tubepos[3], opdir[3];
int ihPMT,isct;

void initscattable(){
  if(!WCSimOpticalPhotonTrackInfo::instance()->isEnabled())
    return;
  //std::cout << "initscattable called" << std::endl;
  if (gFile == NULL) std::cout<<"Now you're in trouble, no gFile"<<std::endl;
  else {
    //std::cout<<"gFile is"<<gFile->GetName()<<std::endl;
    //gFile->cd();
    sttree = new TTree("sttree","Source-Tube Tree");
    //sttree->SetDirectory( gFile->GetDirectory("/") );
    sttree->Branch("srcpos",srcpos,"srcpos[3]/F");
    sttree->Branch("srcdir",srcdir,"srcdir[3]/F");
    sttree->Branch("oppos",oppos,"oppos[3]/F");       //optical photon initial position
    sttree->Branch("tubepos",tubepos,"tubepos[3]/F");
    sttree->Branch("ihPMT",&ihPMT,"ihPMT/I");
    sttree->Branch("isct",&isct,"isct/I");
    sttree->Branch("opdir",opdir,"opdir[3]/F");
  }
}

void fillscattable() {
  WCSimOpticalPhotonTrackInfo* op = WCSimOpticalPhotonTrackInfo::instance();
  
  //std::cout<<"fillscattable() nphotons="<<op->getNumPhotons()<<std::endl;
  // loop over optical photons
  for (int i = 0; i<op->getNumPhotons(); i++){
    if ( op->getPMTId()[ i ] < 0 ) continue;

    // get the parent particle's direction and position
    int paridx = op->getParentIdx()[ i ] ;
    
    srcpos[0] = ( op->getPPos()[ paridx ] ).x();
    srcpos[1] = ( op->getPPos()[ paridx ] ).y();
    srcpos[2] = ( op->getPPos()[ paridx ] ).z();
    
    srcdir[0] = ( op->getPDir()[ paridx ] ).x();
    srcdir[1] = ( op->getPDir()[ paridx ] ).y();
    srcdir[2] = ( op->getPDir()[ paridx ] ).z();

    oppos[0] = op->getPos()[i].x();
    oppos[1] = op->getPos()[i].y();
    oppos[2] = op->getPos()[i].z();
    
    opdir[0] = op->getOpDir()[i].x();
    opdir[1] = op->getOpDir()[i].y();
    opdir[2] = op->getOpDir()[i].z();

    isct = op->getIstory()[i];
    ihPMT = op->getPMTId()[ i ];
    if ( op->getPMTId()[ i ] < 0 ) {
      tubepos[0] = 0.0;
      tubepos[1] = 0.0;
      tubepos[2] = 0.0;      
    } else {
      tubepos[0] = op->getPMTPos()[i].x();
      tubepos[1] = op->getPMTPos()[i].y();
      tubepos[2] = op->getPMTPos()[i].z();
    }
    sttree->Fill();

  }

}

void writescattable(){
  if(!WCSimOpticalPhotonTrackInfo::instance()->isEnabled())
    return;
  //  std::cout << "writescattable called" << std::endl;
  
  sttree->Write();
  //outfile->Close();

}
