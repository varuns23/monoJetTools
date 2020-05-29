//For use with Ntuples made from JetAnalyzer
////Required arguments: 1 is folder containing input files, 2 is output file path, 3 is maxEvents (-1 to run over all events), 4 is reportEvery
////
////To compile using rootcom to an executable named 'analyze':
////$ ./rootcom monoJetClass analyze
////
////To run, assuming this is compiled to an executable named 'analyze':
////$ ./analyze <Output Path> <Input Path> -1 10000
////Runs over every event in the folder monoJet_Ntuples, reporting progress every 10000 events
////and storing the resulting histograms in the file output.root.
////
//
#define monoJetClass_cxx
#include "monoJetClass.h"
#include "Utilities.h"

using namespace std;

void monoJetClass::Loop(Long64_t maxEvents, int reportEvery) {
  if (fChain == 0) return;

  Long64_t nentries = fChain->GetEntries();
  cout<<"Coming in:"<<endl;
  cout<<"nentries:"<<nentries<<endl;
  Long64_t nentriesToCheck = nentries;
  
  if (isMC) SetScalingHistos();

  if (maxEvents != -1LL && nentries > maxEvents)
    nentriesToCheck = maxEvents;
  int nTotal = nentriesToCheck;
  Long64_t nbytes = 0, nb = 0;
  cout<<"Running over "<<nTotal<<" events."<<endl;  
  for (Long64_t jentry=0; jentry<nentriesToCheck;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    if (jentry%reportEvery == 0) {
      cout<<"Analyzing entry "<<jentry<<"/"<<(nentriesToCheck)<<endl;
    }
    
    initVars();

    float event_weight = 1.;
    if (isMC) {
      ApplyPileup(event_weight);
      if (isWZG()) {
      	SetBoson(PID);
      	ApplyKFactor(event_weight);
      }
    }


    fillEvent(0,genWeight);

    if (!getElectronTrigger()) continue;
    if (!inclusiveCut()) continue;
    fillEvent(1,event_weight);

    vector<int> looselist = getLooseEle();
    if (looselist.size() != 1) continue;
    fillEvent(2,event_weight);

    vector<int> tightlist = getTightEle(looselist);
    if(tightlist.size() != 1) continue;

    if (!CRSelection(tightlist, looselist)) continue;
    if (isMC) {
      SetSF( getSF(lepindex) );
      ApplySF(event_weight);
      ApplyElectron_TriggerSF(event_weight);
    }
    fillEvent(3,event_weight);

    JetEnergyScale(event_weight);
    JetEnergyResolution(event_weight);

    if (pfMET <= 50) continue;
    fillEvent(4,event_weight);

    if (lepMET_mt >= lepMETMtCut) continue;
    fillEvent(5,event_weight);

    if (!getMetFilter()) continue;
    fillEvent(6,event_weight);

    if (!muon_veto()) continue;
    fillEvent(7,event_weight);

    if (!photon_veto(lepindex)) continue;
    fillEvent(8,event_weight);

    if (!tau_veto(lepindex)) continue;
    fillEvent(9,event_weight);

    if (!bjet_weights(bjetDeepCSVCut_2018,event_weight)) continue;
    vector<int> jetlist = getLooseJet();
    mindPhiJetMET = dPhiJetMETmin(jetlist,recoilPhi);
    fillEvent(10,event_weight);
    
    if (mindPhiJetMET <= dPhiJetMETCut) continue;
    fillEvent(11,event_weight);
    
    if (dpfcalo >= metRatioCut) continue;
    fillEvent(12,event_weight);

    if (recoil <= recoilCut) continue;
    fillEvent(13,event_weight);
			    
    int jetCand = getJetCand();
    if (jetCand == -1) continue;
    setJetCand(jetCand);                                                                                                                          
    fillEvent(14,event_weight);
	      
    if(!getJetHEMVeto())continue;   

    QCDVariations(event_weight);
    fillEvent(15,event_weight);
  }

}//Closing the Loop function

void monoJetClass::BookHistos(const char* outputFilename) {

  output = new TFile(outputFilename, "RECREATE");
  output->cd();

  cutflow = new Cutflow(this,{s_TotalEvents,s_Triggers,s_OneLooseEle,s_OneTightEle,s_pfMET50,s_EleMET_MT,s_METFilters,
	s_MuonVeto,s_PhotonVeto,s_TauVeto,s_BJetVeto,s_JetSelection,s_dPFCaloMET,s_minDPhiJetMET,s_Recoil,s_HEMVeto});

  BookHistos(-1,"");
  for(int i = 0; i<nHisto; i++) {
    char ptbins[100];
    sprintf(ptbins, "_%d", i);
    TString histname(ptbins);
    auto dir = output->mkdir( ("monoJet"+histname) );
    dir->cd();
    if (i == bHisto) {
      auto treedir = dir->mkdir("trees");
      treedir->cd();
      tree = new TTree("tree","tree");
      initTree(tree);
      scaleUncs.setTree(tree);
      shapeUncs.setDir(treedir);
      dir->cd();
    }
    BookHistos(i,histname);
  }
}

