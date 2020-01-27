from ROOT import *
from mcinfo import xsec as mcxsecs
from array import array
import sys
import json
from os.path import isfile

def writeBkgInfo(bkg_info):
    fname = '.bkginfo.root'
    prompt = 'Writing %s' % "{0:<20}".format(fname)
    rfile = TFile(fname,'recreate')
    tree = TTree('tree','tree')

    nweight = array('f',[0])
    nvar = array('f',[0])
    
    tree.Branch('weight',nweight,'weight')
    tree.Branch('var',nvar,'var')
    tree_total = len(bkg_info)
    nevents = int(0.1*tree_total)
    if nevents == 0: nevents = 1
    for i,event in enumerate(bkg_info):
        if i%nevents == 0:
            complete = 100 * float(i+1)/tree_total
            sys.stdout.write( '\r%s: %i%%' % (prompt,complete) )
            sys.stdout.flush()
        nweight[0] = event['weight']
        nvar[0]= event['var']
        tree.Fill()
    sys.stdout.write( '\r%s: 100%%\n' % (prompt) )
    sys.stdout.flush()
    tree.Write()
    rfile.Write()
    rfile.Close()
#################################################################

def genBkgInfo():
    total_bkg = 0
    bkg_info = []
    keylist = mcxsecs.keys()
    for mcfile in sorted(keylist):
        xsec = mcxsecs[mcfile]
        prompt = 'Processing %s' % "{0:<20}".format(mcfile)
        mcfile += '.root'
        rfile = TFile.Open(mcfile)
        tree = rfile.Get('ZprimeJet_8/tree')
        cutflow = rfile.Get('h_cutflow')
        total = cutflow[1]
        tree_total = tree.GetEntriesFast()
        nevents = int(0.1*tree_total)
        if nevents == 0: nevents = 1
        for i,event in enumerate(tree):
            if i%nevents == 0:
                complete = 100 * float(i+1)/tree_total
                sys.stdout.write( '\r%s: %i%%' % (prompt,complete) )
                sys.stdout.flush()
            weight = xsec*event.weight/total
            var = event.ChNemPtFrac
            total_bkg += weight
            bkg_info.append( {'var':var,'weight':weight} )
        # bkg_info.sort(key=lambda x:x['var'])
        sys.stdout.write( '\r%s: 100%%\n' % (prompt) )
        sys.stdout.flush()
        bkg_info.sort(key=lambda x:x['var'])
    writeBkgInfo(bkg_info)
    return bkg_info,total_bkg
#####################################################################

def readBkgInfo():
    total_bkg = 0
    bkg_info = []
    
    fname = '.bkginfo.root'
    prompt = 'Reading %s' % "{0:<20}".format(fname)
    rfile = TFile.Open(fname)
    tree = rfile.Get('tree')
    tree_total = tree.GetEntriesFast()
    nevents = int(0.1*tree_total)
    if nevents == 0: nevents = 1
    for i,event in enumerate(tree):
        if i%nevents == 0:
            complete = 100 * float(i+1)/tree_total
            sys.stdout.write( '\r%s: %i%%' % (prompt,complete) )
            sys.stdout.flush()
        total_bkg += event.weight
        bkg_info.append( {'var':event.var,'weight':event.weight} )
    sys.stdout.write( '\r%s: 100%%\n' % (prompt) )
    sys.stdout.flush()
    return bkg_info,total_bkg
#####################################################################

nbins = 40
perc = 1./nbins

if isfile('.bkginfo.root'): bkg_info,total_bkg = readBkgInfo()
else:                      bkg_info,total_bkg = genBkgInfo()

binlist = [0]
bkgiter = iter(bkg_info)

for i in range(nbins):

    bin_total = 0
    while float(bin_total)/total_bkg < perc:
        try: tmp = next(bkgiter)
        except StopIteration: break
        bkg_event = tmp
        bin_total += bkg_event['weight']
    binlist.append(bkg_event['var'])
print '----%i Bin List----' % nbins
print
print binlist
