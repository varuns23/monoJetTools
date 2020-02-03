#ifndef monoJetAnalysis_C
#define monoJetAnalysis_C

#include <stdexcept>

#include "monoJetAnalysis.h"
#include "monoJetEnums.h"
#include "VariableBins.h"
#include "Utilities.h"

using namespace std;

const string monoJetAnalysis::REGION = "SignalRegion";

void monoJetAnalysis::SetScalingHistos() {
  //This is the PU histogram obtained from Nick's recipe
  TFile *weights = TFile::Open("RootFiles/PU_Central.root");
  TH1F* PU = (TH1F*)weights->Get("pileup");
  th1fmap["PU"] = PU;
  if (isW_or_ZJet()) {
    //This is the root file with EWK Corrections
    TFile* f_nlo_qcd = TFile::Open("RootFiles/2017_gen_v_pt_stat1_qcd_sf.root");
    TFile* f_nnlo_qcd = TFile::Open("RootFiles/lindert_qcd_nnlo_sf.root");

    TH1F *NNLO_QCD,*NLO_QCD,*NLO_EWK;
    if ( sample.type == WJets ) {
      NLO_EWK = (TH1F*)TFile::Open("RootFiles/merged_kfactors_wjets.root")->Get("kfactor_monojet_ewk");
      NLO_QCD = (TH1F*)f_nlo_qcd->Get("wjet_dilep");
      NNLO_QCD = (TH1F*)f_nnlo_qcd->Get("evj");
    } else {
      NLO_EWK = (TH1F*)TFile::Open("RootFiles/merged_kfactors_zjets.root")->Get("kfactor_monojet_ewk");
      NLO_QCD = (TH1F*)f_nlo_qcd->Get("dy_dilep");
      NNLO_QCD = (TH1F*)f_nnlo_qcd->Get("eej");
    }
    th1fmap["NNLO_QCD"] = NNLO_QCD;
    th1fmap["NLO_QCD"] = NLO_QCD;
    th1fmap["NLO_EWK"] = NLO_EWK;
  }
}

void monoJetAnalysis::initTree(TTree* tree) {
  tree->Branch("weight",&weight);
  tree->Branch("genWeight",&genWeight);
  tree->Branch("pileup",&pileup);
  tree->Branch("sf",&sf);
  tree->Branch("kfactor",&kfactor);
  tree->Branch("recoil",&recoil,"Recoil (GeV)");
  tree->Branch("j1pT",&j1pT,"Leading Jet P_{T} (GeV)");
  tree->Branch("j1Eta",&j1Eta,"Leading Jet Eta");
  tree->Branch("j1Phi",&j1Phi,"Leading Jet Phi");
  tree->Branch("nJets",&n_Jet,"Number of Jets");
  tree->Branch("bosonPt",&bosonPt,"Boson Pt");
  tree->Branch("nVtx",&n_Vtx,"Number of Verticies");
}

void monoJetAnalysis::BookHistos(int i,string histname) {
  if (i == -1) {
    h_metfilters = new TH1F("h_metfilters","metFilters",8,0.5,8.5); h_metfilters->Sumw2();
    h_metcut  = MakeTH1F(new TH1F("h_metcut","h_metcut; |pfMET-caloMET|/pfMET", 50,0,1.2));
    h_dphimin = MakeTH1F(new TH1F("h_dphimin","h_dphimin; Minimum dPhiJetMET",50,0,3.2));
  } else {
    auto Name = [histname](string name) { return name+histname; };
    // Event Info
    h_nVtx[i]           = MakeTH1F(new TH1F(Name("nVtx").c_str()         ,"nVtx;nVtx"                                              ,40,0,80));
    h_eventWeight[i]    = MakeTH1F(new TH1F(Name("eventWeight").c_str()  ,"eventWeight;Event Weight"                               ,50,0,2));
    h_kfactor[i]        = MakeTH1F(new TH1F(Name("kfactor").c_str()      ,"kfactor;kfactor"                                        ,50,-2,2));
    h_pileup[i]         = MakeTH1F(new TH1F(Name("pileup").c_str()       ,"pileup;pileup"                                          ,50,-2,2));
    h_genWeight[i]      = MakeTH1F(new TH1F(Name("genWeight").c_str()    ,"genWeight;genWeight"                                    ,50,-2,2));
    h_sf[i]             = MakeTH1F(new TH1F(Name("scaleFactor").c_str()  ,"scaleFactor;Scale Factor"                               ,50,-2,2));
    // MC Info
    h_puTrueNoW[i]      = MakeTH1F(new TH1F(Name("puTrueNoW").c_str()    ,"puTrue Unweighted;unweighted true number of iteractions",100,0,100));
    h_puTrueReW[i]      = MakeTH1F(new TH1F(Name("puTrueReW").c_str()    ,"puTrue Reweighted;reweighted true number of iteractions",100,0,100));
    h_genHT[i]          = MakeTH1F(new TH1F(Name("genHT").c_str()        ,"genHT;genHT"                                            ,100,0,2500));
    h_bosonPt[i]        = MakeTH1F(new TH1F(Name("bosonPt").c_str()      ,"bosonPt; boson P_{T}"                                   ,nBosonPtBins,BosonPtBins));
    h_bosonPtwK[i]      = MakeTH1F(new TH1F(Name("bosonPtwK").c_str()    ,"bosonPtwK; kfactor boson P_{T}"                         ,nBosonPtBins,BosonPtBins));
    // MET Info
    h_pfMETall[i]       = MakeTH1F(new TH1F(Name("pfMETall").c_str()     ,"pfMETall;E_{T}^{miss} (GeV)"                            ,50,0,2000)); 
    h_pfMET[i]          = MakeTH1F(new TH1F(Name("pfMET").c_str()        ,"pfMET;E_{T}^{miss} (GeV)"                               ,nMetBins,MetBins));
    h_pfMETPhi[i]       = MakeTH1F(new TH1F(Name("pfMETPhi").c_str()     ,"pfMETPhi;pfMET #phi"                                    ,nPhiBins,lPhi,uPhi));
    h_recoilall[i]      = MakeTH1F(new TH1F(Name("recoilall").c_str()    ,"recoilall;Recoil (GeV)"                                 ,50,0,2000));
    h_recoil[i]         = MakeTH1F(new TH1F(Name("recoil").c_str()       ,"recoil;Recoil (GeV)"                                    ,nMetBins,MetBins));
    h_recoilPhi[i]      = MakeTH1F(new TH1F(Name("recoilPhi").c_str()    ,"recoilPhi;Recoil #phi"                                  ,nPhiBins,lPhi,uPhi));
    // Jet Info
    h_nJets[i]          = MakeTH1F(new TH1F(Name("nJets").c_str()        ,"nJets;Number of Jets"                                   ,21,-0.5,20.5));
    h_j1pT[i]           = MakeTH1F(new TH1F(Name("j1pT").c_str()         ,"j1pT;p_{T} of Leading Jet (GeV)"                        ,nPtBins,PtBins));
    h_j1pTall[i]        = MakeTH1F(new TH1F(Name("j1pTall").c_str()      ,"j1pT;p_{T} of Leading Jet (GeV)"                        ,50,0,2000));
    h_j1Eta[i]          = MakeTH1F(new TH1F(Name("j1Eta").c_str()        ,"j1Eta; #eta of Leading Jet"                             ,nEtaBins,lEta,uEta));
    h_j1Phi[i]          = MakeTH1F(new TH1F(Name("j1Phi").c_str()        ,"j1Phi; #phi of Leading Jet"                             ,nPhiBins,lPhi,uPhi));
    h_j1etaWidth[i]     = MakeTH1F(new TH1F(Name("j1etaWidth").c_str()   ,"j1etaWidth; #eta width of Leading Jet"                  ,50,0,0.25));
    h_j1phiWidth[i]     = MakeTH1F(new TH1F(Name("j1phiWidth").c_str()   ,"j1phiWidth; #phi width of Leading Jet"                  ,50, 0,0.5));
    h_j1CHF[i]          = MakeTH1F(new TH1F(Name("j1CHF").c_str()        ,"j1CHF;Charged Hadron Energy Fraction in Leading Jet"    ,50,0,1.1));
    h_j1NHF[i]          = MakeTH1F(new TH1F(Name("j1NHF").c_str()        ,"j1NHF;Neutral Hadron Energy Fraction in Leading Jet"    ,50,0,1.1)); 
    h_j1ChMult[i]       = MakeTH1F(new TH1F(Name("j1ChMult").c_str()     ,"j1ChMult;Charged Multiplicity of Leading Jet"           ,25,0,50));
    h_j1NhMult[i]       = MakeTH1F(new TH1F(Name("j1NhMult").c_str()     ,"j1NhMult;Neutral Multiplicity of Leading Jet"           ,25,0,50)); 
    h_j1Mt[i]           = MakeTH1F(new TH1F(Name("j1Mt").c_str()         ,"j1Mt;M_{T} of Leading Jet (GeV)"                        ,nMtBins,MtBins));
  }
}

void monoJetAnalysis::fillHistos(int nhist,float event_weight) {
  if (sample.isData) event_weight = 1;
  else {
    // MC Info          ;
    h_puTrueNoW[nhist]  ->Fill(puTrue->at(0),weight_nopileup);
    h_puTrueReW[nhist]  ->Fill(puTrue->at(0),event_weight);
    h_genHT[nhist]      ->Fill(genHT,event_weight);
    h_bosonPt[nhist]    ->Fill(bosonPt,genWeight);
    h_bosonPtwK[nhist]  ->Fill(bosonPt,genWeight * kfactor);
  }
  
  // Event Info       
  h_nVtx[nhist]         ->Fill(nVtx,event_weight);   
  h_eventWeight[nhist]  ->Fill(event_weight,event_weight);
  h_kfactor[nhist]      ->Fill(kfactor,event_weight);
  h_pileup[nhist]       ->Fill(pileup,event_weight);
  h_genWeight[nhist]    ->Fill(genWeight,event_weight);
  h_sf[nhist]           ->Fill(sf,event_weight);
  
  // MET Info         ;
  h_pfMETall[nhist]     ->Fill(pfMET,event_weight);
  h_pfMET[nhist]        ->Fill(pfMET,event_weight);
  h_pfMETPhi[nhist]     ->Fill(pfMETPhi,event_weight);
  h_recoil[nhist]       ->Fill(recoil,event_weight);
  h_recoilall[nhist]    ->Fill(recoil,event_weight);
  h_recoilPhi[nhist]    ->Fill(recoilPhi,event_weight);

  // Jet Info         ;
  h_nJets[nhist]        ->Fill(nJet,event_weight);
  if ( jetindex != -1 ) {
    h_j1pT[nhist]         ->Fill(j1pT,event_weight);
    h_j1pTall[nhist]      ->Fill(j1pT,event_weight);
    h_j1Eta[nhist]        ->Fill(j1Eta,event_weight);
    h_j1Phi[nhist]        ->Fill(j1Phi,event_weight);
    h_j1etaWidth[nhist]   ->Fill(jetetaWidth->at(jetindex),event_weight);
    h_j1phiWidth[nhist]   ->Fill(jetphiWidth->at(jetindex),event_weight);
    h_j1CHF[nhist]        ->Fill(jetCHF->at(jetindex),event_weight);
    h_j1NHF[nhist]        ->Fill(jetNHF->at(jetindex),event_weight);
    h_j1ChMult[nhist]     ->Fill(jetNChargedHad->at(jetindex),event_weight);
    h_j1NhMult[nhist]     ->Fill(jetNNeutralHad->at(jetindex),event_weight);
    h_j1Mt[nhist]         ->Fill(jetMt->at(jetindex),event_weight);
  }
}

