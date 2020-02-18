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
  
  if (!sample.isData) SetScalingHistos();

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
      cout<<"Analyzing entry "<<jentry<<"/"<<(nentriesToCheck-1)<<endl;
    }
    
    initVars();

    float event_weight = 1.;
    if (!sample.isData) {
      ApplyPileup(event_weight);
      if (isWZG()) {
      	SetBoson(sample.PID);
      	ApplyKFactor(event_weight);
      }
    }


    fillEvent(0,genWeight);

    if (!inclusiveCut()) continue;

    vector<int> looselist = getLooseEle();
    if (looselist.size() != 1) continue;
    fillEvent(1,event_weight);

    vector<int> tightlist = getTightEle(looselist);
    if(tightlist.size() != 1) continue;
    fillEvent(2,event_weight);

    if (!CRSelection(tightlist, looselist)) continue;
    if (!sample.isData) {
      SetSF( getSF(lepindex) );
      ApplySF(event_weight);
    }

    if (pfMET <= 50) continue;
    fillEvent(3,event_weight);

    h_lepMET_MT->Fill(lepMET_mt,event_weight);
    if (lepMET_mt >= lepMETMtCut) continue;
    fillEvent(4,event_weight);

    if (!getMetFilter()) continue;
    fillEvent(5,event_weight);
	      
    if (!muon_veto()) continue;
    fillEvent(6,event_weight);

    if (!photon_veto(lepindex)) continue;
    fillEvent(7,event_weight);

    if (!tau_veto(lepindex)) continue;
    fillEvent(8,event_weight);

    if (!bjet_veto(lepindex, bjetDeepCSVCut_2017)) continue;
    fillEvent(9,event_weight);

    vector<int> jetlist = jet_veto(lepindex);
    float mindPhiJetMET = dPhiJetMETmin(jetlist,recoilPhi);
    h_dphimin->Fill(mindPhiJetMET,event_weight);
    if (mindPhiJetMET <= dPhiJetMETCut) continue;
    fillEvent(10,event_weight);
      
    float dpfcalo = fabs(pfMET-caloMET)/recoil;
    h_metcut->Fill(dpfcalo,event_weight);
    if (dpfcalo >= metRatioCut) continue;
    fillEvent(11,event_weight);

    if (recoil <= recoilCut) continue;
    fillEvent(12,event_weight);

    jetCand = getJetCand(jetlist,lepindex);
    if (jetCand.size() < 1) continue;
    setJetCand(jetCand);
    fillEvent(13,event_weight);
			    
    if (!getElectronTrigger()) continue;
    fillEvent(14,event_weight);
  }

}//Closing the Loop function

void monoJetClass::BookHistos(const char* outputFilename) {

  output = new TFile(outputFilename, "RECREATE");
  output->cd();

  cutflow = new Cutflow({"Total Events","One Loose Electron","One Tight Electron","pfMET50","Electron MET M_{T}","MET Filters",
	"Muon Veto","Photon Veto","Tau Veto","BJet Veto","minDPhiJetMET","dPFCaloMET","Recoil","Jet Selection","Triggers"});

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
  monoJetSingleEleCR::fillHistos(nhist,event_weight);
  weight = event_weight;
  if (nhist == bHisto) tree->Fill();
}
