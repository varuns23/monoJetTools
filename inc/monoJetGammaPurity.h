#ifndef monoJetGammaPurity_h
#define monoJetGammaPrutiy_h

#include "monoJetGammaCR.h"

class monoJetGammaPurity : public virtual monoJetGammaCR {
public:
  bool runIsoPurity = true;
  
  float photon_phoiso,photon_sieie;
  
  static const int nPhoPtBins = 8;                
  const float phoPtBins[nPhoPtBins+1] = {230., 250., 280., 320., 375., 425., 475., 550., 2000.};
  TString PtBinNames[nPhoPtBins];

  TH1F *h_phoPFIso[maxHisto],*h_phoSieie[maxHisto];
  TH1F *h_phoPt_ptbins[maxHisto][nPhoPtBins],*h_phoPFIso_ptbins[maxHisto][nPhoPtBins],*h_phoSieie_ptbins[maxHisto][nPhoPtBins];
  TH2F *h_phoPFIsoSieie[maxHisto];
  
  virtual void BookHistos(int i,TString histname);
  virtual void fillHistos(int nhist,float event_weight);
  virtual void initVars();
  virtual void initTree(TTree* tree);
  virtual void setPhoton(int ipho);
  virtual int getJetIndex(int ipho);

  bool CutBasedPhotonID(int ipho,TString phoWP);
  bool CutBasedPhotonID_noSieie(int ipho,TString phoWP);
  bool CutBasedPhotonID_invSieie(int ipho,TString phoWP);

  bool CutBasedPhotonIso(int ipho,TString phoWP);
  bool CutBasedPhotonIso_noPhoIso(int ipho,TString phoWP);
  
  double EAcharged(double eta);
  double EAneutral(double eta);
  double EAphoton(double eta);
  
  inline double phoPFChIso_RhoCor(int ipho)  { return TMath::Max(((*phoPFChIso)[ipho]  - rho*EAcharged((*phoSCEta)[ipho])), 0.0); }
  inline double phoPFNeuIso_RhoCor(int ipho) { return TMath::Max(((*phoPFNeuIso)[ipho] - rho*EAneutral((*phoSCEta)[ipho])), 0.0); }
  inline double phoPFPhoIso_RhoCor(int ipho) { return TMath::Max(((*phoPFPhoIso)[ipho] - rho*EAphoton( (*phoSCEta)[ipho])), 0.0); }
};

#endif
