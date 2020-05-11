#!/usr/bin/env python
import sys

import os
sys.path.append("PlotTool")
from PlotTool import *
from ROOT import *
import config

parser.add_argument("-t","--template",help="Template file to fit",nargs="+",type=TFile,required=True)
parser.add_argument("--plot",action="store_true")
parser.add_argument("--save",action="store_true")

if not os.path.isdir("fits"):
    # Create directory to store fits and make git ignore it
    os.mkdir("fits")
    with open("fits/.gitignore","w") as f: f.write("*")

varmap = {
    "photonPFIso":RooRealVar("photonPFIso","Photon PF Isolation",0.,25.)
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
def PlotFit(template,postfit_data,postfit_gjet,postfit_qcd,purity,purity_error):
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

    xaxis_title = "Photon PF Isolation [GeV]"
    
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

    SetBounds(hslist,scale=5,log=10)
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

def fit_fraction(roovar,realpdf,realvalue,realerror,fakepdf,fakevalue,fakeerror):
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

    totalInSig = realInSig + fakeInSig
    totalInSig_error = TMath.Sqrt( realInSig_error**2 + fakeInSig_error**2 )

    # Multiply the numerator observed in data by fakeFractionInNum to get the estimated numerator
    # contribution coming from QCD fakes ( as opposed to real photons )
    # fakeFractionInNum = fakeInSig/(realInSig+fakeInSig)
    purityInNum = realInSig/(realInSig + fakeInSig)
    purity_error = purityInNum*TMath.Sqrt( (fakeInSig_error/fakeInSig)**2 + (totalInSig_error/totalInSig)**2 )

    print "Purity: %f +/- %f"%(purityInNum,purity_error)

    return purityInNum,purity_error
def save_fit(hslist,output,fit):
    tdir = output.mkdir(fit)
    tdir.cd()
    for hs in hslist: hs.Write()
def fit_template(template,output,roovar=varmap["photonPFIso"]):
    prefit_data = template.Get("data")
    fix_zerobins(prefit_data)
    prefit_gjet = template.Get("signal_gjets")
    fix_zerobins(prefit_gjet)
    prefit_qcd = template.Get("fake_qcd")
    fix_zerobins(prefit_qcd)

    if parser.args.save: save_fit([prefit_data,prefit_gjet,prefit_qcd],output,"prefit")

    # Variables that will contain real and fake estimates: can vary between 0 and ndata
    ndata = prefit_data.Integral()
    realnum = RooRealVar("realnum","realnum",0,ndata)
    fakenum = RooRealVar("fakenum","fakenum",0,ndata)

    roovar.setRange("fullrange",0.,25.)
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

    roovar.setRange("signal",0.,10.)
    # Find fraction of fake/real pdfs in partial range, normalized to 1
    purity,purity_error = fit_fraction(roovar,realpdf,realvalue,realerror,fakepdf,fakevalue,fakeerror)

    h_purity = TH1F("purity","purity",1,0,1)
    h_purity.SetBinContent(1,purity)
    h_purity.SetBinError(1,purity_error)
    
    if parser.args.save: save_fit([postfit_data,postfit_gjet,postfit_qcd,h_purity],output,"postfit")
    
    if parser.args.plot: PlotFit(template,postfit_data,postfit_gjet,postfit_qcd,purity,purity_error)
if __name__ == "__main__":
    parser.parse_args()
    output = None
    for template in parser.args.template:
        variable = template.GetName().replace("templates/template_","").replace(".root","")
        template.variable = variable
        if parser.args.save:
            output = TFile("fits/fit_%s.root"%variable,"recreate")
            print "Writing fits to",output.GetName()
        fit_template(template,output)
