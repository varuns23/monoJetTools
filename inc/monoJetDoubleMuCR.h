#ifndef monoJetDoubleMuCR_h
#define monoJetDoubleMuCR_h

#include "monoJetAnalysis.h"

class monoJetDoubleMuCR : public virtual monoJetAnalysis {
public:
  static const std::string REGION;
  
  int leadLepIndx,subleadLepIndx;
  float dilepton_mass,dilepton_pt;
  TLorentzVector lep1,lep2;
  float leadingLepton_pt,leadingLepton_eta,leadingLepton_phi;
  float subleadingLepton_pt,subleadingLepton_eta,subleadingLepton_phi;
  TH1F *h_leadingLeptonPt[maxHisto], *h_leadingLeptonEta[maxHisto],*h_leadingLeptonPhi[maxHisto],*h_subleadingLeptonPt[maxHisto],*h_subleadingLeptonEta[maxHisto], *h_subleadingLeptonPhi[maxHisto],*h_dileptonPt[maxHisto],*h_dileptonM[maxHisto],*h_dileptonMall[maxHisto];
  
  virtual void BookHistos(int i,std::string histname);
  virtual void fillHistos(int nhist,float event_weight);
  virtual void initVars();
  virtual void initTree(TTree* tree);
  inline bool isW_or_ZJet() { return monoJetAnalysis::isW_or_ZJet() || sample.type == DYJets || sample.type == DYJets_NLO; }
  bool getMinDphiJR(vector<int> jetlist, int lead_lepIndex, int sublead_lepIndex, double lepMET_phi);
  virtual vector<int> getJetCand(vector<int> jetlist,int leading,int subleading);
  vector<int> getTightMu(vector<int> looselist);
  vector<int> pho_veto_looseID(int leading,int subleading);
  vector<int> tau_veto(int leading,int subleading);
  vector<int> bjet_veto(vector<int> jetlist,int leading,int subleading);
  bool CRSelection(std::vector<int> tight,std::vector<int> loose);
  float getSF(int leading,int subleading);
};

#endif
