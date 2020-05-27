#ifndef Utilities_h
#define Utilities_h

#include <TH1F.h>
#include <TString.h>
#include <TF1.h>
#include <iostream>
#include <string>
#include <set>
#include <map>

float deltaPhi(float phi1, float phi2);

float deltaR(float eta1, float phi1, float eta2, float phi2);

float getMt(float pt1,float phi1,float pt2,float phi2);

float sigmoid(float x,float a,float b,float c,float d);

float exponential(float x,float a,float b,float c);

float getRounded(float x);

std::vector<TString> split(TString str,TString delim);

bool fileSelection(TString filename,TString fileRange);

TH1F* MakeTH1F(TH1F* temp);

struct EventMask {
  std::map<int,std::map<int,std::set<Long64_t>>> mask;
  
  EventMask();
  EventMask(TString maskfile);
  void setMask(TString maskfile);
  bool contains(int run,int lumis,Long64_t event);
};

struct BTagCSV {
  struct BTagSF : public TF1 {
    int op;
    TString measurement;
    TString sys;
    int jetFlavor;
    float etaMin,etaMax,ptMin,ptMax,discrMin,discrMax;
    BTagSF(TString csvline);
    inline TString GetName() { return TString( std::to_string(op) )+"_"+measurement+"_"+sys+"_"+TString( std::to_string(jetFlavor) ); }
    float EvalSF(float pt, float eta);
  };
  std::map<TString,BTagSF*> sfmap;
  
  BTagCSV(TString csvname);
  BTagCSV::BTagSF* getBTagSF(int op=1, TString measurement="comb", TString sys="central", int jetFlavor=0);
  inline float EvalSF(int op, TString measurement, TString sys, int jetFlavor, float pt, float eta) {
    return getBTagSF(op,measurement,sys,jetFlavor)->EvalSF(pt,eta);
  }
};
#endif
