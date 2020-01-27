#!/usr/bin/env python
from ROOT import *
from PlotTool import *
import os
import sys

gROOT.SetBatch(1)

# Storage list to be used to keep references around for ROOT before TCanvas is saved
parser.add_argument("--thn",help="specifies that all following plots are TH2 or TH3 plots",action="store_true", default=False)
parser.add_argument("--sub",help="specify a sub directory to place output",action="store",type=str,default=None)
parser.add_argument("--run2",help="Specify the region to run an entire run2 plot",action="store",type=str,default=None)
parser.add_argument("--no-plot",help="Dont plot variables",action="store_true",default=False)
parser.add_argument("-u","--uncertainty",help="Specify the uncertainty to apply on variable if available",default=[],nargs="*",type=str)

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

def AutoSave(samples,c):
    file_path="/afs/hep.wisc.edu/home/ekoenig4/public_html/MonoJet/Plots"+samples.year+"/"+samples.region+"Plots_EWK/"
    #print file_path
    sub = ""
    if (samples.args.sub != None): sub = samples.args.sub
    directory=os.path.join(os.path.dirname(file_path),sub)
    if not os.path.exists(directory):
        os.makedirs(directory,0755)
        print directory
    c.SaveAs(directory+"/datamc_"+samples.varname+".pdf")
    c.SaveAs(directory+"/datamc_"+samples.varname+".png")
###################################################################
def plotVariable(samples,variable,initiate=True,saveas=AutoSave,blinded=False):
    del store[:] # Clear storage list 
    print "Plotting",variable
    if initiate:
        if (samples.args.thn):
            HigherDimension(samples,variable)
        else:
            samples.initiate(variable)
            samples.hasUncertainty = any(samples.args.uncertainty) and any(Nuisance.unclist)
            if samples.hasUncertainty: samples.fullUnc(samples.args.uncertainty,stat=True)
    if samples.args.no_plot: return
    samples.hasUncertainty = any(samples.args.uncertainty) and any(Nuisance.unclist)
    if samples['Data'].histo.Integral() == 0: blinded = True
    c = TCanvas("c", "canvas",800,800);
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
        if (samples.args.normalize): data.histo.Scale(1/data.total)

    for mc in samples.MCList:
        mc_proc = samples.processes[mc]
        MCStyle(mc_proc.histo,mc_proc.color)
        if (samples.args.normalize): mc_proc.histo.Scale(1/samples.bkgIntegral)
        

    hs_datamc = THStack("hs_datamc","Data/MC comparison"); samples.stack = hs_datamc
    fillStack(samples,hs_datamc)
    hs_bkg = hs_datamc.GetStack().Last()
    if samples.args.mc_solid:hs_bkg.Draw("hist")
    else:                    hs_datamc.Draw("hist")
        
    if not blinded: data.histo.Draw('pex0same')
    
    if hasattr(samples,'signal'):
        signal = samples.processes[samples.signal]
        signal.histo.SetLineWidth(2)
        signal.histo.Draw("HIST SAME")

    #################################################

    leg = getLegend(xmin=0.5,xmax=0.7); #0.62,0.60,0.86,0.887173
    if not blinded: leg.AddEntry(data.histo,"Data","lp");
    if (hasattr(samples,'signal')): leg.AddEntry(signal.histo, signal.process,'l')

    if samples.args.mc_solid:
        leg.AddEntry(hs_bkg,"Background","f")
    else:
        for mc in samples.MCOrder:
            if samples[mc].scaled_total == 0: continue
            leg.AddEntry(samples[mc].histo,samples[mc].leg,'f')

    if samples.hasUncertainty:
        uncband = samples.getUncBand(samples.args.uncertainty,stat=True)
        UncBandStyle(uncband)
        leg.AddEntry(uncband,"syst #otimes stat",'f')
        
    leg.Draw();

    lumi_label = '%s' % float('%.3g' % (samples.lumi/1000.)) + " fb^{-1}"
    if (samples.args.normalize): lumi_label="Normalized"
    texLumi,texCMS = getCMSText(lumi_label,samples.year,scale=0.8 if blinded else 1)
    texLumi.Draw();
    texCMS.Draw();

    
    if samples.args.mc_solid:
        StackStyle(hs_bkg,scaleWidth=samples.scaleWidth)
        hs_bkg.GetXaxis().SetTitle(samples.name)
    else:
        StackStyle(hs_datamc,scaleWidth=samples.scaleWidth)
        hs_datamc.GetXaxis().SetTitle(samples.name)

    ######################################
    if not blinded:
        c.cd();
        pad2 = TPad("pad2","pad2",0.01,0.01,0.99,0.25);
        pad2.Draw(); pad2.cd();
        pad2.SetFillColor(0); pad2.SetFrameBorderMode(0); pad2.SetBorderMode(0);
        pad2.SetTopMargin(0);
        pad2.SetBottomMargin(0.35);
        
        Ratio = GetRatio(data.histo,hs_bkg)
        
        rymin = 0.65; rymax = 1.35
        # rymin = 0.35; rymax = 1.75
        RatioStyle(Ratio,rymin,rymax)
        Ratio.Draw("A");
        
        if samples.hasUncertainty:
            uncband.Draw('2same')
        Ratio.Draw('pex0same')
        line = getRatioLine(data.histo.GetXaxis().GetXmin(),data.histo.GetXaxis().GetXmax())
        line.Draw("same");
        
        c.Update();
        

        nbins = hs_datamc.GetXaxis().GetNbins();
        xmin = hs_datamc.GetXaxis().GetXmin();
        xmax = hs_datamc.GetXaxis().GetXmax();
        xwmin = xmin;
        xwmax = xmax;
        
        xname = samples.name if type(samples.name) == str else None
        xaxis = makeXaxis(xmin,xmax,rymin,510,name=xname);
        xaxis.Draw("SAME");

        if (samples.name == "Cutflow"): XaxisCutflowStyle(xaxis,hs_bkg)
        
        yaxis = makeYaxis(rymin,rymax,xmin,6,name="Data/MC");
        yaxis.Draw("SAME");
    ######################################
    
    saveas(samples,c)
###################################################################
    
def plotter(args=[]):
    samples = Region()
    if not any(args): args = samples.args.argv
    for variable in args:
        plotVariable(samples,variable)
###################################################################

def run2plotter(region):
    cwd = os.getcwd(); sys.path.append(cwd)
    from importlib import import_module
    yearmap = {}
    yearlist = ["2017","2018"]
    blinded = region == 'SignalRegion'
    for year in yearlist:
        useMaxLumi = region == 'SignalRegion' and blinded
        yearmap[year] = Region(path='%s/%s' % (year,region),config=import_module(".config",year),autovar=True,show=False,useMaxLumi=useMaxLumi)
    
    args = parser.parse_args()
    for variable in args.argv:

        for year in yearlist:
            yearmap[year].initiate(variable)
        combined = Region(copy=yearmap["2017"])
        combined.add(yearmap["2018"])
        combined.year = 'Run2'
        combined.region = region
        combined.output()
        plotVariable(combined,variable,initiate=False,blinded=blinded)
###################################################################
  

if __name__ == "__main__":
    args = parser.parse_args()
    if args.run2 is None: plotter()
    else: run2plotter(args.run2)