void monoJetAnalysis::fillEvent(int nhist,float event_weight) {
  cutflow->Fill(nhist,event_weight);
  fillHistos(nhist,event_weight);
}

bool monoJetAnalysis::getMetFilter(){                                                                                                    
  bool decision = true;

  if(metFilters>>0 & 1) decision = false;
  if(metFilters>>1 & 1) decision = false;
  if(metFilters>>2 & 1) decision = false;
  if(metFilters>>3 & 1) decision = false;
  if(metFilters>>4 & 1) decision = false;
  if(metFilters>>5 & 1) decision = false;
  if(metFilters>>7 & 1) decision = false;
  if(metFilters>>8 & 1) decision = false;

  return decision;  
}

bool monoJetAnalysis::getMetTrigger() {
  if (!sample.isData) return true;
  return ((HLTMet>>7&1) == 1 || (HLTMet>>8&1) == 1 || (HLTMet>>10&1) == 1);
}

bool monoJetAnalysis::getElectronTrigger() {
  if (!sample.isData) return true;
  return (HLTEleMuX>>5&1) == 1 || (HLTEleMuX>>6&1) == 1 || (HLTPho>>11&1) == 1;
}

bool monoJetAnalysis::getPhotonTrigger() {
  if (!sample.isData) return true;
  return (HLTPho>>11&1) == 1;
}

float monoJetAnalysis::dPhiJetMETmin(vector<int> jets,float metPhi) {
  //Only look at first four jets (because that's what monojet analysis do)
  int njets = jets.size();
  if (njets > 4) njets = 4;
  
  float minDPhiJetMET_first4 = TMath::Pi();
  for (int ijet = 0; ijet < njets; ijet++) {
    int index = jets[ijet];
    float dPhiJetMET = deltaPhi(jetPhi->at(index),metPhi);
    if (dPhiJetMET < minDPhiJetMET_first4) minDPhiJetMET_first4 = dPhiJetMET;
  }
  return minDPhiJetMET_first4;
}

vector<int> monoJetAnalysis::getJetCand(float jetPtCut,float jetEtaCut,float jetNHFCut,float jetCHFCut) {
  vector<int> tmpCand; tmpCand.clear();
  for(int i = 0; i < nJet; i++){
    bool tightJetID = (jetID->at(i)>>0&1) == 1;
    bool kinematics = jetPt->at(i) > jetPtCut && fabs(jetEta->at(i)) < jetEtaCut;
    bool cleaning = jetNHF->at(i) < jetNHFCut && jetCHF->at(i) > jetCHFCut;
    if (tightJetID && kinematics && cleaning)
      tmpCand.push_back(i);
  }
  return tmpCand;
}

vector<int> monoJetAnalysis::getJetCand(vector<int> jetlist,float jetPtCut,float jetEtaCut,float jetNHFCut,float jetCHFCut) {
  vector<int> tmpCand; tmpCand.clear();
  for(int i : jetlist){
    bool tightJetID = (jetID->at(i)>>0&1) == 1;
    bool kinematics = jetPt->at(i) > jetPtCut && fabs(jetEta->at(i)) < jetEtaCut;
    bool cleaning = jetNHF->at(i) < jetNHFCut && jetCHF->at(i) > jetCHFCut;
    if (tightJetID && kinematics && cleaning)
      tmpCand.push_back(i);
  }
  return tmpCand;
}

int monoJetAnalysis::setJetCand(vector<int> jetlist) {
  if (jetlist.size() == 0) return -1;
  jetindex = jetlist[0];
  j1pT = jetPt->at(jetindex);
  j1Eta = jetEta->at(jetindex);
  j1Phi = jetPhi->at(jetindex);
}

vector<int> monoJetAnalysis::getLooseJet(float jetPtCut,float jetEtaCut) {
  vector<int> jetindex; jetindex.clear();
  for(int i = 0; i < nJet; i++) {
    if (jetPt->at(i) > jetPtCut && fabs(jetEta->at(i)) < jetEtaCut && (jetID->at(i)>>0&1) == 1)
      jetindex.push_back(i);
  }
  return jetindex;
}

vector<int> monoJetAnalysis::jet_veto_looseID(int jetindex,float jetPtCut,float jetEtaCut) {
  vector<int> jet_cand; jet_cand.clear();
  vector<int> tmpcands = getLooseJet(jetPtCut,jetEtaCut);
  for (int i : tmpcands) {
    float dr_jet = deltaR(jetEta->at(i),jetPhi->at(i),jetEta->at(jetindex),jetPhi->at(jetindex));
    if (dr_jet > Iso4Cut) {
      jet_cand.push_back(i);
    }
  }
  return jet_cand;
}

vector<int> monoJetAnalysis::getLooseBJet(float jetPtCut,float jetEtaCut) {
  vector<int> bjet_cand; bjet_cand.clear();
  for (int i = 0; i < nJet; i++) {
    float bjetTag = jetDeepCSVTags_b->at(i) + jetDeepCSVTags_bb->at(i);
    if (jetPt->at(i) > jetPtCut && fabs(jetEta->at(i)) < jetEtaCut && bjetTag > bjetDeepCSVCut) {
      bjet_cand.push_back(i);
    }
  }
  return bjet_cand;
}

vector<int> monoJetAnalysis::bjet_veto_looseID(int jetindex,float jetPtCut,float jetEtaCut) {
  vector<int> jet_cand; jet_cand.clear();
  vector<int> tmpcands = getLooseBJet(jetPtCut,jetEtaCut);
  for (int i : tmpcands) {
    float dr_jet = deltaR(jetEta->at(i),jetPhi->at(i),jetEta->at(jetindex),jetPhi->at(jetindex));
    if (dr_jet > Iso4Cut) {
      jet_cand.push_back(i);
    }
  }
  return jet_cand;
}

vector<int> monoJetAnalysis::getLooseEle(float elePtCut,float eleEtaCut){
  vector<int> ele_cands;
  ele_cands.clear();

  for(int i = 0; i < nEle; i++){
    if(elePt->at(i) > elePtCut && fabs(eleSCEta->at(i)) < eleEtaCut){
      //Electron passes veto Electron ID cuts
      if( (eleIDbit->at(i)>>3&1) == 1){

	//Ele DZ and D0 selection
	if ((fabs(eleSCEta->at(i)) <= 1.479) && (fabs(eleD0->at(i)) < 0.05) && (fabs(eleDz->at(i)) < 0.1 )) {
	  ele_cands.push_back(i);
	}
	else if( (fabs(eleSCEta->at(i)) > 1.479) && (fabs(eleD0->at(i)) < 0.1) && (fabs(eleDz->at(i)) < 0.2 )){
	  ele_cands.push_back(i);
	}
      }
    }
  }

  return ele_cands;
}

vector<int> monoJetAnalysis::electron_veto_looseID(int jetindex,float elePtCut,float eleEtaCut) {
  vector<int> ele_cand; ele_cand.clear();
  vector<int> tmpcands = getLooseEle(elePtCut,eleEtaCut);
  for (int i : tmpcands) {
    float dr_ele = deltaR(eleSCEta->at(i),eleSCPhi->at(i),jetEta->at(jetindex),jetPhi->at(jetindex));
    if (dr_ele > Iso5Cut) {
      ele_cand.push_back(i);
    }
  }
  return ele_cand;
}

vector<int> monoJetAnalysis::getTightEle(float elePtCut,float eleEtaCut){
  vector<int> ele_cands;
  ele_cands.clear();

  for(int i = 0; i < nEle; i++){
    //Electron passes pt and eta cut
    if((elePt->at(i) > elePtCut) && (fabs(eleSCEta->at(i)) < eleEtaCut)) {
      //Electron passes Tight Electron ID cuts
      if(eleIDbit->at(i)>>2&1 == 1){

	//Electron passes eta cut
	if ( (fabs(eleSCEta->at(i)) <= 1.479) && (fabs(eleD0->at(i)) < 0.05) && (fabs(eleDz->at(i)) < 0.1)){
	  ele_cands.push_back(i);
	}  
	else if( (fabs(eleSCEta->at(i)) > 1.479) && (fabs(eleD0->at(i)) < 0.1)   && (fabs(eleDz->at(i)) < 0.2)){
	  ele_cands.push_back(i);
	}
      }
    }
  }

  return ele_cands;
}

vector<int> monoJetAnalysis::getTightEle(vector<int> looselist,float elePtCut,float eleEtaCut){
  vector<int> ele_cands;
  ele_cands.clear();

  for(int i : looselist){
    //Electron passes pt and eta cut
    if((elePt->at(i) > elePtCut) && (fabs(eleSCEta->at(i)) < eleEtaCut)) {
      //Electron passes Tight Electron ID cuts
      if(eleIDbit->at(i)>>2&1 == 1){

	//Electron passes eta cut
	if ( (fabs(eleSCEta->at(i)) <= 1.479) && (fabs(eleD0->at(i)) < 0.05) && (fabs(eleDz->at(i)) < 0.1)){
	  ele_cands.push_back(i);
	}  
	else if( (fabs(eleSCEta->at(i)) > 1.479) && (fabs(eleD0->at(i)) < 0.1)   && (fabs(eleDz->at(i)) < 0.2)){
	  ele_cands.push_back(i);
	}
      }
    }
  }

  return ele_cands;
}

