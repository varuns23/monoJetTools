#!/usr/bin/env python

from ROOT import *
from sys import argv,path
from PlotTool import *
import config
import os

from PlotTool import parser
parser.add_argument("--plot",help="Produce systematic uncertainty plots",action="store_true")
parser.parse_args()

config.mclist = ["WJets"]

gROOT.SetBatch(1)

nominal = Region(show=False,autovar=True)
lepveto = Region(show=False,autovar=True)

variable = parser.args.argv[0]

nominal.initiate(variable,cut="eleveto_sf==1 && muveto_sf==1 && tauveto_sf==1")

output = TFile("veto_sys.root","recreate")

lepmap = {
    "Electron":"ele",
    "Muon":"mu",
    "Tau":"tau"
}

def save_sys(up,dn,output=output):
    output.cd()
    up.Write()
    dn.Write()

def plotLeptonVeto(nominal,lepveto):
    
    def ProcessVeto(process):
        nom = nominal[process].histo
        veto = lepveto[process].histo
        sf_up=GetRatio(veto,nom)
        sf_dn=sf_up.Clone(); sf_dn.Divide(sf_up); sf_dn.Divide(sf_up);
        up_smooth = sf_up.Clone("%sveto"%lepmap[lepveto.lep])
        up_smooth.Smooth()
        dn_smooth = sf_dn.Clone("%sveto_Down"%lepmap[lepveto.lep])
        dn_smooth.Smooth()

        save_sys(up_smooth,dn_smooth)
        if not parser.args.plot: return
        
        c = TCanvas("c", "canvas",800,800);
        gStyle.SetOptStat(0);
        gStyle.SetLegendBorderSize(0);
        # c.SetLeftMargin(0.15);
        # c.SetLogy();
        # c.cd();
        
        pad1 = TPad("pad1","pad1",0.,0.,1.0,1.0);
        pad1.Draw(); pad1.cd();
        pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);
        pad1.SetLeftMargin(0.15)
        pad1.SetGridy()
        # pad1.SetBottomMargin(0.35);

        upcol=kBlack
        dncol=kBlack
        DataStyle(sf_up);
        sf_up.SetMarkerColor(upcol)
        up_smooth.SetLineColor(upcol)
        up_smooth.SetLineWidth(2)
        DataStyle(sf_dn);
        sf_dn.SetMarkerColor(dncol)
        dn_smooth.SetLineColor(dncol)
        dn_smooth.SetLineWidth(2)

        binlist = list(sf_up)[1:-1]+list(sf_dn)[1:-1]
        ymin = min(binlist)
        ymax = max(binlist)
        diff = ymax - ymin
        ymax += diff
        ymin -= diff
        # ymin = 0.9
        # ymax = 1.1
        sf_up.Draw("hist p");
        sf_up.GetYaxis().SetRangeUser(ymin,ymax);
        sf_up.GetYaxis().SetTitle("Veto SF/Hard Veto")
        sf_up.GetXaxis().SetTitle(nominal.variable.xaxis_title)
        sf_dn.Draw("hist p same")
        up_smooth.Draw("hist l same")
        dn_smooth.Draw("hist l same")

        leg = getLegend(xmax=0.75,ymin=0.7,ymax=0.9,scale=0.8)
        leg.AddEntry(sf_up,"Nominal Correction","p")
        leg.AddEntry(up_smooth,"Smoothed","l")
        leg.Draw()
        
        lumi_label = '%s' % float('%.3g' % (nominal.lumi/1000.)) + " fb^{-1}"
        if (parser.args.normalize): lumi_label="Normalized"
        texLumi,texCMS = getCMSText(lumi_label,nominal.year,scale=0.8)
        texLumi.Draw();
        texCMS.Draw();
        
        
        SaveAs(c,"%s_%s"%(lepveto.lep,lepveto.variable.base),year=nominal.year,sub="LeptonVeto/%s"%process)
    for process in config.mclist: ProcessVeto(process)
#-- Electron Veto --#
lepveto.initiate(variable,cut="eleveto_sf > 0 && muveto_sf==1 && tauveto_sf==1")
lepveto.lep="Electron"
plotLeptonVeto(nominal,lepveto)

#-- Muon Veto --#
lepveto.initiate(variable,cut="eleveto_sf==1 && muveto_sf > 0 && tauveto_sf==1")
lepveto.lep="Muon"
plotLeptonVeto(nominal,lepveto)

#-- Tau --#
lepveto.initiate(variable,cut="muveto_sf==1 && eleveto_sf==1 && tauveto_sf > 0")
lepveto.lep="Tau"
plotLeptonVeto(nominal,lepveto)
