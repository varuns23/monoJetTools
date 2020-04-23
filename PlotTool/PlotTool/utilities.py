from ROOT import *
import os
import sys
from array import array
         
debug_store=[]
def debug_hslist(hslist):
    binlist=[]
    for hs in hslist: binlist += list(hs)[1:-1]
    ymin=min(binlist)
    ymax=max(binlist)
    c=TCanvas("c%i"%len(debug_store),"c%i"%len(debug_store))
    for hs in hslist:
        hs.SetLineWidth(2)
        hs.Draw("hist plc same")
        hs.GetYaxis().SetRangeUser(ymin,ymax)
    c.BuildLegend()
    c.SaveAs("~/public_html/test/"+c.GetName()+".png")
    debug_store.append(c)   
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

    def checkdir(dirname):
        for region,pattern in zip(RegionName,region_pattern):
            if region in dirname: return region
            if any( pattern in fname for fname in os.listdir('.') ): return region
            if os.path.isdir('.output/') and any( pattern in fname for fname in os.listdir('.output/') ): return region

    dirname = os.getcwd()
    region = checkdir(dirname)
    if region != None: return region
    
    if not os.path.isfile('postpath.txt'): return "SignalRegion"
    
    with open('postpath.txt') as f: postpath = f.read().strip()
    cwd = os.getcwd(); os.chdir(postpath)
    dirname = os.path.realpath( os.getcwd() + '/../' )
    region = checkdir(dirname)
    if region != None: return region
    return "SignalRegion"
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

def GetRootFiles():
    wd,wf = os.path.split(__file__)
    def updirectory(path,ntimes=1):
        for _ in range(ntimes): path += '/../';
        return path
    repo_path = os.path.realpath( updirectory(wd,2) )
    def helper(path):
        if any( directory == 'RootFiles' for directory in os.listdir(path) ): return os.path.realpath(path+"/RootFiles/")
        elif os.path.realpath(path) != repo_path: return helper( updirectory(path) )
    return helper('.')

def GetDirname(variable,sub=None):
    ndir = variable.split('_')[-1]
    if not ndir.isdigit(): return None,None
    dirname = 'monoJet_%s' % ndir
    if sub != None: dirname += '/%s' % sub
    return dirname,ndir
def CheckHisto(histo):
    pass
def GetTObject(name,tfile):
    tobject = tfile.Get(name)
    if tobject == None: raise ValueError("Unable to find %s in %s" % (name,tfile.GetName()))
    return tobject
def GetBranch(name,variable,tree,weight=None,cut=None):
    if weight is None: weight = variable.weight
    if cut is None:    cut = variable.cut
    histo = variable.template.Clone(name); histo.Reset()
    if cut == None: tree.Draw("%s>>%s" % (variable.base,name),weight,'goff')
    else:           tree.Draw("%s>>%s" % (variable.base,name),'%s*(%s)' % (weight,cut),'goff')
    if hasattr(variable.template,'post'): variable.template.post(histo)
    return histo
def HistoEqual(hs1,hs2):
    if hs1 == hs2: return True
    if hs1 is None: return True
    if hs2 is None: return False
    xarray1 = list(hs1.GetXaxis().GetXbins())
    xmin1,xmax1 = hs1.GetXaxis().GetXmin(),hs1.GetXaxis().GetXmax()
    xarray2 = list(hs2.GetXaxis().GetXbins())
    xmin2,xmax2 = hs2.GetXaxis().GetXmin(),hs2.GetXaxis().GetXmax()
    if len(xarray1) != len(xarray2): return False
    if xmin1 != xmin2 or xmax1 != xmax2: return False
    return not any( width1 != width2 for width1,width2 in zip(xarray1,xarray2) )
def GetProcessName(name,year,region):
    if region is not None: name = "%s_%s" % (region,name)
    if year is not None:   name = "%s_%s" % (year,name)
    return name