vector<int> monoJetAnalysis::getLooseMu(float muPtCut,float muEtaCut){
  vector<int> mu_cands;
  mu_cands.clear();

  for(int i = 0; i < nMu; i++){
    // mu pt and eta cut
    if(muPt->at(i) > muPtCut && (fabs(muEta->at(i)) < muEtaCut)){
      // muon loose ID and Iso
      if(muIDbit->at(i)>>0&1 == 1 && muIDbit->at(i)>>7&1==1){
	mu_cands.push_back(i);
      }
    }
  }  
  return mu_cands;
}

vector<int> monoJetAnalysis::muon_veto_looseID(int jetindex,float muPtCut,float muEtaCut) {
  vector<int> mu_cand; mu_cand.clear();
  vector<int> tmpcands = getLooseMu(muPtCut,muEtaCut);
  for (int i : tmpcands) {
    float dr_mu = deltaR(muEta->at(i),muPhi->at(i),jetEta->at(jetindex),jetPhi->at(jetindex));
    if (dr_mu > Iso4Cut) {
      mu_cand.push_back(i);
    }
  }
  return mu_cand;
}

vector<int> monoJetAnalysis::getTightMu(float muPtCut,float muEtaCut){
  vector<int> mu_cands;
  mu_cands.clear();

  for(int i = 0; i < nMu; i++){
    //muon pt and eta cut
    if(muPt->at(i) > muPtCut && (fabs(muEta->at(i)) < muEtaCut)){
      // muon tight ID and Iso
      if(muIDbit->at(i)>>3&1 == 1 && muIDbit->at(i)>>9&1 == 1){
	mu_cands.push_back(i);
      }
    }
  }

  return mu_cands;
}

vector<int> monoJetAnalysis::getTightMu(vector<int> looselist,float muPtCut,float muEtaCut){
  vector<int> mu_cands;
  mu_cands.clear();

  for(int i : looselist){
    //muon pt and eta cut
    if(muPt->at(i) > muPtCut && (fabs(muEta->at(i)) < muEtaCut)){
      // muon tight ID and Iso
      if(muIDbit->at(i)>>3&1 == 1 && muIDbit->at(i)>>9&1 == 1){
	mu_cands.push_back(i);
      }
    }
  }

  return mu_cands;
}

vector<int> monoJetAnalysis::getLoosePho(float phoPtCut,float phoEtaCut){
  vector<int> pho_cands;
  pho_cands.clear();

  for(int i = 0; i < nPho; i++){
    // passes pt cut
    bool kinematics = ((phoEt->at(i) > phoPtCut) && (fabs(phoSCEta->at(i)) < phoEtaCut));
    bool IdIso = (phoIDbit->at(i)>>0&1==1);
    bool eleVeto = phoEleVeto->at(i);
    
    if (kinematics && IdIso && eleVeto)
      pho_cands.push_back(i);
  }  

  return pho_cands;
}

vector<int> monoJetAnalysis::photon_veto_looseID(int jetindex,float phoPtCut,float phoEtaCut) {
  vector<int> pho_cand; pho_cand.clear();
  vector<int> tmpcands = getLoosePho(phoPtCut,phoEtaCut);
  for (int i : tmpcands) {
    float dr_pho = deltaR(phoSCEta->at(i),phoSCPhi->at(i),jetEta->at(jetindex),jetPhi->at(jetindex));
    if (dr_pho > Iso5Cut) {
      pho_cand.push_back(i);
    }
  }
  return pho_cand;
}

vector<int> monoJetAnalysis::getTightPho(float phoPtCut,float phoEtaCut){
  vector<int> pho_cands;
  pho_cands.clear();
  
  for(int i = 0; i < nPho; i++){
    // passes pt cut
    bool kinematics = ((phoEt->at(i) > phoPtCut) && (fabs(phoSCEta->at(i)) < phoEtaCut));
    bool IdIso = (phoIDbit->at(i)>>1&1==1);
    bool eleVeto = phoEleVeto->at(i);
    
    if (kinematics && IdIso && eleVeto)
      pho_cands.push_back(i);
  }  
  
  return pho_cands;
}

vector<int> monoJetAnalysis::getTightPho(vector<int> looselist,float phoPtCut,float phoEtaCut){
  vector<int> pho_cands;
  pho_cands.clear();
  
  for(int i : looselist){
    // passes pt cut
    bool kinematics = ((phoEt->at(i) > phoPtCut) && (fabs(phoSCEta->at(i)) < phoEtaCut));
    bool IdIso = (phoIDbit->at(i)>>1&1==1);
    bool eleVeto = phoEleVeto->at(i);
    
    if (kinematics && IdIso && eleVeto)
      pho_cands.push_back(i);
  }  
  
  return pho_cands;
}

vector<int> monoJetAnalysis::getLooseTau(float tauPtCut,float tauEtaCut) {
  vector<int> tau_cands; tau_cands.clear();
  
  for (int i = 0; i < nTau; i++) {
    if ( (tau_IDbits->at(i)>>0&1) == 1 && (tau_IDbits->at(i)>>13&1) == 1 ) {
      if ( fabs(tau_Eta->at(i)) < tauLooseEtaCut ){
	if ( tau_Pt->at(i) > tauPtCut ) {
	  tau_cands.push_back(i);
	}
      }
    }
  }
  return tau_cands;
}

vector<int> monoJetAnalysis::tau_veto_looseID(int jetindex,float tauPtCut,float tauEtaCut) {
  vector<int> tau_cand; tau_cand.clear();
  vector<int> tmpcands = getLooseTau(tauPtCut,tauEtaCut);
  for (int i : tmpcands) {
    float dr_tau = deltaR(tau_Eta->at(i),tau_Phi->at(i),jetEta->at(jetindex),jetPhi->at(jetindex));
    if (dr_tau > Iso4Cut) {
      tau_cand.push_back(i);
    }
  }
  return tau_cand;
}

void monoJetAnalysis::SetBoson(int PID) {
  bosonPt = 0;
  for (int i = 0; i < nMC; i++){
    if((*mcPID)[i] == PID && (mcStatusFlag->at(i)>>2&1) == 1){
      bosonPt = (*mcPt)[i];
      SetKFactors(bosonPt);
    }
  }
}

float monoJetAnalysis::getKFactor(float bosonPt) {
  float nlo_ewk = th1fmap.getBin("NLO_EWK",bosonPt);
  float nlo_qcd = th1fmap.getBin("NLO_QCD",bosonPt);
  float nnlo_qcd = th1fmap.getBin("NNLO_QCD",bosonPt);
  float kfactor = 1;
  if (sample.isNLO) kfactor = nlo_ewk * nnlo_qcd;
  else kfactor = nlo_ewk * nlo_qcd * nnlo_qcd;
  return kfactor;
}

void monoJetAnalysis::SetKFactors(float bosonPt) {
  kfactor = getKFactor(bosonPt);
}

void monoJetAnalysis::ApplyKFactor(float &event_weight) {
  event_weight *= kfactor;
  weight_nogen *= kfactor;
  weight_nopileup *= kfactor;
}

void monoJetAnalysis::SetSF(float sf) {
  this->sf = sf;
}

void monoJetAnalysis::ApplySF(float &event_weight) {
  event_weight *= sf;
  weight_nogen *= sf;
  weight_nopileup *= sf;
}

void monoJetAnalysis::ApplyPileup(float &event_weight) {
  //For each event we find the bin in the PU histogram that corresponds to puTrue->at(0) and store
  //binContent as event_weight
  pileup = th1fmap.getBin("PU",puTrue->at(0));
  genWeight = fabs(genWeight) > 0 ? genWeight/fabs(genWeight) : 0;
  event_weight *= pileup * genWeight;
  weight_nogen *= pileup;
  weight_nopileup *= genWeight;
}

bool monoJetAnalysis::inclusiveCut() {
  if (sample.isInclusive)
    return genHT < 100;
  return true;
}

bool monoJetAnalysis::getJetHEMVeto(float jetPtCut){

  bool pass = true;
  for(int p=0;p<nJet;p++)
  {
    bool kinematic = (*jetPt)[p] > jetPtCut && (*jetEta)[p] < -1.4 && (*jetEta)[p] > -3.0 && (*jetPhi)[p] > -1.57 && (*jetPhi)[p] < -0.87 ;
    bool tightJetID = false;
    if ((*jetID)[p]>>0&1 == 1) tightJetID = true;
    if(kinematic) // not chekcing ID here.                                                                                                                                         
      pass = false;
  }

  return pass;
}

bool monoJetAnalysis::getEleHEMVeto(float elePtCut){

  bool pass = true;
  for(int p=0;p<nEle;p++)
  {
    bool kinematic = (*elePt)[p] > elePtCut && (*eleEta)[p] < -1.4 && (*eleEta)[p] > -3.0 && (*elePhi)[p] > -1.57 && (*elePhi)[p] < -0.87 ;
    if(kinematic) // not chekcing ID here.                                                                                                                                         
      pass = false;
  }

  return pass;
}

void monoJetAnalysis::initVars() {
  jetindex = -1;

  if(sample.isData) {
    // genWeight is used for the total events rather than event_weight since it has pileup and kfactors applied at the beginning
    // data doesn't have genWeight so set it to 1
    genWeight = 1;
  }

  weight = weight_nogen = weight_nopileup = kfactor = pileup = sf = 1;

  bosonPt = j1pT = j1Eta = j1Phi = -99;
  recoil = pfMET;
  recoilPhi = pfMETPhi;
  n_Vtx = nVtx;
  n_Jet = nJet;
}

// Constructor Stuff 
int monoJetAnalysis::getNfiles(TChain *chain,TString path,int nfiles) {
  TSystemDirectory sourceDir("hi",path);
  TList* fileList = sourceDir.GetListOfFiles();
  TIter nextlist(fileList);
  TSystemFile* filename;
  int inFile=0;
  while ((filename = (TSystemFile*)nextlist()) && inFile < nfiles)
  {
    //Debug
    if (debug) {
      cout<<"file path found: "<<(path+filename->GetName())<<endl;
      cout<<"name: "<<(filename->GetName())<<endl;
      cout<<"fileNumber: "<<inFile<<endl;
    }

    TString dataset = ".root";
    TString  FullPathInputFile = (path+filename->GetName());
    if ( !FullPathInputFile.EndsWith(dataset) ) continue;
    if (debug)
      cout<<"Adding FullPathInputFile to chain:"<<FullPathInputFile<<endl<<endl;
    chain->Add(FullPathInputFile);
    inFile++;

  }
  return inFile;
}

