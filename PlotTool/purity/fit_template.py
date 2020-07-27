#!/usr/bin/env python
import sys
import re
import os
sys.path.append("PlotTool")
from PlotTool import *
from ROOT import *

import config

# ptbins = [230, 250, 280, 320, 375, 425, 475, 550, "Inf"]
ptbins = [200,250,300,400,500,600,"Inf"]
iarg = 0
for arg in list(sys.argv):
    if "ptbins" in arg:
        sys.argv.remove(arg)
        for i in range(len(ptbins)-1):
            sys.argv.insert(iarg,arg.replace("ptbins","%sto%s"%(ptbins[i],ptbins[i+1])))
            iarg += 1
    iarg += 1

group = parser.add_group(__file__,__doc__,"Script")
group.add_argument("-t","--template",help="Template file to fit",nargs="+",type=TFile,required=True)
group.add_argument("--plot",action="store_true")
group.add_argument("--save",action="store_true")

varmap = {
    "photonPFIso":{
        RooRealVar:RooRealVar("photonPFIso","Photon PF Isolation [GeV]",0.,25.),
        "fullrange":(0.,25.),
        "signal":(0.,lambda pt:2.08 + 0.004017 * pt),
    },
    "photonSieie":{
        RooRealVar:RooRealVar("photonSieie","Photon #sigma_{i#eta i#eta}",0.,0.025),
        "fullrange":(0.,0.025),
        "signal":(0.,lambda pt:0.01015)
    }
}
def hs_style(hs,color):
    hs.SetLineColor(color)
    hs.SetLineWidth(3)
    hs.SetLineStyle(kDashed)
def fit_style(hs,color=kBlue+2):
    hs.SetLineColor(color)
    hs.SetLineStyle(kSolid)
    hs.SetLineWidth(2)
def PtRangeText(x=0.55,y=0.8,ptrange=(-1,-1),scale=1):
    rangetext = TLatex(x,y,"%s < Photon P_{T} < %s"%(ptrange[0],ptrange[1]))
    rangetext.SetNDC()
    rangetext.SetTextFont(42)
    rangetext.SetTextSize(0.05*scale)
    rangetext.Draw()
    return rangetext
def PurityText(x=0.55,y=0.5,purity=0,purity_error=0,scale=1):
    btext = TLatex(x,y,"S/(S+B) = ")
    btext.SetNDC()
    btext.SetTextFont(42)
    btext.SetTextSize(0.05*scale)
    btext.Draw()
    vtext = TLatex(x,y-0.05,"%f #pm %f"%(purity,purity_error))
    vtext.SetNDC()
    vtext.SetTextFont(42)
    vtext.SetTextSize(0.05*scale)
    vtext.Draw()
    return vtext,btext
def VarBounds(variable,hslist):
    if "photonPFIso" in variable:
        SetBounds(hslist,scale=5,log=1)
    elif "photonSieie" in variable:
        SetBounds(hslist,maxi=5,log=1)
def PlotFit(template,roovar,postfit_data,postfit_gjet,postfit_qcd,purity,purity_error):
    postfit_data = postfit_data.Clone()
    postfit_gjet =postfit_gjet.Clone()
    postfit_qcd = postfit_qcd.Clone()

    postfit_fit = postfit_qcd.Clone()
    postfit_fit.Add(postfit_gjet.Clone())

    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    # c.SetLeftMargin(0.15);
    # c.SetLogy();
    #c.cd();

    pad1 = TPad("pad1","pad1",0.01,0.25,0.99,0.99);
    pad1.SetBottomMargin(0.);
    pad1.Draw(); pad1.cd();
    pad1.SetLogy();
    pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);
    

    hs_style(postfit_gjet,kGreen+2)
    hs_style(postfit_qcd,kRed)
    fit_style(postfit_fit,kBlue+2)
    DataStyle(postfit_data)

    leg = getLegend(xmax=0.7,ymin=0.55,ymax=0.75)

    postfit_gjet.Draw("hist")

    xaxis_title = roovar.GetTitle()
    
    postfit_gjet.SetTitle("")
    postfit_gjet.GetYaxis().SetTitle("Events")
    postfit_gjet.GetXaxis().SetTitle(xaxis_title)
    
    postfit_qcd.Draw("hist same")
    postfit_fit.Draw("hist same")
    postfit_data.Draw("0 E1 same")
    hslist = [postfit_data,postfit_gjet,postfit_qcd,postfit_fit]
    
    leg.AddEntry(postfit_data,"Data","P E1")
    leg.AddEntry(postfit_gjet,"Real GJets","l")
    leg.AddEntry(postfit_qcd,"QCD Fake Template","l")
    leg.AddEntry(postfit_fit,"Fit","l")

    VarBounds(template.variable,hslist)
    leg.Draw()
    
    lumi_label = '%s' % float('%.3g' % (max(config.lumi.values())/1000.)) + " fb^{-1}"
    texLumi,texCMS = getCMSText(lumi_label,config.version)

    if re.search("(\d+to\d+|\d+toInf)",template.variable):
        ptrange = template.variable.split("_")[1].split("to")
        rtext = PtRangeText(ptrange=ptrange)

    ftext = PurityText(purity=purity,purity_error=purity_error)
    
    c.cd();
    pad2 = TPad("pad2","pad2",0.01,0.01,0.99,0.25);
    pad2.SetGridy()
    pad2.Draw(); pad2.cd();
    pad2.SetFillColor(0); pad2.SetFrameBorderMode(0); pad2.SetBorderMode(0);
    pad2.SetTopMargin(0);
    pad2.SetBottomMargin(0.35);
    
    ratio = GetRatio(postfit_data,postfit_fit)
    
    RatioStyle(ratio,rymin=-0.25,rymax=2.25,xname=xaxis_title,yname="Data/Fit")
    ratio.GetXaxis().SetLabelSize(0.135)
    ratio.Draw("pex0");
    line = getRatioLine(postfit_data.GetXaxis().GetXmin(),postfit_data.GetXaxis().GetXmax())
    line.Draw("same");
    
    SaveAs(c,"fit_%s"%template.variable,year=config.version,sub="GammaPurity/Fit/")
