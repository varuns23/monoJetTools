#!/usr/bin/env python
import sys

import os
sys.path.append("PlotTool")
from PlotTool import *
from ROOT import *
import config
from array import array

parser.add_argument("-f","--fit",help="Post fit directory",default="fits/")
parser.add_argument("-v","--variable",help="Variable used for fits",default="photonSieie")
parser.add_argument("--sys",nargs="+",default=["met"])
parser.add_argument("--plot",action="store_true")
parser.add_argument("--save",action="store_true")

bufunc = {
    "2017":TF1("bu_2017","6.35*exp(-4.61e-03*x)+1.05",200.,1000.),
    "2018":TF1("bu_2018","11.92*exp(-8.28e-03*x)+1.55",200,1000.)
}

if not os.path.isdir("impurity"):
    # Create directory to store fits and make git ignore it
    os.mkdir("impurity")
    with open("impurity/.gitignore","w") as f: f.write("*")
def impurity_style(ratio,color=kBlue+2):
    ratio.SetMarkerStyle(20)
    ratio.SetMarkerColor(kBlack)
    ratio.SetLineWidth(3)
    ratio.SetLineColor(color)
def fit_style(fit,color):
    fit.SetTitle("")
    fit.SetLineColor(color)
    fit.SetLineWidth(3)
def PlotImpurityRatio(impurity):

    
    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    # c.SetLeftMargin(0.15);
    # c.SetLogy();
    #c.cd();

    pad1 = TPad("pad1","pad1",0.01,0.25,0.99,0.99);
    pad1.SetBottomMargin(0.);
    pad1.Draw(); pad1.cd();
    pad1.SetGrid()
    pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);
    
    xaxis_title = "Photon P_{T} [GeV]"

    leg = getLegend(xmin=0.5,ymin=0.7,scale=0.75)

    impurity_style(impurity)
    impurity.SetTitle("")
    impurity.GetYaxis().SetTitle("QCD Impurity (%)")
    # impurity.GetYaxis().SetTitleOffset(0.5)
    impurity.GetXaxis().SetTitle(xaxis_title)
    impurity.GetYaxis().SetRangeUser(0,6)
    # SetBounds([impurity],scale=0.5)

    impurity.Draw("p same")
    fit = impurity.GetFunction("impurity_fit")
    fit_style(fit,kRed)
    
    bu_fit = bufunc[config.version]
    fit_style(bu_fit,kGreen+2)
    bu_fit.Draw("same")
    # fit.Draw("same")
    
    leg.AddEntry(impurity,"#frac{QCD_{Fake}}{GJets_{Real} + QCD_{Fake}}","lp")
    leg.AddEntry(fit,"UW Fit","l")
    leg.AddEntry(bu_fit,"BU Fit","l")
    leg.Draw()

    equation = str(fit.GetExpFormula())
    p0 = "%.3f"%fit.GetParameter("p0");
    p1 = "%.3e"%fit.GetParameter("p1");
    p2 = "%.3f"%fit.GetParameter("p2");
    equation = equation.replace("[p0]",p0).replace("[p1]",p1).replace("[p2]",p2)
    etext = TLatex(0.45,0.65,"f(x) = %s"%equation)
    etext.SetNDC();
    etext.SetTextFont(42);
    etext.SetTextSize(0.048*0.8);
    etext.Draw()
    
    lumi_label = '%s' % float('%.3g' % (max(config.lumi.values())/1000.)) + " fb^{-1}"
    texLumi,texCMS = getCMSText(lumi_label,config.version)

    c.cd();
    pad2 = TPad("pad2","pad2",0.01,0.01,0.99,0.25);
    pad2.SetGridy()
    pad2.Draw(); pad2.cd();
    pad2.SetFillColor(0); pad2.SetFrameBorderMode(0); pad2.SetBorderMode(0);
    pad2.SetTopMargin(0);
    pad2.SetBottomMargin(0.35);
    
    ratio = TF1("ratio","%s/%s"%(bu_fit.GetName(),fit.GetName()),200.,1000.)
    RatioStyle(ratio,rymin=-0.25,rymax=2.25,xname=xaxis_title,yname="BU/UW Fit")
    fit_style(ratio,kBlack)
    ratio.Draw()
    
    line = getRatioLine(200.,1000.)
    line.Draw("same");
    
    SaveAs(c,"impurity_ratio_%s"%parser.args.variable,year=config.version,sub="GammaPurity/ImpurityRatio/")
