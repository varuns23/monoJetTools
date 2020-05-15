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
    cutflow->Fill(3,event_weight);

    if (!electron_veto(phoindex)) continue;
    cutflow->Fill(4,event_weight);

    if (!muon_veto(phoindex)) continue;
    cutflow->Fill(5,event_weight);

    if (!tau_veto(phoindex)) continue;
    cutflow->Fill(6,event_weight);

    if (!bjet_veto( bjetDeepCSVCut_2017)) continue;
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

    photon_pt = phoCalibEt->at(phoindex);
    photon_eta = phoEta->at(phoindex);
    photon_phi = phoPhi->at(phoindex);
    photon_sieie = phoSigmaIEtaIEtaFull5x5->at(phoindex);
    photon_phoiso = phoPFPhoIso_RhoCor(phoindex);

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
    if (!(photon_sieie > 0.02 || photon_sieie < 0.015)) return;
    cutflow->Fill(11,event_weight);
    fillHistos(11,event_weight);
}

void monoJetClass::met_variation(int var,float event_weight) {
  // Vary pfMET Cut by 20%
  if ( pfMET >= (60 * ( 1 + var*0.2 )) ) return;
  if (!(photon_sieie > 0.02 || photon_sieie < 0.015)) return;
  switch(var) {
  case 1: fillHistos(12,event_weight); break;
  case -1:fillHistos(13,event_weight); break;
  }
}

void monoJetClass::sideband_variation(int var,float event_weight) {
  if ( pfMET >= 60 ) return;
  if (!(photon_sieie > (0.02 * ( 1 + var*0.1 )) || photon_sieie < 0.015)) return;
  switch(var) {
  case 1: fillHistos(14,event_weight); break;
  case -1:fillHistos(15,event_weight); break;
  }
}

void monoJetClass::BookHistos(const char* outputFilename) {
  output = new TFile(outputFilename, "RECREATE");
  output->cd();
  
  vector<string> cutlist = {s_TotalEvents,s_Triggers,s_METFilters,"Photon Selection",s_ElectronVeto,s_MuonVeto,
			    s_TauVeto,s_BJetVeto,s_minDPhiJetMET,s_JetSelection,"MET60","SigmaIEtaIEta Sideband"};
  cutflow = new Cutflow(this,cutlist);

  string bins[nPhoPtBins];
  for (int i = 0; i < nPhoPtBins; i++) {
    string low = to_string( (int)phoPtBins[i] );
    string high = to_string( (int)phoPtBins[i+1] );
    if ( i+1 == nPhoPtBins ) high = "Inf";
    bins[i] = low+"to"+high;
  }

  BookHistos(-1,"");
  for(int i = bHisto; i<nHisto; i++) {
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
    auto Name = [histname](string name) { return (name+histname); };
    h_phoPFIso[i] = MakeTH1F(new TH1F( Name("photonPFIso").c_str(),"Photon PF Iso",25,0,25));
    h_phoPFIsoSigmaIEtaIEta[i] = new TH2F( Name("photonPFIsoSigmaIEtaIEta").c_str(),"Photon Iso vs SigmaIEtaIEta;Photon PF Iso;Photon #sigma_{i#eta i#eta}",25,0,25,25,0.0,0.025);

    char name[100];
    for (int ibin = 0; ibin < nPhoPtBins; ibin++) {
      sprintf(name, "photonPt_%s", bins[ibin].c_str());
      h_phoPt_ptbins[i][ibin]  = MakeTH1F(new TH1F( Name(name).c_str(),"photon p_{T}", nPhoPtBins, phoPtBins));  
      
      sprintf(name, "photonPFIso_%s", bins[ibin].c_str());
      h_phoPFIso_ptbins[i][ibin] = MakeTH1F(new TH1F( Name(name).c_str(), "Photon PF Iso", 25, 0, 25));

      sprintf(name, "photonSigmaIEtaIEta_%s",bins[ibin].c_str());
      h_phoSigmaIEtaIEta_ptbins[i][ibin] = MakeTH1F(new TH1F( Name(name).c_str(),"Photon #sigma_#{i#eta i#eta}",25,0,0.025));
    }
    
    BookHistos(i,histname);
  }
}

