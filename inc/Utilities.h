#ifndef Utilities_h
#define Utilities_h

#include <TH1F.h>
#include <iostream>
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
#endif
