#ifndef monoJetCR_H
#define monoJetCR_H

#include "monoJetAnalysis.h"

class monoJetDoubleEleCR : public virtual monoJetAnalysis {
public:
  int leadLepIndx,subleadLepIndx;
  float dilepton_mass,dilepton_pt;
  TLorentzVector lep1,lep2;
  float leadingLepton_pt,leadingLepton_eta,leadingLepton_phi;
  float subleadingLepton_pt,subleadingLepton_eta,subleadingLepton_phi;
  float e1reco_sf,e2reco_sf,looseID_sf,tightID_sf;
  TH1F *h_leadingLeptonPt[maxHisto], *h_leadingLeptonEta[maxHisto],*h_leadingLeptonPhi[maxHisto],*h_subleadingLeptonPt[maxHisto],*h_subleadingLeptonEta[maxHisto], *h_subleadingLeptonPhi[maxHisto],*h_dileptonPt[maxHisto],*h_dileptonM[maxHisto],*h_dileptonMall[maxHisto];
  
  virtual void BookHistos(int i,std::string histname);
  virtual void fillHistos(int nhist,float event_weight);
  virtual void initVars();
  virtual void initTree(TTree* tree);
  bool CRSelection(std::vector<int> tight,std::vector<int> loose);
  void setRecoil(int leading,int subleading);
  float getSF(int leading,int subleading);
  virtual int getJetCand(int leading,int subleading);
  virtual vector<int> jet_veto(int leading,int subleading);
  virtual bool muon_veto();
  virtual bool photon_veto(int leading,int subleading);
  virtual bool tau_veto(int leading,int subleading);
  virtual bool bjet_veto(int leading,int subleading, float cutValue);
};

#endif
