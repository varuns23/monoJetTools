//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Mar  8 10:15:33 2017 by ROOT version 6.06/01
// from TTree EventTree/Event data (tag V08_00_24_00)
// found on file: /hdfs/store/user/uhussain/Zprime_Ntuples_Mar7/WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/crab_W3Jets/170306_174919/0000/ggtree_mc_53.root
//////////////////////////////////////////////////////////

#ifndef monoJetClass_h
#define monoJetClass_h

#include "monoJetYear.h"
#include "monoJetSignalRegion.h"

using namespace std;
class monoJetClass : public monoJetYear, public monoJetSignalRegion {
public :
  static const int nHisto = 13;
  static const int bHisto = 12;

  float j1GenpT,j1GenEta,j1GenPhi,j1GenetaWidth;
  float GenPt123Fraction,GenChNemPtFrac;

  
  vector<float>   *jetGenPt;
  vector<float>   *jetGenE;
  vector<float>   *jetGenEta;
  vector<float>   *jetGenPhi;
  vector<float>   *jetGenetaWidth;
  vector<float>   *jetGenphiWidth;
  vector<vector<float> > *jetGenConstPt;
  vector<vector<float> > *jetGenConstEt;
  vector<vector<float> > *jetGenConstEta;
  vector<vector<float> > *jetGenConstPhi;
  vector<vector<int> > *jetGenConstPdgId;
  
  vector<float>   *jetGenJetPt;
  vector<float>   *jetGenJetE;
  vector<float>   *jetGenJetEta;
  vector<float>   *jetGenJetPhi;
  vector<float>   *jetGenJetetaWidth;
  vector<float>   *jetGenJetphiWidth;
  vector<vector<float> > *jetGenJetConstPt;
  vector<vector<float> > *jetGenJetConstEt;
  vector<vector<float> > *jetGenJetConstEta;
  vector<vector<float> > *jetGenJetConstPhi;
  vector<vector<int> > *jetGenJetConstPdgId;
  
  TBranch        *b_jetGenPt;   //!
  TBranch        *b_jetGenE;   //!
  TBranch        *b_jetGenEta;   //!
  TBranch        *b_jetGenPhi;   //!
  TBranch        *b_jetGenetaWidth;   //!
  TBranch        *b_jetGenphiWidth;   //!
  TBranch        *b_jetGenConstPt;   //!
  TBranch        *b_jetGenConstEt;   //!
  TBranch        *b_jetGenConstEta;   //!
  TBranch        *b_jetGenConstPhi;   //!
  TBranch        *b_jetGenConstPdgId;   //!
  
  TBranch        *b_jetGenJetPt;
  TBranch        *b_jetGenJetE;
  TBranch        *b_jetGenJetEta;
  TBranch        *b_jetGenJetPhi;
  TBranch        *b_jetGenJetetaWidth;
  TBranch        *b_jetGenJetphiWidth;
  TBranch        *b_jetGenJetConstPt;
  TBranch        *b_jetGenJetConstEt;
  TBranch        *b_jetGenJetConstEta;
  TBranch        *b_jetGenJetConstPhi;
  TBranch        *b_jetGenJetConstPdgId;
  
  monoJetClass(const char* file1,const char* file2,int nfiles) : monoJetAnalysis(file1,file2,nfiles) {
    BookHistos(file2);
    Init(fChain);
  };
  monoJetClass(const char* inputFilename,const char* outputFilename,const char* fileRange) : monoJetAnalysis(inputFilename,outputFilename,fileRange) {
    BookHistos(outputFilename);
    Init(fChain);
  }
  monoJetClass(const char* inputFilename,const char* outputFilename,vector<const char*> filelist) : monoJetAnalysis(inputFilename,outputFilename,filelist) {
    BookHistos(outputFilename);
    Init(fChain);
  }
  void     Loop(Long64_t maxEvents, int reportEvery);
  void BookHistos(const char* file2);
  void fillHistos(int nhist,float event_weight=1.0);
  virtual void setGenJetCand();
  void Init(TTree* tree);
  void initTree(TTree* tree);
};

#endif
