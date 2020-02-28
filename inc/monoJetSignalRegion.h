#ifndef monoJetSR_H
#define monoJetSR_H

#include "monoJetAnalysis.h"

class monoJetSignalRegion : public virtual monoJetAnalysis {
public:
  
  virtual bool electron_veto();
  virtual bool muon_veto();
  virtual bool photon_veto();
  virtual bool tau_veto();
  virtual bool bjet_veto(float cutValue);
};

#endif
