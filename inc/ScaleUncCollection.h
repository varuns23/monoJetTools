#ifndef ScaleUncCollection_h
#define ScaleUncCollection_h

#include <iostream>
#include <TTree.h>
#include <TH1F.h>

class ScaleUncCollection {
 private:
  TTree* tree;
  struct ScaleUnc {
    std::string name;
    TH1F* histo;
    float up;
    float dn;
    ScaleUnc(std::string name,TH1F* histo) {
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
  std::map<std::string,ScaleUnc*> uncmap;
 public:
  void setTree(TTree* tree);
  void addUnc(std::string name,TH1F* histo);
  TH1F* getHisto(std::string name);
  float getBin(std::string name, float x);
  void setUnc(std::string name,float up,float dn);
  bool contains(std::string name);
};

#endif
