#!/usr/bin/env python

from ROOT import *
from sys import argv,path
from PlotTool import *
import config
import os

gROOT.SetBatch(1)
"""
Generate Trigger Efficiency plots
"""

config.mclist = [] #remove mc from being considered

# (HLTMet>>7&1)==1)     HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60_v
# (HLTMet>>8&1)==1)     HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_v
# (HLTMet>>10&1)==1)    HLT_PFMETNoMu140_PFMHTNoMu140_IDTight_v
# (HLTEleMuX>>5&1)==1)  HLT_Ele35_WPTight_Gsf_v
# (HLTEleMuX>>6&1)==1)  HLT_Ele115_CaloIdVT_GsfTrkIdT_v
# (HLTPho>>11&1)==1)    HLT_Photon200_v
trigmap = {
    "1":"PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60",
    "2":"PFMETNoMu120_PFMHTNoMu120_IDTight",
    "3":"PFMETNoMu140_PFMHTNoMu140_IDTight",
    "4":"Ele35_WPTight_Gsf",
    "5":"Ele115_CaloIdVT_GsfTrkIdT",
    "6":"Photon200"
}

parser.add_argument('-num',help='Specify the numerator of trigger efficiency',nargs='+',required=True)
parser.add_argument('-den',help='Specify the denomenator of trigger efficiency',required=True)

samples = Region(show=False)

args = parser.parse_args()

samples.initiate(args.den)
den=samples['Data'].histo.Clone()

def plotTrigEff(num,den):
    _,ndir = GetDirname(num)
    samples.initiate(num)
    samples.trigger = trigmap[ndir]
    samples.varname = samples.varname.replace('_%s'%ndir,'')
    num=samples['Data'].histo.Clone()
    
    eff=GetRatio(num,den)
        
    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    # c.SetLeftMargin(0.15);
    # c.SetLogy();
    # c.cd();

    pad1 = TPad("pad1","pad1",0.02,0.02,0.99,0.99);
    pad1.Draw(); pad1.cd();
    # pad1.SetLogy();
    pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);
    # pad1.SetBottomMargin(0.35);

    DataStyle(eff)
    eff.SetLineColor(kBlack)
    eff.SetLineStyle(1)
    eff.GetYaxis().SetTitle('Trigger Efficiency')
    eff.GetYaxis().SetRangeUser(0,1.2)
    eff.Draw('lpex0')

    lumi_label = '%s' % float('%.3g' % (samples.lumi/1000.)) + " fb^{-1}"
    if (samples.args.normalize): lumi_label="Normalized"
    texLumi,texCMS = getCMSText(lumi_label,samples.year,scale=0.8)
    texLumi.Draw();
    texCMS.Draw();
    SaveAs(c,"eff_%s"%samples.trigger,year=samples.year,sub="TrigEff/%s"%samples.region,exts=".png")

for num in args.num: plotTrigEff(num,den)
