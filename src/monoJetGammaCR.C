#ifndef monoJetCR_C
#define monoJetCR_C

#include "monoJetGammaCR.h"
#include "VariableBins.h"
#include "Utilities.h"

using namespace std;

Region monoJetAnalysis::REGION = GA;
CRobject monoJetAnalysis::CROBJECT = Photon;

void monoJetGammaCR::initVars() {
  phoindex = photon_pt = photon_eta = photon_phi = photon_sieie = -99;
  tightID_sf = csev_sf = 1;
}

void monoJetGammaCR::initTree(TTree* tree) {
  tree->Branch("photonPt",&photon_pt,"Photon P_{T} (GeV)");
  tree->Branch("photonEta",&photon_eta,"Photon Eta");
  tree->Branch("photonPhi",&photon_phi,"PhotonPhi");
  tree->Branch("photonSigmaIEtaIEta",&photon_sieie,"Photon #sigma_{i#eta i#eta}");
  tree->Branch("tightID_sf",&tightID_sf);
  tree->Branch("csev_sf",&csev_sf);
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
    pho.SetPtEtaPhiE(phoCalibEt->at(phoindex),phoEta->at(phoindex),phoPhi->at(phoindex),phoCalibE->at(phoindex));
    
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
  float eta = phoSCEta->at(phoindex); float pt = phoCalibEt->at(phoindex);
  tightID_sf = th2fmap.getBin("photon_id_tight",eta,pt);
  if ( YEAR == 2017 ) {
    float r9 = phoR9Full5x5->at(phoindex);
    // From bucoffea
    // csev_index = 0.5 * photons.barrel + 3.5 * ~photons.barrel + 1 * (photons.r9 > 0.94) + 2 * (photons.r9 <= 0.94)
    int csev_index = 0.5 + 1 * ( r9 > 0.94 ? 1 : 0 ) + 2 * ( r9 <= 0.94 ? 1 : 0 );
    csev_sf = th1fmap.getBin("photon_csev",csev_index);
  } else if (YEAR == 2018) {
    csev_sf = th2fmap.getBin("photon_csev",pt,fabs(eta));
  }
  return tightID_sf * csev_sf;
}

vector<int> monoJetGammaCR::getPho() {
  vector<int> elelist = getLooseEle();
  vector<int> pholist; pholist.clear();
  for (int ipho = 0; ipho < nPho; ipho++) {
    bool pass = true;
    for (int iele : elelist) {
      float dr = deltaR(eleEta->at(iele),elePhi->at(iele),phoEta->at(ipho),phoPhi->at(ipho));
      if ( dr <= Iso5Cut ) {
	pass = false;
	break;
      }
    }
    if (pass) pholist.push_back(ipho);
  }
  return pholist;
}

//Veto failed if a muon is found that passes Loose Muon ID, Loose Muon Isolation, and muPtcut, and does not overlap the candidate electron and jet within dR of 0.5
bool monoJetGammaCR::muon_veto(int phoindex)
{
  // cout << "Inside Muon Loose Veto" << endl;
  vector<int> mu_cands;
  mu_cands.clear();

  vector<int> tmpcands = getLooseMu();
  for(int imu : tmpcands) {
    double dR_mu = deltaR(muEta->at(imu),muPhi->at(imu),phoSCEta->at(phoindex),phoSCPhi->at(phoindex));
    if ( dR_mu > Iso4Cut )
      mu_cands.push_back(imu);
  }
  
  return mu_cands.size() == 0;
}

bool monoJetGammaCR::electron_veto(int phoindex) {
  vector<int> ele_cands; ele_cands.clear();

  vector<int> tmpcands = getLooseEle();
  for (int iele : tmpcands ) {
    double dR_ele = deltaR(eleSCEta->at(iele),eleSCPhi->at(iele),phoSCEta->at(phoindex),phoSCPhi->at(phoindex));
    if ( dR_ele > Iso5Cut )
      ele_cands.push_back(iele);
  }
  return ele_cands.size() == 0;
}

bool monoJetGammaCR::tau_veto(int phoindex) {
  vector<int> tau_cands; tau_cands.clear();

  vector<int> tmpcands = getLooseTau();
  for (int itau : tmpcands ) {
    double dR_pho = deltaR(tau_Eta->at(itau),tau_Phi->at(itau),phoSCEta->at(phoindex),phoSCPhi->at(phoindex));
    if ( dR_pho > Iso4Cut )
      tau_cands.push_back(itau);
  }
  return tau_cands.size() == 0;
}
#endif
