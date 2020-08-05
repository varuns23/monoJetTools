#ifndef monoJet_TFR_C
#define monoJet_TFR_C

#include "monoJetTauFakeRate.h"
#include "VariableBins.h"
#include "Utilities.h"

using namespace std;

Region monoJetAnalysis::REGION = TFR;
CRobject monoJetAnalysis::CROBJECT = MuTau;

void monoJetTauFakeRate::initVars() {
  tauindex = muindex = -1;
  dilepton_mass = dilepton_pt = -1;
  tau_pt = tau_eta = tau_phi = -99;
  muon_pt = muon_eta = muon_phi = -99;
  tau_isoChMult = tau_isoNhMult = tau_isoPhMult = tau_isoPFCands = -1;
  tau_sigChMult = tau_sigNhMult = tau_sigPhMult = tau_sigPFCands = -1;
  tightID_sf = tightISO_sf = looseID_sf = looseISO_sf = 1;
  iso_category = -1;
}

void monoJetTauFakeRate::initTree(TTree* tree) {
  tree->Branch("dileptonM",&dilepton_mass);
  tree->Branch("dileptonPt",&dilepton_pt);
  tree->Branch("tauPt",&tau_pt);
  tree->Branch("tauEta",&tau_eta);
  tree->Branch("tauPhi",&tau_phi);
  tree->Branch("tauNumIsoPFCands",&tau_isoPFCands);
  tree->Branch("tauIsoChNemPtFrac",&tau_isoChNemPtFrac);
  tree->Branch("tauNumSigPFCands",&tau_sigPFCands);
  tree->Branch("tauSigChNemPtFrac",&tau_sigChNemPtFrac);
  tree->Branch("muonPt",&muon_pt);
  tree->Branch("muonEta",&muon_eta);
  tree->Branch("muonPhi",&muon_phi);
  tree->Branch("isoCategory",&iso_category);
  tree->Branch("tightID_sf",&tightID_sf);
  tree->Branch("tightISO_sf",&tightISO_sf);
  tree->Branch("looseID_sf",&looseID_sf);
  tree->Branch("looseISO_sf",&looseISO_sf);
}

