//For use with Ntuples made from JetAnalyzer
////Required arguments: 1 is folder containing input files, 2 is output file path, 3 is maxEvents (-1 to run over all events), 4 is reportEvery
////
////To compile using rootcom to an executable named 'analyze':
////$ ./rootcom monoJetClass analyze
////
////To run, assuming this is compiled to an executable named 'analyze':
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

    initVars();
    
    float event_weight = 1.;
    if (isMC) {
      ApplyPileup(event_weight);
    }
    
    h_CutFlow->Fill(0.5,event_weight);

    if (!getPhotonTrigger()) continue;
    h_CutFlow->Fill(1.5,event_weight);
    if (!getMetFilter()) continue;
    h_CutFlow->Fill(2.5,event_weight);

    phoindex = getPhoIndex();
    if(phoindex < 0) continue;
    h_CutFlow->Fill(3.5,event_weight);

    if (!electron_veto(phoindex)) continue;
    h_CutFlow->Fill(4.5,event_weight);

    if (!muon_veto(phoindex)) continue;
    h_CutFlow->Fill(5.5,event_weight);

    if (!tau_veto(phoindex)) continue;
    h_CutFlow->Fill(6.5,event_weight);

    if (!bjet_veto( bjetDeepCSVCut_2017)) continue;
    h_CutFlow->Fill(7.5,event_weight);

    pho.SetPtEtaPhiE(phoCalibEt->at(phoindex),phoEta->at(phoindex),phoPhi->at(phoindex),phoCalibE->at(phoindex));
    setRecoil();
    vector<int> jetlist = getLooseJet();
    float mindPhiJetMET = dPhiJetMETmin(jetlist,recoilPhi);
    
    if (mindPhiJetMET <= dPhiJetMETCut) continue;
    h_CutFlow->Fill(8.5,event_weight);

    jetindex = getJetIndex(phoindex);
    if (jetindex < 0) continue; 
    h_CutFlow->Fill(9.5,event_weight);
    
    if (pfMET >= 60) continue;
    h_CutFlow->Fill(10.5,event_weight);

    photon_pt = phoCalibEt->at(phoindex);
    photon_eta = phoEta->at(phoindex);
    photon_phi = phoPhi->at(phoindex);
    photon_sieie = phoSigmaIEtaIEtaFull5x5->at(phoindex);

    fillHistos(1, event_weight);

    if (jentry%reportEvery == 0){
      cout<<"Finished entry "<<jentry<<"/"<<(nentriesToCheck-1)<<endl;
    }
  }

}//Closing the Loop function

void monoJetClass::BookHistos(const char* outputFilename) {

  output = new TFile(outputFilename, "RECREATE");
  output->cd();

  BookHistos(1, "dummy");

  output->Write();
  output->Close();
}

void monoJetClass::fillHistos(int, float event_weight) {
  output->cd();
  
  double photon_PhoIso = TMath::Max(((*phoPFPhoIso)[phoindex] - rho*EAphoton( (*phoSCEta)[phoindex])), 0.0);

  tree->Fill();


  h_phoPt-> Fill(photon_pt, event_weight);
  h_phoPFIso->Fill(photon_PhoIso, event_weight);
  h_phoSigmaIeIe->Fill(photon_sieie, event_weight);
  h_PhoPFiso_SigmaIeIe->Fill(photon_PhoIso, photon_sieie , event_weight);

  if(photon_pt >= 230. && photon_pt < 250.){
    h_phoPt_ptbins[0]     ->Fill(photon_pt, event_weight);
    h_phoPFIso_ptbins[0]  ->Fill(photon_PhoIso, event_weight);
  }

  if(photon_pt >= 250. && photon_pt < 280.){
    h_phoPt_ptbins[1]     ->Fill(photon_pt, event_weight);
    h_phoPFIso_ptbins[1]  ->Fill(photon_PhoIso, event_weight);
  }

  if(photon_pt >= 280. && photon_pt < 320.){
    h_phoPt_ptbins[2]     ->Fill(photon_pt, event_weight);
    h_phoPFIso_ptbins[2]  ->Fill(photon_PhoIso, event_weight);
  }

  if(photon_pt >= 320. && photon_pt < 375.){
    h_phoPt_ptbins[3]     ->Fill(photon_pt, event_weight);
    h_phoPFIso_ptbins[3]  ->Fill(photon_PhoIso, event_weight);
  }

  if(photon_pt >= 375. && photon_pt < 425.){
    h_phoPt_ptbins[4]     ->Fill(photon_pt, event_weight);
    h_phoPFIso_ptbins[4]  ->Fill(photon_PhoIso, event_weight);
  }

  if(photon_pt >= 425. && photon_pt < 475.){
    h_phoPt_ptbins[5]     ->Fill(photon_pt, event_weight);
    h_phoPFIso_ptbins[5]  ->Fill(photon_PhoIso, event_weight);
  }

  if(photon_pt >= 475. && photon_pt < 550.){
    h_phoPt_ptbins[6]     ->Fill(photon_pt, event_weight);
    h_phoPFIso_ptbins[6]  ->Fill(photon_PhoIso, event_weight);
  }

  if(photon_pt >= 550. && photon_pt < 2000.){
    h_phoPt_ptbins[7]     ->Fill(photon_pt, event_weight);
    h_phoPFIso_ptbins[7]  ->Fill(photon_PhoIso, event_weight);
  }

}

