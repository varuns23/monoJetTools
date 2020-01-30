#ifndef monoJetSingleMuCR_C
#define monoJetSingleMuCR_C

#include "monoJetSingleMuCR.h"
#include "VariableBins.h"
#include "monoJetCutConfig.h"
#include "Utilities.h"

using namespace std;

const string monoJetSingleMuCR::REGION = "SingleMuCR";

void monoJetSingleMuCR::initVars() {
  lepindex = lepton_pt = lepton_eta = lepton_phi = -99;
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

vector<int> monoJetSingleMuCR::getJetCand(vector<int> jetlist, int lead_lepIndex){  
  vector<int> jet_cands;   
  jet_cands.clear();       
                           
  for(int j; j<jetlist.size(); j++){
    int i = jetlist.at(j); 
                           
    bool kinematics = (jetPt->at(i) > 100.0 && fabs(jetEta->at(i)) < 2.4);
    bool Id = (jetNHF->at(i) < 0.8 && jetCHF->at(i) > 0.1);
                           
    float dR_lead_mu = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(lead_lepIndex),  muPhi->at(lead_lepIndex));
                           
    if(kinematics && Id && dR_lead_mu > 0.4)
      jet_cands.push_back(i);
  }                        
                           
  return jet_cands;        
}


bool monoJetSingleMuCR::CRSelection(vector<int> tight,vector<int> loose) {
  if (tight.size() == 1 && loose.size() == 1) {
    lepindex = tight[0];
    
    lep.SetPtEtaPhiE(muPt->at(lepindex),muEta->at(lepindex),muPhi->at(lepindex),muE->at(lepindex));

    lepton_pt = lep.Pt();
    lepton_eta = muEta->at(lepindex);
    lepton_phi = muPhi->at(lepindex); 
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

vector<int> monoJetSingleMuCR::getTightMu(vector<int> looseMu){
  vector<int> mu_cands;
  mu_cands.clear();
  
  for(int i = 0; i < looseMu.size(); i++){
    int muCand = looseMu.at(i);
    bool kinematics = (muPt->at(muCand) > 20.0 && (fabs(muEta->at(muCand)) < 2.4)); // muon pt & eta
    bool IdIso = (muIDbit->at(muCand)>>3&1 == 1 && muIDbit->at(muCand)>>9&1 == 1);   // muon tight ID and Iso
  
    if(kinematics && IdIso)
      mu_cands.push_back(muCand);
  }
  return mu_cands;
} 

vector<int> monoJetSingleMuCR::pho_veto_looseID(int leading_mu_index){
  vector<int> pho_cands;
  pho_cands.clear();
  
  for(int i = 0; i < nPho; i++){
    // passes pt cut
    bool kinematics = ((phoEt->at(i) > 15.0) && (fabs(phoSCEta->at(i)) < 2.5));
    bool IdIso = (phoIDbit->at(i)>>0&1==1);
  
    double dR_leadingMu    = deltaR(phoSCEta->at(i),phoSCPhi->at(i), muEta->at(leading_mu_index), muPhi->at(leading_mu_index));
    if( kinematics && IdIso && dR_leadingMu > 0.5 )
      pho_cands.push_back(i);
  }
  return pho_cands;
} 
  
vector<int> monoJetSingleMuCR::tau_veto(int leading_mu_index){
  vector<int> tau_cands;
  tau_cands.clear();
   
  for(int i = 0; i < nTau; i++){
    bool kinematics = ((tau_Pt->at(i) > 18.0) && (fabs(tau_Eta->at(i)) < 2.3));
    bool IdIso = (((tau_IDbits->at(i)>>0&1) == 1) && ((tau_IDbits->at(i)>>13&1) == 1));
   
    double dR_leadingMu    = deltaR(tau_Eta->at(i), tau_Phi->at(i), muEta->at(leading_mu_index), muPhi->at(leading_mu_index));  
    if( kinematics && IdIso && dR_leadingMu > 0.4 )
      tau_cands.push_back(i);
  }
  return tau_cands;
}  
   
vector<int> monoJetSingleMuCR::bjet_veto(vector<int> jetlist, int leading_mu_index){
  vector<int> bjet_cands;
  bjet_cands.clear();
   
  for(int j = 0; j < jetlist.size(); j++){
    int i = jetlist.at(j);
    bool kinematic = (jetPt->at(i) > 20.0 && fabs(jetEta->at(i)) < 2.4);
    bool btagged = ((jetDeepCSVTags_b->at(i) + jetDeepCSVTags_bb->at(i)) > 0.4941);
   
    double dR_leadingMu    = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(leading_mu_index), muPhi->at(leading_mu_index));  
   
    if(kinematic && btagged &&  dR_leadingMu > 0.4 )
      bjet_cands.push_back(i);
  }
  return bjet_cands;
}  
 

bool monoJetSingleMuCR::getMinDphiJR(vector<int> jetlist, int lead_lepIndex, double lepMET_phi){
  bool decision = false;
 
  vector<int> tmpJetlist;
  tmpJetlist.clear();
 
  for(int j=0; j<jetlist.size(); j++){
 
    int i = jetlist.at(j);
 
    bool kinematic = (jetPt->at(i) > 30. && fabs(jetEta->at(i)) < 2.4);
 
    double dR_leadLep    = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(lead_lepIndex), muPhi->at(lead_lepIndex));
 
    if(kinematic && dR_leadLep > 0.4)
      tmpJetlist.push_back(i);
  }
 
  int count=0;
  for(int k=0; k < tmpJetlist.size(); k++){
    if(deltaPhi(jetPhi->at(tmpJetlist.at(k)), lepMET_phi) > 0.5)
      count++;
  }
 
  if(count >= 4 || count == tmpJetlist.size())
    decision = true;
 
  return decision;
}

#endif
