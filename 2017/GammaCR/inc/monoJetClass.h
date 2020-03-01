//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Mar  8 10:15:33 2017 by ROOT version 6.06/01
// from TTree EventTree/Event data (tag V08_00_24_00)
// found on file: /hdfs/store/user/uhussain/Zprime_Ntuples_Mar7/WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/crab_W3Jets/170306_174919/0000/ggtree_mc_53.root
//////////////////////////////////////////////////////////

#ifndef monoJetClass_h
#define monoJetClass_h

#include "monoJetYear.h"
#include "monoJetGammaCR.h"

using namespace std;
class monoJetClass : public monoJetYear, public monoJetGammaCR {
public :
  static const int nHisto = 14;
  static const int bHisto = 13;
  
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
  }
  void initTree(TTree* tree) {
    monoJetYear::initTree(tree);
    monoJetGammaCR::initTree(tree);
  }
  
  void JetEnergyScale(float start_weight);
  void JetEnergyResolution(float start_weight);
};

#endif
