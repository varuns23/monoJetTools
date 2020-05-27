#ifndef monoJetSR_C
#define monoJetSR_C

#include "monoJetSignalRegion.h"
#include "VariableBins.h"
#include "Utilities.h"

using namespace std;

Region monoJetAnalysis::REGION = SR;
CRobject monoJetAnalysis::CROBJECT = None;

void monoJetSignalRegion::monoJetSignalRegion::initTree(TTree* tree) {
  tree->Branch("eleveto_sf",&eleveto_sf);
  tree->Branch("eleveto_sfUp",&eleveto_sfUp);
  tree->Branch("eleveto_sfDown",&eleveto_sfDown);
  tree->Branch("muveto_sf",&muveto_sf);
  tree->Branch("muveto_sfUp",&muveto_sfUp);
  tree->Branch("muveto_sfDown",&muveto_sfDown);
  tree->Branch("tauveto_sf",&tauveto_sf);
  tree->Branch("tauveto_sfUp",&tauveto_sfUp);
  tree->Branch("tauveto_sfDown",&tauveto_sfDown);
}
void monoJetSignalRegion::initVars() {
  eleveto_sf = muveto_sf = tauveto_sf = 1;
  eleveto_sfUp = muveto_sfUp = tauveto_sfUp = 1;
  eleveto_sfDown = muveto_sfDown = tauveto_sfDown = 1;
}

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

bool monoJetSignalRegion::eleveto_weights(float &event_weight) {
  if (isMC) {
    vector<int> elelist = getLooseEle();
    if (elelist.size() > 0){
      eleveto_sf = 1 - getLooseEleSF(elelist[0]);
      eleveto_sfUp = 1 - getLooseEleSF(elelist[0],"up");
      eleveto_sfDown = 1 - getLooseEleSF(elelist[0],"down");
      event_weight *= eleveto_sf;
    }
    return true;
  }
  return electron_veto();
}

bool monoJetSignalRegion::muveto_weights(float &event_weight) {
  if (isMC) {
    vector<int> mulist = getLooseMu();
    if (mulist.size() > 0){
      muveto_sf = 1 - getLooseMuSF(mulist[0]);
      muveto_sfUp = 1 - getLooseMuSF(mulist[0],"up");
      muveto_sfDown = 1 - getLooseMuSF(mulist[0],"down");
      event_weight *= muveto_sf;
    }
    return true;
  }
  return muon_veto();
}

bool monoJetSignalRegion::tauveto_weights(float &event_weight) {
  if (isMC) {
    vector<int> taulist = getLooseTau();
    if (taulist.size() > 0){
      tauveto_sf = 1 - getLooseTauSF(taulist[0]);
      tauveto_sfUp = 1 - getLooseTauSF(taulist[0],"up");
      tauveto_sfDown = 1 - getLooseTauSF(taulist[0],"down");
      event_weight *= tauveto_sf;
    }
    return true;
  }
  return tau_veto();
}

#endif
