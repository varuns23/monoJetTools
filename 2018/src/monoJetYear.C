#ifndef monoJetYear_C
#define monoJetYear_C
#include "monoJetYear.h"
#include "Utilities.h"

using namespace std;

int monoJetAnalysis::YEAR = 2018;
const TString monoJetYear::SRDATA = "A";
bool monoJetAnalysis::nlo_gjets = true;

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
  TFile *weights = TFile::Open("RootFiles/pileup/PU_Central_2018.root");
  TH1F* PU = (TH1F*)weights->Get("pileup");
  th1fmap["PU"] = PU;
  
  // Load trigger sf for electron trigger
  TFile* f_ele_trigger_sf = TFile::Open("RootFiles/trigger/electron_trigger_sf_2018.root");
  th2fmap["electron_trigger_data_eff"] = (TH2F*)f_ele_trigger_sf->Get("EGamma_EffData2D");
  th2fmap["electron_trigger_mc_eff"] = (TH2F*)f_ele_trigger_sf->Get("EGamma_EffMC2D");
    
  // Load trigger sf for met trigger
  TFile* f_met_trigger_sf = TFile::Open("RootFiles/trigger/met_trigger_sf.root");
  th1fmap["met_trigger_sf"] = (TH1F*)f_met_trigger_sf->Get("120pfht_hltmu_1m_2018");
  
  // Electron SF
  TFile* f_ele_reco = TFile::Open("RootFiles/egamma/2018_egammaEffi_txt_EGM2D_updatedAll.root");
  TFile* f_ele_id_loose = TFile::Open("RootFiles/egamma/2018_ElectronWPVeto_Fall17V2.root");
  TFile* f_ele_id_tight = TFile::Open("RootFiles/egamma/2018_ElectronTight.root");
  th2fmap["ele_reco"] = (TH2F*)f_ele_reco->Get("EGamma_SF2D");
  th2fmap["ele_id_loose"] = (TH2F*)f_ele_id_loose->Get("EGamma_SF2D");
  th2fmap["ele_id_tight"] = (TH2F*)f_ele_id_tight->Get("EGamma_SF2D");
    
  // Muon SF
  TFile* f_muon_id = TFile::Open("RootFiles/muon/2018_RunABCD_SF_ID.root");
  TFile* f_muon_iso= TFile::Open("RootFiles/muon/2018_RunABCD_SF_ISO.root");
  th2fmap["muon_id_loose"] = (TH2F*)f_muon_id->Get("NUM_LooseID_DEN_TrackerMuons_pt_abseta");
  th2fmap["muon_id_tight"] = (TH2F*)f_muon_id->Get("NUM_TightID_DEN_TrackerMuons_pt_abseta");
  th2fmap["muon_iso_loose"]= (TH2F*)f_muon_iso->Get("NUM_LooseRelIso_DEN_LooseID_pt_abseta");
  th2fmap["muon_iso_tight"]= (TH2F*)f_muon_iso->Get("NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta");
    
  // Photon SF
  TFile* f_photon_id_tight_2d = TFile::Open("RootFiles/egamma/2018_PhotonsMedium_capped.root");
  TFile* f_photon_id_tight_1d = TFile::Open("RootFiles/egamma/photon_medium_id_sf_v0.root");
  TFile* f_photon_csev = TFile::Open("RootFiles/egamma/CSEV_2018.root");
  th2fmap["photon_id_tight"] = (TH2F*)f_photon_id_tight_2d->Get("EGamma_SF2D");
  th1fmap["photon_id_tight"] = (TH1F*)f_photon_id_tight_1d->Get("photon_medium_id_sf_2018");
  th2fmap["photon_csev"] = (TH2F*)f_photon_csev->Get("eleVeto_SF");

  // Tau SF
  TFile* f_tau_sf = TFile::Open("RootFiles/tau/tau_sf.root");
  th1fmap["tau_vloose"] = (TH1F*)f_tau_sf->Get("tau_sf_VLoose_2018_cent");
  th1fmap["tau_vloose_up"] = (TH1F*)f_tau_sf->Get("tau_sf_VLoose_2018_up");
  th1fmap["tau_vloose_down"] = (TH1F*)f_tau_sf->Get("tau_sf_VLoose_2018_down");

  btag_csv = new BTagCSV("RootFiles/btag/DeepCSV_102XSF_WP_V1.csv");
}
#endif
