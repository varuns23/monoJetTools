#!/usr/bin/env python
import sys

import os
sys.path.append("PlotTool")
from PlotTool import *
from ROOT import *
import config
from array import array

parser.add_argument("-f","--fit",help="Post fit directory",required=True)
parser.add_argument("-v","--variable",help="Variable used for fits",default="photonPFIso")
parser.add_argument("--sys",nargs="+",default=["met","sb"])
parser.add_argument("--plot",action="store_true")
parser.add_argument("--save",action="store_true")

if not os.path.isdir("purity"):
    # Create directory to store fits and make git ignore it
    os.mkdir("purity")
    with open("purity/.gitignore","w") as f: f.write("*")
def purity_style(ratio,color=kBlue+2):
    ratio.SetMarkerStyle(20)
    ratio.SetMarkerColor(kBlack)
    ratio.SetLineWidth(3)
    ratio.SetLineColor(color)
def PlotPurityRatio(ratio):

    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    c.SetGrid()
    # c.SetLeftMargin(0.15);
    # c.SetLogy();
    #c.cd();
    
    xaxis_title = "Photon P_{T} [GeV]"

    leg = getLegend(xmin=0.5,ymin=0.7,scale=0.75)

    purity_style(ratio)
    ratio.SetTitle("")
    ratio.GetYaxis().SetTitle("QCD Purity Ratio")
    # ratio.GetYaxis().SetTitleOffset(0.5)
    ratio.GetXaxis().SetTitle(xaxis_title)
    ratio.GetYaxis().SetRangeUser(0.75,1.2)

    ratio.Draw("p same")
    
    leg.AddEntry(ratio,"#frac{GJets_{Real}}{GJets_{Real} + QCD_{Fake}}","lp")
    leg.Draw()

    lumi_label = '%s' % float('%.3g' % (max(config.lumi.values())/1000.)) + " fb^{-1}"
    texLumi,texCMS = getCMSText(lumi_label,config.version,scale=0.8)
    
    SaveAs(c,"purity_ratio_%s"%parser.args.variable,year=config.version,sub="GammaPurity/PurityRatio/")
def PlotPuritySys(nominal,ptbins):
    if not any(parser.args.sys): return
    sysmap = {}
    for sys in parser.args.sys:
        up = get_purity_ratio("{fitdir}/fit_{variable}_{ptbin}_%sup.root"%sys,ptbins,"purity_ratio_%sup"%sys)
        dn = get_purity_ratio("{fitdir}/fit_{variable}_{ptbin}_%sdn.root"%sys,ptbins,"purity_ratio_%sdn"%sys)
        sysmap[sys] = Nuisance("purity",sys,up,dn,nominal,type="abs")
    colors = [kRed,kBlue,kGreen]
    

    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    c.SetGrid()
    # c.SetLeftMargin(0.15);
    # c.SetLogy();
    #c.cd();
    
    xaxis_title = "Photon P_{T} [GeV]"

    leg = getLegend(xmin=0.5,ymin=0.7,scale=0.75)

    def sys_style(hs,color):
        hs.SetLineColor(color)
        hs.SetLineWidth(2)
        hs.SetFillColor(0)
    coliter = iter(colors)
    hslist = []
    for sysname in parser.args.sys:
        sys = sysmap[sysname]
        up,dn = sys.GetScale()
        color = next(coliter)
        for hs in (up,dn):
            sys_style(hs,color)
            hs.Draw("hist same")
            hs.SetTitle("")
            hs.GetYaxis().SetTitle("Purity Systematics")
            hs.GetXaxis().SetTitle(xaxis_title)
            hslist.append(hs)
        leg.AddEntry(up,sysname,"l")
    SetBounds(hslist)
    leg.Draw()

    lumi_label = '%s' % float('%.3g' % (max(config.lumi.values())/1000.)) + " fb^{-1}"
    texLumi,texCMS = getCMSText(lumi_label,config.version,scale=0.8)
    
    c.RedrawAxis()
    SaveAs(c,"purity_ratio_%s_systematics"%parser.args.variable,year=config.version,sub="GammaPurity/PurityRatio/")
def getFitRatio(fname):
    tfile = TFile(fname)
    # signal = tfile.Get("postfit/signal_gjets")
    # fakeqcd= tfile.Get("postfit/fake_qcd")
    # ratio = signal.Integral()/(fakeqcd.Integral()+signal.Integral())
    # def get_error(h): return sum( h.GetBinError(ibin) for ibin in range(1,h.GetNbinsX()+1) )
    # ratio_error = ratio * TMath.Sqrt( (get_error(signal)/signal.Integral())**2 + TMath.Sqrt((get_error(signal)/signal.Integral())**2 + (get_error(fakeqcd)/fakeqcd.Integral())**2) )
    purity = tfile.Get("postfit/purity")
    ratio = purity.GetBinContent(1)
    ratio_error = purity.GetBinError(1)
    return ratio,ratio_error
def save_ratio(ratio):
    output = TFile("purity/purity_ratio_%s.root"%(parser.args.variable),"recreate")
    ratio.Write()
    output.Close()

def get_purity_ratio(pattern,ptbins,name="purity_ratio"):
    hbins = map(lambda x:x if x is not "Inf" else 1000,ptbins)
    ratio = TH1F(name,"",len(hbins)-1,array('d',hbins))
    fitdir = parser.args.fit
    variable = parser.args.variable
    pattern = pattern.format(fitdir=fitdir,variable=variable,ptbin="{ptbin}")
    print "Pattern:",pattern
    for ibin in range(len(ptbins)-1):
        ptbin = "%sto%s"%(ptbins[ibin],ptbins[ibin+1])
        val,err = getFitRatio( pattern.format(ptbin=ptbin) )
        ratio.SetBinContent(ibin+1,val)
        ratio.SetBinError(ibin+1,err)
        print "Bin %s: %f +/- %f" %(ibin,val,err)
    return ratio
def compute_purity_ratio(ptbins):
    ratio = get_purity_ratio("{fitdir}/fit_{variable}_{ptbin}.root",ptbins)
    if parser.args.plot:
        PlotPurityRatio(ratio)
        PlotPuritySys(ratio,ptbins)
    if parser.args.save: save_ratio(ratio)
    
if __name__ == "__main__":
    parser.parse_args()
    # ptbins = [230, 250, 280, 320, 375, 425, 475, 550, "Inf"]
    ptbins = [200,250,300,400,500,600,"Inf"]
    compute_purity_ratio(ptbins)
    
