TRatioPlot* compare(TFile* f1,TFile* f2,const char* name) {
  auto h1 = (TH1F*)f1->Get(name);
  auto h2 = (TH1F*)f2->Get(name);

  auto ratio = new TRatioPlot(h1,h2);
  ratio->SetH1DrawOpt("pex0");
  ratio->SetH2DrawOpt("hist");
  return ratio;
}
