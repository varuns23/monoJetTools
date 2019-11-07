#ifndef SHAPEUNC_H
#define SHAPEUNC_H

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

class ShapeUncCollection {
private:
  TDirectory* dir;
  struct ShapeUnc {
    TTree* treeUp;
    TTree* treeDn;
    string name;
    ShapeUnc(string name,TTree* treeUp,TTree* treeDn) {
      this->name = name;
      this->treeUp = treeUp;
      this->treeDn = treeDn;
    }
    void fillUp() { treeUp->Fill(); }
    void fillDn() { treeDn->Fill(); }
  };
  map<string,ShapeUnc*> uncmap;
public:
  ShapeUncCollection(TDirectory* dir) {
    this->dir = dir;
  }
  void addUnc(string name) {
    dir->cd();
    TTree* treeUp = new TTree( (name+"Up").c_str(), (name+"Up").c_str());
    TTree* treeDn = new TTree( (name+"Down").c_str(), (name+"Down").c_str());
    uncmap[name] = new ShapeUnc(name,treeUp,treeDn);
  }
  TTree* getTreeUp(string name) { return uncmap[name]->treeUp; }
  TTree* getTreeDn(string name) { return uncmap[name]->treeDn; }
  void fillUp(string name) { uncmap[name]->fillUp(); }
  void fillDn(string name) { uncmap[name]->fillDn(); }
  bool contains(string name) { return uncmap.find(name) != uncmap.end(); }
};

#endif
