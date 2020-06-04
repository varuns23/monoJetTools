#ifndef ScaleUncCollection_h
#define ScaleUncCollection_h

#include <iostream>
#include <TTree.h>
#include <TH1F.h>

class ScaleUncCollection {
 private:
  TTree* tree;
  struct ScaleUnc {
    TString name;
    TH1F* histo;
    float up;
    float dn;
    ScaleUnc(TString name,TH1F* histo) {
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
  std::map<TString,ScaleUnc*> uncmap;
 public:
  void setTree(TTree* tree);
  void addUnc(TString name,TH1F* histo);
  TH1F* getHisto(TString name);
  float getBin(TString name, float x);
  void setUnc(TString name,float up,float dn);
  bool contains(TString name);
};

#endif
