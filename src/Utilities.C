#ifndef Utilities_C
#define Utilities_C

#include <TMath.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "TString.h"
#include <vector>

#include "Utilities.h"

using namespace std;

float deltaPhi(float phi1, float phi2) {
  float pi = TMath::Pi();
  float dphi = fabs(phi1-phi2);
  if(dphi>pi)
    dphi = 2.0*pi - dphi;
  return dphi;
}

float deltaR(float eta1, float phi1, float eta2, float phi2) {
  float deltaeta = abs(eta1 - eta2);
  float deltaphi = deltaPhi(phi1, phi2);
  float deltar = sqrt(deltaeta*deltaeta + deltaphi*deltaphi);
  return deltar;
}

float getMt(float pt1,float phi1,float pt2,float phi2) {
  return TMath::Sqrt( 2 * pt1 * pt2 * (1 - TMath::Cos(deltaPhi(phi1,phi2))) );
}

float sigmoid(float x,float a,float b,float c,float d) {
  return c + (d - c) / (1 + TMath::Exp(-a * (x - b)));
}

float exponential(float x,float a,float b,float c) {
  return a * TMath::Exp(-b * x) + c;
}

float getRounded(float x) {
  union { float f; uint32_t u; };
  f = x;
  u &= 0xfffe0000u; 
  return f;
}

vector<TString> split(TString str,TString delim) {
  vector<TString> splitTString;
  TString token;
  int idx = 0;
  while ( str.Tokenize(token,idx,delim) ) splitTString.push_back(token);
  return splitTString;
}

bool fileSelection(TString filename,TString fileRange)
{
  if (fileRange == "-1") return true;
  int numPos;
  for (int i = filename.Length(); i > 0; --i) {
    if (filename[i] == '_') {
      numPos = i+1;
      break;
    }
  }
  filename.Remove(0,numPos);
  int fileNum = filename.Atoi();
  //1-100/200-250/300-300
  vector<TString> rangeOfFiles = split(fileRange,"/");
  for (unsigned int i = 0; i < rangeOfFiles.size(); i++) {
    vector<TString> range = split(rangeOfFiles[i],"-");
    if (range[0].Atoi() <= fileNum && fileNum <= range[1].Atoi()) {
      return true;
    }
  }
  return false;
}

TH1F* MakeTH1F(TH1F* temp) {
  temp->Sumw2();
  return temp;
}

EventMask::EventMask(TString maskfile) {
  setMask(maskfile);
}
void EventMask::setMask(TString maskfile) {
  mask.clear();
  TString path = maskfile;
  cout << "Mask: " << path << endl;
  ifstream file(path);
  if ( !file.is_open() ) {
    cout << "Unable to read " << path << endl;
    return;
  }
  TString line;
  int iline = 0;

  int run,lumis;
  Long64_t event;
  while ( file >> line ){
    switch(iline%3) {
    case 0:
      run = line.Atoi();
      if ( !mask.count(run) ) mask.insert( {run,std::map<int,std::set<Long64_t>>()} );
      break;
    case 1:
      lumis = line.Atoi();
      if ( !mask[run].count(lumis) ) mask[run].insert( {lumis,std::set<Long64_t>()} );
      break;
    case 2:
      event = line.Atoll();
      mask[run][lumis].insert(event);
      break;
    }
    iline++;
  }
}
bool EventMask::contains(int run,int lumis,Long64_t event) {
  if ( !mask.count(run) ) return false;
  if ( !mask[run].count(lumis) ) return false;
  return mask[run][lumis].count(event) != 0;
}

BTagCSV::BTagSF::BTagSF(TString csvline) {
  vector<TString> values = split(csvline,",");
  op = values[0].Atoi();
  measurement = values[1].ReplaceAll(" ","");
  sys = values[2].ReplaceAll(" ","");
  jetFlavor = values[3].Atoi();
  etaMin = values[4].Atof();
  etaMax = values[5].Atof();
  ptMin = values[6].Atof();
  ptMax = values[7].Atof();
  discrMin = values[8].Atoi();
  discrMax = values[9].Atoi();
  TF1(GetName(),values[10].ReplaceAll(" ","").ReplaceAll("\"",""),ptMin,ptMax);
}

float BTagCSV::BTagSF::EvalSF(float pt,float eta) {
  if ( fabs(eta) < etaMin || fabs(eta) > etaMax ) return 1;
  return Eval(pt);
}

BTagCSV::BTagCSV(TString csvname) {
  ifstream csvfile(csvname);
  if ( !csvfile.is_open() ) {
    cout << "Unable to read " << csvname << endl;
    return;
  }
  std::string csvline;
  std::getline(csvfile,csvline);
  while ( std::getline(csvfile,csvline) ) {
    BTagSF btagsf(csvline);
    sfmap[btagsf.GetName()] = &btagsf;
  }
}

BTagCSV::BTagSF* BTagCSV::getBTagSF(int op, TString measurement, TString sys, int jetFlavor) {
  TString sfname = TString( std::to_string(op) )+"_"+measurement+"_"+sys+"_"+TString( std::to_string(jetFlavor) );
  return sfmap[sfname];
}
#endif
