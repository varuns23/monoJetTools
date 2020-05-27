#ifndef monoJetSR_C
#define monoJetSR_C

#include "monoJetSignalRegion.h"
#include "VariableBins.h"
#include "Utilities.h"

using namespace std;

Region monoJetAnalysis::REGION = SR;
CRobject monoJetAnalysis::CROBJECT = None;

bool monoJetSignalRegion::electron_veto() {
  vector<int> tmpcands = getLooseEle();
  return tmpcands.size() == 0;
}

bool monoJetSignalRegion::muon_veto() {
  vector<int> tmpcands = getLooseMu();
  return tmpcands.size() == 0;
}

bool monoJetSignalRegion::photon_veto(){
  vector<int> pho_cands = getLoosePho();
  return pho_cands.size() == 0;
} 
  
bool monoJetSignalRegion::tau_veto(){
  vector<int> tau_cands = getLooseTau();
  return tau_cands.size() == 0;
}  

#endif
