#ifndef monoJetSingleEleCR_C
#define monoJetSingleEleCR_C

#include "monoJetSingleEleCR.h"
#include "VariableBins.h"
#include "Utilities.h"

using namespace std;

const string monoJetSingleEleCR::REGION = "SingleEleCR";

void monoJetSingleEleCR::initVars() {
  lepindex = lepton_pt = lepton_eta = lepton_phi = lepMET_mt = -99;
}

void monoJetSingleEleCR::initTree(TTree* tree) {
  tree->Branch("LeptonPt",&lepton_pt,"Lepton P_{T} (GeV)");
  tree->Branch("LeptonEta",&lepton_eta,"Lepton Eta");
  tree->Branch("LeptonPhi",&lepton_phi,"LeptonPhi");
}

void monoJetSingleEleCR::BookHistos(int i,string histname) {
  if (i == -1) {
    h_lepMET_MT    = MakeTH1F(new TH1F("h_lepMET_MT","h_lepMET_MT; transverse mass of the lepton-Emiss system",40,0,400));
  } else {
    auto Name = [histname](string name) { return (name+histname); };
    h_LeptonPt[i]  = MakeTH1F(new TH1F(Name("LeptonPt").c_str() ,"LeptonPt;Lepton P_{T}" ,nLeadingLeptonPtBins,LeadingLeptonPtBins));
    h_LeptonEta[i] = MakeTH1F(new TH1F(Name("LeptonEta").c_str(),"LeptonEta;Lepton #eta" ,nEtaBins,lEta,uEta));
    h_LeptonPhi[i] = MakeTH1F(new TH1F(Name("LeptonPhi").c_str(),"LeptonPhi;Lepton #phi" ,nPhiBins,lPhi,uPhi));
  }
}

void monoJetSingleEleCR::fillHistos(int nhist,float event_weight) {
  //CR Histograms
  if(lepindex >= 0){
    h_LeptonPt[nhist] ->Fill(lepton_pt,event_weight);
    h_LeptonEta[nhist]->Fill(lepton_eta,event_weight);
    h_LeptonPhi[nhist]->Fill(lepton_phi,event_weight);
  }
}

bool monoJetSingleEleCR::CRSelection(vector<int> tight,vector<int> loose) {
  if (tight.size() == 1 && loose.size() == 1) {
    lepindex = tight[0];
    lep.SetPtEtaPhiE(elePt->at(lepindex),eleEta->at(lepindex),elePhi->at(lepindex),eleE->at(lepindex));
    
    lepton_pt = lep.Pt();
    lepton_eta = eleEta->at(lepindex);
    lepton_phi = elePhi->at(lepindex);
    lepMET_mt = getMt(elePt->at(lepindex),elePhi->at(lepindex),pfMET,pfMETPhi);
    TLorentzVector met_4vec;
    met_4vec.SetPtEtaPhiE(pfMET,0.,pfMETPhi,pfMET);
    TLorentzVector leptoMET_4vec = lep+met_4vec;
    recoil = fabs(leptoMET_4vec.Pt());
    recoilPhi = leptoMET_4vec.Phi();
    return true;
  }
  return false;
}

float monoJetSingleEleCR::getSF(int lepindex) {
  float eta = eleSCEta->at(lepindex); float pt = elePt->at(lepindex);
  float eleRecoSF_corr= th2fmap.getBin("eleRecoSF_highpt",eta,pt);
  // std::cout<<"eleRecoSF_corr =  "<< eleRecoSF_corr<<std::endl;
  float eleEffSF_corr= th2fmap.getBin("eleIDSF_tight",eta,pt);
  // std::cout<<"eleEffSF_corr =  "<< eleEffSF_corr<<std::endl;
  float eleTriggSF = th2fmap.getBin("eleTriggSF",fabs(eta),pt);

  return eleRecoSF_corr * eleEffSF_corr * eleTriggSF;
}

vector<int> monoJetSingleEleCR::getJetCand(vector<int> jetlist,int lepindex) {
  vector<int> jet_cand; jet_cand.clear();

  vector<int> tmpcands = monoJetAnalysis::getJetCand(jetlist);
  for (int ijet : tmpcands) {
    float dR_jet = deltaR(jetEta->at(ijet),jetPhi->at(ijet),eleSCEta->at(lepindex),eleSCPhi->at(lepindex));
    if (dR_jet > Iso4Cut)
      jet_cand.push_back(ijet);
  }
  return jet_cand;
}

vector<int> monoJetSingleEleCR::jet_veto(int lepindex) {
  vector<int> jetindex; jetindex.clear();

  vector<int> tmpcands = getLooseJet();
  for(int ijet : tmpcands ) {
    float dR_ele = deltaR(jetEta->at(ijet),jetPhi->at(ijet),eleSCEta->at(lepindex),eleSCPhi->at(lepindex));
    if( dR_ele > Iso4Cut )
      jetindex.push_back(ijet);
  }
  return jetindex;
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

bool monoJetSingleEleCR::bjet_veto(int lepindex) {
  vector<int> bjet_cands; bjet_cands.clear();

  vector<int> tmpcands = getLooseBJet();
  for (int ijet : tmpcands) {
    float dR_ele = deltaR(jetEta->at(ijet),jetPhi->at(ijet),eleSCEta->at(lepindex),eleSCPhi->at(lepindex));
    if ( dR_ele > Iso4Cut )
      bjet_cands.push_back(ijet);
  }
  return bjet_cands.size() == 0;
}
#endif
