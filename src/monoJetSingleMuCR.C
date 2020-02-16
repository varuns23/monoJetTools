#ifndef monoJetSingleMuCR_C
#define monoJetSingleMuCR_C

#include "monoJetSingleMuCR.h"
#include "VariableBins.h"
#include "Utilities.h"

using namespace std;

const string monoJetSingleMuCR::REGION = "SingleMuCR";

void monoJetSingleMuCR::initVars() {
  lepindex = lepton_pt = lepton_eta = lepton_phi = lepMET_mt = -99;
}

void monoJetSingleMuCR::initTree(TTree* tree) {
  tree->Branch("LeptonPt",&lepton_pt,"Lepton P_{T} (GeV)");
  tree->Branch("LeptonEta",&lepton_eta,"Lepton Eta");
  tree->Branch("LeptonPhi",&lepton_phi,"LeptonPhi");
}

void monoJetSingleMuCR::BookHistos(int i,string histname) {
  if (i == -1) {
    h_lepMET_MT = MakeTH1F(new TH1F("h_lepMET_MT","h_lepMET_MT; transverse mass of the lepton-Emiss system",40,0,400));
  } else {
    auto Name = [histname](string name) { return (name+histname); };
    h_LeptonPt[i]  = MakeTH1F(new TH1F(Name("LeptonPt").c_str() ,"LeptonPt;Lepton P_{T}" ,nLeadingLeptonPtBins,LeadingLeptonPtBins));
    h_LeptonEta[i] = MakeTH1F(new TH1F(Name("LeptonEta").c_str(),"LeptonEta;Lepton #eta" ,nEtaBins,lEta,uEta));
    h_LeptonPhi[i] = MakeTH1F(new TH1F(Name("LeptonPhi").c_str(),"LeptonPhi;Lepton #phi" ,nPhiBins,lPhi,uPhi));
  }
}

void monoJetSingleMuCR::fillHistos(int nhist,float event_weight) {
  //CR Histograms
  if(lepindex >= 0){ 
    h_LeptonPt[nhist] ->Fill(lepton_pt,event_weight);
    h_LeptonEta[nhist]->Fill(lepton_eta,event_weight);
    h_LeptonPhi[nhist]->Fill(lepton_phi,event_weight);
  }
}

bool monoJetSingleMuCR::CRSelection(vector<int> tight,vector<int> loose) {
  if (tight.size() == 1 && loose.size() == 1) {
    lepindex = tight[0];
    
    lep.SetPtEtaPhiE(muPt->at(lepindex),muEta->at(lepindex),muPhi->at(lepindex),muE->at(lepindex));

    lepton_pt = lep.Pt();
    lepton_eta = muEta->at(lepindex);
    lepton_phi = muPhi->at(lepindex); 
    lepMET_mt = getMt(muPt->at(lepindex),muPhi->at(lepindex),pfMET,pfMETPhi);
    TLorentzVector met_4vec;
    met_4vec.SetPtEtaPhiE(pfMET,0.,pfMETPhi,pfMET);
    TLorentzVector leptoMET_4vec = lep+met_4vec;
    recoil = fabs(leptoMET_4vec.Pt());
    recoilPhi = leptoMET_4vec.Phi();
    return true;
  }
  return false;
}

float monoJetSingleMuCR::getSF(int lepindex) {
  float eta = muEta->at(lepindex);
  float pt = muPt->at(lepindex);

  float tightMuISO_SF_corr;
  float tightMuID_SF_corr;
    
  if ( th2fmap.contains("tightMuSF_ISO_abseta") ) {
    // Use abseta instead of eta
    tightMuISO_SF_corr = th2fmap.getBin("tightMuSF_ISO_abseta",pt,fabs(eta));
    tightMuID_SF_corr = th2fmap.getBin("tightMuSF_ID_abseta",pt,fabs(eta));
  } else {
    // Use eta
    tightMuISO_SF_corr = th2fmap.getBin("tightMuSF_ISO",pt,eta);
    tightMuID_SF_corr = th2fmap.getBin("tightMuSF_ID",pt,eta);
  }
  
  
  return tightMuISO_SF_corr*tightMuID_SF_corr;
}


vector<int> monoJetSingleMuCR::getJetCand(vector<int> jetlist, int lead_lepIndex){  
  vector<int> jet_cands;   
  jet_cands.clear();       
                    
  vector<int> tmpcands = monoJetAnalysis::getJetCand(jetlist);       
  for(int i : tmpcands){
    float dR_lead_mu = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(lead_lepIndex),  muPhi->at(lead_lepIndex));
                           
    if(dR_lead_mu > 0.4)
      jet_cands.push_back(i);
  }                        
                           
  return jet_cands;        
}


vector<int> monoJetSingleMuCR::jet_veto(int lepindex) {
  vector<int> jetindex; jetindex.clear();
			  
  vector<int> tmpcands = getLooseJet();
  for (int ijet : tmpcands) {
    float dR_mu = deltaR(jetEta->at(ijet),jetPhi->at(ijet),muEta->at(lepindex),muPhi->at(lepindex));
    if ( dR_mu > Iso4Cut)
      jetindex.push_back(ijet);
  }
  return jetindex;
}

bool monoJetSingleMuCR::electron_veto() {
  vector<int> tmpcands = getLooseEle();
  return tmpcands.size() == 0;
}

bool monoJetSingleMuCR::photon_veto(int leading_mu_index){
  vector<int> pho_cands;
  pho_cands.clear();

  vector<int> tmpcands = getLoosePho();
  for(int i : tmpcands ){
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
   
bool monoJetSingleMuCR::bjet_veto(int leading_mu_index){
  vector<int> bjet_cands;
  bjet_cands.clear();
   
  vector<int> tmpcands = getLooseBJet();
  for(int i : tmpcands){
    double dR_leadingMu    = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(leading_mu_index), muPhi->at(leading_mu_index));  
    if(dR_leadingMu > 0.4 )
      bjet_cands.push_back(i);
  }
  return bjet_cands.size() == 0;
}  

#endif
