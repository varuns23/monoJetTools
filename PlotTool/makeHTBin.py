#!/usr/bin/env python
from ROOT import *
from PlotTool import *
import os
import sys

gROOT.SetBatch(1)

def plotVariable(samples,variable,initiate=True,blinded=False):
    del store[:] # Clear storage list 
    samples.initiate(variable)
    print "Plotting",variable

    def plotHTBin(sample):
        c = TCanvas(sample.process, "canvas",800,800);
        gStyle.SetOptStat(0);
        gStyle.SetLegendBorderSize(0);
        #c.SetLeftMargin(0.15);
        #c.SetLogy();
        #c.cd();
        
        pad1 = TPad("pad1","pad1",0.01,0.01,0.99,0.99);
        pad1.Draw(); pad1.cd();
        pad1.SetLogy();
        pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);

        stack = THStack("htbin_stack","")
        htbin_order = [ process for process in sample ]
        htbin_order.sort(key=lambda process:process.scaled_total)
        for process in htbin_order: stack.Add(process.histo)

        stack.Draw("HIST PFC")

        leg = getLegend(xmin=0.5,xmax=0.7)
        for process in sample: leg.AddEntry(process.histo,process.process,"f")
        leg.Draw()
        
        texLumi,texCMS = getCMSText(samples.lumi_label,samples.year,scale=0.8)
        texLumi.Draw();
        texCMS.Draw();
        
        SaveAs(c,'binned_%s'%samples.varname,year=samples.year,region=samples.region,sub="HTBin/%s"%sample.process)
    for process in samples:
        if len(process) > 1 and process.scaled_total > 0:
            plotHTBin(process)
###################################################################
    
def plotter(args=[]):
    samples = Region()
    if not any(args): args = parser.args.argv
    for variable in args:
        plotVariable(samples,variable)
###################################################################
  

if __name__ == "__main__":
    parser.parse_args()
    plotter()
