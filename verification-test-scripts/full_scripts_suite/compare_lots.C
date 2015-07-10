void split_args(TString str, vector<string> & vec, const char * delim = ":") {
  vec.clear();
  TObjArray * tokens = str.Tokenize(delim);
  for(int i = 0; i < tokens->GetEntries(); i++) {
    TObjString * objstr = (TObjString*)tokens->At(i);
    string value = objstr->String().Data();
    vec.push_back(value);
  }//i
}

void compare_lots(const char* histname, double axis_low, double axis_high, TString infilenames, TString labels, const char * filenamestub, int rebin = -1)
{
  TCanvas * c = new TCanvas();
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  c->SetTopMargin(0.15);
  c->SetBottomMargin(0.15);
  c->SetLeftMargin(0.15);
  const int max_colors = 7;
  Color_t colors[max_colors] = { kBlack, kBlue, kRed, kGreen+2, kOrange, kMagenta, kCyan };

  //check the input filenames - should be at least 2
  vector<string> infilename_tokens, label_tokens;
  split_args(infilenames, infilename_tokens, "$");
  split_args(labels, label_tokens);
  if(label_tokens.size() < 2 || (label_tokens.size() < infilename_tokens.size())) {
    cout << "Must provide an equal number of labels and filenames. And at least 2. Exiting..." << endl;
    for(int i = 0; i < label_tokens.size(); i++)
      cout << label_tokens[i] << endl;
    for(int i = 0; i < infilename_tokens.size(); i++)
      cout << infilename_tokens[i] << endl;
    exit(1);
  }
  else if(label_tokens.size() > max_colors) {
    cout << "Need to add another entry to the color array. Exiting..." << endl;
    exit(1);
  }
  const int nhists = label_tokens.size();
  int looper = nhists;
  TFile * f[nhists];
  TH1   * h[nhists];

  double ymax = 0;
  double xmin =  999999;
  double xmax = -999999;
  //for(int i = nhists - 1; i > 0; i--) {
  for(int i = 0; i < nhists; i++) {
    cout << i << "\t";
    cout << infilename_tokens[i] << endl;

    f[i] = new TFile(infilename_tokens[i].c_str());
    /* 
       if(!f[i].IsOpen())
       exit(1);
    */
    h[i] = 0;
    f[i]->GetObject(histname, h[i]);
    if(!h[i]) {
      cout << "Cannot find histogram " << histname << " in file " << infilename_tokens[i] << endl;
      looper--;
      continue;//return;
    }
    if(rebin > 0)
      h[i]->Rebin(rebin);
    if(h[i]->GetXaxis()->GetBinCenter(h[i]->FindFirstBinAbove(1E-5)) < xmin)
      xmin = h[i]->GetXaxis()->GetBinCenter(h[i]->FindFirstBinAbove(1E-5));
    if(h[i]->GetXaxis()->GetBinCenter(h[i]->FindLastBinAbove(1E-5)) > xmax)
      xmax = h[i]->GetXaxis()->GetBinCenter(h[i]->FindLastBinAbove(1E-5));
    if(axis_low < axis_high)
      h[i]->GetXaxis()->SetRangeUser(axis_low, axis_high);
    if(h[i]->GetMaximum() > ymax)
      ymax = h[i]->GetMaximum();
    h[i]->SetLineColor(colors[i]);
  }
  ymax *= 1.05;

  for(int i = 0; i < looper; i++) {
    if(axis_low < -9999 && axis_high < -9999)
      h[i]->GetXaxis()->SetRangeUser(xmin, xmax);      
    h[i]->GetYaxis()->SetRangeUser(0, ymax);
    if(i == 0) {
      h[i]->Draw();
    }
    else
      h[i]->Draw("SAME");
  }
  h[0]->Draw("SAME");


  TLegend * l = new TLegend(0.4,0.85,0.9,1.0);
  l->SetTextSize(0.042);
  l->SetTextFont(132);
  for(int i = 0; i < looper; i++) {
    l->AddEntry(h[i], label_tokens[i].c_str(), "l");
  }
  if(nhists == 2) {
    double prob = h[0]->KolmogorovTest(h[1]);
    l->AddEntry((TObject*)0, TString::Format("KS prob %.2f", prob), "");
  }
  l->Draw();

  
  TString filename("compare_");
  filename += histname;
  filename += "_";
  filename += filenamestub;
  filename += ".pdf";
  c->SaveAs(filename.Data());
}
