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
#include "monoJetSingleEleCR.h"
#include "monoJetSingleMuCR.h"

using namespace std;
class monoJetClass : public monoJetYear, public monoJetSignalRegion {
public :
  static const int nHisto = 12;
  static const int bHisto = 11;

  float eleveto_sf,muveto_sf,tauveto_sf;
  float eleveto_sfUp,muveto_sfUp,tauveto_sfUp;
  float eleveto_sfDown,muveto_sfDown,tauveto_sfDown;
  
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
  void initTree(TTree* tree) {
    monoJetYear::initTree(tree);
    tree->Branch("eleveto_sf",&eleveto_sf);
    tree->Branch("eleveto_sfUp",&eleveto_sfUp);
    tree->Branch("eleveto_sfDown",&eleveto_sfDown);
    tree->Branch("muveto_sf",&muveto_sf);
    tree->Branch("muveto_sfUp",&muveto_sfUp);
    tree->Branch("muveto_sfDown",&muveto_sfDown);
    tree->Branch("tauveto_sf",&tauveto_sf);
    tree->Branch("tauveto_sfUp",&tauveto_sfUp);
    tree->Branch("tauveto_sfDown",&tauveto_sfDown);
  }
  void initVars() {
    monoJetYear::initVars();
    eleveto_sf = muveto_sf = tauveto_sf = 1;
    eleveto_sfUp = muveto_sfUp = tauveto_sfUp = 1;
    eleveto_sfDown = muveto_sfDown = tauveto_sfDown = 1;
  }
};

#endif
