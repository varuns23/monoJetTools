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

nominal.initiate(variable,weight="weight",cut="eleveto_sf==1 && muveto_sf==1 && tauveto_sf==1")

output = TFile("veto_sys.root","recreate")

lepmap = {
    "Electron":"ele",
    "Muon":"mu",
    "Tau":"tau"
}

def hs_style(hs,color):
    hs.SetMarkerColor(color)
    hs.SetMarkerStyle(20)
    hs.SetLineColor(color)

def save_sys(up,dn,output=output):
    output.cd()
    up.Write()
    dn.Write()

def plotLeptonVeto(nominal,lepveto,weight,cut):
    lepveto.initiate(variable,weight.format(var=""),cut.format(var=""))
    for process in config.mclist: lepveto[process].no = lepveto[process].histo.Clone()

    lepveto.initiate(variable,weight.format(var="Up"),cut.format(var="Up"))
    for process in config.mclist: lepveto[process].up = lepveto[process].histo.Clone()

    lepveto.initiate(variable,weight.format(var="Down"),cut.format(var="Down"))
    for process in config.mclist: lepveto[process].dn = lepveto[process].histo.Clone()

    def VetoUncertainty(sf,sfup,sfdn):
        up = sfup.Clone(); up.Divide(sf)
        dn = sfdn.Clone(); dn.Divide(sf)
        
        vetounc = Nuisance(lepveto.lep,"%sveto_%s"%(lepmap[lepveto.lep],nominal.year),up,dn,nominal[process].histo)
        sf_up,sf_dn = vetounc.GetScale()
        up_smooth = sf_up.Clone(vetounc.name)
        up_smooth.Smooth()
        dn_smooth = sf_dn.Clone(vetounc.name+"_Down")
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
        sf_up.GetYaxis().SetTitle("%s Veto Uncertainty"%(lepveto.lep))
        sf_up.GetXaxis().SetTitle(nominal.variable.xaxis_title)
        sf_dn.Draw("hist p same")
        up_smooth.Draw("hist l same")
        dn_smooth.Draw("hist l same")

        leg = getLegend(xmax=0.75,ymin=0.7,ymax=0.9,scale=0.8)
        leg.AddEntry(sf_up,"%s Veto"%lepveto.lep,"p")
        leg.AddEntry(up_smooth,"Smoothed","l")
        leg.Draw()
        
        lumi_label = '%s' % float('%.3g' % (nominal.lumi/1000.)) + " fb^{-1}"
        if (parser.args.normalize): lumi_label="Normalized"
        texLumi,texCMS = getCMSText(lumi_label,nominal.year,scale=0.8)
        texLumi.Draw();
        texCMS.Draw();
        
        
        SaveAs(c,"%s_%s"%(lepveto.lep,lepveto.variable.base),year=nominal.year,sub="LeptonVeto/%s/Uncertainty/"%process)
    def ProcessVeto(process):
        hardveto = nominal[process].histo

        vetosf = lepveto[process].no
        vetosfUp = lepveto[process].up
        vetosfDn = lepveto[process].dn

        vetosf.Divide(hardveto)
        vetosfUp.Divide(hardveto)
        vetosfDn.Divide(hardveto)

        VetoUncertainty(vetosf,vetosfUp,vetosfDn)
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

        hs_style(vetosf,kBlack)
        hs_style(vetosfUp,kBlue)
        hs_style(vetosfDn,kBlue)
        
        binlist = list(vetosf)[1:-1]+list(vetosfUp)[1:-1]+list(vetosfDn)[1:-1]
        ymin = min(binlist)
        ymax = max(binlist)
        diff = ymax - ymin
        ymax += diff
        ymin -= diff
        # ymin = 0.9
        # ymax = 1.1
        for hs in (vetosf,vetosfUp,vetosfDn):
            hs.Draw("hist pl same");
            hs.GetYaxis().SetRangeUser(ymin,ymax);
            hs.GetYaxis().SetTitle("Veto SF/Hard Veto")
            hs.GetXaxis().SetTitle(nominal.variable.xaxis_title)

        leg = getLegend(xmax=0.75,ymin=0.7,ymax=0.9,scale=0.8)
        leg.AddEntry(vetosf,"Nominal Weight","pl")
        leg.AddEntry(vetosfUp,"Weight Variation","pl")
        leg.Draw()
        
        lumi_label = '%s' % float('%.3g' % (nominal.lumi/1000.)) + " fb^{-1}"
        if (parser.args.normalize): lumi_label="Normalized"
        texLumi,texCMS = getCMSText(lumi_label,nominal.year,scale=0.8)
        texLumi.Draw();
        texCMS.Draw();
        SaveAs(c,"%s_%s"%(lepveto.lep,lepveto.variable.base),year=nominal.year,sub="LeptonVeto/%s"%process)
    for process in config.mclist: ProcessVeto(process)
#-- Electron Veto --#
lepveto.lep="Electron"
plotLeptonVeto(nominal,lepveto,"weight*eleveto_sf{var}","eleveto_sf{var} <= 1 && muveto_sf==1 && tauveto_sf==1")

#-- Muon Veto --#
lepveto.lep="Muon"
plotLeptonVeto(nominal,lepveto,"weight*muveto_sf{var}","eleveto_sf==1 && muveto_sf{var} <= 1 && tauveto_sf==1")

#-- Tau --#
lepveto.lep="Tau"
plotLeptonVeto(nominal,lepveto,"weight*tauveto_sf{var}","muveto_sf==1 && eleveto_sf==1 && tauveto_sf{var} <= 1")
