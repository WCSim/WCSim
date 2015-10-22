#include <iostream>
#include <TH1F.h>
#include <stdio.h>     
#include <stdlib.h>    
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <fstream>

int main(){

  ofstream myfile;
  myfile.open ("detector.txt");


  // open input file
  TFile *f = new TFile("output.root","READ");
  
  // geometry tree
  TTree * geom_tree = (TTree*)f->Get("geom_tree");
  Float_t detector_length, detector_radius, pmt_radius;
  Int_t number_of_pmts;
  geom_tree->SetBranchAddress("detector_length",&detector_length);
  geom_tree->SetBranchAddress("detector_radius",&detector_radius);
  geom_tree->SetBranchAddress("pmt_radius",&pmt_radius);
  geom_tree->SetBranchAddress("number_of_pmts",&number_of_pmts);
  geom_tree->GetEntry(0);
  std::clog << " detector_length " << detector_length << " detector_radius " << detector_radius << " pmt_radius " << pmt_radius << " number_of_pmts " << number_of_pmts << std::endl;

  myfile << detector_length << "  " << detector_radius << " " << pmt_radius << " \n";

  myfile.close();

  exit(0);

  return 1;


}


