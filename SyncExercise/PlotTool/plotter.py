#!/usr/bin/env python

from ROOT import *
from sys import argv, path
import Plot as plot
import os

gROOT.SetBatch(1)

store = [] # Storage list to be used to keep references around for ROOT before TCanvas is saved

def HigherDimension(samples,variable):
    axis = variable[-1]
    samples.initiate(variable[:-1])
    if (axis in ('x','y','z')):
        samples.name = samples.name[axis]
    for name,process in samples.processes.iteritems():
        if axis == "x":
           process.histo = process.histo.ProjectionX()
        if axis == "y":
            process.histo = process.histo.ProjectionY()
        if axis == "z":
            process.histo = process.histo.ProjectionZ()
            
###################################################################
def DataStyle(hs_data):
    hs_data.SetLineWidth(2)
    hs_data.SetLineColor(kWhite);
    hs_data.SetTitle("");
    hs_data.GetXaxis().SetTitle("");
    hs_data.GetXaxis().SetTickLength(0);
    hs_data.GetXaxis().SetLabelOffset(999);
    hs_data.GetYaxis().SetTitle("");
    hs_data.GetYaxis().SetTickLength(0);
    hs_data.GetYaxis().SetLabelOffset(999);
    hs_data.SetLineColor(kBlack);
    hs_data.SetMarkerStyle(20);
    hs_data.SetMarkerSize(0.9);
###################################################################
    
def MCStyle(hs_mc,color):
    hs_mc.SetTitle("");
    hs_mc.GetXaxis().SetTitle("");
    hs_mc.GetXaxis().SetTickLength(0);
    hs_mc.GetXaxis().SetLabelOffset(999);
    hs_mc.GetYaxis().SetTitle("");
    hs_mc.GetYaxis().SetTickLength(0);
    hs_mc.GetYaxis().SetLabelOffset(999);
    hs_mc.SetFillColor(color);
###################################################################

def fillStack(samples,hs_datamc):
    order = [ process for name,process in samples.processes.iteritems() if process.proctype == 'bkg' ]
    if (samples.name == "Cutflow"):
        nbin = samples.processes['Data'].histo.GetNbinsX()
        order.sort(key=lambda process: process.histo.GetBinContent(nbin))
    else: order.sort(key=lambda process: process.scaled_total)
    for process in order: hs_datamc.Add(process.histo)
###################################################################

def getLegend(xmin,ymin,xmax,ymax):
    leg = TLegend(xmin,ymin,xmax,ymax,"")
    leg.SetFillColor(kWhite);
    leg.SetFillStyle(0);
    leg.SetTextSize(0.025);
    return leg
###################################################################

def getCMSText(lumi,year):
    texS = TLatex(0.20,0.837173,("#sqrt{s} = 13 TeV, "+lumi));
    texS.SetNDC();
    texS.SetTextFont(42);
    texS.SetTextSize(0.040);
    texS.Draw();
    texS1 = TLatex(0.12092,0.907173,"#bf{CMS} : #it{Preliminary} ("+year+")");
    texS1.SetNDC();
    texS1.SetTextFont(42);
    texS1.SetTextSize(0.040);
    texS1.Draw();
    return texS,texS1
###################################################################

def RatioStyle(ratio,rymin,rymax):
    ratio.GetYaxis().SetRangeUser(rymin,rymax);
    ratio.SetStats(0);
    ratio.GetYaxis().CenterTitle();
    ratio.SetMarkerStyle(20);
    ratio.SetMarkerSize(0.7);
    ratio.GetYaxis().SetLabelSize(0.14);
    ratio.GetYaxis().SetTitleSize(0.12);
    ratio.GetYaxis().SetLabelFont(42);
    ratio.GetYaxis().SetTitleFont(42);
    ratio.GetYaxis().SetTitleOffset(0.25);
    ratio.GetYaxis().SetNdivisions(100);
    ratio.GetYaxis().SetTickLength(0.05);
    
    ratio.GetXaxis().SetLabelSize(0.15);
    ratio.GetXaxis().SetTitleSize(0.12);
    ratio.GetXaxis().SetLabelFont(42);
    ratio.GetXaxis().SetTitleFont(42);
    ratio.GetXaxis().SetTitleOffset(0.9);
    ratio.GetXaxis().SetTickLength(0.05);
###################################################################

def getRatioLine(xmin,xmax):
    line = TLine(xmin, 1.,xmax, 1.);
    line.SetLineStyle(8);
    line.SetLineColor(kBlack);
    return line
###################################################################

def StackStyle(hs_stack,ymin,ymax):
    hs_stack.SetMinimum(ymin);
    hs_stack.SetMaximum(ymax);
    hs_stack.GetYaxis().SetTitle("Events");
    hs_stack.GetYaxis().SetTitleOffset(1.5);
    hs_stack.SetTitle("");
###################################################################

def makeXaxis(xmin,xmax,ymin,ndiv,name=None):
    xaxis = TGaxis(xmin,ymin,xmax,ymin,xmin,xmax,ndiv);
    if name != None: xaxis.SetTitle(name);
    xaxis.SetLabelFont(42);
    xaxis.SetLabelSize(0.10);
    xaxis.SetTitleFont(42);
    xaxis.SetTitleSize(0.12);
    xaxis.SetTitleOffset(1.2);
    return xaxis
###################################################################

def XaxisCutflowStyle(xaxis,hs):
    xaxis.SetLabelOffset(-999)
    xaxis.SetTitle("");
    for i in range(1,hs.GetXaxis().GetNbins()+1):
        label = TLatex(i-0.5,xaxis.GetY1()-0.2,hs.GetXaxis().GetBinLabel(i));
	label.SetTextSize(0.065);
	label.SetTextAngle(-30.);
	label.Draw("SAME");
        store.append(label)
