//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Mar  8 10:15:33 2017 by ROOT version 6.06/01
// from TTree EventTree/Event data (tag V08_00_24_00)
//////////////////////////////////////////////////////////

#ifndef monoJetClass_h
#define monoJetClass_h

#include "monoJetYear.h"
#include "monoJetGammaPurity.h"

using namespace std;
class monoJetClass : virtual public monoJetYear, virtual public monoJetGammaPurity {
public :
  static const int nHisto = 14;
  static const int bHisto = 11;
 
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
  void BookHistos(int i,TString histname) {
    monoJetYear::BookHistos(i,histname);
    monoJetGammaPurity::BookHistos(i,histname);
  }
  void fillHistos(int nhist,float event_weight);
  void initVars() {
    monoJetYear::initVars();
    monoJetGammaPurity::initVars();
  }
  void initTree(TTree* tree) {
    monoJetYear::initTree(tree);
    monoJetGammaPurity::initTree(tree);
  }

  void nominal(float);
  void met_variation(int,float);

  int getPhoIndex();
};

#endif
