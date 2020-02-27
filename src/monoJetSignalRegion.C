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
   
bool monoJetSignalRegion::bjet_veto(float cutValue){
  vector<int> bjet_cands;
  bjet_cands.clear();
  
  for(int i = 0; i < nJet; i++){
    bool kinematic = (jetPt->at(i) > bjetVetoPtCut && fabs(jetEta->at(i)) < bjetVetoEtaCut);
    float bjetTag = jetDeepCSVTags_b->at(i) + jetDeepCSVTags_bb->at(i);
    bool btagged = bjetTag > cutValue;
    if(kinematic && btagged )
      bjet_cands.push_back(i);
  }
  return bjet_cands.size() == 0;
}  

#endif
