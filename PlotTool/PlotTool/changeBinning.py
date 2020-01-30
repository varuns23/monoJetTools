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

def percentBinning(arg,sample,name):
    nbins = int(arg.replace('perc',''))
    if nbins == 10:
        bins = array('f',[0, 0.33487558364868164, 0.4118022918701172, 0.4752994775772095, 0.5325630903244019, 0.5877697467803955, 0.6420815587043762, 0.6980608105659485, 0.7570456862449646, 0.8272378444671631, 1.1] )
    elif nbins == 20:
        bins = array('f',[0, 0.28098151087760925, 0.33487558364868164, 0.3758664131164551, 0.4118022918701172, 0.44430872797966003, 0.4752994775772095, 0.5040212273597717, 0.5325647592544556, 0.5604197978973389, 0.5877697467803955, 0.6149126291275024, 0.6420828700065613, 0.669815719127655, 0.6980621814727783, 0.7268058061599731, 0.7570472955703735, 0.7895939946174622, 0.8272466659545898, 0.8742754459381104, 1.1] )
    elif nbins == 30:
        bins = array('f',[0, 0.2552464008331299, 0.3010248839855194, 0.3348802924156189, 0.36318421363830566, 0.38882893323898315, 0.4118075370788574, 0.4338323771953583, 0.45493653416633606, 0.4753064513206482, 0.4946276843547821, 0.5137432813644409, 0.5325729250907898, 0.5511188507080078, 0.5697106122970581, 0.5877792835235596, 0.605805516242981, 0.6240274906158447, 0.6420839428901672, 0.6605100631713867, 0.6791989207267761, 0.6980732083320618, 0.7170977592468262, 0.7366478443145752, 0.757056713104248, 0.7784285545349121, 0.8015246987342834, 0.8272632956504822, 0.8568188548088074, 0.8955174684524536, 1.1] )
    elif nbins == 40:
        bins = array('f',[0, 0.23924162983894348, 0.2809852957725525, 0.31024056673049927, 0.3348769545555115, 0.3565257787704468, 0.3758702874183655, 0.39489343762397766, 0.41180410981178284, 0.4283630847930908, 0.44431108236312866, 0.46008819341659546, 0.4752994775772095, 0.4898572564125061, 0.5040220022201538, 0.5183907747268677, 0.5325657725334167, 0.5464531183242798, 0.5604209899902344, 0.5741685628890991, 0.5877729058265686, 0.6011970639228821, 0.614918053150177, 0.6285155415534973, 0.6420839428901672, 0.655813992023468, 0.6698312163352966, 0.6839112043380737, 0.698074460029602, 0.7122794389724731, 0.7268199920654297, 0.7416467070579529, 0.7570573091506958, 0.7729695439338684, 0.7896040081977844, 0.8075507879257202, 0.827260434627533, 0.8487527966499329, 0.8742923140525818, 0.9082523584365845, 1.1] )
    else: return
    b_info.template = TH1F(name,'',nbins,bins)

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

def customBinV1(arg,sample,name):
    bins = array('d',linspace(0,0.2,2)+ linspace(0.2,0.7,10) + linspace(0.7,1.0,12));
    b_info.template = TH1F(name,"",len(bins)-1,bins)
    b_info.post = AddOverflow

def customBinV2(arg,sample,name):
    bins = linspace(0,1,35)
    del bins[1:4]
    bins = array('d',bins)
    b_info.template = TH1F(name,'',len(bins)-1,bins)
    b_info.post = AddOverflow
    

def gradualBinning(arg,sample,name):
    bins = array('d',linspace(0,0.2,2)+linspace(0.2,0.4,6)+linspace(0.4,0.6,7)+linspace(0.6,0.8,8)+linspace(0.8,1.0,9))
    b_info.template = TH1F(name,'',len(bins)-1,bins)
    b_info.template = AddOverflow
    return hs
    
b_info.binninglist = {
    'perc':percentBinning,
    'incl':inclusiveBinning,
    'incu':inclusiveCutBinning,
    'cuv1':customBinV1,
    'grad':gradualBinning,
    'cuv2':customBinV2
}
