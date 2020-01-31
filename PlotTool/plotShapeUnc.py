from ROOT import *
import os
from sys import argv
from PlotTool import *
import config

gROOT.SetBatch(1)

out_dir = "/afs/hep.wisc.edu/home/ekoenig4/public_html/MonoJet/Plots%s/"
def SaveCanvas(c,sample,uncname):
    nhist = config.regions[sample.region]
    variable = b_info.template.GetName()
    varname = sample.varname.replace('_%s' % nhist,'')
    outdir = out_dir % sample.year
    outdir = "%s/%sPlots_EWK/UncertaintyPlots/%s"  % (outdir,sample.region,variable)
    if not os.path.isdir(outdir): os.mkdir(outdir)
    
    outname = "%s_%s" % (uncname,varname)
    c.SaveAs( "%s/%s.png" % (outdir,outname) )
def plotCRUnc(sample,uncname):
    if 'Single' in sample.region: process = 'WJets'
    if 'Double' in sample.region: process = 'DYJets'

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
    
    pad1 = TPad("pad1","pad1",0.01,0.25,0.99,0.99);
    pad1.Draw(); pad1.cd();
    pad1.SetLogy();
    pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);
    pad1.SetBottomMargin(0.);

    ymax = max( h.GetMaximum() for h in (norm,up,dn) ) * pow(10,2.5)
    ymin = 0.001

    for h in (up,dn): h.SetLineStyle(2)
    for h in (norm,up,dn):
        h.SetTitle("")
        h.GetYaxis().SetTitle("Events")
        h.GetYaxis().SetRangeUser(ymin,ymax)
    
    norm.Draw('hist')
    up.Draw('hist same')
    dn.Draw('hist same')
    
    lumi_label = '%s' % float('%.3g' % (sample.lumi/1000.)) + " fb^{-1}"
    texLumi,texCMS = getCMSText(lumi_label,sample.year)
    texLumi.Draw();
    texCMS.Draw();

    pt = TPaveText(0.62,0.60,0.86,0.887173,'NDC');
    pt.AddText("Systematic Variation")
    pt.AddText(uncname)
    pt.AddText(process)
    pt.Draw()
    
    c.cd();
    pad2 = TPad("pad2","pad2",0.01,0.01,0.99,0.25);
    pad2.Draw(); pad2.cd();
    pad2.SetFillColor(0); pad2.SetFrameBorderMode(0); pad2.SetBorderMode(0);
    pad2.SetTopMargin(0);
    pad2.SetBottomMargin(0.35);

    rbins = []
    for r in (r_up,r_dn): rbins += [ b for b in r if b != 0 ]

    def avg(bins): return sum(bins)/len(bins)
    def stdv(bins):
        bavg = avg(bins)
        return ( sum( (b - bavg)**2 for b in bins )/(len(bins)-1) ) ** 0.5
    
    rstdv = stdv(rbins)
    rymin = 1 - 3*rstdv; rymax = 1 + 3*rstdv
    
    RatioStyle(r_up,rymin,rymax)
    RatioStyle(r_dn,rymin,rymax)

    for r in (r_up,r_dn):
        r.SetMarkerStyle(1)
        r.SetTitle("")
        r.GetXaxis().SetTitle("");
        r.GetXaxis().SetTickLength(0);
        r.GetXaxis().SetLabelOffset(999);
        r.GetYaxis().SetTitle("");
        r.GetYaxis().SetTickLength(0);
        r.GetYaxis().SetLabelOffset(999);
    
    r_up.Draw()
    r_dn.Draw('same')
    
    nbins = norm.GetNbinsX();
    xmin = norm.GetXaxis().GetXmin();
    xmax = norm.GetXaxis().GetXmax();
    xwmin = xmin;
    xwmax = xmax;
    
    line = getRatioLine(xmin,xmax)
    line.Draw("same");

    xname = sample.name if type(sample.name) == str else None
    xaxis = makeXaxis(xmin,xmax,rymin,510,name=xname);
    xaxis.Draw("SAME");

    yaxis = makeYaxis(rymin,rymax,xmin,6,name="syst./cent.");
    yaxis.Draw("SAME");

    SaveCanvas(c,sample,uncname)
    
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
    
    pad1 = TPad("pad1","pad1",0.01,0.25,0.99,0.99);
    pad1.Draw(); pad1.cd();
    pad1.SetLogy();
    pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);
    pad1.SetBottomMargin(0.);

    ymax = max( h.GetMaximum() for h in (z_norm,w_norm,z_up,z_dn,w_up,w_dn) ) * pow(10,2.5)
    ymin = 0.001

    z_norm.SetLineColor(kRed)
    for h in (z_up,z_dn): h.SetLineStyle(2); h.SetLineColor(kRed)

    w_norm.SetLineColor(kBlue)
    for h in (w_up,w_dn): h.SetLineStyle(2); h.SetLineColor(kBlue)
    for h in (z_norm,w_norm,z_up,z_dn,w_up,w_dn):
        h.SetTitle("")
        h.GetYaxis().SetTitle("Events")
        h.GetYaxis().SetRangeUser(ymin,ymax)
        h.Draw('hist same')
    
    lumi_label = '%s' % float('%.3g' % (sample.lumi/1000.)) + " fb^{-1}"
    texLumi,texCMS = getCMSText(lumi_label,sample.year)
    texLumi.Draw();
    texCMS.Draw();

    pt = TPaveText(0.62,0.7,0.86,0.887173,'NDC');
    pt.AddText("Systematic Variation")
    pt.AddText(uncname)
    pt.Draw()

    leg = getLegend(0.62,0.5,0.86,0.7)
    leg.AddEntry(z_norm,'Z+jets','l')
    leg.AddEntry(w_norm,'W+jets','l')
    leg.Draw()
    
    c.cd();
    pad2 = TPad("pad2","pad2",0.01,0.01,0.99,0.25);
    pad2.Draw(); pad2.cd();
    pad2.SetFillColor(0); pad2.SetFrameBorderMode(0); pad2.SetBorderMode(0);
    pad2.SetTopMargin(0);
    pad2.SetBottomMargin(0.35);

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
    for r in (z_r_up,z_r_dn,w_r_up,w_r_dn):
        RatioStyle(r,rymin,rymax)
        r.SetMarkerStyle(1)
        r.SetTitle("")
        r.GetXaxis().SetTitle("");
        r.GetXaxis().SetTickLength(0);
        r.GetXaxis().SetLabelOffset(999);
        r.GetYaxis().SetTitle("");
        r.GetYaxis().SetTickLength(0);
        r.GetYaxis().SetLabelOffset(999);
        r.Draw('same')
    
    
    nbins = z_norm.GetNbinsX();
    xmin = z_norm.GetXaxis().GetXmin();
    xmax = z_norm.GetXaxis().GetXmax();
    xwmin = xmin;
    xwmax = xmax;
    
    line = getRatioLine(xmin,xmax)
    line.Draw("same");

    xname = sample.name if type(sample.name) == str else None
    xaxis = makeXaxis(xmin,xmax,rymin,510,name=xname);
    xaxis.Draw("SAME");

    yaxis = makeYaxis(rymin,rymax,xmin,6,name="syst./cent.");
    yaxis.Draw("SAME");

    SaveCanvas(c,sample,uncname)

def runRegion(args):
    sample = Region()
    variable = args.argv[0]
    nvariable = variable+'_'+config.regions[sample.region]
    variations = []
    for name,unclist in config.Uncertainty.iteritems(): variations += unclist

    print 'Running for %s' % nvariable
    sample.initiate(nvariable)
    for uncname in variations:
        print 'Fetching %s' % uncname
        sample.addUnc(uncname)
    
    if sample.region == 'SignalRegion':
        for uncname in variations: plotSRUnc(sample,uncname)
    else:
        for uncname in variations: plotCRUnc(sample,uncname)
def runAll(args):
    cwd = os.getcwd()
    for region,nhist in config.regions.items():
        os.chdir(region)
        runRegion(args)
        os.chdir(cwd)
    
if __name__ == "__main__":
    args = parser.parse_args()
    if not any(args.argv): args.argv.append('ChNemPtFrac')
    runall = ( GetRegion() == None )

    if runall: runAll(args)
    else:      runRegion(args)
