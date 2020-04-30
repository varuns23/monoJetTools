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
  for (Long64_t jentry=0; jentry<nentriesToCheck; isMC ? jentry++ : jentry += 4) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    if (jentry%reportEvery == 0){
      cout<<"Analyzing entry "<<jentry<<"/"<<(nentriesToCheck)<<endl;
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
    if (isMC) {
      ApplyMET_TriggerSF(event_weight);
    }
    fillEvent(1,event_weight);

    if (!getMetFilter()) continue;
    fillEvent(2,event_weight);

    vector<int> elelist = getLooseEle();
    if (elelist.size() > 0) {
      eleveto_sf = 1 - getLooseEleSF(elelist[0]);
      eleveto_sfUp = 1 - getLooseEleSF(elelist[0],"up");
      eleveto_sfDown = 1 - getLooseEleSF(elelist[0],"down");
      // event_weight *= eleveto_sf;
    }
    fillEvent(3,event_weight);
    
    vector<int> mulist = getLooseMu();
    if (mulist.size() > 0) {
      muveto_sf = 1 - getLooseMuSF(mulist[0]);
      muveto_sfUp = 1 - getLooseMuSF(mulist[0],"up");
      muveto_sfDown = 1 - getLooseMuSF(mulist[0],"down");
      // event_weight *= muveto_sf;
    }
    fillEvent(4,event_weight);

    if (!photon_veto()) continue;
    fillEvent(5,event_weight);
    
    vector<int> taulist = getLooseTau();
    if (taulist.size() > 0) {
      tauveto_sf = 1 - getLooseTauSF(taulist[0]);
      tauveto_sfUp = 1 - getLooseTauSF(taulist[0],"up");
      tauveto_sfDown = 1 - getLooseTauSF(taulist[0],"down");
      // event_weight *= tauveto_sf;
    }
    fillEvent(6,event_weight);
 
    if (!bjet_veto(bjetDeepCSVCut_2017)) continue;
    vector<int> jetlist = getLooseJet();
    mindPhiJetMET = dPhiJetMETmin(jetlist,pfMETPhi);
    fillEvent(7,event_weight);
    
    if (mindPhiJetMET <= dPhiJetMETCut) continue;
    fillEvent(8,event_weight);
		      
    if (dpfcalo >= metRatioCut) continue;
    fillEvent(9,event_weight);

    if (pfMET <= recoilCut) continue;
    fillEvent(10,event_weight);

    int jetCand = getJetCand();
    if (jetCand == -1) continue;
    setJetCand(jetCand);

    fillEvent(11,event_weight);
  }

}//Closing the Loop function

void monoJetClass::BookHistos(const char* outputFilename) {

  output = new TFile(outputFilename, "RECREATE");
  output->cd();

  cutflow = new Cutflow(this,{s_TotalEvents,s_Triggers,s_METFilters,s_ElectronVeto,s_MuonVeto,
	s_PhotonVeto,s_TauVeto,s_BJetVeto,s_minDPhiJetMET,s_dPFCaloMET,s_Recoil,s_JetSelection});

  monoJetYear::BookHistos(-1,"");
  for(int i = 0; i<nHisto; i++) {
    char ptbins[100];
    sprintf(ptbins, "_%d", i);
    string histname(ptbins);
    auto dir = output->mkdir( ("monoJet"+histname).c_str() );
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
    monoJetYear::BookHistos(i,histname);
  }
}

void monoJetClass::fillHistos(int nhist,float event_weight) {
  monoJetYear::fillHistos(nhist,event_weight);
  weight = event_weight;
  if (nhist == bHisto) tree->Fill();
}
