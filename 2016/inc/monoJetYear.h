#ifndef monoJetYear_h
#define monoJetYear_h

#include "monoJetAnalysis.h"

class monoJetYear : public virtual monoJetAnalysis {
public:
  static const TString SRDATA;
  
  virtual void BookHistos(int i,TString histname);
  virtual void fillHistos(int nhist,float event_weight);
  virtual void initVars();
  virtual void initTree(TTree* tree);
  virtual void SetScalingHistos();
};

#endif
