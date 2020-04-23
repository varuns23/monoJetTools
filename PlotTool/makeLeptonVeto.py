#!/usr/bin/env python

from ROOT import *
from sys import argv,path
from PlotTool import *
import config
import os

from PlotTool import parser
parser.parse_args()

config.mcinfo = ["WJets","QCD"]

gROOT.SetBatch(1)

nominal = Region(show=False,autovar=True)
lepveto = Region(show=False,autovar=True)

variable = parser.args.argv[0]

nominal.initiate(variable,cut="eleveto_sf==1 && muveto_sf==1 && tauveto_sf==1")

def plotLeptonVeto(nominal,lepveto):
    
    def ProcessVeto(process):
        nom = nominal[process].histo
        veto = lepveto[process].histo
        eff=GetRatio(veto,nom)
        
        c = TCanvas("c", "canvas",800,800);
        gStyle.SetOptStat(0);
        gStyle.SetLegendBorderSize(0);
        # c.SetLeftMargin(0.15);
        # c.SetLogy();
        # c.cd();
        
        pad1 = TPad("pad1","pad1",0.02,0.02,0.99,0.99);
        pad1.Draw(); pad1.cd();
        # pad1.SetLogy();
        pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);
        # pad1.SetBottomMargin(0.35);

        DataStyle(eff)
        eff.GetYaxis().SetTitle('%s Veto' % lepveto.lep)
        eff.GetYaxis().SetRangeUser(0,2)
        eff.Draw('lpex0')
        
        lumi_label = '%s' % float('%.3g' % (nominal.lumi/1000.)) + " fb^{-1}"
        if (parser.args.normalize): lumi_label="Normalized"
        texLumi,texCMS = getCMSText(lumi_label,nominal.year,scale=0.8)
        texLumi.Draw();
        texCMS.Draw();
        SaveAs(c,"%s_%s"%(lepveto.lep,lepveto.variable.base),year=nominal.year,sub="LeptonVeto/%s"%process)
    for process in ("WJets","QCD"): ProcessVeto(process)
#-- Electron Veto --#
lepveto.initiate(variable,"muveto_sf==1 && tauveto_sf==1")
lepveto.lep="Electron"
plotLeptonVeto(nominal,lepveto)

#-- Muon Veto --#
lepveto.initiate(variable,"eleveto_sf==1 && tauveto_sf==1")
lepveto.lep="Muon"
plotLeptonVeto(nominal,lepveto)

#-- Tau --#
lepveto.initiate(variable,"muveto_sf==1 && eleveto_sf==1")
lepveto.lep="Tau"
plotLeptonVeto(nominal,lepveto)
