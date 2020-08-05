#!/usr/bin/env python

"""
Generates NLO_QCD functions comparison plots 
Usage: python PlotTool/compareNLOQCD.py
"""

from sys import argv,path
from PlotTool import *
from ROOT import *
import config
import os

group = parser.add_group(__file__,__doc__,"Script")

gROOT.SetBatch(1)

bkgmap = {
    "Single":"WJets",
    "Double":"DYJets",
    "Gamma":"GJets"
}

fitmap = {
    "WJets":(1.053,3.163e-3,0.746),
    "DYJets":(1.434, 2.210e-3, 0.443),
    "GJets":(1.159, 1.944e-3, 1.0)
}

sample = Region(show=0,autovar=True)

bkg = next( bkgmap[cr] for cr in bkgmap if cr in sample.region )

sample.initiate('bosonPt','genWeight')
unweightedBKG = sample[bkg].histo.Clone()
sample.initiate('bosonPt','genWeight*nlo_qcd')
fitWeightedBKG = sample[bkg].histo.Clone()
sample.initiate('bosonPt','genWeight*nlo_qcd_binned')
binWeightedBKG = sample[bkg].histo.Clone()

fitfun = TF1("NLO QCD Fit","%f * TMath::Exp(-%f * x) + %f" % fitmap[bkg],unweightedBKG.GetXaxis().GetXmin(),unweightedBKG.GetXaxis().GetXmax())
fitWeightedBKG.Divide(unweightedBKG)
binWeightedBKG.Divide(unweightedBKG)

print "Binned/Gen: %f" % binWeightedBKG.Integral()
print "Fitted/Gen: %f" % fitWeightedBKG.Integral()

c = TCanvas("c","c",800,800)
c.SetGrid()
gStyle.SetOptStat(0)
gStyle.SetLegendBorderSize(0)

DataStyle(fitWeightedBKG)

binWeightedBKG.SetTitle("")
binWeightedBKG.GetXaxis().SetTitle("Gen Boson P_{T}")
binWeightedBKG.GetYaxis().SetTitle("NLO QCD")
binWeightedBKG.SetLineColor(kBlue)
binWeightedBKG.SetLineWidth(2)

ymax = max( hs.GetMaximum() for hs in (binWeightedBKG,fitWeightedBKG) )

binWeightedBKG.SetMaximum(ymax)

binWeightedBKG.Draw("hist")
fitfun.Draw("same")
fitWeightedBKG.Draw("pex0 same")

leg = getLegend(xmin=0.5,xmax=0.7)
leg.AddEntry(binWeightedBKG,"%s Binned NLO QCD" % bkg,"l")
leg.AddEntry(fitWeightedBKG,"%s Fitted NLO QCD" % bkg,"lp")
leg.AddEntry(fitfun,"%s NLO QCD Fit Function" % bkg,"l")
leg.Draw()

lumi_label = '%s' % float('%.3g' % (sample.lumi/1000.)) + " fb^{-1}"
t1,t2 = getCMSText(lumi_label,sample.year,0.8)
t1.Draw(); t2.Draw()

SaveAs(c,"%s_NLO_QCD"%bkg,year=sample.year,region=sample.region)