void monoJetTauFakeRate::BookHistos(int i,TString histname) {
  if (i == -1) {
    
  } else {
    auto Name = [histname](TString name) { return (name + histname); };
    h_recoilall[i]->SetBins(50,0.0,800.0); // redefine binning
    h_pfMETall[i]->SetBins(50,0.0,800.0);
    h_caloMET[i]->SetBins(50,0.0,800.0);
    h_pfMETlow[i]  = MakeTH1F(new TH1F(Name("pfMETlow") ,"pfMET;E_{T}^{miss} [GeV]"    ,50,0.0,80.0));
    h_tauPt[i]     = MakeTH1F(new TH1F(Name("tauPt")    ,"tauPt;Tau P_{T} [GeV]"       ,50,0.0,1000.0));   
    h_tauEta[i]    = MakeTH1F(new TH1F(Name("tauEta")   ,"tauEta;Tau #eta"             ,nEtaBins,lEta,uEta));              
    h_tauPhi[i]    = MakeTH1F(new TH1F(Name("tauPhi")   ,"tauPhi;Tau #phi"             ,nPhiBins,lPhi,uPhi));
    h_tauIsoChMult[i] = MakeTH1F(new TH1F(Name("tauIsoChMult"),"tauIsoChMult;Tau Isolated Charged Hadron Multiplicity",20,0,20));
    h_tauIsoNhMult[i] = MakeTH1F(new TH1F(Name("tauIsoNhMult"),"tauIsoNhMult;Tau Isolated Neutral Hadron Multiplicity",20,0,20));
    h_tauIsoPhMult[i] = MakeTH1F(new TH1F(Name("tauIsoPhMult"),"tauIsoPhMult;Tau Isolated Photon Multiplicity",20,0,20));
    h_tauIsoPFCands[i] = MakeTH1F(new TH1F(Name("tauIsoPFCands"),"tauIsoPFCands;Tau Isolated PF Candidates",20,0,20));
    h_tauIsoChNemPt[i]        = MakeTH1F(new TH1F(Name("tauIsoChNemPt")      ,"tauIsoChNemPt;Ch + NEM Tau Isolation P_{T} [GeV]"               ,nPt123Bins,Pt123Bins));
    h_tauIsoChNemPt123[i]     = MakeTH1F(new TH1F(Name("tauIsoChNemPt123")   ,"tauIsoChNemPt123;Ch + NEM Tau Isolation P^{123}_{T} [GeV]"                  ,nPt123Bins,Pt123Bins));
    h_tauIsoChNemPtFrac[i]    = MakeTH1F(new TH1F(Name("tauIsoChNemPtFrac")  ,"tauIsoChNemPtFrac;Ch + NEM Tau Isolation P_{T}^{123} Fraction"              ,nPtFracBins,PtFracBins));
    h_tauIsoChNemPtFracV2[i]  = MakeTH1F(new TH1F(Name("tauIsoChNemPtFracV2"),"tauIsoChNemPtFrac;Ch + NEM Tau Isolation P_{T}^{123} Fraction"              ,50,0,1));
    h_tauIsoEtaWidth[i]     = MakeTH1F(new TH1F(Name("tauIsoEtaWidth")   ,"tauIsoEtaWidth; Isolation #eta width of Leading Tau"                  ,50,0,0.25));
    h_tauIsoPhiWidth[i]     = MakeTH1F(new TH1F(Name("tauIsoPhiWidth")   ,"tauIsoPhiWidth; Isolation #phi width of Leading Tau"                  ,50, 0,0.5));
    
    h_tauSigChMult[i] = MakeTH1F(new TH1F(Name("tauSigChMult"),"tauSigChMult;Tau Signal Charged Hadron Multiplicity",20,0,20));
    h_tauSigNhMult[i] = MakeTH1F(new TH1F(Name("tauSigNhMult"),"tauSigNhMult;Tau Signal Neutral Hadron Multiplicity",20,0,20));
    h_tauSigPhMult[i] = MakeTH1F(new TH1F(Name("tauSigPhMult"),"tauSigPhMult;Tau Signal Photon Multiplicity",20,0,20));
    h_tauSigPFCands[i] = MakeTH1F(new TH1F(Name("tauSigPFCands"),"tauSigPFCands;Tau Signal PF Candidates",20,0,20));
    h_tauSigChNemPt[i]        = MakeTH1F(new TH1F(Name("tauSigChNemPt")      ,"tauSigChNemPt;Ch + NEM Tau Signal P_{T} [GeV]"               ,nPt123Bins,Pt123Bins));
    h_tauSigChNemPt123[i]     = MakeTH1F(new TH1F(Name("tauSigChNemPt123")   ,"tauSigChNemPt123;Ch + NEM Tau Signal P^{123}_{T} [GeV]"                  ,nPt123Bins,Pt123Bins));
    h_tauSigChNemPtFrac[i]    = MakeTH1F(new TH1F(Name("tauSigChNemPtFrac")  ,"tauSigChNemPtFrac;Ch + NEM Tau Signal P_{T}^{123} Fraction"              ,nPtFracBins,PtFracBins));
    h_tauSigChNemPtFracV2[i]  = MakeTH1F(new TH1F(Name("tauSigChNemPtFracV2"),"tauSigChNemPtFrac;Ch + NEM Tau Signal P_{T}^{123} Fraction"              ,50,0,1));
    h_tauSigEtaWidth[i]     = MakeTH1F(new TH1F(Name("tauSigEtaWidth")   ,"tauSigEtaWidth; Signal #eta width of Leading Tau"                  ,50,0,0.25));
    h_tauSigPhiWidth[i]     = MakeTH1F(new TH1F(Name("tauSigPhiWidth")   ,"tauSigPhiWidth; Signal #phi width of Leading Tau"                  ,50, 0,0.5));
    
    h_muonPt[i]  = MakeTH1F(new TH1F(Name("muonPt") ,"muonPt;Muon P_{T} [GeV]" ,50,0.0,1000.0));
    h_muonEta[i] = MakeTH1F(new TH1F(Name("muonEta"),"muonEta;Muon #eta"       ,nEtaBins,lEta,uEta));              
    h_muonPhi[i] = MakeTH1F(new TH1F(Name("muonPhi"),"muonPhi;Muon #phi"       ,nPhiBins,lPhi,uPhi));        
    h_dileptonPt[i]          = MakeTH1F(new TH1F(Name("dileptonPt")         ,"dileptonPt;Z P_{T} [GeV]"                         ,30,0.,1500.));              
    h_dileptonM[i]           = MakeTH1F(new TH1F(Name("dileptonM")          ,"dileptonM;Z Mass [GeV]"                           ,24,60.,120.));
    h_dileptonMall[i]        = MakeTH1F(new TH1F(Name("dileptonMall")       ,"dileptonM;Z Mass [GeV]"                           ,50,30.,200.));
    h_isoCategory[i]      = MakeTH1F(new TH1F(Name("isoCategory")     ,"isoCategory;isoCategory"                               ,2,0,2));
    
    h_tauEtaPhi[i] = new TH2F(Name("tauEtaPhi")   ,"tauEtaPhi;Leading Tau #eta;Leading Tau #phi"             ,nEtaBins,lEta,uEta,nPhiBins,lPhi,uPhi);  
    h_muonEtaPhi[i] = new TH2F(Name("muonEtaPhi")   ,"muonEtaPhi;Muon #eta;Muon #phi"             ,nEtaBins,lEta,uEta,nPhiBins,lPhi,uPhi);      
  }
}