void monoJetClass::fillHistos(int nhist,float event_weight) {
  monoJetYear::fillHistos(nhist,event_weight);
  monoJetSingleEleCR::fillHistos(nhist,event_weight);
  weight = event_weight;
  if (nhist == bHisto) tree->Fill();
}

bool monoJetClass::UncLoop(float &event_weight) {
  if (pfMET <= 50) return false;

  if (lepMET_mt >= lepMETMtCut) return false;

  if (!getMetFilter()) return false;

  if (!muon_veto()) return false;

  if (!photon_veto(lepindex)) return false;

  if (!tau_veto(lepindex)) return false;

  if (!bjet_weights(bjetDeepCSVCut_2018,event_weight)) return false;
      
  vector<int> jetlist = getLooseJet();
  mindPhiJetMET = dPhiJetMETmin(jetlist,recoilPhi);
  if (mindPhiJetMET <= dPhiJetMETCut) return false;
  
  if (dpfcalo >= metRatioCut) return false;

  if (recoil <= recoilCut) return false;
			    
  int jetCand = getJetCand();
  if (jetCand == -1) return false;
  setJetCand(jetCand);
	      
  if(!getJetHEMVeto())return false;
  
  return true;
}
void monoJetClass::JetEnergyScale(float start_weight) {
  TString uncname = "JES";
  if ( !shapeUncs.contains(uncname) ) {
    shapeUncs.addUnc(uncname);
    initTree(shapeUncs.getTreeUp(uncname));
    initTree(shapeUncs.getTreeDn(uncname));
  }

  /* Initializing Variables */
  int n_jetindex = jetindex;
  vector<float> n_jetPt;
  for (float pt : (*jetPt)) n_jetPt.push_back(pt);
  float n_pfMET = pfMET;
  float n_pfMETPhi = pfMETPhi;
  float n_recoil = recoil;
  float n_recoilPhi = recoilPhi;

  int unclist[2] = {-1,1};
  for (int unc : unclist) {
    float event_weight = start_weight;
    for (int i = 0; i < nJet; i++)
      jetPt->at(i) = n_jetPt[i]*(1 + unc*jetJECUnc->at(i));
    switch(unc) {
    case  1:
      pfMET = pfMET_T1JESUp;
      pfMETPhi = pfMETPhi_T1JESUp;
      break;
    case -1:
      pfMET = pfMET_T1JESDo;
      pfMETPhi = pfMETPhi_T1JESDo;
      break;
    }
    recoil = pfMET;
    recoilPhi = pfMETPhi;

    setRecoil();

    if (!UncLoop(event_weight)) continue;

    weight = event_weight;
    switch(unc) {
    case  1: shapeUncs.fillUp(uncname); break;
    case -1: shapeUncs.fillDn(uncname); break;
    }
  }

  /* Reset Changed Variables */
  for (int i = 0; i < nJet; i++)
    jetPt->at(i) = n_jetPt[i];
  jetindex = n_jetindex;
  setJetCand(jetindex);
  pfMET = n_pfMET;
  pfMETPhi = n_pfMETPhi;
  recoil = n_recoil;
  recoilPhi = n_recoilPhi;
  setRecoil();
}
void monoJetClass::JetEnergyResolution(float start_weight) {
  TString uncname = "JER";
  if ( !shapeUncs.contains(uncname) ) {
    shapeUncs.addUnc(uncname);
    initTree(shapeUncs.getTreeUp(uncname));
    initTree(shapeUncs.getTreeDn(uncname));
  }
  if (isData || isSignal) return;
  /* Initializing Variables */
  int n_jetindex = jetindex;
  vector<float> n_jetPt;
  for (float pt : (*jetPt)) n_jetPt.push_back(pt);
  float n_pfMET = pfMET;
  float n_recoil = recoil;

  int unclist[2] = {-1,1};
  for (int unc : unclist) {
    float event_weight = start_weight;
    for (int i = 0; i < nJet; i++) {
      switch(unc) {
      case 1: jetPt->at(i) = jetUnCorrPt->at(i) * jetP4SmearUp->at(i); break;
      case -1: jetPt->at(i) = jetUnCorrPt->at(i) * jetP4SmearDo->at(i); break;
      }
    }
    switch(unc) {
    case 1:
      pfMET = pfMET_T1JERUp; break;
    case -1:
      pfMET = pfMET_T1JERDo; break;
    }
    recoil = pfMET;

    setRecoil();

    if (!UncLoop(event_weight)) continue;

    weight = event_weight;
    switch(unc) {
    case 1: shapeUncs.fillUp(uncname); break;
    case -1: shapeUncs.fillDn(uncname); break;
    }
  }

  /* Reset Changed Variables */
  for (int i = 0; i < nJet; i++)
    jetPt->at(i) = n_jetPt[i];
  jetindex = n_jetindex;
  setJetCand(jetindex);
  pfMET = n_pfMET;
  recoil = n_recoil;
  setRecoil();
}