void monoJetClass::BookHistos(int, string){
  char name[100];

  const int nPhoPtBins = 8;                
  const float phoPtBins[nPhoPtBins+1] = {230., 250., 280., 320., 375., 425., 475., 550., 2000.};

  std::string bins[nPhoPtBins] = {"230to250", "250to280", "280to320", "320to375", "375to425", "425to475", "475to550", "550toInf"};

  for( int hist = 0; hist < nPhoPtBins; ++hist){
    sprintf(name, "h_phoPt_%s", bins[hist].c_str());
    h_phoPt_ptbins[hist]  = new TH1F(name,"photon p_{T}", nPhoPtBins, phoPtBins);  
    h_phoPt_ptbins[hist]->Sumw2();

    sprintf(name, "h_phoPFIso_%s", bins[hist].c_str());
    h_phoPFIso_ptbins[hist] = new TH1F(name, "Photon PF Iso", 25, 0, 25);
    h_phoPFIso_ptbins[hist]->Sumw2();
  }

  h_phoPt    = new TH1F("h_phoPt", "photon p_{T}",  nPhoPtBins, phoPtBins);
  h_phoPt->Sumw2();
  h_phoPFIso = new TH1F("h_phoPFIso", "Photon PF Iso", 25, 0, 25);
  h_phoPFIso->Sumw2();
  h_phoSigmaIeIe = new TH1F("h_phoSigmaIeIe", "Photon SigmaIeIe", 25, 0., 0.025);
  h_phoSigmaIeIe->Sumw2();
  h_PhoPFiso_SigmaIeIe = new TH2F("h_PhoPFiso_SigmaIeIe", "Pho Iso vs SigmaIeIe", 25, 0, 25, 25, 0.0, 0.025);
  h_PhoPFiso_SigmaIeIe->Sumw2();

  h_CutFlow = new TH1F("h_CutFlow", "cut flow of selection", 11, 0, 11); 
  TString cutFlowLabel[11] = {"Total Events","Triggers","MET Filters","Photon Selection", "Ele veto", "Mu veto", "Tau veto", "bjet veto", "MinDPhi", "Jet Selection","MET60"};
  for( Int_t bin = 1; bin <= h_CutFlow->GetNbinsX(); ++bin){
    h_CutFlow->GetXaxis()->SetBinLabel(bin, cutFlowLabel[bin-1]);
  }
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
	  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.00996 ) &&
          ((*phoEleVeto)[ipho]                == 1) &&
	  (TMath::Max(((*phoPFChIso)[ipho]  - rho*EAcharged((*phoSCEta)[ipho])), 0.0) < 0.65 )  &&
	  (TMath::Max(((*phoPFNeuIso)[ipho] - rho*EAneutral((*phoSCEta)[ipho])), 0.0) < (0.317 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
	  //                    (TMath::Max(((*phoPFPhoIso)[ipho] - rho*EAphoton( (*phoSCEta)[ipho])), 0.0) < (2.044 + (0.004017 * (*phoEt)[ipho])) )
	  ); 
    }
    if( fabs((*phoSCEta)[ipho]) > 1.4442){ // EE
      photonId = ( 
	  ((*phoHoverE)[ipho]                <  0.0321 ) &&
	  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0271 ) &&
          ((*phoEleVeto)[ipho]                == 1) &&
	  (TMath::Max(((*phoPFChIso)[ipho]  - rho*EAcharged((*phoSCEta)[ipho])), 0.0) < 0.517 )  &&
	  (TMath::Max(((*phoPFNeuIso)[ipho] - rho*EAneutral((*phoSCEta)[ipho])), 0.0) < (2.716 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
	  //                    (TMath::Max(((*phoPFPhoIso)[ipho] - rho*EAphoton( (*phoSCEta)[ipho])), 0.0) < (3.032 + (0.0037 * (*phoEt)[ipho])) )
	  );
    }
  }

  if(phoWP == "medium"){ // Medium
    if( fabs((*phoSCEta)[ipho]) < 1.4442){ // EB
      photonId = ( 
	  ((*phoHoverE)[ipho]                <  0.02197 ) &&
	  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.01015 ) &&
          ((*phoEleVeto)[ipho]                == 1) &&
	  (TMath::Max(((*phoPFChIso)[ipho]  - rho*EAcharged((*phoSCEta)[ipho])), 0.0) < 1.141 )  &&
	  (TMath::Max(((*phoPFNeuIso)[ipho] - rho*EAneutral((*phoSCEta)[ipho])), 0.0) < (1.189 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
	  //                    (TMath::Max(((*phoPFPhoIso)[ipho] - rho*EAphoton( (*phoSCEta)[ipho])), 0.0) < (2.08 + (0.004017 * (*phoEt)[ipho])) )
	  );
    }
    if( fabs((*phoSCEta)[ipho]) > 1.4442){ // EE
      photonId = ( 
	  ((*phoHoverE)[ipho]                <  0.0326 ) &&
	  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0272 ) &&
          ((*phoEleVeto)[ipho]                == 1) &&
	  (TMath::Max(((*phoPFChIso)[ipho]  - rho*EAcharged((*phoSCEta)[ipho])), 0.0) < 1.051 )  &&
	  (TMath::Max(((*phoPFNeuIso)[ipho] - rho*EAneutral((*phoSCEta)[ipho])), 0.0) < (2.718 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
	  //                    (TMath::Max(((*phoPFPhoIso)[ipho] - rho*EAphoton( (*phoSCEta)[ipho])), 0.0) < (3.867 + (0.0037 * (*phoEt)[ipho])) )
	  );
    }
  }

  if(phoWP == "loose"){ // Loose
    if( fabs((*phoSCEta)[ipho]) < 1.4442){ // EB
      photonId = ( 
	  ((*phoHoverE)[ipho]                <  0.04596 ) &&
	  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0106 ) &&
          ((*phoEleVeto)[ipho]                == 1) &&
	  (TMath::Max(((*phoPFChIso)[ipho]  - rho*EAcharged((*phoSCEta)[ipho])), 0.0) < 1.694 )  &&
	  (TMath::Max(((*phoPFNeuIso)[ipho] - rho*EAneutral((*phoSCEta)[ipho])), 0.0) < (24.032 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
	  //                    (TMath::Max(((*phoPFPhoIso)[ipho] - rho*EAphoton( (*phoSCEta)[ipho])), 0.0) < (2.876 + (0.004017 * (*phoEt)[ipho])) )
	  ); 
    }
    if( fabs((*phoSCEta)[ipho]) > 1.4442){ // EE
      photonId = ( 
	  ((*phoHoverE)[ipho]                <  0.0590 ) &&
	  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0272 ) &&
          ((*phoEleVeto)[ipho]                == 1) &&
	  (TMath::Max(((*phoPFChIso)[ipho]  - rho*EAcharged((*phoSCEta)[ipho])), 0.0) < 2.089 )  &&
	  (TMath::Max(((*phoPFNeuIso)[ipho] - rho*EAneutral((*phoSCEta)[ipho])), 0.0) < (19.722 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
	  //                    (TMath::Max(((*phoPFPhoIso)[ipho] - rho*EAphoton( (*phoSCEta)[ipho])), 0.0) < (4.162 + (0.0037 * (*phoEt)[ipho])) )
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
