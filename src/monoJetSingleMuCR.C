#ifndef monoJetCR_C
#define monoJetCR_C

#include "monoJetSingleMuCR.h"
#include "VariableBins.h"
#include "Utilities.h"

using namespace std;

Region monoJetAnalysis::REGION = WM;
CRobject monoJetAnalysis::CROBJECT = Muon;

void monoJetSingleMuCR::initVars() {
  lepindex = lepton_pt = lepton_eta = lepton_phi = lepMET_mt = -99;
  tightID_sf = tightISO_sf = 1;
}

void monoJetSingleMuCR::initTree(TTree* tree) {
  // tree->Branch("LeptonPt",&lepton_pt,"Lepton P_{T} (GeV)");
  // tree->Branch("LeptonEta",&lepton_eta,"Lepton Eta");
  // tree->Branch("LeptonPhi",&lepton_phi,"LeptonPhi");
  tree->Branch("tightID_sf",&tightID_sf);
  tree->Branch("tightISO_sf",&tightISO_sf);
}

void monoJetSingleMuCR::BookHistos(int i,string histname) {
  if (i == -1) {
    h_lepMET_MTBefore = MakeTH1F(new TH1F("h_lepMET_MT","h_lepMET_MT; transverse mass of the lepton-Emiss system",40,0,400));
  } else {
    auto Name = [histname](string name) { return (name+histname); };
    h_LeptonPt[i]  = MakeTH1F(new TH1F(Name("LeptonPt").c_str() ,"LeptonPt;Lepton P_{T}" ,nLeadingLeptonPtBins,LeadingLeptonPtBins));
    h_LeptonEta[i] = MakeTH1F(new TH1F(Name("LeptonEta").c_str(),"LeptonEta;Lepton #eta" ,nEtaBins,lEta,uEta));
    h_LeptonPhi[i] = MakeTH1F(new TH1F(Name("LeptonPhi").c_str(),"LeptonPhi;Lepton #phi" ,nPhiBins,lPhi,uPhi));
    h_lepMET_MT[i] = MakeTH1F(new TH1F(Name("lepMET_MT").c_str(),"lepMET_MT; transverse mass of the lepton-Emiss system",40,0,400));
    
    h_LeptonEtaPhi[i] = new TH2F(Name("LeptonEtaPhi").c_str(),"LeptonEtaPhi;Lepton #eta;Lepton #phi" ,nEtaBins,lEta,uEta,nPhiBins,lPhi,uPhi);
  }
}

void monoJetSingleMuCR::fillHistos(int nhist,float event_weight) {
  if (cutflow->getLabel(nhist+1) == s_MuMET_MT) {
    bool cleaning = recoil > recoilCut;
    if ( YEAR == 2018 ) cleaning = cleaning && getJetHEMVeto();
    if (cleaning) h_lepMET_MTBefore->Fill(lepMET_mt,event_weight);
  }
  //CR Histograms
  if(lepindex >= 0){ 
    h_LeptonPt[nhist] ->Fill(lepton_pt,event_weight);
    h_LeptonEta[nhist]->Fill(lepton_eta,event_weight);
    h_LeptonPhi[nhist]->Fill(lepton_phi,event_weight);
    h_lepMET_MT[nhist]->Fill(lepMET_mt,event_weight);
    
    h_LeptonEtaPhi[nhist]->Fill(lepton_eta,lepton_phi,event_weight);
  }
}

bool monoJetSingleMuCR::CRSelection(vector<int> tight,vector<int> loose) {
  if (tight.size() == 1 && loose.size() == 1) {
    lepindex = tight[0];
    
    lep.SetPtEtaPhiE(muPt->at(lepindex),muEta->at(lepindex),muPhi->at(lepindex),muE->at(lepindex));

    lepton_pt = lep.Pt();
    lepton_eta = muEta->at(lepindex);
    lepton_phi = muPhi->at(lepindex);
    setRecoil();
    return true;
  }
  return false;
}

void monoJetSingleMuCR::setRecoil() {
  lepMET_mt = getMt(muPt->at(lepindex),muPhi->at(lepindex),pfMET,pfMETPhi);
  TLorentzVector met_4vec;
  met_4vec.SetPtEtaPhiE(pfMET,0.,pfMETPhi,pfMET);
  TLorentzVector leptoMET_4vec = lep+met_4vec;
  recoil = fabs(leptoMET_4vec.Pt());
  recoilPhi = leptoMET_4vec.Phi();
  dpfcalo = dPFCaloMET(recoil);
}

float monoJetSingleMuCR::getSF(int lepindex) {
  float abseta = fabs(muEta->at(lepindex));
  float pt = muPt->at(lepindex);

  tightID_sf = th2fmap.getBin("muon_id_tight",pt,abseta);
  tightISO_sf = th2fmap.getBin("muon_iso_tight",pt,abseta);
  
  return tightID_sf * tightISO_sf;
}

bool monoJetSingleMuCR::electron_veto() {
  vector<int> tmpcands = getLooseEle();
  return tmpcands.size() == 0;
}

bool monoJetSingleMuCR::photon_veto(int leading_mu_index){
  vector<int> pho_cands;
  pho_cands.clear();

  vector<int> tmpcands = getLoosePho();
  for(int i : tmpcands){
    double dR_leadingMu    = deltaR(phoSCEta->at(i),phoSCPhi->at(i), muEta->at(leading_mu_index), muPhi->at(leading_mu_index));
    if( dR_leadingMu > 0.5 )
      pho_cands.push_back(i);
  }
  return pho_cands.size() == 0;
} 
  
bool monoJetSingleMuCR::tau_veto(int leading_mu_index){
  vector<int> tau_cands;
  tau_cands.clear();
   
  vector<int> tmpcands = getLooseTau();
  for(int i : tmpcands){
    double dR_leadingMu    = deltaR(tau_Eta->at(i), tau_Phi->at(i), muEta->at(leading_mu_index), muPhi->at(leading_mu_index));  
    if( dR_leadingMu > 0.4 )
      tau_cands.push_back(i);
  }
  return tau_cands.size() == 0;
}   

#endif
