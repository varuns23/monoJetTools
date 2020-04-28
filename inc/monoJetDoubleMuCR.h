#ifndef monoJetCR_H
#define monoJetCR_H

#include "monoJetAnalysis.h"

class monoJetDoubleMuCR : public virtual monoJetAnalysis {
public:
  static const Region REGION = ZM;
  static const CRobject CROBJECT = Muon;
  
  int leadLepIndx,subleadLepIndx;
  float dilepton_mass,dilepton_pt;
  TLorentzVector lep1,lep2;
  float leadingLepton_pt,leadingLepton_eta,leadingLepton_phi;
  float subleadingLepton_pt,subleadingLepton_eta,subleadingLepton_phi;
  float tightID_sf,tightISO_sf,looseID_sf,looseISO_sf;
  TH1F *h_leadingLeptonPt[maxHisto], *h_leadingLeptonEta[maxHisto],*h_leadingLeptonPhi[maxHisto],*h_subleadingLeptonPt[maxHisto],*h_subleadingLeptonEta[maxHisto], *h_subleadingLeptonPhi[maxHisto],*h_dileptonPt[maxHisto],*h_dileptonM[maxHisto],*h_dileptonMall[maxHisto];

  TH2F *h_leadingLeptonEtaPhi[maxHisto],*h_subleadingLeptonEtaPhi[maxHisto];
  
  virtual void BookHistos(int i,std::string histname);
  virtual void fillHistos(int nhist,float event_weight);
  virtual void initVars();
  virtual void initTree(TTree* tree);
  bool CRSelection(std::vector<int> tight,std::vector<int> loose);
  void setRecoil();
  float getSF(int leading,int subleading);
  virtual bool electron_veto();
  virtual bool photon_veto(int leading,int subleading);
  virtual bool tau_veto(int leading,int subleading);
};

#endif
