#ifndef monoJetCR_C
#define monoJetCR_C

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
  tightID_sf = tightISO_sf = looseID_sf = looseISO_sf = 1;
  
}

void monoJetDoubleMuCR::initTree(TTree* tree) {
  // tree->Branch("dileptonM",&dilepton_mass,"Z Mass (GeV)");
  // tree->Branch("dileptonPt",&dilepton_pt,"Z P_{T} (GeV)");
  // tree->Branch("leadingLeptonPt",&leadingLepton_pt,"Leading Lepton P_{T} (GeV)");
  // tree->Branch("leadingLeptonEta",&leadingLepton_eta,"Leading Lepton Eta");
  // tree->Branch("leadingLeptonPhi",&leadingLepton_phi,"Leading Lepton Phi");
  // tree->Branch("subleadingLeptonPt",&subleadingLepton_pt,"Subleading Lepton P_{T} (GeV)");
  // tree->Branch("subleadingLeptonEta",&subleadingLepton_eta,"Subleading Lepton Eta");
  // tree->Branch("subleadingLeptonPhi",&subleadingLepton_phi,"Subleading Lepton Phi");
  tree->Branch("tightID_sf",&tightID_sf);
  tree->Branch("tightISO_sf",&tightISO_sf);
  tree->Branch("looseID_sf",&looseID_sf);
  tree->Branch("looseISO_sf",&looseISO_sf);
}

void monoJetDoubleMuCR::BookHistos(int i,string histname) {
  if (i == -1) {
    
  } else {
    auto Name = [histname](string name) { return (name + histname); };
    
    h_leadingLeptonPt[i]     = MakeTH1F(new TH1F(Name("leadingLeptonPt").c_str()    ,"leadingLeptonPt;Leading Lepton P_{T} (GeV)"       ,nLeadingLeptonPtBins,LeadingLeptonPtBins));   
    h_leadingLeptonEta[i]    = MakeTH1F(new TH1F(Name("leadingLeptonEta").c_str()   ,"leadingLeptonEta;Leading Lepton #eta"             ,nEtaBins,lEta,uEta));              
    h_leadingLeptonPhi[i]    = MakeTH1F(new TH1F(Name("leadingLeptonPhi").c_str()   ,"leadingLeptonPhi;Leading Lepton #phi"             ,nPhiBins,lPhi,uPhi));        
    h_subleadingLeptonPt[i]  = MakeTH1F(new TH1F(Name("subleadingLeptonPt").c_str() ,"subleadingLeptonPt;Subleading Lepton P_{T} (GeV)" ,nSubLeadingLeptonPtBins,subLeadingLeptonPtBins));
    h_subleadingLeptonEta[i] = MakeTH1F(new TH1F(Name("subleadingLeptonEta").c_str(),"subleadingLeptonEta;Subleading Lepton #eta"       ,nEtaBins,lEta,uEta));              
    h_subleadingLeptonPhi[i] = MakeTH1F(new TH1F(Name("subleadingLeptonPhi").c_str(),"subleadingLeptonPhi;Subleading Lepton #phi"       ,nPhiBins,lPhi,uPhi));        
    h_dileptonPt[i]          = MakeTH1F(new TH1F(Name("dileptonPt").c_str()         ,"dileptonPt;Z P_{T} (GeV)"                         ,30,0.,1500.));              
    h_dileptonM[i]           = MakeTH1F(new TH1F(Name("dileptonM").c_str()          ,"dileptonM;Z Mass (GeV)"                           ,24,60.,120.));
    h_dileptonMall[i]        = MakeTH1F(new TH1F(Name("dileptonMall").c_str()       ,"dileptonM;Z Mass (GeV)"                           ,50,30.,200.));      
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
  }
}

bool monoJetDoubleMuCR::CRSelection(vector<int> tightlist,vector<int> looselist) {
  if (tightlist.size() == 0) return false;
  for(int j=0; j<looselist.size(); ++j){
    //Event must have exactly two muons with opposite charge
    if(muCharge->at(tightlist[0]) * muCharge->at(looselist[j]) == -1){

      lep1.SetPtEtaPhiE(muPt->at(tightlist[0]), muEta->at(tightlist[0]), muPhi->at(tightlist[0]), muE->at(tightlist[0]));
      lep2.SetPtEtaPhiE(muPt->at(looselist[j]), muEta->at(looselist[j]), muPhi->at(looselist[j]), muE->at(looselist[j]));
      leadLepIndx    = tightlist[0];
      subleadLepIndx = looselist[j];
      
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
  }
  return false;
}

void monoJetDoubleMuCR::setRecoil() {
  TLorentzVector met_4vec;
  met_4vec.SetPtEtaPhiE(pfMET,0.,pfMETPhi,pfMET);
  TLorentzVector leptoMET_4vec = ll+met_4vec;
  recoil = fabs(leptoMET_4vec.Pt());
  recoilPhi = leptoMET_4vec.Phi();
}


float monoJetDoubleMuCR::getSF(int leading,int subleading) {
  float leading_pt = muPt->at(leading); float leading_abseta = fabs(muEta->at(leading)); 
  float subleading_pt = muPt->at(subleading); float subleading_abseta = fabs(muEta->at(subleading));

  tightID_sf = th2fmap.getBin("muon_id_tight",leading_pt,leading_abseta);
  tightISO_sf = th2fmap.getBin("muon_iso_tight",leading_pt,leading_abseta);
  looseID_sf = th2fmap.getBin("muon_id_loose",subleading_pt,subleading_abseta);
  looseISO_sf = th2fmap.getBin("muon_iso_loose",subleading_pt,subleading_abseta);

  return tightID_sf * tightISO_sf * looseID_sf * looseISO_sf;
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