def make_pdf(histo,roovar,varnum,label,nrange):
    template = RooDataHist("%s_template"%label,"%s template"%label,RooArgList(roovar),histo)
    pdf = RooHistPdf("%s_pdf"%label,"%s pdf"%label,RooArgSet(roovar),template)    
    extpdf = RooExtendPdf(label,"%s extend pdf"%label,pdf,varnum,nrange)
    return template,pdf,extpdf
def fix_zerobins(histo):
    # RooFit doesnt like 0 bins
    for ibin in range(1,histo.GetNbinsX()+1):
        if histo[ibin] == 0: histo.SetBinContent(ibin,0.0001)
def estimates(model,data,realnum,fakenum,show=True):
    def helper():
        realvalue = realnum.getValV()
        realerror = max(abs(realnum.getErrorHi()),abs(realnum.getErrorLo()))
        
        fakevalue = fakenum.getValV()
        fakeerror = max(abs(fakenum.getErrorHi()),abs(fakenum.getErrorLo()))
        
        if show:
            print "realvalue = %d %+d/%+d"%(realvalue,realnum.getErrorHi(),realnum.getErrorLo())
            print "fakevalue = %d %+d/%+d"%(fakevalue,fakenum.getErrorHi(),fakenum.getErrorLo())
        return realvalue,realerror,fakevalue,fakeerror
    realvalue,realerror,fakevalue,fakeerror = helper()
    if (realnum.getErrorHi() == 0 or realnum.getErrorLo() == 0 or fakenum.getErrorHi() == 0 or fakenum.getErrorLo() == 0):
        model.fitTo(data)
        realvalue,realerror,fakevalue,fakeerror = helper()
    return realvalue,realerror,fakevalue,fakeerror

def fit_fraction(roovar,realpdf,realvalue,realerror,fakepdf,fakevalue,fakeerror,data):
    rooset = RooArgSet(roovar)
    norset = RooFit.NormSet(rooset)
    fullrange = RooFit.Range("fullrange")
    signalrange = RooFit.Range("signal")
    
    int_full_fakepdf = fakepdf.createIntegral(rooset,norset,fullrange)
    int_sgnl_fakepdf = fakepdf.createIntegral(rooset,norset,signalrange)
    frac_fakeInSig = int_sgnl_fakepdf.getVal()
    fakeInSig = fakevalue*frac_fakeInSig
    fakeInSig_error = fakeerror*frac_fakeInSig
    
    int_full_realpdf = realpdf.createIntegral(rooset,norset,fullrange)
    int_sgnl_realpdf = realpdf.createIntegral(rooset,norset,signalrange)
    frac_realInSig = int_sgnl_realpdf.getVal()
    realInSig = realvalue*frac_realInSig
    realInSig_error = realerror*frac_realInSig

    # int_full_data = data.createIntegral(rooset,norset,fullrange)
    # int_sgnl_data = data.createIntegral(rooset,norset,signalrange)
    # frac_dataInSig = int_sgnl_data.getVal()

    totalInSig = realInSig + fakeInSig
    totalInSig_error = TMath.Sqrt( realInSig_error**2 + fakeInSig_error**2 )

    # Multiply the numerator observed in data by fakeFractionInNum to get the estimated numerator
    # contribution coming from QCD fakes ( as opposed to real photons )
    # fakeFractionInNum = fakeInSig/(realInSig+fakeInSig)
    purityInNum = realInSig/totalInSig
    purity_error = purityInNum*TMath.Sqrt( (realInSig_error/realInSig)**2 + (totalInSig_error/totalInSig)**2 )

    impurityInNum = fakeInSig/totalInSig
    impurity_error = impurityInNum*TMath.Sqrt( (fakeInSig_error/fakeInSig)**2 + (totalInSig_error/totalInSig)**2 )

    print "Purity:   %f +/- %f"%(purityInNum,purity_error)
    print "Impurity: %f +/- %f"%(impurityInNum,impurity_error)

    return purityInNum,purity_error,impurityInNum,impurity_error
