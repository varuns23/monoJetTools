//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Mar  8 10:15:33 2017 by ROOT version 6.06/01
// from TTree EventTree/Event data (tag V08_00_24_00)
//////////////////////////////////////////////////////////

#ifndef monoJetClass_h
#define monoJetClass_h

#include "monoJetYear.h"
#include "monoJetGammaCR.h"

using namespace std;
class monoJetClass : public monoJetYear, public monoJetGammaCR {
public :
  static const int nHisto = 12;
  static const int bHisto = 11;

  float photon_phoiso;
  
  static const int nPhoPtBins = 8;                
  const float phoPtBins[nPhoPtBins+1] = {230., 250., 280., 320., 375., 425., 475., 550., 2000.};

  TH1F *h_phoPFIso[nHisto],*h_phoPt_ptbins[nHisto][nPhoPtBins],*h_phoPFIso_ptbins[nHisto][nPhoPtBins];
  TH2F *h_phoPFIsoSigmaIEtaIEta[nHisto];
 
  monoJetClass(const char* file1,const char* file2,int nfiles) : monoJetAnalysis(file1,file2,nfiles) {
    BookHistos(file2); };
  monoJetClass(const char* inputFilename,const char* outputFilename,const char* fileRange) : monoJetAnalysis(inputFilename,outputFilename,fileRange) {
    BookHistos(outputFilename);
  }
  monoJetClass(const char* inputFilename,const char* outputFilename,vector<const char*> filelist) : monoJetAnalysis(inputFilename,outputFilename,filelist) {
    BookHistos(outputFilename);
  }
  void     Loop(Long64_t maxEvents, int reportEvery);
  void BookHistos(const char* file2);
  void BookHistos(int i,string histname) {
    monoJetYear::BookHistos(i,histname);
    monoJetGammaCR::BookHistos(i,histname);
  }
  void fillHistos(int nhist,float event_weight=1.0);
  void initVars() {
    monoJetYear::initVars();
    monoJetGammaCR::initVars();
    photon_phoiso = -1;
  }
  void initTree(TTree* tree) {
    monoJetYear::initTree(tree);
    monoJetGammaCR::initTree(tree);
    tree->Branch("photonPt",&photon_pt,"Photon P_{T} (GeV)");
    tree->Branch("photonSigmaIEtaIEta",&photon_sieie,"Photon #sigma_{i#eta i#eta}");
    tree->Branch("phoPFIso",&photon_phoiso);
  }

  int getPhoIndex();
  int getJetIndex(int phoindex);

  bool CutBasedPFPhotonID_noPhoIso(Int_t ipho, TString phoWP);
  double EAcharged(double eta);
  double EAneutral(double eta);
  double EAphoton(double eta);
  inline double phoPFChIso_RhoCor(int ipho)  { return TMath::Max(((*phoPFChIso)[ipho]  - rho*EAcharged((*phoSCEta)[ipho])), 0.0); }
  inline double phoPFNeuIso_RhoCor(int ipho) { return TMath::Max(((*phoPFNeuIso)[ipho] - rho*EAneutral((*phoSCEta)[ipho])), 0.0); }
  inline double phoPFPhoIso_RhoCor(int ipho) { return TMath::Max(((*phoPFPhoIso)[ipho] - rho*EAphoton( (*phoSCEta)[ipho])), 0.0); }
};

#endif
