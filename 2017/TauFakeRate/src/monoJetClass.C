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
  for (Long64_t jentry=0; jentry<nentriesToCheck; jentry++) {
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

    if (!getMuTauTrigger()) continue;
    if (!inclusiveCut()) continue;
    fillEvent(1,event_weight);

    if (!getMetFilter()) continue;
    fillEvent(2,event_weight);

    if (pfMET >= 60) continue;
    fillEvent(3,event_weight);
    
    vector<int> taulist = getLooseTau();
    vector<int> mulist = getLooseMu();
    
    if (taulist.size() != 1 || mulist.size() != 1) continue;
    if (!CRSelection(taulist,mulist)) continue;
    iso_category = isMedium(tauindex);
    fillEvent(4,event_weight);

    if (tau_pt <= 30) continue;
    fillEvent(5,event_weight);

    if (muon_pt <= 50) continue;
    fillEvent(6,event_weight);
    
    if (!electron_veto()) continue;
    fillEvent(7,event_weight);

    if (!photon_veto(tauindex,muindex)) continue;
    fillEvent(8,event_weight);

    if (!bjet_veto(bjetDeepCSVCut_2017)) continue;
    fillEvent(9,event_weight);

    if (iso_category) fillEvent(10,event_weight);
    else              fillEvent(11,event_weight);
  }

}//Closing the Loop function

void monoJetClass::BookHistos(const char* outputFilename) {

  output = new TFile(outputFilename, "RECREATE");
  output->cd();

  cutflow = new Cutflow(this,{s_TotalEvents,s_Triggers,s_METFilters,"MET60","Exactly 1 Tau and Muon","tauPt30","muonPt50",s_ElectronVeto,s_PhotonVeto,s_BJetVeto,"IsoCategory","Non-IsoCategory"});

  monoJetYear::BookHistos(-1,"");
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
    monoJetYear::BookHistos(i,histname);
    monoJetTauFakeRate::BookHistos(i,histname);
  }
}

void monoJetClass::fillHistos(int nhist,float event_weight) {
  monoJetYear::fillHistos(nhist,event_weight);
  monoJetTauFakeRate::fillHistos(nhist,event_weight);
  weight = event_weight;
  if (nhist == bHisto) tree->Fill();
}
