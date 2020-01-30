#ifndef monoJetGammaCR_C
#define monoJetGammaCR_C

#include "monoJetGammaCR.h"
#include "VariableBins.h"
#include "Utilities.h"

using namespace std;

const string monoJetGammaCR::REGION = "GammaCR";

void monoJetGammaCR::initVars() {
  phoindex = photon_pt = photon_eta = photon_phi = photon_sieie = -99;
}

void monoJetGammaCR::initTree(TTree* tree) {
  tree->Branch("photonPt",&photon_pt,"Photon P_{T} (GeV)");
  tree->Branch("photonEta",&photon_eta,"Photon Eta");
  tree->Branch("photonPhi",&photon_phi,"PhotonPhi");
  tree->Branch("photonSigmaIEtaIEta",&photon_sieie,"Photon #sigma_{i#eta i#eta}");
}

void monoJetGammaCR::BookHistos(int i,string histname) {
  if (i == -1) {
    
  } else {
    auto Name = [histname](string name) { return (name+histname); };
    h_PhotonPt[i]  = MakeTH1F(new TH1F(Name("photonPt").c_str() ,"PhotonPt;Photon P_{T}" ,nLeadingLeptonPtBins,LeadingLeptonPtBins));
    h_PhotonEta[i]   = MakeTH1F(new TH1F(Name("photonEta").c_str(),"PhotonEta;Photon #eta" ,nEtaBins,lEta,uEta));
    h_PhotonPhi[i]   = MakeTH1F(new TH1F(Name("photonPhi").c_str(),"PhotonPhi;Photon #phi" ,nPhiBins,lPhi,uPhi));
    h_PhotonSIeIe[i] = MakeTH1F(new TH1F(Name("photonSigmaIEtaIEta").c_str(),"PhotonSigmaIEtaIEta;Photon #sigma_{i#eta i#eta}",20,0,0.05));
  }
}

void monoJetGammaCR::fillHistos(int nhist,float event_weight) {
  //CR Histograms
  if(phoindex >= 0){
    h_PhotonPt[nhist] ->Fill(photon_pt,event_weight);
    h_PhotonEta[nhist]->Fill(photon_eta,event_weight);
    h_PhotonPhi[nhist]->Fill(photon_phi,event_weight);
    h_PhotonSIeIe[nhist]->Fill(photon_sieie,event_weight);
  }
}

bool monoJetGammaCR::CRSelection(vector<int> tight,vector<int> loose) {
  if (tight.size() == 1 && loose.size() == 1) {
    phoindex = tight[0];
    pho.SetPtEtaPhiE(phoEt->at(phoindex),phoEta->at(phoindex),phoPhi->at(phoindex),phoE->at(phoindex));
    
    photon_pt = pho.Pt();
    photon_eta = phoEta->at(phoindex);
    photon_phi = phoPhi->at(phoindex);
    photon_sieie = phoSigmaIEtaIEtaFull5x5->at(phoindex);
    TLorentzVector met_4vec;
    met_4vec.SetPtEtaPhiE(pfMET,0.,pfMETPhi,pfMET);
    TLorentzVector photoMET_4vec = pho+met_4vec;
    recoil = fabs(photoMET_4vec.Pt());
    recoilPhi = photoMET_4vec.Phi();
    return true;
  }
  return false;
}

float monoJetGammaCR::getSF(int phoindex) {
  float eta = phoSCEta->at(phoindex); float pt = phoEt->at(phoindex);
  float phoRecoSF_corr= th2fmap.getBin("phoRecoSF_highpt",eta,pt);
  // std::cout<<"phoRecoSF_corr =  "<< phoRecoSF_corr<<std::endl;
  float phoEffSF_corr= th2fmap.getBin("phoIDSF_tight",eta,pt);
  // std::cout<<"phoEffSF_corr =  "<< phoEffSF_corr<<std::endl;
  float phoTriggSF = th2fmap.getBin("phoTriggSF",fabs(eta),pt);

  return phoRecoSF_corr * phoEffSF_corr * phoTriggSF;
}

vector<int> monoJetGammaCR::getJetCand(vector<int> jetlist,int phoindex) {
  vector<int> jet_cand; jet_cand.clear();

  vector<int> tmpcands = monoJetAnalysis::getJetCand(jetlist);
  for (int ijet : tmpcands) {
    float dR_jet = deltaR(jetEta->at(ijet),jetPhi->at(ijet),phoSCEta->at(phoindex),phoSCPhi->at(phoindex));
    if (dR_jet > Iso4Cut)
      jet_cand.push_back(ijet);
  }
  return jet_cand;
}

vector<int> monoJetGammaCR::jet_veto(int phoindex) {
  vector<int> jetindex; jetindex.clear();

  vector<int> tmpcands = getLooseJet();
  for(int ijet : tmpcands ) {
    float dR_jet = deltaR(jetEta->at(ijet),jetPhi->at(ijet),phoSCEta->at(phoindex),phoSCPhi->at(phoindex));
    if( dR_jet > Iso4Cut )
      jetindex.push_back(ijet);
  }
  return jetindex;
}

//Veto failed if a muon is found that passes Loose Muon ID, Loose Muon Isolation, and muPtcut, and does not overlap the candidate electron and jet within dR of 0.5
bool monoJetGammaCR::muon_veto(int phoindex)
{
  // cout << "Inside Muon Loose Veto" << endl;
  vector<int> mu_cands;
  mu_cands.clear();

  vector<int> tmpcands = getLooseMu();
  for(int imu : tmpcands) {
    float dR_mu = deltaR(muEta->at(imu),muPhi->at(imu),phoSCEta->at(phoindex),phoSCPhi->at(phoindex));
    if ( dR_mu > Iso4Cut )
      mu_cands.push_back(imu);
  }
  
  return mu_cands.size() == 0;
}

bool monoJetGammaCR::electron_veto(int phoindex) {
  vector<int> ele_cands; ele_cands.clear();

  vector<int> tmpcands = getLooseEle();
  for (int iele : tmpcands ) {
    float dR_ele = deltaR(eleSCEta->at(iele),eleSCPhi->at(iele),phoSCEta->at(phoindex),phoSCPhi->at(phoindex));
    if ( dR_ele > Iso5Cut )
      ele_cands.push_back(iele);
  }
  return ele_cands.size() == 0;
}

bool monoJetGammaCR::tau_veto(int phoindex) {
  vector<int> tau_cands; tau_cands.clear();

  vector<int> tmpcands = getLooseTau();
  for (int itau : tmpcands ) {
    float dR_pho = deltaR(tau_Eta->at(itau),tau_Phi->at(itau),phoSCEta->at(phoindex),phoSCPhi->at(phoindex));
    if ( dR_pho > Iso4Cut )
      tau_cands.push_back(itau);
  }
  return tau_cands.size() == 0;
}

bool monoJetGammaCR::bjet_veto(int phoindex) {
  vector<int> bjet_cands; bjet_cands.clear();

  vector<int> tmpcands = getLooseBJet();
  for (int ijet : tmpcands) {
    float dR_pho = deltaR(jetEta->at(ijet),jetPhi->at(ijet),phoSCEta->at(phoindex),phoSCPhi->at(phoindex));
    if ( dR_pho > Iso4Cut )
      bjet_cands.push_back(ijet);
  }
  return bjet_cands.size() == 0;
}
#endif
