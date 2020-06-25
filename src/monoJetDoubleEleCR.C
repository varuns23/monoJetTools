#ifndef monoJet_ZECR_C
#define monoJet_ZECR_C

#include "monoJetDoubleEleCR.h"
#include "VariableBins.h"
#include "Utilities.h"

using namespace std;

Region monoJetAnalysis::REGION = ZE;
CRobject monoJetAnalysis::CROBJECT = Electron;

void monoJetDoubleEleCR::initVars() {
  leadLepIndx = subleadLepIndx = -1;
  dilepton_mass = dilepton_pt = -1;
  leadingLepton_pt = leadingLepton_eta = leadingLepton_phi = -99;
  subleadingLepton_pt = subleadingLepton_eta = subleadingLepton_phi = -99;
  e1reco_sf = e2reco_sf = tightID_sf = looseID_sf = 1;
}

void monoJetDoubleEleCR::initTree(TTree* tree) {
  tree->Branch("dileptonM",&dilepton_mass,"Z Mass (GeV)");
  tree->Branch("dileptonPt",&dilepton_pt,"Z P_{T} (GeV)");
  tree->Branch("leadingLeptonPt",&leadingLepton_pt,"Leading Lepton P_{T} (GeV)");
  tree->Branch("leadingLeptonEta",&leadingLepton_eta,"Leading Lepton Eta");
  tree->Branch("leadingLeptonPhi",&leadingLepton_phi,"Leading Lepton Phi");
  tree->Branch("subleadingLeptonPt",&subleadingLepton_pt,"Subleading Lepton P_{T} (GeV)");
  tree->Branch("subleadingLeptonEta",&subleadingLepton_eta,"Subleading Lepton Eta");
  tree->Branch("subleadingLeptonPhi",&subleadingLepton_phi,"Subleading Lepton Phi");
  tree->Branch("e1reco_sf",&e1reco_sf);
  tree->Branch("tightID_sf",&tightID_sf);
  tree->Branch("e2reco_sf",&e2reco_sf);
  tree->Branch("looseID_sf",&looseID_sf);
}

void monoJetDoubleEleCR::BookHistos(int i,TString histname) {
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

void monoJetDoubleEleCR::fillHistos(int nhist,float event_weight) {
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
    h_subleadingLeptonEtaPhi[nhist]   ->Fill(subleadingLepton_eta,subleadingLepton_phi,event_weight);
  }
}

bool monoJetDoubleEleCR::CRSelection(vector<int> tightlist,vector<int> looselist) {
  if (tightlist.size() == 0) return false;
  int leading = tightlist[0];
  for (int subleading : looselist) {
    //Event must have exactly two loose electrons with opposite charge
    if (eleCharge->at(leading)*eleCharge->at(subleading) == -1) {
      lep1.SetPtEtaPhiE(eleCalibEt->at(leading),eleEta->at(leading),elePhi->at(leading),eleCalibE->at(leading));
      lep2.SetPtEtaPhiE(eleCalibEt->at(subleading),eleEta->at(subleading),elePhi->at(subleading),eleCalibE->at(subleading));
      leadLepIndx = leading;
      subleadLepIndx = subleading;
      TLorentzVector ll = lep1 + lep2;
      dilepton_mass = ll.M();
      dilepton_pt = ll.Pt();

      leadingLepton_pt = lep1.Pt();
      leadingLepton_eta = lep1.Eta();
      leadingLepton_phi = lep1.Phi();
	
      subleadingLepton_pt = lep2.Pt();
      subleadingLepton_eta = lep2.Eta();
      subleadingLepton_phi = lep2.Phi();
      return true;
    }
  }
  return false;
}

void monoJetDoubleEleCR::setRecoil() {
  TLorentzVector ll = lep1 + lep2;
  TLorentzVector met_4vec;
  met_4vec.SetPtEtaPhiE(pfMET,0.,pfMETPhi,pfMET);
  TLorentzVector leptoMET_4vec = ll+met_4vec;
  recoil = fabs(leptoMET_4vec.Pt());
  recoilPhi = leptoMET_4vec.Phi();
  dpfcalo = dPFCaloMET(recoil);
}

float monoJetDoubleEleCR::getSF(int leading, int subleading) {
  float leading_eta = eleSCEta->at(leading); float leading_pt = eleCalibEt->at(leading);
  float subleading_eta = eleSCEta->at(subleading); float subleading_pt = eleCalibEt->at(subleading);

  e1reco_sf = th2fmap.getBin("ele_reco",leading_eta,leading_pt);
  tightID_sf = th2fmap.getBin("ele_id_tight",leading_eta,leading_pt);
  if ( YEAR == 2017 && leading_pt < 20 )
    e1reco_sf = th2fmap.getBin("ele_reco_pt_lt_20",leading_eta,leading_pt);

  e2reco_sf = th2fmap.getBin("ele_reco",subleading_eta,subleading_pt);
  looseID_sf = th2fmap.getBin("ele_id_loose",subleading_eta,subleading_pt);
  if ( YEAR == 2017 && subleading_pt < 20 )
    e2reco_sf = th2fmap.getBin("ele_reco_pt_lt_20",subleading_eta,subleading_pt);
  
  return e1reco_sf * tightID_sf * e2reco_sf * looseID_sf;
}

//Veto failed if a muon is found that passes Loose Muon ID, Loose Muon Isolation, and muPtcut, and does not overlap the candidate electrons and jet within dR of 0.5
bool monoJetDoubleEleCR::muon_veto() {
  vector<int> tmpcands = getLooseMu();
  return tmpcands.size() == 0;
}

bool monoJetDoubleEleCR::photon_veto(int leading,int subleading) {
  vector<int> pho_cands; pho_cands.clear();

  vector<int> tmpcands = getLoosePho();
  for (int ipho : tmpcands ) {
    float dR_leading = deltaR(phoSCEta->at(ipho),phoSCPhi->at(ipho),eleSCEta->at(leading),eleSCPhi->at(leading));
    float dR_subleading = deltaR(phoSCEta->at(ipho),phoSCPhi->at(ipho),eleSCEta->at(subleading),eleSCPhi->at(subleading));
    if ( dR_leading > Iso5Cut && dR_subleading > Iso5Cut )
      pho_cands.push_back(ipho);
  }
  return pho_cands.size() == 0;
}

bool monoJetDoubleEleCR::tau_veto(int leading,int subleading) {
  vector<int> tau_cands; tau_cands.clear();

  vector<int> tmpcands = getLooseTau();
  for (int itau : tmpcands ) {
    float dR_leading = deltaR(tau_Eta->at(itau),tau_Phi->at(itau),eleSCEta->at(leading),eleSCPhi->at(leading));
    float dR_subleading = deltaR(tau_Eta->at(itau),tau_Phi->at(itau),eleSCEta->at(subleading),eleSCPhi->at(subleading));
    if ( dR_leading > Iso4Cut && dR_subleading > Iso4Cut )
      tau_cands.push_back(itau);
  }
  return tau_cands.size() == 0;
}

#endif
