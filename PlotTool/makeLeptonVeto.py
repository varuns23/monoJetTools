#!/usr/bin/env python

from ROOT import *
from sys import argv,path
from PlotTool import *
import config
import os

from PlotTool import parser
parser.parse_args()

config.mclist = ["WJets","QCD"]

gROOT.SetBatch(1)

nominal = Region(show=False,autovar=True)
lepveto = Region(show=False,autovar=True)

variable = parser.args.argv[0]

nominal.initiate(variable,cut="eleveto_sf==1 && muveto_sf==1 && tauveto_sf==1")

def plotLeptonVeto(nominal,lepveto):
    
    def ProcessVeto(process):
        nom = nominal[process].histo
        veto = lepveto[process].histo
        sf=GetRatio(veto,nom)
        
        c = TCanvas("c", "canvas",800,800);
        gStyle.SetOptStat(0);
        gStyle.SetLegendBorderSize(0);
        # c.SetLeftMargin(0.15);
        # c.SetLogy();
        # c.cd();
        
        pad1 = TPad("pad1","pad1",0.01,0.25,0.99,0.99);
        pad1.Draw(); pad1.cd();
        pad1.SetLogy();
        pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);
        pad1.SetBottomMargin(0);

        DataStyle(nom)
        MCStyle(veto,kRed-7)

        veto.Draw("hist same")
        nom.Draw("pex0 same")

        leg = getLegend()
        leg.AddEntry(nom,"Nominal Hard Veto","pl")
        leg.AddEntry(veto,"%s Veto SF"%lepveto.lep,"f")
        leg.Draw()
        
        lumi_label = '%s' % float('%.3g' % (nominal.lumi/1000.)) + " fb^{-1}"
        if (parser.args.normalize): lumi_label="Normalized"
        texLumi,texCMS = getCMSText(lumi_label,nominal.year)
        texLumi.Draw();
        texCMS.Draw();

        c.cd()
        pad2 = TPad("pad2","pad2",0.01,0.01,0.99,0.25)
        pad2.Draw(); pad2.cd();
        pad2.SetFillColor(0); pad2.SetFrameBorderMode(0); pad2.SetBorderMode(0);
        pad2.SetTopMargin(0.)
        pad2.SetBottomMargin(0.35);
        pad2.SetGridy()

        RatioStyle(sf,rymin=0,rymax=2,xname=nominal.variable.xaxis_title,yname="SF/Hard")
        sf.Draw("pex0")
        
        
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
