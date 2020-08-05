#!/usr/bin/env python

"""
Comparison between rounded variable and non-rounded variable
Usage: python PlotTool/compareRounded.py variable1 variable2
"""

from sys import argv,path
from PlotTool import *
from ROOT import *
import config
import os

group = parser.add_group(__file__,__doc__,"Script")

gROOT.SetBatch(1)

sample = Region(show=0,autovar=True)

def compare(variable,proc):
    sample.initiate(variable); sample.setSumOfBkg()
    trueBKG = sample[proc].histo.Clone()
    sample.initiate('%srounded'%variable); sample.setSumOfBkg()
    roundedBKG = sample[proc].histo.Clone()
    
    xmin,xmax = roundedBKG.GetXaxis().GetXmin(),roundedBKG.GetXaxis().GetXmax()
    
    trueBKG.SetTitle("")
    roundedBKG.SetTitle("")
    
    # roundedBKG.Scale(1/roundedBKG.Integral())
    # trueBKG.Scale(1/trueBKG.Integral())
    
    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    # c.SetLeftMargin(0.15);
    # c.SetLogy();
    # c.cd();
    
    pad1 = TPad("pad1","pad1",0.02,0.25,0.99,0.99);
    pad1.Draw(); pad1.cd();
    # pad1.SetLogy();
    pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);
    pad1.SetBottomMargin(0.);

    DataStyle(trueBKG)
    roundedBKG.SetLineWidth(2)
    roundedBKG.SetLineColor(kRed)
    
    roundedBKG.Draw("HIST")
    trueBKG.Draw("pex0SAME")
    
    roundedBKG.GetYaxis().SetRangeUser(0,roundedBKG.GetMaximum()*1.2)
    roundedBKG.GetYaxis().SetTitle("Events")
    roundedBKG.GetXaxis().SetTitle(sample.name)
    
    leg = getLegend(xmin=0.65,xmax=0.85,ymin=0.7,ymax=0.9,textsize=0.04)
    leg.AddEntry(trueBKG,"True %s"%variable,"l");
    leg.AddEntry(roundedBKG,"Rounded %s"%variable,"l");
    leg.SetFillColor(kWhite);
    leg.SetFillStyle(0);
    leg.SetTextSize(0.025);
    leg.Draw();
    
    lumi_label = '%s' % float('%.3g' % (sample.lumi/1000.))
    texS,texS1 = getCMSText(lumi_label,sample.year)
    texS.Draw();
    texS1.Draw();
    
    c.cd();
    pad2 = TPad("pad2","pad2",0.02,0.01,0.99,0.25);
    pad2.Draw(); pad2.cd();
    pad2.SetFillColor(0); pad2.SetFrameBorderMode(0); pad2.SetBorderMode(0);
    pad2.SetTopMargin(0);
    pad2.SetBottomMargin(0.35);
    
    ######################################
    rmin = 0.3; rmax = 1.7;
    
    Ratio = GetRatio(roundedBKG,trueBKG)
    RatioStyle(Ratio,yname="Rounded/True")
    
    xmin,xmax = trueBKG.GetXaxis().GetXmin(),trueBKG.GetXaxis().GetXmax()
    
    line = TLine(xmin, 1.,xmax, 1.);
    line.SetLineStyle(8);
    
    
    Ratio.Draw("pex0");
    line.Draw("same");
    
    c.Update();
    SaveAs(c,"compare_%s_%s"%(proc,variable),year=sample.year,region=sample.region,sub="Rounded",exts=(".png",".pdf"))

bkgmap = {
    "Single":"WJets",
    "Double":"DYJets",
    "Gamma":"GJets"
}
bkg = next( bkgmap[region] for region in bkgmap if region in sample.region )
for variable in ("j1CHF","j1NHF"):
    compare(variable,bkg)
    compare(variable,'Data')
