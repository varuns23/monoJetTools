TCanvas* ptres_plot(TTree* tree,TCut cut) {

  TCanvas* c = new TCanvas;
  tree->Draw("1-ChNemPtFrac/GenChNemPtFrac>>htemp(50,-1,1)",cut,"goff");
  TH1F* h = (TH1F*)(gDirectory->Get("htemp"))->Clone(cut);
  h->SetTitle("ChNemPtFrac Resolution");
  h->GetXaxis()->SetTitle("(GEN-RECO)/GEN");
  h->Draw("hist");
  return c;
}

void binres_sys() {
  TTree* tree = (TTree*)_file0->Get("monoJet_12/trees/tree");

  const vector<float> ptbins = {0.,0.3,0.5,0.7,0.8,0.9,1.0};

  for (int i = 0; i < ptbins.size()-1; i++) {
    TCut cut = ( to_string(ptbins[i]) + " <= ChNemPtFrac && ChNemPtFrac" + (ptbins[i+1] == 1.0 ? " <= " : " < ") + to_string(ptbins[i+1]) ).c_str();
    ptres_plot(tree,cut);
  }
}
