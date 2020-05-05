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
  static const int nHisto = 1;
  static const int bHisto = 0;
 
  TH1F * h_CutFlow;
  TH1F * h_phoPt;
  TH1F * h_phoPFIso;
  TH1F * h_phoSigmaIeIe;
  TH2F * h_PhoPFiso_SigmaIeIe;

  TH1F * h_phoPt_ptbins[8];
  TH1F * h_phoPFIso_ptbins[8];
 
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
  void BookHistos();
  void fillHistos(int nhist,float event_weight=1.0);
  void fillHistos(float event_weight=1.0);
  void initVars() {
    monoJetYear::initVars();
    monoJetGammaCR::initVars();
  }
  void initTree(TTree* tree) {
    monoJetYear::initTree(tree);
    monoJetGammaCR::initTree(tree);
  }

  int getPhoIndex();
  int getJetIndex(int phoindex);

  bool CutBasedPFPhotonID_noPhoIso(Int_t ipho, TString phoWP);
  double EAcharged(double eta);
  double EAneutral(double eta);
  double EAphoton(double eta);
  
};

#endif
