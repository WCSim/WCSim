#include <iostream>
#include <TH1F.h>
#include <stdio.h>     
#include <stdlib.h>    
#include <vector>

void write_all_pmts()
{

  ofstream myfile;
  myfile.open ("all_pmts.txt");

  // open input file
  TFile *f = new TFile("output.root","READ");
  
  // all pmts tree
  TTree * all_pmts_tree = (TTree*)f->Get("all_pmts_tree");
  Int_t pmt_number, pmt_location;
  Float_t pmt_ux, pmt_uy, pmt_uz, pmt_x, pmt_y, pmt_z;
  all_pmts_tree->SetBranchAddress("pmt_number",&pmt_number);
  all_pmts_tree->SetBranchAddress("pmt_location",&pmt_location);
  all_pmts_tree->SetBranchAddress("pmt_ux",&pmt_ux);
  all_pmts_tree->SetBranchAddress("pmt_uy",&pmt_uy);
  all_pmts_tree->SetBranchAddress("pmt_uz",&pmt_uz);
  all_pmts_tree->SetBranchAddress("pmt_x",&pmt_x);
  all_pmts_tree->SetBranchAddress("pmt_y",&pmt_y);
  all_pmts_tree->SetBranchAddress("pmt_z",&pmt_z);
  for(size_t ipmt = 0; ipmt < all_pmts_tree->GetEntries(); ipmt++){
    all_pmts_tree->GetEntry(ipmt);

    myfile << pmt_number << "  " << pmt_x << "  " << pmt_y << "  " << pmt_z << " \n";
  }

  myfile.close();

  exit(0);

  return;

}


