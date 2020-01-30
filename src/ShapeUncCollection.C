#ifndef ShapeUncCollection_C
#define ShapeUncCollection_C

#include "ShapeUncCollection.h"

using namespace std;

void ShapeUncCollection::setDir(TDirectory* dir) { this->dir = dir; }
void ShapeUncCollection::addUnc(string name) {
  dir->cd();
  TTree* treeUp = new TTree( (name+"Up").c_str(), (name+"Up").c_str());
  TTree* treeDn = new TTree( (name+"Down").c_str(), (name+"Down").c_str());
  uncmap[name] = new ShapeUnc(name,treeUp,treeDn);
}
TTree* ShapeUncCollection::getTreeUp(string name) { return uncmap[name]->treeUp; }
TTree* ShapeUncCollection::getTreeDn(string name) { return uncmap[name]->treeDn; }
void ShapeUncCollection::fillUp(string name) { uncmap[name]->fillUp(); }
void ShapeUncCollection::fillDn(string name) { uncmap[name]->fillDn(); }
bool ShapeUncCollection::contains(string name) { return uncmap.find(name) != uncmap.end(); }

#endif