def save_fit(hslist,output,fit):
    tdir = output.mkdir(fit)
    tdir.cd()
    for hs in hslist: hs.Write()
def GetPhotonPt(fname):
    ptbin = re.findall("(\d+to\d+|\d+toInf)",fname)
    if not any(ptbin): return 600
    to_pt = lambda pt: int(pt) if pt != "Inf" else 1000
    ptlo,pthi = [ to_pt(pt) for pt in ptbin[0].split("to") ]
    return (pthi+ptlo)/2
def fit_template(template,output,varinfo):
    prefit_data = template.Get("total_data")
    fix_zerobins(prefit_data)
    prefit_gjet = template.Get("signal_gjets")
    fix_zerobins(prefit_gjet)
    prefit_qcd = template.Get("fake_qcd")
    fix_zerobins(prefit_qcd)

    roovar = varinfo[RooRealVar]

    if parser.args.save: save_fit([prefit_data,prefit_gjet,prefit_qcd],output,"prefit")

    # Variables that will contain real and fake estimates: can vary between 0 and ndata
    ndata = prefit_data.Integral()
    realnum = RooRealVar("realnum","realnum",0,ndata)
    fakenum = RooRealVar("fakenum","fakenum",0,ndata)

    frange = varinfo["fullrange"]
    roovar.setRange("fullrange",frange[0],frange[1])
    faketemplate,fakepdf,fakeextpdf = make_pdf(prefit_qcd,roovar,fakenum,"fake","fullrange")
    realtemplate,realpdf,realextpdf = make_pdf(prefit_gjet,roovar,realnum,"real","fullrange")

    data = RooDataHist("data","data used in fitting",RooArgList(roovar),prefit_data)

    model = RooAddPdf("model","real photons + QCD fakes",RooArgList(realextpdf,fakeextpdf))
    model.fitTo(data,RooFit.Minos())

    realvalue,realerror,fakevalue,fakeerror = estimates(model,data,realnum,fakenum)

    postfit_data = prefit_data.Clone()
    postfit_gjet = prefit_gjet.Clone()
    postfit_gjet.Scale(realvalue/prefit_gjet.Integral())
    postfit_qcd = prefit_qcd.Clone()
    postfit_qcd.Scale(fakevalue/prefit_qcd.Integral())

    pt = GetPhotonPt(template.GetName())
    srange = varinfo["signal"]
    roovar.setRange("signal",srange[0],srange[1](pt))
    # Find fraction of fake/real pdfs in partial range, normalized to 1
    purity,purity_error,impurity,impurity_error = fit_fraction(roovar,realpdf,realvalue,realerror,fakepdf,fakevalue,fakeerror,data)

    h_purity = TH1F("purity","purity",1,0,1)
    h_purity.SetBinContent(1,purity)
    h_purity.SetBinError(1,purity_error)

    h_impurity = TH1F("impurity","impurity",1,0,1)
    h_impurity.SetBinContent(1,impurity)
    h_impurity.SetBinError(1,impurity_error)
    
    if parser.args.save: save_fit([postfit_data,postfit_gjet,postfit_qcd,h_purity,h_impurity],output,"postfit")
    
    if parser.args.plot: PlotFit(template,roovar,postfit_data,postfit_gjet,postfit_qcd,purity,purity_error)
if __name__ == "__main__":
    parser.parse_args()

    if not os.path.isdir("fits") and parser.args.save:
        # Create directory to store fits and make git ignore it
        os.mkdir("fits")
        with open("fits/.gitignore","w") as f: f.write("*")
    
    output = None
    for template in parser.args.template:
        variable = template.GetName().replace("templates/template_","").replace(".root","")
        template.variable = variable
        if parser.args.save:
            output = TFile("fits/fit_%s.root"%variable,"recreate")
            print "Writing fits to",output.GetName()
        fit_template(template,output,next( varinfo for key,varinfo in varmap.items() if key in variable ))
