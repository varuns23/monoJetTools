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

    cutflow->Fill(0,genWeight);

    if (!getPhotonTrigger()) continue;
    cutflow->Fill(1,event_weight);
    if (!getMetFilter()) continue;
    cutflow->Fill(2,event_weight);

    phoindex = getPhoIndex();
    if(phoindex < 0) continue;
    if (isMC) {
      SetSF( getSF(phoindex) );
      ApplySF(event_weight);
      ApplyPhoton_TriggerSF(event_weight);
    }
    
    cutflow->Fill(3,event_weight);

    if (!electron_veto(phoindex)) continue;
    cutflow->Fill(4,event_weight);

    if (!muon_veto(phoindex)) continue;
    cutflow->Fill(5,event_weight);

    if (!tau_veto(phoindex)) continue;
    cutflow->Fill(6,event_weight);

    if (!bjet_weights(bjetDeepCSVCut_2018,event_weight)) continue;
    cutflow->Fill(7,event_weight);

    pho.SetPtEtaPhiE(phoCalibEt->at(phoindex),phoEta->at(phoindex),phoPhi->at(phoindex),phoCalibE->at(phoindex));
    setRecoil();
    vector<int> jetlist = getLooseJet();
    float mindPhiJetMET = dPhiJetMETmin(jetlist,recoilPhi);
    
    if (mindPhiJetMET <= dPhiJetMETCut) continue;
    cutflow->Fill(8,event_weight);

    jetindex = getJetIndex(phoindex);
    if (jetindex < 0) continue; 
    cutflow->Fill(9,event_weight);
   
    setJetCand(jetindex);
    setPhoton(phoindex);

    nominal(event_weight);
    met_variation(1,event_weight);
    met_variation(-1,event_weight);
    sideband_variation(1,event_weight);
    sideband_variation(-1,event_weight);
  }
}//Closing the Loop function

void monoJetClass::nominal(float event_weight) {
    if (pfMET >= 60) return;
    cutflow->Fill(10,event_weight);
    if (!getSideband()) return;
    cutflow->Fill(11,event_weight);
    if (!getJetHEMVeto()) return;
    cutflow->Fill(12,event_weight);
    fillHistos(12,event_weight);
}

void monoJetClass::met_variation(int var,float event_weight) {
  // Vary pfMET Cut by 20%
  if ( pfMET >= (60 * ( 1 + var*0.2 )) ) return;
  if (!getSideband()) return;
  if (!getJetHEMVeto()) return;
  switch(var) {
  case 1: fillHistos(13,event_weight); break;
  case -1:fillHistos(14,event_weight); break;
  }
}

void monoJetClass::sideband_variation(int var,float event_weight) {
  if ( pfMET >= 60 ) return;
  if (!getSideband(var)) return;
  if (!getJetHEMVeto()) return;
  switch(var) {
  case 1: fillHistos(15,event_weight); break;
  case -1:fillHistos(16,event_weight); break;
  }
}


bool monoJetClass::getSideband(int var,int type) {
  if (runIsoPurity){
    switch(type){
      // outside (0.015,0.02)
    case 0: return (photon_sieie > (0.02 * ( 1 + var*0.1 )) || photon_sieie < 0.015);
      
      // (0.01015,0.015)
    case 1: return 0.01015 < photon_sieie && photon_sieie < (0.015 * ( 1 + var*0.2 ));
      
      // (0.01015,0.02)
    case 2: return 0.01015 < photon_sieie && photon_sieie < (0.02 * ( 1 + var*0.2 ));
    }
  }
  return true;
}

void monoJetClass::BookHistos(const char* outputFilename) {
  output = new TFile(outputFilename, "RECREATE");
  output->cd();
  
  vector<TString> cutlist = {s_TotalEvents,s_Triggers,s_METFilters,"Photon Selection",s_ElectronVeto,s_MuonVeto,
			    s_TauVeto,s_BJetVeto,s_minDPhiJetMET,s_JetSelection,"MET60","SigmaIEtaIEta Sideband",s_HEMVeto};
  cutflow = new Cutflow(this,cutlist);

  BookHistos(-1,"");
  for(int i = bHisto; i<nHisto; i++) {
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
  monoJetGammaPurity::fillHistos(nhist,event_weight);
  weight = event_weight;
  if (nhist == bHisto) tree->Fill();
}

int monoJetClass::getPhoIndex() {
  for (int ipho = 0; ipho < nPho; ipho++) {
    
    bool kinematics = ((phoCalibEt->at(ipho) > 230.0) && (fabs(phoSCEta->at(ipho)) < 1.4442));
    bool id,iso;
    if (runIsoPurity) {
      // Isolation Purity Photon ID
      id = CutBasedPhotonID_noSieie(ipho, "medium");
      iso= CutBasedPhotonIso_noPhoIso(ipho, "medium");
    } else {
      // Sieie Purity Photon ID
      id = CutBasedPhotonID_noSieie(ipho, "medium");
      iso= !CutBasedPhotonIso(ipho, "medium");
    }

    if(kinematics && id && iso)
      return ipho;
  }

  //if (type == GJets && PFIso) return ipho;
  //else if (type == Data && !PFIso) return ipho;
  return -1;
}
