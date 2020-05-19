#ifndef ShapeUncCollection_h
#define ShapeUncCollection_h

#include <iostream>
#include <TDirectory.h>
#include <TTree.h>

class ShapeUncCollection {
 private:
  TDirectory* dir;
  struct ShapeUnc {
    TTree* treeUp;
    TTree* treeDn;
    TString name;
    ShapeUnc(TString name,TTree* treeUp,TTree* treeDn) {
      this->name = name;
      this->treeUp = treeUp;
      this->treeDn = treeDn;
    }
    void fillUp() { treeUp->Fill(); }
    void fillDn() { treeDn->Fill(); }
  };
  std::map<TString,ShapeUnc*> uncmap;
 public:
  void setDir(TDirectory* dir);
  void addUnc(TString name);
  TTree* getTreeUp(TString name);
  TTree* getTreeDn(TString name);
  void fillUp(TString name);
  void fillDn(TString name);
  bool contains(TString name);
};

#endif
