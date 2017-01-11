void compare_stacks(const char* stackname, const char *hist1name, const char *hist2name, double axis_low, double axis_high, const char* file1, const char* file2, const char * label11, const char * label12, const char * label21, const char * label22, const char * filenamestub, int rebin = -1)
{
  TCanvas * c = new TCanvas();
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  c->SetBottomMargin(0.15);
  c->SetLeftMargin(0.15);

  TFile f1(file1);
  TFile f2(file2);

  THStack *s1 = 0, *s2 = 0;
  TH1 * h11 = 0, * h12 = 0;
  TH1 * h21 = 0, * h22 = 0;
  //f1.GetObject(stackname, s1);
  f1.GetObject(hist1name, h11);
  f1.GetObject(hist2name, h12);
  //f2.GetObject(stackname, s2);
  f2.GetObject(hist1name, h21);
  f2.GetObject(hist2name, h22);

  if(rebin > 0) {
    h11->Rebin(rebin);
    h12->Rebin(rebin);
    h21->Rebin(rebin);
    h22->Rebin(rebin);
  }

  if(axis_low < axis_high) {
    h11->GetXaxis()->SetRangeUser(axis_low, axis_high);
    h12->GetXaxis()->SetRangeUser(axis_low, axis_high);
    h21->GetXaxis()->SetRangeUser(axis_low, axis_high);
    h22->GetXaxis()->SetRangeUser(axis_low, axis_high);
  }

  h11->SetLineColor(kBlue);
  h12->SetLineColor(kGreen);//kBlue+3);
  h21->SetLineColor(kRed);
  h22->SetLineColor(kYellow);//Red+3);

  h11->SetFillStyle(0);
  h12->SetFillStyle(0);
  h21->SetFillStyle(0);
  h22->SetFillStyle(0);

  s1 = new THStack("s1",TString::Format("s1;%s;%s", h1->GetXaxis()->GetTitle(), h1->GetYaxis()->GetTitle()));
  s2 = new THStack("s2",TString::Format("s2;%s;%s", h1->GetXaxis()->GetTitle(), h1->GetYaxis()->GetTitle()));
  s1->Add(h11);
  s1->Add(h12);
  s2->Add(h21);
  s2->Add(h22);

  if(s1->GetMaximum() > s2->GetMaximum()) {
    s1->Draw();
    s2->Draw("SAME");
  }
  else {
    s2->Draw();
    s1->Draw("SAME");
  }

  TLegend * l = new TLegend(0.4,0.9,0.9,1.0);
  l->SetTextSize(0.042);
  l->SetTextFont(132);
  l->AddEntry(h11, label11, "lpf");
  l->AddEntry(h12, label12, "lpf");
  l->AddEntry(h21, label21, "lpf");
  l->AddEntry(h22, label22, "lpf");
  l->Draw();

  TString filename("compare_");
  filename += stackname;
  filename += "_";
  filename += filenamestub;
  filename += ".pdf";
  c->SaveAs(filename.Data());
}
