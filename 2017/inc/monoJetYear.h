#ifndef monoJetYear_h
#define monoJetYear_h

#include "monoJetAnalysis.h"

class monoJetYear : public virtual monoJetAnalysis {
public:
  static const std::string SRDATA;
  
  virtual void BookHistos(int i,string histname);
  virtual void fillHistos(int nhist,float event_weight);
  virtual void initVars();
  virtual void initTree(TTree* tree);
  virtual void SetScalingHistos();
};

#endif
