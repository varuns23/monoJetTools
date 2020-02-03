#ifndef monoJetYear_C
#define monoJetYear_C
#include "monoJetYear.h"
#include "Utilities.h"

using namespace std;

const string monoJetYear::SRDATA = "B";

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
  
  // Electron Scale Factors
  TFile *f_eleReconstrucSF_highpt=new TFile("RootFiles/egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root");
  TFile *f_eleIDeffSF_loose=new TFile("RootFiles/2017_ElectronLoose.root");
  TFile *f_eleIDeffSF_tight=new TFile("RootFiles/2017_ElectronTight.root");
  TFile *f_eleTriggSF = new TFile("RootFiles/EleTriggSF.root");
  th2fmap["eleRecoSF_highpt"]=(TH2F*) f_eleReconstrucSF_highpt->Get("EGamma_SF2D");
  th2fmap["eleIDSF_loose"]=(TH2F*) f_eleIDeffSF_loose->Get("EGamma_SF2D");
  th2fmap["eleIDSF_tight"]=(TH2F*) f_eleIDeffSF_tight->Get("EGamma_SF2D");
  th2fmap["eleTriggSF"] = (TH2F*) f_eleTriggSF->Get("EleTriggSF_abseta_pt");
  
  // Muon Scale Factors
  TFile *f_muSF_ISO = new TFile("RootFiles/RunBCDEF_SF_ISO.root");
  TFile *f_muSF_ID = new TFile("RootFiles/RunBCDEF_SF_ID.root");
  th2fmap["tightMuSF_ISO_abseta"] = (TH2F*)f_muSF_ISO->Get("NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta");
  th2fmap["looseMuSF_ISO_abseta"] = (TH2F*)f_muSF_ISO->Get("NUM_LooseRelIso_DEN_LooseID_pt_abseta");
  th2fmap["tightMuSF_ID_abseta"] = (TH2F*)f_muSF_ID->Get("NUM_TightID_DEN_genTracks_pt_abseta");
  th2fmap["looseMuSF_ID_abseta"] = (TH2F*)f_muSF_ID->Get("NUM_LooseID_DEN_genTracks_pt_abseta");
  
  // Photon Scale Factors
  TFile *f_phoIDeffSF_medium=new TFile("RootFiles/2017_PhotonsMedium.root");
  th2fmap["phoIDSF_medium"]=(TH2F*) f_phoIDeffSF_medium->Get("EGamma_SF2D");
}
#endif