int monoJetAnalysis::getFilesByNumber(TChain *chain,TString path,const char* fileRange) {
  TSystemDirectory sourceDir("hi",path);
  TList* fileList = sourceDir.GetListOfFiles();
  TIter nextlist(fileList);
  TSystemFile* filename;
  int fileNumber = 0;
  int maxFiles = -1;
  int inFile=0;
  while ((filename = (TSystemFile*)nextlist()) && fileNumber >  maxFiles)
  {
    //Debug
    if (debug) {
      cout<<"file path found: "<<(path+filename->GetName())<<endl;
      cout<<"name: "<<(filename->GetName())<<endl;
      cout<<"fileNumber: "<<fileNumber<<endl;
    }

    TString dataset = ".root";
    TString  FullPathInputFile = (path+filename->GetName());
    TString name = filename->GetName();
    if (name.Contains(dataset))
    {
      string fname = string(name);
      fname.erase(fname.end()-5,fname.end());
      bool isin = fileSelection(fname,string(fileRange));
      if(isin)
      {
	if (debug)
	  cout<<"Adding FullPathInputFile to chain:"<<FullPathInputFile<<endl<<endl;
	chain->Add(FullPathInputFile);
	inFile++;
      }
    }
    fileNumber++;
  }
  return inFile;
}

int monoJetAnalysis::getFilesByList(TChain *chain,TString path,vector<const char*> filelist) {
  int inFile=0;
  TSystemFile* filename;
  for (const char* fname : filelist) {
    filename = new TSystemFile(fname,path);
    TString FullPathInputFile = (path+filename->GetName());
    if (debug) {
      cout<<"file path found: "<<FullPathInputFile<<endl;
      cout<<"name:"<<filename->GetName()<<endl;
      cout<<"fileNumber"<<inFile<<endl;
      cout <<"Adding FullPathInputFile to chain:"<<FullPathInputFile<<endl<<endl;
    }
    chain->Add(FullPathInputFile);
    inFile++;
  }
  return inFile;
}


monoJetAnalysis::monoJetAnalysis(const char* inputFilename,const char* outputFilename,int nfiles) {
  TChain *chain = new TChain("phoJetNtuplizer/eventTree");
  TString path = inputFilename;
  sample.setInfo(string(inputFilename));
  int inFile = getNfiles(chain,path,nfiles);
  cout<<"Sample type: "<< sample.GetTypeName() << (sample.isInclusive ? " Inclusive" : " not Inclusive") <<endl;
  cout<<inFile<<" files added."<<endl;
  cout<<"Initializing chain."<<endl;
  Init(chain);
}

monoJetAnalysis::monoJetAnalysis(const char* inputFilename,const char* outputFilename,vector<const char*> filelist) {
  TChain *chain = new TChain("phoJetNtuplizer/eventTree");
  TString path = inputFilename;
  sample.setInfo(string(inputFilename));
  int inFile = 0;
  if ( filelist.size() == 0 )
    inFile = getFilesByNumber(chain,path,"-1");
  else
    inFile = getFilesByList(chain,path,filelist);
  cout<<"Sample type: "<< sample.GetTypeName() << (sample.isInclusive ? " Inclusive" : " not Inclusive") <<endl;
  cout<<inFile<<" files added."<<endl;
  cout<<"Initializing chain."<<endl;
  Init(chain);
}

monoJetAnalysis::monoJetAnalysis(const char* inputFilename,const char* outputFilename,const char* fileRange) 
{
  TChain *chain = new TChain("phoJetNtuplizer/eventTree");
  TString path = inputFilename;
  sample.setInfo(string(inputFilename));
  int inFile = getFilesByNumber(chain,path,fileRange);
  cout<<"Sample type: "<< sample.GetTypeName() << (sample.isInclusive ? " Inclusive" : " not Inclusive") <<endl;
  cout<<inFile<<" files added."<<endl;
  cout<<"Initializing chain."<<endl;
  Init(chain);
}

monoJetAnalysis::~monoJetAnalysis()
{
  if (!fChain) return;
  delete fChain->GetCurrentFile();
  cout << "Writing to: " << output->GetName() << endl;
  output->cd();
  output->Write();
  output->Close();
}

