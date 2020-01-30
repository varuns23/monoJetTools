#ifndef monoJetDoubleEleCR_C
#define monoJetDoubleEleCR_C

#include "monoJetDoubleEleCR.h"
#include "VariableBins.h"
#include "Utilities.h"

using namespace std;

const string monoJetDoubleEleCR::REGION = "DoubleEleCR";

void monoJetDoubleEleCR::initVars() {
  lepindex_leading = lepindex_subleading = -1;
  dilepton_mass = dilepton_pt = -1;
  leadingLepton_pt = leadingLepton_eta = leadingLepton_phi = -99;
  subleadingLepton_pt = subleadingLepton_eta = subleadingLepton_phi = -99;
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
}

void monoJetDoubleEleCR::BookHistos(int i,string histname) {
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

void monoJetDoubleEleCR::fillHistos(int nhist,float event_weight) {
  if(lepindex_leading >= 0 && lepindex_subleading >= 0){ 
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

bool monoJetDoubleEleCR::CRSelection(vector<int> tightlist,vector<int> looselist) {
  for (int leading : tightlist) {
    for (int subleading : looselist) {
      //Event must have exactly two loose electrons with opposite charge
      if (eleCharge->at(leading)*eleCharge->at(subleading) == -1) {
	lep1.SetPtEtaPhiE(elePt->at(leading),eleEta->at(leading),elePhi->at(leading),eleE->at(leading));
	lep2.SetPtEtaPhiE(elePt->at(subleading),eleEta->at(subleading),elePhi->at(subleading),eleE->at(subleading));
	lepindex_leading = leading;
	lepindex_subleading = subleading;
	TLorentzVector ll = lep1 + lep2;
	dilepton_mass = ll.M();
	dilepton_pt = ll.Pt();

	leadingLepton_pt = lep1.Pt();
	leadingLepton_eta = lep1.Eta();
	leadingLepton_phi = lep1.Phi();
	
	subleadingLepton_pt = lep2.Pt();
	subleadingLepton_eta = lep2.Eta();
	subleadingLepton_phi = lep2.Phi();
	
	TLorentzVector met_4vec;
	met_4vec.SetPtEtaPhiE(pfMET,0.,pfMETPhi,pfMET);
	TLorentzVector leptoMET_4vec = ll+met_4vec;
	recoil = fabs(leptoMET_4vec.Pt());
	recoilPhi = leptoMET_4vec.Phi();
	return true;
      }
    }
  }
  return false;
}

float monoJetDoubleEleCR::getSF(int leading, int subleading) {
  float leading_eta = eleSCEta->at(leading); float leading_pt = elePt->at(leading);
  float subleading_eta = eleSCEta->at(subleading); float subleading_pt = elePt->at(subleading);
  
  float leadingEleRecoSF_corr= th2fmap.getBin("eleRecoSF_highpt",leading_eta,leading_pt);
  float leadingEleEffSF_corr= th2fmap.getBin("eleIDSF_tight",leading_eta,leading_pt);
  float leadingEleTriggSF = th2fmap.getBin("eleTriggSF",fabs(leading_eta),leading_pt);
  
  float subleadingEleRecoSF_corr= th2fmap.getBin("eleRecoSF_highpt",subleading_eta,subleading_pt);
  float subleadingEleEffSF_corr= th2fmap.getBin("eleIDSF_loose",subleading_eta,subleading_pt);
  float subleadingEleTriggSF = th2fmap.getBin("eleTriggSF",fabs(subleading_eta),subleading_pt);
  
  return leadingEleRecoSF_corr*leadingEleEffSF_corr*leadingEleTriggSF*subleadingEleRecoSF_corr*subleadingEleEffSF_corr*subleadingEleTriggSF;
}

vector<int> monoJetDoubleEleCR::jet_veto(int leading, int subleading) {
  vector<int> jetindex; jetindex.clear();
			  
  vector<int> tmpcands = jet_looseID();
  for (int ijet : tmpcands) {
    float dR_leading = deltaR(jetEta->at(ijet),jetPhi->at(ijet),eleSCEta->at(leading),eleSCPhi->at(leading));
    float dR_subleading = deltaR(jetEta->at(ijet),jetPhi->at(ijet),eleSCEta->at(subleading),eleSCPhi->at(subleading));
    if ( dR_leading > Iso4Cut && dR_subleading > Iso4Cut )
      jetindex.push_back(ijet);
  }
  return jetindex;
}

bool monoJetDoubleEleCR::electron_veto(int jet_index,int leading,int subleading,float elePtCut) {
  return monoJetAnalysis::electron_veto(jet_index,elePtCut);
}

//Veto failed if a muon is found that passes Loose Muon ID, Loose Muon Isolation, and muPtcut, and does not overlap the candidate electrons and jet within dR of 0.5
bool monoJetDoubleEleCR::muon_veto(int jet_index, int leading, int subleading, float muPtCut) {
  vector<int> mu_cands;
  mu_cands.clear();
  
  vector<int> tmpcands = muon_looseID(jet_index,muPtCut);
  for(int imu : tmpcands) {
    float dR_leading = deltaR(muEta->at(imu),muPhi->at(imu),eleSCEta->at(leading),eleSCPhi->at(leading));
    float dR_subleading = deltaR(muEta->at(imu),muPhi->at(imu),eleSCEta->at(subleading),eleSCPhi->at(subleading));
    if ( dR_leading > Iso4Cut && dR_subleading > Iso4Cut)
      mu_cands.push_back(imu);
  }
  
  return mu_cands.size() == 0;
}

bool monoJetDoubleEleCR::photon_veto(int jet_index,int leading,int subleading,float phoPtCut) {
  vector<int> pho_cands; pho_cands.clear();

  vector<int> tmpcands = photon_looseID(jet_index,phoPtCut);
  for (int ipho : tmpcands ) {
    float dR_leading = deltaR(phoSCEta->at(ipho),phoSCPhi->at(ipho),eleSCEta->at(leading),eleSCPhi->at(leading));
    float dR_subleading = deltaR(phoSCEta->at(ipho),phoSCPhi->at(ipho),eleSCEta->at(subleading),eleSCPhi->at(subleading));
    if ( dR_leading > Iso5Cut && dR_subleading > Iso5Cut )
      pho_cands.push_back(ipho);
  }
  return pho_cands.size() == 0;
}

bool monoJetDoubleEleCR::tau_veto(int jet_index,int leading,int subleading,float tauPtCut) {
  vector<int> tau_cands; tau_cands.clear();

  vector<int> tmpcands = tau_looseID(jet_index,tauPtCut);
  for (int itau : tmpcands ) {
    float dR_leading = deltaR(tau_Eta->at(itau),tau_Phi->at(itau),eleSCEta->at(leading),eleSCPhi->at(leading));
    float dR_subleading = deltaR(tau_Eta->at(itau),tau_Phi->at(itau),eleSCEta->at(subleading),eleSCPhi->at(subleading));
    if ( dR_leading > Iso4Cut && dR_subleading > Iso4Cut )
      tau_cands.push_back(itau);
  }
  return tau_cands.size() == 0;
}

bool monoJetDoubleEleCR::bjet_veto(int leading,int subleading) {
  vector<int> bjet_cands; bjet_cands.clear();

  vector<int> tmpcands = bjet_looseID();
  for (int ijet : tmpcands) {
    float dR_leading = deltaR(jetEta->at(ijet),jetPhi->at(ijet),eleSCEta->at(leading),eleSCPhi->at(leading));
    float dR_subleading = deltaR(jetEta->at(ijet),jetPhi->at(ijet),eleSCEta->at(subleading),eleSCPhi->at(subleading));
    if ( dR_leading > Iso4Cut && dR_subleading > Iso4Cut )
      bjet_cands.push_back(ijet);
  }
  return bjet_cands.size() == 0;
}

#endif
