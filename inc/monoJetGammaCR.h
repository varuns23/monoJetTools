#ifndef monoJetGammaCR_h
#define monoJetGammaCR_h

#include "monoJetAnalysis.h"

class monoJetGammaCR : public virtual monoJetAnalysis {
public:
  static const std::string REGION;
  int phoindex;
  TLorentzVector pho;
  float photon_pt,photon_eta,photon_phi,photon_sieie;

  TH1F *h_PhotonPt[maxHisto], *h_PhotonEta[maxHisto],*h_PhotonPhi[maxHisto],*h_PhotonSIeIe[maxHisto];
  
  virtual void BookHistos(int i,std::string histname);
  virtual void fillHistos(int nhist,float event_weight);
  virtual void initVars();
  virtual void initTree(TTree* tree);
  virtual float getSF(int phoindex);
  bool CRSelection(std::vector<int> tight,std::vector<int> loose);
  virtual vector<int> getJetCand(vector<int> jetlist, int phoindex);
  virtual vector<int> jet_veto(int phoindex);
  virtual bool muon_veto(int phoindex);
  virtual bool electron_veto(int phoindex);
  virtual bool tau_veto(int phoindex);
  virtual bool bjet_veto(int phoindex);
};

#endif
