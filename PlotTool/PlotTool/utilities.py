from ROOT import *
import os
import sys
from array import array
            
def GetRatio(hs_num,hs_den):
    nbins = hs_num.GetNbinsX();
    ratio = hs_num.Clone()
    ratio.Divide(hs_den)
    return ratio
#######################################
def Get2DRatio(hs_num,hs_den):
    xbins = hs_num.GetNbinsX()
    ybins = hs_num.GetNbinsY()
    Ratio = hs_num.Clone("Ratio")
    last = hs_den.Clone("last")
    for xbin in range(1,xbins+1):
        for ybin in range(1,ybins+1):
            stackcontent = last.GetBinContent(xbin,ybin)
            stackerror = last.GetBinError(xbin,ybin)
            datacontent = hs_num.GetBinContent(xbin,ybin)
            dataerror = hs_num.GetBinError(xbin,ybin)
            ratiocontent = 0
            if(datacontent!=0 and stackcontent != 0):ratiocontent = ( datacontent) / stackcontent
            error=0;
            if(datacontent!=0 and stackcontent != 0): error = ratiocontent*((dataerror/datacontent)**2 + (stackerror/stackcontent)**2)**(0.5)
            else: error = 2.07
            Ratio.SetBinContent(xbin,ybin,ratiocontent)
            Ratio.SetBinError(xbin,ybin,error)
    return Ratio
########################################
def GetUncBand(up,dn,norm=None):
    xbins = up.GetNbinsX()
    x = []; y = []; ex = []; ey = []
    nbins = 0
    for ibin in range(1,xbins+1):
        if up.GetBinContent(ibin) == 0 and dn.GetBinContent(ibin) == 0: continue
        x.append(up.GetBinCenter(ibin))
        ex.append(up.GetBinWidth(ibin)/2)
        if norm is None: y.append((up.GetBinContent(ibin)+dn.GetBinContent(ibin))/2)
        else:            y.append(norm)
        ey.append(abs(up.GetBinContent(ibin)-dn.GetBinContent(ibin))/2)
        nbins += 1
    return TGraphErrors(nbins,array('d',x),array('d',y),array('d',ex),array('d',ey))
def valid_directory(directory):
    if not os.path.isdir(directory): raise ValueError("%s is not a valid directory" % directory)
    return directory

def GetRegion():
    region_pattern = ["postMETdata","postSingleEle","postSingleMu","postDoubleEle","postDoubleMu","postGamma"]
    RegionName = ["SignalRegion","SingleEleCR","SingleMuCR","DoubleEleCR","DoubleMuCR","GammaCR"]

    found = False
    for region,pattern in zip(RegionName,region_pattern):
        if os.path.isdir('.output/'):
            if any( pattern in fname for fname in os.listdir('.output/') ): found = True; break
        if any( pattern in fname for fname in os.listdir('.') ): found = True; break
    if not found and os.path.isfile('postpath.txt'):
        with open('postpath.txt') as f: postpath = f.read().strip()
        cwd = os.getcwd(); os.chdir(postpath)
        for region,pattern in zip(RegionName,region_pattern):
            if os.path.isdir('.output/'):
                if any( pattern in fname for fname in os.listdir('.output/') ): found = True; break
            if any( pattern in fname for fname in os.listdir('.') ): found = True; break
        os.chdir(cwd)
    if not found: return None
    return region
def GetMCxsec(filenames,xsecMap):
    return { fname:xsecMap[fname] for fname in filenames }

def IsSignal(signal):
    return 'Mx' in signal and '_Mv' in signal

def FindConfig():
    wd,wf = os.path.split(__file__)
    def updirectory(path,ntimes=1):
        for _ in range(ntimes): path += '/../';
        return path
    repo_path = os.path.realpath( updirectory(wd,2) )
    def helper(path):
        if any( directory == 'config' for directory in os.listdir(path) ): return os.path.realpath(path)
        elif os.path.realpath(path) != repo_path: return helper( updirectory(path) )
    config_path = helper('.')
    if config_path is not None: sys.path.append(config_path)

def GetDirname(variable,sub=None):
    ndir = variable.split('_')[-1]
    if not ndir.isdigit(): return None,None
    dirname = 'ZprimeJet_%s' % ndir
    if sub != None: dirname += '/%s' % sub
    return dirname,ndir
