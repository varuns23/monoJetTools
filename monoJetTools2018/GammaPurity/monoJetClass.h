//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Oct 11 16:56:08 2018 by ROOT version 6.06/01
// from TTree eventTree/event tree for analysis
// found on file: /hdfs/store/user/varuns/NTuples/MC/MC2017_12Apr2018_94X/DYJets/DYJetsToLL_HT100To200/MC_DYJetsToLL_HT100-200_1.root
//////////////////////////////////////////////////////////

#ifndef monoJetClass_h
#define monoJetClass_h

#include "monoJetCommon.h"
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TSystemDirectory.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLorentzVector.h>
#include <TMath.h>

// Header file for the classes stored in the TTree if any.
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>

using namespace std;

class monoJetClass : monoJetCommon{
public :
  static const int nHisto = 1;
  static const int bHisto = 0;
 
  //CR variables
  int phoindex,jetindex;
  //CR histograms

  TH1F *h_photonPt,*h_phoSigmaIEtaIEta;

  monoJetClass(const char* inputFilename,const char* outputFilename,const char* fileRange): monoJetCommon(inputFilename,outputFilename,fileRange){ 
     BookHistos(outputFilename); 
  };
 
  virtual void Loop(Long64_t maxEvents,int reportEvery);
  virtual void BookHistos(const char* outputFilename);
  virtual void fillHistos(int nhist,float event_weight);
  virtual int getJetIndex();
  virtual int getPhoIndex();
  virtual void initTree(TTree* tree);
  
};

#endif
