#ifndef monoJetGammaPurity_C
#define monoJetGammaPurity_C
#include "monoJetGammaPurity.h"
#include "Utilities.h"

using namespace std;

void monoJetGammaPurity::initTree(TTree* tree) {
  monoJetYear::initTree(tree);
  monoJetGammaCR::initTree(tree);
  tree->Branch("photonPt",&photon_pt);
  tree->Branch("photonSieie",&photon_sieie);
  tree->Branch("phoPFIso",&photon_phoiso);
}

void monoJetGammaPurity::initVars() {
  monoJetYear::initVars();
  monoJetGammaCR::initVars();
  photon_phoiso = photon_sieie = -1;
}

void monoJetGammaPurity::BookHistos(int i,string histname) {
  monoJetYear::BookHistos(i,histname);
  monoJetGammaCR::BookHistos(i,histname);
  if (i == -1) {
    for (int i = 0; i < nPhoPtBins; i++) {
      string low = to_string( (int)phoPtBins[i] );
      string high = to_string( (int)phoPtBins[i+1] );
      if ( i+1 == nPhoPtBins ) high = "Inf";
      PtBinNames[i] = low+"to"+high;
    }
    return;
  }
  
  auto Name = [histname](string name) { return (name+histname); };
  h_phoSieie[i] = MakeTH1F(new TH1F(Name("photonSieie").c_str(),"PhotonSieie;Photon #sigma_{i#eta i#eta}",25,0,0.025));
  h_phoPFIso[i] = MakeTH1F(new TH1F( Name("photonPFIso").c_str(),"Photon PF Iso",25,0,25));
  h_phoPFIsoSieie[i] = new TH2F( Name("photonPFIsoSieie").c_str(),"Photon Iso vs Sieie;Photon PF Iso;Photon #sigma_{i#eta i#eta}",25,0,25,25,0.0,0.025);

  char name[100];
  for (int ibin = 0; ibin < nPhoPtBins; ibin++) {
    sprintf(name, "photonPt_%s", PtBinNames[ibin].c_str());
    h_phoPt_ptbins[i][ibin]  = MakeTH1F(new TH1F( Name(name).c_str(),"photon p_{T}", nPhoPtBins, phoPtBins));  
    
    sprintf(name, "photonPFIso_%s", PtBinNames[ibin].c_str());
    h_phoPFIso_ptbins[i][ibin] = MakeTH1F(new TH1F( Name(name).c_str(), "Photon PF Iso", 25, 0, 25));
    
    sprintf(name, "photonSieie_%s",PtBinNames[ibin].c_str());
    h_phoSieie_ptbins[i][ibin] = MakeTH1F(new TH1F( Name(name).c_str(),"Photon #sigma_#{i#eta i#eta}",25,0,0.025));
  }
}

void monoJetGammaPurity::fillHistos(int nhist,float event_weight) {
  monoJetAnalysis::fillHistos(nhist,event_weight);
  
  h_phoPFIso[nhist]->Fill(photon_phoiso, event_weight);
  h_phoSieie[nhist]->Fill(photon_sieie, event_weight);
  h_phoPFIsoSieie[nhist]->Fill(photon_phoiso, photon_sieie , event_weight);

  for (int i = 0; i < nPhoPtBins; i++) {
    if (photon_pt >= phoPtBins[i] && photon_pt < phoPtBins[i+1]) {
      h_phoPt_ptbins[nhist][i]->Fill(photon_pt,event_weight);
      h_phoPFIso_ptbins[nhist][i]->Fill(photon_phoiso,event_weight);
      h_phoSieie_ptbins[nhist][i]->Fill(photon_sieie,event_weight);
    }
  }
}

void monoJetGammaPurity::setPhoton(int phoindex) {
  photon_pt = phoCalibEt->at(phoindex);
  photon_eta = phoEta->at(phoindex);
  photon_phi = phoPhi->at(phoindex);
  photon_sieie = phoSigmaIEtaIEtaFull5x5->at(phoindex);
  photon_phoiso = phoPFPhoIso_RhoCor(phoindex);
}

int monoJetGammaPurity::getJetIndex(int phoindex) {

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

bool monoJetGammaPurity::CutBasedPhotonID(Int_t ipho, TString phoWP){

  bool photonId = false;
  bool inBarrel = fabs((*phoSCEta)[ipho]) < 1.4442;

  if(phoWP == "tight"){  // Tight
    if( inBarrel ){ // EB
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.02148 ) &&
		  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.00996 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   ); 
    } else { // EE
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.0321 ) &&
		  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0271 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   );
    }
  }

  if(phoWP == "medium"){ // Medium
    if( inBarrel ){ // EB
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.02197 ) &&
		  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.01015 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   );
    } else { // EE
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.0326 ) &&
		  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0272 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   );
    }
  }

  if(phoWP == "loose"){ // Loose
    if( inBarrel ){ // EB
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.04596 ) &&
		  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0106 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   ); 
    } else { // EE
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.0590 ) &&
		  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0272 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   );
    }
  }
  return photonId; 
}