def PlotImpuritySys(nominal,ptbins):
    if not any(parser.args.sys): return
    sysmap = {}
    for sys in parser.args.sys:
        up = get_impurity_ratio("{fitdir}/fit_{variable}_{ptbin}_%sup.root"%sys,ptbins,"impurity_ratio_%sup"%sys)
        dn = get_impurity_ratio("{fitdir}/fit_{variable}_{ptbin}_%sdn.root"%sys,ptbins,"impurity_ratio_%sdn"%sys)
        sysmap[sys] = Nuisance("impurity",sys,up,dn,nominal,type="abs")
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
            hs.GetYaxis().SetTitle("Impurity Systematics")
            hs.GetXaxis().SetTitle(xaxis_title)
            hslist.append(hs)
        leg.AddEntry(up,sysname,"l")
    SetBounds(hslist)
    leg.Draw()

    lumi_label = '%s' % float('%.3g' % (max(config.lumi.values())/1000.)) + " fb^{-1}"
    texLumi,texCMS = getCMSText(lumi_label,config.version,scale=0.8)
    
    c.RedrawAxis()
    SaveAs(c,"impurity_ratio_%s_systematics"%parser.args.variable,year=config.version,sub="GammaPurity/ImpurityRatio/")
def getFitRatio(fname):
    tfile = TFile(fname)
    # signal = tfile.Get("postfit/signal_gjets")
    # fakeqcd= tfile.Get("postfit/fake_qcd")
    # ratio = signal.Integral()/(fakeqcd.Integral()+signal.Integral())
    # def get_error(h): return sum( h.GetBinError(ibin) for ibin in range(1,h.GetNbinsX()+1) )
    # ratio_error = ratio * TMath.Sqrt( (get_error(signal)/signal.Integral())**2 + TMath.Sqrt((get_error(signal)/signal.Integral())**2 + (get_error(fakeqcd)/fakeqcd.Integral())**2) )
    impurity = tfile.Get("postfit/impurity")
    ratio = impurity.GetBinContent(1)
    ratio_error = impurity.GetBinError(1)
    return ratio,ratio_error
def save_obj(objlist):
    output = TFile("impurity/impurity_ratio_%s.root"%(parser.args.variable),"recreate")
    for obj in objlist: obj.Write()
    output.Close()

def get_impurity_ratio(pattern,ptbins,name="impurity_ratio"):
    hbins = map(lambda x:x if x is not "Inf" else 1000,ptbins)
    ratio = TH1F(name,"",len(hbins)-1,array('d',hbins))
    fitdir = parser.args.fit
    variable = parser.args.variable
    pattern = pattern.format(fitdir=fitdir,variable=variable,ptbin="{ptbin}")
    print "Pattern:",pattern
    for ibin in range(len(ptbins)-1):
        ptbin = "%sto%s"%(ptbins[ibin],ptbins[ibin+1])
        val,err = getFitRatio( pattern.format(ptbin=ptbin) )
        val *= 100
        err *= 100
        ratio.SetBinContent(ibin+1,val)
        ratio.SetBinError(ibin+1,err)
        print "Bin %s: %f +/- %f" %(ibin,val,err)
    return ratio
def compute_impurity_ratio(ptbins):
    ratio = get_impurity_ratio("{fitdir}/fit_{variable}_{ptbin}.root",ptbins)
    func = TF1("impurity_fit","[0]*exp(-[1]*x)+[2]",200.,1000.)
    func.SetParameters(6,0.001,1)
    ratio.Fit("impurity_fit")
    if parser.args.plot:
        PlotImpurityRatio(ratio)
        PlotImpuritySys(ratio,ptbins)
    if parser.args.save: save_obj([ratio,func])
    
if __name__ == "__main__":
    parser.parse_args()
    # ptbins = [230, 250, 280, 320, 375, 425, 475, 550, "Inf"]
    ptbins = [200,250,300,400,500,600,"Inf"]
    compute_impurity_ratio(ptbins)
    