void monoJetClass::fillHistos(int nhist,float event_weight) {
  monoJetYear::fillHistos(nhist,event_weight);
  monoJetGammaCR::fillHistos(nhist,event_weight);
  weight = event_weight;
  h_phoPFIso[nhist]->Fill(photon_phoiso, event_weight);
  h_phoPFIsoSigmaIEtaIEta[nhist]->Fill(photon_phoiso, photon_sieie , event_weight);

  for (int i = 0; i < nPhoPtBins; i++) {
    if (photon_pt >= phoPtBins[i] && photon_pt < phoPtBins[i+1]) {
      h_phoPt_ptbins[nhist][i]->Fill(photon_pt,event_weight);
      h_phoPFIso_ptbins[nhist][i]->Fill(photon_phoiso,event_weight);
      h_phoSigmaIEtaIEta_ptbins[nhist][i]->Fill(photon_sieie,event_weight);
    }
  }
  if (nhist == bHisto) tree->Fill();
}

int monoJetClass::getJetIndex(int phoindex) {

  for (int ijet = 0; ijet < nJet; ijet++) {
    bool id = (jetID->at(ijet)>>0&1) == 1;
    bool kinematics = (jetPt->at(ijet) > 100 && fabs(jetEta->at(ijet)) < 2.4);
    bool DR = deltaR(jetEta->at(ijet),jetPhi->at(ijet),phoSCEta->at(phoindex),phoSCPhi->at(phoindex)) > 0.4;
    bool clean = jetNHF->at(ijet) < 0.8 && jetCHF->at(ijet) > 0.1;

    if(id && kinematics && DR && clean)
      return ijet;
  }
  return -1;
}

int monoJetClass::getPhoIndex() {
  for (int ipho = 0; ipho < nPho; ipho++) {
    
    bool kinematics = ((phoCalibEt->at(ipho) > 230.0) && (fabs(phoSCEta->at(ipho)) < 1.4442));
    bool IdIso = CutBasedPFPhotonID_noPhoIso(ipho, "medium");

    if(kinematics && IdIso)
      return ipho;
  }

  //if (type == GJets && PFIso) return ipho;
  //else if (type == Data && !PFIso) return ipho;
  return -1;
}

bool monoJetClass::CutBasedPFPhotonID_noPhoIso(Int_t ipho, TString phoWP){

  bool photonId = false;

  if(phoWP == "tight"){  // Tight
    if( fabs((*phoSCEta)[ipho]) < 1.4442){ // EB
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.02148 ) &&
		  // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.00996 ) &&
		  ((*phoEleVeto)[ipho]                == 1) &&
		  (phoPFChIso_RhoCor(ipho) < 0.65 )  &&
		  (phoPFNeuIso_RhoCor(ipho) < (0.317 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
		  //                    (phoPFPhoIso_RhoCor(ipho) < (2.044 + (0.004017 * (*phoEt)[ipho])) )
		   ); 
    }
    if( fabs((*phoSCEta)[ipho]) > 1.4442){ // EE
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.0321 ) &&
		  // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0271 ) &&
		  ((*phoEleVeto)[ipho]                == 1) &&
		  (phoPFChIso_RhoCor(ipho) < 0.517 )  &&
		  (phoPFNeuIso_RhoCor(ipho) < (2.716 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
		  //                    (phoPFPhoIso_RhoCor(ipho) < (3.032 + (0.0037 * (*phoEt)[ipho])) )
		   );
    }
  }

  if(phoWP == "medium"){ // Medium
    if( fabs((*phoSCEta)[ipho]) < 1.4442){ // EB
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.02197 ) &&
		  // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.01015 ) &&
		  ((*phoEleVeto)[ipho]                == 1) &&
		  (phoPFChIso_RhoCor(ipho) < 1.141 )  &&
		  (phoPFNeuIso_RhoCor(ipho) < (1.189 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
		  //                    (phoPFPhoIso_RhoCor(ipho) < (2.08 + (0.004017 * (*phoEt)[ipho])) )
		   );
    }
    if( fabs((*phoSCEta)[ipho]) > 1.4442){ // EE
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.0326 ) &&
		  // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0272 ) &&
		  ((*phoEleVeto)[ipho]                == 1) &&
		  (phoPFChIso_RhoCor(ipho) < 1.051 )  &&
		  (phoPFNeuIso_RhoCor(ipho) < (2.718 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
		  //                    (phoPFPhoIso_RhoCor(ipho) < (3.867 + (0.0037 * (*phoEt)[ipho])) )
		   );
    }
  }

  if(phoWP == "loose"){ // Loose
    if( fabs((*phoSCEta)[ipho]) < 1.4442){ // EB
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.04596 ) &&
		  // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0106 ) &&
		  ((*phoEleVeto)[ipho]                == 1) &&
		  (phoPFChIso_RhoCor(ipho) < 1.694 )  &&
		  (phoPFNeuIso_RhoCor(ipho) < (24.032 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
		  //                    (phoPFPhoIso_RhoCor(ipho) < (2.876 + (0.004017 * (*phoEt)[ipho])) )
		   ); 
    }
    if( fabs((*phoSCEta)[ipho]) > 1.4442){ // EE
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.0590 ) &&
		  // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0272 ) &&
		  ((*phoEleVeto)[ipho]                == 1) &&
		  (phoPFChIso_RhoCor(ipho) < 2.089 )  &&
		  (phoPFNeuIso_RhoCor(ipho) < (19.722 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
		  //                    (phoPFPhoIso_RhoCor(ipho) < (4.162 + (0.0037 * (*phoEt)[ipho])) )
		   );
    }
  }
  return photonId; 
}

