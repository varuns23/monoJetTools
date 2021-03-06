#ifndef monoJetYear_C
#define monoJetYear_C
#include "monoJetYear.h"
#include "Utilities.h"

using namespace std;

int monoJetAnalysis::YEAR = 2017;
const TString monoJetYear::SRDATA = "B";

void monoJetYear::initTree(TTree* tree) {
  monoJetAnalysis::initTree(tree);
  tree->Branch("prefiring",&prefiringweight,"prefiring/D");
  tree->Branch("prefiringUp",&prefiringweightup,"prefiringup/D");
  tree->Branch("prefiringDown",&prefiringweightdown,"prefiringdown/D");
}

void monoJetYear::initVars() {
  monoJetAnalysis::initVars();
}

void monoJetYear::BookHistos(int i,TString histname) {
  monoJetAnalysis::BookHistos(i,histname);
}

void monoJetYear::fillHistos(int nhist,float event_weight) {
  monoJetAnalysis::fillHistos(nhist,event_weight);
}

void monoJetYear::SetScalingHistos() {
  monoJetAnalysis::SetScalingHistos();

  //This is the PU histogram obtained from Nick's recipe
  TFile *weights = TFile::Open("RootFiles/pileup/PU_Central_2017.root");
  TH1F* PU = (TH1F*)weights->Get("pileup");
  th1fmap["PU"] = PU;
  
  // Load trigger sf for electron trigger
  TFile* f_ele_trigger_sf = TFile::Open("RootFiles/trigger/electron_trigger_sf_2017.root");
  th2fmap["electron_trigger_data_eff"] = (TH2F*)f_ele_trigger_sf->Get("EGamma_EffData2D");
  th2fmap["electron_trigger_mc_eff"] = (TH2F*)f_ele_trigger_sf->Get("EGamma_EffMC2D");
  
  // Load trigger sf for met trigger
  TFile* f_met_trigger_sf = TFile::Open("RootFiles/trigger/met_trigger_sf.root");
  th1fmap["met_trigger_sf"] = (TH1F*)f_met_trigger_sf->Get("120pfht_hltmu_1m_2017");
  
  // Electron SF
  TFile* f_ele_reco = TFile::Open("RootFiles/egamma/2017_egammaEffi_txt_EGM2D_runBCDEF_passingRECO.root");
  TFile* f_ele_reco_pt_lt_20 = TFile::Open("RootFiles/egamma/2017_egammaEffi_txt_EGM2D_runBCDEF_passingRECO_lowEt.root");
  TFile* f_ele_id_loose = TFile::Open("RootFiles/egamma/2017_ElectronWPVeto_Fall17V2.root");
  TFile* f_ele_id_tight = TFile::Open("RootFiles/egamma/2017_ElectronTight.root");
  th2fmap["ele_reco"] = (TH2F*)f_ele_reco->Get("EGamma_SF2D");
  th2fmap["ele_reco_pt_lt_20"] = (TH2F*)f_ele_reco_pt_lt_20->Get("EGamma_SF2D");
  th2fmap["ele_id_loose"] = (TH2F*)f_ele_id_loose->Get("EGamma_SF2D");
  th2fmap["ele_id_tight"] = (TH2F*)f_ele_id_tight->Get("EGamma_SF2D");
  
  // Muon SF
  TFile* f_muon_id = TFile::Open("RootFiles/muon/2017_RunBCDEF_SF_ID.root");
  TFile* f_muon_iso= TFile::Open("RootFiles/muon/2017_RunBCDEF_SF_ISO.root");
  th2fmap["muon_id_loose"] = (TH2F*)f_muon_id->Get("NUM_LooseID_DEN_genTracks_pt_abseta");
  th2fmap["muon_id_tight"] = (TH2F*)f_muon_id->Get("NUM_TightID_DEN_genTracks_pt_abseta");
  th2fmap["muon_iso_loose"]= (TH2F*)f_muon_iso->Get("NUM_LooseRelIso_DEN_LooseID_pt_abseta");
  th2fmap["muon_iso_tight"]= (TH2F*)f_muon_iso->Get("NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta");
  
  // Photon SF
  TFile* f_photon_id_tight_2d = TFile::Open("RootFiles/egamma/2017_PhotonsMedium_capped.root");
  TFile* f_photon_id_tight_1d = TFile::Open("RootFiles/egamma/photon_medium_id_sf_v0.root");
  TFile* f_photon_csev = TFile::Open("RootFiles/egamma/CSEV_ScaleFactors_2017.root");
  th2fmap["photon_id_tight"] = (TH2F*)f_photon_id_tight_2d->Get("EGamma_SF2D");
  th1fmap["photon_id_tight"] = (TH1F*)f_photon_id_tight_1d->Get("photon_medium_id_sf_2017");
  th1fmap["photon_csev"] = (TH1F*)f_photon_csev->Get("Medium_ID");

  // Tau SF
  TFile* f_tau_sf = TFile::Open("RootFiles/tau/tau_sf.root");
  th1fmap["tau_vloose"] = (TH1F*)f_tau_sf->Get("tau_sf_VLoose_2017_cent");
  th1fmap["tau_vloose_up"] = (TH1F*)f_tau_sf->Get("tau_sf_VLoose_2017_up");
  th1fmap["tau_vloose_down"] = (TH1F*)f_tau_sf->Get("tau_sf_VLoose_2017_down");

  // BTag SF
  btag_csv = new BTagCSV("RootFiles/btag/DeepCSV_94XSF_WP_V4_B_F.csv");
}
#endif
