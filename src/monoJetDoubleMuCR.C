#ifndef monoJet_ZMCR_C
#define monoJet_ZMCR_C

#include "monoJetDoubleMuCR.h"
#include "VariableBins.h"
#include "Utilities.h"

using namespace std;

Region monoJetAnalysis::REGION = ZM;
CRobject monoJetAnalysis::CROBJECT = Muon;

void monoJetDoubleMuCR::initVars() {
  leadLepIndx = subleadLepIndx = -1;
  dilepton_mass = dilepton_pt = -1;
  leadingLepton_pt = leadingLepton_eta = leadingLepton_phi = -99;
  subleadingLepton_pt = subleadingLepton_eta = subleadingLepton_phi = -99;
  m1tightID_sf = m1tightISO_sf = m1looseID_sf = m1looseISO_sf = 1;
  m2tightID_sf = m2tightISO_sf = m2looseID_sf = m2looseISO_sf = 1;
  
}

void monoJetDoubleMuCR::initTree(TTree* tree) {
  tree->Branch("dileptonM",&dilepton_mass,"Z Mass (GeV)");
  tree->Branch("dileptonPt",&dilepton_pt,"Z P_{T} (GeV)");
  tree->Branch("leadingLeptonPt",&leadingLepton_pt,"Leading Lepton P_{T} (GeV)");
  tree->Branch("leadingLeptonEta",&leadingLepton_eta,"Leading Lepton Eta");
  tree->Branch("leadingLeptonPhi",&leadingLepton_phi,"Leading Lepton Phi");
  tree->Branch("subleadingLeptonPt",&subleadingLepton_pt,"Subleading Lepton P_{T} (GeV)");
  tree->Branch("subleadingLeptonEta",&subleadingLepton_eta,"Subleading Lepton Eta");
  tree->Branch("subleadingLeptonPhi",&subleadingLepton_phi,"Subleading Lepton Phi");
  tree->Branch("m1tightID_sf",&m1tightID_sf);
  tree->Branch("m1tightISO_sf",&m1tightISO_sf);
  tree->Branch("m1looseID_sf",&m1looseID_sf);
  tree->Branch("m1looseISO_sf",&m1looseISO_sf);
  tree->Branch("m2tightID_sf",&m2tightID_sf);
  tree->Branch("m2tightISO_sf",&m2tightISO_sf);
  tree->Branch("m2looseID_sf",&m2looseID_sf);
  tree->Branch("m2looseISO_sf",&m2looseISO_sf);
}

void monoJetDoubleMuCR::BookHistos(int i,TString histname) {
  if (i == -1) {
    
  } else {
    auto Name = [histname](TString name) { return (name + histname); };
    
    h_leadingLeptonPt[i]     = MakeTH1F(new TH1F(Name("leadingLeptonPt")    ,"leadingLeptonPt;Leading Lepton P_{T} (GeV)"       ,nLeadingLeptonPtBins,LeadingLeptonPtBins));   
    h_leadingLeptonEta[i]    = MakeTH1F(new TH1F(Name("leadingLeptonEta")   ,"leadingLeptonEta;Leading Lepton #eta"             ,nEtaBins,lEta,uEta));              
    h_leadingLeptonPhi[i]    = MakeTH1F(new TH1F(Name("leadingLeptonPhi")   ,"leadingLeptonPhi;Leading Lepton #phi"             ,nPhiBins,lPhi,uPhi));        
    h_subleadingLeptonPt[i]  = MakeTH1F(new TH1F(Name("subleadingLeptonPt") ,"subleadingLeptonPt;Subleading Lepton P_{T} (GeV)" ,nSubLeadingLeptonPtBins,subLeadingLeptonPtBins));
    h_subleadingLeptonEta[i] = MakeTH1F(new TH1F(Name("subleadingLeptonEta"),"subleadingLeptonEta;Subleading Lepton #eta"       ,nEtaBins,lEta,uEta));              
    h_subleadingLeptonPhi[i] = MakeTH1F(new TH1F(Name("subleadingLeptonPhi"),"subleadingLeptonPhi;Subleading Lepton #phi"       ,nPhiBins,lPhi,uPhi));        
    h_dileptonPt[i]          = MakeTH1F(new TH1F(Name("dileptonPt")         ,"dileptonPt;Z P_{T} (GeV)"                         ,30,0.,1500.));              
    h_dileptonM[i]           = MakeTH1F(new TH1F(Name("dileptonM")          ,"dileptonM;Z Mass (GeV)"                           ,24,60.,120.));
    h_dileptonMall[i]        = MakeTH1F(new TH1F(Name("dileptonMall")       ,"dileptonM;Z Mass (GeV)"                           ,50,30.,200.));  
    
    h_leadingLeptonEtaPhi[i] = new TH2F(Name("leadingLeptonEtaPhi")   ,"leadingLeptonEtaPhi;Leading Lepton #eta;Leading Lepton #phi"             ,nEtaBins,lEta,uEta,nPhiBins,lPhi,uPhi);  
    h_subleadingLeptonEtaPhi[i] = new TH2F(Name("subleadingLeptonEtaPhi")   ,"subleadingLeptonEtaPhi;Subleading Lepton #eta;Subleading Lepton #phi"             ,nEtaBins,lEta,uEta,nPhiBins,lPhi,uPhi);      
  }
}

