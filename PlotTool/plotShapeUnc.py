from ROOT import *
import os
from sys import argv
from PlotTool import *
import config

gROOT.SetBatch(1)

def plotCRUnc(sample,uncname):
    print 'Fetching %s' % uncname
    if 'Single' in sample.region: process = 'WJets'
    if 'Double' in sample.region: process = 'DYJets'
    if 'Gamma' in sample.region: process = 'GJets'

    norm = sample.processes[process].histo.Clone('norm')
    up,dn = sample.processes[process].nuisances[uncname].GetHistos()

    r_up = up.Clone('ratio_up'); r_up.Divide(norm)
    r_dn = dn.Clone('ratio_dn'); r_dn.Divide(norm)

    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    #c.SetLeftMargin(0.15);
    #c.SetLogy();
    #c.cd();
    
    pad1 = TPad("pad1","pad1",0.01,0.01,0.99,0.99);
    pad1.Draw(); pad1.cd();
    pad1.SetLogy();
    pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);
    pad1.SetBottomMargin(0.);

    rbins = []
    for r in (r_up,r_dn): rbins += [ b for b in r if b != 0 ]

    def avg(bins): return sum(bins)/len(bins)
    def stdv(bins):
        bavg = avg(bins)
        return ( sum( (b - bavg)**2 for b in bins )/(len(bins)-1) ) ** 0.5
    
    rstdv = stdv(rbins)
    rymin = 1 - 3*rstdv; rymax = 1 + 3*rstdv
    

    for r in (r_up,r_dn):
        RatioStyle(r_up,rymin,rymax,yname="syst./cent.")
    
    r_up.Draw("hist")
    r_dn.Draw('hist same')
    
    nbins = norm.GetNbinsX();
    xmin = norm.GetXaxis().GetXmin();
    xmax = norm.GetXaxis().GetXmax();
    xwmin = xmin;
    xwmax = xmax;
    
    
    lumi_label = '%s' % float('%.3g' % (sample.lumi/1000.)) + " fb^{-1}"
    texLumi,texCMS = getCMSText(lumi_label,sample.year,scale=0.8)
    texLumi.Draw();
    texCMS.Draw();
    
    line = getRatioLine(xmin,xmax)
    line.Draw("same");

    SaveAs(c,"%s_%s" % (uncname,sample.varname),year=sample.year,region=sample.region,sub="UncertaintyPlots/%s" % sample.variable.base)
    
def plotSRUnc(sample,uncname):
    print 'Fetching %s' % uncname
    z_norm = sample.processes['ZJets'].histo.Clone('z_norm')
    w_norm = sample.processes['WJets'].histo.Clone('w_norm')

    z_up,z_dn = sample.processes['ZJets'].nuisances[uncname].GetHistos()

    w_up,w_dn = sample.processes['WJets'].nuisances[uncname].GetHistos()

    z_r_up = z_up.Clone('ratio_up'); z_r_up.Divide(z_norm)
    z_r_dn = z_dn.Clone('ratio_dn'); z_r_dn.Divide(z_norm)
    
    w_r_up = w_up.Clone('ratio_up'); w_r_up.Divide(w_norm)
    w_r_dn = w_dn.Clone('ratio_dn'); w_r_dn.Divide(w_norm)

    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    #c.SetLeftMargin(0.15);
    #c.SetLogy();
    #c.cd();
    
    # pad1 = TPad("pad1","pad1",0.01,0.01,0.99,0.99);
    # pad1.Draw(); pad1.cd();
    # pad1.SetLogy();
    # pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);
    # pad1.SetBottomMargin(0.);
    # pad1.SetGridy()

    rbins = []
    for r in (z_r_up,z_r_dn,w_r_up,w_r_dn): rbins += [ b for b in r if b != 0 ]

    def avg(bins): return sum(bins)/len(bins)
    def stdv(bins):
        bavg = avg(bins)
        return ( sum( (b - bavg)**2 for b in bins )/(len(bins)-1) ) ** 0.5
    
    rstdv = stdv(rbins) if any(rbins) else 0.1
    rymin = 1 - 3*rstdv; rymax = 1 + 3*rstdv

    for r in (z_r_up,z_r_dn): r.SetLineColor(kRed)
    for r in (w_r_up,w_r_dn): r.SetLineColor(kBlue)
    hslist =(z_r_up,z_r_dn,w_r_up,w_r_dn)
    for r in hslist:
        # RatioStyle(r,rymin,rymax,yname=uncname)
        r.GetYaxis().SetTitle(uncname)
        r.GetYaxis().SetRangeUser(rymin,rymax)
        r.Draw('hist same')
    
    
    nbins = z_norm.GetNbinsX();
    xmin = z_norm.GetXaxis().GetXmin();
    xmax = z_norm.GetXaxis().GetXmax();
    xwmin = xmin;
    xwmax = xmax;
    
    line = getRatioLine(xmin,xmax)
    line.Draw("same");

    
    lumi_label = '%s' % float('%.3g' % (sample.lumi/1000.)) + " fb^{-1}"
    texLumi,texCMS = getCMSText(lumi_label,sample.year,scale=0.8)
    texLumi.Draw();
    texCMS.Draw();

    leg = getLegend(xmin=0.1,xmax=0.3,ymin=0.5,ymax=0.8)
    leg.AddEntry(z_r_up,'Z+jets','l')
    leg.AddEntry(w_r_up,'W+jets','l')
    leg.Draw()

    SaveAs(c,"%s_%s" % (uncname,sample.varname),year=sample.year,region=sample.region,sub="UncertaintyPlots/%s" % sample.variable.base)

def runRegion(region,args):
    sample = Region(autovar=True)
    variable = args.argv[0]
    # for name,unclist in config.Uncertainty.iteritems(): variations += unclist

    print 'Running for %s' % variable
    sample.initiate(variable)
    variations = sample.variable.nuisances.keys()
    for uncname in variations: sample.addUnc(uncname,True)
        
        
    if sample.region == 'SignalRegion':
        for uncname in variations: plotSRUnc(sample,uncname)
    else:
        for uncname in variations:  plotCRUnc(sample,uncname)
def runAll(args):
    runRegion('SignalRegion',args)
    # for region,nhist in config.regions.items():
        # runRegion(region,args)
    
if __name__ == "__main__":
    from PlotTool import parser
    parser.parse_args()
    if not any(parser.args.argv): parser.args.argv.append('recoil')
    region = GetRegion()
    runall = ( region  == None )

    if runall: runAll(parser.args)
    else:      runRegion(region,parser.args)
