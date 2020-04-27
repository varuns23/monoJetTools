from ROOT import *
import os
import sys
sys.path.append("PlotTool")
from PlotTool import *
from PlotTool import parser
parser.add_argument("--plot",action="store_true")
parser.parse_args()

gROOT.SetBatch(1)

regions = ["SignalRegion","SingleEleCR","SingleMuCR","DoubleEleCR","DoubleMuCR","GammaCR"]
procs = {
    "ZJets":("SignalRegion",),
    "WJets":("SignalRegion","SingleEleCR","SingleMuCR"),
    "GJets":("GammaCR",),
    "DYJets":("DoubleEleCR","DoubleMuCR")
}

pswlist = ["PSW_isrCon","PSW_fsrCon"]

def AverageHistos(hslist):
    histo = hslist[0].Clone()
    for ibin in range(1,histo.GetNbinsX()+1):
        bin = 0
        for hs in hslist: bin += hs[ibin]
        bin /= len(hslist)
        histo.SetBinContent(ibin,bin)
    return histo
def MakeSymmetric(up,dn,bias="up"):
    sign = lambda x : (1,-1)[x<0]
    for ibin in range(1,up.GetNbinsX()+1):
        val = max( abs(up[ibin]-1),abs(dn[ibin]-1) )
        if bias == "up":
            up[ibin] = sign(up[ibin]-1)*val+1
            dn[ibin] =-sign(up[ibin]-1)*val+1
        elif bias == "dn":
            up[ibin] =-sign(dn[ibin]-1)*val+1
            dn[ibin] = sign(dn[ibin]-1)*val+1
def PlotPSW(process,psw,raw,smooth):
    sf_up,sf_dn = raw
    up_smooth,dn_smooth = smooth
    
    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    # c.SetLeftMargin(0.15);
    # c.SetLogy();
    # c.cd();
    
    pad1 = TPad("pad1","pad1",0.,0.,1.0,1.0);
    pad1.Draw(); pad1.cd();
    pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);
    pad1.SetLeftMargin(0.15)
    pad1.SetGridy()
    # pad1.SetBottomMargin(0.35);
    
    upcol=kBlack
    dncol=kBlack
    DataStyle(sf_up);
    sf_up.SetMarkerColor(upcol)
    up_smooth.SetLineColor(upcol)
    up_smooth.SetLineWidth(2)
    DataStyle(sf_dn);
    sf_dn.SetMarkerColor(dncol)
    dn_smooth.SetLineColor(dncol)
    dn_smooth.SetLineWidth(2)
    
    binlist = list(sf_up)[1:-1]+list(sf_dn)[1:-1]
    ymin = min(binlist)
    ymax = max(binlist)
    diff = ymax - ymin
    ymax += diff
    ymin -= diff
    # ymin = 0.9
    # ymax = 1.1
    sf_up.Draw("hist p");
    sf_up.GetYaxis().SetRangeUser(ymin,ymax);
    sf_up.GetYaxis().SetTitle(psw)
    sf_up.GetXaxis().SetTitle("Ch + NEM P^{123}_{T} Fraction")
    sf_dn.Draw("hist p same")
    up_smooth.Draw("hist l same")
    dn_smooth.Draw("hist l same")
    
    leg = getLegend(xmax=0.75,ymin=0.7,ymax=0.9,scale=0.8)
    leg.AddEntry(sf_up,"Raw","p")
    leg.AddEntry(up_smooth,"Smoothed","l")
    leg.Draw()
    
    SaveAs(c,psw,year="2018",sub="PSWeights/%s"%process)
def ProducePSW(process,tfiles,psw):
    print process,psw
    tdirs = [ tfile.GetDirectory(process) for tfile in tfiles ]
    def ProduceVariation(tdirs,psw):
        hslist = [ tdir.Get(psw) for tdir in tdirs ]
        raw = AverageHistos(hslist)
        for hs in hslist: hs.Smooth(1,"r")
        smooth = AverageHistos(hslist)
        return raw,smooth
    rup,sup = ProduceVariation(tdirs,psw+'Up')
    rdn,sdn = ProduceVariation(tdirs,psw+'Down')
    MakeSymmetric(rup,rdn)
    MakeSymmetric(sup,sdn)
    if parser.args.plot: PlotPSW(process,psw,(rup,rdn),(sup,sdn))
    return sup,sdn

filemap = { region:TFile("%s/PSW_SF.root"%region) for region in regions }
output = TFile("PSW_SF.root","RECREATE")
for proc,filelist in procs.items():
    tfiles = [ filemap[region] for region in filelist ]
    output.cd()
    output.mkdir(proc).cd()
    for psw in pswlist:
        up,dn = ProducePSW(proc,tfiles,psw)
        up.Write()
        dn.Write()
