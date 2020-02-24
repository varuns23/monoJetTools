#ifndef monoJetYear_C
#define monoJetYear_C
#include "monoJetYear.h"
#include "Utilities.h"

using namespace std;

int monoJetAnalysis::YEAR = 2018;
const string monoJetYear::SRDATA = "A";

void monoJetYear::initVars() {
  monoJetAnalysis::initVars();
}

void monoJetYear::BookHistos(int i,string histname) {
  monoJetAnalysis::BookHistos(i,histname);
}

void monoJetYear::fillHistos(int nhist,float event_weight) {
  monoJetAnalysis::fillHistos(nhist,event_weight);
}

void monoJetYear::SetScalingHistos() {
  monoJetAnalysis::SetScalingHistos();

  if ( CROBJECT == Electron ) {
    // Electron SF
    TFile* f_ele_reco = TFile::Open("RootFiles/egamma/2018_egammaEffi_txt_EGM2D_updatedAll.root");
    TFile* f_ele_id_loose = TFile::Open("RootFiles/egamma/2018_ElectronWPVeto_Fall17V2.root");
    TFile* f_ele_id_tight = TFile::Open("RootFiles/egamma/2018_ElectronTight.root");
    th2fmap["ele_reco"] = (TH2F*)f_ele_reco->Get("EGamma_SF2D");
    th2fmap["ele_id_loose"] = (TH2F*)f_ele_id_loose->Get("EGamma_SF2D");
    th2fmap["ele_id_tight"] = (TH2F*)f_ele_id_tight->Get("EGamma_SF2D");
  } else if ( CROBJECT == Muon ){
    // Muon SF
    TFile* f_muon_id = TFile::Open("RootFiles/muon/2018_RunABCD_SF_ID.root");
    TFile* f_muon_iso= TFile::Open("RootFiles/muon/2018_RunABCD_SF_ISO.root");
    th2fmap["muon_id_loose"] = (TH2F*)f_muon_id->Get("NUM_LooseID_DEN_TrackerMuons_pt_abseta");
    th2fmap["muon_id_tight"] = (TH2F*)f_muon_id->Get("NUM_TightID_DEN_TrackerMuons_pt_abseta");
    th2fmap["muon_iso_loose"]= (TH2F*)f_muon_iso->Get("NUM_LooseRelIso_DEN_LooseID_pt_abseta");
    th2fmap["muon_iso_tight"]= (TH2F*)f_muon_iso->Get("NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta");
  } else if ( CROBJECT == Photon ){
    // Photon SF
    TFile* f_photon_id_tight = TFile::Open("RootFiles/egamma/2018_PhotonsMedium_capped.root");
    TFile* f_photon_csev = TFile::Open("RootFiles/egamma/CSEV_2018.root");
    th2fmap["photon_id_tight"] = (TH2F*)f_photon_id_tight->Get("EGamma_SF2D");
    th2fmap["photon_csev"] = (TH2F*)f_photon_csev->Get("eleVeto_SF");
  }
}
#endif