void monoJetTauFakeRate::fillHistos(int nhist,float event_weight) {
  h_pfMETlow[nhist]   ->Fill(pfMET,event_weight);
  if(tauindex >= 0 && muindex >= 0){ 
    h_tauPt[nhist]    ->Fill(tau_pt,event_weight);
    h_tauEta[nhist]   ->Fill(tau_eta,event_weight);
    h_tauPhi[nhist]   ->Fill(tau_phi,event_weight);

    h_tauIsoChMult[nhist]->Fill(tau_isoChMult,event_weight);
    h_tauIsoNhMult[nhist]->Fill(tau_isoNhMult,event_weight);
    h_tauIsoPhMult[nhist]->Fill(tau_isoPhMult,event_weight);
    h_tauIsoPFCands[nhist]->Fill(tau_isoPFCands,event_weight);
    
    h_tauIsoChNemPt[nhist]      ->Fill(tau_isoChNemPt,event_weight);
    h_tauIsoChNemPt123[nhist]   ->Fill(tau_isoChNemPt123,event_weight);
    h_tauIsoChNemPtFrac[nhist]  ->Fill(tau_isoChNemPtFrac,event_weight);
    h_tauIsoChNemPtFracV2[nhist]->Fill(tau_isoChNemPtFrac,event_weight);
    
    h_tauIsoEtaWidth[nhist]   ->Fill(tau_isoEtaWidth,event_weight);
    h_tauIsoPhiWidth[nhist]   ->Fill(tau_isoPhiWidth,event_weight);
    
    h_tauSigChMult[nhist]->Fill(tau_sigChMult,event_weight);
    h_tauSigNhMult[nhist]->Fill(tau_sigNhMult,event_weight);
    h_tauSigPhMult[nhist]->Fill(tau_sigPhMult,event_weight);
    h_tauSigPFCands[nhist]->Fill(tau_sigPFCands,event_weight);
    
    h_tauSigChNemPt[nhist]      ->Fill(tau_sigChNemPt,event_weight);
    h_tauSigChNemPt123[nhist]   ->Fill(tau_sigChNemPt123,event_weight);
    h_tauSigChNemPtFrac[nhist]  ->Fill(tau_sigChNemPtFrac,event_weight);
    h_tauSigChNemPtFracV2[nhist]->Fill(tau_sigChNemPtFrac,event_weight);
    
    h_tauSigEtaWidth[nhist]   ->Fill(tau_sigEtaWidth,event_weight);
    h_tauSigPhiWidth[nhist]   ->Fill(tau_sigPhiWidth,event_weight);
    
    h_muonPt[nhist] ->Fill(muon_pt,event_weight);
    h_muonEta[nhist]->Fill(muon_eta,event_weight);
    h_muonPhi[nhist]->Fill(muon_phi,event_weight);
    
    h_dileptonPt[nhist]         ->Fill(dilepton_pt,event_weight);
    h_dileptonM[nhist]          ->Fill(dilepton_mass,event_weight);
    h_dileptonMall[nhist]       ->Fill(dilepton_mass,event_weight);

    h_isoCategory[nhist]     ->Fill(iso_category,event_weight);
    
    h_tauEtaPhi[nhist]   ->Fill(tau_eta,tau_phi,event_weight);
    h_muonEtaPhi[nhist]->Fill(muon_eta,muon_phi,event_weight);
  }
}

