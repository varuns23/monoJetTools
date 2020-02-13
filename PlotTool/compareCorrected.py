from ROOT import *
from array import array
from PlotTool import *
import config
from sys import argv

gROOT.SetBatch(1)

def compare(corr,uncorr):
    samples = Region(autovar=True,show=False)
    procname = corr.replace('.root','').replace('post','')
    corrlist = [ corr.replace('.root','') ]
    xsecmap = GetMCxsec(corrlist,config.xsec)
    corr = Process("corr",corrlist,xsecmap,color=kRed)
    uncorr = Process("uncorr",[ f.replace('post','uncorr') for f in corrlist ],{ f.replace('post','uncorr'):xsec for f,xsec in xsecmap.iteritems() },color=kBlack)
    samples.SampleList = ["corr","uncorr"]
    samples.processes = {'corr':corr,'uncorr':uncorr}
    def plot(variable,weight=None):
        samples.initiate(variable,weight)
        c = TCanvas('c','c',800,800)
        gStyle.SetOptStat(0)
        gStyle.SetLegendBorderSize(0)
        c.SetLogy()

        pad1 = TPad("pad1","pad1",0.01,0.25,0.99,0.99);
        pad1.SetBottomMargin(0.);
        pad1.Draw(); pad1.cd();
        pad1.SetLogy();
        pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);

        corr = samples['corr'].histo
        uncorr = samples['uncorr'].histo

        DataStyle(corr)
        uncorr.SetLineColor(kRed)
        uncorr.SetLineWidth(2)

        corr.GetYaxis().SetTitle('Events / GeV')
        ymax = max( max(hs[ibin] for hs in (corr,uncorr)) for ibin in range(1,corr.GetNbinsX()+1) )
        uncorr.SetMaximum(ymax*pow(10,1.2))
        uncorr.Draw('hist')
        corr.Draw('pex0same')
        
        leg = getLegend(xmin=0.5,xmax=0.7,ymin=0.7,ymax=0.9)
        leg.AddEntry(samples['corr'].histo,'Corrected %s' % procname,'pl')
        leg.AddEntry(samples['uncorr'].histo,'Uncorrected %s' % procname,'l')
        leg.Draw()
        
        lumi_label = '%s' % float('%.3g' % (samples.lumi/1000.)) + " fb^{-1}"
        if (samples.args.normalize): lumi_label="Normalized"
        texLumi,texCMS = getCMSText(lumi_label,samples.year,scale=0.8)
        texLumi.Draw();
        texCMS.Draw();

        
        c.cd();
        pad2 = TPad("pad2","pad2",0.01,0.01,0.99,0.25);
        pad2.SetGridy()
        pad2.Draw(); pad2.cd();
        pad2.SetFillColor(0); pad2.SetFrameBorderMode(0); pad2.SetBorderMode(0);
        pad2.SetTopMargin(0);
        pad2.SetBottomMargin(0.35);

        ratio = GetRatio(samples['corr'].histo,samples['uncorr'].histo)
        RatioStyle(ratio,xname=samples.name,yname='Corr/Uncorr')
        ratio.Draw('pex0')
        
        line = getRatioLine(ratio.GetXaxis().GetXmin(),ratio.GetXaxis().GetXmax())
        line.Draw("same");
        
        c.Update()
        out_dir = '/afs/hep.wisc.edu/home/ekoenig4/public_html/MonoJet/Plots%s/%sPlots_EWK/Compare_Corrected/' % (samples.year,samples.region) 
        if not os.path.isdir(out_dir): os.mkdir(out_dir)
        c.SaveAs('%s/compare_%s.png' % (out_dir,variable))
    plot('h_cutflow')
    plot('recoil')
    plot('recoilPhi')
    plot('j1pT')
    plot('pfMET')
    plot('pfMETPhi')
compare(argv[1],argv[2])
