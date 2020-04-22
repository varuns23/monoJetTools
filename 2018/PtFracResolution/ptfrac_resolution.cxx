void ptfrac_resolution() {
  TTree* tree = (TTree*)_file0->Get("monoJet_12/trees/tree");

  tree->Draw("1-ChNemPtFrac/GenChNemPtFrac>>ChNemPtFrac_Res(20,-1,1)");
  TH1F* h = (TH1F*)gDirectory->Get("ChNemPtFrac_Res");
  h->SetTitle("ChNemPtFrac Resolution");
  h->GetXaxis()->SetTitle("(GEN-RECO)/GEN");
  h->Fit("landau");
  gPad->Clear();
  auto ratio = new TRatioPlot(h);
  ratio->Draw();
  gPad->Update();
}
