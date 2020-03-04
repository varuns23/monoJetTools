//For use with Ntuples made from JetAnalyzer
////Required arguments: 1 is folder containing input files, 2 is output file path, 3 is maxEvents (-1 to run over all events), 4 is reportEvery
////
////To compile using rootcom to an executable named 'analyze':
////$ ./rootcom monoJetClass analyze
////
////To run, assuming this is compiled to an executable named 'analyze':
////$ ./analyze /hdfs/store/user/uhussain/monoJet_Ntuples/ /cms/uhussain/MonoJet/CMSSW_8_0_8/src/LightZPrimeAnalysis/JetAnalyzer/test/output.root -1 10000
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

    if (jentry%reportEvery == 0){
      cout<<"Analyzing entry "<<jentry<<"/"<<(nentriesToCheck-1)<<endl;
    }
    
    initVars();

    float event_weight = 1.;
    if (isMC) {
      ApplyPileup(event_weight);
      ApplyPrefiring(event_weight);
      if (isWZG()) {
    	SetBoson(PID);
    	ApplyKFactor(event_weight);
      }
    }

    
    fillEvent(0,genWeight);
    
    
    if (!getMetTrigger()) continue;
    if (!inclusiveCut()) continue;
    fillEvent(1,event_weight);

    vector<int> looselist = getLooseMu();
    if (looselist.size() != 1) continue;
    fillEvent(2,event_weight);
    
    vector<int> tightlist = getTightMu(looselist);
    if(tightlist.size() != 1) continue;
    fillEvent(3,event_weight);

    if (!CRSelection(tightlist, looselist)) continue;
    if (isMC) {
      SetSF( getSF(lepindex) );
      ApplySF(event_weight);
      ApplyMET_TriggerSF(event_weight);
    }

    if (recoil > recoilCut) h_lepMET_MT->Fill(lepMET_mt,event_weight);
    if (lepMET_mt >= lepMETMtCut) continue;
    fillEvent(4,event_weight);
	      
    if (!getMetFilter()) continue;
    fillEvent(5,event_weight);

    if (!electron_veto()) continue;
    fillEvent(6,event_weight);

    if (!photon_veto(lepindex)) continue;
    fillEvent(7,event_weight);

    if (!tau_veto(lepindex)) continue;
    fillEvent(8,event_weight);

    if (!bjet_veto( bjetDeepCSVCut_2017)) continue;
    fillEvent(9,event_weight);

    vector<int> jetlist = getLooseJet();
    float mindPhiJetMET = dPhiJetMETmin(jetlist,recoilPhi);
    if ( recoil > recoilCut) h_dphimin->Fill(mindPhiJetMET,event_weight);
    if (mindPhiJetMET <= dPhiJetMETCut) continue;
    fillEvent(10,event_weight);

    float dpfcalo = fabs(pfMET-caloMET)/recoil;
    if ( recoil > recoilCut) h_metcut->Fill(dpfcalo,event_weight);
    if (dpfcalo >= metRatioCut) continue;
    fillEvent(11,event_weight);

    if (recoil <= recoilCut) continue;
    fillEvent(12,event_weight);
 
    int jetCand = getJetCand();
    if (jetCand == -1) continue; 
    setJetCand(jetCand);

    PSWeights(event_weight);
    PFUncertainty(event_weight);
    QCDVariations(event_weight);
    fillEvent(13,event_weight);
  }

}//Closing the Loop function

void monoJetClass::BookHistos(const char* outputFilename) {

  output = new TFile(outputFilename, "RECREATE");
  output->cd();

  cutflow = new Cutflow(this,{"Total Events","Triggers","One Loose Muon","One Tight Muon","Electron MET M_{T}","MET Filters",
	"Electron Veto","Photon Veto","Tau Veto","BJet Veto","minDPhiJetMET","dPFCaloMET","Recoil","Jet Selection"});

  BookHistos(-1,"");
  for(int i = 0; i<nHisto; i++) {
    char ptbins[100];
    sprintf(ptbins, "_%d", i);
    string histname(ptbins);
    auto dir = output->mkdir( ("monoJet"+histname).c_str() );
    dir->cd();
    if (i == bHisto) {
      auto treedir = dir->mkdir("trees");
      treedir->cd();
      tree = new TTree("norm","norm");
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
  monoJetSingleMuCR::fillHistos(nhist,event_weight);
  weight = event_weight;
  if (nhist == bHisto) tree->Fill();
}

bool monoJetClass::UncLoop(float &event_weight) {
  if (lepMET_mt >= lepMETMtCut) return false;
	      
  if (!getMetFilter()) return false;

  if (!electron_veto()) return false;

  if (!photon_veto(lepindex)) return false;

  if (!tau_veto(lepindex)) return false;

  if (!bjet_veto( bjetDeepCSVCut_2017)) return false;

  vector<int> jetlist = getLooseJet();
  float mindPhiJetMET = dPhiJetMETmin(jetlist,recoilPhi);
  if (mindPhiJetMET <= dPhiJetMETCut) return false;

  float dpfcalo = fabs(pfMET-caloMET)/recoil;
  if (dpfcalo >= metRatioCut) return false;

  if (recoil <= recoilCut) return false;
 
  int jetCand = getJetCand();
  if (jetCand == -1) return false; 
  setJetCand(jetCand);
  return true;
}
void monoJetClass::JetEnergyScale(float start_weight) {
  string uncname = "JES";
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
  string uncname = "JER";
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
