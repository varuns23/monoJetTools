from ROOT import TH1F
from array import array
import numpy as np

class b_info:
    template = None
    cut = None
    weight = None
    @staticmethod
    def initVariable():
        b_info.template = None
        b_info.cut = None

def linspace(xmin,xmax,nx): return list(np.linspace(xmin,xmax,nx+1))

def rebin(arg,sample,name):
    bins = array('d',[250.,280.,310.,340.,370.,400.,430.,470.,510.,550.,590.,640.,690.,740.,790.,840.,900.,960.,1020.,1090.,1160.,1250.,1400.])
    histo = TH1F(name,'',len(bins)-1,bins)
    histo.Rebin(2)
    b_info.template = histo

@staticmethod
def AddOverflow(hs):
    nbins = hs.GetNbinsX()
    overflow = hs.GetBinContent(nbins) + hs.GetBinContent(nbins+1)
    hs.SetBinContent(nbins,overflow)
    return

def inclusiveBinning(arg,sample,name):
    nbins = int(arg.replace('incl',''))
    b_info.template = TH1F(name,'',nbins,0,1)
    b_info.post = AddOverflow

def inclusiveCutBinning(arg,sample,name):
    nbins = arg.replace('incu','')
    cut = sample.cut
    inclusiveBinning(nbins,sample,name)
    hs = b_info.template
    if '>' in cut:
        lim = float(cut.split('>')[-1])
        bmin = hs.GetXaxis().FindBin(lim); bmax = hs.GetNbinsX()

    binlist = array('d',[ hs.GetXaxis().GetBinLowEdge(ibin) for ibin in range(bmin,bmax+2) ])
    b_info.template= hs.Rebin(len(binlist)-1,name,binlist)
    
b_info.binninglist = {
    'rebin':rebin,
    'incl':inclusiveBinning,
    'incu':inclusiveCutBinning,
}
