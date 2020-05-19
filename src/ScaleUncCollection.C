#ifndef ScaleUncCollection_c
#define ScaleUncCollection_c

#include "ScaleUncCollection.h"

using namespace std;

void ScaleUncCollection::setTree(TTree* tree) { this->tree = tree; }

void ScaleUncCollection::addUnc(TString name,TH1F* histo) {
  uncmap[name] = new ScaleUnc(name,histo);
  tree->Branch( (name+"Up"),&uncmap[name]->up, (name+"Up"));
  tree->Branch( (name+"Down"),&uncmap[name]->dn, (name+"Down"));
}

TH1F* ScaleUncCollection::getHisto(TString name) { return uncmap[name]->histo; }

float ScaleUncCollection::getBin(TString name, float x) { return uncmap[name]->getBin(x); }

void ScaleUncCollection::setUnc(TString name,float up,float dn) { uncmap[name]->setUnc(up,dn); }

bool ScaleUncCollection::contains(TString name) { return uncmap.find(name) != uncmap.end(); }

#endif
