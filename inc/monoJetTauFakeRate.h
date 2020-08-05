#ifndef monoJet_TFR_H
#define monoJet_TFR_H

#include "monoJetAnalysis.h"

class monoJetTauFakeRate : public virtual monoJetAnalysis {
public:
  static const Region REGION = TFR;
  static const CRobject CROBJECT = MuTau;

  int tauindex,muindex;
  TLorentzVector tau,muon;
  float dilepton_mass,dilepton_pt;
  float tau_pt,tau_eta,tau_phi;
  int tau_isoChMult,tau_isoNhMult,tau_isoPhMult,tau_isoPFCands;
  int tau_sigChMult,tau_sigNhMult,tau_sigPhMult,tau_sigPFCands;
  float muon_pt,muon_eta,muon_phi;
  float tightID_sf,tightISO_sf,looseID_sf,looseISO_sf;
  int iso_category;

  
  vector<float> tau_isoConsEt;
  vector<float>tau_isoConsPt;
  vector<float>tau_isoConsEta;
  vector<float>tau_isoConsPhi;
  vector<int>tau_isoConsPID;
  float tau_isoChNemPtFrac,tau_isoChNemPt,tau_isoChNemPt123;
  float tau_isoEtaWidth,tau_isoPhiWidth;
  
  vector<float> tau_sigConsEt;
  vector<float>tau_sigConsPt;
  vector<float>tau_sigConsEta;
  vector<float>tau_sigConsPhi;
  vector<int>tau_sigConsPID;
  float tau_sigChNemPtFrac,tau_sigChNemPt,tau_sigChNemPt123;
  float tau_sigEtaWidth,tau_sigPhiWidth;
  
  
  TH1F *h_tauPt[maxHisto], *h_tauEta[maxHisto],*h_tauPhi[maxHisto],*h_muonPt[maxHisto],*h_muonEta[maxHisto], *h_muonPhi[maxHisto],*h_dileptonPt[maxHisto],*h_dileptonM[maxHisto],*h_dileptonMall[maxHisto],*h_isoCategory[maxHisto];
  TH1F *h_tauIsoChMult[maxHisto],*h_tauIsoNhMult[maxHisto],*h_tauIsoPhMult[maxHisto],*h_tauIsoPFCands[maxHisto];
  TH1F *h_tauSigChMult[maxHisto],*h_tauSigNhMult[maxHisto],*h_tauSigPhMult[maxHisto],*h_tauSigPFCands[maxHisto];

  TH1F *h_tauIsoChNemPtFrac[maxHisto],*h_tauIsoChNemPtFracV2[maxHisto],*h_tauIsoChNemPt[maxHisto],*h_tauIsoChNemPt123[maxHisto];
  TH1F *h_tauSigChNemPtFrac[maxHisto],*h_tauSigChNemPtFracV2[maxHisto],*h_tauSigChNemPt[maxHisto],*h_tauSigChNemPt123[maxHisto];

  TH1F *h_tauIsoEtaWidth[maxHisto],*h_tauIsoPhiWidth[maxHisto],*h_tauSigEtaWidth[maxHisto],*h_tauSigPhiWidth[maxHisto];

  TH1F *h_pfMETlow[maxHisto];
  
  TH2F *h_tauEtaPhi[maxHisto],*h_muonEtaPhi[maxHisto];
  
  virtual void BookHistos(int i,TString histname);
  virtual void fillHistos(int nhist,float event_weight);
  virtual void initVars();
  virtual void initTree(TTree* tree);
  virtual void Init(TTree* tree);
  bool CRSelection(std::vector<int> taulist,std::vector<int> mulist);
  void setRecoil();
  float getSF(int tauindex,int muindex);
  bool isMedium(int tauindex);
  virtual bool electron_veto();
  virtual bool photon_veto(int tauindex,int muindex);
  virtual void SetTauIsoPtFrac(int tauindex);
  virtual void SetTauSigPtFrac(int tauindex);
  
  vector<float>   *tau_IsolationEtaWidth;
  vector<float>   *tau_IsolationPhiWidth;
  vector<vector<float> > *tau_IsolationConstPt;
  vector<vector<float> > *tau_IsolationConstEt;
  vector<vector<float> > *tau_IsolationConstEta;
  vector<vector<float> > *tau_IsolationConstPhi;
  vector<vector<int> > *tau_IsolationConstPdgId;
  vector<float>   *tau_SignalEtaWidth;
  vector<float>   *tau_SignalPhiWidth;
  vector<vector<float> > *tau_SignalConstPt;
  vector<vector<float> > *tau_SignalConstEt;
  vector<vector<float> > *tau_SignalConstEta;
  vector<vector<float> > *tau_SignalConstPhi;
  vector<vector<int> > *tau_SignalConstPdgId;
  
  TBranch        *b_tau_IsolationEtaWidth;   //!
  TBranch        *b_tau_IsolationPhiWidth;   //!
  TBranch        *b_tau_IsolationConstPt;   //!
  TBranch        *b_tau_IsolationConstEt;   //!
  TBranch        *b_tau_IsolationConstEta;   //!
  TBranch        *b_tau_IsolationConstPhi;   //!
  TBranch        *b_tau_IsolationConstPdgId;   //!
  TBranch        *b_tau_SignalEtaWidth;   //!
  TBranch        *b_tau_SignalPhiWidth;   //!
  TBranch        *b_tau_SignalConstPt;   //!
  TBranch        *b_tau_SignalConstEt;   //!
  TBranch        *b_tau_SignalConstEta;   //!
  TBranch        *b_tau_SignalConstPhi;   //!
  TBranch        *b_tau_SignalConstPdgId;   //!
};

#endif