bool monoJetGammaPurity::CutBasedPhotonID_noSieie(Int_t ipho, TString phoWP){

  bool photonId = false;
  bool inBarrel = fabs((*phoSCEta)[ipho]) < 1.4442;

  if(phoWP == "tight"){  // Tight
    if( inBarrel ){ // EB
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.02148 ) &&
		  // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.00996 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   ); 
    } else { // EE
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.0321 ) &&
		  // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0271 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   );
    }
  }

  if(phoWP == "medium"){ // Medium
    if( inBarrel ){ // EB
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.02197 ) &&
		  // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.01015 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   );
    } else { // EE
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.0326 ) &&
		  // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0272 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   );
    }
  }

  if(phoWP == "loose"){ // Loose
    if( inBarrel ){ // EB
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.04596 ) &&
		  // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0106 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   ); 
    } else { // EE
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.0590 ) &&
		  // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0272 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   );
    }
  }
  return photonId; 
}

bool monoJetGammaPurity::CutBasedPhotonID_invSieie(Int_t ipho, TString phoWP){

  bool photonId = false;
  bool inBarrel = fabs((*phoSCEta)[ipho]) < 1.4442;

  if(phoWP == "tight"){  // Tight
    if( inBarrel ){ // EB
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.02148 ) &&
		  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  >  0.00996 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   ); 
    } else { // EE
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.0321 ) &&
		  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  >  0.0271 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   );
    }
  }

  if(phoWP == "medium"){ // Medium
    if( inBarrel ){ // EB
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.02197 ) &&
		  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  >  0.01015 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   );
    } else { // EE
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.0326 ) &&
		  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  >  0.0272 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   );
    }
  }

  if(phoWP == "loose"){ // Loose
    if( inBarrel ){ // EB
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.04596 ) &&
		  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  >  0.0106 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   ); 
    } else { // EE
      photonId = ( 
		  ((*phoHoverE)[ipho]                <  0.0590 ) &&
		  ((*phoSigmaIEtaIEtaFull5x5)[ipho]  >  0.0272 ) &&
		  ((*phoEleVeto)[ipho]                == 1)
		   );
    }
  }
  return photonId; 
}

