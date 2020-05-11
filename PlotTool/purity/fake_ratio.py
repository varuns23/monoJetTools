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
parser.add_argument("--plot",action="store_true")
parser.add_argument("--save",action="store_true")

if not os.path.isdir("fake"):
    # Create directory to store fits and make git ignore it
    os.mkdir("fake")
    with open("fake/.gitignore","w") as f: f.write("*")
def fake_style(ratio,color=kBlue+2):
    ratio.SetMarkerStyle(20)
    ratio.SetMarkerColor(kBlack)
    ratio.SetLineWidth(3)
    ratio.SetLineColor(color)
def PlotFakeRatio(ratio):

    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    # c.SetLeftMargin(0.15);
    # c.SetLogy();
    #c.cd();
    
    xaxis_title = "Photon P_{T} [GeV]"

    leg = getLegend(scale=0.75)

    fake_style(ratio)
    ratio.SetTitle("")
    ratio.GetYaxis().SetTitle("QCD Fake Ratio")
    ratio.GetXaxis().SetTitle(xaxis_title)

    ratio.Draw("p same")
    
    leg.AddEntry(ratio,"#frac{Real GJets}{Fake QCD}","lp")
    leg.Draw()

    SetBounds([ratio],scale=0.5)

    lumi_label = '%s' % float('%.3g' % (max(config.lumi.values())/1000.)) + " fb^{-1}"
    texLumi,texCMS = getCMSText(lumi_label,config.version,scale=0.8)
    
    SaveAs(c,"fake_ratio_%s"%parser.args.variable,year=config.version,sub="GammaPurity/FakeRatio/")
def getFitRatio(fname):
    tfile = TFile(fname)
    signal = tfile.Get("postfit/signal_gjets")
    fakeqcd= tfile.Get("postfit/fake_qcd")
    ratio = signal.Integral()/fakeqcd.Integral()
    def get_error(h): return sum( h.GetBinError(ibin) for ibin in range(1,h.GetNbinsX()+1) )
    ratio_error = ratio * TMath.Sqrt( (get_error(signal)/signal.Integral())**2 + (get_error(fakeqcd)/fakeqcd.Integral())**2 )
    return ratio,ratio_error
def save_ratio(ratio):
    output = TFile("fake/fake_ratio_%s.root"%parser.args.variable,"recreate")
    ratio.Write()
    output.Close()
def compute_fake_ratio(ptbins):
    hbins = map(lambda x:x if x is not "Inf" else 1000,ptbins)
    ratio = TH1F("fake_ratio","",len(hbins)-1,array('d',hbins))
    for ibin in range(len(ptbins)-1):
        val,err = getFitRatio( "%s/fit_%s_%sto%s.root"%(parser.args.fit,parser.args.variable,ptbins[ibin],ptbins[ibin+1]) )
        ratio.SetBinContent(ibin+1,val)
        ratio.SetBinError(ibin+1,err)
        print "Bin %s: %f +/- %f" %(ibin,val,err)
        
    if parser.args.plot: PlotFakeRatio(ratio)
    if parser.args.save: save_ratio(ratio)
    
if __name__ == "__main__":
    parser.parse_args()

    ptbins = [230, 250, 280, 320, 375, 425, 475, 550, "Inf"]
    compute_fake_ratio(ptbins)
    
