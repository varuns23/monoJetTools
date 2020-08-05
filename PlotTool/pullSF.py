"""
Draw scale factor plots
Usage: python PlotTool/pullSF.py
"""

from PlotTool import *
from ROOT import *
import config

sf2017 = {
    'pileup':{file:'pileup/PU_Central.root',id:'pileup',range:'True Number of Interactions'},
    'nlo_ewk_w':{file:'theory/merged_kfactors_wjets.root',id:'kfactor_monojet_ewk',range:'Gen Boson P_{T}'},
    'nlo_qcd_w':{file:'theory/2017_gen_v_pt_qcd_sf.root',id:'wjet_dress_monojet',range:'Gen Boson P_{T}'},
    'nnlo_qcd_w':{file:'theory/lindert_qcd_nnlo_sf.root',id:'evj',range:'Gen Boson P_{T}'},
    'nlo_ewk_z':{file:'theory/merged_kfactors_zjets.root',id:'kfactor_monojet_ewk',range:'Gen Boson P_{T}'},
    'nlo_qcd_z':{file:'theory/2017_gen_v_pt_qcd_sf.root',id:'dy_dress_monojet',range:'Gen Boson P_{T}'},
    'nnlo_qcd_z':{file:'theory/lindert_qcd_nnlo_sf.root',id:'eej',range:'Gen Boson P_{T}'},
    'nlo_ewk_g':{file:'theory/merged_kfactors_gjets.root',id:'kfactor_monojet_ewk',range:'Gen Boson P_{T}'},
    'nlo_qcd_g':{file:'theory/merged_kfactors_gjets.root',id:'kfactor_monojet_qcd',range:'Gen Boson P_{T}'},
    'nnlo_qcd_g':{file:'theory/lindert_qcd_nnlo_sf.root',id:'aj',range:'Gen Boson P_{T}'},
    'ele_reco':{file:'egamma/2017_egammaEffi_txt_EGM2D_runBCDEF_passingRECO.root',id:'EGamma_SF2D',range:('SC Eta','P_{T}')},
    'ele_id_loose':{file:'egamma/2017_ElectronWPVeto_Fall17V2.root',id:'EGamma_SF2D',range:('SC Eta','P_{T}')},
    'ele_id_tight':{file:'egamma/2017_ElectronTight.root',id:'EGamma_SF2D',range:('SC Eta','P_{T}')},
    'muon_id_loose':{file:'muon/2017_RunBCDEF_SF_ID.root',id:'NUM_LooseID_DEN_genTracks_pt_abseta',range:('P_{T}','|Eta|')},
    'muon_id_tight':{file:'muon/2017_RunBCDEF_SF_ID.root',id:'NUM_TightID_DEN_genTracks_pt_abseta',range:('P_{T}','|Eta|')},
    'muon_iso_loose':{file:'muon/2017_RunBCDEF_SF_ISO.root',id:'NUM_LooseRelIso_DEN_LooseID_pt_abseta',range:('P_{T}','|Eta|')},
    'muon_iso_tight':{file:'muon/2017_RunBCDEF_SF_ISO.root',id:'NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta',range:('P_{T}','|Eta|')},
    'photon_id_tight':{file:'egamma/2017_PhotonsMedium_capped.root',id:'EGamma_SF2D',range:('SC Eta','P_{T}')},
    'photon_csev':{file:'egamma/CSEV_ScaleFactors_2017.root',id:'Medium_ID',range:None},
    'trigger_electron':{file:'trigger/electron_trigger_sf_2017.root',id:'EGamma_SF2D',range:('SC Eta','P_{T}')},
    'trigger_met':{file:'trigger/met_trigger_sf.root',id:'120pfht_hltmu_1m_2017',range:'Recoil'}
}

gROOT.SetBatch(1)

def pullTH1F(info):
    print 'Pulling %s' % info[type]
    c = TCanvas(info[type],info[type],1000,1000)
    gStyle.SetOptStat(0)
    c.SetGrid()
    hs = info[id]
    hs.Draw("hist")
    hs.SetLineWidth(2)
    if info[range] is not None: hs.GetXaxis().SetTitle(info[range])
    hs.GetYaxis().SetTitle("SF")
    hs.SetTitle(info[type])
    SaveAs(c,info[type],year=config.version,sub="SF_Histograms")

def pullTH2F(info):
    print 'Pulling %s' % info[type]
    c = TCanvas(info[type],info[type],1200,800)
    gStyle.SetOptStat(0)
    gStyle.SetPaintTextFormat("4.3f");
    c.SetGrid()
    c.SetRightMargin(0.15);
    c.SetLeftMargin(0.15);
    c.SetBottomMargin(0.15);
    hs = info[id]
    hs.Draw("COLZ TEXT")
    hs.GetXaxis().SetTitle(info[range][0])
    hs.GetYaxis().SetTitle(info[range][1])
    hs.SetTitle(info[type])
    SaveAs(c,info[type],year=config.version,sub="SF_Histograms")
    
for sf,info in sf2017.iteritems():
    info[type] = sf
    info[file] = TFile.Open('RootFiles/%s' % info[file])
    info[id] = info[file].Get(info[id])
    if any( type(info[id]) == hsclass for hsclass in (TH1F,TH1D) ): pullTH1F(info)
    if any( type(info[id]) == hsclass for hsclass in (TH2F,TH2D) ): pullTH2F(info)
