#!/usr/bin/env python

from ROOT import *
from sys import argv,path
from PlotTool import *
import config
import os

"""
Generates pileup comparison plots 
example https://www.hep.wisc.edu/~ekoenig4/MonoZprimeJet/Plots2017/SingleEleCRPlots_EWK/pileupComparison.png
Directions: In region directory with all proper files present, and the correct year dataPileup.root file contained in the PlotTool Directory
Usage: python ../PlotTool/makePileup.py
"""

gROOT.SetBatch(1)

sample = Region(show=0,autovar=True)

sample.initiate('bosonPt','genWeight')
sample.setSumOfBkg()
unweightedBKG = sample['SumOfBkg'].histo.Clone()
sample.initiate('bosonPt','genWeight*kfactor')
sample.setSumOfBkg()
weightedBKG = sample['SumOfBkg'].histo.Clone()

weightedEvents = weightedBKG.Integral()
unweightedEvents = unweightedBKG.Integral()
print 'weighted/gen: %.3f' % (weightedEvents/unweightedEvents)

xmin,xmax = weightedBKG.GetXaxis().GetXmin(),weightedBKG.GetXaxis().GetXmax()

unweightedBKG.SetTitle("")
weightedBKG.SetTitle("")

# weightedBKG.Scale(1/weightedBKG.Integral())
# unweightedBKG.Scale(1/unweightedBKG.Integral())

c = TCanvas("c", "canvas",800,800);
gStyle.SetOptStat(0);
gStyle.SetLegendBorderSize(0);
# c.SetLeftMargin(0.15);
# c.SetLogy();
# c.cd();

pad1 = TPad("pad1","pad1",0.02,0.25,0.99,0.99);
pad1.Draw(); pad1.cd();
# pad1.SetLogy();
pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);
pad1.SetBottomMargin(0.);

weightedBKG.SetLineColor(kGreen)
unweightedBKG.SetLineColor(kBlue)

weightedBKG.Draw("HIST")
unweightedBKG.Draw("HIST SAME")

unweightedBKG.GetYaxis().SetTickLength(0);
unweightedBKG.GetYaxis().SetLabelOffset(999);
unweightedBKG.GetYaxis().SetTitle("")
unweightedBKG.GetXaxis().SetTickLength(0);
unweightedBKG.GetXaxis().SetLabelOffset(999);
unweightedBKG.GetXaxis().SetTitle("");

weightedBKG.GetYaxis().SetRangeUser(0,weightedBKG.GetMaximum()*1.2)
weightedBKG.GetYaxis().SetTitle("Events")
weightedBKG.GetXaxis().SetTickLength(0);
weightedBKG.GetXaxis().SetLabelOffset(999);
weightedBKG.GetXaxis().SetTitle("Gen Boson P_{T} (GeV)")

Ratio = GetRatio(weightedBKG,unweightedBKG)

leg = TLegend(0.62,0.60,0.86,0.887173,"");
leg.AddEntry(unweightedBKG,"genWeight","l");
leg.AddEntry(weightedBKG,"kfactor","l");
leg.SetFillColor(kWhite);
leg.SetFillStyle(0);
leg.SetTextSize(0.025);
leg.Draw();

lumi_label = '%s' % float('%.3g' % (sample.lumi/1000.))
texS = TLatex(0.20,0.837173,("#sqrt{s} = 13 TeV, "+lumi_label+" fb^{-1}"));
texS.SetNDC();
texS.SetTextFont(42);
texS.SetTextSize(0.040);
texS.Draw();
texS1 = TLatex(0.12092,0.907173,"#bf{CMS} : #it{Preliminary} ("+sample.year+")");
texS1.SetNDC();
texS1.SetTextFont(42);
texS1.SetTextSize(0.040);
texS1.Draw();

c.cd();
pad2 = TPad("pad2","pad2",0.02,0.01,0.99,0.25);
pad2.Draw(); pad2.cd();
pad2.SetFillColor(0); pad2.SetFrameBorderMode(0); pad2.SetBorderMode(0);
pad2.SetTopMargin(0);
pad2.SetBottomMargin(0.35);

######################################
rmin = 0.3; rmax = 1.7;
Ratio.GetYaxis().SetRangeUser(rmin,rmax);
Ratio.SetStats(0);

Ratio.GetYaxis().SetTitle("weight/gen")
Ratio.GetYaxis().CenterTitle();
Ratio.SetMarkerStyle(20);
Ratio.SetMarkerSize(0.7);

xmin,xmax = unweightedBKG.GetXaxis().GetXmin(),unweightedBKG.GetXaxis().GetXmax()

line = TLine(xmin, 1.,xmax, 1.);
line.SetLineStyle(8);


Ratio.Draw("pex0");
Ratio.GetYaxis().SetLabelSize(0.14);
Ratio.GetYaxis().SetTitleSize(0.12);
Ratio.GetYaxis().SetLabelFont(42);
Ratio.GetYaxis().SetTitleFont(42);
Ratio.GetYaxis().SetTitleOffset(0.25);
Ratio.GetYaxis().SetNdivisions(100);
Ratio.GetYaxis().SetTickLength(0.05);

Ratio.GetXaxis().SetLabelSize(0.15);
Ratio.GetXaxis().SetTitleSize(0.12);
Ratio.GetXaxis().SetLabelFont(42);
Ratio.GetXaxis().SetTitleFont(42);
Ratio.GetXaxis().SetTitleOffset(0.9);
Ratio.GetXaxis().SetTickLength(0.05);
line.SetLineColor(kBlack);
line.Draw("same");
    
c.Update();

########################################

xwmin = xmin;
xwmax = xmax;

xaxis = TGaxis(xmin,rmin,xmax,rmin,xwmin,xwmax,510);
xaxis.SetLabelFont(42);
xaxis.SetLabelSize(0.10);
xaxis.SetTitleFont(42);
xaxis.SetTitleSize(0.12);
xaxis.SetTitleOffset(1.2);
xaxis.Draw("SAME");

yaxis = TGaxis(xmin,rmin,xmin,rmax,rmin,rmax,6,"");
yaxis.SetLabelFont(42);
yaxis.SetLabelSize(0.10);
yaxis.SetTitleFont(42);
yaxis.SetTitleSize(0.12);
yaxis.SetTitleOffset(0.35);
yaxis.Draw("SAME");

dir = os.getcwd().split("/")[-1]
file_path="/afs/hep.wisc.edu/home/ekoenig4/public_html/MonoZprimeJet/Plots"+sample.year+"/"+dir+"Plots_EWK/"
#print file_path
directory=os.path.join(os.path.dirname(file_path),"")
if not os.path.exists(directory):
    os.mkdir(directory,0755)
    print directory
c.SaveAs(directory+"/kfactorComparison.pdf")
c.SaveAs(directory+"/kfactorComparison.png")