void monoJetAnalysis::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the branch addresses and branch
  // pointers of the tree will be set.
  // It is normally not necessary to make changes to the generated
  // code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running on PROOF
  // (once per file to be processed).

  // Set object pointer
  phoE = 0;
  phoEt = 0;
  phoEta = 0;
  phoPhi = 0;
  phoUnCalibE = 0;
  phoUnCalibESigma = 0;
  phoCalibE = 0;
  phoCalibESigma = 0;
  phoCalibEt = 0;
  phoEnergyScale = 0;
  phoEnergySigma = 0;
  phoSCE = 0;
  phoSCRawE = 0;
  phoSCEta = 0;
  phoSCPhi = 0;
  phoSCEtaWidth = 0;
  phoSCPhiWidth = 0;
  phohasPixelSeed = 0;
  phoEleVeto = 0;
  phoR9Full5x5 = 0;
  phoHoverE = 0;
  phoSigmaIEtaIEtaFull5x5 = 0;
  phoSigmaIEtaIPhiFull5x5 = 0;
  phoSigmaIPhiIPhiFull5x5 = 0;
  phoPFChIso = 0;
  phoPFChWorstIso = 0;
  phoPFPhoIso = 0;
  phoPFNeuIso = 0;
  phoIDMVA = 0;
  phoIDbit = 0;
  phoSeedTime = 0;
  phoSeedEnergy = 0;
  phoFiredSingleTrgs = 0;
  phoFiredDoubleTrgs = 0;
  phoFiredTripleTrgs = 0;
  phoFiredL1Trgs = 0;
  phoScale_up = 0;
  phoScale_dn = 0;
  phoScale_stat_up = 0;
  phoScale_stat_dn = 0;
  phoScale_syst_up = 0;
  phoScale_syst_dn = 0;
  phoScale_gain_up = 0;
  phoScale_gain_dn = 0;
  phoResol_up = 0;
  phoResol_dn = 0;
  phoResol_rho_up = 0;
  phoResol_rho_dn = 0;
  phoResol_phi_up = 0;
  phoResol_phi_dn = 0;
  jetPt = 0;
  jetE = 0;
  jetEta = 0;
  jetPhi = 0;
  jetRawPt = 0;
  jetRawE = 0;
  jetMt = 0;
  jetArea = 0;
  jetMass = 0;
  jetMaxDistance = 0;
  jetPhiPhiMoment = 0;
  jetConstituentEtaPhiSpread = 0;
  jetConstituentPtDistribution = 0;
  jetPileup = 0;
  jetID = 0;
  jetPUID = 0;
  jetPUFullID = 0;
  jetPartonID = 0;
  jetHadFlvr = 0;
  jetJECUnc = 0;
  jetCEF = 0;
  jetNEF = 0;
  jetCHF = 0;
  jetNHF = 0;
  jetPhotonEnF = 0;
  jetElectronEnF = 0;
  jetMuonEnF = 0;
  jetChargedMuonEnF = 0;
  jetHFHAE = 0;
  jetHFEME = 0;
  jetNConst = 0;
  jetNConstituents = 0;
  jetNCharged = 0;
  jetNNeutral = 0;
  jetNChargedHad = 0;
  jetNNeutralHad = 0;
  jetNPhoton = 0;
  jetNElectron = 0;
  jetNMuon = 0;
  jetCSV2BJetTags = 0;
  jetDeepCSVTags_b = 0;
  jetDeepCSVTags_bb = 0;
  jetDeepCSVTags_c = 0;
  jetDeepCSVTags_udsg = 0;
  jetDeepFlavour_b = 0;
  jetDeepFlavour_bb = 0;
  jetDeepFlavour_lepb = 0;
  jetDeepFlavour_c = 0;
  jetDeepFlavour_uds = 0;
  jetDeepFlavour_g = 0;
  jetetaWidth = 0;
  jetphiWidth = 0;
  jetConstPt = 0;
  jetConstEt = 0;
  jetConstEta = 0;
  jetConstPhi = 0;
  jetConstPdgId = 0;
  ak8JetPt = 0;
  ak8JetE = 0;
  ak8JetEta = 0;
  ak8JetPhi = 0;
  ak8JetMass = 0;
  ak8JetCEF = 0;
  ak8JetNEF = 0;
  ak8JetCHF = 0;
  ak8JetNHF = 0;
  ak8JetNCH = 0;
  ak8JetNNP = 0;
  ak8JetMUF = 0;
  ak8Jet_tau1 = 0;
  ak8Jet_tau2 = 0;
  ak8Jet_tau3 = 0;
  ak8Jet_PrunedMass = 0;
  ak8Jet_SoftDropMass = 0;
  ak8Jet_PuppiSoftDropMass = 0;
  ak8Jet_PuppiTau1 = 0;
  ak8Jet_PuppiTau2 = 0;
  ak8Jet_PuppiTau3 = 0;
  ak8JetCHS_pt = 0;
  ak8JetCHS_eta = 0;
  ak8JetCHS_phi = 0;
  ak8JetCHS_mass = 0;
  ak8Jet_nb1AK8PuppiSoftDropN2 = 0;
  ak8Jet_nb1AK8PuppiSoftDropN3 = 0;
  ak8Jet_nb2AK8PuppiSoftDropN2 = 0;
  ak8Jet_nb2AK8PuppiSoftDropN3 = 0;
  ak8Jet_CSVv2Tags = 0;
  ak8Jet_DeepCSVTags_b = 0;
  ak8Jet_DeepCSVTags_bb = 0;
  ak8Jet_BoostedDSVTags_bb = 0;
  ak8JetJECUnc = 0;
  elePt = 0;
  eleEta = 0;
  elePhi = 0;
  eleR9Full5x5 = 0;
  eleE = 0;
  eleCharge = 0;
  eleChargeConsistent = 0;
  eleD0 = 0;
  eleDz = 0;
  eleSIP = 0;
  eleUnCalibE = 0;
  eleUnCalibESigma = 0;
  eleCalibEecalonly = 0;
  eleCalibE = 0;
  eleCalibESigma = 0;
  eleCalibEt = 0;
  eleCalibEtSigma = 0;
  eleEnergyScale = 0;
  eleEnergySigma = 0;
  eleSCRawE = 0;
  eleSCE = 0;
  eleSCEta = 0;
  eleSCPhi = 0;
  eleSCEtaWidth = 0;
  eleSCPhiWidth = 0;
  eleHoverE = 0;
  eleEoverP = 0;
  eleEoverPInv = 0;
  eleBrem = 0;
  eledEtaAtVtx = 0;
  eledPhiAtVtx = 0;
  eledEtaseedAtVtx = 0;
  eleSigmaIEtaIEtaFull5x5 = 0;
  eleSigmaIPhiIPhiFull5x5 = 0;
  eleConvVeto = 0;
  eleMissHits = 0;
  elePFChIso = 0;
  elePFPhoIso = 0;
  elePFNeuIso = 0;
  eleFiredSingleTrgs = 0;
  eleFiredDoubleTrgs = 0;
  eleFiredL1Trgs = 0;
  eleHEEPID = 0;
  eleMVAIsoID = 0;
  eleMVAnoIsoID = 0;
  eleIDbit = 0;
  eleScale_up = 0;
  eleScale_dn = 0;
  eleScale_stat_up = 0;
  eleScale_stat_dn = 0;
  eleScale_syst_up = 0;
  eleScale_syst_dn = 0;
  eleScale_gain_up = 0;
  eleScale_gain_dn = 0;
  eleResol_up = 0;
  eleResol_dn = 0;
  eleResol_rho_up = 0;
  eleResol_rho_dn = 0;
  eleResol_phi_up = 0;
  eleResol_phi_dn = 0;
  muPt = 0;
  muE = 0;
  muEta = 0;
  muPhi = 0;
  muCharge = 0;
  muType = 0;
  muIDbit = 0;
  muD0 = 0;
  muDz = 0;
  muSIP = 0;
  muChi2NDF = 0;
  muInnerD0 = 0;
  muInnerDz = 0;
  muTrkLayers = 0;
  muPixelLayers = 0;
  muPixelHits = 0;
  muMuonHits = 0;
  muStations = 0;
  muMatches = 0;
  muTrkQuality = 0;
  muInnervalidFraction = 0;
  muIsoTrk = 0;
  muPFChIso = 0;
  muPFPhoIso = 0;
  muPFNeuIso = 0;
  muPFPUIso = 0;
  muFiredTrgs = 0;
  muFiredL1Trgs = 0;
  tau_Pt = 0;
  tau_Et = 0;
  tau_Eta = 0;
  tau_Phi = 0;
  tau_Charge = 0;
  tau_DecayMode = 0;
  tau_P = 0;
  tau_Vz = 0;
  tau_Energy = 0;
  tau_Mass = 0;
  tau_Dxy = 0;
  tau_ZImpact = 0;
  tau_byCombinedIsolationDeltaBetaCorrRaw3Hits = 0;
  tau_chargedIsoPtSum = 0;
  tau_neutralIsoPtSum = 0;
  tau_neutralIsoPtSumWeight = 0;
  tau_footprintCorrection = 0;
  tau_photonPtSumOutsideSignalCone = 0;
  tau_puCorrPtSum = 0;
  tau_NumSignalPFChargedHadrCands = 0;
  tau_NumSignalPFNeutrHadrCands = 0;
  tau_NumSignalPFGammaCands = 0;
  tau_NumSignalPFCands = 0;
  tau_NumIsolationPFChargedHadrCands = 0;
  tau_NumIsolationPFNeutrHadrCands = 0;
  tau_NumIsolationPFGammaCands = 0;
  tau_NumIsolationPFCands = 0;
  tau_LeadChargedHadronEta = 0;
  tau_LeadChargedHadronPhi = 0;
  tau_LeadChargedHadronPt = 0;
  tau_LeadChargedHadron_dz = 0;
  tau_LeadChargedHadron_dxy = 0;
  tau_IDbits = 0;
  tau_byIsolationMVArun2017v2DBoldDMwLTraw2017 = 0;
  pdf = 0;
  pdfSystWeight = 0;
  psWeight = 0;
  nPU = 0;
  puBX = 0;
  puTrue = 0;
  mcPID = 0;
  mcVtx = 0;
  mcVty = 0;
  mcVtz = 0;
  mcPt = 0;
  mcMass = 0;
  mcEta = 0;
  mcPhi = 0;
  mcE = 0;
  mcEt = 0;
  mcStatus = 0;
  mcStatusFlag = 0;
  mcIndex = 0;
  // Set branch addresses and branch pointers
  if (!tree) return;
  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("run", &run, &b_run);
  fChain->SetBranchAddress("event", &event, &b_event);
  fChain->SetBranchAddress("lumis", &lumis, &b_lumis);
  fChain->SetBranchAddress("isData", &isData, &b_isData);
  fChain->SetBranchAddress("nVtx", &nVtx, &b_nVtx);
  fChain->SetBranchAddress("vtxX", &vtxX, &b_vtxX);
  fChain->SetBranchAddress("vtxY", &vtxY, &b_vtxY);
  fChain->SetBranchAddress("vtxZ", &vtxZ, &b_vtxZ);
  fChain->SetBranchAddress("vtxNtrks", &vtxNtrks, &b_vtxNtrks);
  fChain->SetBranchAddress("vtx_isFake", &vtx_isFake, &b_vtx_isFake);
  fChain->SetBranchAddress("vtx_ndof", &vtx_ndof, &b_vtx_ndof);
  fChain->SetBranchAddress("vtx_rho", &vtx_rho, &b_vtx_rho);
  fChain->SetBranchAddress("isGoodVtx", &isGoodVtx, &b_isGoodVtx);
  fChain->SetBranchAddress("nGoodVtx", &nGoodVtx, &b_nGoodVtx);
  fChain->SetBranchAddress("rho", &rho, &b_rho);
  fChain->SetBranchAddress("rhoCentral", &rhoCentral, &b_rhoCentral);
  fChain->SetBranchAddress("HLTEleMuX", &HLTEleMuX, &b_HLTEleMuX);
  fChain->SetBranchAddress("HLTEleMuXIsPrescaled", &HLTEleMuXIsPrescaled, &b_HLTEleMuXIsPrescaled);
  fChain->SetBranchAddress("HLTEleMuXRejectedByPS", &HLTEleMuXRejectedByPS, &b_HLTEleMuXRejectedByPS);
  fChain->SetBranchAddress("HLTPho", &HLTPho, &b_HLTPho);
  fChain->SetBranchAddress("HLTPhoIsPrescaled", &HLTPhoIsPrescaled, &b_HLTPhoIsPrescaled);
  fChain->SetBranchAddress("HLTPhoRejectedByPS", &HLTPhoRejectedByPS, &b_HLTPhoRejectedByPS);
  fChain->SetBranchAddress("HLTTau", &HLTTau, &b_HLTTau);
  fChain->SetBranchAddress("HLTTauIsPrescaled", &HLTTauIsPrescaled, &b_HLTTauIsPrescaled);
  fChain->SetBranchAddress("HLTTauRejectedByPS", &HLTTauRejectedByPS, &b_HLTTauRejectedByPS);
  fChain->SetBranchAddress("HLTMet", &HLTMet, &b_HLTMet);
  fChain->SetBranchAddress("HLTMetIsPrescaled", &HLTMetIsPrescaled, &b_HLTMetIsPrescaled);
  fChain->SetBranchAddress("HLTMetRejectedByPS", &HLTMetRejectedByPS, &b_HLTMetRejectedByPS);
  fChain->SetBranchAddress("HLTJet", &HLTJet, &b_HLTJet);
  fChain->SetBranchAddress("HLTJetIsPrescaled", &HLTJetIsPrescaled, &b_HLTJetIsPrescaled);
  fChain->SetBranchAddress("HLTJetRejectedByPS", &HLTJetRejectedByPS, &b_HLTJetRejectedByPS);
  fChain->SetBranchAddress("nPho", &nPho, &b_nPho);
  fChain->SetBranchAddress("phoE", &phoE, &b_phoE);
  fChain->SetBranchAddress("phoEt", &phoEt, &b_phoEt);
  fChain->SetBranchAddress("phoEta", &phoEta, &b_phoEta);
  fChain->SetBranchAddress("phoPhi", &phoPhi, &b_phoPhi);
  fChain->SetBranchAddress("phoUnCalibE", &phoUnCalibE, &b_phoUnCalibE);
  fChain->SetBranchAddress("phoUnCalibESigma", &phoUnCalibESigma, &b_phoUnCalibESigma);
  fChain->SetBranchAddress("phoCalibE", &phoCalibE, &b_phoCalibE);
  fChain->SetBranchAddress("phoCalibESigma", &phoCalibESigma, &b_phoCalibESigma);
  fChain->SetBranchAddress("phoCalibEt", &phoCalibEt, &b_phoCalibEt);
  fChain->SetBranchAddress("phoEnergyScale", &phoEnergyScale, &b_phoEnergyScale);
  fChain->SetBranchAddress("phoEnergySigma", &phoEnergySigma, &b_phoEnergySigma);
  fChain->SetBranchAddress("phoSCE", &phoSCE, &b_phoSCE);
  fChain->SetBranchAddress("phoSCRawE", &phoSCRawE, &b_phoSCRawE);
  fChain->SetBranchAddress("phoSCEta", &phoSCEta, &b_phoSCEta);
  fChain->SetBranchAddress("phoSCPhi", &phoSCPhi, &b_phoSCPhi);
  fChain->SetBranchAddress("phoSCEtaWidth", &phoSCEtaWidth, &b_phoSCEtaWidth);
  fChain->SetBranchAddress("phoSCPhiWidth", &phoSCPhiWidth, &b_phoSCPhiWidth);
  fChain->SetBranchAddress("phohasPixelSeed", &phohasPixelSeed, &b_phohasPixelSeed);
  fChain->SetBranchAddress("phoEleVeto", &phoEleVeto, &b_phoEleVeto);
  fChain->SetBranchAddress("phoR9Full5x5", &phoR9Full5x5, &b_phoR9Full5x5);
  fChain->SetBranchAddress("phoHoverE", &phoHoverE, &b_phoHoverE);
  fChain->SetBranchAddress("phoSigmaIEtaIEtaFull5x5", &phoSigmaIEtaIEtaFull5x5, &b_phoSigmaIEtaIEtaFull5x5);
  fChain->SetBranchAddress("phoSigmaIEtaIPhiFull5x5", &phoSigmaIEtaIPhiFull5x5, &b_phoSigmaIEtaIPhiFull5x5);
  fChain->SetBranchAddress("phoSigmaIPhiIPhiFull5x5", &phoSigmaIPhiIPhiFull5x5, &b_phoSigmaIPhiIPhiFull5x5);
  fChain->SetBranchAddress("phoPFChIso", &phoPFChIso, &b_phoPFChIso);
  fChain->SetBranchAddress("phoPFChWorstIso", &phoPFChWorstIso, &b_phoPFChWorstIso);
  fChain->SetBranchAddress("phoPFPhoIso", &phoPFPhoIso, &b_phoPFPhoIso);
  fChain->SetBranchAddress("phoPFNeuIso", &phoPFNeuIso, &b_phoPFNeuIso);
  fChain->SetBranchAddress("phoIDMVA", &phoIDMVA, &b_phoIDMVA);
  fChain->SetBranchAddress("phoIDbit", &phoIDbit, &b_phoIDbit);
  fChain->SetBranchAddress("phoSeedTime", &phoSeedTime, &b_phoSeedTime);
  fChain->SetBranchAddress("phoSeedEnergy", &phoSeedEnergy, &b_phoSeedEnergy);
  fChain->SetBranchAddress("phoFiredSingleTrgs", &phoFiredSingleTrgs, &b_phoFiredSingleTrgs);
  fChain->SetBranchAddress("phoFiredDoubleTrgs", &phoFiredDoubleTrgs, &b_phoFiredDoubleTrgs);
  fChain->SetBranchAddress("phoFiredTripleTrgs", &phoFiredTripleTrgs, &b_phoFiredTripleTrgs);
  fChain->SetBranchAddress("phoFiredL1Trgs", &phoFiredL1Trgs, &b_phoFiredL1Trgs);
  fChain->SetBranchAddress("phoScale_up", &phoScale_up, &b_phoScale_up);
  fChain->SetBranchAddress("phoScale_dn", &phoScale_dn, &b_phoScale_dn);
  fChain->SetBranchAddress("phoScale_stat_up", &phoScale_stat_up, &b_phoScale_stat_up);
  fChain->SetBranchAddress("phoScale_stat_dn", &phoScale_stat_dn, &b_phoScale_stat_dn);
  fChain->SetBranchAddress("phoScale_syst_up", &phoScale_syst_up, &b_phoScale_syst_up);
  fChain->SetBranchAddress("phoScale_syst_dn", &phoScale_syst_dn, &b_phoScale_syst_dn);
  fChain->SetBranchAddress("phoScale_gain_up", &phoScale_gain_up, &b_phoScale_gain_up);
  fChain->SetBranchAddress("phoScale_gain_dn", &phoScale_gain_dn, &b_phoScale_gain_dn);
  fChain->SetBranchAddress("phoResol_up", &phoResol_up, &b_phoResol_up);
  fChain->SetBranchAddress("phoResol_dn", &phoResol_dn, &b_phoResol_dn);
  fChain->SetBranchAddress("phoResol_rho_up", &phoResol_rho_up, &b_phoResol_rho_up);
  fChain->SetBranchAddress("phoResol_rho_dn", &phoResol_rho_dn, &b_phoResol_rho_dn);
  fChain->SetBranchAddress("phoResol_phi_up", &phoResol_phi_up, &b_phoResol_phi_up);
  fChain->SetBranchAddress("phoResol_phi_dn", &phoResol_phi_dn, &b_phoResol_phi_dn);
  fChain->SetBranchAddress("nJet", &nJet, &b_nJet);
  fChain->SetBranchAddress("jetPt", &jetPt, &b_jetPt);
  fChain->SetBranchAddress("jetE", &jetE, &b_jetE);
  fChain->SetBranchAddress("jetEta", &jetEta, &b_jetEta);
  fChain->SetBranchAddress("jetPhi", &jetPhi, &b_jetPhi);
  fChain->SetBranchAddress("jetRawPt", &jetRawPt, &b_jetRawPt);
  fChain->SetBranchAddress("jetRawE", &jetRawE, &b_jetRawE);
  fChain->SetBranchAddress("jetMt", &jetMt, &b_jetMt);
  fChain->SetBranchAddress("jetArea", &jetArea, &b_jetArea);
  fChain->SetBranchAddress("jetMass", &jetMass, &b_jetMass);
  fChain->SetBranchAddress("jetMaxDistance", &jetMaxDistance, &b_jetMaxDistance);
  fChain->SetBranchAddress("jetPhiPhiMoment", &jetPhiPhiMoment, &b_jetPhiPhiMoment);
  fChain->SetBranchAddress("jetConstituentEtaPhiSpread", &jetConstituentEtaPhiSpread, &b_jetConstituentEtaPhiSpread);
  fChain->SetBranchAddress("jetConstituentPtDistribution", &jetConstituentPtDistribution, &b_jetConstituentPtDistribution);
  fChain->SetBranchAddress("jetPileup", &jetPileup, &b_jetPileup);
  fChain->SetBranchAddress("jetID", &jetID, &b_jetID);
  fChain->SetBranchAddress("jetPUID", &jetPUID, &b_jetPUID);
  fChain->SetBranchAddress("jetPUFullID", &jetPUFullID, &b_jetPUFullID);
  fChain->SetBranchAddress("jetPartonID", &jetPartonID, &b_jetPartonID);
  fChain->SetBranchAddress("jetHadFlvr", &jetHadFlvr, &b_jetHadFlvr);
  fChain->SetBranchAddress("jetJECUnc", &jetJECUnc, &b_jetJECUnc);
  fChain->SetBranchAddress("jetCEF", &jetCEF, &b_jetCEF);
  fChain->SetBranchAddress("jetNEF", &jetNEF, &b_jetNEF);
  fChain->SetBranchAddress("jetCHF", &jetCHF, &b_jetCHF);
  fChain->SetBranchAddress("jetNHF", &jetNHF, &b_jetNHF);
  fChain->SetBranchAddress("jetPhotonEnF", &jetPhotonEnF, &b_jetPhotonEnF);
  fChain->SetBranchAddress("jetElectronEnF", &jetElectronEnF, &b_jetElectronEnF);
  fChain->SetBranchAddress("jetMuonEnF", &jetMuonEnF, &b_jetMuonEnF);
  fChain->SetBranchAddress("jetChargedMuonEnF", &jetChargedMuonEnF, &b_jetChargedMuonEnF);
  fChain->SetBranchAddress("jetHFHAE", &jetHFHAE, &b_jetHFHAE);
  fChain->SetBranchAddress("jetHFEME", &jetHFEME, &b_jetHFEME);
  fChain->SetBranchAddress("jetNConst", &jetNConst, &b_jetNConst);
  fChain->SetBranchAddress("jetNConstituents", &jetNConstituents, &b_jetNConstituents);
  fChain->SetBranchAddress("jetNCharged", &jetNCharged, &b_jetNCharged);
  fChain->SetBranchAddress("jetNNeutral", &jetNNeutral, &b_jetNNeutral);
  fChain->SetBranchAddress("jetNChargedHad", &jetNChargedHad, &b_jetNChargedHad);
  fChain->SetBranchAddress("jetNNeutralHad", &jetNNeutralHad, &b_jetNNeutralHad);
  fChain->SetBranchAddress("jetNPhoton", &jetNPhoton, &b_jetNPhoton);
  fChain->SetBranchAddress("jetNElectron", &jetNElectron, &b_jetNElectron);
  fChain->SetBranchAddress("jetNMuon", &jetNMuon, &b_jetNMuon);
  fChain->SetBranchAddress("jetCSV2BJetTags", &jetCSV2BJetTags, &b_jetCSV2BJetTags);
  fChain->SetBranchAddress("jetDeepCSVTags_b", &jetDeepCSVTags_b, &b_jetDeepCSVTags_b);
  fChain->SetBranchAddress("jetDeepCSVTags_bb", &jetDeepCSVTags_bb, &b_jetDeepCSVTags_bb);
  fChain->SetBranchAddress("jetDeepCSVTags_c", &jetDeepCSVTags_c, &b_jetDeepCSVTags_c);
  fChain->SetBranchAddress("jetDeepCSVTags_udsg", &jetDeepCSVTags_udsg, &b_jetDeepCSVTags_udsg);
  fChain->SetBranchAddress("jetDeepFlavour_b", &jetDeepFlavour_b, &b_jetDeepFlavour_b);
  fChain->SetBranchAddress("jetDeepFlavour_bb", &jetDeepFlavour_bb, &b_jetDeepFlavour_bb);
  fChain->SetBranchAddress("jetDeepFlavour_lepb", &jetDeepFlavour_lepb, &b_jetDeepFlavour_lepb);
  fChain->SetBranchAddress("jetDeepFlavour_c", &jetDeepFlavour_c, &b_jetDeepFlavour_c);
  fChain->SetBranchAddress("jetDeepFlavour_uds", &jetDeepFlavour_uds, &b_jetDeepFlavour_uds);
  fChain->SetBranchAddress("jetDeepFlavour_g", &jetDeepFlavour_g, &b_jetDeepFlavour_g);
  fChain->SetBranchAddress("jetetaWidth", &jetetaWidth, &b_jetetaWidth);
  fChain->SetBranchAddress("jetphiWidth", &jetphiWidth, &b_jetphiWidth);
  fChain->SetBranchAddress("jetConstPt", &jetConstPt, &b_jetConstPt);
  fChain->SetBranchAddress("jetConstEt", &jetConstEt, &b_jetConstEt);
  fChain->SetBranchAddress("jetConstEta", &jetConstEta, &b_jetConstEta);
  fChain->SetBranchAddress("jetConstPhi", &jetConstPhi, &b_jetConstPhi);
  fChain->SetBranchAddress("jetConstPdgId", &jetConstPdgId, &b_jetConstPdgId);
  fChain->SetBranchAddress("nEle", &nEle, &b_nEle);
  fChain->SetBranchAddress("elePt", &elePt, &b_elePt);
  fChain->SetBranchAddress("eleEta", &eleEta, &b_eleEta);
  fChain->SetBranchAddress("elePhi", &elePhi, &b_elePhi);
  fChain->SetBranchAddress("eleR9Full5x5", &eleR9Full5x5, &b_eleR9Full5x5);
  fChain->SetBranchAddress("eleE", &eleE, &b_eleE);
  fChain->SetBranchAddress("eleCharge", &eleCharge, &b_eleCharge);
  fChain->SetBranchAddress("eleChargeConsistent", &eleChargeConsistent, &b_eleChargeConsistent);
  fChain->SetBranchAddress("eleD0", &eleD0, &b_eleD0);
  fChain->SetBranchAddress("eleDz", &eleDz, &b_eleDz);
  fChain->SetBranchAddress("eleSIP", &eleSIP, &b_eleSIP);
  fChain->SetBranchAddress("eleUnCalibE", &eleUnCalibE, &b_eleUnCalibE);
  fChain->SetBranchAddress("eleUnCalibESigma", &eleUnCalibESigma, &b_eleUnCalibESigma);
  fChain->SetBranchAddress("eleCalibEecalonly", &eleCalibEecalonly, &b_eleCalibEecalonly);
  fChain->SetBranchAddress("eleCalibE", &eleCalibE, &b_eleCalibE);
  fChain->SetBranchAddress("eleCalibESigma", &eleCalibESigma, &b_eleCalibESigma);
  fChain->SetBranchAddress("eleCalibEt", &eleCalibEt, &b_eleCalibEt);
  fChain->SetBranchAddress("eleCalibEtSigma", &eleCalibEtSigma, &b_eleCalibEtSigma);
  fChain->SetBranchAddress("eleEnergyScale", &eleEnergyScale, &b_eleEnergyScale);
  fChain->SetBranchAddress("eleEnergySigma", &eleEnergySigma, &b_eleEnergySigma);
  fChain->SetBranchAddress("eleSCRawE", &eleSCRawE, &b_eleSCRawE);
  fChain->SetBranchAddress("eleSCE", &eleSCE, &b_eleSCE);
  fChain->SetBranchAddress("eleSCEta", &eleSCEta, &b_eleSCEta);
  fChain->SetBranchAddress("eleSCPhi", &eleSCPhi, &b_eleSCPhi);
  fChain->SetBranchAddress("eleSCEtaWidth", &eleSCEtaWidth, &b_eleSCEtaWidth);
  fChain->SetBranchAddress("eleSCPhiWidth", &eleSCPhiWidth, &b_eleSCPhiWidth);
  fChain->SetBranchAddress("eleHoverE", &eleHoverE, &b_eleHoverE);
  fChain->SetBranchAddress("eleEoverP", &eleEoverP, &b_eleEoverP);
  fChain->SetBranchAddress("eleEoverPInv", &eleEoverPInv, &b_eleEoverPInv);
  fChain->SetBranchAddress("eleBrem", &eleBrem, &b_eleBrem);
  fChain->SetBranchAddress("eledEtaAtVtx", &eledEtaAtVtx, &b_eledEtaAtVtx);
  fChain->SetBranchAddress("eledPhiAtVtx", &eledPhiAtVtx, &b_eledPhiAtVtx);
  fChain->SetBranchAddress("eledEtaseedAtVtx", &eledEtaseedAtVtx, &b_eledEtaseedAtVtx);
  fChain->SetBranchAddress("eleSigmaIEtaIEtaFull5x5", &eleSigmaIEtaIEtaFull5x5, &b_eleSigmaIEtaIEtaFull5x5);
  fChain->SetBranchAddress("eleSigmaIPhiIPhiFull5x5", &eleSigmaIPhiIPhiFull5x5, &b_eleSigmaIPhiIPhiFull5x5);
  fChain->SetBranchAddress("eleConvVeto", &eleConvVeto, &b_eleConvVeto);
  fChain->SetBranchAddress("eleMissHits", &eleMissHits, &b_eleMissHits);
  fChain->SetBranchAddress("elePFChIso", &elePFChIso, &b_elePFChIso);
  fChain->SetBranchAddress("elePFPhoIso", &elePFPhoIso, &b_elePFPhoIso);
  fChain->SetBranchAddress("elePFNeuIso", &elePFNeuIso, &b_elePFNeuIso);
  fChain->SetBranchAddress("eleFiredSingleTrgs", &eleFiredSingleTrgs, &b_eleFiredSingleTrgs);
  fChain->SetBranchAddress("eleFiredDoubleTrgs", &eleFiredDoubleTrgs, &b_eleFiredDoubleTrgs);
  fChain->SetBranchAddress("eleFiredL1Trgs", &eleFiredL1Trgs, &b_eleFiredL1Trgs);
  fChain->SetBranchAddress("eleHEEPID", &eleHEEPID, &b_eleHEEPID);
  fChain->SetBranchAddress("eleMVAIsoID", &eleMVAIsoID, &b_eleMVAIsoID);
  fChain->SetBranchAddress("eleMVAnoIsoID", &eleMVAnoIsoID, &b_eleMVAnoIsoID);
  fChain->SetBranchAddress("eleIDbit", &eleIDbit, &b_eleIDbit);
  fChain->SetBranchAddress("eleScale_up", &eleScale_up, &b_eleScale_up);
  fChain->SetBranchAddress("eleScale_dn", &eleScale_dn, &b_eleScale_dn);
  fChain->SetBranchAddress("eleScale_stat_up", &eleScale_stat_up, &b_eleScale_stat_up);
  fChain->SetBranchAddress("eleScale_stat_dn", &eleScale_stat_dn, &b_eleScale_stat_dn);
  fChain->SetBranchAddress("eleScale_syst_up", &eleScale_syst_up, &b_eleScale_syst_up);
  fChain->SetBranchAddress("eleScale_syst_dn", &eleScale_syst_dn, &b_eleScale_syst_dn);
  fChain->SetBranchAddress("eleScale_gain_up", &eleScale_gain_up, &b_eleScale_gain_up);
  fChain->SetBranchAddress("eleScale_gain_dn", &eleScale_gain_dn, &b_eleScale_gain_dn);
  fChain->SetBranchAddress("eleResol_up", &eleResol_up, &b_eleResol_up);
  fChain->SetBranchAddress("eleResol_dn", &eleResol_dn, &b_eleResol_dn);
  fChain->SetBranchAddress("eleResol_rho_up", &eleResol_rho_up, &b_eleResol_rho_up);
  fChain->SetBranchAddress("eleResol_rho_dn", &eleResol_rho_dn, &b_eleResol_rho_dn);
  fChain->SetBranchAddress("eleResol_phi_up", &eleResol_phi_up, &b_eleResol_phi_up);
  fChain->SetBranchAddress("eleResol_phi_dn", &eleResol_phi_dn, &b_eleResol_phi_dn);
  fChain->SetBranchAddress("nMu", &nMu, &b_nMu);
  fChain->SetBranchAddress("muPt", &muPt, &b_muPt);
  fChain->SetBranchAddress("muE", &muE, &b_muE);
  fChain->SetBranchAddress("muEta", &muEta, &b_muEta);
  fChain->SetBranchAddress("muPhi", &muPhi, &b_muPhi);
  fChain->SetBranchAddress("muCharge", &muCharge, &b_muCharge);
  fChain->SetBranchAddress("muType", &muType, &b_muType);
  fChain->SetBranchAddress("muIDbit", &muIDbit, &b_muIDbit);
  fChain->SetBranchAddress("muD0", &muD0, &b_muD0);
  fChain->SetBranchAddress("muDz", &muDz, &b_muDz);
  fChain->SetBranchAddress("muSIP", &muSIP, &b_muSIP);
  fChain->SetBranchAddress("muChi2NDF", &muChi2NDF, &b_muChi2NDF);
  fChain->SetBranchAddress("muInnerD0", &muInnerD0, &b_muInnerD0);
  fChain->SetBranchAddress("muInnerDz", &muInnerDz, &b_muInnerDz);
  fChain->SetBranchAddress("muTrkLayers", &muTrkLayers, &b_muTrkLayers);
  fChain->SetBranchAddress("muPixelLayers", &muPixelLayers, &b_muPixelLayers);
  fChain->SetBranchAddress("muPixelHits", &muPixelHits, &b_muPixelHits);
  fChain->SetBranchAddress("muMuonHits", &muMuonHits, &b_muMuonHits);
  fChain->SetBranchAddress("muStations", &muStations, &b_muStations);
  fChain->SetBranchAddress("muMatches", &muMatches, &b_muMatches);
  fChain->SetBranchAddress("muTrkQuality", &muTrkQuality, &b_muTrkQuality);
  fChain->SetBranchAddress("muInnervalidFraction", &muInnervalidFraction, &b_muInnervalidFraction);
  fChain->SetBranchAddress("muIsoTrk", &muIsoTrk, &b_muIsoTrk);
  fChain->SetBranchAddress("muPFChIso", &muPFChIso, &b_muPFChIso);
  fChain->SetBranchAddress("muPFPhoIso", &muPFPhoIso, &b_muPFPhoIso);
  fChain->SetBranchAddress("muPFNeuIso", &muPFNeuIso, &b_muPFNeuIso);
  fChain->SetBranchAddress("muPFPUIso", &muPFPUIso, &b_muPFPUIso);
  fChain->SetBranchAddress("muFiredTrgs", &muFiredTrgs, &b_muFiredTrgs);
  fChain->SetBranchAddress("muFiredL1Trgs", &muFiredL1Trgs, &b_muFiredL1Trgs);
  fChain->SetBranchAddress("nTau", &nTau, &b_nTau);
  fChain->SetBranchAddress("tau_Pt", &tau_Pt, &b_tau_Pt);
  fChain->SetBranchAddress("tau_Et", &tau_Et, &b_tau_Et);
  fChain->SetBranchAddress("tau_Eta", &tau_Eta, &b_tau_Eta);
  fChain->SetBranchAddress("tau_Phi", &tau_Phi, &b_tau_Phi);
  fChain->SetBranchAddress("tau_Charge", &tau_Charge, &b_tau_Charge);
  fChain->SetBranchAddress("tau_DecayMode", &tau_DecayMode, &b_tau_DecayMode);
  fChain->SetBranchAddress("tau_P", &tau_P, &b_tau_P);
  fChain->SetBranchAddress("tau_Vz", &tau_Vz, &b_tau_Vz);
  fChain->SetBranchAddress("tau_Energy", &tau_Energy, &b_tau_Energy);
  fChain->SetBranchAddress("tau_Mass", &tau_Mass, &b_tau_Mass);
  fChain->SetBranchAddress("tau_Dxy", &tau_Dxy, &b_tau_Dxy);
  fChain->SetBranchAddress("tau_ZImpact", &tau_ZImpact, &b_tau_ZImpact);
  fChain->SetBranchAddress("tau_byCombinedIsolationDeltaBetaCorrRaw3Hits", &tau_byCombinedIsolationDeltaBetaCorrRaw3Hits, &b_tau_byCombinedIsolationDeltaBetaCorrRaw3Hits);
  fChain->SetBranchAddress("tau_chargedIsoPtSum", &tau_chargedIsoPtSum, &b_tau_chargedIsoPtSum);
  fChain->SetBranchAddress("tau_neutralIsoPtSum", &tau_neutralIsoPtSum, &b_tau_neutralIsoPtSum);
  fChain->SetBranchAddress("tau_neutralIsoPtSumWeight", &tau_neutralIsoPtSumWeight, &b_tau_neutralIsoPtSumWeight);
  fChain->SetBranchAddress("tau_footprintCorrection", &tau_footprintCorrection, &b_tau_footprintCorrection);
  fChain->SetBranchAddress("tau_photonPtSumOutsideSignalCone", &tau_photonPtSumOutsideSignalCone, &b_tau_photonPtSumOutsideSignalCone);
  fChain->SetBranchAddress("tau_puCorrPtSum", &tau_puCorrPtSum, &b_tau_puCorrPtSum);
  fChain->SetBranchAddress("tau_NumSignalPFChargedHadrCands", &tau_NumSignalPFChargedHadrCands, &b_tau_NumSignalPFChargedHadrCands);
  fChain->SetBranchAddress("tau_NumSignalPFNeutrHadrCands", &tau_NumSignalPFNeutrHadrCands, &b_tau_NumSignalPFNeutrHadrCands);
  fChain->SetBranchAddress("tau_NumSignalPFGammaCands", &tau_NumSignalPFGammaCands, &b_tau_NumSignalPFGammaCands);
  fChain->SetBranchAddress("tau_NumSignalPFCands", &tau_NumSignalPFCands, &b_tau_NumSignalPFCands);
  fChain->SetBranchAddress("tau_NumIsolationPFChargedHadrCands", &tau_NumIsolationPFChargedHadrCands, &b_tau_NumIsolationPFChargedHadrCands);
  fChain->SetBranchAddress("tau_NumIsolationPFNeutrHadrCands", &tau_NumIsolationPFNeutrHadrCands, &b_tau_NumIsolationPFNeutrHadrCands);
  fChain->SetBranchAddress("tau_NumIsolationPFGammaCands", &tau_NumIsolationPFGammaCands, &b_tau_NumIsolationPFGammaCands);
  fChain->SetBranchAddress("tau_NumIsolationPFCands", &tau_NumIsolationPFCands, &b_tau_NumIsolationPFCands);
  fChain->SetBranchAddress("tau_LeadChargedHadronEta", &tau_LeadChargedHadronEta, &b_tau_LeadChargedHadronEta);
  fChain->SetBranchAddress("tau_LeadChargedHadronPhi", &tau_LeadChargedHadronPhi, &b_tau_LeadChargedHadronPhi);
  fChain->SetBranchAddress("tau_LeadChargedHadronPt", &tau_LeadChargedHadronPt, &b_tau_LeadChargedHadronPt);
  fChain->SetBranchAddress("tau_LeadChargedHadron_dz", &tau_LeadChargedHadron_dz, &b_tau_LeadChargedHadron_dz);
  fChain->SetBranchAddress("tau_LeadChargedHadron_dxy", &tau_LeadChargedHadron_dxy, &b_tau_LeadChargedHadron_dxy);
  fChain->SetBranchAddress("tau_IDbits", &tau_IDbits, &b_tau_IDbits);
  fChain->SetBranchAddress("tau_byIsolationMVArun2017v2DBoldDMwLTraw2017", &tau_byIsolationMVArun2017v2DBoldDMwLTraw2017, &b_tau_byIsolationMVArun2017v2DBoldDMwLTraw2017);
  fChain->SetBranchAddress("metFilters", &metFilters, &b_metFilters);
  fChain->SetBranchAddress("caloMET", &caloMET, &b_caloMET);
  fChain->SetBranchAddress("caloMETPhi", &caloMETPhi, &b_caloMETPhi);
  fChain->SetBranchAddress("caloMETsumEt", &caloMETsumEt, &b_caloMETsumEt);
  fChain->SetBranchAddress("pfMET", &pfMET, &b_pfMET);
  fChain->SetBranchAddress("pfMETPhi", &pfMETPhi, &b_pfMETPhi);
  fChain->SetBranchAddress("pfMETsumEt", &pfMETsumEt, &b_pfMETsumEt);
  fChain->SetBranchAddress("pfMETmEtSig", &pfMETmEtSig, &b_pfMETmEtSig);
  fChain->SetBranchAddress("pfMETSig", &pfMETSig, &b_pfMETSig);
  fChain->SetBranchAddress("pfMET_T1JERUp", &pfMET_T1JERUp, &b_pfMET_T1JERUp);
  fChain->SetBranchAddress("pfMET_T1JERDo", &pfMET_T1JERDo, &b_pfMET_T1JERDo);
  fChain->SetBranchAddress("pfMET_T1JESUp", &pfMET_T1JESUp, &b_pfMET_T1JESUp);
  fChain->SetBranchAddress("pfMET_T1JESDo", &pfMET_T1JESDo, &b_pfMET_T1JESDo);
  fChain->SetBranchAddress("pfMET_T1UESUp", &pfMET_T1UESUp, &b_pfMET_T1UESUp);
  fChain->SetBranchAddress("pfMET_T1UESDo", &pfMET_T1UESDo, &b_pfMET_T1UESDo);
  fChain->SetBranchAddress("pfMETPhi_T1JESUp", &pfMETPhi_T1JESUp, &b_pfMETPhi_T1JESUp);
  fChain->SetBranchAddress("pfMETPhi_T1JESDo", &pfMETPhi_T1JESDo, &b_pfMETPhi_T1JESDo);
  fChain->SetBranchAddress("pfMETPhi_T1UESUp", &pfMETPhi_T1UESUp, &b_pfMETPhi_T1UESUp);
  fChain->SetBranchAddress("pfMETPhi_T1UESDo", &pfMETPhi_T1UESDo, &b_pfMETPhi_T1UESDo);
  if (!sample.isData) {

    fChain->SetBranchAddress("genMET", &genMET, &b_genMET);
    fChain->SetBranchAddress("genMETPhi", &genMETPhi, &b_genMETPhi);
    fChain->SetBranchAddress("pdf", &pdf, &b_pdf);
    fChain->SetBranchAddress("pthat", &pthat, &b_pthat);
    fChain->SetBranchAddress("processID", &processID, &b_processID);
    fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
    fChain->SetBranchAddress("genHT", &genHT, &b_genHT);
    fChain->SetBranchAddress("pdfWeight", &pdfWeight, &b_pdfWeight);
    fChain->SetBranchAddress("pdfSystWeight", &pdfSystWeight, &b_pdfSystWeight);
    fChain->SetBranchAddress("nPUInfo", &nPUInfo, &b_nPUInfo);
    fChain->SetBranchAddress("nPU", &nPU, &b_nPU);
    fChain->SetBranchAddress("puBX", &puBX, &b_puBX);
    fChain->SetBranchAddress("puTrue", &puTrue, &b_puTrue);
    fChain->SetBranchAddress("nMC", &nMC, &b_nMC);
    fChain->SetBranchAddress("mcPID", &mcPID, &b_mcPID);
    fChain->SetBranchAddress("mcVtx", &mcVtx, &b_mcVtx);
    fChain->SetBranchAddress("mcVty", &mcVty, &b_mcVty);
    fChain->SetBranchAddress("mcVtz", &mcVtz, &b_mcVtz);
    fChain->SetBranchAddress("mcPt", &mcPt, &b_mcPt);
    fChain->SetBranchAddress("mcMass", &mcMass, &b_mcMass);
    fChain->SetBranchAddress("mcEta", &mcEta, &b_mcEta);
    fChain->SetBranchAddress("mcPhi", &mcPhi, &b_mcPhi);
    fChain->SetBranchAddress("mcE", &mcE, &b_mcE);
    fChain->SetBranchAddress("mcEt", &mcEt, &b_mcEt);
    fChain->SetBranchAddress("mcStatus", &mcStatus, &b_mcStatus);
    fChain->SetBranchAddress("mcStatusFlag", &mcStatusFlag, &b_mcStatusFlag);
    fChain->SetBranchAddress("mcIndex", &mcIndex, &b_mcIndex);
  }
}