// Effective area to be needed in PF Iso for photon ID -- Fall2017v2 bx25ns
double monoJetClass::EAcharged(double eta){
  Float_t EffectiveArea = 0.0;
  if(fabs(eta) >= 0.0   && fabs(eta) < 1.0   ) EffectiveArea = 0.0112;
  if(fabs(eta) >= 1.0   && fabs(eta) < 1.479 ) EffectiveArea = 0.0108;
  if(fabs(eta) >= 1.479 && fabs(eta) < 2.0   ) EffectiveArea = 0.0106;
  if(fabs(eta) >= 2.0   && fabs(eta) < 2.2   ) EffectiveArea = 0.01002;
  if(fabs(eta) >= 2.2   && fabs(eta) < 2.3   ) EffectiveArea = 0.0098;
  if(fabs(eta) >= 2.3   && fabs(eta) < 2.4   ) EffectiveArea = 0.0089;
  if(fabs(eta) >= 2.4                        ) EffectiveArea = 0.0087;

  return EffectiveArea;
}

double monoJetClass::EAneutral(double eta){
  Float_t EffectiveArea = 0.;
  if(fabs(eta) >= 0.0   && fabs(eta) < 1.0   ) EffectiveArea = 0.0668;
  if(fabs(eta) >= 1.0   && fabs(eta) < 1.479 ) EffectiveArea = 0.1054;
  if(fabs(eta) >= 1.479 && fabs(eta) < 2.0   ) EffectiveArea = 0.0786;
  if(fabs(eta) >= 2.0   && fabs(eta) < 2.2   ) EffectiveArea = 0.0233;
  if(fabs(eta) >= 2.2   && fabs(eta) < 2.3   ) EffectiveArea = 0.0078;
  if(fabs(eta) >= 2.3   && fabs(eta) < 2.4   ) EffectiveArea = 0.0028;
  if(fabs(eta) >= 2.4                        ) EffectiveArea = 0.0137;

  return EffectiveArea;
}

double monoJetClass::EAphoton(double eta){
  Float_t EffectiveArea = 0.;
  if(fabs(eta) >= 0.0   && fabs(eta) < 1.0   ) EffectiveArea = 0.1113;
  if(fabs(eta) >= 1.0   && fabs(eta) < 1.479 ) EffectiveArea = 0.0953;
  if(fabs(eta) >= 1.479 && fabs(eta) < 2.0   ) EffectiveArea = 0.0619;
  if(fabs(eta) >= 2.0   && fabs(eta) < 2.2   ) EffectiveArea = 0.0837;
  if(fabs(eta) >= 2.2   && fabs(eta) < 2.3   ) EffectiveArea = 0.1070;
  if(fabs(eta) >= 2.3   && fabs(eta) < 2.4   ) EffectiveArea = 0.1212;
  if(fabs(eta) >= 2.4                        ) EffectiveArea = 0.1466;

  return EffectiveArea;
}