bool monoJetTauFakeRate::CRSelection(vector<int> taulist,vector<int> mulist) {
  if (taulist.size() == 0) return false;
  for(int j=0; j<mulist.size(); ++j){
    //Event must have exactly two muons with opposite charge
    if(tau_Charge->at(taulist[0]) * muCharge->at(mulist[j]) == -1){
      float dr = deltaR(tau_Eta->at(taulist[0]),tau_Phi->at(taulist[0]),muEta->at(mulist[j]),muPhi->at(mulist[j]));
      if (dr > 0.5){
	tau.SetPtEtaPhiE(tau_Pt->at(taulist[0]), tau_Eta->at(taulist[0]), tau_Phi->at(taulist[0]), tau_Energy->at(taulist[0]));
	muon.SetPtEtaPhiE(muPt->at(mulist[j]), muEta->at(mulist[j]), muPhi->at(mulist[j]), muE->at(mulist[j]));
	tauindex    = taulist[0];
	muindex = mulist[j];
	
	TLorentzVector ll = tau + muon;
	dilepton_mass = ll.M();
	dilepton_pt = ll.Pt();
	
	tau_pt = tau.Pt();
	tau_eta = tau.Eta();
	tau_phi = tau.Phi();

	tau_isoChMult = tau_NumIsolationPFChargedHadrCands->at(tauindex);
	tau_isoNhMult = tau_NumIsolationPFNeutrHadrCands->at(tauindex);
	tau_isoPhMult = tau_NumIsolationPFGammaCands->at(tauindex);
	tau_isoPFCands = tau_NumIsolationPFCands->at(tauindex);
	
	tau_sigChMult = tau_NumSignalPFChargedHadrCands->at(tauindex);
	tau_sigNhMult = tau_NumSignalPFNeutrHadrCands->at(tauindex);
	tau_sigPhMult = tau_NumSignalPFGammaCands->at(tauindex);
	tau_sigPFCands = tau_NumSignalPFCands->at(tauindex);

	SetTauIsoPtFrac(tauindex);
	SetTauSigPtFrac(tauindex);
	
	muon_pt = muon.Pt();
	muon_eta = muon.Eta();
	muon_phi = muon.Phi();
	setRecoil();
	return true;
      }
    }
  }
  return false;
}

void monoJetTauFakeRate::setRecoil() {
  TLorentzVector ll = muon;
  TLorentzVector met_4vec;
  met_4vec.SetPtEtaPhiE(pfMET,0.,pfMETPhi,pfMET);
  TLorentzVector leptoMET_4vec = ll+met_4vec;
  recoil = fabs(leptoMET_4vec.Pt());
  recoilPhi = leptoMET_4vec.Phi();
  dpfcalo = dPFCaloMET(recoil);
}

bool monoJetTauFakeRate::isMedium(int tauindex) {
  return (tau_IDbits->at(tauindex)>>15&1) == 1; //tau_Pt->at(tauindex) > tauMediumPtCut && fabs(tau_Eta->at(tauindex)) < tauMediumEtaCut && 
}


