#ifndef monoJetSR_H
#define monoJetSR_H

#include "monoJetAnalysis.h"

class monoJetSignalRegion : public virtual monoJetAnalysis {
public:
  float eleveto_sf,muveto_sf,tauveto_sf;
  float eleveto_sfUp,muveto_sfUp,tauveto_sfUp;
  float eleveto_sfDown,muveto_sfDown,tauveto_sfDown;
  
  void initTree(TTree* tree);
  void initVars();
  
  virtual bool electron_veto();
  virtual bool muon_veto();
  virtual bool photon_veto();
  virtual bool tau_veto();
  
  virtual bool eleveto_weights(float &event_weight);
  virtual bool muveto_weights(float &event_weight);
  virtual bool tauveto_weights(float &event_weight);
};

#endif
