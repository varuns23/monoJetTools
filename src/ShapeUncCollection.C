#ifndef ShapeUncCollection_C
#define ShapeUncCollection_C

#include "ShapeUncCollection.h"

using namespace std;

void ShapeUncCollection::setDir(TDirectory* dir) { this->dir = dir; }
void ShapeUncCollection::addUnc(TString name) {
  dir->cd();
  TTree* treeUp = new TTree( (name+"Up"), (name+"Up"));
  TTree* treeDn = new TTree( (name+"Down"), (name+"Down"));
  uncmap[name] = new ShapeUnc(name,treeUp,treeDn);
}
TTree* ShapeUncCollection::getTreeUp(TString name) { return uncmap[name]->treeUp; }
TTree* ShapeUncCollection::getTreeDn(TString name) { return uncmap[name]->treeDn; }
void ShapeUncCollection::fillUp(TString name) { uncmap[name]->fillUp(); }
void ShapeUncCollection::fillDn(TString name) { uncmap[name]->fillDn(); }
bool ShapeUncCollection::contains(TString name) { return uncmap.find(name) != uncmap.end(); }

#endif
