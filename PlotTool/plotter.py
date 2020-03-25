#!/usr/bin/env python
from ROOT import *
from PlotTool import *
from PlotTool import parser
import os
import sys

gROOT.SetBatch(1)

# Storage list to be used to keep references around for ROOT before TCanvas is saved
parser.add_argument("--thn",help="specifies that all following plots are TH2 or TH3 plots",action="store_true", default=False)
parser.add_argument("--run2",help="Specify the region to run an entire run2 plot",action="store",type=str,default=None)
parser.add_argument("--no-plot",help="Dont plot variables",action="store_true",default=False)
parser.add_argument("-u","--uncertainty",help="Specify the uncertainty to apply on variable if available",default=[],nargs="*",type=str)
parser.add_argument("--ignore-mc",help="Ignore MC with less than a threshold percent of total MC (default = 0.001, use 0 to keep all MC)",default=0.001,type=float)
parser.add_argument("--dimension",help="Specify the pixel dimensions to draw the canvas",default=[800,800],type=int,nargs=2)

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
################################################p###################
def plotVariable(samples,variable,initiate=True,blinded=False):
    print "Plotting",variable
    if initiate:
        if (parser.args.thn):
            HigherDimension(samples,variable)
        else:
            samples.initiate(variable)
            if samples.isBlinded: blinded = True
            if 'Stat' not in parser.args.uncertainty: parser.args.uncertainty.append('Stat')
    if parser.args.no_plot: return
    if samples['Data'].histo.Integral() == 0: blinded = True
    xwidth,ywidth = parser.args.dimension # default is 800,800
    c = TCanvas("c", "canvas",xwidth,ywidth);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    #c.SetLeftMargin(0.15);
    #c.SetLogy();
    #c.cd();

    if not blinded:
        pad1 = TPad("pad1","pad1",0.01,0.25,0.99,0.99);
        pad1.SetBottomMargin(0.);
    else:
        pad1 = TPad("pad1","pad1",0.01,0.01,0.99,0.99);
    pad1.Draw(); pad1.cd();
    pad1.SetLogy();
    pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);

    if not blinded:
        data = samples.processes['Data']
        DataStyle(data.histo)
        if (parser.args.normalize): data.histo.Scale(1/data.total)

    for mc in samples.MCList:
        mc_proc = samples.processes[mc]
        MCStyle(mc_proc.histo,mc_proc.color)
        if (parser.args.normalize): mc_proc.histo.Scale(1/samples.bkgIntegral)
        

    hs_datamc = THStack("hs_datamc","Data/MC comparison"); samples.stack = hs_datamc
    MCLegOrder = fillStack(samples,hs_datamc,parser.args.ignore_mc)
    hs_bkg = hs_datamc.GetStack().Last()
    if parser.args.mc_solid:hs_bkg.Draw("hist")
    else:                    hs_datamc.Draw("hist")
        
    if not blinded: data.histo.Draw('pex0same')
    
    if hasattr(samples,'SignalList'):
        signals = [ samples[signal] for signal in samples.SignalList ]
        for signal in signals:
            signal.histo.SetLineWidth(2)
            signal.histo.Draw("HIST SAME")

    #################################################

    leg = getLegend(xmin=0.5,xmax=0.7); #0.62,0.60,0.86,0.887173
    if not blinded: leg.AddEntry(data.histo,"Data","lp");
    if (hasattr(samples,'SignalList')):
        for signal in signals: leg.AddEntry(signal.histo, signal.process,'l')

    if parser.args.mc_solid:
        leg.AddEntry(hs_bkg,"Background","f")
    else:
        for mc in MCLegOrder:
            if samples[mc].scaled_total == 0: continue
            leg.AddEntry(samples[mc].histo,samples[mc].leg,'f')

    uncband = samples.getUncBand(parser.args.uncertainty)
    UncBandStyle(uncband)
    uncband_leg = 'syst #otimes stat' if len(parser.args.uncertainty) > 1 else 'stat'
    leg.AddEntry(uncband,uncband_leg,'f')
        
    leg.Draw();

    lumi_label = '%s' % float('%.3g' % (samples.lumi/1000.)) + " fb^{-1}"
    if (parser.args.normalize): lumi_label="Normalized"
    texLumi,texCMS = getCMSText(lumi_label,samples.year,scale=0.8 if blinded else 1)

    
    if parser.args.mc_solid:
        StackStyle(hs_bkg,scaleWidth=samples.scaleWidth)
        hs_bkg.GetXaxis().SetTitle(samples.name)
    else:
        StackStyle(hs_datamc,scaleWidth=samples.scaleWidth)
        hs_datamc.GetXaxis().SetTitle(samples.name)

    ######################################
    if not blinded:
        c.cd();
        pad2 = TPad("pad2","pad2",0.01,0.01,0.99,0.25);
        pad2.SetGridy()
        pad2.Draw(); pad2.cd();
        pad2.SetFillColor(0); pad2.SetFrameBorderMode(0); pad2.SetBorderMode(0);
        pad2.SetTopMargin(0);
        pad2.SetBottomMargin(0.35);
        
        Ratio = GetRatio(data.histo,hs_bkg)
        
        RatioStyle(Ratio,xname=samples.name)
        Ratio.Draw("pex0");
        uncband.Draw('2same')
        Ratio.Draw('pex0same')
        line = getRatioLine(data.histo.GetXaxis().GetXmin(),data.histo.GetXaxis().GetXmax())
        line.Draw("same");
        
        c.Update();
    ######################################
    SaveAs(c,'datamc_%s'%samples.varname,year=samples.year,region=samples.region,exts=('.png','.pdf'))
###################################################################
    
def plotter(args=[]):
    samples = Region()
    if not any(args): args = parser.args.argv
    for variable in args:
        plotVariable(samples,variable)
###################################################################

def run2plotter(region):
    cwd = os.getcwd(); sys.path.append(cwd)
    from importlib import import_module
    yearmap = {}
    yearlist = ["2016","2017","2018"]
    blinded = region == 'SignalRegion'
    for year in yearlist:
        useMaxLumi = region == 'SignalRegion' and blinded
        yearmap[year] = Region(path='%s/%s' % (year,region),config=import_module(".config",year),autovar=True,show=False,useMaxLumi=useMaxLumi)
    
    parser.parse_args()
    for variable in parser.args.argv:

        for year in yearlist:
            yearmap[year].initiate(variable)
        combined = Region(copy=yearmap["2016"])
        combined.add(yearmap["2017"])
        combined.add(yearmap["2018"])
        combined.year = 'Run2'
        combined.region = region
        combined.output()
        plotVariable(combined,variable,initiate=False,blinded=blinded)
###################################################################
  

if __name__ == "__main__":
    from PlotTool import parser
    parser.parse_args()
    if parser.args.run2 is None: plotter()
    else: run2plotter(args.run2)