void monoJetDoubleMuCR::fillHistos(int nhist,float event_weight) {
  if(leadLepIndx >= 0 && subleadLepIndx >= 0){ 
    h_leadingLeptonPt[nhist]    ->Fill(leadingLepton_pt,event_weight);
    h_leadingLeptonEta[nhist]   ->Fill(leadingLepton_eta,event_weight);
    h_leadingLeptonPhi[nhist]   ->Fill(leadingLepton_phi,event_weight);
    h_subleadingLeptonPt[nhist] ->Fill(subleadingLepton_pt,event_weight);
    h_subleadingLeptonEta[nhist]->Fill(subleadingLepton_eta,event_weight);
    h_subleadingLeptonPhi[nhist]->Fill(subleadingLepton_phi,event_weight);
    
    h_dileptonPt[nhist]         ->Fill(dilepton_pt,event_weight);
    h_dileptonM[nhist]          ->Fill(dilepton_mass,event_weight);
    h_dileptonMall[nhist]       ->Fill(dilepton_mass,event_weight);
    
    h_leadingLeptonEtaPhi[nhist]   ->Fill(leadingLepton_eta,leadingLepton_phi,event_weight);
    h_subleadingLeptonEtaPhi[nhist]->Fill(subleadingLepton_eta,subleadingLepton_phi,event_weight);
  }
}

bool monoJetDoubleMuCR::CRSelection(vector<int> tightlist,vector<int> looselist) {
  if (tightlist.size() == 0) return false;
  if (looselist.size() <  2) return false;
  int i1 = looselist[0];
  int i2 = looselist[1];
  
  if(muCharge->at(i1) * muCharge->at(i2) == -1){

    lep1.SetPtEtaPhiE(muPt->at(i1), muEta->at(i1), muPhi->at(i1), muE->at(i1));
    lep2.SetPtEtaPhiE(muPt->at(i2), muEta->at(i2), muPhi->at(i2), muE->at(i2));
    leadLepIndx    = i1;
    subleadLepIndx = i2;
    
    TLorentzVector ll = lep1 + lep2;
    dilepton_mass = ll.M();
    dilepton_pt = ll.Pt();
    
    leadingLepton_pt = lep1.Pt();
    leadingLepton_eta = lep1.Eta();
    leadingLepton_phi = lep1.Phi();
    
    subleadingLepton_pt = lep2.Pt();
    subleadingLepton_eta = lep2.Eta();
    subleadingLepton_phi = lep2.Phi();
    
    setRecoil();
    return true;
  }
  return false;
}

void monoJetDoubleMuCR::setRecoil() {
  TLorentzVector ll = lep1 + lep2;
  TLorentzVector met_4vec;
  met_4vec.SetPtEtaPhiE(pfMET,0.,pfMETPhi,pfMET);
  TLorentzVector leptoMET_4vec = ll+met_4vec;
  recoil = fabs(leptoMET_4vec.Pt());
  recoilPhi = leptoMET_4vec.Phi();
  dpfcalo = dPFCaloMET(recoil);
}


float monoJetDoubleMuCR::getSF(int leading,int subleading) {
  float leading_pt = muPt->at(leading); float leading_abseta = fabs(muEta->at(leading)); 
  float subleading_pt = muPt->at(subleading); float subleading_abseta = fabs(muEta->at(subleading));

  m1tightID_sf = th2fmap.getBin("muon_id_tight",leading_pt,leading_abseta);
  m1tightISO_sf = th2fmap.getBin("muon_iso_tight",leading_pt,leading_abseta);
  m1looseID_sf = th2fmap.getBin("muon_id_loose",leading_pt,leading_abseta);
  m1looseISO_sf = th2fmap.getBin("muon_iso_loose",leading_pt,leading_abseta);
  
  m2tightID_sf = th2fmap.getBin("muon_id_tight",subleading_pt,subleading_abseta);
  m2tightISO_sf = th2fmap.getBin("muon_iso_tight",subleading_pt,subleading_abseta);
  m2looseID_sf = th2fmap.getBin("muon_id_loose",subleading_pt,subleading_abseta);
  m2looseISO_sf = th2fmap.getBin("muon_iso_loose",subleading_pt,subleading_abseta);

  return 0.5*(m1tightID_sf * m1tightISO_sf * m2looseID_sf * m2looseISO_sf + m2tightID_sf * m2tightISO_sf * m1looseID_sf * m1looseISO_sf);
}

bool monoJetDoubleMuCR::electron_veto() {
  vector<int> tmpcands = getLooseEle();
  return tmpcands.size() == 0;
}

bool monoJetDoubleMuCR::photon_veto(int leadLep_index, int subleadLep_index){
  vector<int> pho_cands;
  pho_cands.clear();
  
  vector<int> tmpcands = getLoosePho();
  for(int i : tmpcands){
    double dR_leadLep    = deltaR(phoSCEta->at(i),phoSCPhi->at(i), muEta->at(leadLep_index), muPhi->at(leadLep_index));  
    double dR_subleadLep = deltaR(phoSCEta->at(i),phoSCPhi->at(i), muEta->at(subleadLep_index), muPhi->at(subleadLep_index)); 
    if( dR_leadLep > 0.5 && dR_subleadLep > 0.5)
      pho_cands.push_back(i);
  }
  return pho_cands.size() == 0;
}

bool monoJetDoubleMuCR::tau_veto(int leadLep_index, int subleadLep_index){
  vector<int> tau_cands;
  tau_cands.clear();

  vector<int> tmpcands = getLooseTau();
  for(int i : tmpcands){
    double dR_leadLep    = deltaR(tau_Eta->at(i), tau_Phi->at(i), muEta->at(leadLep_index), muPhi->at(leadLep_index));  
    double dR_subleadLep = deltaR(tau_Eta->at(i), tau_Phi->at(i), muEta->at(subleadLep_index), muPhi->at(subleadLep_index)); 
    if( dR_leadLep > 0.4 && dR_subleadLep > 0.4)
      tau_cands.push_back(i);
  }
  return tau_cands.size() == 0;
}

#endif
