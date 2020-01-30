#ifndef ScaleUncCollection_c
#define ScaleUncCollection_c

#include "ScaleUncCollection.h"

using namespace std;

void ScaleUncCollection::setTree(TTree* tree) { this->tree = tree; }

void ScaleUncCollection::addUnc(string name,TH1F* histo) {
  uncmap[name] = new ScaleUnc(name,histo);
  tree->Branch( (name+"Up").c_str(),&uncmap[name]->up, (name+"Up").c_str());
  tree->Branch( (name+"Down").c_str(),&uncmap[name]->dn, (name+"Down").c_str());
}

TH1F* ScaleUncCollection::getHisto(string name) { return uncmap[name]->histo; }

float ScaleUncCollection::getBin(string name, float x) { return uncmap[name]->getBin(x); }

void ScaleUncCollection::setUnc(string name,float up,float dn) { uncmap[name]->setUnc(up,dn); }

bool ScaleUncCollection::contains(string name) { return uncmap.find(name) != uncmap.end(); }

#endif
