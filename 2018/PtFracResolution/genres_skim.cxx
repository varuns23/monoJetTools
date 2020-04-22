void hstyle(TH1F* h,const char* name) {
  h->SetTitle(name);
  h->GetXaxis()->SetName("(P^{GEN}_T - P^{RECO}_T)/P^{GEN}_T");
  gPad->SaveAs( ("output/"+string(h->GetName())+".png").c_str() );
}

void genres_skim() {
  TTree* tree = (TTree*)_file0->Get("eventTree");
  TCut chplus = "((jetConstPdgId == 211 || jetConstPdgId == 321) && (jetGenJetConstPdgId == 211 || jetGenJetConstPdgId == 321))";
  TCut chminu = "((jetConstPdgId == -211 || jetConstPdgId == -321) && (jetGenJetConstPdgId == -211 || jetGenJetConstPdgId == -321))";
  TCut photon = "((jetConstPdgId == 22 || jetConstPdgId == 111) && (jetGenJetConstPdgId == 22 || jetGenJetConstPdgId == 111))";

  // gStyle->SetOptStat(0);
  
  TH1F* h;
  new TCanvas();
  tree->Draw("1-jetConstPt/jetGenJetConstPt>>ChPlusPt_Res(50,-1,1)",chplus);
  h = (TH1F*)gDirectory->Get("ChPlusPt_Res");
  hstyle(h,"Plus Charged Constituent Pt");
  
  new TCanvas();
  tree->Draw("1-jetConstPt/jetGenJetConstPt>>ChMinuPt_Res(50,-1,1)",chminu);
  h = (TH1F*)gDirectory->Get("ChMinuPt_Res");
  hstyle(h,"Minus Charged Constituent Pt");
  
  new TCanvas();
  tree->Draw("1-jetConstPt/jetGenJetConstPt>>PhoPt_Res(50,-1,1)",photon);
  h = (TH1F*)gDirectory->Get("PhoPt_Res");
  hstyle(h,"Photon Constituent Pt");
  
  new TCanvas();
  tree->Draw("1-jetConstPt/jetGenJetConstPt>>ChPt_Res(50,-1,1)",chplus||chminu);
  h = (TH1F*)gDirectory->Get("ChPt_Res");
  hstyle(h,"Charged Constituent Pt");
  
  new TCanvas();
  tree->Draw("1-jetConstPt/jetGenJetConstPt>>ChNemPt_Res(50,-1,1)",chplus||chminu||photon);
  h = (TH1F*)gDirectory->Get("ChNemPt_Res");
  hstyle(h,"Charged Photon Constituent Pt");
}