Int_t monoJetAnalysis::GetEntry(Long64_t entry)
{
  // Read contents of entry.
  if (!fChain) return 0;
  return fChain->GetEntry(entry);
}

Long64_t monoJetAnalysis::LoadTree(Long64_t entry)
{
  // Set the environment to read one entry
  if (!fChain) return -5;
  Long64_t centry = fChain->LoadTree(entry);
  if (centry < 0) return centry;
  if (fChain->GetTreeNumber() != fCurrent) {
    fCurrent = fChain->GetTreeNumber();
  }
  return centry;
}

void monoJetAnalysis::QCDVariations(float event_weight) {
  /* 14 Histograms
     up  dn
     d1K_NLO     11  12
     d2K_NLO     13  14
     d3K_NLO     15  16
     d1K_EW      23  24
     d2K_EW      25  26
     d3K_EW      27  28
     dK_NLO_mix  29  30 
     */
  string uncnames[7] = {"QCD_Scale","QCD_Shape","QCD_Proc","NNLO_EWK","NNLO_Miss","NNLO_Sud","QCD_EWK_Mix"};
  // Initializing
  if ( !scaleUncs.contains(uncnames[0]) ) {
    string hnames[7] = {"d1K_NLO","d2K_NLO","d3K_NLO","d1kappa_EW","d2kappa_EW","d3kappa_EW","dK_NLO_mix"};
    TFile* file = NULL;
    string prefix = "";
    if (isW_or_ZJet()) {
      if (sample.type == WJets || sample.type == WJets_NLO) {
	file = TFile::Open("RootFiles/WJets_NLO_EWK.root");
	prefix = "evj_pTV_";
      } else if (sample.type == ZJets || sample.type == ZJets_NLO) {
	file = TFile::Open("RootFiles/ZJets_NLO_EWK.root");
	prefix = "vvj_pTV_";
      } else if (sample.type == DYJets || sample.type == DYJets_NLO) {
	file = TFile::Open("RootFiles/DYJets_NLO_EWK.root");
	prefix = "eej_pTV_";
      }
      th1fmap["K_NLO_QCD"] = (TH1F*)file->Get( (prefix+"K_NLO").c_str() );
      th1fmap["K_EW"]      = (TH1F*)file->Get( (prefix+"kappa_EW").c_str() );
    }

    for (int i = 0; i < 7; i++) {
      string name = uncnames[i];
      TH1F* histo = NULL;
      if (isW_or_ZJet()) histo = (TH1F*)file->Get( (prefix+hnames[i]).c_str() );
      scaleUncs.addUnc(name,histo);
    }
  }

  for (int i = 0; i < 7; i++) {
    string name = uncnames[i];
    float weightUp = event_weight;
    float weightDn = event_weight;

    if (isW_or_ZJet()) {
      float unc = scaleUncs.getBin(name,bosonPt);
      float k_qcd = th1fmap.getBin("K_NLO_QCD",bosonPt);
      float k_ewk = th1fmap.getBin("K_EW",bosonPt);
      if ( name.find("NNLO") != string::npos || name.find("EWK") != string::npos ) unc *= k_ewk;
      else if ( name.find("QCD") != string::npos ) unc *= k_qcd;
      weightUp += unc;
      weightDn -= unc;
    }
    scaleUncs.setUnc(name,weightUp,weightDn);
  }
}

#endif
