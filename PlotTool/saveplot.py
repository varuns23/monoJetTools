#!/usr/bin/env python

from ROOT import *
from sys import argv,path
from PlotTool import *
from os import system,getcwd,path,mkdir
import config

gROOT.SetBatch(1)

dirlist = ["SignalRegion","SingleEleCR","SingleMuCR","DoubleEleCR","DoubleMuCR","GammaCR"]
dirmap = {"SignalRegion":"signal","DoubleEleCR":"Zee","DoubleMuCR":"Zmm","SingleEleCR":"Wen","SingleMuCR":"Wmn","GammaCR":"gjets"}
procmap = {"Data":"data","ZJets":"zjets","WJets":"wjets","DYJets":"zll","GJets":"gjets","TTJets":"top","DiBoson":"diboson","QCD":"qcd"}
signalmap = {"Axial":"axial","Zprime":"zprime"}
if not path.isdir("Systematics"): mkdir("Systematics")

def validHisto(hs,total=0,threshold=0.2):return hs.Integral() > threshold*total
def validShape(up,dn):return any( up[ibin] != dn[ibin] for ibin in range(1,up.GetNbinsX()+1) ) and validHisto(up) and validHisto(dn)

def SaveRegion(region,save):
    region = Region(path=region,show=False,autovar=True,blinded=region=="SignalRegion")
    region.initiate(variable)
    
    if save.tfile is None: save.tfile = TFile("Systematics/%s_%s.sys.root" % (region.varname,region.year),'recreate')
    output = save.tfile
    
    print "Writing Histograms from %s" % region.region
    output.cd();
    # region['SumOfBkg'].histo.Write("SumOfBkg")
    if region.isBlinded:
        data = region['Data'].histo
        name = "%s_data" % dirmap[region.region]
        data.SetName(name)
        data.SetTitle(name)
        data.Write()
    for process in region:
        print "--Writing %s Histogram" % process.name
        
        if process.proctype == "signal":
            sigproc = next( signalmap[signal] for signal in signalmap if signal in process.process )
            export = "%s_%s" % (dirmap[region.region],sigproc)
        else: export = "%s_%s" % (dirmap[region.region],procmap[process.process])
        process.histo.SetName(export)
        process.histo.SetTitle(export)
        process.histo.Write()
            # for nuisance in region.variable.nuisances:
            #     process.addUnc(nuisance)
            #     cwd.cd()
            #     if nuisance in process.nuisances:
            #         up,dn = process.nuisances[nuisance].GetHistos()
            #         if not validShape(up,dn): continue
            #         print "----Writing",process.nuisances[nuisance]
            #         up.Write("%s_%sUp"%(process.process,nuisance))
            #         dn.Write("%s_%sDown"%(process.process,nuisance))
    return region
def SavePlot(variable):
    print variable

    class save: pass
    save.tfile = None
    regionmap = { dirmap[region]:SaveRegion(region,save) for region in dirlist }
################################################################################
if __name__ == "__main__":
    from PlotTool import parser
    import sys
    if "--no-width" not in sys.argv: sys.argv.append("--no-width")
    parser.parse_args()
    for variable in parser.args.argv: SavePlot(variable)
