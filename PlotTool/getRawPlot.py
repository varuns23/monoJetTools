#!/usr/bin/env python

from ROOT import *
import sys
import os
from PlotTool import *

gROOT.SetBatch(1)

def GetVariableName(variable):
    name = 'Xaxis Title'
    for title in samplenames:
        if title in variable:
            name = samplenames[title];
        key = variable.split("_")[-2]
        if key == title:
            name = samplenames[title];
            break
    return name

def get_comma_separated_args(option, opt, value, parser):
    setattr(parser.values, option.dest, value.split(','))

parser = OptionParser()
parser.add_option('-f', '--files',
                  type='string',
                  help='Specify the raw files to generate plots from',
                  action='callback',
                  callback=get_comma_separated_args,
                  dest = "files")

(options, args) = parser.parse_args()

for f in options.files:
    rfile = TFile.Open(f)
    for variable in args:
        name =GetVariableName(variable)
        h = rfile.Get(variable)
        c = TCanvas("c", "canvas",800,800);
        gStyle.SetOptStat(0);
        gStyle.SetLegendBorderSize(0);
        #c.SetLeftMargin(0.15);
        #c.SetLogy();
        #c.cd();
        
        h.SetLineWidth(2)
        h.SetLineColor(kWhite);
        h.SetTitle("");
        # h.GetXaxis().SetTitle("");
        # h.GetXaxis().SetTickLength(0);
        # h.GetXaxis().SetLabelOffset(999);
        # h.GetYaxis().SetTitle("");
        # h.GetYaxis().SetTickLength(0);
        # h.GetYaxis().SetLabelOffset(999);
        h.SetLineColor(kBlack);
        h.SetMarkerStyle(20);
        h.SetMarkerSize(0.9);

        h.Draw("HIST")
        h.GetXaxis().SetTitle(name)
        h.GetYaxis().SetTitle("Raw Events")
        
        leg = TLegend(0.62,0.60,0.86,0.887173,"");
        leg.AddEntry(h,f.replace(".root",""))
        leg.SetFillColor(kWhite);
        leg.SetFillStyle(0);
        leg.SetTextSize(0.025);
        leg.Draw();
        
        texS = TLatex(0.20,0.837173,("#sqrt{s} = 13 TeV"));
        texS.SetNDC();
        texS.SetTextFont(42);
        texS.SetTextSize(0.040);
        texS.Draw();
        texS1 = TLatex(0.12092,0.907173,"#bf{CMS} : #it{Preliminary} (2018)");
        texS1.SetNDC();
        texS1.SetTextFont(42);
        texS1.SetTextSize(0.040);
        texS1.Draw();
        
        dir = os.getcwd().split("/")[-1]
        file_path="/afs/hep.wisc.edu/home/ekoenig4/public_html/MonoJet/Plots"+samples.year+"/"+dir+"Plots_EWK/RawPlots/"
        #print file_path
        sub = f.replace(".root","")
        directory=os.path.join(os.path.dirname(file_path),sub)
        if not os.path.exists(directory):
            os.mkdir(directory,0755)
            print directory
        c.SaveAs(directory+"/datamc_"+variable+".pdf")
        c.SaveAs(directory+"/datamc_"+variable+".png")