###################################################################

def makeYaxis(ymin,ymax,xmin,ndiv,name=None):
    if name == None: name == 'YAxis'
    yaxis = TGaxis(xmin,ymin,xmin,ymax,ymin,ymax,ndiv,"");
    yaxis.SetTitle(name);
    yaxis.SetLabelFont(42);
    yaxis.SetLabelSize(0.10);
    yaxis.SetTitleFont(42);
    yaxis.SetTitleSize(0.12);
    yaxis.SetTitleOffset(0.35);
    return yaxis
###################################################################

def plotVariable(samples,variable):
    del store[:] # Clear storage list 
    print "Plotting",variable
    if (samples.options.thn):
        HigherDimension(samples,variable)
    else:
        samples.initiate(variable)
    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    #c.SetLeftMargin(0.15);
    #c.SetLogy();
    #c.cd();
    
    pad1 = TPad("pad1","pad1",0.01,0.25,0.99,0.99);
    pad1.Draw(); pad1.cd();
    pad1.SetLogy();
    pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);
    pad1.SetBottomMargin(0.);

    data = samples.processes['Data']
    DataStyle(data.histo)
    if (samples.options.normalize): data.histo.Scale(1/data.total)

    for mc in samples.MCList:
        mc_proc = samples.processes[mc]
        MCStyle(mc_proc.histo,mc_proc.color)
        if (samples.options.normalize): mc_proc.histo.Scale(1/samples.BkgIntegral)
        

    hs_datamc = THStack("hs_datamc","Data/MC comparison");
    fillStack(samples,hs_datamc)
    ymin_s=pow(10,-6);ymax_s=pow(10,2.5);
    ymin = 0.1 if not samples.options.normalize else hs_datamc.GetMaximum()*ymin_s
    ymax = hs_datamc.GetMaximum()*ymax_s
    hs_datamc.Draw("HIST")
    StackStyle(hs_datamc,ymin,ymax)

    data.histo.Draw('pex0same')
    
    if samples.signal != None:
        signal = samples.processes['Signal']
        signal[0].histo.SetLineWidth(2)
        signal[0].histo.Draw("HIST SAME")

    #################################################

    leg = getLegend(0.62,0.60,0.86,0.887173);
    leg.AddEntry(data.histo,"Data","lp");
    if (samples.signal != None): leg.AddEntry(signal[0].histo, signal[0].name)   
    leg.AddEntry(samples.processes['WJets'].histo  ,"W#rightarrowl#nu","f");
    leg.AddEntry(samples.processes['DYJets'].histo ,"Z#rightarrow ll","F"); 
    leg.AddEntry(samples.processes['DiBoson'].histo,"WW/WZ/ZZ","F");
    leg.AddEntry(samples.processes['QCD'].histo    ,"QCD","F");
    leg.AddEntry(samples.processes['TTJets'].histo , "Top Quark", "F"); 
    leg.AddEntry(samples.processes['GJets'].histo  ,"#gamma+jets", "F"); 
    leg.AddEntry(samples.processes['ZJets'].histo  ,"Z#rightarrow#nu#nu","F");
    leg.Draw();

    lumi_label = '%s' % float('%.3g' % (samples.lumi/1000.)) + " fb^{-1}"
    if (samples.options.normalize): lumi_label="Normalized"
    texLumi,texCMS = getCMSText(lumi_label,samples.version)
    texLumi.Draw();
    texCMS.Draw();

    c.cd();
    pad2 = TPad("pad2","pad2",0.01,0.01,0.99,0.25);
    pad2.Draw(); pad2.cd();
    pad2.SetFillColor(0); pad2.SetFrameBorderMode(0); pad2.SetBorderMode(0);
    pad2.SetTopMargin(0);
    pad2.SetBottomMargin(0.35);

    ######################################

    Ratio = plot.GetRatio(data.histo,hs_datamc.GetStack().Last())

    rymin = 0.3; rymax = 1.7
    RatioStyle(Ratio,rymin,rymax)
    Ratio.Draw("pex0");
    
    line = getRatioLine(hs_datamc.GetXaxis().GetXmin(),hs_datamc.GetXaxis().GetXmax())
    line.Draw("same");

    c.Update();

    ###########################################

    nbins = data.histo.GetNbinsX();
    xmin = hs_datamc.GetXaxis().GetXmin();
    xmax = hs_datamc.GetXaxis().GetXmax();
    xwmin = xmin;
    xwmax = xmax;

    xname = samples.name if type(samples.name) == str else None
    xaxis = makeXaxis(xmin,xmax,rymin,510,name=xname);
    xaxis.Draw("SAME");

    if (samples.name == "Cutflow"): XaxisCutflowStyle(xaxis,hs_datamc)
      

    yaxis = makeYaxis(rymin,rymax,xmin,6,name="Data/MC");
    yaxis.Draw("SAME");

    dir = os.getcwd().split("/")[-1]
    file_path="/afs/hep.wisc.edu/home/ekoenig4/public_html/MonoZprimeJet/Plots"+samples.version+"/"+dir+"Plots_EWK/"
    #print file_path
    sub = ""
    if (samples.options.allHisto):sub = "all"
    if (samples.options.sub != None): sub = samples.options.sub
    directory=os.path.join(os.path.dirname(file_path),sub)
    if not os.path.exists(directory):
        os.mkdir(directory,0755)
        print directory
    c.SaveAs(directory+"/datamc_"+samples.varname+".pdf")
    c.SaveAs(directory+"/datamc_"+samples.varname+".png")
###################################################################
    
def plotter():
    samples = plot.datamc()
    for variable in samples.args:
        plotVariable(samples,variable)
###################################################################
    
  

if __name__ == "__main__":
    plotter()
