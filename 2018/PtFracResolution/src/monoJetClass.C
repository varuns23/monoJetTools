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

    if(!getJetHEMVeto())continue;                                                                                                                             
    fillEvent(3,event_weight);
    
    if (!electron_veto()) continue;
    fillEvent(4,event_weight);

    if (!muon_veto()) continue;
    fillEvent(5,event_weight);

    if (!photon_veto()) continue;
    fillEvent(6,event_weight);

    if (!tau_veto()) continue;
    fillEvent(7,event_weight);

    if (!bjet_veto(bjetDeepCSVCut_2018)) continue;
    vector<int> jetlist = getLooseJet();
    mindPhiJetMET = dPhiJetMETmin(jetlist,pfMETPhi);
    fillEvent(8,event_weight);
    
    if (mindPhiJetMET <= dPhiJetMETCut) continue;
    fillEvent(9,event_weight);
		      
    if (dpfcalo >= metRatioCut) continue;
    fillEvent(10,event_weight);

    if (pfMET <= recoilCut) continue;
    fillEvent(11,event_weight);

    int jetCand = getJetCand();
    if (jetCand == -1) continue;
    setJetCand(jetCand);
    setGenJetCand();
    fillEvent(12,event_weight);
  }

}//Closing the Loop function

void monoJetClass::BookHistos(const char* outputFilename) {

  output = new TFile(outputFilename, "RECREATE");
  output->cd();

  cutflow = new Cutflow(this,{s_TotalEvents,s_Triggers,s_METFilters,s_HEMVeto,s_ElectronVeto,s_MuonVeto
	,s_PhotonVeto,s_TauVeto,s_BJetVeto,s_minDPhiJetMET,s_dPFCaloMET,s_Recoil,s_JetSelection});

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


void monoJetClass::initTree(TTree* tree) {
  monoJetYear::initTree(tree);
  tree->Branch("j1pT",&j1pT);
  tree->Branch("j1GenpT",&j1GenpT);
  tree->Branch("j1GenetaWidth",&j1GenetaWidth);
  tree->Branch("GenChNemPtFrac",&GenChNemPtFrac);
  tree->Branch("Pt123Fraction",&Pt123Fraction);
  tree->Branch("GenPt123Fraction",&GenPt123Fraction);
}


void monoJetClass::setGenJetCand() {
  if (nJet == 0 || jetindex < 0) return;
  if (jetGenJetPt->at(jetindex) < 0) return;
  j1GenpT = jetGenJetPt->at(jetindex);
  j1GenEta = jetGenJetEta->at(jetindex);
  j1GenPhi = jetGenJetPhi->at(jetindex);
  j1GenetaWidth = jetGenJetetaWidth->at(jetindex);
  
  vector<float> j1GenPFConsEt = jetGenJetConstEt->at(jetindex);
  vector<float> j1GenPFConsPt = jetGenJetConstPt->at(jetindex);
  vector<float> j1GenPFConsEta = jetGenJetConstEta->at(jetindex);
  vector<float> j1GenPFConsPhi = jetGenJetConstPhi->at(jetindex);
  vector<int> j1GenPFConsPID = jetGenJetConstPdgId->at(jetindex);

  GenPt123Fraction=GenChNemPtFrac=-1.0;
  float pfHadronPt[4] = {0,0,0,0};
  float first3_HadronPt[4] = {0,0,0,0};

  for (int i = 0; i < j1GenPFConsPID.size(); i++) {
    int PID = j1GenPFConsPID[i];
    int absPID = abs(PID);
    float consPt = j1GenPFConsPt[i];
    // cout << i << " : Gen " << PID << " : Gen " << consPt << endl;
    // cout << i << " : Rec " << j1PFConsPID[i] << " : Rec " << j1PFConsPt[i] << endl;
    if ( i < 3 ) {
      switch(absPID) {
      case Hadron::Charged: first3_HadronPt[0] += consPt; break;
      // case 321:             first3_HadronPt[0] += consPt; break;
      case Hadron::Neutral: first3_HadronPt[1] += consPt; break;
      case Hadron::Gamma:   first3_HadronPt[2] += consPt; break;
      case 111:             first3_HadronPt[2] += consPt; break;
      default:              first3_HadronPt[3] += consPt; break;
      }
    }
    switch(absPID) {
    case Hadron::Charged: pfHadronPt[0] += consPt; break;
    // case 321:             pfHadronPt[0] += consPt; break;
    case Hadron::Neutral: pfHadronPt[1] += consPt; break;
    case Hadron::Gamma:   pfHadronPt[2] += consPt; break;
    case 111:             pfHadronPt[2] += consPt; break;
    default:              pfHadronPt[3] += consPt; break;
    }
  }
  float pt123 = first3_HadronPt[0] + first3_HadronPt[1] + first3_HadronPt[2] + first3_HadronPt[3];
  GenPt123Fraction = pt123/j1GenpT;
  float chnempt = pfHadronPt[0] + pfHadronPt[2];
  float chnempt123 = first3_HadronPt[0] + first3_HadronPt[2];
  GenChNemPtFrac = chnempt123/chnempt;

  // printf("Gen Info\n");
  // printf("jetPt:       %f\n",j1GenpT);
  // printf("Pt123:       %f\n",pt123);
  // printf("Pt123Frac:   %f\n",GenPt123Fraction);
  // printf("ChNemPt:     %f\n",chnempt);
  // printf("ChNemPt123:  %f\n",chnempt123);
  // printf("ChNemPtFrac: %f\n",GenChNemPtFrac);
  // printf("Rec Info\n");
  // printf("ChNemPt:     %f\n",ChNemPt);
  // printf("ChNemPt123:  %f\n",ChNemPt123);
  // printf("ChNemPtFrac: %f\n",ChNemPtFrac);
}

void monoJetClass::Init(TTree* fChain) {
  jetGenPt = 0;
  jetGenE = 0;
  jetGenEta = 0;
  jetGenPhi = 0;
  jetGenetaWidth = 0;
  jetGenphiWidth = 0;
  jetGenConstPt = 0;
  jetGenConstEt = 0;
  jetGenConstEta = 0;
  jetGenConstPhi = 0;
  jetGenConstPdgId = 0;
  
  jetGenJetPt = 0;
  jetGenJetE = 0;
  jetGenJetEta = 0;
  jetGenJetPhi = 0;
  jetGenJetetaWidth = 0;
  jetGenJetphiWidth = 0;
  jetGenJetConstPt = 0;
  jetGenJetConstEt = 0;
  jetGenJetConstEta = 0;
  jetGenJetConstPhi = 0;
  jetGenJetConstPdgId = 0;

  fChain->SetBranchAddress("jetGenPt", &jetGenPt, &b_jetGenPt);
  fChain->SetBranchAddress("jetGenE", &jetGenE, &b_jetGenE);
  fChain->SetBranchAddress("jetGenEta", &jetGenEta, &b_jetGenEta);
  fChain->SetBranchAddress("jetGenPhi", &jetGenPhi, &b_jetGenPhi);
  // fChain->SetBranchAddress("jetGenetaWidth", &jetGenetaWidth, &b_jetGenetaWidth);
  // fChain->SetBranchAddress("jetGenphiWidth", &jetGenphiWidth, &b_jetGenphiWidth);
  // fChain->SetBranchAddress("jetGenConstPt", &jetGenConstPt, &b_jetGenConstPt);
  // fChain->SetBranchAddress("jetGenConstEt", &jetGenConstEt, &b_jetGenConstEt);
  // fChain->SetBranchAddress("jetGenConstEta", &jetGenConstEta, &b_jetGenConstEta);
  // fChain->SetBranchAddress("jetGenConstPhi", &jetGenConstPhi, &b_jetGenConstPhi);
  // fChain->SetBranchAddress("jetGenConstPdgId", &jetGenConstPdgId, &b_jetGenConstPdgId);
  
  fChain->SetBranchAddress("jetGenJetPt", &jetGenJetPt, &b_jetGenJetPt);
  fChain->SetBranchAddress("jetGenJetE", &jetGenJetE, &b_jetGenJetE);
  fChain->SetBranchAddress("jetGenJetEta", &jetGenJetEta, &b_jetGenJetEta);
  fChain->SetBranchAddress("jetGenJetPhi", &jetGenJetPhi, &b_jetGenJetPhi);
  fChain->SetBranchAddress("jetGenJetetaWidth", &jetGenJetetaWidth, &b_jetGenJetetaWidth);
  fChain->SetBranchAddress("jetGenJetphiWidth", &jetGenJetphiWidth, &b_jetGenJetphiWidth);
  fChain->SetBranchAddress("jetGenJetConstPt", &jetGenJetConstPt, &b_jetGenJetConstPt);
  fChain->SetBranchAddress("jetGenJetConstEt", &jetGenJetConstEt, &b_jetGenJetConstEt);
  fChain->SetBranchAddress("jetGenJetConstEta", &jetGenJetConstEta, &b_jetGenJetConstEta);
  fChain->SetBranchAddress("jetGenJetConstPhi", &jetGenJetConstPhi, &b_jetGenJetConstPhi);
  fChain->SetBranchAddress("jetGenJetConstPdgId", &jetGenJetConstPdgId, &b_jetGenJetConstPdgId);
}

