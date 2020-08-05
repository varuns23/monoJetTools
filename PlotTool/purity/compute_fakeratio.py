#!/usr/bin/env python

"""
Compute fake ratio from Photon Purity fits
Usage: python PlotTool/compute_fakeratio.py -v variable
"""

import os
import sys
sys.path.append("PlotTool")
from PlotTool import *
from ROOT import *
from ROOT import Double
import config
import re
config.mclist = []

group = parser.add_group(__file__,__doc__,"Script")
group.add_argument("-v","--variable",help="Variable used for fits",default="photonPFIso")
group.add_argument("--plot",action="store_true")
group.add_argument("--save",action="store_true")

if not os.path.isdir("fakeratio"):
    # Create directory to store templates and make git ignore it
    os.mkdir("fakeratio")
    with open("fakeratio/.gitignore","w") as f: f.write("*")
    
from ROOT import TCanvas,gStyle,kRed,kGreen,kBlue,TLatex,TPad

num_path = "SigTemplate/sieie_purity/"
den_path = "DenTemplate"

num_template = Region(path=num_path,autovar=True,show=0)
den_template = Region(path=den_path,autovar=True,show=0)

xaxismap = {
    "photonPFIso":"Photon PF Isolation [GeV]",
    "photonSieie":"Photon #sigma_{i#eta i#eta}"
}
def fake_style(ratio,color=kGreen+2):
    ratio.SetMarkerStyle(20)
    ratio.SetMarkerColor(kBlack)
    ratio.SetLineWidth(3)
    ratio.SetLineColor(color)
def save_ratio(ratio):
    output = TFile("fakeratio/fake_ratio_%s.root"%(parser.args.variable),"recreate")
    ratio.Write()
    output.Close()
def PlotFakeRatio(ratio):
    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);

    pad = TPad("pad","pad",0.01,0.01,0.99,0.99)
    pad.Draw(); pad.cd();
    pad.SetGrid()
    pad.SetFillColor(0); pad.SetFrameBorderMode(0); pad.SetBorderMode(0);
    
    xaxis_title = "Photon P_{T} [GeV]"

    leg = getLegend(xmin=0.5,ymin=0.7,scale=0.75)

    fake_style(ratio)
    ratio.SetTitle("")
    ratio.GetYaxis().SetTitle("Fake Ratio")
    # ratio.GetYaxis().SetTitleOffset(0.5)
    ratio.GetXaxis().SetTitle(xaxis_title)
    # ratio.GetYaxis().SetRangeUser(0.75,1.2)
    print SetBounds([ratio],scale=1)

    ratio.Draw("p same")
    
    leg.AddEntry(ratio,"#frac{Data_{ISO}}{Data_{Non-ISO}}","lp")
    leg.Draw()

    lumi_label = '%s' % float('%.3g' % (max(config.lumi.values())/1000.)) + " fb^{-1}"
    texLumi,texCMS = getCMSText(lumi_label,config.version,scale=0.8)
    
    SaveAs(c,"fake_ratio_%s"%parser.args.variable,year=config.version,sub="GammaPurity/FakeRatio/")
def get_fakeratio(ptbins):
    hbins = map(lambda x:x if x is not "Inf" else 1000,ptbins)
    fakeratio = TH1F("fakeratio","",len(hbins)-1,array('d',hbins))
    for ibin in range( len(ptbins)-1 ):
        ptbin = "%sto%s"%(ptbins[ibin],ptbins[ibin+1])
        variable = "%s_%s"%(parser.args.variable,ptbin)
        num_template.initiate(variable)
        den_template.initiate(variable)

        num = num_template["Data"].histo.Clone()
        den = den_template["Data"].histo.Clone()

        num_err = Double()
        num_int = num.IntegralAndError(1,num.GetNbinsX(),num_err)
        
        den_err = Double()
        den_int = den.IntegralAndError(1,den.GetNbinsX(),den_err)

        val = (num_int/den_int)
        err = val*TMath.Sqrt( (num_err/num_int)**2 + (den_err/den_int)**2 )
        fakeratio.SetBinContent(ibin+1,val)
        fakeratio.SetBinError(ibin+1,err)
    return fakeratio
def compute_fakeratio(ptbins):
    fakeratio = get_fakeratio(ptbins)

    if parser.args.plot: PlotFakeRatio(fakeratio)
    if parser.args.save: save_ratio(fakeratio)
    
if __name__ == "__main__":
    parser.parse_args()
    ptbins = [230, 250, 280, 320, 375, 425, 475, 550, "Inf"]
    compute_fakeratio(ptbins)
