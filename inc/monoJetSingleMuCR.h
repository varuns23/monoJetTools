#ifndef monoJetSingleMuCR_h
#define monoJetSingleMuCR_h

#include "monoJetAnalysis.h"

class monoJetSingleMuCR : public virtual monoJetAnalysis {
public:
  static const std::string REGION;
  int lepindex;
  TLorentzVector lep;
  float lepton_pt,lepton_eta,lepton_phi;

  TH1F *h_lepMET_MT;
  TH1F *h_LeptonPt[maxHisto], *h_LeptonEta[maxHisto],*h_LeptonPhi[maxHisto];
  
  virtual void BookHistos(int i,std::string histname);
  virtual void fillHistos(int nhist,float event_weight);
  virtual void initVars();
  virtual void initTree(TTree* tree);
  inline bool isW_or_ZJet() { return monoJetAnalysis::isW_or_ZJet() || sample.type == DYJets || sample.type == DYJets_NLO; }
  virtual float getSF(int lepindex);
  bool CRSelection(std::vector<int> tight,std::vector<int> loose);
  virtual vector<int> getJetCand(vector<int> jetlist, int leading_lep_index);
  virtual vector<int> getTightMu(vector<int> looseMu);
  virtual vector<int> pho_veto_looseID(int leading_mu_index);
  virtual vector<int> tau_veto(int leading_mu_index);
  virtual vector<int> bjet_veto(vector<int> jetlist, int leading_mu_index);
  virtual bool getMinDphiJR(vector<int> jetlist, int lead_lep_index, double lepMET_phi);
};

#endif
