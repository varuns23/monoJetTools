from ROOT import *
import os
from sys import argv
from PlotTool import *
import config
from PlotTool import parser

gROOT.SetBatch(1)
gStyle.SetOptStat(0)
gStyle.SetLegendBorderSize(0)

regionmap = {
    "SignalRegion":"sr",
    "SingleEleCR":"we",
    "SingleMuCR":"wm",
    "DoubleEleCR":"ze",
    "DoubleMuCR":"zm",
    "GammaCR":"ga"
}

procmap = {
    "SignalRegion":["ZJets","WJets"],
    "SingleEleCR":["WJets"],
    "SingleMuCR":["WJets"],
    "DoubleEleCR":["DYJets"],
    "DoubleMuCR":["DYJets"],
    "GammaCR":["GJets"]
}

def unc_style(hs,color=kBlack):
    hs.SetLineColor(color)
    hs.SetLineWidth(2)
def PlotUncertainty(process,nuisance):
    process.addUnc(nuisance)

    if nuisance not in process.nuisances: return

    nuisance = process.nuisances[nuisance]
    print nuisance

    up,dn = nuisance.GetScaleDiff()

    unc_style(up,kRed)
    unc_style(dn,kBlue)

    c = TCanvas()

    SetBounds([up,dn])
    
    up.GetYaxis().SetTitle(nuisance.name)
    up.GetXaxis().SetTitle(process.variable.xaxis_title)
    up.Draw("hist")
    dn.Draw("hist same")

    SaveAs(c,nuisance.name,year=process.year,sub="Uncertainty/%s_%s"%(regionmap[process.region],process.process))
    return

def PlotRegion():
    sample = Region(autovar=True)
    proclist = procmap[sample.region]
    sample.SampleList = ["Data"] + proclist

    nuisances = {"PSW_isrCon":True,"PSW_fsrCon":True}
    sample.initiate(parser.args.argv[0])
    nuisances.update(sample.variable.nuisances)
    for proc in proclist:
        for nuisance in nuisances:
            PlotUncertainty(sample[proc],nuisance)

if __name__ == "__main__": PlotRegion()
