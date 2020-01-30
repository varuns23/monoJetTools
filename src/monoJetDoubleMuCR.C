#ifndef monoJetDoubleMuCR_C
#define monoJetDoubleMuCR_C

#include "monoJetDoubleMuCR.h"
#include "VariableBins.h"
#include "monoJetCutConfig.h"
#include "Utilities.h"

using namespace std;

const string monoJetDoubleMuCR::REGION = "DoubleMuCR";

void monoJetDoubleMuCR::initVars() {
  leadLepIndx = subleadLepIndx = -1;
  dilepton_mass = dilepton_pt = -1;
  leadingLepton_pt = leadingLepton_eta = leadingLepton_phi = -99;
  subleadingLepton_pt = subleadingLepton_eta = subleadingLepton_phi = -99;
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
    h_leadingLeptonPt[nhist]    ->Fill(muPt->at(leadLepIndx),event_weight);
    h_leadingLeptonEta[nhist]   ->Fill(muEta->at(leadLepIndx),event_weight);
    h_leadingLeptonPhi[nhist]   ->Fill(muPhi->at(leadLepIndx),event_weight);
    h_subleadingLeptonPt[nhist] ->Fill(muPt->at(subleadLepIndx),event_weight);
    h_subleadingLeptonEta[nhist]->Fill(muEta->at(subleadLepIndx),event_weight);
    h_subleadingLeptonPhi[nhist]->Fill(muPhi->at(subleadLepIndx),event_weight);
    
    h_dileptonPt[nhist]         ->Fill(dilepton_pt,event_weight);
    h_dileptonM[nhist]          ->Fill(dilepton_mass,event_weight);
    h_dileptonMall[nhist]       ->Fill(dilepton_mass,event_weight);
  }
}

vector<int> monoJetDoubleMuCR::getTightMu(vector<int> looseMu){
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

bool monoJetDoubleMuCR::CRSelection(vector<int> tightlist,vector<int> looselist) {
  bool muPairSet = false;
  for(int j=0; j<looselist.size(); ++j){
    //Event must have exactly two muons with opposite charge
    if(muCharge->at(tightlist[0]) * muCharge->at(looselist[j]) == -1){

      lep1.SetPtEtaPhiE(muPt->at(tightlist[0]), muEta->at(tightlist[0]), muPhi->at(tightlist[0]), muE->at(tightlist[0]));
      lep2.SetPtEtaPhiE(muPt->at(looselist[j]), muEta->at(looselist[j]), muPhi->at(looselist[j]), muE->at(looselist[j]));
      leadLepIndx    = tightlist[0];
      subleadLepIndx = looselist[j];
      muPairSet = true;
    }
  }
  return muPairSet;
}


float monoJetDoubleMuCR::getSF(int leading,int subleading) {
  float leading_pt = muPt->at(leading); float leading_eta = muEta->at(leading); 
  float subleading_pt = muPt->at(subleading); float subleading_eta = muEta->at(subleading);
  
  float tightMuISO_SF_corr;
  float tightMuID_SF_corr; 
  float looseMuISO_SF_corr;
  float looseMuID_SF_corr;
  
  if ( th2fmap.contains("tightMuSF_ISO_abseta") ) {
    tightMuISO_SF_corr = th2fmap.getBin("tightMuSF_ISO_abseta",leading_pt,fabs(leading_eta));
    tightMuID_SF_corr = th2fmap.getBin("tightMuSF_ID_abseta",leading_pt,fabs(leading_eta));
    looseMuISO_SF_corr = th2fmap.getBin("looseMuSF_ISO_abseta",subleading_pt,fabs(subleading_eta));
    looseMuID_SF_corr = th2fmap.getBin("looseMuSF_ID_abseta",subleading_pt,fabs(subleading_eta));
  } else {
    tightMuISO_SF_corr = th2fmap.getBin("tightMuSF_ISO",leading_pt,leading_eta);
    tightMuID_SF_corr = th2fmap.getBin("tightMuSF_ID",leading_pt,leading_eta);
    looseMuISO_SF_corr = th2fmap.getBin("looseMuSF_ISO",subleading_pt,subleading_eta);
    looseMuID_SF_corr = th2fmap.getBin("looseMuSF_ID",subleading_pt,subleading_eta);
  }
  
  return tightMuISO_SF_corr*tightMuID_SF_corr*looseMuISO_SF_corr*looseMuID_SF_corr;
}

vector<int> monoJetDoubleMuCR::getJetCand(vector<int> jetlist, int lead_lepIndex, int sublead_lepIndex){
  vector<int> jet_cands;
  jet_cands.clear();

  for(int j; j<jetlist.size(); j++){
    int i = jetlist.at(j);

    bool kinematics = (jetPt->at(i) > 100.0 && fabs(jetEta->at(i)) < 2.4);
    bool Id = (jetNHF->at(i) < 0.8 && jetCHF->at(i) > 0.1); 

    float dR_lead_mu = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(lead_lepIndex),  muPhi->at(lead_lepIndex));
    float dR_sublead_mu = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(sublead_lepIndex), muPhi->at(sublead_lepIndex));

    if(kinematics && Id && dR_lead_mu > 0.4 && dR_sublead_mu > 0.4)
      jet_cands.push_back(i);
  }

  return jet_cands;
}

