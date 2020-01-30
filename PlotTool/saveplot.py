#!/usr/bin/env python

from ROOT import *
from sys import argv,path
from PlotTool import *
from os import system,getcwd,path,mkdir
import config

gROOT.SetBatch(1)

def printVars(obj):
    for var,val in vars(obj).iteritems():
        print var,':',val
    exit()

outdir = "Systematics"
if not path.isdir(outdir): mkdir(outdir)
dirlist = ["SignalRegion","SingleEleCR","SingleMuCR","DoubleEleCR","DoubleMuCR"]
dirmap = {"SignalRegion":"sr","DoubleEleCR":"ze","DoubleMuCR":"zm","SingleEleCR":"we","SingleMuCR":"wm"}

def GetZWLinking(rfile,centmap):
    print '-Fetching Z/W Linking'
    if type(rfile) == str: rfile = TFile.Open(rfile)
    rfile.cd(); rfile.mkdir('sr/zwlink'); rfile.cd('sr/zwlink')
    sr = centmap["SignalRegion"]
    tf = Transfer('zwlink',sr['ZJets'],sr['WJets'],namelist=['Znn','Wln'])
    rfile.cd('sr/zwlink')
    tf.histo.SetName('ZWlink'); tf.histo.Write()
    for nuisance in tf.nuisances.values():
        uncname = 'ZWlink_%s' % nuisance.name
        up,dn = nuisance.GetHistos()
        rfile.cd('sr/zwlink')
        up.SetName('%sUp' % uncname); up.Write()
        dn.SetName('%sDown' % uncname); dn.Write()
##################################################################

def GetTransferFactors(rfile,centmap):
    if type(rfile) == str: rfile = TFile.Open(rfile)
    namemap = {''}
    for region,dirname in dirmap.iteritems():
        if 'CR' not in region: continue
        print '-Fetching %s Transfer Factors' % region
        rfile.cd(); rfile.mkdir('%s/transfer' % dirname); rfile.cd('%s/transfer' % dirname)
        if 'Double' in region:
            den = centmap[region]['DYJets']; num = centmap['SignalRegion']['ZJets']
            if 'Ele' in region: namelist = ['Znn','Zee']
            else:               namelist = ['Znn','Zmm']
        else:
            den = centmap[region]['WJets']; num = centmap['SignalRegion']['WJets']
            if 'Ele' in region: namelist = ['Wln','Wen']
            else:               namelist = ['Wln','Wmn']
        tf = Transfer('CSlink',num,den,namelist=namelist)
        rfile.cd('%s/transfer' % dirname)
        tf.histo.SetName(den.process); tf.histo.Write()
        for nuisance in tf.nuisances.values():
            uncname = '%s_%s' % (den.process,nuisance.name)
            up,dn = nuisance.GetHistos()
            rfile.cd('%s/transfer' % dirname)
            up.SetName('%sUp' % uncname); up.Write()
            dn.SetName('%sDown' % uncname); dn.Write()
###################################################################

def saveplot(variable):
    print variable
    lumi = max( config.lumi.values() )
    centmap = {}
    for region in dirlist:
        print 'Fetching Variable in %s' % region
        centmap[region] = Region(path=region,lumi=lumi,show=False,autovar=True)
        centmap[region].initiate(variable)
        centmap[region].setSumOfBkg()
        varname = centmap[region].varname.split('_')[0]
    rfile = TFile( "%s/%s_%s.sys.root" % (outdir,varname,config.version) ,'recreate')
    for region,norm in centmap.iteritems():
        print "Writing Histograms from %s" % region
        rfile.cd(); cwd = rfile.mkdir(dirmap[region]); cwd.cd()
        norm["SumOfBkg"].histo.Write("SumOfBkg")
        for process in norm:
            print '--Writing %s Histogram' % process.name
            if process.proctype == 'data':
                if region == 'SignalRegion':
                    data_obs = norm["SumOfBkg"].histo.Clone("data_obs")
                else:
                    data_obs = process.histo.Clone("data_obs")
                data_obs.Write()
            elif process.proctype == 'signal':
                signal = process.histo.Clone(process.process)
                signal.Write()
            else:
                bkg = process.histo.Clone(process.process)
                bkg.Write()
        for unclist in config.Uncertainty.values():
            for unc in unclist:
                print '-Fetching %s Uncertainty' % unc
                for process in norm:
                    print '--Writing %s Histogram' % process.name
                    process.addUnc(unc,show=False)
                    cwd.cd()
                    if process.proctype == 'data': continue
                    elif process.proctype == 'signal':
                        for subprocess in process:
                            up,dn = subprocess.nuisances[unc].GetHistos()
                            up = up.Clone('%s_%sUp' % (subprocess.process,unc))
                            dn = dn.Clone('%s_%sDown' % (subprocess.process,unc))
                            up.Write();dn.Write()
                    else:
                        up,dn = process.nuisances[unc].GetHistos()
                        up = up.Clone('%s_%sUp' % (process.process,unc))
                        dn = dn.Clone('%s_%sDown' % (process.process,unc))
                        up.Write(); dn.Write()
    ###########################################################################
    GetZWLinking(rfile,centmap)
    GetTransferFactors(rfile,centmap)
    rfile.cd()
    lumi_hs = TH1F("lumi","lumi",1,0,1)
    lumi_hs.SetBinContent(1,norm.lumi)
    lumi_hs.Write()
    year_hs = TH1F("year","year",1,0,1)
    year_hs.SetBinContent(1,config.year)
    year_hs.Write()
    var_hs = TH1F("variable",variable+';'+norm.name,1,0,1)
    var_hs = b_info.template.Clone('variable')
    var_hs.Reset(); var_hs.SetTitle(variable); var_hs.GetXaxis().SetTitle(norm.name);
    var_hs.Write()
    rfile.Close()
################################################################################
if __name__ == "__main__":

    args = parser.parse_args()
    for variable in args.argv: saveplot(variable)