float monoJetTauFakeRate::getSF(int leading,int subleading) {
  float leading_pt = tau_Pt->at(leading); float leading_abseta = fabs(tau_Eta->at(leading)); 
  float subleading_pt = tau_Pt->at(subleading); float subleading_abseta = fabs(tau_Eta->at(subleading));

  // tightID_sf = th2fmap.getBin("muon_id_tight",leading_pt,leading_abseta);
  // tightISO_sf = th2fmap.getBin("muon_iso_tight",leading_pt,leading_abseta);
  // looseID_sf = th2fmap.getBin("muon_id_loose",subleading_pt,subleading_abseta);
  // looseISO_sf = th2fmap.getBin("muon_iso_loose",subleading_pt,subleading_abseta);

  return tightID_sf * tightISO_sf * looseID_sf * looseISO_sf;
}

bool monoJetTauFakeRate::electron_veto() {
  vector<int> tmpcands = getLooseEle();
  return tmpcands.size() == 0;
}

bool monoJetTauFakeRate::photon_veto(int tauindex, int muindex){
  vector<int> pho_cands;
  pho_cands.clear();
  
  vector<int> tmpcands = getLoosePho();
  for(int i : tmpcands){
    double dR_tau    = deltaR(phoSCEta->at(i),phoSCPhi->at(i), tau_Eta->at(tauindex), tau_Phi->at(tauindex));  
    double dR_mu = deltaR(phoSCEta->at(i),phoSCPhi->at(i), muEta->at(muindex), muPhi->at(muindex)); 
    if( dR_tau > 0.5 && dR_mu > 0.5)
      pho_cands.push_back(i);
  }
  return pho_cands.size() == 0;
}

void monoJetTauFakeRate::SetTauIsoPtFrac(int tauindex) {
  tau_isoEtaWidth = tau_IsolationEtaWidth->at(tauindex);
  tau_isoPhiWidth = tau_IsolationPhiWidth->at(tauindex);

  tau_isoConsEt = tau_IsolationConstEt->at(tauindex);
  tau_isoConsPt = tau_IsolationConstPt->at(tauindex);
  tau_isoConsEta = tau_IsolationConstEta->at(tauindex);
  tau_isoConsPhi = tau_IsolationConstPhi->at(tauindex);
  tau_isoConsPID = tau_IsolationConstPdgId->at(tauindex);
  
  tau_isoChNemPtFrac=tau_isoChNemPt=tau_isoChNemPt123=0.0;
  float pfHadronPt[4];
  for (int i = 0; i < 4; i++){
    pfHadronPt[i] = 0.;
  }
  float first3_HadronPt[4] = {0,0,0,0};

  for (int i = 0; i < tau_isoConsPID.size(); i++) {
    int PID = tau_isoConsPID[i];
    int absPID = abs(PID);
    float consPt = tau_isoConsPt[i];

    if ( i < 3 ) {
      Pt123 += consPt;
      switch(absPID) {
      case Hadron::Charged: first3_HadronPt[0] += consPt; break;
      case Hadron::Neutral: first3_HadronPt[1] += consPt; break;
      case Hadron::Gamma:   first3_HadronPt[2] += consPt; break;
      default:              first3_HadronPt[3] += consPt; break;
      }
    }
    switch(absPID) {
    case Hadron::Charged: pfHadronPt[0] += consPt; break;
    case Hadron::Neutral: pfHadronPt[1] += consPt; break;
    case Hadron::Gamma:   pfHadronPt[2] += consPt; break;
    default:              pfHadronPt[3] += consPt; break;
    }
  }
  tau_isoChNemPt = pfHadronPt[0] + pfHadronPt[2];
  tau_isoChNemPt123 = first3_HadronPt[0] + first3_HadronPt[2];
  tau_isoChNemPtFrac = tau_isoChNemPt123/tau_isoChNemPt;
}