vector<int> monoJetDoubleMuCR::pho_veto_looseID(int leadLep_index, int subleadLep_index){
  vector<int> pho_cands;
  pho_cands.clear();

  for(int i = 0; i < nPho; i++){
    // passes pt cut
    bool kinematics = ((phoEt->at(i) > 15.0) && (fabs(phoSCEta->at(i)) < 2.5));
    bool IdIso = (phoIDbit->at(i)>>0&1==1);

    double dR_leadLep    = deltaR(phoSCEta->at(i),phoSCPhi->at(i), muEta->at(leadLep_index), muPhi->at(leadLep_index));  
    double dR_subleadLep = deltaR(phoSCEta->at(i),phoSCPhi->at(i), muEta->at(subleadLep_index), muPhi->at(subleadLep_index)); 
    if( kinematics && IdIso && dR_leadLep > 0.5 && dR_subleadLep > 0.5)
      pho_cands.push_back(i);
  }
  return pho_cands;
}

vector<int> monoJetDoubleMuCR::tau_veto(int leadLep_index, int subleadLep_index){
  vector<int> tau_cands;
  tau_cands.clear();

  for(int i = 0; i < nTau; i++){
    bool kinematics = ((tau_Pt->at(i) > 18.0) && (fabs(tau_Eta->at(i)) < 2.3));
    bool IdIso = (((tau_IDbits->at(i)>>0&1) == 1) && ((tau_IDbits->at(i)>>13&1) == 1));

    double dR_leadLep    = deltaR(tau_Eta->at(i), tau_Phi->at(i), muEta->at(leadLep_index), muPhi->at(leadLep_index));  
    double dR_subleadLep = deltaR(tau_Eta->at(i), tau_Phi->at(i), muEta->at(subleadLep_index), muPhi->at(subleadLep_index)); 
    if( kinematics && IdIso && dR_leadLep > 0.4 && dR_subleadLep > 0.4)
      tau_cands.push_back(i);
  }
  return tau_cands;
}

vector<int> monoJetDoubleMuCR::bjet_veto(vector<int> jetlist, int leadLep_index, int subleadLep_index){
  vector<int> bjet_cands;
  bjet_cands.clear();

  for(int j = 0; j < jetlist.size(); j++){
    int i = jetlist.at(j);
    bool kinematic = (jetPt->at(i) > 20.0 && fabs(jetEta->at(i)) < 2.4);
    bool btagged = ((jetDeepCSVTags_b->at(i) + jetDeepCSVTags_bb->at(i)) > 0.4941);

    double dR_leadLep    = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(leadLep_index), muPhi->at(leadLep_index));  
    double dR_subleadLep = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(subleadLep_index), muPhi->at(subleadLep_index)); 

    if(kinematic && btagged &&  dR_leadLep > 0.4 && dR_subleadLep > 0.4)
      bjet_cands.push_back(i);
  }
  return bjet_cands;
}


bool monoJetDoubleMuCR::getMinDphiJR(vector<int> jetlist, int lead_lepIndex, int sublead_lepIndex, double lepMET_phi){
  bool decision = false;

  vector<int> tmpJetlist;
  tmpJetlist.clear();

  for(int j=0; j<jetlist.size(); j++){ 

    int i = jetlist.at(j);

    bool kinematic = (jetPt->at(i) > 30. && fabs(jetEta->at(i)) < 2.4);

    double dR_leadLep    = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(lead_lepIndex), muPhi->at(lead_lepIndex));  
    double dR_subLeadLep = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(sublead_lepIndex), muPhi->at(sublead_lepIndex)); 

    if(kinematic && dR_leadLep > 0.4 && dR_subLeadLep > 0.4)
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
