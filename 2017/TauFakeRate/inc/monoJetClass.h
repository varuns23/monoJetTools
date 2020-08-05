//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Mar  8 10:15:33 2017 by ROOT version 6.06/01
// from TTree EventTree/Event data (tag V08_00_24_00)
// found on file: /hdfs/store/user/uhussain/Zprime_Ntuples_Mar7/WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/crab_W3Jets/170306_174919/0000/ggtree_mc_53.root
//////////////////////////////////////////////////////////

#ifndef monoJetClass_h
#define monoJetClass_h

#include "monoJetYear.h"
#include "monoJetTauFakeRate.h"

using namespace std;
class monoJetClass : public monoJetYear, public monoJetTauFakeRate {
public :
  static const int nHisto = 12;
  static const int bHisto = 9;
  
  monoJetClass(const char* file1,const char* file2,int nfiles) : monoJetAnalysis(file1,file2,nfiles) {
    monoJetTauFakeRate::Init(fChain);
    BookHistos(file2); };
  monoJetClass(const char* inputFilename,const char* outputFilename,const char* fileRange) : monoJetAnalysis(inputFilename,outputFilename,fileRange) {
    monoJetTauFakeRate::Init(fChain);
    BookHistos(outputFilename);
  }
  monoJetClass(const char* inputFilename,const char* outputFilename,vector<const char*> filelist) : monoJetAnalysis(inputFilename,outputFilename,filelist) {
    monoJetTauFakeRate::Init(fChain);
    BookHistos(outputFilename);
  }
  void     Loop(Long64_t maxEvents, int reportEvery);
  void BookHistos(const char* file2);
  void BookHistos(int i,TString histname) {
    monoJetYear::BookHistos(i,histname);
    monoJetTauFakeRate::BookHistos(i,histname);
  }
  void initVars() {
    monoJetYear::initVars();
    monoJetTauFakeRate::initVars();
  }
  void initTree(TTree* tree) {
    monoJetYear::initTree(tree);
    monoJetTauFakeRate::initTree(tree);
  }
  void fillHistos(int nhist,float event_weight=1.0);
};

#endif
