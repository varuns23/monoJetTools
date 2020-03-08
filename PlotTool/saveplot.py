#!/usr/bin/env python

from ROOT import *
from sys import argv,path
from PlotTool import *
from os import system,getcwd,path,mkdir
import config

gROOT.SetBatch(1)

dirlist = ["SignalRegion","SingleEleCR","SingleMuCR","DoubleEleCR","DoubleMuCR","GammaCR"]
dirmap = {"SignalRegion":"sr","DoubleEleCR":"ze","DoubleMuCR":"zm","SingleEleCR":"we","SingleMuCR":"wm","GammaCR":"ga"}
if not path.isdir("Systematics"): mkdir("Systematics")

def SavePlot(variable):
    print variable

    class save: pass
    save.tfile = None
    def SaveRegion(region,save=save):
        region = Region(path=region,show=False,autovar=True)
        region.initiate(variable)
        region.setSumOfBkg()
        
        if save.tfile is None: save.tfile = TFile("Systematics/%s_%s.sys.root" % (region.varname,region.year),'recreate')
        output = save.tfile

        print "Writing Histograms from %s" % region.region
        output.cd(); cwd = output.mkdir(dirmap[region.region]); cwd.cd()
        region['SumOfBkg'].histo.Write("SumOfBkg")
        if region.isBlinded:
            region['Data'].histo.Write('data_obs')
        for process in region:
            print "--Writing %s Histogram" % process.name
            if process.proctype == "data": process.histo.Write("data_obs")
            else: process.histo.Write(process.process)
        return region
    regionmap = { dirmap[region]:SaveRegion(region) for region in dirlist }
    def WCR_TF(wln,sr,output):
        print "-%s to %s Transfer" % (dirmap[wln.region],dirmap[sr.region])
        tf = Transfer('%s_to_sr' % dirmap[wln.region],sr['WJets'],wln['WJets'],['sr',dirmap[wln.region]])
        output.cd(); output.cd(dirmap[wln.region]);
        cwd = gDirectory.mkdir("transfer"); cwd.cd()
        tf.histo.Write('%s_to_sr' % dirmap[wln.region])
    def ZCR_TF(zll,sr,output):
        print "-%s to %s Transfer" % (dirmap[zll.region],dirmap[sr.region])
        tf = Transfer("%s_to_sr" % dirmap[zll.region],sr["ZJets"],zll["DYJets"],['sr',dirmap[zll.region]])
        output.cd(); output.cd(dirmap[zll.region])
        cwd = gDirectory.mkdir("transfer"); cwd.cd()
        tf.histo.Write('%s_to_sr' % dirmap[zll.region])
    def GCR_TF(ga,sr,output):
        print "-%s to %s Transfer" % (dirmap[ga.region],dirmap[sr.region])
        tf = Transfer("%s_to_sr" % dirmap[ga.region],sr["ZJets"],ga["GJets"],['sr',dirmap[ga.region]])
        output.cd(); output.cd(dirmap[ga.region])
        cwd = gDirectory.mkdir("transfer"); cwd.cd()
        tf.histo.Write('%s_to_sr' % dirmap[ga.region])
        for nuisance in tf.nuisances.values():
            uncname = "ga_to_sr_%s" % nuisance.name
            up,dn = nuisance.GetHistos()
            up.Write(uncname+"Up")
            dn.Write(uncname+"Down")
        return
    def SR_TF(sr,output):
        print "-Wsr to Zsr Transfer"
        tf = Transfer("wsr_to_zsr",sr["ZJets"],sr["WJets"],['zsr','wsr'])
        output.cd(); output.cd(dirmap[sr.region])
        cwd = gDirectory.mkdir("transfer"); cwd.cd()
        tf.histo.Write('wsr_to_zsr')
        for nuisance in tf.nuisances.values():
            uncname = "wsr_to_zsr_%s" % nuisance.name
            up,dn = nuisance.GetHistos()
            up.Write(uncname+"Up")
            dn.Write(uncname+"Down")
        return
    output = save.tfile
    SR_TF(regionmap["sr"],output)
    for wln in ("we","wm"): WCR_TF(regionmap[wln],regionmap["sr"],output)
    for zll in ("ze","zm"): ZCR_TF(regionmap[zll],regionmap["sr"],output)
    GCR_TF(regionmap["ga"],regionmap["sr"],output)
    output.cd()
    norm = regionmap["sr"]
    lumi_hs = TH1F("lumi","lumi",1,0,1)
    lumi_hs.SetBinContent(1,norm.lumi)
    lumi_hs.Write()
    year_hs = TH1F("year","year",1,0,1)
    year_hs.SetBinContent(1,int(norm.year))
    year_hs.Write()
    var_hs = TH1F("variable",variable+';'+norm.name,1,0,1)
    var_hs = norm.variable.template.Clone('variable')
    var_hs.Reset(); var_hs.SetTitle(variable); var_hs.GetXaxis().SetTitle(norm.name);
    var_hs.Write()
    output.Close()
################################################################################
if __name__ == "__main__":
    from PlotTool import parser
    parser.parse_args()
    for variable in parser.args.argv: SavePlot(variable)
