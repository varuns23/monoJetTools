#ifndef monoJetCutConfig_h
#define monoJetCutConfig_h

#include <iostream>

/* Cutflow Names */
const std::string s_TotalEvents = "Total Events";
const std::string s_Triggers = "Triggers";
const std::string s_METFilters = "MET Filters";
const std::string s_HEMVeto = "HEM Veto";
const std::string s_ElectronVeto = "Electron Veto";
const std::string s_MuonVeto = "Muon Veto";
const std::string s_PhotonVeto = "Photon Veto";
const std::string s_TauVeto = "Tau Veto";
const std::string s_BJetVeto = "BJet Veto";
const std::string s_minDPhiJetMET = "minDPhiJetMET";
const std::string s_dPFCaloMET = "dPFCaloMET";
const std::string s_Recoil = "Recoil";
const std::string s_JetSelection = "Jet Selection";

/* SingleLepCR Cutfow Names */
const std::string s_OneLooseEle = "One Loose Electron";
const std::string s_OneTightEle = "One Tight Electron";
const std::string s_pfMET50 = "pfMET50";
const std::string s_EleMET_MT = "Electron MET M_{T}";

const std::string s_OneLooseMu = "One Loose Muon";
const std::string s_OneTightMu = "One Tight Muon";
const std::string s_MuMET_MT = "Muon MET M_{T}";

/* DoubleLepCR Cutflow Names */
const std::string s_TwoLooseEle = "Two Loose Electron";
const std::string s_TwoLooseMu = "Two Loose Muon";
const std::string s_OppCharge = "Opp Charge";
const std::string s_ZMass = "ZMass";

/* GammaCR Cutflow Names */
const std::string s_OneLoosePho = "One Loose Photon";
const std::string s_OneTightPho = "One Tight Photon";
const std::string s_PhoPt230 = "Pho P_{T} > 230.0";

/* Jet Selection Floating Cuts */
const float jetCandPtCut = 100.;
const float jetCandEtaCut = 2.4;
const float jetCandNHFCut = 0.8;
const float jetCandCHFCut = 0.1;

/* Jet Veto Floating Cuts */
const float jetVetoPtCut = 30.;
const float jetVetoEtaCut = 2.4;

/* B-Jet Veto Floating Cuts */
const float bjetVetoPtCut = 20.;
const float bjetVetoEtaCut = 2.4;
const float bjetVetoCSVv2Cut_2016 = 0.8484;
const float bjetVetoCSVv2Cut_2017 = 0.8838;
const float bjetDeepCSVCut_2017 = 0.4941;
const float bjetDeepCSVCut_2018 = 0.4184;

/* Isolation Cone Cuts */
const float Iso4Cut = 0.4;
const float Iso5Cut = 0.5;

/* HEM Veto Pt Cuts */
const float eleHEMVetoPtCut = 40.;
const float jetHEMVetoPtCut = 30.;

/* Lepton Veto Floating Cuts */
const float eleTightPtCut  = 40.;
const float eleTightEtaCut = 2.5;

const float eleLoosePtCut  = 10.;
const float eleLooseEtaCut = 2.5;

const float eleEtaCutLow   = 1.479;
const float eleD0CutLow    = 0.05;
const float eleDzCutLow    = 0.1;

const float eleEtaCutHigh  = 1.479;
const float eleD0CutHigh   = 0.1;
const float eleDzCutHigh   = 0.2;

const float muTightPtCut   = 20.;
const float muTightEtaCut  = 2.4;

const float muLoosePtCut   = 10.;
const float muLooseEtaCut  = 2.4;

const float phoTightPtCut  = 230.;
const float phoTightEtaCut = 1.4442;

const float phoLoosePtCut  = 15.;
const float phoLooseEtaCut = 2.5;

const float tauLoosePtCut  = 18.;
const float tauLooseEtaCut = 2.3;

/* Signal Region Floating Cuts */
const float recoilCut     = 250.;
const float metRatioCut   = 0.5;
const float dPhiJetMETCut = 0.5;

/* Single Electron CR Floating Cuts */
const float pfMET50Cut    = 50.;

/* Single Lepton CR Floating Cuts */
const float lepMETMtCut   = 160.;

/* Double Lepton CR Floating Cuts */
const float diLeptonMassCutLow  = 60.;
const float diLeptonMassCutHigh = 120.;

#endif
