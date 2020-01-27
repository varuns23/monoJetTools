#!/usr/bin/env python

from ROOT import *
from sys import argv,path
import os
import PlotTool as pt
from config import config,lumi as mc,version

gROOT.SetBatch(1)

def SaveRegion(region,nhist):
    cwd = os.getcwd()
    if not os.path.isdir('Output'): os.mkdir('Output')
    os.chdir(region)
    fname = '../Output/'+region.replace('/','.root')
    samples = pt.datamc()
    samples.initiate('recoil_%s' % nhist)
    output = TFile(fname,'RECREATE')
    for name,process in samples.processes.iteritems():
        process.histo.SetName('h_recoil_%s' % name)
        process.histo.Write()
    output.Close()
    os.chdir(cwd)

print version
for region,nhist in config['regions'].items(): SaveRegion(region,nhist)

