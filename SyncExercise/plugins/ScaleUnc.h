#ifndef SCALEUNC_H
#define SCALEUNC_H

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <iostream>
#include <functional>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <map>
#include <list>
#include <vector>
#include <bitset>
#include <TCanvas.h>
#include <TSystem.h>
#include <TPostScript.h>
#include <TH2.h>
#include <TH1.h>
#include <TF1.h>
#include <TMath.h>
#include <TLegend.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TRef.h>
#include <TList.h>
#include <TSystemFile.h>
#include <TSystemDirectory.h>
#include <TDCacheFile.h>
#include <TLorentzVector.h>
#include "TIterator.h"
#include "string"
// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "TString.h"
#include "vector"

using namespace std;

class ScaleUncCollection {
private:
  TTree* tree;
  struct ScaleUnc {
    string name;
    TH1F* histo;
    float up;
    float dn;
    ScaleUnc(string name,TH1F* histo) {
      this->name = name;
      this->histo = histo;
      up = dn = 0;
    }
    void setUnc(float up,float dn) {
      this->up = up;
      this->dn = dn;
    }
    float getBin(float x) { return histo->GetBinContent( histo->GetXaxis()->FindBin(x) ); }
  };
  map<string,ScaleUnc*> uncmap;
public:
  ScaleUncCollection(TTree* tree) {
    this->tree = tree;
  }
  void addUnc(string name,TH1F* histo) {
    uncmap[name] = new ScaleUnc(name,histo);
    tree->Branch( (name+"Up").c_str(),&uncmap[name]->up, (name+"Up").c_str());
    tree->Branch( (name+"Down").c_str(),&uncmap[name]->dn, (name+"Down").c_str());
  }
  TH1F* getHisto(string name) { return uncmap[name]->histo; }
  float getBin(string name, float x) { return uncmap[name]->getBin(x); }
  void setUnc(string name,float up,float dn) { uncmap[name]->setUnc(up,dn); }
  bool contains(string name) { return uncmap.find(name) != uncmap.end(); }
};

#endif
