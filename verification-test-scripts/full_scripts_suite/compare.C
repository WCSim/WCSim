void compare(const char* histname, double axis_low, double axis_high, const char* file1, const char* file2, const char * label1, const char * label2, const char * filenamestub, int rebin = -1)
{
  TCanvas * c = new TCanvas();
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  c->SetBottomMargin(0.15);
  c->SetLeftMargin(0.15);

  TFile f1(file1);
  TFile f2(file2);

  TH1 * h1 = 0, * h2 = 0;
  f1.GetObject(histname, h1);
  f2.GetObject(histname, h2);

  if(rebin > 0) {
    h1->Rebin(rebin);
    h2->Rebin(rebin);
  }

  if(axis_low < axis_high) {
    h1->GetXaxis()->SetRangeUser(axis_low, axis_high);
    h2->GetXaxis()->SetRangeUser(axis_low, axis_high);
  }

  h1->SetLineColor(kBlue);
  h2->SetLineColor(kRed);

  if(h1->GetMaximum() > h2->GetMaximum()) {
    h1->Draw();
    h2->SetLineStyle(kDashed);
    h2->Draw("SAME");
  }
  else {
    h2->Draw();
    h1->SetLineStyle(kDashed);
    h1->Draw("SAME");
  }

  TLegend * l = new TLegend(0.4,0.9,0.9,1.0);
  l->SetTextSize(0.042);
  l->SetTextFont(132);
  l->AddEntry(h1, label1, "l");
  l->AddEntry(h2, label2, "l");
  l->Draw();

  TString filename("compare_");
  filename += histname;
  filename += "_";
  filename += filenamestub;
  filename += ".pdf";
  c->SaveAs(filename.Data());
}
