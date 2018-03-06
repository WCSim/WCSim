#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

void compareProcessed(){
  
  std::vector<std::string> histName;
  histName.push_back("hPEByEvtsByPMT");
  histName.push_back("hPECollectedByEvtsByPMT");
  histName.push_back("hPEByEvts");
  histName.push_back("hPECollectedByEvts");

  int histColor=0;

  TFile *f1 = new TFile("PROCESSED_OD_mu2000MeV.root");
  TFile *f2 = new TFile("PROCESSED_OD_mu2000MeV_noWLS.root");

  std::vector<TH1D *> f1h;
  std::vector<TH1D *> f2h;

  for(int i=0;i<4;i++){
    histColor = kBlue-4;
    f1h.push_back((TH1D*)f1->Get(histName[i].c_str()));
    f1h[i]->SetLineColor(histColor);
    histColor = kRed-4;
    f2h.push_back((TH1D*)f2->Get(histName[i].c_str()));
    f2h[i]->SetLineColor(histColor);
  }

  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  c1->Divide(2,2);
  for(int i=0;i<4;i++){
    c1->cd(i+1);
    if(i==0 || i==1) gPad->SetLogy();
    f1h[i]->Draw(""); f2h[i]->Draw("same");
    f1h[i]->Sumw2(); f2h[i]->Sumw2();
  }

  TH1D *h[4];
  for(int i=0;i<4;i++){
    h[i] = (TH1D*)f1h[i]->Clone("h1");
    h[i]->Add(f2h[i],-1);
    h[i]->Divide(f1h[i]);
  }
  
  TCanvas *c1c1 = new TCanvas("c2","c2",900,720);
  c1c1->Divide(2,4);
  std::string nameLeg = "OD - noWLS (%)";

  
  ///////////////////////////////////////
  //  TOP PART : Nb HITS
  ///////////////////////////////////////

  c1c1->cd(1);
  gPad->SetLogy();
  f1h[0]->Draw("HIST");
  f2h[0]->Draw("HISTsame");
  f1h[0]->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  f1h[0]->GetYaxis()->SetLabelSize(10);
  f1h[0]->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  f1h[0]->GetXaxis()->SetLabelSize(10);
  c1c1->cd(1)->SetPad(0.01, 0.65, 0.50, 0.99);
  c1c1->cd(3);
  c1c1->cd(3)->SetPad(0.01, 0.50, 0.50, 0.67);
  h[0]->Draw("ep");
  h[0]->SetTitle("");
  h[0]->SetStats(0);      // No statistics on lower plot
  h[0]->GetYaxis()->SetTitle(nameLeg.c_str());
  h[0]->GetYaxis()->SetTitleSize(15);
  h[0]->GetYaxis()->SetTitleFont(43);
  h[0]->GetYaxis()->SetTitleOffset(1.60);
  h[0]->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  h[0]->GetYaxis()->SetLabelSize(10);
  
  c1c1->cd(2);
  gPad->SetLogy();
  f1h[1]->Draw("HIST");
  f2h[1]->Draw("HISTsame");
  f1h[1]->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  f1h[1]->GetYaxis()->SetLabelSize(10);
  f1h[1]->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  f1h[1]->GetXaxis()->SetLabelSize(10);
  c1c1->cd(2)->SetPad(0.50, 0.65, 0.99, 0.99);
  c1c1->cd(4);
  c1c1->cd(4)->SetPad(0.50, 0.50, 0.99, 0.67);
  h[1]->Draw();
  h[1]->SetTitle("");
  h[1]->SetStats(0);      // No statistics on lower plot
  h[1]->GetYaxis()->SetTitle(nameLeg.c_str());
  h[1]->GetYaxis()->SetTitleSize(15);
  h[1]->GetYaxis()->SetTitleFont(43);
  h[1]->GetYaxis()->SetTitleOffset(1.60);
  h[1]->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  h[1]->GetYaxis()->SetLabelSize(10);

  ///////////////////////////////////////
  //  BOTTOM PART : Q and TIME
  ///////////////////////////////////////

  c1c1->cd(5);
  f1h[2]->Draw("HIST");
  f2h[2]->Draw("HISTsame");
  f1h[2]->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  f1h[2]->GetYaxis()->SetLabelSize(10);
  f1h[2]->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  f1h[2]->GetXaxis()->SetLabelSize(10);
  c1c1->cd(5)->SetPad(0.01, 0.13, 0.50, 0.50);
  c1c1->cd(7);
  c1c1->cd(7)->SetPad(0.01, 0.01, 0.50, 0.15);
  h[2]->Draw();
  h[2]->SetTitle("");
  h[2]->SetStats(0);      // No statistics on lower plot
  h[2]->GetYaxis()->SetTitle(nameLeg.c_str());
  h[2]->GetYaxis()->SetTitleSize(15);
  h[2]->GetYaxis()->SetTitleFont(43);
  h[2]->GetYaxis()->SetTitleOffset(1.50);
  h[2]->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  h[2]->GetYaxis()->SetLabelSize(10);

  c1c1->cd(6);
  f1h[3]->Draw("HIST");
  f2h[3]->Draw("HISTsame");
  f1h[3]->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  f1h[3]->GetYaxis()->SetLabelSize(10);
  f1h[3]->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  f1h[3]->GetXaxis()->SetLabelSize(10);
  c1c1->cd(6)->SetPad(0.50, 0.13, 0.99, 0.50);
  c1c1->cd(8);
  c1c1->cd(8)->SetPad(0.50, 0.01, 0.99, 0.15);
  h[3]->Draw();
  h[3]->SetTitle("");
  h[3]->SetStats(0);      // No statistics on lower plot
  h[3]->GetYaxis()->SetTitle(nameLeg.c_str());
  h[3]->GetYaxis()->SetTitleSize(15);
  h[3]->GetYaxis()->SetTitleFont(43);
  h[3]->GetYaxis()->SetTitleOffset(1.70);
  h[3]->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  h[3]->GetYaxis()->SetLabelSize(10);

}
