#ifndef monoJet_WECR_C
#define monoJet_WECR_C

#include "monoJetSingleEleCR.h"
#include "VariableBins.h"
#include "Utilities.h"

using namespace std;

Region monoJetAnalysis::REGION = WE;
CRobject monoJetAnalysis::CROBJECT = Electron;

void monoJetSingleEleCR::initVars() {
  lepindex = lepton_pt = lepton_eta = lepton_phi = lepMET_mt = -99;
  reco_sf = tightID_sf = 1;
}

void monoJetSingleEleCR::initTree(TTree* tree) {
  tree->Branch("LeptonPt",&lepton_pt,"Lepton P_{T} (GeV)");
  tree->Branch("LeptonEta",&lepton_eta,"Lepton Eta");
  tree->Branch("LeptonPhi",&lepton_phi,"LeptonPhi");
  tree->Branch("reco_sf",&reco_sf);
  tree->Branch("tightID_sf",&tightID_sf);
}

void monoJetSingleEleCR::BookHistos(int i,TString histname) {
  if (i == -1) {
    h_lepMET_MTBefore  = MakeTH1F(new TH1F("h_lepMET_MT","h_lepMET_MT; transverse mass of the lepton-Emiss system",40,0,400));
  } else {
    auto Name = [histname](TString name) { return (name+histname); };
    h_LeptonPt[i]  = MakeTH1F(new TH1F(Name("LeptonPt") ,"LeptonPt;Lepton P_{T}" ,nLeadingLeptonPtBins,LeadingLeptonPtBins));
    h_LeptonEta[i] = MakeTH1F(new TH1F(Name("LeptonEta"),"LeptonEta;Lepton #eta" ,nEtaBins,lEta,uEta));
    h_LeptonPhi[i] = MakeTH1F(new TH1F(Name("LeptonPhi"),"LeptonPhi;Lepton #phi" ,nPhiBins,lPhi,uPhi));
    h_lepMET_MT[i] = MakeTH1F(new TH1F(Name("lepMET_MT"),"lepMET_MT; transverse mass of the lepton-Emiss system",40,0,400));
    
    h_LeptonEtaPhi[i] = new TH2F(Name("LeptonEtaPhi"),"LeptonEtaPhi;Lepton #eta;Lepton #phi" ,nEtaBins,lEta,uEta,nPhiBins,lPhi,uPhi);
  }
}

void monoJetSingleEleCR::fillHistos(int nhist,float event_weight) {
  if (cutflow->getLabel(nhist+1) == s_EleMET_MT) {
    bool cleaning = recoil > recoilCut;
    float bjetCut = bjetDeepCSVCut_2017;
    if ( YEAR == 2018 ) {
      bjetCut = bjetDeepCSVCut_2018;
      cleaning = cleaning && getJetHEMVeto();
    }
    if (cleaning) {
    vector<int> jetlist = getLooseJet();
    float dphi = dPhiJetMETmin(jetlist,recoilPhi);
    int jetCand = getJetCand();
    float temp_weight = event_weight;
    if (getMetFilter() &&
	muon_veto()   &&
	photon_veto(lepindex) &&
	tau_veto(lepindex) &&
	bjet_weights(bjetCut,temp_weight) &&
	dphi > dPhiJetMETCut &&
	dpfcalo < metRatioCut &&
	jetCand != -1
	)
      h_lepMET_MTBefore->Fill(lepMET_mt,temp_weight); 
    }
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

bool monoJetSingleEleCR::CRSelection(vector<int> tight,vector<int> loose) {
  if (tight.size() == 1 && loose.size() == 1) {
    lepindex = tight[0];
    lep.SetPtEtaPhiE(eleCalibEt->at(lepindex),eleEta->at(lepindex),elePhi->at(lepindex),eleCalibE->at(lepindex));
    
    lepton_pt = lep.Pt();
    lepton_eta = eleEta->at(lepindex);
    lepton_phi = elePhi->at(lepindex);
    setRecoil();
    return true;
  }
  return false;
}

void monoJetSingleEleCR::setRecoil() {
  lepMET_mt = getMt(eleCalibEt->at(lepindex),elePhi->at(lepindex),pfMET,pfMETPhi);
  TLorentzVector met_4vec;
  met_4vec.SetPtEtaPhiE(pfMET,0.,pfMETPhi,pfMET);
  TLorentzVector leptoMET_4vec = lep+met_4vec;
  recoil = fabs(leptoMET_4vec.Pt());
  recoilPhi = leptoMET_4vec.Phi();
  dpfcalo = dPFCaloMET(recoil);
}

float monoJetSingleEleCR::getSF(int lepindex) {
  float eta = eleSCEta->at(lepindex); float pt = eleCalibEt->at(lepindex);

  reco_sf = th2fmap.getBin("ele_reco",eta,pt);
  tightID_sf = th2fmap.getBin("ele_id_tight",eta,pt);
  if ( YEAR == 2017 && pt < 20 )
    reco_sf = th2fmap.getBin("ele_reco_pt_lt_20",eta,pt);
  return reco_sf * tightID_sf;
}

//Veto failed if a muon is found that passes Loose Muon ID, Loose Muon Isolation, and muPtcut, and does not overlap the candidate electron and jet within dR of 0.5
bool monoJetSingleEleCR::muon_veto() {
  vector<int> tmpcands = getLooseMu();
  return tmpcands.size() == 0;
}

bool monoJetSingleEleCR::photon_veto(int lepindex) {
  vector<int> pho_cands; pho_cands.clear();

  vector<int> tmpcands = getLoosePho();
  for (int ipho : tmpcands ) {
    float dR_ele = deltaR(phoSCEta->at(ipho),phoSCPhi->at(ipho),eleSCEta->at(lepindex),eleSCPhi->at(lepindex));
    if ( dR_ele > Iso5Cut )
      pho_cands.push_back(ipho);
  }
  return pho_cands.size() == 0;
}

bool monoJetSingleEleCR::tau_veto(int lepindex) {
  vector<int> tau_cands; tau_cands.clear();

  vector<int> tmpcands = getLooseTau();
  for (int itau : tmpcands ) {
    float dR_ele = deltaR(tau_Eta->at(itau),tau_Phi->at(itau),eleSCEta->at(lepindex),eleSCPhi->at(lepindex));
    if ( dR_ele > Iso4Cut )
      tau_cands.push_back(itau);
  }
  return tau_cands.size() == 0;
}
#endif