bool monoJetGammaPurity::CutBasedPhotonIso(Int_t ipho,TString phoWP) {

  bool photonIso = false;
  bool inBarrel = fabs((*phoSCEta)[ipho]) < 1.4442;

  if(phoWP == "tight") {  // Tight
    if( inBarrel ){ // EB
      photonIso = ( 
		   (phoPFChIso_RhoCor(ipho) <   0.65 )  &&
		   (phoPFNeuIso_RhoCor(ipho) < (0.317 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
		   && (phoPFPhoIso_RhoCor(ipho) < (2.044 + (0.004017 * (*phoEt)[ipho])))
		    );
    } else { // EE
      photonIso = ( 
		   (phoPFChIso_RhoCor(ipho) <   0.517 )  &&
		   (phoPFNeuIso_RhoCor(ipho) < (2.716 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
		   && (phoPFPhoIso_RhoCor(ipho) < (3.032 + (0.0037 * (*phoEt)[ipho])))
		    );
    }
  }

  if(phoWP == "medium") { // Medium
    if( inBarrel ){ // EB
      photonIso = ( 
		   (phoPFChIso_RhoCor(ipho) <   1.141 )  &&
		   (phoPFNeuIso_RhoCor(ipho) < (1.189 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
		   && (phoPFPhoIso_RhoCor(ipho) < (2.08 + (0.004017 * (*phoEt)[ipho])))
		    );
    } else { // EE
      photonIso = ( 
		   (phoPFChIso_RhoCor(ipho) <   1.051 )  &&
		   (phoPFNeuIso_RhoCor(ipho) < (2.718 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
		   && (phoPFPhoIso_RhoCor(ipho) < (3.867 + (0.0037 * (*phoEt)[ipho])))
		    );
    }
  }

  if(phoWP == "loose") { // Loose
    if( inBarrel ){ // EB
      photonIso = ( 
		   (phoPFChIso_RhoCor(ipho) <   1.694 )  &&
		   (phoPFNeuIso_RhoCor(ipho) < (24.032 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
		   && (phoPFPhoIso_RhoCor(ipho) < (2.876 + (0.004017 * (*phoEt)[ipho])))
		    );
    } else { // EE
      photonIso = ( 
		   (phoPFChIso_RhoCor(ipho) <   2.089 )  &&
		   (phoPFNeuIso_RhoCor(ipho) < (19.722 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
		   && (phoPFPhoIso_RhoCor(ipho) < (4.162 + (0.0037 * (*phoEt)[ipho])))
		    );
    }
  }

  if(phoWP == "veryloose") { // Loose
    if( inBarrel ){ // EB
      photonIso = ( 
		   (phoPFChIso_RhoCor(ipho) <     5*1.694 )  &&
		   (phoPFNeuIso_RhoCor(ipho) <    5*(24.032 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
		   && (phoPFPhoIso_RhoCor(ipho) < 5*(2.876 + (0.004017 * (*phoEt)[ipho])))
		    );
    } else { // EE
      photonIso = ( 
		   (phoPFChIso_RhoCor(ipho) <     5*2.089 )  &&
		   (phoPFNeuIso_RhoCor(ipho) <    5*(19.722 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
		   && (phoPFPhoIso_RhoCor(ipho) < 5*(4.162 + (0.0037 * (*phoEt)[ipho])))
		    );
    }
  }
  return photonIso;
}
bool monoJetGammaPurity::CutBasedPhotonIso_noPhoIso(Int_t ipho,TString phoWP) {

  bool photonIso = false;
  bool inBarrel = fabs((*phoSCEta)[ipho]) < 1.4442;

  if(phoWP == "tight") {  // Tight
    if( inBarrel ){ // EB
      photonIso = ( 
		   (phoPFChIso_RhoCor(ipho) <   0.65 )  &&
		   (phoPFNeuIso_RhoCor(ipho) < (0.317 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
		   // && (phoPFPhoIso_RhoCor(ipho) < (2.044 + (0.004017 * (*phoEt)[ipho])))
		    );
    } else { // EE
      photonIso = ( 
		   (phoPFChIso_RhoCor(ipho) <   0.517 )  &&
		   (phoPFNeuIso_RhoCor(ipho) < (2.716 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
		   // && (phoPFPhoIso_RhoCor(ipho) < (3.032 + (0.0037 * (*phoEt)[ipho])))
		    );
    }
  }

  if(phoWP == "medium") { // Medium
    if( inBarrel ){ // EB
      photonIso = ( 
		   (phoPFChIso_RhoCor(ipho) <   1.141 )  &&
		   (phoPFNeuIso_RhoCor(ipho) < (1.189 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
		   // && (phoPFPhoIso_RhoCor(ipho) < (2.08 + (0.004017 * (*phoEt)[ipho])))
		    );
    } else { // EE
      photonIso = ( 
		   (phoPFChIso_RhoCor(ipho) <   1.051 )  &&
		   (phoPFNeuIso_RhoCor(ipho) < (2.718 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
		   // && (phoPFPhoIso_RhoCor(ipho) < (3.867 + (0.0037 * (*phoEt)[ipho])))
		    );
    }
  }

  if(phoWP == "loose") { // Loose
    if( inBarrel ){ // EB
      photonIso = ( 
		   (phoPFChIso_RhoCor(ipho) <   1.694 )  &&
		   (phoPFNeuIso_RhoCor(ipho) < (24.032 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
		   // && (phoPFPhoIso_RhoCor(ipho) < (2.876 + (0.004017 * (*phoEt)[ipho])))
		    );
    } else { // EE
      photonIso = ( 
		   (phoPFChIso_RhoCor(ipho) <   2.089 )  &&
		   (phoPFNeuIso_RhoCor(ipho) < (19.722 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
		   // && (phoPFPhoIso_RhoCor(ipho) < (4.162 + (0.0037 * (*phoEt)[ipho])))
		    );
    }
  }

  if(phoWP == "veryloose") { // Loose
    if( inBarrel ){ // EB
      photonIso = ( 
		   (phoPFChIso_RhoCor(ipho) <     5*1.694 )  &&
		   (phoPFNeuIso_RhoCor(ipho) <    5*(24.032 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
		   // && (phoPFPhoIso_RhoCor(ipho) < 5*(2.876 + (0.004017 * (*phoEt)[ipho])))
		    );
    } else { // EE
      photonIso = ( 
		   (phoPFChIso_RhoCor(ipho) <     5*2.089 )  &&
		   (phoPFNeuIso_RhoCor(ipho) <    5*(19.722 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
		   // && (phoPFPhoIso_RhoCor(ipho) < 5*(4.162 + (0.0037 * (*phoEt)[ipho])))
		    );
    }
  }
  return photonIso;
}

// Effective area to be needed in PF Iso for photon ID -- Fall2017v2 bx25ns
double monoJetGammaPurity::EAcharged(double eta){
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

double monoJetGammaPurity::EAneutral(double eta){
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

double monoJetGammaPurity::EAphoton(double eta){
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
#endif
