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
  static const int nHisto = 5;
  static const int bHisto = 4;

  map<string,float> theorymap;
  
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
  void fillHistos(int nhist,float event_weight=1.0);
  void SetScalingHistos() {
    TFile* zwfile = new TFile("RootFiles/wz_unc.root");
    TFile* zgfile = new TFile("RootFiles/gz_unc.root");

    TList* zwlist = zwfile->GetListOfKeys();
    TList* zglist = zgfile->GetListOfKeys();

    for (auto key : *zwlist) {
      th1fmap[string(key->GetName())] = (TH1F*)zwfile->Get(key->GetName());
      theorymap[string(key->GetName())] = 0;
    }
    for (auto key : *zglist) {
      cout << key->GetName() << endl;
      th1fmap[string(key->GetName())] = (TH1F*)zgfile->Get(key->GetName());
      theorymap[string(key->GetName())] = 0;
    }
    for (auto key : theorymap) {
      cout << key.first << endl;
      tree->Branch(key.first.c_str(),&theorymap[key.first]);
    }
  }
};

#endif
