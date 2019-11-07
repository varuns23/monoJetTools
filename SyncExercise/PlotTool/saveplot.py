#!/usr/bin/env python

from ROOT import *
from sys import argv
from sys import path
from Plot import datamc,getargs,GetRatio
from os import system,getcwd,path,mkdir
from cfg_saveplot import config
from mcinfo import lumi as mc,version
from optparse import OptionParser

gROOT.SetBatch(1)

outdir = "Systematics"
if not path.isdir(outdir): mkdir(outdir)
dir = {"SignalRegion/":"sr","DoubleEleCR/":"ee","DoubleMuCR/":"mm","SingleEleCR/":"e","SingleMuCR/":"m"}

def GetZWLinking(rfile):
    if type(rfile) == str: rfile = TFile.Open(rfile)
    rfile.cd(); rfile.cd('sr')
    lhistos = {}
    keylist = [ key.GetName().replace('WJets','ZWlink') for key in gDirectory.GetListOfKeys() if 'WJets' in key.GetName() and 'WJets' != key.GetName() ]
    wjet_norm = gDirectory.Get('WJets')
    zjet_norm = gDirectory.Get('ZJets')
    lhistos['ZWlink'] = GetRatio(wjet_norm,zjet_norm).Clone('ZWlink')
    for key in keylist:
        wkey = '%s_WJets' % key; zkey = '%s_ZJets' % key
        wjet_unc = gDirectory.Get(key.replace('ZWlink','WJets'))
        zjet_unc = gDirectory.Get(key.replace('ZWlink','ZJets'))
        lhistos[wkey] = GetRatio(zjet_norm,wjet_unc).Clone(wkey)
        lhistos[zkey] = GetRatio(zjet_unc,wjet_norm).Clone(zkey)
    rfile.cd(); rfile.mkdir('sr/zwlink'); rfile.cd('sr/zwlink')
    for key,hs in lhistos.iteritems():
        hs.Write()
##################################################################

def GetTransferFactors(rfile):
    if type(rfile) == str: rfile = TFile.Open(rfile)
    tfactors = {'ZJets':{},'WJets':{}}
    varmap = {'ZJets':{'sample':'DYJets','regions':['ee','mm']},
              'WJets':{'sample':'WJets' ,'regions':['e','m']}}
    for sample,transfer in tfactors.items():
        rfile.cd(); rfile.cd('sr')
        sr = { key.GetName():rfile.Get('sr/%s' % key.GetName()).Clone() for key in gDirectory.GetListOfKeys()
               if sample in key.GetName() }
        infomap = varmap[sample]
        for region in infomap['regions']:
            thistos = {}
            rfile.cd(); rfile.cd(region)
            cr = { key.GetName().replace(infomap['sample'],sample):rfile.Get( '%s/%s' % (region,key.GetName()) ).Clone() for key in gDirectory.GetListOfKeys()
                   if infomap['sample'] in key.GetName() }
            for key in cr:
                cr_hs = cr[key]
                sr_hs = sr[key]
                thistos[key] = GetRatio(cr_hs,sr_hs).Clone(key)
            transfer[region] = thistos
    # Write transfer factor histograms to systematic file
    for sample,transfer in tfactors.iteritems():
        for region,histos in transfer.iteritems():
            rfile.cd(); rfile.cd(region);
            rfile.mkdir('%s/transfer' % region); rfile.cd('%s/transfer' % region)
            for key,hs in histos.items():
                hs.Write()
###################################################################

def saveplot(variable):
    print variable
    cut = ''
    if '>' in variable: cut = '>'+variable.split('>')[-1]
    if '<' in variable: cut = '<'+variable.split('<')[-1]
    varname = variable.replace('>','+').replace('<','-')
    variable = variable.replace(cut,'')
    rfile = TFile( "%s/%s_%s.sys.root" % (outdir,varname,version) ,'recreate')
    Uncertainty = config['Uncertainty']
    lumi = max( lumi for region,lumi in mc.items() )
    for region,nhisto in config['regions'].items():
        rfile.cd()
        if variable == 'h_recoil' and region == 'SignalRegion/': variable = 'pfMET'
        elif variable == 'pfMET' and region != 'SignalRegion/' : variable = 'h_recoil'
        var = variable+'_'+nhisto+cut
        print region,var
        directory = rfile.mkdir(dir[region])
        norm = datamc(fileDir=region,lumi=lumi,show=False)
        norm.initiate(var)
        directory.cd()
        sumOfBkg = norm.getSumOfBkg()
        sumOfBkg.SetName('sumOfBkg')
        sumOfBkg.Write()
        data_obs = norm.getSumOfBkg()
        data_obs.SetName('data_obs')
        data_obs.Write()
        for sample in norm.SampleList:
            if sample == "Signal":
                signals = norm.processes['Signal']
                for signal in signals:
                    signal.histo.SetName(signal.name)
                    signal.histo.Write()
            else:
                process = norm.processes[sample]
                process.histo.SetName(sample)
                process.histo.Write()
        ##############################################
        for name,nuisances in Uncertainty.iteritems():
            for nuisance in nuisances:
                print '\t',nuisance,
                norm.addUnc(nuisance)
                directory.cd()
                for sample in norm.SampleList:
                    if sample == 'Data': continue
                    for variation in ('Up','Down'):
                        if sample == "Signal":
                            signals = norm.processes['Signal']
                            for signal in signals:
                                signal.nuisances[nuisance][variation].SetName(signal.name+'_'+nuisance+variation)
                                signal.nuisances[nuisance][variation].Write()
                        else:
                            process = norm.processes[sample]
                            process.nuisances[nuisance][variation].SetName(process.name+'_'+nuisance+variation)
                            process.nuisances[nuisance][variation].Write()
                        
            ###############################################################
            print
    ###########################################################################
    GetZWLinking(rfile)
    GetTransferFactors(rfile)
    rfile.cd()
    lumi_hs = TH1F("lumi","lumi",1,0,1)
    lumi_hs.SetBinContent(1,norm.lumi)
    lumi_hs.Write()
    year_hs = TH1F("year","year",1,0,1)
    year_hs.SetBinContent(1,int(norm.version))
    year_hs.Write()
    var_hs = TH1F("variable",variable+';'+norm.name,1,0,1)
    var_hs.Write()
    rfile.Close()
################################################################################
if __name__ == "__main__":

    options,args = getargs()
    for variable in args: saveplot(variable)
