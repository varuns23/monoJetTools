#ifndef Utilities_C
#define Utilities_C

#include <TMath.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
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

vector<string> split(string str,string delim) {
  vector<string> splitString;
  char strChar[str.size() + 1];
  strcpy(strChar,str.c_str());
  char *token = strtok(strChar,delim.c_str());
  while (token != NULL) {
    splitString.push_back(string(token));
    token = strtok(NULL,delim.c_str());
  }
  return splitString;
}

bool fileSelection(string filename,string fileRange)
{
  if (fileRange == "-1") return true;
  int numPos;
  for (int i = filename.size(); i > 0; --i) {
    if (filename[i] == '_') {
      numPos = i+1;
      break;
    }
  }
  filename.erase(filename.begin(),filename.begin()+numPos);
  int fileNum = atoi(filename.c_str());
  //1-100/200-250/300-300
  vector<string> rangeOfFiles = split(fileRange,"/");
  for (unsigned int i = 0; i < rangeOfFiles.size(); i++) {
    vector<string> range = split(rangeOfFiles[i],"-");
    if (atoi(range[0].c_str()) <= fileNum && fileNum <= atoi(range[1].c_str())) {
      return true;
    }
  }
  return false;
}

TH1F* MakeTH1F(TH1F* temp) {
  temp->Sumw2();
  return temp;
}

EventMask::EventMask(std::string maskfile) {
  setMask(maskfile);
}
void EventMask::setMask(std::string maskfile) {
  mask.clear();
  std::string path = maskfile;
  cout << "Mask: " << path << endl;
  ifstream file(path.c_str());
  if ( !file.is_open() ) {
    cout << "Unable to read " << path << endl;
    return;
  }
  std::string line;
  int iline = 0;

  int run,lumis;
  Long64_t event;
  while ( file >> line ){
    switch(iline%3) {
    case 0:
      run = std::stoi(line);
      if ( !mask.count(run) ) mask.insert( {run,std::map<int,std::set<Long64_t>>()} );
      break;
    case 1:
      lumis = std::stoi(line);
      if ( !mask[run].count(lumis) ) mask[run].insert( {lumis,std::set<Long64_t>()} );
      break;
    case 2:
      event = std::stoll(line);
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
#endif