void monoJetTauFakeRate::SetTauSigPtFrac(int tauindex) {
  tau_sigEtaWidth = tau_SignalEtaWidth->at(tauindex);
  tau_sigPhiWidth = tau_SignalPhiWidth->at(tauindex);

  tau_sigConsEt = tau_SignalConstEt->at(tauindex);
  tau_sigConsPt = tau_SignalConstPt->at(tauindex);
  tau_sigConsEta = tau_SignalConstEta->at(tauindex);
  tau_sigConsPhi = tau_SignalConstPhi->at(tauindex);
  tau_sigConsPID = tau_SignalConstPdgId->at(tauindex);
  
  tau_sigChNemPtFrac=tau_sigChNemPt=tau_sigChNemPt123=0.0;
  float pfHadronPt[4];
  for (int i = 0; i < 4; i++){
    pfHadronPt[i] = 0.;
  }
  float first3_HadronPt[4] = {0,0,0,0};

  for (int i = 0; i < tau_sigConsPID.size(); i++) {
    int PID = tau_sigConsPID[i];
    int absPID = abs(PID);
    float consPt = tau_sigConsPt[i];

    if ( i < 3 ) {
      Pt123 += consPt;
      switch(absPID) {
      case Hadron::Charged: first3_HadronPt[0] += consPt; break;
      case Hadron::Neutral: first3_HadronPt[1] += consPt; break;
      case Hadron::Gamma:   first3_HadronPt[2] += consPt; break;
      default:              first3_HadronPt[3] += consPt; break;
      }
    }
    switch(absPID) {
    case Hadron::Charged: pfHadronPt[0] += consPt; break;
    case Hadron::Neutral: pfHadronPt[1] += consPt; break;
    case Hadron::Gamma:   pfHadronPt[2] += consPt; break;
    default:              pfHadronPt[3] += consPt; break;
    }
  }
  tau_sigChNemPt = pfHadronPt[0] + pfHadronPt[2];
  tau_sigChNemPt123 = first3_HadronPt[0] + first3_HadronPt[2];
  tau_sigChNemPtFrac = tau_sigChNemPt123/tau_sigChNemPt;
}

void monoJetTauFakeRate::Init(TTree* fChain) {
   tau_IsolationEtaWidth = 0;
   tau_IsolationPhiWidth = 0;
   tau_IsolationConstPt = 0;
   tau_IsolationConstEt = 0;
   tau_IsolationConstEta = 0;
   tau_IsolationConstPhi = 0;
   tau_IsolationConstPdgId = 0;
   tau_SignalEtaWidth = 0;
   tau_SignalPhiWidth = 0;
   tau_SignalConstPt = 0;
   tau_SignalConstEt = 0;
   tau_SignalConstEta = 0;
   tau_SignalConstPhi = 0;
   tau_SignalConstPdgId = 0;

   fChain->SetBranchAddress("tau_IsolationEtaWidth", &tau_IsolationEtaWidth, &b_tau_IsolationEtaWidth);
   fChain->SetBranchAddress("tau_IsolationPhiWidth", &tau_IsolationPhiWidth, &b_tau_IsolationPhiWidth);
   fChain->SetBranchAddress("tau_IsolationConstPt", &tau_IsolationConstPt, &b_tau_IsolationConstPt);
   fChain->SetBranchAddress("tau_IsolationConstEt", &tau_IsolationConstEt, &b_tau_IsolationConstEt);
   fChain->SetBranchAddress("tau_IsolationConstEta", &tau_IsolationConstEta, &b_tau_IsolationConstEta);
   fChain->SetBranchAddress("tau_IsolationConstPhi", &tau_IsolationConstPhi, &b_tau_IsolationConstPhi);
   fChain->SetBranchAddress("tau_IsolationConstPdgId", &tau_IsolationConstPdgId, &b_tau_IsolationConstPdgId);
   fChain->SetBranchAddress("tau_SignalEtaWidth", &tau_SignalEtaWidth, &b_tau_SignalEtaWidth);
   fChain->SetBranchAddress("tau_SignalPhiWidth", &tau_SignalPhiWidth, &b_tau_SignalPhiWidth);
   fChain->SetBranchAddress("tau_SignalConstPt", &tau_SignalConstPt, &b_tau_SignalConstPt);
   fChain->SetBranchAddress("tau_SignalConstEt", &tau_SignalConstEt, &b_tau_SignalConstEt);
   fChain->SetBranchAddress("tau_SignalConstEta", &tau_SignalConstEta, &b_tau_SignalConstEta);
   fChain->SetBranchAddress("tau_SignalConstPhi", &tau_SignalConstPhi, &b_tau_SignalConstPhi);
   fChain->SetBranchAddress("tau_SignalConstPdgId", &tau_SignalConstPdgId, &b_tau_SignalConstPdgId);
}

#endif
