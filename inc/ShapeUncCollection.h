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
    std::string name;
    ShapeUnc(std::string name,TTree* treeUp,TTree* treeDn) {
      this->name = name;
      this->treeUp = treeUp;
      this->treeDn = treeDn;
    }
    void fillUp() { treeUp->Fill(); }
    void fillDn() { treeDn->Fill(); }
  };
  std::map<std::string,ShapeUnc*> uncmap;
 public:
  void setDir(TDirectory* dir);
  void addUnc(std::string name);
  TTree* getTreeUp(std::string name);
  TTree* getTreeDn(std::string name);
  void fillUp(std::string name);
  void fillDn(std::string name);
  bool contains(std::string name);
};

#endif
