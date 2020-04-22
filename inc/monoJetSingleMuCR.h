#ifndef monoJet_WMCR_H
#define monoJet_WMCR_H

#include "monoJetAnalysis.h"

class monoJetSingleMuCR : public virtual monoJetAnalysis {
public:
  int lepindex;
  TLorentzVector lep;
  float lepton_pt,lepton_eta,lepton_phi,lepMET_mt;
  float tightID_sf,tightISO_sf;

  TH1F *h_lepMET_MTBefore;
  TH1F *h_LeptonPt[maxHisto], *h_LeptonEta[maxHisto],*h_LeptonPhi[maxHisto],*h_lepMET_MT[maxHisto];
  
  virtual void BookHistos(int i,std::string histname);
  virtual void fillHistos(int nhist,float event_weight);
  virtual void initVars();
  virtual void initTree(TTree* tree);
  virtual float getSF(int lepindex);
  bool CRSelection(std::vector<int> tight,std::vector<int> loose);
  void setRecoil();
  virtual bool electron_veto();
  virtual bool photon_veto(int lepindex);
  virtual bool tau_veto(int lepindex);
};

#endif
