from ROOT import *
from array import array
from sys import argv
import re
import os

def FindTree(tfile):
    if type(tfile) == str: tfile = TFile(tfile)
    finaldir = None
    finalnhs = None
    for key in tfile.GetListOfKeys():
        if tfile.GetDirectory(key.GetName()):
            finaldir = key.GetName()
            finalnhs = finaldir.split('_')[1]
    return tfile.Get("%s/trees/norm" % finaldir)
def GetRootFiles():
    wd,wf = os.path.split(__file__)
    def updirectory(path,ntimes=1):
        for _ in range(ntimes): path += '/../';
        return path
    repo_path = os.path.realpath( updirectory(wd,2) )
    def helper(path):
        if any( directory == 'RootFiles' for directory in os.listdir(path) ): return os.path.realpath(path)
        elif os.path.realpath(path) != repo_path: return helper( updirectory(path) )
    return helper('.')

filemap = {
    "WJets" :("RootFiles/theory/unc/WJets_NLO_EWK.root","evj_pTV"),
    "ZJets" :("RootFiles/theory/unc/ZJets_NLO_EWK.root","vvj_pTV"),
    "DYJets":("RootFiles/theory/unc/DYJets_NLO_EWK.root","eej_pTV"),
    "GJets" :("RootFiles/theory/unc/GJets_NLO_EWK.root","vvj_pTV")
}

uncmap = {
    "QCD_Scale":"d1K_NLO",
    "QCD_Shape":"d2K_NLO",
    "QCD_Proc":"d3K_NLO",
    "NNLO_EWK":"d1kappa_EW",
    "NNLO_Miss":"d2kappa_EW",
    "NNLO_Sud":"d3kappa_EW",
    "QCD_EWK_Mix":"dK_NLO_mix"
}

patternmap = [
    ("WJets",re.compile("postW\d")),
    ("ZJets",re.compile("postZ\d")),
    ("DYJets",re.compile("postDY\d")),
    ("GJets",re.compile("postGJets\d")),
    ("None",re.compile(".*"))
]

def getBin(hs,x): return hs.GetBinContent( hs.GetXaxis().FindBin(x) )
def qcd(nlo_qcd,nlo_ewk,bosonPt,hs=None):
    if hs is None: return
    unc = getBin(hs,bosonPt)
    weightUp = ( (nlo_qcd + unc) * nlo_ewk )
    weightDn = ( (nlo_qcd - unc) * nlo_ewk )
    return weightUp,weightDn
def ewk(nlo_qcd,nlo_ewk,bosonPt,hs=None):
    if hs is None: return
    unc = getBin(hs,bosonPt)
    weightUp = ( nlo_qcd * (nlo_ewk + unc) )
    weightDn = ( nlo_qcd * (nlo_ewk - unc) )
    return weightUp,weightDn
def mix(nlo_qcd,nlo_ewk,bosonPt,hs=None):
    if hs is None: return
    unc = getBin(hs,bosonPt)
    kfactor = nlo_qcd * nlo_ewk
    weightUp = ( kfactor + unc )
    weightDn = ( kfactor - unc )
    return weightUp,weightDn
applymap = {
    "QCD_Scale":qcd,
    "QCD_Shape":qcd,
    "QCD_Proc":qcd,
    "NNLO_EWK":ewk,
    "NNLO_Miss":ewk,
    "NNLO_Sud":ewk,
    "QCD_EWK_Mix":mix
}
def ApplyTheory(tfile):
    tfile = TFile(tfile,"update")
    
    for mc,pattern in patternmap:
        if any(pattern.findall(tfile.GetName())): break

    print tfile.GetName(),mc
    tree = FindTree(tfile)
    
    rootdir = GetRootFiles()

    varmap = {}
    branchmap = {}
    hsmap = {}
    getmap = {}
    def ones(qcd,ewk,bosonPt): return 1,1
    for theory in applymap:
        varmap[theory+'Up'] = array('f',[1])
        varmap[theory+'Down'] = array('f',[1])

        branchmap[theory+'Up'] = tree.Branch(theory+'Up',varmap[theory+'Up'],theory+'Up/F')
        branchmap[theory+'Dn'] = tree.Branch(theory+'Down',varmap[theory+'Down'],theory+'Down/F')
        if mc is not 'None':
            fname,prefix = filemap[mc]
            hsmap[theory] = {}
            hsmap[theory][TFile] = TFile("%s/%s" % (rootdir,fname))
            hsmap[theory][TH1F] = hsmap[theory][TFile].Get("%s_%s" % (prefix,uncmap[theory]))
            getmap[theory] = lambda nlo_qcd,nlo_ewk,bosonPt : applymap[theory](nlo_qcd,nlo_ewk,bosonPt,hs = hsmap[theory][TH1F])
        else:
            getmap[theory] = ones
    for i,event in enumerate(tree):
        if i%10000 == 0: print "Processing %i of %i" % (i,tree.GetEntriesFast())
        nlo_qcd,nlo_ewk,bosonPt = event.nlo_qcd,event.nlo_ewk,event.bosonPt
        for theory,get in getmap.iteritems():
            varmap[theory+'Up'][0] = 1
            varmap[theory+'Down'][0] = 1
            up,dn = get(nlo_qcd,nlo_ewk,bosonPt)
            varmap[theory+'Up'][0] = up
            varmap[theory+'Down'][0] = dn
            branchmap[theory+'Up'].Fill()
            branchmap[theory+'Dn'].Fill()
    dir = tree.GetDirectory()
    dir.cd()
    tree.Write("",TObject.kOverwrite)
ApplyTheory(argv[1])
